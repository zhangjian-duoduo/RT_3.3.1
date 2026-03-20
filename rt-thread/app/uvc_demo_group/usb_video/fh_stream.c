#include <unistd.h>
#include "types/type_def.h"
#include "sample_common_isp.h"
#include "sample_common_dsp.h"
#include "FHAdv_Isp_mpi_v3.h"
#include <stdio.h>
#include <pthread.h>
#include "usb_video.h"
#include "uvc_init.h"
#include "format.h"
#include "isp_format.h"
#include "rtconfig_app.h"
#include <bind/include/sample_bind.h>
#include "../dsp/chn_rc_type.h"
#include "../dsp/chn_yuv_type.h"
#include "overlay/sample_overlay.h"
#include "uvc_crosshair.h"
#include "uvc_sei_timestamp.h"
#if defined (NN_ENABLE_G0)
#include "sample_nna_detect.h"
#endif
#define TMP_DBG(fmt, x...)

void change_sensor_fps(int fps);

#define STREAM_TYPE_UNKNOW 0X888
FH_UINT32 v4l2_type_to_stream_type(FH_UINT32 vtype)
{
    FH_UINT32 stype = STREAM_TYPE_UNKNOW;

    switch (vtype)
    {
    case V4L2_PIX_FMT_MJPEG:
            stype = FH_STREAM_MJPEG;
            break;

    case V4L2_PIX_FMT_H264:
            stype = FH_STREAM_H264;
            break;

    case V4L2_PIX_FMT_H265:
            stype = FH_STREAM_H265;
            break;

    case V4L2_PIX_FMT_NV12:
            stype = FH_STREAM_NV12;
            break;
    case V4L2_PIX_FMT_YUY2:
            stype = FH_STREAM_YUY2;
            break;
    case V4L2_PIX_FMT_IR:
            stype = FH_STREAM_IR;
            break;
    default:
            printf("unknow type =%x, set to mjpeg.\n", vtype);
            stype = V4L2_PIX_FMT_MJPEG;
            break;

    }

    return stype;
}

FH_UINT32 change_ratio_by_res(int chan, int s32Width, int s32Height)
{
    FH_VPU_CROP stVpucropinfo;
    int VI_ratio;
    int cur_ratio;
    int ret;

    VI_ratio = VI_INPUT_WIDTH * 10 / VI_INPUT_HEIGHT;
    cur_ratio = s32Width * 10 / s32Height;

    if (VI_ratio >= 17 && cur_ratio < VI_ratio) /* vi 16:9 & cur 4:3 */
    {
        stVpucropinfo.vpu_crop_area.u32Width =  (VI_INPUT_WIDTH * s32Height / s32Width) & 0xfffffffc;
        stVpucropinfo.vpu_crop_area.u32Height = VI_INPUT_HEIGHT;
        stVpucropinfo.vpu_crop_area.u32X = (VI_INPUT_WIDTH - stVpucropinfo.vpu_crop_area.u32Width)/2;
        stVpucropinfo.vpu_crop_area.u32Y = 0;
    } else if (VI_ratio < 14 && cur_ratio > VI_ratio)
    {
        stVpucropinfo.vpu_crop_area.u32Width =  VI_INPUT_WIDTH;
        stVpucropinfo.vpu_crop_area.u32Height = (VI_INPUT_WIDTH * s32Height / s32Width) & 0xfffffffc;
        stVpucropinfo.vpu_crop_area.u32X = 0;
        stVpucropinfo.vpu_crop_area.u32Y = (VI_INPUT_WIDTH - stVpucropinfo.vpu_crop_area.u32Height)/2;
    } else
    {
        stVpucropinfo.vpu_crop_area.u32Width = VI_INPUT_WIDTH;
        stVpucropinfo.vpu_crop_area.u32Height = VI_INPUT_HEIGHT;
        stVpucropinfo.vpu_crop_area.u32X = 0;
        stVpucropinfo.vpu_crop_area.u32Y = 0;
    }
    if (stVpucropinfo.vpu_crop_area.u32X || stVpucropinfo.vpu_crop_area.u32Y)
        stVpucropinfo.crop_en = 1;
    ret = FH_VPSS_SetChnCrop(FH_GRP_ID, chan, VPU_CROP_SCALER, &stVpucropinfo);
    if (ret)
        printf("FH_VPSS_SetChnCrop failed! %d\n", ret);

    return 0;
}

