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
#include "fh_algo.h"
#include "sample_common.h"


#define RGB_CHAN (3) /* 只有通道3支持RGB输出格式 */
#define DRAW_BOX_CHAN (0)
#define NN_RGB_W (512)
#define NN_RGB_H (288)
#define GESTURE_RGB_W (64)
#define GESTURE_RGB_H (64)

typedef struct
{
    FH_SINT32 box_x;
    FH_SINT32 box_y;
    FH_SINT32 box_w;
    FH_SINT32 box_h;

} FH_BOX;

static FH_SINT32 g_nna_running = 0;
static FH_SINT32 g_nna_stop = 0;
// draw box
extern FH_SINT32 sample_set_gbox(FH_UINT32 chan, FH_UINT32 enable, FH_UINT32 box_id, FH_UINT32 box_x, FH_UINT32 box_y, FH_UINT32 w, FH_UINT32 h, FHT_RgbColor_t color);
// over size check
static FH_SINT32 check_parmas(FH_BOX *rect, int chn_width, int chn_height)
{
    FH_SINT32 ret = 0;

    if (rect->box_x < 0 || rect->box_y < 0 || rect->box_w < 0 || rect->box_h < 0)
    {
        ret = -1;
    }

    if ((rect->box_x + rect->box_w) > chn_width) // fix over size
    {
        rect->box_w = chn_width - rect->box_x - 1;
    }
    if ((rect->box_y + rect->box_h) > chn_height)
    {
        rect->box_h = chn_height - rect->box_y - 1;
    }

    return ret;
}

static void draw_gbox(FH_DETECTION_T *nn_out, FH_SINT32 chnw, FH_SINT32 chnh)
{
    FH_SINT32 boxnum = 0;
    FH_BOX box;
    FHT_RgbColor_t box_color = {255, 0, 0, 255};
    static FH_SINT32 box_id = 100;
    for (int i = 100; i < box_id; i++) // clear box
    {
        sample_set_gbox(DRAW_BOX_CHAN, 0, i, 0, 0, 0, 0, box_color);
    }
    box_id = 100;
    boxnum = nn_out->boxNum;
    for (int i = 0; i < boxnum; i++)
    {
        box.box_x = nn_out->detBBox[i].bbox.x * chnw;
        box.box_y = nn_out->detBBox[i].bbox.y * chnh;
        box.box_w = nn_out->detBBox[i].bbox.w * chnw;
        box.box_h = nn_out->detBBox[i].bbox.h * chnh;

        if (!check_parmas(&box, chnw, chnh))
        {
            sample_set_gbox(DRAW_BOX_CHAN, 1, box_id, box.box_x, box.box_y, box.box_w, box.box_h, box_color);
            box_id++;
        }
    }
}

// load model
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

// init model
static FH_VOID *_init_nn_models(FH_TYPE_E mode_type)
{
    FH_CHAR modelPath[256];
    FH_NN_INIT_PARAM_T nna_param;
    FH_VOID *detHandle = NULL;
    FH_UINT32 ret;
    FH_UINT32 chan;
    switch (mode_type)
    {
    case FN_DET_GESTUREDET:
    {
        strcpy(modelPath, "./gesture_det/gesturedet.nbg");
        chan = 1;
        nna_param.type = FN_DET_GESTUREDET;
        nna_param.src_w_in = NN_RGB_W;
        nna_param.src_h_in = NN_RGB_H;
        nna_param.src_c_in = 4;
        nna_param.conf_thr = 0.5;
        nna_param.rotate = FN_ROT_0;
        nna_param.nbg_data = load_nn_models(modelPath);
        break;
    }
    case FN_DET_GESTUREREC:
    {
        strcpy(modelPath, "./gesture_rec/gesturerec.nbg");
        chan = 2;
        nna_param.type = FN_DET_GESTUREREC;
        nna_param.src_w_in = GESTURE_RGB_W;
        nna_param.src_h_in = GESTURE_RGB_H;
        nna_param.src_c_in = 4;
        nna_param.conf_thr = 0.5;
        nna_param.rotate = FN_ROT_0;
        nna_param.nbg_data = load_nn_models(modelPath);
        break;
    }
    default:
    {
        printf("model fault\n");
    }
    }
    ret = FH_NNA_DET_Init(&detHandle, chan, &nna_param);
    if (ret)
    {
        printf("FH_NNA_DET_Init failed with %x\n", ret);
        detHandle = NULL;
    }
    return detHandle;
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
            return -1;
        }
    }

    return ret;
}
// process nn result
static FH_VOID _process_gesture_det(FH_UINT8 typeid, FH_UINT8 i)
{
    switch (typeid)
    {
    case 1:
    {
        printf("id = %d,FH_GESTURE_FIST\n", i);
        break;
    }
    case 2:
    {
        // printf("id = %d,FH_GESTURE_FOUR\n", i);
        break;
    }
    case 3:
    {
        printf("id = %d,FH_GESTURE_OK\n", i);
        break;
    }
    case 4:
    {
        // printf("id = %d,FH_GESTURE_PEACE\n", i);
        break;
    }
    case 5:
    {
        printf("id = %d,FH_GESTURE_STOP\n", i);
        break;
    }
    }
}


