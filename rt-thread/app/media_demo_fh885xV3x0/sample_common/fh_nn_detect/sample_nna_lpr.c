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
#include "fh_nn_plate_rectify.h"
#include "sample_nna_lpr.h"
#include "fh_lpmerge.h"

extern unsigned char gb2312[267616];
extern unsigned char *asc16;

#define OSD_FONT_DISP_SIZE (32)

// #define DEBUG

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

static FH_SINT32 setText(FH_SINT32 channel, char *text_data, FH_UINT32 chn_w, FH_UINT32 chn_h)
{
    FH_SINT32 ret;
    FHT_OSD_CONFIG_t osd_cfg;
    FHT_OSD_Layer_Config_t pOsdLayerInfo[1];
    FHT_OSD_TextLine_t text_line_cfg[1];

    memset(&osd_cfg, 0, sizeof(osd_cfg));
    memset(&pOsdLayerInfo[0], 0, sizeof(FHT_OSD_Layer_Config_t));
    memset(&text_line_cfg[0], 0, sizeof(FHT_OSD_TextLine_t));

    /* 不旋转 */
    osd_cfg.osdRotate = 0;
    osd_cfg.pOsdLayerInfo = &pOsdLayerInfo[0];
    /* 设置text行块个数 */
    osd_cfg.nOsdLayerNum = 1; /*我们的demo中只演示了一个行块*/

    pOsdLayerInfo[0].layerStartX = 0;
    pOsdLayerInfo[0].layerStartY = 0;
    /* pOsdLayerInfo[0].layerMaxWidth = 640; */ /*根据需求配置，如果设置则以设置的最大值配置内存，如果缺省则以实际幅面大小申请内存*/
    /* pOsdLayerInfo[0].layerMaxHeight = 480; */
    /* 设置字符大小,像素单位 */
    pOsdLayerInfo[0].osdSize = OSD_FONT_DISP_SIZE;

    /* 设置字符颜色为白色 */
    pOsdLayerInfo[0].normalColor.fAlpha = 255;
    pOsdLayerInfo[0].normalColor.fRed = 255;
    pOsdLayerInfo[0].normalColor.fGreen = 255;
    pOsdLayerInfo[0].normalColor.fBlue = 255;

    /* 设置字符反色颜色为黑色 */
    pOsdLayerInfo[0].invertColor.fAlpha = 255;
    pOsdLayerInfo[0].invertColor.fRed = 0;
    pOsdLayerInfo[0].invertColor.fGreen = 0;
    pOsdLayerInfo[0].invertColor.fBlue = 0;

    /* 设置字符钩边颜色为黑色 */
    pOsdLayerInfo[0].edgeColor.fAlpha = 255;
    pOsdLayerInfo[0].edgeColor.fRed = 0;
    pOsdLayerInfo[0].edgeColor.fGreen = 0;
    pOsdLayerInfo[0].edgeColor.fBlue = 0;

    /* 不显示背景 */
    pOsdLayerInfo[0].bkgColor.fAlpha = 0;

    /* 钩边像素为1 */
    pOsdLayerInfo[0].edgePixel = 1;

    /* 反色控制 */
    pOsdLayerInfo[0].osdInvertEnable = FH_OSD_INVERT_DISABLE; /*disable反色功能*/
    // pOsdLayerInfo[0].osdInvertEnable  = FH_OSD_INVERT_BY_CHAR; /*以字符为单位进行反色控制*/
    /*osd_cfg.osdInvertEnable  = FH_OSD_INVERT_BY_LINE;*/ /*以行块为单位进行反色控制*/
    pOsdLayerInfo[0].osdInvertThreshold.high_level = 180;
    pOsdLayerInfo[0].osdInvertThreshold.low_level = 160;
    pOsdLayerInfo[0].layerFlag = FH_OSD_LAYER_USE_TWO_BUF;
    pOsdLayerInfo[0].layerId = 0;

    ret = FHAdv_Osd_Ex_SetText(FH_APP_GRP_ID, channel, &osd_cfg);
    if (ret != FH_SUCCESS)
    {
        if (channel == 0) /*TOSD可能位于分通道前,这样channel 1可能没有,因此就不打印出错*/
        {
            printf("FHAdv_Osd_Ex_SetText failed with %d - %x\n", ret, ret);
        }
        return ret;
    }

    text_line_cfg[0].textInfo = text_data;
    text_line_cfg[0].textEnable = 1;                                     /* 使能自定义text */
    text_line_cfg[0].timeOsdEnable = 0;                                  /* 去使能时间显示 */
    text_line_cfg[0].textLineWidth = (OSD_FONT_DISP_SIZE / 2) * 36;      /* 每行最多显示36个像素宽度为16的字符 */
    text_line_cfg[0].linePositionX = chn_w / 2 - OSD_FONT_DISP_SIZE * 3; /* 左上角起始点宽度像素位置 */
    text_line_cfg[0].linePositionY = OSD_FONT_DISP_SIZE;                 /* 左上角起始点高度像素位置 */

    text_line_cfg[0].lineId = 0;
    text_line_cfg[0].enable = 1;
    ret = FHAdv_Osd_SetTextLine(FH_APP_GRP_ID, channel, pOsdLayerInfo[0].layerId, &text_line_cfg[0]);
    if (ret != FH_SUCCESS)
    {
        if (channel == 0) /*TOSD可能位于分通道前,这样channel 1可能没有,因此就不打印出错*/
        {
            printf("FHAdv_Osd_Ex_SetTextLine failed with %d - %x\n", ret, ret);
        }
        return ret;
    }

    return 0;
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

static FH_VOID *sample_init_fh_nn(FH_UINT32 chan, FH_UINT32 type)
{
    FH_UINT32 ret;
    FH_VOID *detHandle = NULL;
    FH_CHAR modelPath[256];
    FH_NN_INIT_PARAM_T nna_param;

    nna_param.src_c_in = 4;
    nna_param.type = type;
    nna_param.rotate = FN_ROT_0;
    if (type == FN_DET_NET_LPDET)
    {
        nna_param.src_w_in = NN_RGB_W;
        nna_param.src_h_in = NN_RGB_H;
        nna_param.conf_thr = 0.78;
        strcpy(modelPath, "./LPR/lpdet.nbg");
    }
    else if (type == FN_DET_LPREC)
    {
        nna_param.src_w_in = 160;
        nna_param.src_h_in = 32;
        nna_param.conf_thr = 0.9;
        strcpy(modelPath, "./lp_recognition/lprec.nbg");
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

/* 加载字库 */
static FH_SINT32 load_font_lib(FHT_OSD_FontType_e type, FH_UINT8 *font_array, FH_SINT32 font_array_size)
{
    FH_SINT32 ret;
    FHT_OSD_FontLib_t font_lib;

    font_lib.pLibData = font_array;
    font_lib.libSize = font_array_size;
    ret = FHAdv_Osd_LoadFontLib(type, &font_lib);
    if (ret != 0)
    {
        printf("Error: FHAdv_Osd_LoadFontLib failed, ret=%d\n", ret);
        return ret;
    }

    return 0;
}

static FH_SINT32 sample_init_font(FH_VOID)
{
    FH_SINT32 ret;

    sleep(1);

    /* 加载gb2312字库 */
    ret = load_font_lib(FHEN_FONT_TYPE_CHINESE, gb2312_lpr, sizeof(gb2312_lpr));
    if (ret != 0)
    {
        printf("Load CHINESE font lib failed with %d\n", ret);
    }

    /* 加载asc字库 */
    ret = load_font_lib(FHEN_FONT_TYPE_ASC, asc16_lpr, sizeof(asc16_lpr));
    if (ret != 0)
    {
        printf("Load ASC font lib failed with %d\n", ret);
    }

    return ret;
}

FH_VOID draw_box(FH_UINT32 chan, FH_UINT32 enable, FH_UINT32 box_id, FH_UINT32 x, FH_UINT32 y, FH_UINT32 w, FH_UINT32 h, FHT_RgbColor_t color, FH_UINT32 chn_w, FH_UINT32 chn_h)
{
    if (x < 0 || y < 0 || w < 0 || h < 0)
    {
        return;
    }

    if ((x + w) > chn_w) // fix over size
    {
        w = chn_w - x - 1;
    }
    if ((y + h) > chn_h)
    {
        h = chn_h - y - 1;
    }

    sample_set_gbox(chan, enable, box_id, x, y, w, h, color);
}

#ifdef DEBUG
static int nna_read_file(const char *file_path, void *addr)
{
    FILE *file;
    int length, act;
    file = fopen(file_path, "rb");
    if (!file)
    {
        printf("%s is null\n", file_path);
        exit(0);
    }
    fseek(file, 0L, SEEK_END);
    length = ftell(file);
    fseek(file, 0L, SEEK_SET);
    act = fread(addr, 1, length, file);
    if (act != length)
    {
        printf("read %s err. %d %d\n", file_path, act, length);
        return -1;
    }
    fclose(file);
    return RETURN_OK;
}

// static FH_SINT32 save_pic_c(FH_IMAGE_t src, char *name)
// {
//     static int num = 0;
//     char file_name[64];
//     int fd = 0;
//     snprintf(file_name, 64, "./pic_raw/%s_%d.raw", name, num);
//     printf("save_pic_start[%s-%d]\n", name, num);
//     printf("ADDR = %p,SIZE = %X\n", src.data, src.width * src.height * 4);
//     fd = open(file_name, O_WRONLY | O_CREAT);
//     write(fd, src.data, src.width * src.height * 4);
//     close(fd);
//     printf("save_pic_end[%s-%d]\n", name, num);

//     return num++;
// }

static FH_SINT32 save_pic_c(FH_IMAGE_T src, char *name)
{
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
}

static FH_VOID save_data(FH_UINT8 *data, FH_FLOAT *f_data, int data_len, char *name, int num)
{
    printf("save_data_start[%s-%d]\n", name, num);
    FILE *fp;
    char file_name[64];
    snprintf(file_name, 64, "./pic_raw/%s_%d.log", name, num);
    fp = fopen(file_name, "w");
    for (int i = 0; i < data_len; i++)
    {
        fprintf(fp, "index = %d, val_dec data = %d, conf = %f\n", i, data[i], f_data[i]);
    }
    fclose(fp);
    printf("save_data_end[%s-%d]\n", name, num);
}
#endif

/* 512 * 288 */
FH_VOID *thread_lpr(FH_VOID *args)
{
    FH_SINT32 ret;
    FH_VOID *detHandle = NULL;
    FH_VOID *lprHandle = NULL;
    FH_DETECTION_INFO det_out;
    FH_DETECTION_INFO lpr_out;
    FH_VPU_STREAM_ADV rgbStr;
    MEM_DESC nn_data_trans; // 512x288 -> 160x32
    fh_lprectify_processor processor = NULL;
    FHA_SEGMENTATION_PROCESSOR processor_seg = NULL;
    fh_lpd_landmark_t input_pts;
    FHT_RgbColor_t box_color = {255, 0, 0, 255};
    FH_CHAR licensePlate[128];
    FH_ADDR data_164x48 = malloc(164 * 48 * 4);
    // BLOCK_SIZE - 是自适应阈值取的窗口范围
    int BLOCK_SIZE = 11;
    // THRESH_PERCENT - 是自适应阈值的百分比设定，决定了前后景分割阈值
    float THRESH_PERCENT = 0.05;

    ret = sample_init_font();
    if (ret)
    {
        printf("sample_init_font failed\n");
        goto Exit;
    }

    /* algo init */
    processor = FH_PLATE_RECTIFY_Create();

    if (NULL == processor)
    {
        printf("FH_PLATE_RECTIFY_Create failed.\n");
        goto Exit;
    }

    processor_seg = FHA_SEGMENTATION_Create();
    if (NULL == processor_seg)
    {
        printf("FHA_SEGMENTATION_Create failed.\n");
        goto Exit;
    }

    /* nn init */
    detHandle = sample_init_fh_nn(0, FN_DET_NET_LPDET);
    if (detHandle == NULL)
    {
        printf("Error[FH_NN]: sample_init_fh_nn failed\n");
        goto Exit;
    }

    lprHandle = sample_init_fh_nn(1, FN_DET_LPREC);
    if (lprHandle == NULL)
    {
        printf("Error[FH_NN]: sample_init_fh_nn failed\n");
        goto Exit;
    }

    /* buffer init */
    ret = buffer_malloc_withname(&nn_data_trans, 160 * 32 * 4, 1024, "nn_data_trans");
    if (ret)
    {
        printf("buffer malloc failed\n");
        goto Exit;
    }

    memset(nn_data_trans.vbase, 0, nn_data_trans.size);

    FH_UINT32 chn_w = 0;
    FH_UINT32 chn_h = 0;
    FH_VPU_CHN_CONFIG box_chan_cfg;
    ret = FH_VPSS_GetChnAttr(FH_APP_GRP_ID, DRAW_BOX_CHAN, &box_chan_cfg);
    if (ret)
    {
        printf("Error[FH_NN]: FH_VPSS_GetChnAttr failed, ret = %x\n", ret);
        goto Exit;
    }

    chn_w = box_chan_cfg.vpu_chn_size.u32Width;
    chn_h = box_chan_cfg.vpu_chn_size.u32Height;

    /* params init */
    FH_IMAGE_t algo_src; // 矫正
    FH_IMAGE_t algo_dst;

    algo_src.width = NN_RGB_W;
    algo_src.height = NN_RGB_H;
    algo_src.stride = NN_RGB_W * 4;
    algo_src.imageType = FH_IMAGE_FORMAT_RGBA;

    algo_dst.width = 164;
    // algo_dst.height = 32;
    algo_dst.height = 48;
    algo_dst.stride = algo_dst.width * 4;
    algo_dst.imageType = FH_IMAGE_FORMAT_RGBA;
    // algo_dst.data = (FH_UINT8 *)nn_data_trans.vbase;
    algo_dst.data = data_164x48;

    FH_IMAGE_t lpm_src; // 拼接
    FH_IMAGE_t lpm_dst;

    lpm_src.width = 164;
    lpm_src.height = 48;
    lpm_src.stride = lpm_src.width * 4;
    lpm_src.imageType = FH_IMAGE_FORMAT_RGBA;
    lpm_src.data = data_164x48;

    lpm_dst.width = 160;
    lpm_dst.height = 32;
    lpm_dst.stride = lpm_dst.width * 4;
    lpm_dst.imageType = FH_IMAGE_FORMAT_RGBA;
    lpm_dst.data = (FH_UINT8 *)nn_data_trans.vbase;

    FH_SINT32 dt_x = 0;
    FH_SINT32 dt_y = 0;
    FH_SINT32 dt_w = 0;
    FH_SINT32 dt_h = 0;

    FH_IMAGE_T lpr_src;
    lpr_src.width = 160;
    lpr_src.height = 32;
    lpr_src.stride = 160;
    lpr_src.imageType = FH_IMAGE_FORMAT_RGB888;
    lpr_src.src_data.vbase = nn_data_trans.vbase;
    lpr_src.src_data.base = nn_data_trans.base;
    lpr_src.src_data.size = nn_data_trans.size;

    while (!g_stop_nna_detect)
    {
        ret = FH_VPSS_GetChnFrameAdv(FH_APP_GRP_ID, RGB_CHAN, &rgbStr, 10000);
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

        ret = FH_NNA_DET_POINT_Process(detHandle, &nn_src, &det_out);
        if (ret)
        {
            printf("Error[FH_NN]: FH_NNA_DET_Process failed, ret = %x\n", ret);
            continue;
        }

        algo_src.timestamp = rgbStr.time_stamp;
        algo_src.data = (FH_UINT8 *)rgbStr.frm_rgb888.data.vbase;
        lpr_src.timestamp = rgbStr.time_stamp;

        for (int i = 0; i < 25; i++)
        {
            sample_set_gbox(0, 0, i, 0, 0, 0, 0, box_color);
        }

        for (int i = 0; i < det_out.lpd_detect_t.boxNum; i++)
        {
            memset(nn_data_trans.vbase, 0, nn_data_trans.size);

            for (int j = 0; j < FH_MAX_LPD_NUM; j++)
            {
                dt_x = det_out.lpd_detect_t.detBBox[i].landmark[j].x * chn_w;
                dt_y = det_out.lpd_detect_t.detBBox[i].landmark[j].y * chn_h;
                input_pts.point[j].x = det_out.lpd_detect_t.detBBox[i].landmark[j].x * NN_RGB_W;
                input_pts.point[j].y = det_out.lpd_detect_t.detBBox[i].landmark[j].y * NN_RGB_H;
                draw_box(0, 1, i, dt_x, dt_y, 4, 4, box_color, chn_w, chn_h);
            }

            ret = FH_PLATE_RECTIFY_Process(processor, &algo_src, &input_pts, &algo_dst);
            if (ret)
            {
                printf("FH_PLATE_RECTIFY_Process failed.\n");
                continue;
            }

            dt_x = det_out.lpd_detect_t.detBBox[i].bbox.x * chn_w;
            dt_y = det_out.lpd_detect_t.detBBox[i].bbox.y * chn_h;
            dt_w = det_out.lpd_detect_t.detBBox[i].bbox.w * chn_w;
            dt_h = det_out.lpd_detect_t.detBBox[i].bbox.h * chn_h;

            draw_box(0, 1, (i + 1) * (FH_MAX_LPD_NUM + 1) - 1, dt_x, dt_y, dt_w, dt_h, box_color, chn_w, chn_h);

            // nna_read_file("./in_test_164x48.raw", lpm_src.data);
            ret = FHA_SEGMENTATION_Process(processor_seg, &lpm_src, &lpm_dst, BLOCK_SIZE, THRESH_PERCENT);
            if (ret)
            {
                printf("FHA_SEGMENTATION_Process failed.\n");
                continue;
            }

            // save_pic_c(lpm_src, "in_164x48");
            // save_pic_c(lpm_dst, "out_160x32");

            ret = FH_NNA_DET_POINT_Process(lprHandle, &lpr_src, &lpr_out);
            if (ret)
            {
                printf("Error[FH_NN]: FH_NNA_DET_Process failed, ret = %x\n", ret);
                continue;
            }

            static FH_SINT32 correct = 1;

            memset(licensePlate, 0, sizeof(licensePlate));
            correct = 1;
            if (lpr_out.lpr_t.plate_len < 3)
            {
                correct = 0;
            }
            else
            {
                for (int i = 0; i < lpr_out.lpr_t.plate_len; i++)
                {
                    int index = lpr_out.lpr_t.val_dec[i];
                    strncat(licensePlate, lpr_char[index], 2);
                    if (lpr_out.lpr_t.prob_dec[i] < 0.97)
                    {
                        correct = 0;
                        break;
                    }
                }
            }

            if (correct)
            {
#ifdef DEBUG
                printf("\n============is Correct=====================\n");
                for (int i = 0; i < lpr_out.lpr_t.plate_len; i++)
                {
                    printf("i = %d,lpr_out.lpr_t.val_dec[i] = %d conf = %f\n", i, lpr_out.lpr_t.val_dec[i], lpr_out.lpr_t.prob_dec[i]);
                }
                int num = save_pic_c(lpr_src, "160x32");
                save_data(lpr_out.lpr_t.val_dec, lpr_out.lpr_t.prob_dec, lpr_out.lpr_t.plate_len, "conf", num);
                printf("===========================================\n");
                sprintf(licensePlate, "%s-%d ", licensePlate, num);
#else
                sprintf(licensePlate, "%s", licensePlate);
                setText(0, licensePlate, chn_w, chn_h); // osd
#endif
            }
        }
    }

Exit:
    if (data_164x48)
        free(data_164x48);
    g_nna_running = 0;
    FHA_SEGMENTATION_Destory(processor_seg);
    FH_PLATE_RECTIFY_Destory(processor);
    sample_nna_deinit(detHandle);
    sample_nna_deinit(lprHandle);
    FH_VPSS_CloseChn(FH_APP_GRP_ID, DRAW_BOX_CHAN);

    return NULL;
}

FH_SINT32 sample_fh_nn_lpr_detect_start(FH_VOID)
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
    pthread_attr_setstacksize(&nn_attr, 32 * 1024);
#endif
    if (pthread_create(&nn_thread, &nn_attr, thread_lpr, NULL))
    {
        printf("Error[FH_NN]: pthread_create failed\n");
        goto Exit;
    }

    g_nna_running = 1;

    return 0;

Exit:
    return -1;
}

FH_SINT32 sample_fh_nn_lpr_detect_stop(FH_VOID)
{
    g_stop_nna_detect = 1;

    while (g_nna_running)
    {
        usleep(10);
    }

    return 0;
}