static void ClearChnData(FH_UINT32 request_type, int giveupNum)
{
    FH_VENC_STREAM stream;
    int max_time = 1000, waitcnt = 0;
    int getNum = 0;
    int i;
    int  s32Ret;

    if (request_type == STREAM_TYPE_UNKNOW)
        return;

    for (i = 0; i < max_time; i++)
    {
        s32Ret = FH_VENC_GetStream(request_type, &stream);
        if (s32Ret == RETURN_OK)
        {
            FH_VENC_ReleaseStream(&stream);
            getNum++;
            waitcnt = 0;
        }
        else
        {
            if (waitcnt++ >= 2 && (getNum >= giveupNum))
                break;
            usleep(1000*5);
        }
    }
}

void fh_stream_exit(void)
{
#if defined (NN_ENABLE_G0)
    sample_fh_nn_obj_detect_stop();
    printf("=====sample_fh_nn_obj_detect_stop=====\n");
#endif
#ifdef FH_APP_OPEN_OVERLAY
    sample_overlay_stop();
    printf("=====sample_overlay_stop=====\n");
#endif
    sample_common_isp_stop();
    sample_common_media_sys_exit();
}


FH_UINT32 change_scaler_by_res(int chn_id, int s32Width, int s32Height)
{
    uint8_t coeff_val;

    if (s32Width > 1920)
    {
        coeff_val = 14; /* 0 */
    }
    else if (s32Width > 1024)
    {
        coeff_val = 14; /* 0 */
    }
    else if (s32Width > 800)
    {
        coeff_val = 6; /* 3 */
    }
    else if (s32Width > 640)
    {
        coeff_val = 6; /* 4 */
    }
    else if (s32Width > 320)
    {
        coeff_val = 6; /* 6 */
    }
    else
    {
        coeff_val = 6; /* 6 */
    }

    FH_VPSS_SetScalerCoeff(FH_GRP_ID, chn_id, coeff_val);

    return 0;
}

