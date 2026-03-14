
#include "rtconfig_app.h"
#include "media_config_proc.h"
#include <sample_common.h>
#include "chn_yuv_type.h"
#include "chn_enc_type.h"
#include "chn_rc_type.h"
#include "chn_set_rc.h"
#include "dsp_struct_info.h"

extern FH_SINT32 sample_dmc_init(FH_CHAR *dst_ip, FH_UINT32 port);
extern FH_VOID *sample_common_get_stream_proc(FH_VOID *arg);
extern FH_VOID *sample_common_send_stream_proc(FH_VOID *arg);

extern FH_SINT32 sample_dmc_deinit(FH_VOID);

FH_VOID sample_common_dsp_get_grp_info(FH_UINT32 chan_vpu, struct grp_vpu_info *info) /* can ben optimized */
{
    *info = g_vpu_grp_infos[chan_vpu];
}

FH_VOID sample_common_dsp_get_vpu_chn_info(FH_UINT32 grpid, FH_UINT32 chan_vpu, struct vpu_channel_info *info) /* can ben optimized */
{
    *info = g_vpu_chn_infos[grpid][chan_vpu];
            /*detect invalid max_width & max_height, and correct it...*/
}

FH_VOID sample_common_dsp_get_enc_chn_info(FH_UINT32 grpid, FH_UINT32 chan_vpu, struct enc_channel_info *info) /* can ben optimized */
{
    *info = g_enc_chn_infos[grpid][chan_vpu];
}

FH_SINT32 sample_common_media_sys_init(FH_VOID)
{
#if defined(CONFIG_ARCH_FH8636_FH8852V20X) || defined(CONFIG_CHIP_FH8626V200) || defined(CONFIG_CHIP_FH8852V310)
    jpeg_write_proc("frmsize_15_1000000_1000000");
    jpeg_write_proc("allchnstm_0_5000000_32");
    enc_write_proc("allchnstm_0_6000000_128");
    enc_write_proc("stm_6000000_128");
    jpeg_write_proc("mjpgstm_5000000_32");
#else
    jpeg_write_proc("frmsize_15_3000000_3000000");
    jpeg_write_proc("allchnstm_0_12000000_32");
    enc_write_proc("allchnstm_0_12000000_128");
    enc_write_proc("stm_12000000_128");
    jpeg_write_proc("mjpgstm_12000000_32");
#endif
    return FH_SYS_Init();
}

FH_SINT32 sample_common_media_sys_exit(FH_VOID)
{
    return FH_SYS_Exit();
}

FH_SINT32 sample_common_dsp_init(FH_UINT32 grpid)
{
    FH_SINT32 ret;
    FH_VPU_SIZE vi_pic;
    FH_VPU_SET_GRP_INFO grp_info;

    grp_info.vi_max_size.u32Width = sample_isp_get_vi_w(grpid);
    grp_info.vi_max_size.u32Height = sample_isp_get_vi_h(grpid);
    grp_info.ycmean_en = 1;
    grp_info.ycmean_ds = 16;
    ret = FH_VPSS_CreateGrp(grpid, &grp_info);
    if (ret)
    {
        printf("Error(%d - %x): FH_VPSS_CreateGrp (grp):(%d)!\n", ret, ret, grpid);
        return -1;
    }
    vi_pic.vi_size.u32Width = sample_isp_get_vi_w(grpid);
    vi_pic.vi_size.u32Height = sample_isp_get_vi_h(grpid);
#if !defined(CONFIG_ARCH_FH8636_FH8852V20X) && !defined(CONFIG_CHIP_FH8626V200)
    vi_pic.crop_area.crop_en = 0;
    vi_pic.crop_area.vpu_crop_area.u32X = 0;
    vi_pic.crop_area.vpu_crop_area.u32Y = 0;
    vi_pic.crop_area.vpu_crop_area.u32Width = 0;
    vi_pic.crop_area.vpu_crop_area.u32Height = 0;
#endif
    ret = FH_VPSS_SetViAttr(grpid, &vi_pic);
    if (ret != RETURN_OK)
    {
        printf("Error(%d - %x): FH_VPSS_SetViAttr (grp):(%d)!\n", ret, ret, grpid);
        return ret;
    }

    return FH_VPSS_Enable(grpid, g_vpu_grp_infos[grpid].mode);
}


