/* 适用芯片: FH865x系列 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/prctl.h>
#include <unistd.h>
#include <math.h>
#include <assert.h>

#include "types/bufCtrl.h"
#include "dsp/fh_system_mpi.h"
#include "dsp/fh_nna_mpi.h"
#include "dsp/fh_vpu_mpi.h"
#include "dsp/fh_venc_mpi.h"
#include "types/type_def.h"
#include "sample_common.h"
#include "sample_nna_detect.h"
#include "fh_nn_face_align.h"

#define RGB_CHAN (3)

// #define DEBUG

#define RGB_CHAN_WIDTH (512)
#define RGB_CHAN_HEIGHT (288)

#define NN_RGB_W (512)
#define NN_RGB_H (288)
#define DRAW_BOX_CHAN (0)

extern FH_SINT32 sample_set_gbox(FH_UINT32 chan, FH_UINT32 enable, FH_UINT32 box_id, FH_UINT32 x, FH_UINT32 y, FH_UINT32 w, FH_UINT32 h, FHT_RgbColor_t color);

static FH_SINT32 g_stop_nna_detect = 0;
static FH_SINT32 g_nna_running = 0;

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

static FH_SINT32 sample_nna_deinit(FH_VOID *handle)
{
    int ret = 0;

    if (handle)
    {
        ret = FH_NNA_DET_Exit(handle);
        if (ret != RETURN_OK)
        {
            printf("Error[FH_NN]: FH_NNA_DET_Exit failed, ret = %x\n", ret);
        }
    }

    return ret;
}

static FH_VOID *sample_init_fh_nn(FH_UINT32 chan, FH_UINT32 type)
{
    FH_UINT32 ret;
    FH_VOID *handle = NULL;
    FH_CHAR modelPath[256];
    FH_NN_INIT_PARAM_T nna_param;

    nna_param.src_c_in = 4;
    nna_param.type = type;
    nna_param.rotate = FN_ROT_0;
    if (type == FN_DET_FACEDET_FULL)
    {
        nna_param.src_w_in = NN_RGB_W;
        nna_param.src_h_in = NN_RGB_H;
        nna_param.conf_thr = 0.8;
        strcpy(modelPath, "./face_landmark/facedet_full.nbg");
    }
    else if (type == FN_DET_FACEATTR)
    {
        nna_param.src_w_in = 112;
        nna_param.src_h_in = 112;
        nna_param.conf_thr = 0.5;
        strcpy(modelPath, "./face_attrs/faceattr.nbg");
    }

    nna_param.nbg_data = load_nn_models(modelPath);
    if (!nna_param.nbg_data)
    {
        return NULL;
    }

    ret = FH_NNA_DET_Init(&handle, chan, &nna_param);
    if (ret)
    {
        printf("FH_NNA_DET_Init failed with %x\n", ret);
        handle = NULL;
    }

    free_nn_models(nna_param.nbg_data);

    printf("FH_NNA_DET_Init ok !!! %p\n", handle);

    return handle;
}

#ifdef DEBUG
static void read_file(char *file_name, MEM_DESC *addr)
{
    int fd = 0;
    fd = open(file_name, O_RDONLY);
    int num = read(fd, addr->vbase, addr->size);
    printf("read ADDR = %p,SIZE = %X, read size = %X\n", addr->vbase, addr->size, num);
    close(fd);
}

void save_data(char *data, char *file_name, int id_num)
{
    FILE *fp;
    char name[64];
    snprintf(name, 64, "./pic_raw/%s_%d.log", file_name, id_num);
    fp = fopen(name, "a+");
    fprintf(fp, "%s", data);
    fclose(fp);
    printf("=========save_data[%s-%d]==============\n", name, id_num);
}

void save_pic_c(unsigned char *src, int src_size, char *file_name, int id_num)
{
    FILE *fp = NULL;
    char name[64];
    snprintf(name, 64, "./pic_raw/%s_%d.raw", file_name, id_num);
    fp = fopen(name, "w");
    fwrite(src, src_size, 1, fp);
    fclose(fp);
    printf("======save pic[%s-%d]==ADDR[%p]==SIZE[%X]======\n", name, id_num, src, src_size);
}
#endif

/* 512 * 288 */
FH_VOID *thread_face_attr(FH_VOID *args)
{
    FH_SINT32 ret;
    FH_VOID *landmarkHandle = NULL;
    FH_VOID *face_attr_handle = NULL;
    FH_DETECTION_INFO landmark_out;
    fh_facealign_param_t param;
    fh_facealign_processor processor;
    FH_VPU_STREAM_ADV rgbStr;
    MEM_DESC nn_data_trans; // 512x288 -> 112x112
    FHT_RgbColor_t box_color = {255, 0, 0, 255};

    /* algo init */
    param.width = 112;
    param.height = 112;
    processor = FH_FACEALIGN_Create(&param);
    if (NULL == processor)
    {
        printf("FH_FACEALIGN_Create failed.\n");
        goto Exit;
    }

    // printf("FH_FACEALIGN Version: %d\n", FH_FACEALIGN_Version());

    /* nn init */
    landmarkHandle = sample_init_fh_nn(0, FN_DET_FACEDET_FULL);
    if (landmarkHandle == NULL)
    {
        printf("Error[FH_NN]: sample_init_fh_nn failed\n");
        goto Exit;
    }

    face_attr_handle = sample_init_fh_nn(1, FN_DET_FACEATTR);
    if (face_attr_handle == NULL)
    {
        printf("Error[FH_NN]: sample_init_fh_nn failed\n");
        goto Exit;
    }

    /* buffer init */
    ret = buffer_malloc_withname(&nn_data_trans, 112 * 112 * 4, 1024, "nn_data_trans");
    if (ret)
    {
        printf("buffer malloc failed\n");
        goto Exit;
    }
    memset(nn_data_trans.vbase, 0, nn_data_trans.size);
    MEM_DESC in1_data; // 512x288 -> 112x112
    ret = buffer_malloc_withname(&in1_data, 512 * 288 * 4, 1024, "test");
    memset(in1_data.vbase, 0, in1_data.size);

    /* params init */
    FH_UINT32 chn_w = 0;
    FH_UINT32 chn_h = 0;
    FH_VPU_CHN_CONFIG chn_info;
    ret = FH_VPSS_GetChnAttr(FH_APP_GRP_ID, DRAW_BOX_CHAN, &chn_info);
    if (ret)
    {
        printf("Error[FH_NN]: FH_VPSS_GetChnAttr failed, ret = %x\n", ret);
        goto Exit;
    }

    chn_w = chn_info.vpu_chn_size.u32Width;
    chn_h = chn_info.vpu_chn_size.u32Height;

    FH_IMAGE_t algo_src;
    FH_IMAGE_t algo_dst;
    fh_face_landmark_t face_landmark;

    algo_src.width = NN_RGB_W;
    algo_src.height = NN_RGB_H;
    algo_src.stride = NN_RGB_W * 4;
    algo_src.imageType = FH_IMAGE_FORMAT_RGBA;

    algo_dst.width = 112;
    algo_dst.height = 112;
    algo_dst.stride = algo_dst.width * 4;
    algo_dst.imageType = FH_IMAGE_FORMAT_RGBA;
    algo_dst.data = (FH_UINT8 *)nn_data_trans.vbase;

    FH_SINT32 dt_x = 0;
    FH_SINT32 dt_y = 0;
    FH_SINT32 dt_w = 0;
    FH_SINT32 dt_h = 0;

    FH_IMAGE_T face_attr_src;
    face_attr_src.width = 112;
    face_attr_src.height = 112;
    face_attr_src.stride = face_attr_src.width;
    face_attr_src.imageType = FH_IMAGE_FORMAT_RGB888;
    face_attr_src.src_data.vbase = nn_data_trans.vbase;
    face_attr_src.src_data.base = nn_data_trans.base;
    face_attr_src.src_data.size = nn_data_trans.size;
    int frame_cnt = 0;

    while (!g_stop_nna_detect)
    {
        frame_cnt++;
        ret = FH_VPSS_GetChnFrameAdv(FH_APP_GRP_ID, RGB_CHAN, &rgbStr, 1000);
        if (ret)
        {
            continue;
        }

        FH_IMAGE_T nn_src;
        nn_src.width = NN_RGB_W;
        nn_src.height = NN_RGB_H;
        nn_src.stride = NN_RGB_W;
        nn_src.imageType = FH_IMAGE_FORMAT_RGB888;
        nn_src.src_data = rgbStr.frm_rgb888.data;
        nn_src.timestamp = rgbStr.time_stamp;

        // save_pic_c(nn_src.src_data.vbase, nn_src.src_data.size, "512x288", frame_cnt);

        ret = FH_NNA_DET_POINT_Process(landmarkHandle, &nn_src, &landmark_out);
        if (ret)
        {
            printf("Error[FH_NN]: FH_NNA_DET_Process failed, ret = %x\n", ret);
            continue;
        }

        algo_src.timestamp = rgbStr.time_stamp;
        algo_src.data = (FH_UINT8 *)nn_src.src_data.vbase;
        face_attr_src.timestamp = rgbStr.time_stamp;

        for (int i = 0; i < 60; i++)
        {
            sample_set_gbox(0, 0, i, 0, 0, 0, 0, box_color);
        }

        // printf("box num = %d\n", landmark_out.face_detect_t.boxNum);

        if (landmark_out.face_detect_t.boxNum > 12)
        {
            landmark_out.face_detect_t.boxNum = 12;
        }

        for (int i = 0; i < landmark_out.face_detect_t.boxNum; i++)
        {
            char data[256];
            memset(data, 0, 256);
            memset(nn_data_trans.vbase, 0, nn_data_trans.size);

            for (int j = 0; j < 5; j++)
            {
                // sprintf(data, "landmark[%d].x = %f,landmark[%d].y = %f,landmark[%d].conf = %f\n", j, landmark_out.face_detect_t.detBBox[i].landmark[j].x, j, landmark_out.face_detect_t.detBBox[i].landmark[j].y, j, landmark_out.face_detect_t.detBBox[i].conf);
                face_landmark.landmark[j].x = landmark_out.face_detect_t.detBBox[i].landmark[j].x;
                face_landmark.landmark[j].y = landmark_out.face_detect_t.detBBox[i].landmark[j].y;
                dt_x = face_landmark.landmark[j].x * chn_w;
                dt_y = face_landmark.landmark[j].y * chn_h;
                dt_w = 8;
                dt_h = 8;
                sample_set_gbox(0, 1, i * 5 + j, dt_x, dt_y, dt_w, dt_h, box_color);
                // save_data(data, "landmark_info", frame_cnt);
            }

            ret = FH_FACEALIGN_Process(processor, &algo_src, &face_landmark, &algo_dst);
            if (ret)
            {
                printf("IMG_FACECROP_Process failed.\n");
                goto Exit;
            }

            FH_DETECTION_INFO face_attr_out;

            // save_pic_c(face_attr_src.src_data.vbase, face_attr_src.src_data.size, "112x112", frame_cnt);

            ret = FH_NNA_DET_POINT_Process(face_attr_handle, &face_attr_src, &face_attr_out);
            if (ret)
            {
                printf("Error[FH_NN]: FH_NNA_DET_Process failed, ret = %x\n", ret);
                continue;
            }

            int isCorrect = 1;
            if (face_attr_out.face_attr.gender.prob < 0.90 || face_attr_out.face_attr.glasses.prob < 0.90 || face_attr_out.face_attr.mask.prob < 0.90 || face_attr_out.face_attr.expr.prob < 0.90)
            {
                isCorrect = 0;
            }

            if (isCorrect)
            {
                printf("\n=========FACE ATTR===========\n");
                printf("Age = %f, gender = %d prob = %f\n", face_attr_out.face_attr.age, face_attr_out.face_attr.gender.attr, face_attr_out.face_attr.gender.prob);
                printf("glasses = %d prob = %f\n", face_attr_out.face_attr.glasses.attr, face_attr_out.face_attr.glasses.prob);
                printf("mask = %d prob = %f\n", face_attr_out.face_attr.mask.attr, face_attr_out.face_attr.mask.prob);
                printf("expr = %d prob = %f\n", face_attr_out.face_attr.expr.attr, face_attr_out.face_attr.expr.prob);

                printf("AGE = %f\n", face_attr_out.face_attr.age);
                if (face_attr_out.face_attr.gender.attr == FH_FEMALE)
                {
                    printf("Woman!\n");
                }
                else
                {
                    printf("Man!\n");
                }

                if (face_attr_out.face_attr.glasses.attr == FH_GLASSES)
                {
                    printf("GLASSES!\n");
                }
                else
                {
                    printf("NO GLASSED!\n");
                }
                if (face_attr_out.face_attr.mask.attr == FH_MASK)
                {
                    printf("MASK!\n");
                }
                else
                {
                    printf("NO MASK!\n");
                }
                switch (face_attr_out.face_attr.expr.attr)
                {
                case FH_ANGRY:
                    printf("ANGRY!\n");
                    break;
                case FH_DISGUST:
                    printf("FH_DISGUST!\n");
                    break;
                case FH_FEAR:
                    printf("FH_FEAR!\n");
                    break;
                case FH_HAPPY:
                    printf("FH_HAPPY!\n");
                    break;
                case FH_SAD:
                    printf("FH_SAD!\n");
                    break;
                case FH_SURPRISE:
                    printf("FH_SURPRISE!\n");
                    break;
                case FH_NEUTRAL:
                    printf("FH_NEUTRAL!\n");
                    break;
                }
                printf("=========FACE ATTR===========\n");
            }
        }
    }

Exit:
    g_nna_running = 0;
    FH_FACEALIGN_Destory(processor);
    sample_nna_deinit(landmarkHandle);
    sample_nna_deinit(face_attr_handle);
    FH_VPSS_CloseChn(FH_APP_GRP_ID, DRAW_BOX_CHAN);

    return NULL;
}

FH_SINT32 sample_fh_nn_face_attr_start(FH_VOID)
{
    pthread_t nn_thread;
    pthread_attr_t nn_attr;
    if (g_nna_running)
    {
        printf("[FH_NN] nna demo is running\n");
        return 0;
    }

    g_stop_nna_detect = 0;

    pthread_attr_init(&nn_attr);

    pthread_attr_setdetachstate(&nn_attr, PTHREAD_CREATE_DETACHED);
#ifdef __RTTHREAD_OS__
    pthread_attr_setstacksize(&nn_attr, 64 * 1024);
#endif
    if (pthread_create(&nn_thread, &nn_attr, thread_face_attr, NULL))
    {
        printf("Error[FH_NN]: pthread_create failed\n");
        goto Exit;
    }

    g_nna_running = 1;

    return 0;

Exit:
    return -1;
}

FH_SINT32 sample_fh_nn_face_attr_stop(FH_VOID)
{
    g_stop_nna_detect = 1;

    while (g_nna_running)
    {
        usleep(10);
    }

    return 0;
}
