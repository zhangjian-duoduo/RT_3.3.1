/* 适用芯片: FH865x系列 */

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
#include "fh_face_rec.h"
#include "sample_common.h"
#include "sample_nna_detect.h"

#define RGB_CHAN (3) /* 只有通道3支持RGB输出格式 */
#define RGB_CHAN_WIDTH (512)
#define RGB_CHAN_HEIGHT (288)

#define DRAW_BOX_CHAN (0)

#define CHECK_COUNT 25

#define NN_DRAW_BOX

#define DEBUG 0

#define ALIGNTO(addr, edge) ((addr + edge - 1) & ~(edge - 1))

extern FH_SINT32 sample_set_gbox(FH_UINT32 chan, FH_UINT32 enable, FH_UINT32 box_id, FH_UINT32 x, FH_UINT32 y, FH_UINT32 w, FH_UINT32 h, FHT_RgbColor_t color);

static FH_SINT32 g_stop_nna_detect = 0;
static FH_SINT32 g_nna_running = 0;
static FH_SINT32 g_do_recg = 0;

typedef struct jpeg_config
{
    MEM_DESC luma_buf;
    MEM_DESC chroma_buf;
    FH_UINT32 jpeg_enc_channel;
    FH_UINT32 jpeg_width;
    FH_UINT32 jpeg_height;
} JPEG_CONFIG;