FH_SINT32 sample_common_vpu_create_chan(FH_UINT32 grpid, FH_UINT32 chan_vpu, FH_UINT32 width, FH_UINT32 height, FH_UINT32 yuv_type)
{
    FH_SINT32 ret;
    FH_VPU_CHN_INFO chn_info;
    FH_VPU_CHN_CONFIG chn_attr;

    if (chan_vpu == 0)
    {
        chn_info.bgm_enable = g_vpu_grp_infos[grpid].bgm_enable;
        chn_info.cpy_enable = g_vpu_grp_infos[grpid].cpy_enable;
        chn_info.sad_enable = g_vpu_grp_infos[grpid].sad_enable;
        chn_info.bgm_ds = g_vpu_grp_infos[grpid].bgm_ds;
    }
    else
    {
        chn_info.bgm_enable = 0;
        chn_info.cpy_enable = 0;
        chn_info.sad_enable = 0;
        chn_info.bgm_ds = 0;
    }
    chn_info.chn_max_size.u32Width = width;
    chn_info.chn_max_size.u32Height = height;
    chn_info.out_mode = yuv_type;
    chn_info.support_mode = 1 << yuv_type;
    chn_info.bufnum = g_vpu_chn_infos[grpid][chan_vpu].bufnum;
    chn_info.max_stride = 0;
    ret = FH_VPSS_CreateChn(grpid, chan_vpu, &chn_info);
    if (ret != RETURN_OK)
    {
        printf("Error(%d - %x): FH_VPSS_CreateChn (grp-chn):(%d-%d)!\n", ret, ret, grpid, chan_vpu);
        return ret;
    }
    chn_attr.vpu_chn_size.u32Width  = width;
    chn_attr.vpu_chn_size.u32Height = height;
    chn_attr.crop_area.crop_en = 0;
    chn_attr.crop_area.vpu_crop_area.u32X = 0;
    chn_attr.crop_area.vpu_crop_area.u32Y = 0;
    chn_attr.crop_area.vpu_crop_area.u32Width = 0;
    chn_attr.crop_area.vpu_crop_area.u32Height = 0;
    chn_attr.offset = 0;
    chn_attr.depth = 1;
    chn_attr.stride = 0;
    ret = FH_VPSS_SetChnAttr(grpid, chan_vpu, &chn_attr);
    if (ret != RETURN_OK)
    {
        printf("Error(%d - %x): FH_VPSS_SetChnAttr (grp-chn):(%d-%d)!\n", ret, ret, grpid, chan_vpu);
        return ret;
    }

    ret = FH_VPSS_SetVOMode(grpid, chan_vpu, yuv_type);
    if (ret != RETURN_OK)
    {
        printf("Error(%d - %x): FH_VPSS_SetVOMode (grp-chn):(%d-%d)!\n", ret, ret, grpid, chan_vpu);
        return ret;
    }
    return FH_VPSS_OpenChn(grpid, chan_vpu);
}

FH_SINT32 sample_common_vpu_bind_to_enc(FH_UINT32 grpid, FH_UINT32 chan_vpu, FH_UINT32 chan_enc)
{
    FH_SINT32 ret;
    FH_BIND_INFO src, dst;

    ret = FH_VENC_StartRecvPic(grpid * MAX_VPU_CHN_NUM + chan_enc);
    if (ret != RETURN_OK)
    {
        printf("Error(%d-%x): FH_VENC_StartRecvPic!\n", ret, ret);
        return ret;
    }

    src.obj_id = FH_OBJ_VPU_VO;
    src.dev_id = grpid;
    src.chn_id = chan_vpu;

    dst.obj_id = FH_OBJ_ENC;
    dst.dev_id = 0;
    dst.chn_id = grpid * MAX_VPU_CHN_NUM + chan_enc;
    FH_SYS_UnBindbyDst(dst);
    return FH_SYS_Bind(src, dst);
}

FH_SINT32 sample_common_vpu_bind_to_jpeg(FH_UINT32 grpid, FH_UINT32 chan_vpu, FH_UINT32 chan_enc)
{
    FH_SINT32 ret;
    FH_BIND_INFO src, dst;
    FH_SINT32 jpeg_chn = 0;
    jpeg_chn = JPEG_ENC_CHN - chan_enc - (FH_GRP_ID * MAX_VPU_CHN_NUM);
    ret = FH_VENC_StartRecvPic(jpeg_chn);
    if (ret != RETURN_OK)
    {
        printf("Error(%d-%x): FH_VENC_StartRecvPic!\n", ret, ret);
        return ret;
    }
    src.obj_id = FH_OBJ_VPU_VO;
    src.dev_id = grpid;
    src.chn_id = chan_vpu;
    dst.obj_id = FH_OBJ_JPEG;
    dst.dev_id = 0;
    dst.chn_id = jpeg_chn;
    FH_SYS_UnBindbyDst(dst);
    return FH_SYS_Bind(src, dst);
}
/* 设置码控类型和参数 */

