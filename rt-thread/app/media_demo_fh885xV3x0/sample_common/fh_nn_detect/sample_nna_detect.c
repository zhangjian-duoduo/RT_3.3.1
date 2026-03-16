/* 适用芯片: FH885x与FH8862系列 */

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

#define RGB_CHAN (3)
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

// draw function
#if defined(FH_APP_OPEN_PVF_DETECT) || defined(FH_APP_OPEN_LPD_DETECT)
typedef struct
{
    FH_SINT32 box_x;
    FH_SINT32 box_y;
    FH_SINT32 box_w;
    FH_SINT32 box_h;

} FH_BOX;

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
#endif

#ifdef FH_APP_OPEN_LPD_DETECT
static FH_VOID draw_lpdBox(FH_DETECTION_INFO *out, FH_UINT32 box_chnw, FH_UINT32 box_chnh)
{
    FH_BOX box;
    static FH_DETECTION_INFO out_last = {0};
    FHT_RgbColor_t box_color = {255, 0, 0, 255};

    for (int i = 0; i < out_last.lpd_detect_t.boxNum; i++)
    {
        sample_set_gbox(0, 0, i, 0, 0, 0, 0, box_color);
    }

    for (int i = 0; i < out->lpd_detect_t.boxNum; i++)
    {
        box.box_x = out->lpd_detect_t.detBBox[i].bbox.x * box_chnw;
        box.box_y = out->lpd_detect_t.detBBox[i].bbox.y * box_chnh;
        box.box_w = out->lpd_detect_t.detBBox[i].bbox.w * box_chnw;
        box.box_h = out->lpd_detect_t.detBBox[i].bbox.h * box_chnh;

        if (!check_parmas(&box, box_chnw, box_chnh))
        {
            sample_set_gbox(DRAW_BOX_CHAN, 1, i , box.box_x, box.box_y, box.box_w, box.box_h, box_color);
        }
    }
    out_last.lpd_detect_t.boxNum = out->lpd_detect_t.boxNum;
}
#elif defined(FH_APP_OPEN_PVF_DETECT) 
static FH_VOID draw_pcfBox(FH_DETECTION_T *out, FH_UINT32 box_chnw, FH_UINT32 box_chnh)
{
    FH_BOX box;
    FH_SINT32 step = 0;
    FHT_RgbColor_t box_color = {0, 0, 0, 0};
    FHT_RgbColor_t box_color_p = {255, 0, 0, 255};
    FHT_RgbColor_t box_color_c = {0, 255, 0, 255};
    FHT_RgbColor_t box_color_f = {0, 0, 255, 255};

    for (int i = 0; i < 255; i++)
    {
        sample_set_gbox(0, 0, i, 0, 0, 0, 0, box_color_p);
    }

    for (int i = 0; i < out->boxNum; i++)
    {
        box.box_x = out->detBBox[i].bbox.x * box_chnw;
        box.box_y = out->detBBox[i].bbox.y * box_chnh;
        box.box_w = out->detBBox[i].bbox.w * box_chnw;
        box.box_h = out->detBBox[i].bbox.h * box_chnh;
        if (out->detBBox[i].clsType == 0)
        {
            box_color = box_color_p;
            step = 0;
        }
        else if (out->detBBox[i].clsType == 1)
        {
            box_color = box_color_c;
            step = 100;
        }
        else if (out->detBBox[i].clsType == 2)
        {
            box_color = box_color_f;
            step = 200;
        }

        if (!check_parmas(&box, box_chnw, box_chnh))
        {
            sample_set_gbox(DRAW_BOX_CHAN, 1, i + step, box.box_x, box.box_y, box.box_w, box.box_h, box_color);
        }
    }
}
#else
static FH_UINT32 compare_rect(FH_RECT_T rect1, FH_RECT_T rect2)
{
    FH_UINT32 dt_x = 0;
    FH_UINT32 dt_y = 0;
    FH_UINT32 dt_w = 0;
    FH_UINT32 dt_h = 0;
    FH_UINT32 thresh = 10; // 0~100% percent

    if (rect1.x > rect2.x)
    {
        dt_x = (FH_UINT32)(rect1.x - rect2.x);
    }
    else
    {
        dt_x = (FH_UINT32)(rect2.x - rect1.x);
    }

    if ((FH_UINT32)dt_x * 100 > (FH_UINT32)rect1.w * thresh)
    {
        return 1;
    }

    if (rect1.y > rect2.y)
    {
        dt_y = (FH_UINT32)(rect1.y - rect2.y);
    }
    else
    {
        dt_y = (FH_UINT32)(rect2.y - rect1.y);
    }

    if ((FH_UINT32)dt_y * 100 > (FH_UINT32)rect1.h * thresh)
    {
        return 1;
    }

    if (rect1.w > rect2.w)
    {
        dt_w = (FH_UINT32)(rect1.w - rect2.w);
    }
    else
    {
        dt_w = (FH_UINT32)(rect2.w - rect1.w);
    }

    if ((FH_UINT32)dt_w * 100 > (FH_UINT32)rect1.w * thresh)
    {
        return 1;
    }

    if (rect1.h > rect2.h)
    {
        dt_h = (FH_UINT32)(rect1.h - rect2.h);
    }
    else
    {
        dt_h = (FH_UINT32)(rect2.h - rect1.h);
    }

    if ((FH_UINT32)dt_h * 100 > (FH_UINT32)rect1.h * thresh)
    {
        return 1;
    }

    return 0; // needn't draw
}

