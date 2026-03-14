#include "dsp/fh_vpu_mpi.h"
#include "isp/isp_common.h"
#include "isp/isp_api.h"
#include "isp/isp_enum.h"
#include "sample_common.h"

#define FH_APP_JPEG_CHANNEL 31

static volatile FH_SINT32 g_stop_venc = 0;
static volatile FH_SINT32 g_venc_runnig = 0;
static FH_SINT32 g_jpeg_channel_inited = 0;

#ifdef FH_APP_CHANGE_DSP_RESOLUTION
static void ClearChnData(FH_UINT32 request_type, int giveupNum)
{
    FH_VENC_STREAM stream;
    FH_SINT32 max_time = 1000, waitcnt = 0;
    FH_SINT32 getNum = 0;
    FH_SINT32 i;
    FH_SINT32 ret;

    if (request_type == 0)
        return;

    for (i = 0; i < max_time; i++)
    {
        ret = FH_VENC_GetStream(request_type, &stream);
        if (ret == RETURN_OK)
        {
            FH_VENC_ReleaseStream(&stream);
            getNum++;
            waitcnt = 0;
        }
        else
        {
            if (waitcnt++ >= 2 && (getNum >= giveupNum))
                break;
            usleep(1000 * 5);
        }
    }
}

/* 切换主码流分辨率 */
static FH_VOID change_dsp_resolution(FH_VOID)
{
    FH_SINT32 ret;
    static FH_SINT32 resolution_changed = 0;
    struct grp_vpu_info grp_info;
    struct vpu_channel_info vpu_info;
    struct enc_channel_info enc_info;
    FH_BIND_INFO src, dst;

    /* 获取目标分辨率 */
    sample_common_dsp_get_vpu_chn_info(FH_APP_GRP_ID, 0, &vpu_info);
    sample_common_dsp_get_enc_chn_info(FH_APP_GRP_ID, 0, &enc_info);
    sample_common_dsp_get_grp_info(FH_APP_GRP_ID, &grp_info);

    if (!grp_info.enable || !vpu_info.enable || !enc_info.enable)
    {
        printf("[DEMO_reso] error! group[%d] vpu[0] enc[0] needs to be enabled!\n", FH_APP_GRP_ID);
        return;
    }

    if (!resolution_changed)
    {
        vpu_info.width = ALIGNTO(vpu_info.width * 4 / 5, 16);
        vpu_info.height = ALIGNTO(vpu_info.height * 4 / 5, 16);
        enc_info.width = vpu_info.width;
        enc_info.height = vpu_info.height;
    }

    printf("Change resolution to %d x %d\n\n", vpu_info.width, vpu_info.height);

    if (sample_common_bgm_get_init_status(FH_APP_GRP_ID))
    {
        ret = FH_BGM_Disable(FH_APP_GRP_ID);
        if (ret != 0)
        {
            printf("Error(%d - %x): FH_BGM_Disable\n", ret, ret);
            return;
        }
    }

    /* Close GOSD MASK and GOSD GBOX */

    /* 关闭主码流VPU通道 */
    ret = FH_VPSS_CloseChn(FH_APP_GRP_ID, 0);
    if (ret != 0)
    {
        printf("Error(%d - %x): FH_VPSS_CloseChn\n", ret, ret);
        return;
    }

    if (enc_info.enc_type)
    {
        /* 关闭主码流编码通道 */
        ret = FH_VENC_StopRecvPic(FH_APP_GRP_ID * MAX_GRP_NUM + 0);
        if (ret != 0)
        {
            printf("Error(%d - %x): FH_VENC_StopRecvPic\n", ret, ret);
            return;
        }
    }

    ClearChnData(enc_info.enc_type, 0);

    ret = FH_VPSS_DestroyChn(FH_APP_GRP_ID, 0);
    if (ret != 0)
    {
        printf("Error(%d - %x): FH_VPSS_DestroyChn\n", ret, ret);
        return;
    }

    /* 按照目标分辨率重新创建主码流VPU通道 */
    ret = sample_common_vpu_create_chan(FH_APP_GRP_ID, 0, vpu_info.width, vpu_info.height, vpu_info.yuv_type);
    if (ret != 0)
    {
        printf("Error(%d - %x): sample_common_vpu_create_chan\n", ret, ret);
        return;
    }

    /* 按照目标分辨率重新创建主码流编码通道 */
    ret = sample_common_enc_set_chan(FH_APP_GRP_ID, 0, &enc_info);
    if (ret != 0)
    {
        printf("Error(%d - %x): sample_common_enc_set_chan\n", ret, ret);
        return;
    }

    if (enc_info.enc_type)
    {
        /* 绑定主码流vpu通道和编码通道 */
        ret = sample_common_vpu_bind_to_enc(FH_APP_GRP_ID, 0, 0);
        if (ret != 0)
        {
            printf("Error(%d - %x): sample_common_vpu_bind_to_enc\n", ret, ret);
            return;
        }
    }

    if (sample_common_bgm_get_init_status(FH_APP_GRP_ID))
    {
        FH_SIZE bgm_size;

        bgm_size.u32Width = vpu_info.width / grp_info.bgm_ds;
        bgm_size.u32Height = vpu_info.height / grp_info.bgm_ds;
        ret = FH_BGM_SetConfig(FH_APP_GRP_ID, &bgm_size);
        if (ret != 0)
        {
            printf("Error(%d - %x): FH_BGM_SetConfig\n", ret, ret);
            return;
        }

        ret = FH_BGM_Enable(FH_APP_GRP_ID);
        if (ret != 0)
        {
            printf("Error(%d - %x): FH_BGM_Enable\n", ret, ret);
            return;
        }

        src.obj_id = FH_OBJ_VPU_BGM;
        src.dev_id = 0;
        src.chn_id = grp_info.channel;
        dst.obj_id = FH_OBJ_BGM;
        dst.dev_id = 0;
        dst.chn_id = grp_info.channel;
        ret = FH_SYS_Bind(src, dst);
        if (ret != 0)
        {
            printf("Error(%d - %x): FH_SYS_BindVpu2Bgm\n", ret, ret);
            return;
        }
    }

    /* Reset GOSD MASK and GOSD GBOX */

    resolution_changed = !resolution_changed;
}
#endif