static FH_SINT32 sample_common_set_rc_type(struct enc_channel_info *info, FH_VENC_CHN_CONFIG *cfg)
{
    FH_SINT32 ret = 0;
    FH_UINT32 rc_type = info->rc_type;

    switch (rc_type)
    {
    case FH_RC_H264_FIXQP:
    {
        sample_common_set_h264_rc_fixqp(info, cfg);
        break;
    }
    case FH_RC_H264_VBR:
    {
        sample_common_set_h264_rc_vbr(info, cfg);
        break;
    }
    case FH_RC_H264_CBR:
    {
        sample_common_set_h264_rc_cbr(info, cfg);
        break;
    }
    case FH_RC_H264_AVBR:
    {
        sample_common_set_h264_rc_avbr(info, cfg);
        break;
    }
    case FH_RC_H264_CVBR:
    {
        sample_common_set_h264_rc_cvbr(info, cfg);
        break;
    }

    case FH_RC_H265_FIXQP:
    {
        sample_common_set_h265_rc_fixqp(info, cfg);
        break;
    }
    case FH_RC_H265_VBR:
    {
        sample_common_set_h265_rc_vbr(info, cfg);
        break;
    }
    case FH_RC_H265_CBR:
    {
        sample_common_set_h265_rc_cbr(info, cfg);
        break;
    }
    case FH_RC_H265_CVBR:
    {
        sample_common_set_h265_rc_cvbr(info, cfg);
        break;
    }
    case FH_RC_H265_AVBR:
    {
        sample_common_set_h265_rc_avbr(info, cfg);
        break;
    }

    case FH_RC_H264_QVBR:
    {
        sample_common_set_h264_rc_qvbr(info, cfg);
        break;
    }
    case FH_RC_H265_QVBR:
    {
        sample_common_set_h265_rc_qvbr(info, cfg);
        break;
    }

    case FH_RC_MJPEG_FIXQP:
    {
        sample_common_set_mjpeg_rc_fixqp(info, cfg);
        break;
    }
    case FH_RC_MJPEG_VBR:
    {
        sample_common_set_mjpeg_rc_vbr(info, cfg);
        break;
    }
    default:
    {
        ret = -1;
        break;
    }
    }

    return ret;
}

/* 设置H264编码通道 */
static FH_SINT32 sample_common_set_h264_chan(FH_UINT32 grpid, FH_UINT32 chan, struct enc_channel_info *info)
{
    FH_VENC_CHN_CONFIG cfg_param;

    cfg_param.chn_attr.enc_type                      = FH_NORMAL_H264;
    cfg_param.chn_attr.h264_attr.profile             = H264_PROFILE_MAIN;
    cfg_param.chn_attr.h264_attr.i_frame_intterval = 4;
    cfg_param.chn_attr.h264_attr.size.u32Width       = info->width;
    cfg_param.chn_attr.h264_attr.size.u32Height      = info->height;

    if (sample_common_set_rc_type(info, &cfg_param))
    {
        sample_common_set_h264_rc_vbr(info, &cfg_param);
    }

    return FH_VENC_SetChnAttr(grpid * MAX_VPU_CHN_NUM + chan, &cfg_param);
}