static int change_video(int id, int s32Format, int s32Width, int s32Height)
{
    struct enc_channel_info enc_info;
    struct vpu_channel_info chn_info;
    FH_BIND_INFO src;
    FH_UINT32 stream_type = v4l2_type_to_stream_type(s32Format);
    int i, ret;

    i = id;

    while (!g_uvc_dev[STREAM_ID1].isp_complete)
        usleep(100);

    sample_common_dsp_get_enc_chn_info(FH_GRP_ID, i, &enc_info);
    if (!enc_info.enable)
    {
        printf("Error: DSP channel %d is not enable!\n", i);
        goto test_exit;
    }

    enc_info.enc_type = stream_type;
    enc_info.width = s32Width;
    enc_info.height = s32Height;
    enc_info.channel = i;

    switch (stream_type)
    {
    case FH_STREAM_H264:
        enc_info.enc_type = FH_NORMAL_H264;
        enc_info.rc_type = H264_RC_TYPE;
        break;

    case FH_STREAM_H265:
        enc_info.enc_type = FH_NORMAL_H265;
        enc_info.rc_type = H265_RC_TYPE;
        break;

    case FH_STREAM_MJPEG:
        enc_info.enc_type = FH_MJPEG;
        enc_info.rc_type = MJPEG_RC_TYPE;
        break;
    }

    if (s32Format != V4L2_PIX_FMT_MJPEG)
    {
        ret = FH_VENC_StopRecvPic(i);
    }
    else
    {
        ret = FH_VENC_StopRecvPic((JPEG_ENC_CHN - i) - (FH_GRP_ID * MAX_VPU_CHN_NUM));
    }

    src.obj_id = FH_OBJ_VPU_VO;
    src.dev_id = FH_GRP_ID;
    src.chn_id = i;
    FH_SYS_UnBindbySrc(src);

    ret = FH_VPSS_CloseChn(FH_GRP_ID, i);
    if (ret != RETURN_OK)
    {
        printf("Error: FH_VPSS_CloseChn failed with %d\n", ret);
        goto test_exit;
    }

    ret = FH_VPSS_DestroyChn(FH_GRP_ID, i);
    if (ret != 0)
    {
        printf("Error(%d - %x): FH_VPSS_DestroyChn\n", ret, ret);
        goto test_exit;
    }

    ClearChnData(stream_type, 0);
	
#if defined RT_USING_HS_CUSTOM_8852V201_GC2083_DZ_20230619
		//API_ISP_SetMirrorAndflip(0, 1, 0);
#endif
    sample_common_dsp_get_vpu_chn_info(FH_GRP_ID, i, &chn_info);
    if (s32Format == V4L2_PIX_FMT_NV12 || s32Format == V4L2_PIX_FMT_IR)
    {
        chn_info.yuv_type = VPU_VOMODE_SCAN;
    }
    else if (s32Format == V4L2_PIX_FMT_YUY2)
    {
        chn_info.yuv_type = VPU_VOMODE_YUYV;
#if defined(CONFIG_ARCH_FH8636_FH8852V20X) || defined(CONFIG_CHIP_FH8626V200)
        chn_info.yuv_type = VPU_VOMODE_SCAN;
#endif
    }

    ret = sample_common_vpu_create_chan(FH_GRP_ID, i, s32Width, s32Height, chn_info.yuv_type);
    if (ret != RETURN_OK)
    {
        printf("Error(%d - %x): sample_common_vpu_create_chan!\n", ret, ret);
        goto test_exit;
    }

    if (s32Format == V4L2_PIX_FMT_NV12 || s32Format == V4L2_PIX_FMT_YUY2 || s32Format == V4L2_PIX_FMT_IR)
    {
        //change_ratio_by_res(i, s32Width, s32Height);
        change_scaler_by_res(i, s32Width, s32Height);
		
#if defined RT_USING_HS_CUSTOM_8852V201_GC2083_DZ_20230619
	//API_ISP_SetMirrorAndflip(0, 1, 0);
#endif
#ifdef FH_APP_OPEN_OVERLAY
    /******************************************
      step  14: add overlayer
     ******************************************/
    sample_overlay_start();
#if defined (FH_APP_OVERLAY_CROSSHAIR)
    uvc_crosshair_update(s32Width, s32Height);
#endif
#endif
        return 0;
    }

    if (s32Format == V4L2_PIX_FMT_H265 || s32Format == V4L2_PIX_FMT_H264)
    {
        ret = FH_VPSS_SetVOMode(FH_GRP_ID, i, i ? CH1_YUV_TYPE : CH0_YUV_TYPE);
        if (ret != RETURN_OK)
        {
            printf("Error: FH_VPSS_SetVOMode failed with %d - %x\n", ret, ret);
            goto test_exit;
        }
    }
    //change_ratio_by_res(i, s32Width, s32Height);

    ret = sample_common_enc_set_chan(FH_GRP_ID, i, &enc_info);
    if (ret != RETURN_OK)
    {
        printf("Error(%d - %x): sample_common_enc_set_chan!\n", ret, ret);
        goto test_exit;
    }

    /******************************************/
    /* step  7: start venc channel            */
    /******************************************/
    if (s32Format != V4L2_PIX_FMT_MJPEG)
        ret = sample_common_vpu_bind_to_enc(FH_GRP_ID, i, i);
    else
        ret = sample_common_vpu_bind_to_jpeg(FH_GRP_ID, i, i);
    if (ret != RETURN_OK)
    {
        printf("Error(%d - %x): sample_common_vpu_bind_to_enc!\n", ret, ret);
        goto test_exit;
    }

#ifdef FH_APP_OPEN_OVERLAY
    /******************************************
      step  14: add overlayer
     ******************************************/
    sample_overlay_start();
#if defined (FH_APP_OVERLAY_CROSSHAIR)
    uvc_crosshair_update(s32Width, s32Height);
#endif
#endif
    return RETURN_OK;

test_exit:
    printf("FH_SYS_Error!\n");
    return -1;

}