static FH_VOID *thread_gesture_detect(FH_VOID *args)
{
    FH_SINT32 ret;
    FH_DETECTION_T nn_out;
    FH_DETECTION_INFO gesture_out;
    FH_UINT32 box_chn_w;
    FH_UINT32 box_chn_h;
    FH_VPU_CHN_CONFIG box_chan_cfg;
    FH_VOID *gesturedet_handle = NULL;
    FH_VOID *gesturerec_handle = NULL;
    FH_VPU_STREAM_ADV aiChnStream;
    FH_IMAGE_T src;
    FH_IMAGE_T dst;
    MEM_DESC nn_data_trans; // 512x288 -> 64x64

    gesturedet_handle = _init_nn_models(FN_DET_GESTUREDET);
    if (gesturedet_handle == NULL)
    {
        printf("Error[FH_NN]: _init_nn_models FN_DET_GESTUREDET failed\n");
        goto Exit;
    }
    gesturerec_handle = _init_nn_models(FN_DET_GESTUREREC);
    if (gesturerec_handle == NULL)
    {
        printf("Error[FH_NN]: _init_nn_models FN_DET_GESTUREREC failed\n");
        goto Exit;
    }

    ret = buffer_malloc_withname(&nn_data_trans, GESTURE_RGB_W * GESTURE_RGB_H * 4, 1024, "nn_data_trans");
    if (ret)
    {
        printf("Error[FH_NN]: buffer malloc failed\n");
        goto Exit;
    }


    ret = FH_VPSS_GetChnAttr(FH_APP_GRP_ID, DRAW_BOX_CHAN, &box_chan_cfg);
    if (ret)
    {
        printf("Error[FH_NN]: FH_VPSS_GetChnAttr failed, ret = %x\n", ret);
        goto Exit;
    }
    box_chn_w = box_chan_cfg.vpu_chn_size.u32Width;
    box_chn_h = box_chan_cfg.vpu_chn_size.u32Height;

    while (!g_nna_stop)
    {
        ret = FH_VPSS_GetChnFrameAdv(FH_APP_GRP_ID, RGB_CHAN, &aiChnStream, 1000);
        if (ret != 0)
        {
            // printf("FH_VPSS_GetChnFrameAdv 0x%x(%d)\n", ret, ret);
            continue;
        }

        src.width = NN_RGB_W;
        src.height = NN_RGB_H;
        src.stride = NN_RGB_W;
        src.timestamp = aiChnStream.time_stamp;
        src.imageType = FH_IMAGE_FORMAT_RGB888;
        src.src_data = aiChnStream.frm_rgb888.data;
        ret = FH_NNA_DET_Process(gesturedet_handle, &src, &nn_out);
        if (ret != 0)
        {
            printf("FH_NNA_DET_Process 0x%x(%d)\n", ret, ret);
            continue;
        }
        draw_gbox(&nn_out, box_chn_w, box_chn_h);

        for (FH_UINT8 i = 0; i < nn_out.boxNum; i++)
        {
            resize_rgb_t *src_img = NULL;
            resize_rgb_t *dst_img = NULL;
            resize_norbox_t nor_maxbox = {0};
            resize_unnorbox_t unnor_maxbox = {0};

            src_img = (resize_rgb_t *)aiChnStream.frm_rgb888.data.vbase;
            dst_img = (resize_rgb_t *)nn_data_trans.vbase;

            nor_maxbox.x = nn_out.detBBox[i].bbox.x;
            nor_maxbox.y = nn_out.detBBox[i].bbox.y;
            nor_maxbox.w = nn_out.detBBox[i].bbox.w;
            nor_maxbox.h = nn_out.detBBox[i].bbox.h;

            unnormalize(&nor_maxbox, &unnor_maxbox, NN_RGB_W, NN_RGB_H);
            create_square_box_enlarge(&unnor_maxbox, &unnor_maxbox, NN_RGB_W, NN_RGB_H, 0);
            resizeBilinear(src_img, NN_RGB_W, NN_RGB_H, &unnor_maxbox, dst_img, GESTURE_RGB_W, GESTURE_RGB_H, 1);

            dst.width = GESTURE_RGB_W;
            dst.height = GESTURE_RGB_H;
            dst.stride = GESTURE_RGB_W;
            dst.timestamp = aiChnStream.time_stamp;
            dst.imageType = FH_IMAGE_FORMAT_RGB888;
            dst.src_data.base = nn_data_trans.base;
            dst.src_data.vbase = nn_data_trans.vbase;
            dst.src_data.size = nn_data_trans.size;

            FH_NNA_DET_POINT_Process(gesturerec_handle, &dst, &gesture_out);
            _process_gesture_det(gesture_out.guesture_t.type_id, i);
        }
    }

Exit:
    sample_nna_deinit(gesturedet_handle);
    g_nna_running = 0;
    return NULL;
}

FH_SINT32 sample_gesture_start(FH_VOID)
{
    pthread_t nn_thread;
    pthread_attr_t nn_attr;
    if (g_nna_running)
    {
        printf("[FH_NN] nna demo is running\n");
        return -1;
    }
    g_nna_stop = 0;
    pthread_attr_init(&nn_attr);
    pthread_attr_setdetachstate(&nn_attr, PTHREAD_CREATE_DETACHED);
#ifdef __RTTHREAD_OS__
    pthread_attr_setstacksize(&nn_attr, 64 * 1024);
#endif 
    if (pthread_create(&nn_thread, &nn_attr, thread_gesture_detect, NULL))
    {
        printf("Error[FH_NN]: pthread_create failed\n");
        return -1;
    }
    g_nna_running = 1;
    return 0;
}

FH_SINT32 sample_gesture_stop(FH_VOID)
{
    g_nna_stop = 1;
    while (g_nna_running)
    {
        usleep(10);
    }
    return -1;
}