/* 设置H264智能编码通道 */
static FH_SINT32 sample_common_set_s264_chan(FH_UINT32 grpid, FH_UINT32 chan, struct enc_channel_info *info)
{
    FH_VENC_CHN_CONFIG cfg_param;

    cfg_param.chn_attr.enc_type                          = FH_SMART_H264;
    cfg_param.chn_attr.s264_attr.profile = H264_PROFILE_MAIN;
    cfg_param.chn_attr.s264_attr.refresh_frame_intterval = 50;
    cfg_param.chn_attr.s264_attr.size.u32Width           = info->width;
    cfg_param.chn_attr.s264_attr.size.u32Height          = info->height;
    cfg_param.chn_attr.s264_attr.smart_en                = FH_TRUE;
    cfg_param.chn_attr.s264_attr.texture_en              = FH_TRUE;
    cfg_param.chn_attr.s264_attr.backgroudmodel_en       = FH_TRUE;
    cfg_param.chn_attr.s264_attr.bgm_chn = grpid;

    cfg_param.chn_attr.s264_attr.gop_th.GOP_TH_NUM       = 4;
    cfg_param.chn_attr.s264_attr.gop_th.TH_VAL[0]        = 8;
    cfg_param.chn_attr.s264_attr.gop_th.TH_VAL[1]        = 15;
    cfg_param.chn_attr.s264_attr.gop_th.TH_VAL[2]        = 25;
    cfg_param.chn_attr.s264_attr.gop_th.TH_VAL[3]        = 35;
    cfg_param.chn_attr.s264_attr.gop_th.MIN_GOP[0]       = 380;
    cfg_param.chn_attr.s264_attr.gop_th.MIN_GOP[1]       = 330;
    cfg_param.chn_attr.s264_attr.gop_th.MIN_GOP[2]       = 270;
    cfg_param.chn_attr.s264_attr.gop_th.MIN_GOP[3]       = 220;
    cfg_param.chn_attr.s264_attr.gop_th.MIN_GOP[4]       = 160;

    if (sample_common_set_rc_type(info, &cfg_param))
    {
        sample_common_set_h264_rc_vbr(info, &cfg_param);
    }

    return FH_VENC_SetChnAttr(grpid * MAX_VPU_CHN_NUM + chan, &cfg_param);
}

/* 设置H265编码通道 */
static FH_SINT32 sample_common_set_h265_chan(FH_UINT32 grpid, FH_UINT32 chan, struct enc_channel_info *info)
{
    FH_VENC_CHN_CONFIG cfg_param;

    cfg_param.chn_attr.enc_type                      = FH_NORMAL_H265;
    cfg_param.chn_attr.h265_attr.profile             = H265_PROFILE_MAIN;
    cfg_param.chn_attr.h265_attr.i_frame_intterval   = 50;
    cfg_param.chn_attr.h265_attr.size.u32Width       = info->width;
    cfg_param.chn_attr.h265_attr.size.u32Height      = info->height;

    if (sample_common_set_rc_type(info, &cfg_param))
    {
        sample_common_set_h265_rc_vbr(info, &cfg_param);
    }

    return FH_VENC_SetChnAttr(grpid * MAX_VPU_CHN_NUM + chan, &cfg_param);
}

/* 设置H265智能编码通道 */
static FH_SINT32 sample_common_set_s265_chan(FH_UINT32 grpid, FH_UINT32 chan, struct enc_channel_info *info)
{
    FH_VENC_CHN_CONFIG cfg_param;

    cfg_param.chn_attr.enc_type                          = FH_SMART_H265;
    cfg_param.chn_attr.s265_attr.profile                 = H265_PROFILE_MAIN;
    cfg_param.chn_attr.s265_attr.refresh_frame_intterval = 50;
    cfg_param.chn_attr.s265_attr.size.u32Width           = info->width;
    cfg_param.chn_attr.s265_attr.size.u32Height          = info->height;
    cfg_param.chn_attr.s265_attr.smart_en                = FH_TRUE;
    cfg_param.chn_attr.s265_attr.texture_en              = FH_TRUE;
    cfg_param.chn_attr.s265_attr.backgroudmodel_en       = FH_TRUE;
    cfg_param.chn_attr.s265_attr.bgm_chn = grpid;

    cfg_param.chn_attr.s265_attr.gop_th.GOP_TH_NUM       = 4;
    cfg_param.chn_attr.s265_attr.gop_th.TH_VAL[0]        = 8;
    cfg_param.chn_attr.s265_attr.gop_th.TH_VAL[1]        = 15;
    cfg_param.chn_attr.s265_attr.gop_th.TH_VAL[2]        = 25;
    cfg_param.chn_attr.s265_attr.gop_th.TH_VAL[3]        = 35;
    cfg_param.chn_attr.s265_attr.gop_th.MIN_GOP[0]       = 380;
    cfg_param.chn_attr.s265_attr.gop_th.MIN_GOP[1]       = 330;
    cfg_param.chn_attr.s265_attr.gop_th.MIN_GOP[2]       = 270;
    cfg_param.chn_attr.s265_attr.gop_th.MIN_GOP[3]       = 220;
    cfg_param.chn_attr.s265_attr.gop_th.MIN_GOP[4]       = 160;

    if (sample_common_set_rc_type(info, &cfg_param))
    {
        sample_common_set_h265_rc_vbr(info, &cfg_param);
    }

    return FH_VENC_SetChnAttr(grpid * MAX_VPU_CHN_NUM + chan, &cfg_param);
}