static FH_VOID draw_box(FH_DETECTION_T *out, FH_UINT32 box_chnw, FH_UINT32 box_chnh)
{
    static FH_DETECTION_T out_last = {0};
    FH_SINT32 i;
    FH_SINT32 dt_x;
    FH_SINT32 dt_y;
    FH_SINT32 dt_w;
    FH_SINT32 dt_h;

    FHT_RgbColor_t box_color = {255, 0, 0, 255};

    for (i = 0; i < out->boxNum; i++)
    {
        /*draw gosd rect*/
        out->detBBox[i].bbox.x *= box_chnw;
        out->detBBox[i].bbox.y *= box_chnh;
        out->detBBox[i].bbox.w *= box_chnw;
        out->detBBox[i].bbox.h *= box_chnh;

        if (!compare_rect(out_last.detBBox[i].bbox, out->detBBox[i].bbox))
        {
            continue;
        }
        else
        {
            out_last.detBBox[i].bbox = out->detBBox[i].bbox;
        }

        dt_x = out->detBBox[i].bbox.x;
        dt_y = out->detBBox[i].bbox.y;
        dt_w = out->detBBox[i].bbox.w;
        dt_h = out->detBBox[i].bbox.h;

        sample_set_gbox(0, 1, i, dt_x, dt_y, dt_w, dt_h, box_color);
    }

    for (; i < out_last.boxNum; i++)
    {
        sample_set_gbox(0, 0, i, 0, 0, 0, 0, box_color);
    }

    out_last.boxNum = out->boxNum;
}
#endif //draw faction

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
#if defined(FH_APP_OPEN_NN_C2D)
    nna_param.type = FN_DET_TYPE_C2;
    strcpy(modelPath, "./pedestrian_vehicle/c2det.nbg");
#elif defined(FH_APP_OPEN_NN_FACE)
    nna_param.type = FN_DET_TYPE_FACE;
    strcpy(modelPath, "./face_detection/facedet.nbg");
#elif defined(FH_APP_OPEN_NN_DETECT)
    nna_param.type     = FN_DET_TYPE_PERSON;
    strcpy(modelPath, "./pedestrian/persondet.nbg");
#elif defined(FH_APP_OPEN_PVF_DETECT)
    nna_param.type = FN_DET_C3DET;
    strcpy(modelPath, "./pedestrian_vehicle_fire/c3det.nbg");
#elif defined(FH_APP_OPEN_LPD_DETECT)
    nna_param.type = FN_DET_NET_LPDET;
    strcpy(modelPath, "./LPR/lpdet.nbg");
#else
    printf("Invalid detect type %d\n", nna_param.type);
    return NULL;
#endif

    nna_param.conf_thr = 0.5;
    nna_param.rotate = FN_ROT_0;

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

