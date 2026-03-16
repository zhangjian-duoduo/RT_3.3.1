#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/prctl.h>
#include <unistd.h>
#include <math.h>
#include <assert.h>
#include <sys/time.h>

#include "types/bufCtrl.h"
#include "dsp/fh_system_mpi.h"
#include "dsp/fh_nna_mpi.h"
#include "dsp/fh_vpu_mpi.h"
#include "dsp/fh_venc_mpi.h"
#include "types/type_def.h"
#include "fh_face_snap.h"
#include "sample_common.h"

#define RGB_CHAN (3) /* 只有通道3支持RGB输出格式 */

#define NN_RGB_W (512)
#define NN_RGB_H (288)

#define DRAW_BOX_CHAN (0)

#define JPEG_BUF_NUM (3)

typedef struct _face_frame_list
{
    void *pre;
    void *next;
    unsigned int frame_id;
    unsigned long long time_stamp;
    unsigned char *luma;
    unsigned char *chroma;
    FH_DETECTION_EXT_T det_face;
    FH_FS_OUT_t best_face;
} FACE_FRAME_LIST_t;

extern FH_SINT32 sample_set_gbox(FH_UINT32 chan, FH_UINT32 enable, FH_UINT32 box_id, FH_UINT32 x, FH_UINT32 y, FH_UINT32 w, FH_UINT32 h, FHT_RgbColor_t color);

static FH_SINT32 g_stop_nna_detect = 0;
static FH_SINT32 g_stop_submit_frame = 0;
static FH_SINT32 g_nna_running = 0;
static FH_SINT32 g_submit_frame_running = 0;

FACE_FRAME_LIST_t *g_face_list_head;
FACE_FRAME_LIST_t *g_face_list_tail;

static pthread_mutex_t g_list_lock;
static int g_lock_init;

static int push_face_info(unsigned int frame_id,
                          unsigned long long time_stamp,
                          unsigned char *luma,
                          unsigned char *chroma,
                          unsigned int w,
                          unsigned int h,
                          FH_DETECTION_EXT_T *det_face,
                          FH_FS_OUT_t *face_info)
{
    FACE_FRAME_LIST_t *cur;
    FACE_FRAME_LIST_t *lst;

    cur = malloc(sizeof(FACE_FRAME_LIST_t));
    if (!cur)
    {
        printf("malloc face info failed\n");
        return -1;
    }

    cur->frame_id = frame_id;
    cur->time_stamp = time_stamp;

    cur->luma = luma;
    cur->chroma = chroma;
    cur->det_face = *det_face;

    if (face_info)
        cur->best_face = *face_info;
    else
        cur->best_face.best_face_num = 0;

    pthread_mutex_lock(&g_list_lock);

    if (g_face_list_head == NULL)
    {
        g_face_list_head = malloc(sizeof(FACE_FRAME_LIST_t));
        g_face_list_head->pre = NULL;
    }

    if (g_face_list_tail == NULL)
    {
        g_face_list_tail = malloc(sizeof(FACE_FRAME_LIST_t));
        g_face_list_tail->next = NULL;
        g_face_list_tail->pre = g_face_list_head;
        g_face_list_head->next = g_face_list_tail;
    }

    lst = g_face_list_head->next;
    cur->next = lst;
    lst->pre = cur;
    g_face_list_head->next = cur;
    cur->pre = g_face_list_head;

    pthread_mutex_unlock(&g_list_lock);

    return 0;
}

static FACE_FRAME_LIST_t *pop_face_info(void)
{
    FACE_FRAME_LIST_t *tail;
    FACE_FRAME_LIST_t *tmp;

    pthread_mutex_lock(&g_list_lock);

    if (!g_face_list_tail)
    {
        pthread_mutex_unlock(&g_list_lock);
        return NULL;
    }

    tail = g_face_list_tail->pre;

    if (tail == g_face_list_head)
    {
        pthread_mutex_unlock(&g_list_lock);
        return NULL;
    }

    tmp = tail->pre;
    tmp->next = g_face_list_tail;
    g_face_list_tail->pre = tmp;

    pthread_mutex_unlock(&g_list_lock);

    return tail;
}