static FH_SINT32 sample_common_set_mjpeg_chan(FH_UINT32 grpid, FH_UINT32 chan, struct enc_channel_info *info)
{
    int ret;
    FH_VENC_CHN_CONFIG cfg_param;

    cfg_param.chn_attr.enc_type = FH_MJPEG;
    cfg_param.chn_attr.mjpeg_attr.pic_size.u32Width     = info->width;
    cfg_param.chn_attr.mjpeg_attr.pic_size.u32Height    = info->height;
    cfg_param.chn_attr.mjpeg_attr.rotate                = 0;
    cfg_param.chn_attr.mjpeg_attr.encode_speed          = 0;/* 0-9 */

    if (sample_common_set_rc_type(info, &cfg_param))
    {
        sample_common_set_mjpeg_rc_fixqp(info, &cfg_param);
    }
    ret = FH_VENC_SetChnAttr((JPEG_ENC_CHN - chan) - (grpid * MAX_VPU_CHN_NUM), &cfg_param);
    if (ret != RETURN_OK)
    {
        printf("Error: FH_VENC_SetChnAttr failed with %d\n", ret);
        return -1;
    }
    return 0;
}

static FH_SINT32 sample_common_set_jpeg_chan(FH_UINT32 grpid, FH_UINT32 chan, struct enc_channel_info *info)
{
    FH_VENC_CHN_CONFIG enc_cfg_param;

    enc_cfg_param.chn_attr.enc_type               = FH_JPEG;
    enc_cfg_param.chn_attr.jpeg_attr.qp           = 50;
    enc_cfg_param.chn_attr.jpeg_attr.rotate       = 0;
    enc_cfg_param.chn_attr.jpeg_attr.encode_speed = 0;

    return FH_VENC_SetChnAttr(grpid * MAX_VPU_CHN_NUM + chan, &enc_cfg_param);
}

FH_SINT32 sample_common_enc_create_chan(FH_UINT32 grpid, FH_UINT32 chan_enc, struct enc_channel_info *enc_info)
{
    FH_VENC_CHN_CAP cfg_vencmem;

    cfg_vencmem.support_type = enc_info->enc_type;
    cfg_vencmem.max_size.u32Width = enc_info->max_width;
    cfg_vencmem.max_size.u32Height = enc_info->max_height;

    if (enc_info->enc_type == FH_MJPEG)
        return FH_VENC_CreateChn(chan_enc, &cfg_vencmem);
    else
    return FH_VENC_CreateChn(grpid * MAX_VPU_CHN_NUM + chan_enc, &cfg_vencmem);
}

static FH_SINT32 sample_common_enc_set_breath(FH_UINT32 grpid, FH_UINT32 chan, FH_UINT16 breath_en)
{
    FH_DEBREATH breath;

    breath.debreath_en = breath_en;
    breath.debreath_ratio = 16;

    return FH_VENC_SetDeBreathEffect(grpid * MAX_VPU_CHN_NUM + chan, &breath);
}

FH_SINT32 sample_common_enc_set_chan(FH_UINT32 grpid, FH_UINT32 chan_enc, struct enc_channel_info *info)
{
    FH_SINT32 ret;

    switch (info->enc_type)
    {
    case FH_NORMAL_H264:
    ret = sample_common_set_h264_chan(grpid, chan_enc, info);
        break;

    case FH_SMART_H264:
    ret = sample_common_set_s264_chan(grpid, chan_enc, info);
        break;

    case FH_NORMAL_H265:
    ret = sample_common_set_h265_chan(grpid, chan_enc, info);
        break;

    case FH_SMART_H265:
    ret = sample_common_set_s265_chan(grpid, chan_enc, info);
        break;

    case FH_MJPEG:
    ret = sample_common_set_mjpeg_chan(grpid, chan_enc, info);
        break;

    case FH_JPEG:
    ret = sample_common_set_jpeg_chan(grpid, chan_enc, info);
        break;

    default:
        ret = 0;
        break;
    }

    if (ret)
    {
        printf("Error: set encode chan failed %d(%x)\n", ret, ret);
        return ret;
    }

    if (info->enc_type != FH_JPEG && info->enc_type != FH_MJPEG)
    {
        ret = sample_common_enc_set_breath(grpid, chan_enc, info->breath_on);
    }

    return ret;
}