#ifdef FH_APP_CHANGE_FPS
/* 切换ISP帧率 */
static FH_VOID change_isp_fps(FH_VOID)
{
    sample_isp_change_fps(FH_APP_GRP_ID);
}
#endif

#ifdef FH_APP_CHANGE_ISP_RESOLUTION
/* 切换ISP帧率 */
static FH_VOID change_isp_resolution(FH_VOID)
{
    sample_isp_change_resolution(FH_APP_GRP_ID);
}
#endif

#ifdef FH_APP_ROTATE
/* 旋转编码通道 */
static FH_VOID rotate(FH_VOID)
{
#if defined(CONFIG_ARCH_FH8626V100)
    /* ZT不支持编码旋转 */
    printf("FH8626V100 do not support encode rotation!\n\n");
#else

    FH_SINT32 ret;
    static FH_UINT32 rotate_value = 0;
    FH_ROTATE_OPS rotate_info;
    struct grp_vpu_info grp_info;
    struct enc_channel_info enc_info;

    sample_common_dsp_get_enc_chn_info(FH_APP_GRP_ID, 0, &enc_info);
    sample_common_dsp_get_grp_info(FH_APP_GRP_ID, &grp_info);

    if (!grp_info.enable || !enc_info.enable)
    {
        printf("[DEMO_rotate] error! group[%d] enc[0] needs to be enabled!\n", FH_APP_GRP_ID);
        return;
    }

    rotate_value = (rotate_value + 1) % 4;

    /* 停止编码通道 */
    ret = FH_VENC_StopRecvPic(FH_APP_GRP_ID * MAX_GRP_NUM + 0);
    if (ret != 0)
    {
        printf("Error(%d - %x): FH_VENC_StopRecvPic\n", ret, ret);
        return;
    }

    /* 设置编码旋转角度 */
    rotate_info = rotate_value;

    ret = FH_VPSS_SetVORotate(FH_APP_GRP_ID, 0, rotate_info); /* yuv vofmt为tile时，需要该设置 */
    if (ret != 0)
    {
        printf("Error(%d - %x): FH_VPSS_SetVORotate\n", ret, ret);
    }

    ret = FH_VENC_SetRotate(FH_APP_GRP_ID * MAX_GRP_NUM + 0, rotate_info);
    if (ret != 0)
    {
        printf("Error(%d - %x): FH_VENC_SetRotate\n", ret, ret);
    }

    /* 开始编码 */
    ret = FH_VENC_StartRecvPic(FH_APP_GRP_ID * MAX_GRP_NUM + 0);
    if (ret != 0)
    {
        printf("Error(%d - %x): FH_VENC_StartRecvPic\n", ret, ret);
    }

    printf("Rotate to %d degree\n\n", rotate_value * 90);
#endif /* CONFIG_ARCH_FH8626V100 LINE204*/
}
#endif