static FH_UINT8 *load_nn_models(FH_CHAR *modelPath)
{
    FH_SINT32 readLen;
    FH_SINT32 fileLen;
    FILE *fp;

    if (!(fp = fopen(modelPath, "rb")))
    {
        printf("Error[FH_NN]: open %s failed\n", modelPath);
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    fileLen = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    FH_UINT8 *buf;

    buf = malloc(fileLen);
    if (buf == NULL)
    {
        printf("Error[FH_NN]: malloc model buf failed, size = %d\n", fileLen);
        fclose(fp);
        return NULL;
    }

    readLen = fread(buf, 1, fileLen, fp);
    if (readLen < fileLen)
    {
        printf("Error[FH_NN]: read model file failed, fileLen = %d, readLen = %d\n", fileLen, readLen);
        free(buf);
        buf = NULL;
    }

    fclose(fp);
    return buf;
}

static void free_nn_models(FH_UINT8 *buf)
{
    free(buf);
}

static FH_SINT32 sample_nna_deinit(FH_VOID *detHandle)
{
    int ret = 0;

    if (detHandle)
    {
        ret = FH_NNA_DET_Exit(detHandle);
        if (ret != RETURN_OK)
        {
            printf("Error[FH_NN]: FH_NNA_DET_Exit failed, ret = %x\n", ret);
        }
    }

    return ret;
}

static FH_VOID *sample_init_fh_nn(FH_VOID)
{
    FH_UINT32 ret;
    FH_VOID *detHandle = NULL;
    FH_CHAR modelPath[256];
    FH_NN_INIT_PARAM_T nna_param;

    nna_param.src_w_in = NN_RGB_W;
    nna_param.src_h_in = NN_RGB_H;
    nna_param.src_c_in = 4;
    nna_param.type = FN_DET_FACEDET_FULL;
    nna_param.conf_thr = 0.5;
    nna_param.rotate = FN_ROT_0;

    strcpy(modelPath, "./face_landmark/facedet_full_03.nbg");

    nna_param.nbg_data = load_nn_models(modelPath);
    if (!nna_param.nbg_data)
    {
        return NULL;
    }

    ret = FH_NNA_DET_Init(&detHandle, 0, &nna_param);
    if (ret)
    {
        printf("FH_NNA_DET_Init failed with %x\n", ret);
        detHandle = NULL;
    }

    free_nn_models(nna_param.nbg_data);

    printf("FH_NNA_DET_Init ok !!! %p\n", detHandle);

    return detHandle;
}

static int draw_face_box(unsigned char *luma, unsigned char *chroma, unsigned int frame_w, unsigned int frame_h, unsigned int dt_x, unsigned int dt_y, unsigned int dt_w, unsigned int dt_h)
{
    int i, j;

    unsigned char *luma_start;
    unsigned short *chroma_start;

    unsigned int stride = frame_w;

    unsigned char y = 150;
    unsigned short c = (43 << 8) | 21;

    int line_len = 2;

    if (dt_x >= frame_w || dt_y >= frame_h || (dt_x + dt_w) < 0 || (dt_y + dt_h) < 0)
    {
        printf("%s %d %d %d %d %d %d\n", __func__, frame_w, frame_h, dt_x, dt_y, dt_w, dt_h);
        return 0;
    }

    if (dt_x < 0)
        dt_x = 0;

    if (dt_y < 0)
        dt_y = 0;

    if ((dt_x + dt_w) > frame_w)
        dt_w = frame_w - dt_x;

    if ((dt_y + dt_h) > frame_h)
        dt_h = frame_h - dt_y;

    // printf("face %d %d %d %d\n", dt_x, dt_y, dt_w, dt_h);

    luma_start = luma + dt_y * stride + dt_x;

    for (i = 0; i < line_len; i++)
    {
        memset(luma_start, y, dt_w);
        luma_start += stride;
    }

    chroma_start = (unsigned short *)(chroma + dt_y / 2 * stride + dt_x);

    for (i = 0; i < line_len / 2; i++)
    {
        for (j = 0; j < dt_w / 2; j++)
            chroma_start[j] = c;
        chroma_start += (stride / 2);
    }

    luma_start = luma + (dt_y + dt_h) * stride + dt_x;

    for (i = 0; i < line_len; i++)
    {
        memset(luma_start, y, dt_w);
        luma_start += stride;
    }

    chroma_start = (unsigned short *)(chroma + (dt_y + dt_h) / 2 * stride + dt_x);
    for (i = 0; i < line_len / 2; i++)
    {
        for (j = 0; j < dt_w / 2; j++)
            chroma_start[j] = c;
        chroma_start += (stride / 2);
    }

    luma_start = luma + dt_y * stride + dt_x;
    for (i = 0; i < dt_h; i++)
    {
        memset(luma_start, y, line_len);
        memset(luma_start + dt_w, y, line_len);
        luma_start += stride;
    }

    chroma_start = (unsigned short *)(chroma + dt_y / 2 * stride + dt_x);
    for (i = 0; i < dt_h / 2; i++)
    {
        for (j = 0; j < line_len / 2; j++)
        {
            chroma_start[j] = c;
            chroma_start[dt_w / 2 + j] = c;
        }
        chroma_start += (stride / 2);
    }

    return 0;
}

static int draw_face_point(unsigned char *luma, unsigned char *chroma, unsigned int frame_w, unsigned int frame_h, unsigned int x, unsigned int y, unsigned int point_edge_w)
{
    unsigned int start_x, start_y;
    unsigned int point_w, point_h;
    unsigned char *luma_start;
    unsigned short *chroma_start;

    unsigned char y_value = 156;
    unsigned short c_value = (43 << 8) | 21;

    int i, j;

    if (x > frame_w || y > frame_h || x < 0 || y < 0)
    {
        return 0;
    }

    start_x = x & (~15);
    start_y = y & (~15);

    point_w = (start_x + 16) > frame_w ? (frame_w - start_x) : 16;
    point_h = (start_y + 16) > frame_h ? (frame_h - start_y) : 16;

    luma_start = luma + start_y * frame_w + start_x;
    for (i = 0; i < point_h; i++)
    {
        memset(luma_start, y_value, point_w);
        luma_start += frame_w;
    }

    chroma_start = (unsigned short *)(chroma + start_y / 2 * frame_w + start_x);
    for (i = 0; i < point_h / 2; i++)
    {
        for (j = 0; j < point_w / 2; j++)
        {
            chroma_start[j] = c_value;
        }

        chroma_start += (frame_w / 2);
    }

    return 0;
}

#ifdef SAVE_FACES
static int clear_last_face(char *luma, char *chroma, unsigned int w, unsigned int h, unsigned int stride)
{
    int i;

    if (w == 0 || h == 0)
        return 0;

    for (i = 0; i < h; i++)
    {
        memset(luma, 0, w);
        luma += stride;
    }

    for (i = 0; i < h / 2; i++)
    {
        memset(chroma, 0, w);
        chroma += stride;
    }

    return 0;
}

static int save_face_yuv(char *name, char *luma, char *chroma, unsigned int w, unsigned int h, unsigned int stride)
{
    FILE *fp = fopen(name, "wb");

    if (!fp)
        return -1;

    int i;

    for (i = 0; i < h; i++)
    {
        fwrite(luma, 1, w, fp);
        luma += stride;
    }

    for (i = 0; i < (h + 1) / 2; i++)
    {
        fwrite(chroma, 1, w, fp);
        chroma += stride;
    }

    fclose(fp);

    return 0;
}
#endif

typedef struct _capture_pos
{
    unsigned int face_id;
    unsigned int start_x;
    int age;
} CAPTURE_POS_t;

FH_VOID *thread_submit_frame(FH_VOID *args)
{
    int ret;
    int i;
    int j;

    unsigned int frame_w, frame_h;
    unsigned int dt_x, dt_y, dt_w, dt_h;

    unsigned char *luma;
    unsigned char *chroma;
    FH_VPU_CHN_CONFIG vpu_attr;
    FH_ENC_FRAME encStr;
    struct enc_channel_info enc_info;

    sample_common_dsp_get_enc_chn_info(FH_APP_GRP_ID, DRAW_BOX_CHAN, &enc_info);
    enc_info.height = (enc_info.height + enc_info.height / 4) / 16 * 16;
    enc_info.max_height = enc_info.height;

    ret = FH_VENC_DestroyChn(FH_APP_GRP_ID * MAX_GRP_NUM + DRAW_BOX_CHAN);
    if (ret != 0)
    {
        printf("Error(%d - %x): FH_VENC_DestroyChn\n", ret, ret);
        goto Exit1;
    }

    ret = sample_common_enc_create_chan(FH_APP_GRP_ID, DRAW_BOX_CHAN, &enc_info);
    if (ret != 0)
    {
        printf("Error(%d - %x): sample_common_enc_create_chan\n", ret, ret);
        goto Exit1;
    }

    /* 按照目标分辨率重新创建主码流编码通道 */
    ret = sample_common_enc_set_chan(FH_APP_GRP_ID, DRAW_BOX_CHAN, &enc_info);
    if (ret != 0)
    {
        printf("Error(%d - %x): sample_common_enc_set_chan\n", ret, ret);
        goto Exit1;
    }

    ret = FH_VENC_StartRecvPic(FH_APP_GRP_ID * MAX_VPU_CHN_NUM + DRAW_BOX_CHAN);
    if (ret != 0)
    {
        printf("Error(%d-%x): FH_VENC_StartRecvPic!\n", ret, ret);
        goto Exit1;
    }

    ret = FH_VPSS_GetChnAttr(FH_APP_GRP_ID, DRAW_BOX_CHAN, &vpu_attr);
    frame_w = vpu_attr.vpu_chn_size.u32Width;
    frame_h = vpu_attr.vpu_chn_size.u32Height;

    unsigned int ext_h = frame_h / 4;
    unsigned int ext_w = frame_h / 4;

    FACE_FRAME_LIST_t *info;

    encStr.size.u32Width = frame_w;
    encStr.size.u32Height = (frame_h + ext_h) / 16 * 16;

    MEM_DESC encBuf[2];
    unsigned int buf_index = 0;

    buffer_malloc_withname(&encBuf[0], frame_w * (frame_h + ext_h), 1024, "face_eva_y_buf");
    buffer_malloc_withname(&encBuf[1], frame_w * (frame_h + ext_h) / 2, 1024, "face_eva_uv_buf");

    memset(encBuf[0].vbase, 0, encBuf[0].size);
    memset(encBuf[1].vbase, 0, encBuf[1].size);

    struct timeval t1, t2;
    unsigned int timediff;

    gettimeofday(&t2, NULL);

    unsigned int start_x = 0;

    unsigned int capture_num = frame_w / ext_w;
    CAPTURE_POS_t *face_pos = malloc(capture_num * sizeof(CAPTURE_POS_t));
    if (!face_pos)
    {
        goto Exit1;
    }

    for (i = 0; i < capture_num; i++)
    {
        face_pos[i].start_x = ext_w * i;
        face_pos[i].face_id = -1;
        face_pos[i].age = -1;
    }

    while (1)
    {
        info = pop_face_info();
        if (!info)
        {
            if (g_stop_submit_frame)
            {
                printf("exit submit thread\n");
                break;
            }

            usleep(5000);
            continue;
        }
        gettimeofday(&t1, NULL);

        timediff = (t1.tv_sec - t2.tv_sec) * 1000000 + (t1.tv_usec - t2.tv_usec);
        if (timediff < 30000)
        {
            usleep(30000 - timediff);
        }

        t2 = t1;

        luma = info->luma;
        chroma = info->chroma;

        if (!luma || !chroma)
        {
            if (luma)
                free(luma);
            if (chroma)
                free(chroma);
            printf("frame %d is not valid\n", info->frame_id);
            free(info);
            continue;
        }

#ifdef SAVE_FACES
        if (info->best_face.best_face_num)
        {
            memset(encBuf[buf_index].vbase + frame_w * frame_h, 0, frame_w * ext_h);
            memset(encBuf[buf_index].vbase + frame_w * (frame_h + ext_h) + frame_w * frame_h / 2, 0, frame_w * ext_h / 2);
            clear_last_face(encBuf.vbase + frame_w * frame_h, encBuf.vbase + frame_w * (frame_h + 120) + frame_w * frame_h / 2, ext_w, ext_h, frame_w);
        }

        if (info->best_face.best_face_num)
        {
            snprintf(name, sizeof(name), "%d_%dx%d.yuv", info->frame_id, frame_w, frame_h);
            save_face_yuv(name, luma, chroma, frame_w, frame_h, frame_w);
        }
#endif
        for (i = 0; i < info->best_face.best_face_num; i++)
        {
            start_x = -1;

            for (j = 0; j < capture_num; j++)
            {
                if (face_pos[j].face_id == info->best_face.best_faces[i].face_id)
                {
                    start_x = face_pos[j].start_x;
                    face_pos[j].age = 0;
                    printf("refresh face_pos[%d].face_id = %d\n", j, face_pos[j].face_id);
                    break;
                }
            }
            if (start_x == -1)
            {
                unsigned int max_age_id = 0;
                unsigned int max_age = 0;

                for (j = 0; j < capture_num; j++)
                {
                    if (face_pos[j].face_id == -1)
                    {
                        max_age_id = j;
                        printf("new face_pos[%d].face_id = %d\n", j, face_pos[j].face_id);
                        break;
                    }

                    if (face_pos[j].age > max_age)
                    {
                        max_age = face_pos[j].age;
                        max_age_id = j;
                    }
                }

                printf("replace face_pos[%d].face_id = %d last id = %d\n", max_age_id, face_pos[max_age_id].face_id,
                       info->best_face.best_faces[i].face_id);

                start_x = face_pos[max_age_id].start_x;
                face_pos[max_age_id].face_id = info->best_face.best_faces[i].face_id;
                face_pos[max_age_id].age = 1;
            }
            printf("face id %d start_x %d\n", info->best_face.best_faces[i].face_id, start_x);

            dt_x = frame_w * info->best_face.best_faces[i].face_info.bbox.x;
            dt_y = frame_h * info->best_face.best_faces[i].face_info.bbox.y;
            dt_w = frame_w * info->best_face.best_faces[i].face_info.bbox.w;
            dt_h = frame_h * info->best_face.best_faces[i].face_info.bbox.h;

            if (dt_x >= frame_w || dt_y >= frame_h || (dt_x + dt_w) < 0 || (dt_y + dt_h) < 0)
                continue;

            if (dt_x < 0)
                dt_x = 0;

            if (dt_y < 0)
                dt_y = 0;

            if ((dt_x + dt_w) > frame_w)
                dt_w = frame_w - dt_x;

            if ((dt_y + dt_h) > frame_h)
                dt_h = frame_h - dt_y;

            ext_w = ext_w & (~3);
            ext_h = ext_h & (~3);
            dt_x = (dt_x + 3) & (~3);
            dt_y = (dt_y + 3) & (~3);
            dt_w = (dt_w + 7) & (~7);
            dt_h = (dt_h + 7) & (~7);
            printf("rect %d %d %d %d\n", dt_x, dt_y, dt_w, dt_h);

            if (dt_w < ext_h && dt_h < ext_h)
            {
                unsigned char *src = luma + dt_y * frame_w + dt_x;
                unsigned char *dst = encBuf[buf_index].vbase + frame_w * frame_h + start_x;

                for (j = 0; j < dt_h; j++)
                {
                    memset(dst, 0, ext_w);
                    memcpy(dst, src, dt_w);
                    src += frame_w;
                    dst += frame_w;
                }

                for (; j < ext_h; j++)
                {
                    memset(dst, 0, ext_w);
                    dst += frame_w;
                }

                src = chroma + frame_w * dt_y / 2 + dt_x;
                dst = encBuf[1].vbase + frame_w * frame_h / 2 + start_x;

                for (j = 0; j < (dt_h + 1) / 2; j++)
                {
                    memset(dst, 0, ext_w);
                    memcpy(dst, src, dt_w);
                    src += frame_w;
                    dst += frame_w;
                }

                for (; j < (ext_h + 1) / 2; j++)
                {
                    memset(dst, 0, ext_w);
                    dst += frame_w;
                }

                start_x += ext_w;

                if (start_x >= (frame_w - ext_w))
                    start_x = 0;
            }
            else
            {
                luma_resize(encBuf[buf_index].vbase + frame_w * frame_h + start_x, ext_w, ext_h, frame_w, luma, frame_w, frame_h, frame_w,
                            dt_x, dt_y, dt_w, dt_h);

                chroma_resize(encBuf[1].vbase + frame_w * frame_h / 2 + start_x, ext_w, ext_h, frame_w, chroma, frame_w, frame_h, frame_w,
                              dt_x, dt_y, dt_w, dt_h);

                start_x += ext_w;
                if (start_x >= (frame_w - ext_w))
                    start_x = 0;
            }
        }

        for (j = 0; j < capture_num; j++)
        {
            face_pos[j].age++;
        }

        for (i = 0; i < info->det_face.boxNum; i++)
        {
            dt_x = frame_w * info->det_face.detBBox[i].bbox.x;
            dt_y = frame_h * info->det_face.detBBox[i].bbox.y;
            dt_w = frame_w * info->det_face.detBBox[i].bbox.w;
            dt_h = frame_h * info->det_face.detBBox[i].bbox.h;

            draw_face_box(luma, chroma, frame_w, frame_h, dt_x, dt_y, dt_w, dt_h);

            unsigned int point_w;

            if (dt_w > ext_w)
                point_w = 8;
            else
                point_w = 2;

            for (j = 0; j < 5; j++)
            {
                dt_x = frame_w * info->det_face.detBBox[i].landmark[j].x;
                dt_y = frame_h * info->det_face.detBBox[i].landmark[j].y;
                draw_face_point(luma, chroma, frame_w, frame_h, dt_x, dt_y, point_w);
            }
        }

        memcpy(encBuf[buf_index].vbase, luma, frame_w * frame_h);
        memcpy(encBuf[1].vbase, chroma, frame_w * frame_h / 2);

        encStr.lumma_addr = encBuf[buf_index].base;
        encStr.chroma_addr = encBuf[1].base;
        encStr.time_stamp = info->time_stamp;
        ret = FH_VENC_Submit_ENC_Ex(FH_APP_GRP_ID * MAX_VPU_CHN_NUM + DRAW_BOX_CHAN, &encStr, VPU_VOMODE_SCAN);
        if (ret)
        {
            printf("FH_VENC_Submit_ENC_Ex failed, ret = %x\n", ret);
        }

        free(luma);
        free(chroma);
        free(info);
    }

Exit1:
    g_submit_frame_running = 0;

    return NULL;
}

static FH_SINT32 dsp_init(FH_VOID)
{
    FH_SINT32 ret;
    FH_BIND_INFO src;
    struct vpu_channel_info vpu_info;
    struct grp_vpu_info grp_info;

    ret = FH_VPSS_DestroyChn(FH_APP_GRP_ID, DRAW_BOX_CHAN);
    if (ret)
    {
        printf("Error: FH_VPSS_DestroyChn failed with %x!\n", ret);
        return ret;
    }

    sample_common_dsp_get_vpu_chn_info(FH_APP_GRP_ID, DRAW_BOX_CHAN, &vpu_info);
    ret = sample_common_vpu_create_chan(FH_APP_GRP_ID, DRAW_BOX_CHAN, vpu_info.width, vpu_info.height, VPU_VOMODE_SCAN);
    if (ret != 0)
    {
        printf("Error(%d - %x): sample_common_vpu_create_chan\n", ret, ret);
        return ret;
    }

    sample_common_dsp_get_grp_info(FH_APP_GRP_ID, &grp_info);
    if (grp_info.bgm_enable)
    {
        FH_BIND_INFO src, dst;

        src.obj_id = FH_OBJ_VPU_BGM;
        src.dev_id = 0;
        src.chn_id = grp_info.channel;
        dst.obj_id = FH_OBJ_BGM;
        dst.dev_id = 0;
        dst.chn_id = grp_info.channel;
        ret = FH_SYS_Bind(src, dst);
        if (ret != 0)
        {
            printf("Error(%d - %x): FH_SYS_Bind VPU to BGM(%d to %d)\n", ret, ret, grp_info.channel, grp_info.channel);
            return ret;
        }
    }

    src.obj_id = FH_OBJ_VPU_VO;
    src.dev_id = FH_APP_GRP_ID;
    src.chn_id = DRAW_BOX_CHAN;

    ret = FH_SYS_UnBindbyDst(src);
    if (ret != 0)
    {
        printf("Error(%d - %x): FH_SYS_UnBindbyDst!\n", ret, ret);
        return ret;
    }

    return ret;
}

/* 512 * 288 */
FH_VOID *thread_face_caputre(FH_VOID *args)
{
    FH_SINT32 ret;
    FH_VOID *detHandle = NULL;
    FH_DETECTION_INFO nn_out;

    FH_VPU_STREAM_ADV vpuStr;
    FH_VPU_STREAM_ADV rgbStr;

    unsigned long long last_pts = -1;

    detHandle = sample_init_fh_nn();
    if (detHandle == NULL)
    {
        printf("Error[FH_NN]: sample_init_fh_nn failed\n");
        goto Exit;
    }

    struct timeval t1, t2;
    ret = dsp_init();
    if (ret)
    {
        printf("Error: unBind failed with %x, cannot draw box on jpeg\n", ret);
        goto Exit;
    }

    FH_FS_Cfg_t fs_cfg;

    fs_cfg.src_w = NN_RGB_W;
    fs_cfg.src_h = NN_RGB_H;

    fs_cfg.quality_th = 100;
    fs_cfg.roll_th = 90;
    fs_cfg.yaw_th = 90;
    fs_cfg.pitch_th = 90;
    fs_cfg.pose_alpha = 60;
    fs_cfg.rotate_angle = 0; //根据FN_ROT_0填写
    fs_cfg.hold_time = 30;

    FH_SINT32 pose_weight[3] = {500, 1000, 2000};
    FH_SINT32 occlude_weight[5] = {5, 5, 1, 1, 1};

    memcpy(fs_cfg.pose_weight, pose_weight, sizeof(fs_cfg.pose_weight));
    memcpy(fs_cfg.occlude_weight, occlude_weight, sizeof(fs_cfg.occlude_weight));

    fs_cfg.maxTrackNum = 15;
    fs_cfg.iouThresh = 10;
    fs_cfg.minHits = 2;
    fs_cfg.maxAge = 15;

    ret = FH_FS_Init(&fs_cfg);
    if (ret)
    {
        printf("Error: FH_FS_Init failed, ret = %d\n", ret);
        goto Exit;
    }

    FH_FS_OUT_t best_face;
    unsigned int frame_id = 0;

    while (!g_stop_nna_detect)
    {
        ret = FH_VPSS_GetChnFrameAdv(FH_APP_GRP_ID, DRAW_BOX_CHAN, &vpuStr, 0);
        if (ret)
        {
            continue;
        }

        if (last_pts == vpuStr.time_stamp)
        {
            usleep(10 * 1000);
            continue;
        }

        last_pts = vpuStr.time_stamp;

        ret = FH_VPSS_GetChnFrameAdv(FH_APP_GRP_ID, RGB_CHAN, &rgbStr, 0);
        if (ret)
        {
            continue;
        }

        if (rgbStr.time_stamp != vpuStr.time_stamp)
        {
            printf("frame not sync!!!!!!!!!!!!!!!!!\n");
        }

        unsigned char *luma = malloc(vpuStr.frm_scan.luma.data.size);
        unsigned char *chroma = malloc(vpuStr.frm_scan.chroma.data.size);

        if (luma)
            memcpy(luma, vpuStr.frm_scan.luma.data.vbase, vpuStr.frm_scan.luma.data.size);

        if (chroma)
            memcpy(chroma, vpuStr.frm_scan.chroma.data.vbase, vpuStr.frm_scan.chroma.data.size);

        FH_IMAGE_T src;
        src.width = NN_RGB_W;
        src.height = NN_RGB_H;
        src.stride = NN_RGB_W;
        src.timestamp = rgbStr.time_stamp;
        src.imageType = FH_IMAGE_FORMAT_RGB888;
        src.src_data = rgbStr.frm_rgb888.data;

        gettimeofday(&t2, NULL);

        ret = FH_NNA_DET_POINT_Process(detHandle, &src, &nn_out);
        if (ret)
        {
			free(luma);
			free(chroma);
            printf("Error[FH_NN]: FH_NNA_DET_Process failed, ret = %x\n", ret);
            continue;
        }

        ret = FH_FS_Process(&nn_out.face_detect_t, &best_face);
        if (ret)
        {
            printf("FH_FS_Process failed, ret = %d\n", ret);
        }

        if (best_face.best_face_num)
            printf("frameid %d best_face num %d\n", frame_id, best_face.best_face_num);

        if (best_face.best_face_num)
            push_face_info(frame_id, vpuStr.time_stamp, luma, chroma, vpuStr.size.u32Width, vpuStr.size.u32Height, &nn_out.face_detect_t, &best_face);
        else
            push_face_info(frame_id, vpuStr.time_stamp, luma, chroma, vpuStr.size.u32Width, vpuStr.size.u32Height, &nn_out.face_detect_t, NULL);

        frame_id++;

        gettimeofday(&t1, NULL);

        unsigned int timediff;
        timediff = (t1.tv_sec - t2.tv_sec) * 1000000 + t1.tv_usec - t2.tv_usec;

        if (timediff < 40000)
            usleep(40000 - timediff);
    }

Exit:
    g_nna_running = 0;

    FH_FS_Exit();

    if (detHandle)
        sample_nna_deinit(detHandle);

    return NULL;
}

FH_SINT32 sample_face_snap_start(FH_VOID)
{
    pthread_t nn_thread;
    pthread_attr_t nn_attr;

    pthread_t submit_thread;
    pthread_attr_t submit_attr;

    if (g_nna_running)
    {
        printf("[FH_NN] nna demo is running\n");
        return 0;
    }

    if (!g_lock_init)
    {
        pthread_mutex_init(&g_list_lock, NULL);
        g_lock_init = 1;
    }

    g_stop_nna_detect = 0;

    pthread_attr_init(&nn_attr);

    pthread_attr_setdetachstate(&nn_attr, PTHREAD_CREATE_DETACHED);
#ifdef __RTTHREAD_OS__
    pthread_attr_setstacksize(&nn_attr, 60 * 1024);
#endif
    if (pthread_create(&nn_thread, &nn_attr, thread_face_caputre, NULL))
    {
        printf("Error[FH_NN]: pthread_create failed\n");
        goto Exit;
    }

    g_nna_running = 1;

    g_stop_submit_frame = 0;

    pthread_attr_init(&submit_attr);
    pthread_attr_setdetachstate(&submit_attr, PTHREAD_CREATE_DETACHED);
#ifdef __RTTHREAD_OS__
    pthread_attr_setstacksize(&submit_attr, 60 * 1024);
#endif    
    if (pthread_create(&submit_thread, &submit_attr, thread_submit_frame, NULL))
    {
        printf("Error[FH_NN]: pthread_create failed\n");
        goto Exit;
    }

    g_submit_frame_running = 1;

    return 0;

Exit:
    return -1;
}

FH_SINT32 sample_face_snap_stop(FH_VOID)
{
    g_stop_nna_detect = 1;

    while (g_nna_running)
    {
        usleep(10);
    }

    g_stop_submit_frame = 1;

    while (g_submit_frame_running)
    {
        usleep(10);
    }

    if (g_lock_init)
    {
        pthread_mutex_destroy(&g_list_lock);
        g_lock_init = 0;
    }

    if (g_face_list_tail)
    {
        free(g_face_list_tail);
        g_face_list_tail = NULL;
    }

    if (g_face_list_head)
    {
        free(g_face_list_head);
        g_face_list_head = NULL;
    }

    return 0;
}