static int fh_stream_init(int s32Format, int s32Width, int s32Height)
{
    FH_SINT32 ret;

    /********************************************/
    /* step  1: init media platform             */
    /********************************************/
    ret = sample_common_media_sys_init();
    if (ret != RETURN_OK)
    {
        printf("[%s-%d]Error(%d - %x): sample_common_media_sys_init!\n", __func__, __LINE__, ret, ret);
        goto err_exit;
    }

    /******************************************
      step  2: init vpu vi and open vpu
     ******************************************/
    ret = sample_common_start_vpu();
    if (ret != RETURN_OK)
    {
        printf("Error(%d - %x): sample_common_start_vpu!\n", ret, ret);
        goto err_exit;
    }

    /******************************************
      step  2: init vpu vi and open vpu
     ******************************************/
    ret = sample_common_start_enc();
    if (ret != RETURN_OK)
    {
        printf("Error(%d - %x): sample_common_start_enc!\n", ret, ret);
        goto err_exit;
    }
    ret = sample_common_start_mjpeg();
    if (ret != RETURN_OK)
    {
        printf("Error(%d - %x): sample_common_start_mjpeg!\n", ret, ret);
        goto err_exit;
    }
    /************************************************
        step  10: start isp
    ************************************************/
    ret = sample_common_start_isp();
    if (ret)
    {
        goto err_exit;
    }
	
#if defined RT_USING_HS_CUSTOM_8852V201_GC2083_DZ_20230619
		//API_ISP_SetMirrorAndflip(0, 1, 0);	
#endif
    /************************************************
        step  10: start bind
    ************************************************/
    ret = sample_common_start_bind();
    if (ret)
    {
        goto err_exit;
    }
    /******************************************
      step  12: send stream
     ******************************************/
    sample_common_isp_run();
#ifdef FH_APP_OPEN_OVERLAY
    /******************************************
      step  14: add overlayer
     ******************************************/
    sample_overlay_start();
#if defined (FH_APP_OVERLAY_CROSSHAIR)
    uvc_crosshair_init();
    uvc_crosshair_update(s32Width, s32Height);
#endif
#endif
#if defined (FH_APP_SEI_TIMESTAMP)
    uvc_sei_timestamp_init();
#endif
#if defined (NN_ENABLE_G0)
    sample_fh_nn_obj_detect_start();
#endif

    printf("===========[successful!]==[%s-%s]=========\n", __DATE__, __TIME__);

    return 0;

err_exit:
    fh_stream_exit();

    return ret;
}


void uvc_stream_init(struct uvc_dev_app *pDev)
{
    pDev[STREAM_ID1].fcc = V4L2_PIX_FMT_MJPEG;
    pDev[STREAM_ID1].width = 1280;
    pDev[STREAM_ID1].height = 720;
    pDev[STREAM_ID2].fcc = V4L2_PIX_FMT_H264;
    pDev[STREAM_ID2].width = 720;
    pDev[STREAM_ID2].height = 576;

    if (fh_stream_init(pDev[STREAM_ID1].fcc, pDev[STREAM_ID1].width, pDev[STREAM_ID1].height) == RETURN_OK)
    {
        FHAdv_Isp_Init(FH_GRP_ID);
        /* ISP_sleep_mode(ISP_SENSOR_SLEEP); */
        pDev[STREAM_ID1].isp_complete = 1;
    }

}

void set_stream_probe(int stream_id, int fmt, int w, int h, int fps)
{
    if (w > YUY2_MAX_W && (fmt == V4L2_PIX_FMT_NV12 || fmt == V4L2_PIX_FMT_YUY2))
    {
        printf("%s:error, width is more than YUY2_MAX_W:%d\n", __func__, YUY2_MAX_W);
        return;
    }
    if (h > YUY2_MAX_H && (fmt == V4L2_PIX_FMT_NV12 || fmt == V4L2_PIX_FMT_YUY2))
    {
        printf("%s:error, height is more than YUY2_MAX_H:%d\n", __func__,YUY2_MAX_H);
        return;
    }
    if (change_video(stream_id, fmt, w, h) == RETURN_OK)
    {
        if ((fmt == V4L2_PIX_FMT_NV12 || fmt == V4L2_PIX_FMT_YUY2) && fps < 25)
            fps = 25;
        //if (stream_id == STREAM_ID1)
            //change_sensor_fps(fps);
    }
}