static FH_SINT32 jpeg_chan_init(JPEG_CONFIG *jpg_config)
{
    FH_SINT32 ret;
    struct vpu_channel_info vpu_info;
    struct enc_channel_info jpeg_info;
    struct enc_channel_info enc_info;

    jpg_config->jpeg_enc_channel = 31;

    ret = FH_VPSS_DestroyChn(FH_APP_GRP_ID, DRAW_BOX_CHAN);
    if (ret)
    {
        printf("Error [FH_REC]: jpeg_chan_init FH_VPSS_DestroyChn failed with= %x\n", ret);
        return ret;
    }

    sample_common_dsp_get_vpu_chn_info(FH_APP_GRP_ID, DRAW_BOX_CHAN, &vpu_info);
    ret = sample_common_vpu_create_chan(FH_APP_GRP_ID, DRAW_BOX_CHAN, vpu_info.width, vpu_info.height, VPU_VOMODE_SCAN);
    if (ret != 0)
    {
        printf("Error [FH_REC]: jpeg_chan_init sample_common_vpu_create_chan failed with= %x\n", ret);
        return ret;
    }

    sample_common_dsp_get_enc_chn_info(FH_APP_GRP_ID, DRAW_BOX_CHAN, &enc_info);
    ret = sample_common_vpu_bind_to_enc(FH_APP_GRP_ID, DRAW_BOX_CHAN, enc_info.channel);
    if (ret != 0)
    {
        printf("Error [FH_REC]: (%d - %x) FH_SYS_Bind VPU to ENC([%d][%d] to %d)\n", ret, ret, FH_APP_GRP_ID, DRAW_BOX_CHAN, enc_info.channel);
        return ret;
    }

    jpg_config->jpeg_width = vpu_info.width;
    jpg_config->jpeg_height = vpu_info.height;

    jpeg_info.channel = jpg_config->jpeg_enc_channel;

    jpeg_info.enc_type = FH_JPEG;
    jpeg_info.max_width = vpu_info.max_width;
    jpeg_info.max_height = vpu_info.max_height;
    ret = sample_common_enc_create_chan(0, jpeg_info.channel, &jpeg_info);
    if (ret)
    {
        printf("Error [FH_REC]: jpeg enc chn create failed = %x\n", ret);
        return ret;
    }

    ret = sample_common_enc_set_chan(0, jpeg_info.channel, &jpeg_info);
    if (ret)
    {
        printf("Error [FH_REC]: jpeg enc chn config set failed = %x\n", ret);
        return ret;
    }

    // 为提交到jpeg通道的yuv数据分配buffer
    ret = buffer_malloc_withname(&jpg_config->luma_buf, vpu_info.width * vpu_info.height, 1024, "jpeg_luma");
    if (ret)
    {
        printf("Error [FH_REC]: luma_buf buffer malloc failed = %x\n", ret);
        return ret;
    }

    ret = buffer_malloc_withname(&jpg_config->chroma_buf, vpu_info.width * vpu_info.height / 2, 1024, "jpeg_chroma");
    if (ret)
    {
        printf("Error [FH_REC]: chroma_buf buffer malloc failed = %x\n", ret);
        return ret;
    }

    return 0;
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
        dt_w = frame_w - dt_x - line_len;

    if ((dt_y + dt_h) > frame_h)
        dt_h = frame_h - dt_y - line_len;

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

static FH_SINT32 jpeg_save(FH_VPU_STREAM_ADV vpuChn, FH_FS_BEST_FACE_t best_face_info, JPEG_CONFIG jpg_config, FH_FACE_REC_BASE face_base)
{
    FH_SINT32 ret = 0;
    FH_FACE_REC_FACEBASE_t faceBaseInfo;
    FH_UINT32 dt_x, dt_y, dt_w, dt_h;

    memcpy(jpg_config.luma_buf.vbase, vpuChn.frm_scan.luma.data.vbase, vpuChn.frm_scan.luma.data.size);
    memcpy(jpg_config.chroma_buf.vbase, vpuChn.frm_scan.chroma.data.vbase, vpuChn.frm_scan.chroma.data.size);

    ret = FH_FACE_REC_Info(face_base, &faceBaseInfo);
    if (ret)
    {
        printf("Error [FH_REC]: FH_FACE_REC_Info  failed = %x\n", ret);
    }

    dt_x = jpg_config.jpeg_width * best_face_info.face_info.bbox.x;
    dt_y = jpg_config.jpeg_height * best_face_info.face_info.bbox.y;
    dt_w = jpg_config.jpeg_width * best_face_info.face_info.bbox.w;
    dt_h = jpg_config.jpeg_height * best_face_info.face_info.bbox.h;

    draw_face_box(jpg_config.luma_buf.vbase, jpg_config.chroma_buf.vbase, jpg_config.jpeg_width, jpg_config.jpeg_height, dt_x, dt_y, dt_w, dt_h);

    FH_ENC_FRAME encStr;

    encStr.size.u32Width = jpg_config.jpeg_width;
    encStr.size.u32Height = jpg_config.jpeg_height;
    encStr.lumma_addr = jpg_config.luma_buf.base;
    encStr.chroma_addr = jpg_config.chroma_buf.base;
    encStr.time_stamp = vpuChn.time_stamp;

    ret = FH_VENC_Submit_ENC_Ex(jpg_config.jpeg_enc_channel, &encStr, VPU_VOMODE_SCAN);
    if (ret)
    {
        printf("Error [FH_REC]: FH_VENC_Submit_ENC_Ex  failed = %x\n", ret);
    }

    FH_VENC_STREAM stream;
    ret = FH_VENC_GetStream_Block(FH_STREAM_JPEG, &stream);
    if (ret)
    {
        printf("Error [FH_REC]: FH_VENC_GetStream_Block  failed = %x\n", ret);
        return ret;
    }

    char name[64];
    snprintf(name, 64, "face_pic_%d.jpeg", ++faceBaseInfo.count);

    FILE *fp_face = fopen(name, "wb");
    if (fp_face)
    {
        fwrite(stream.jpeg_stream.start, 1, stream.jpeg_stream.length, fp_face);
        fflush(fp_face);
        fclose(fp_face);
    }
    printf("save face_pic_%d.jpeg success!\n", faceBaseInfo.count);

    ret = FH_VENC_ReleaseStream(&stream);
    if (ret)
    {
        printf("Error [FH_REC]: FH_VENC_ReleaseStream  failed = %x\n", ret);
        return ret;
    }

    return faceBaseInfo.count;
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

static FH_VOID *sample_init_fh_nn(FH_UINT32 chan, FH_UINT32 detType, FH_UINT32 src_w, FH_UINT32 src_h, FH_FLOAT thresh)
{
    FH_UINT32 ret;
    FH_VOID *detHandle = NULL;
    FH_CHAR modelPath[256];
    FH_NN_INIT_PARAM_T nna_param;

    nna_param.src_w_in = src_w;
    nna_param.src_h_in = src_h;
    nna_param.src_c_in = 4;
    nna_param.type = detType;
    nna_param.conf_thr = thresh;
    nna_param.rotate = FN_ROT_0;

    if (nna_param.type == FN_DET_FACEDET_FULL)
    {
        strcpy(modelPath, "./face_landmark/facedet_full.nbg");
    }
    else if (nna_param.type == FN_DET_FACEREC)
    {
        strcpy(modelPath, "./face_recognition/facerec.nbg");
    }
    else
    {
        printf("Invalid detect type %d\n", nna_param.type);
        return NULL;
    }

    nna_param.nbg_data = load_nn_models(modelPath);
    if (!nna_param.nbg_data)
    {
        return NULL;
    }

    ret = FH_NNA_DET_Init(&detHandle, chan, &nna_param);
    if (ret)
    {
        printf("FH_NNA_DET_Init failed with %x\n", ret);
        detHandle = NULL;
    }

    free_nn_models(nna_param.nbg_data);

    printf("FH_NNA_DET_Init ok !!! %p\n", detHandle);

    return detHandle;
}

static FH_VOID sample_fr_init(FH_VOID)
{
    int ret;
    FH_FS_Cfg_t fs_cfg;

    FH_SINT32 pose_weight[3] = {500, 1000, 2000};
    FH_SINT32 occlude_weight[5] = {5, 5, 1, 1, 1};

    fs_cfg.src_w = RGB_CHAN_WIDTH;
    fs_cfg.src_h = RGB_CHAN_HEIGHT;

    fs_cfg.quality_th = FH_FACE_CAPTURE_TH;
    fs_cfg.roll_th = FH_FACE_ROLL_TH;
    fs_cfg.yaw_th = FH_FACE_YAW_TH;
    fs_cfg.pitch_th = FH_FACE_PITCH_TH;
    fs_cfg.pose_alpha = 60;
    fs_cfg.rotate_angle = 0; //根据FN_ROT_0填写
    fs_cfg.hold_time = 30;

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
        return;
    }

    return;
}

static FH_SINT32 check_parmas(FH_RECT_T *rect, int chn_width, int chn_height)
{
    FH_SINT32 ret = 0;

    if (rect->x < 0 || rect->y < 0 || rect->w < 0 || rect->h < 0)
    {
        ret = -1;
    }

    if ((rect->x + rect->w) > chn_width) // fix over size
    {
        rect->w = chn_width - rect->x - 1;
    }
    if ((rect->y + rect->h) > chn_height)
    {
        rect->h = chn_height - rect->y - 1;
    }

    return ret;
}

static FH_VOID clear_box(FH_VOID)
{
    FHT_RgbColor_t box_color = {0};

    sample_set_gbox(DRAW_BOX_CHAN, 0, 0, 0, 0, 0, 0, box_color);
}

static FH_VOID draw_box(FH_FS_BEST_FACE_t best_face_info)
{
#ifdef NN_DRAW_BOX
    FH_SINT32 ret = -1;
    FHT_RgbColor_t box_color;
    FH_RECT_T box;
    FH_UINT32 chnw;
    FH_UINT32 chnh;
    FH_VPU_CHN_CONFIG box_chan_cfg;

    ret = FH_VPSS_GetChnAttr(FH_APP_GRP_ID, DRAW_BOX_CHAN, &box_chan_cfg);
    if (ret)
    {
        printf("Error[FH_NN]: FH_VPSS_GetChnAttr failed, ret = %x\n", ret);
        return;
    }

    chnw = box_chan_cfg.vpu_chn_size.u32Width;
    chnh = box_chan_cfg.vpu_chn_size.u32Height;

    box_color.fRed = 255;
    box_color.fGreen = 0;
    box_color.fBlue = 0;
    box_color.fAlpha = 255;

    sample_set_gbox(DRAW_BOX_CHAN, 0, 0, 0, 0, 0, 0, box_color);

    box.x = best_face_info.face_info.bbox.x * chnw;
    box.y = best_face_info.face_info.bbox.y * chnh;
    box.w = best_face_info.face_info.bbox.w * chnw;
    box.h = best_face_info.face_info.bbox.h * chnh;
    if (!check_parmas(&box, chnw, chnh)) // over size check
    {
        sample_set_gbox(DRAW_BOX_CHAN, 1, 0, box.x, box.y, box.w, box.h, box_color);
    }

#endif
}

static FH_SINT32 save_pic_c(FH_IMAGE_T src, char *name)
{
#if DEBUG
    static int num = 0;
    char file_name[64];
    int fd = 0;
    snprintf(file_name, 64, "./pic_raw/%s_%d.raw", name, num);
    printf("save_pic_start[%s-%d]\n", name, num);
    printf("ADDR = %p,SIZE = %X\n", src.src_data.vbase, src.src_data.size);
    fd = open(file_name, O_WRONLY | O_CREAT);
    write(fd, src.src_data.vbase, src.src_data.size);
    close(fd);
    printf("save_pic_end[%s-%d]\n", name, num);

    return num++;
#endif
    return -1;
}

/* 512 * 288 */
FH_VOID *thread_face_rec(FH_VOID *args)
{
    FH_SINT32 ret, i;
    FH_SINT64 time_stamp = 0;
    FH_VOID *detHandle = NULL;
    FH_VOID *recHandle = NULL;
    FH_FACE_REC_BASE face_base = NULL;
    FH_VPU_STREAM_ADV vpuStr;
    FH_VPU_STREAM_ADV rgbStr;
    FH_FS_OUT_t best_face;
    FH_DETECTION_INFO nn_out;
    MEM_DESC nn_face_in;
    JPEG_CONFIG jpg_config;

    // 初始化人脸检测NN通道
    detHandle = sample_init_fh_nn(0, FN_DET_FACEDET_FULL, RGB_CHAN_WIDTH, RGB_CHAN_HEIGHT, 0.8);
    if (detHandle == NULL)
    {
        printf("Error [FH_REC]: sample_init_fh_nn FN_DET_FACEDET_FULL failed\n");
        goto Exit;
    }
    // 初始化人脸识别NN通道
    recHandle = sample_init_fh_nn(1, FN_DET_FACEREC, 112, 112, 0.5);
    if (recHandle == NULL)
    {
        printf("Error [FH_REC]: sample_init_fh_nn FN_DET_FACEREC failed\n");
        goto Exit;
    }

    //创建人脸抓拍handle
    sample_fr_init();

    //分配人脸rbg buffer，方便提交数据到nn人脸识别通道
    ret = buffer_malloc_withname(&nn_face_in, 112 * 112 * 4, 1024, "nn_data_face");
    if (ret)
    {
        printf("Error [FH_REC]: buffer_malloc_withname failed with= %x\n", ret);
        goto Exit;
    }

    if (g_do_recg)
        face_base = FH_FACE_REC_CreateFaceBase("face_recg.bin"); // 如果做人脸识别，加载已有的人脸数据库
    else
        face_base = FH_FACE_REC_CreateFaceBase(NULL); // 如果不做人脸识别，创建新的人脸数据库

    // 初始化jpeg相关配置
    if (jpeg_chan_init(&jpg_config))
    {
        printf("Error [FH_REC]: jpeg_chan_init failed!\n");
        goto Exit;
    }

    while (!g_stop_nna_detect)
    {
        ret = FH_VPSS_GetChnFrameAdv(FH_APP_GRP_ID, DRAW_BOX_CHAN, &vpuStr, 1000);
        if (ret)
        {
            continue;
        }

        ret = FH_VPSS_GetChnFrameAdv(FH_APP_GRP_ID, RGB_CHAN, &rgbStr, 0);
        if (ret)
        {
            continue;
        }

        if (rgbStr.time_stamp != vpuStr.time_stamp)
        {
            continue;
        }

        if (time_stamp % CHECK_COUNT == 0) //达到刷新要求
        {
            time_stamp = 1;
            clear_box();
        }

        time_stamp++;

        FH_IMAGE_T src;
        src.width = RGB_CHAN_WIDTH;
        src.height = RGB_CHAN_HEIGHT;
        src.stride = RGB_CHAN_WIDTH;
        src.timestamp = rgbStr.time_stamp;
        src.imageType = FH_IMAGE_FORMAT_RGB888;
        src.src_data = rgbStr.frm_rgb888.data;

        ret = FH_NNA_DET_POINT_Process(detHandle, &src, &nn_out);
        if (ret)
        {
            printf("Error [FH_REC]: FH_NNA_DET_POINT_Process failed with= %x\n", ret);
            continue;
        }

        // printf("face num %d\n", nn_out.face_detect_t.boxNum);

        ret = FH_FS_Process(&nn_out.face_detect_t, &best_face);
        if (ret)
        {
            printf("Error [FH_REC]: FH_FS_Process failed with= %x\n", ret);
            continue;
        }

        // printf("best face num %d\n", best_face.best_face_num);
        for (i = 0; i < best_face.best_face_num; i++)
        {
            FH_IMAGE_T faceTransIN;
            FH_IMAGE_T faceTransOut;

            faceTransIN.width = RGB_CHAN_WIDTH;
            faceTransIN.height = RGB_CHAN_HEIGHT;
            faceTransIN.stride = RGB_CHAN_WIDTH * 4;
            faceTransIN.timestamp = rgbStr.time_stamp;
            faceTransIN.imageType = FH_IMAGE_FORMAT_RGB888;
            faceTransIN.src_data.vbase = rgbStr.frm_rgb888.data.vbase;
            faceTransIN.src_data.size = rgbStr.frm_rgb888.data.size;
            faceTransIN.src_data.base = rgbStr.frm_rgb888.data.base;

            faceTransOut.width = 112;
            faceTransOut.height = 112;
            faceTransOut.stride = 112 * 4;
            faceTransOut.timestamp = rgbStr.time_stamp;
            faceTransOut.imageType = FH_IMAGE_FORMAT_RGB888;
            faceTransOut.src_data.vbase = nn_face_in.vbase;
            faceTransOut.src_data.base = nn_face_in.base;
            faceTransOut.src_data.size = nn_face_in.size;

            ret = FH_FR_Data_Trans(&faceTransIN, &best_face.best_faces[i].face_info, &faceTransOut);
            if (ret)
            {
                printf("Error [FH_REC]: FH_FR_Data_Trans failed with= %x\n", ret);
                break;
            }

            save_pic_c(faceTransIN, "768x448");
            save_pic_c(faceTransOut, "112x112");
            ret = FH_NNA_DET_POINT_Process(recHandle, &faceTransOut, &nn_out);
            if (ret)
            {
                printf("Error [FH_REC]: FH_NNA_DET_Process failed with= %x\n", ret);
                continue;
            }

            if (!g_do_recg)
            {
                int pic_num = jpeg_save(vpuStr, best_face.best_faces[i], jpg_config, face_base);
                printf("please check pic %d and input face label(label should not be contained of more than %d characters):\n", pic_num, FH_FACE_REC_NAME_LENGTH);
                char label[FH_FACE_REC_NAME_LENGTH];

                scanf("%s", label);

                if (!strcmp(label, "skip"))
                {
                    printf("label %s\n", label);
                    continue;
                }

                ret = FH_FACE_REC_Register(face_base, &nn_out.face_recognition_t.embedding_t, label);
                if (ret)
                {
                    printf("register face failed\n");
                }
                else
                {
                    printf("register label %s success\n", label);
                    FH_FACE_REC_Save(face_base, "face_recg.bin");
                }
            }
            else
            {
                FH_FACE_REC_INFO_t rec_out;
                FH_FACE_REC_Probe(face_base, &nn_out.face_recognition_t.embedding_t, &rec_out);

                if (rec_out.similarity > 0.5)
                {
                    printf("Find face label[%s], Similarity = %d%%\n", rec_out.label, (int)(rec_out.similarity * 100));
                    draw_box(best_face.best_faces[i]);
                }
            }
        }
    }

Exit:
    if (face_base)
    {
        FH_FACE_REC_DestroyFaceBase(face_base);
    }
    FH_FS_Exit();
    sample_nna_deinit(recHandle);
    sample_nna_deinit(detHandle);
    g_nna_running = 0;

    return NULL;
}

FH_SINT32 sample_face_rec_start()
{
    pthread_t nn_thread;
    pthread_attr_t nn_attr;

    if (g_nna_running)
    {
        printf("[FH_NN] nna demo is running\n");
        return 0;
    }

    g_stop_nna_detect = 0;

    g_do_recg = FH_FACE_REC_MODE;

    pthread_attr_init(&nn_attr);

    pthread_attr_setdetachstate(&nn_attr, PTHREAD_CREATE_DETACHED);
    
#ifdef __RTTHREAD_OS__
    pthread_attr_setstacksize(&nn_attr, 256 * 1024);
#endif
    if (pthread_create(&nn_thread, &nn_attr, thread_face_rec, NULL))
    {
        printf("Error[FH_NN]: pthread_create failed\n");
        goto Exit;
    }

    g_nna_running = 1;

    return 0;

Exit:
    return -1;
}

FH_SINT32 sample_face_rec_stop(FH_VOID)
{
    g_stop_nna_detect = 1;

    while (g_nna_running)
    {
        usleep(10);
    }

    return 0;
}