#ifdef FH_APP_OPEN_PVF_DETECT
    FH_DET_SETPARAM_EXT_T setParams;
    setParams.cls_id = 1;
    setParams.conf_thr = 0.4;
    ret = FH_NNA_DET_SetParam_EXT(detHandle, &setParams);
    if (ret)
    {
        printf("FH_NNA_DET_SetParam_EXT failed with %x\n", ret);
    }
#endif

    free_nn_models(nna_param.nbg_data);

    printf("FH_NNA_DET_Init ok !!! %p\n", detHandle);

    return detHandle;
}

static FH_SINT32 sample_create_rgb_chan(FH_VOID)
{
    FH_SINT32 ret;
    FH_BIND_INFO src, dst;

    src.obj_id = FH_OBJ_VPU_VO;
    src.dev_id = FH_APP_GRP_ID;
    src.chn_id = RGB_CHAN;
    dst.obj_id = FH_OBJ_NN;
    dst.dev_id = 0;
    dst.chn_id = 0;
    ret = FH_SYS_Bind(src, dst);
    if (ret != RETURN_OK)
    {
        printf("Error(%d - %x): FH_SYS_BindVpu2nn!\n", ret, ret);
        return -1;
    }

    return ret;
}

/* 512 * 288 */
FH_VOID *thread_nn_detect(FH_VOID *args)
{
    FH_SINT32 ret;
    FH_VOID *detHandle = NULL;
#ifdef FH_APP_OPEN_LPD_DETECT
    FH_DETECTION_INFO nn_out;
#else
    FH_DETECTION_T nn_out;
#endif
    detHandle = sample_init_fh_nn();
    if (detHandle == NULL)
    {
        printf("Error[FH_NN]: sample_init_fh_nn failed\n");
        goto Exit;
    }

    ret = sample_create_rgb_chan();
    if (ret)
    {
        goto Exit;
    }

    FH_UINT32 box_chn_w;
    FH_UINT32 box_chn_h;
    FH_VPU_CHN_CONFIG box_chan_cfg;

    ret = FH_VPSS_GetChnAttr(FH_APP_GRP_ID, DRAW_BOX_CHAN, &box_chan_cfg);
    if (ret)
    {
        printf("Error[FH_NN]: FH_VPSS_GetChnAttr failed, ret = %x\n", ret);
        goto Exit;
    }

    box_chn_w = box_chan_cfg.vpu_chn_size.u32Width;
    box_chn_h = box_chan_cfg.vpu_chn_size.u32Height;

    while (!g_stop_nna_detect)
    {
#ifdef FH_APP_OPEN_LPD_DETECT
        ret = FH_NNA_DET_POINT_Forward(detHandle, &nn_out);
        if (ret)
        {
            printf("Error[FH_NN]: FH_NNA_DET_Process failed, ret = %x\n", ret);
            continue;
        }
#else
        ret = FH_NNA_DET_Forward(detHandle, &nn_out);
        if (ret)
        {
            printf("Error[FH_NN]: FH_NNA_DET_Process failed, ret = %x\n", ret);
            continue;
        }
#endif

#if defined(FH_APP_OPEN_PVF_DETECT)
        draw_pcfBox(&nn_out, box_chn_w, box_chn_h);
#elif defined(FH_APP_OPEN_LPD_DETECT)
        draw_lpdBox(&nn_out, box_chn_w, box_chn_h);
#else
        draw_box(&nn_out, box_chn_w, box_chn_h);
#endif
    }

Exit:
    g_nna_running = 0;
    sample_nna_deinit(detHandle);

    return NULL;
}

FH_SINT32 sample_fh_nn_obj_detect_start(FH_VOID)
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
    pthread_attr_setstacksize(&nn_attr, 256 * 1024);
#endif
    if (pthread_create(&nn_thread, &nn_attr, thread_nn_detect, NULL))
    {
        printf("Error[FH_NN]: pthread_create failed\n");
        goto Exit;
    }

    g_nna_running = 1;

    return 0;

Exit:
    return -1;
}

FH_SINT32 sample_fh_nn_obj_detect_stop(FH_VOID)
{
    g_stop_nna_detect = 1;

    while (g_nna_running)
    {
        usleep(10);
    }

    return 0;
}