#ifdef FH_APP_CAPTURE_JPEG
/* jpeg抓图 */
static FH_VOID capture_jpeg(FH_VOID)
{
    int ret;
    FH_VENC_STREAM stream;
    FH_UINT32 len;
    FH_BIND_INFO src, dst;

    struct grp_vpu_info grp_info;
    struct vpu_channel_info vpu_info;
    struct enc_channel_info jpeg_info;

    sample_common_dsp_get_vpu_chn_info(FH_APP_GRP_ID, 0, &vpu_info);
    sample_common_dsp_get_grp_info(FH_APP_GRP_ID, &grp_info);
    jpeg_info.channel = FH_APP_JPEG_CHANNEL;

    if (!grp_info.enable || !vpu_info.enable)
    {
        printf("[DEMO_jpeg] error! group[%d] vpu[0] needs to be enabled!\n", FH_APP_GRP_ID);
        return;
    }

    if (!g_jpeg_channel_inited)
    {
        /* 第一次调用需要创建jpeg编码通道 */
        jpeg_info.enc_type = FH_JPEG;
        jpeg_info.max_width = vpu_info.max_width;
        jpeg_info.max_height = vpu_info.max_height;
        ret = sample_common_enc_create_chan(0, jpeg_info.channel, &jpeg_info);
        if (ret)
        {
            printf("Error: create jpeg 0 failed,ret=%d!\n", ret);
            return;
        }

        ret = sample_common_enc_set_chan(0, jpeg_info.channel, &jpeg_info);
        if (ret)
        {
            printf("Error: set jpeg 0 failed,ret=%d!\n", ret);
            return;
        }

        g_jpeg_channel_inited = 1;
    }

    /* jpeg编码通道需要每次都绑定 */
    src.obj_id = FH_OBJ_VPU_VO;
    src.dev_id = FH_APP_GRP_ID;
    src.chn_id = 0;

    dst.obj_id = FH_OBJ_JPEG;
    dst.dev_id = 0;
    dst.chn_id = jpeg_info.channel;
    ret = FH_SYS_Bind(src, dst);
    if (ret != 0)
    {
        printf("Error(%d - %x): FH_SYS_BindVpu2jpeg\n", ret, ret);
    }

    /* 获取jpeg数据 */
    ret = FH_VENC_GetStream_Block(FH_STREAM_JPEG, &stream);
    if (ret == 0)
    {
        len = stream.jpeg_stream.length;
        FH_VENC_ReleaseStream(&stream);
        printf("Capture JPEG OK, size=%d\n\n", len);
        ret = FH_SYS_UnBindbyDst(dst);
        if (ret != 0)
        {
            printf("Error(%d - %x): FH_SYS_UnBindbyDst\n", ret, ret);
        }
    }
    else
    {
        printf("Error(%d - %x): GetSteam failed for JPEG!\n", ret, ret);
    }
}
#endif

#ifdef FH_APP_TOOGLE_FREEZE
/* 切换冻屏状态 */
static FH_VOID toggle_freeze(FH_VOID)
{
    static FH_SINT32 on = 0;
    FH_SINT32 ret;
    struct grp_vpu_info grp_info;

    sample_common_dsp_get_grp_info(FH_APP_GRP_ID, &grp_info);

    if (!grp_info.enable)
    {
        printf("[DEMO_freeze] error! group[%d] needs to be enabled!\n", FH_APP_GRP_ID);
        return;
    }

    on = !on;
    if (on)
    {
        printf("Freeze Video\n\n");
        ret = FH_VPSS_FreezeVideo(FH_APP_GRP_ID);
        if (ret != 0)
        {
            printf("Error(%d - %x): FH_VPSS_FreezeVideo\n", ret, ret);
        }
    }
    else
    {
        printf("Unfreeze Video\n\n");
        ret = FH_VPSS_UnfreezeVideo(FH_APP_GRP_ID);
        if (ret != 0)
        {
            printf("Error(%d - %x): FH_VPSS_UnfreezeVideo\n", ret, ret);
        }
    }
}
#endif