void uvc_stream_change(int stream_id, UVC_FORMAT fmt, int w, int h, int fps)
{
    uint32_t isp_format;

    g_uvc_dev[stream_id].fcc = fmt;
    g_uvc_dev[stream_id].fps = fps;
    g_uvc_dev[stream_id].width = w;
    g_uvc_dev[stream_id].height = h;
    isp_format = v4l2_type_to_stream_type(fmt);
    pthread_mutex_lock(&mutex_sensor);
    set_stream_probe(stream_id, fmt, w, h, fps);
    pthread_mutex_unlock(&mutex_sensor);
    if (isp_format == FH_STREAM_H264)
    {
        g_uvc_dev[stream_id].g_h264_delay = 5;
        printf("stream_id=%d f=H264 w=%d h=%d fps=%d\n", stream_id, w, h, fps);
    }
    else if (isp_format == FH_STREAM_MJPEG)
    {
        printf("stream_id=%d f=MJPG w=%d h=%d fps=%d\n", stream_id, w, h, fps);
    }
    else if (isp_format == FH_STREAM_H265)
    {
        g_uvc_dev[stream_id].g_h265_delay = 5;
        printf("stream_id=%d f=H265 w=%d h=%d fps=%d\n", stream_id, w, h, fps);
    } else if (isp_format == FH_STREAM_NV12)
    {
        printf("stream_id=%d f=NV12 w=%d h=%d fps=%d\n", stream_id, w, h, fps);
    } else if (isp_format == FH_STREAM_YUY2)
    {
        printf("stream_id=%d f=YUY2 w=%d h=%d fps=%d\n", stream_id, w, h, fps);
    } else if (isp_format == FH_STREAM_IR)
    {
        printf("stream_id=%d f=IR w=%d h=%d fps=%d\n", stream_id, w, h, fps);
    }
    else
    {
        printf("error: stream_id=%d f=%d w=%d h=%d fps=%d\n", stream_id, fmt, w, h, fps);
    }
}


struct uvc_format_info *get_uvc_format(int stream_id)
{
    if (stream_id == STREAM_ID1)
        return (struct uvc_format_info *)formats;
#ifdef UVC_DOUBLE_STREAM
    else
        return (struct uvc_format_info *)formats2;
#endif
    return NULL;
}

int get_uvc_format_num(int stream_id)
{
    if (stream_id == STREAM_ID1)
        return ARRAY_SIZE(formats);
#ifdef UVC_DOUBLE_STREAM
    else
        return ARRAY_SIZE(formats2);
#endif
    return 0;
}

#if defined(CONFIG_ARCH_FH8636_FH8852V20X) || defined(CONFIG_CHIP_FH8626V200)       /* YUY2 */
static unsigned int  g_YuyvDataLen;

void convertNV12ToYUY2(unsigned char *pNV12_Y, unsigned char *pNV12_UV, unsigned char *pYUY2, int width, int height)
{
    int *pdwY1 = (int *)pNV12_Y;
    int *pdwY2 = (int *)(pNV12_Y + width);
    int *pdwUV = (int *)(pNV12_UV);

    int *pdwYUYV1 = (int *)(pYUY2);
    int *pdwYUYV2 = (int *)(pYUY2 + (width<<1));
    int halfWidth = width>>1;
    int quarterWidth = width>>2;
    int halfHeight = height>>1;

    register int dwUV, Y1, Y2, UV;
    register int line, i;

    if (width > YUY2_MAX_W)
    {
        printf("error, width is more than YUY2_MAX_W\n");
        return;
    }

    if (height > YUY2_MAX_H)
    {
        printf("error, height is more than YUY2_MAX_H\n");
        return;
    }

    for (line = 0; line < halfHeight; line++)
    {
        for (i = 0; i < quarterWidth; i++)
        {
            UV = *pdwUV;
            Y1 = pdwY1[i];
            Y2 = pdwY2[i];

            dwUV = ((UV<<16)&0xff000000) | ((UV<<8)&0xff00);
            *pdwYUYV1 = (Y1 & 0xff) | ((Y1<<8)&0xff0000) | dwUV;
            *pdwYUYV2 = (Y2 & 0xff) | ((Y2<<8)&0xff0000) | dwUV;

            pdwYUYV1++;
            pdwYUYV2++;

            dwUV = ((UV)&0xff000000) | ((UV>>8)&0xff00);
            *pdwYUYV1 = ((Y1>>16) & 0xff) | ((Y1>>8)&0xff0000) | dwUV;
            *pdwYUYV2 = ((Y2>>16) & 0xff) | ((Y2>>8)&0xff0000) | dwUV;

            pdwYUYV1++;
            pdwYUYV2++;

            pdwUV++;

        }

        pdwY1 += halfWidth;
        pdwY2 += halfWidth;
        pdwYUYV1 += halfWidth;
        pdwYUYV2 += halfWidth;

    }
    if (width == 1920 && height == 1088)
    {
        g_YuyvDataLen = 1920*1088*2;
    }
    else if (width == 800 && height == 608)
    {
        g_YuyvDataLen = 800*608*2;
    }
    else
    {
        g_YuyvDataLen = width*height*2;
    }
}

unsigned int uvc_get_yuy2_data_len(void)
{
    return g_YuyvDataLen;
}
#endif