FH_SINT32 sample_common_start_vpu(FH_VOID)
{
    int ret, i;
    struct vpu_channel_info *pvpu;
    ret = sample_common_dsp_init(FH_GRP_ID);
    if (ret != RETURN_OK)
    {
        printf("Error(%d - %x): sample_common_dsp_init (grp):(%d)!\n", ret, ret, FH_GRP_ID);
        return -1;
    }

    for (i = 0; i < MAX_VPU_CHN_NUM; i++)
    {
        pvpu = &g_vpu_chn_infos[FH_GRP_ID][i];
        if (pvpu->enable)
        {
            ret = sample_common_vpu_create_chan(FH_GRP_ID, i, pvpu->width, pvpu->height, pvpu->yuv_type);
            if (ret != RETURN_OK)
            {
                printf("Error(%d - %x): sample_common_vpu_create_chan (grp-chn):(%d-%d)!\n", ret, ret, FH_GRP_ID, i);
                return -1;
            }
        }
    }
    return 0;
}

FH_SINT32 sample_common_start_enc(FH_VOID)
{
    int ret;
    int grploop, encloop;
    struct enc_channel_info *penc;
    struct grp_vpu_info *pgrp;

    for (grploop = 0; grploop < MAX_GRP_NUM; grploop++)
    {
        if (grploop != FH_GRP_ID)
            continue;
        pgrp = &g_vpu_grp_infos[grploop];
        if (pgrp->enable)
        {
            for (encloop = 0; encloop < MAX_VPU_CHN_NUM; encloop++)
            {
                penc = &g_enc_chn_infos[grploop][encloop];
                if (penc->enable)
                {
                    penc->enc_type = FH_NORMAL_H264 | FH_NORMAL_H265;
                    ret = sample_common_enc_create_chan(grploop, encloop, penc);
                    if (ret != RETURN_OK)
                    {
                        printf("Error(%d - %x): sample_common_enc_create_chan (grp-chn):(%d-%d)!\n", ret, ret, grploop, encloop);
                        return -1;
                    }
                    penc->enc_type = FH_NORMAL_H264;
                    ret = sample_common_enc_set_chan(grploop, encloop, penc);
                    if (ret != RETURN_OK)
                    {
                        printf("Error(%d - %x): sample_common_enc_set_chan (grp-chn):(%d-%d)!\n", ret, ret, grploop, encloop);
                        return -1;
                    }
                }
            }
        }
    }
    return 0;
}

FH_SINT32 sample_common_start_mjpeg(FH_VOID)
{
    int ret;
    int jpeg_chn = 0;
    int grploop, jpegloop;
    struct enc_channel_info *pjpeg;

    for (grploop = 0; grploop < MAX_GRP_NUM; grploop++)
    {
        if (grploop != FH_GRP_ID)
            continue;
        for (jpegloop = 0; jpegloop < MAX_VPU_CHN_NUM; jpegloop++)
        {
            pjpeg = &g_jpeg_chn_infos[grploop * MAX_VPU_CHN_NUM + jpegloop];
            if (pjpeg->enable)
            {
                jpeg_chn = JPEG_ENC_CHN - jpegloop - (FH_GRP_ID * MAX_VPU_CHN_NUM);
                pjpeg->channel = jpeg_chn;
                pjpeg->enc_type = FH_MJPEG;
                pjpeg->rc_type = FH_RC_MJPEG_FIXQP;
                ret = sample_common_enc_create_chan(grploop, jpeg_chn, pjpeg);
                if (ret != RETURN_OK)
                {
                    printf("Error(%d - %x): jpeg sample_common_enc_create_chan (chn):(%d)!\n", ret, jpeg_chn, jpeg_chn);
                    return -1;
                }

                ret = sample_common_enc_set_chan(grploop, jpegloop, pjpeg);
                if (ret != RETURN_OK)
                {
                    printf("Error(%d - %x): jpeg sample_common_enc_set_chan (chn):(%d)!\n", ret, jpeg_chn, jpeg_chn);
                    return -1;
    }
            }
        }
    }
    return 0;
}