#ifdef FH_APP_CAPTURE_YUV
/* 获取VPU YUV数据 */
static FH_VOID capture_yuv(FH_VOID)
{
    FH_VPU_STREAM_ADV yuv_data;
    FH_UINT8 *dst, *in_buf;
    FH_SINT32 ret;
    FH_UINT32 w, h;
    struct grp_vpu_info grp_info;
    struct vpu_channel_info vpu_info;

    sample_common_dsp_get_vpu_chn_info(FH_APP_GRP_ID, 0, &vpu_info);
    sample_common_dsp_get_grp_info(FH_APP_GRP_ID, &grp_info);
    if (!grp_info.enable || !vpu_info.enable)
    {
        printf("[DEMO_cap] error! group[%d] vpu[0] needs to be enabled!\n", FH_APP_GRP_ID);
        return;
    }

    w = (vpu_info.width + 15) & (~15);
    h = (vpu_info.height + 15) & (~15);

    in_buf = FH_NULL;
    dst = FH_NULL;

    /* 分配yuv格式转换源数据空间 */
    in_buf = malloc(w * h * 3 / 2);
    if (in_buf == FH_NULL)
    {
        printf("Error: failed to malloc yuv transform input buffer\n");
        return;
    }

    /* 分配yuv格式转换目标数据空间 */
    dst = malloc(w * h * 3 / 2);
    if (dst == FH_NULL)
    {
        printf("Error: failed to allocate yuv transform buffer\n");
        free(in_buf);
        return;
    }

    /* 获取一帧YUV数据 */
    ret = FH_VPSS_GetChnFrameAdv(FH_APP_GRP_ID, 0, &yuv_data, 1000);
    if (ret == 0)
    {
        if (vpu_info.yuv_type == VPU_VOMODE_BLK) //
        {
            memcpy(in_buf, yuv_data.frm_blk.luma.data.vbase, w * h);
            memcpy(in_buf + w * h, yuv_data.frm_blk.chroma.data.vbase, w * h / 2);
            yuv_transform(dst, in_buf, in_buf + w * h, w, h, w);
        }
        else if (vpu_info.yuv_type == VPU_VOMODE_SCAN)
        {
            memcpy(dst, yuv_data.frm_scan.luma.data.vbase, w * h);
            memcpy(dst + w * h, yuv_data.frm_scan.chroma.data.vbase, w * h / 2);
        }
        else
        {
            printf("chan[0] yuv type is %d, tile format cannot be analyzed\n", vpu_info.yuv_type);
        }

        printf("GET CHN 0 YUV DATA w:%d h:%d OK\n\n", w, h);
    }
    else
    {
        printf("Error(%d - %x): FH_VPSS_GetChnFrameAdv chan[0]\n\n", ret, ret);
    }

    free(dst);
    free(in_buf);
}
#endif

FH_VOID *sample_venc_thread(FH_VOID *args)
{
    int cmd = 0;
    int k;

    prctl(PR_SET_NAME, "demo_venc");
    while (!g_stop_venc)
    {
#ifdef FH_APP_CHANGE_DSP_RESOLUTION
        if (cmd == 0)
        {
#if defined(NN_ENABLE_G0) || defined(NN_ENABLE_G1) || defined(NN_ENABLE_G2) || defined(NN_ENABLE_G3)
            printf("Warning: Change resolution may cause problems when nn is open\n");
#else
            change_dsp_resolution();
#endif
        }
#endif

#ifdef FH_APP_CHANGE_FPS
        if (cmd == 1)
        {
            change_isp_fps();
        }
#endif

#ifdef FH_APP_CHANGE_ISP_RESOLUTION
        if (cmd == 2)
        {
#if defined(NN_ENABLE_G0) || defined(NN_ENABLE_G1) || defined(NN_ENABLE_G2) || defined(NN_ENABLE_G3)
            printf("Warning: Change resolution may cause problems when nn is open\n");
#else
            change_isp_resolution();
#endif
        }
#endif

#ifdef FH_APP_ROTATE
        if (cmd == 3)
        {
#if defined(NN_ENABLE_G0) || defined(NN_ENABLE_G1) || defined(NN_ENABLE_G2) || defined(NN_ENABLE_G3)
            printf("Warning: Rotating screen may cause problems when nn is open\n");
#else
            rotate();
#endif
        }
#endif

#ifdef FH_APP_CAPTURE_JPEG
        if (cmd == 4)
        {
            capture_jpeg();
        }
#endif

#ifdef FH_APP_TOOGLE_FREEZE
        if (cmd == 5)
        {
            toggle_freeze();
        }
#endif

#ifdef FH_APP_CAPTURE_YUV
        if (cmd == 6)
        {
            capture_yuv();
        }
#endif

        if (++cmd > 6)
            cmd = 0;

        for (k = 0; k < 20 && !g_stop_venc; k++)
        {
            usleep(100 * 1000);
        }
    }

    g_venc_runnig = 0;

    return FH_NULL;
}

FH_SINT32 sample_venc_start(FH_VOID)
{
    FH_SINT32 ret;

    pthread_t venc_thread;
    pthread_attr_t attr;

    if (g_venc_runnig)
    {
        printf("Venc is already running\n");
        return 0;
    }

    g_venc_runnig = 1;
    g_stop_venc = 0;
    g_jpeg_channel_inited = 0;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize(&attr, 10 * 1024);
#ifdef __RTTHREAD_OS__
    struct sched_param param;
    param.sched_priority = 130;
    pthread_attr_setschedparam(&attr, &param);
#endif
    ret = pthread_create(&venc_thread, &attr, sample_venc_thread, FH_NULL);
    if (ret)
    {
        printf("Error: Create Venc thread failed!\n");
        g_venc_runnig = 0;
    }

    return ret;
}

FH_VOID sample_venc_stop(FH_VOID)
{
    g_stop_venc = 1;
    while (g_venc_runnig)
    {
        usleep(40 * 1000);
    }
}
