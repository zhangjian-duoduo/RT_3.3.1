#include <sample_common.h>
#include "chn_yuv_type.h"
#include "chn_enc_type.h"
#include "chn_rc_type.h"
#include "chn_set_rc.h"
#include "mem_in_type.h"
#include "vpu_input_type.h"
#include "dsp_struct_info.h"

static FH_SINT32 g_get_stream_stop;
static FH_SINT32 g_get_stream_running;
static FH_SINT32 g_jpeg_chn = 30;

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
}

FH_VOID sample_common_dsp_get_enc_chn_info(FH_UINT32 grpid, FH_UINT32 chan_vpu, struct enc_channel_info *info) /* can ben optimized */
{
    *info = g_enc_chn_infos[grpid][chan_vpu];
}

FH_SINT32 sample_common_media_sys_init(FH_VOID)
{
    FH_SINT32 ret = 0;
    struct vpu_channel_info *chn_info;
    FH_UINT32 blk_size;
    VB_CONF_S stVbConf;
    FH_VB_Exit();

    memset(&stVbConf, 0, sizeof(VB_CONF_S));

    WR_PROC_DEV(JPEG_PROC, "allchnstm_0_12000000_32");
    WR_PROC_DEV(JPEG_PROC, "frmsize_30_3000000_3000000");
    WR_PROC_DEV(JPEG_PROC, "jpgstm_12000000_32");
    WR_PROC_DEV(JPEG_PROC, "mjpgstm_12000000_32");
    WR_PROC_DEV(ENC_PROC, "stm_12000000_128");
    WR_PROC_DEV(ENC_PROC, "allchnstm_0_12000000_128");

    ret = FH_SYS_Init();
    if (ret)
    {
        printf("[FH_SYS_Init] failed with:%x\n", ret);
        return ret;
    }

    stVbConf.u32MaxPoolCnt = MAX_GRP_NUM * (MAX_VPU_CHN_NUM + 2); // 6 为3个VICAP+3个ISP的最大值
    for (int grp_id = 0; grp_id < MAX_GRP_NUM; grp_id++)
    {
        for (int chn_id = 0; chn_id < MAX_VPU_CHN_NUM; chn_id++)
        {
            chn_info = &g_vpu_chn_infos[grp_id][chn_id];
            if (chn_info->enable && (chn_info->bufnum == 0))
            {
                ret = FH_VPSS_GetFrameBlkSize(chn_info->max_width, chn_info->max_height, 1 << chn_info->yuv_type, &blk_size);
                if (ret)
                {
                    printf("[FH_VPSS_GetFrameBlkSize] failed with:%x\n", ret);
                    return ret;
                }
                stVbConf.astCommPool[grp_id * MAX_VPU_CHN_NUM + chn_id].u32BlkSize = blk_size;
                stVbConf.astCommPool[grp_id * MAX_VPU_CHN_NUM + chn_id].u32BlkCnt = 3;
                printf("[VB] GRP[%d]-CHN[%d] blk_size=%X\n", grp_id, chn_id, blk_size);
            }
        }
    }

    ret = FH_VB_SetConf(&stVbConf);
    if (ret)
    {
        printf("[FH_VB_SetConf] failed with:%x\n", ret);
        return -1;
    }

    ret = FH_VB_Init();
    if (ret)
    {
        printf("[FH_VB_Init] failed with:%x\n", ret);
        return -1;
    }

    return ret;
}

FH_SINT32 sample_common_media_sys_exit(FH_VOID)
{
    int ret32;
    ret32 = FH_SYS_Exit();
    if (ret32)
    {
        printf("[FH_SYS_Exit] failed with:%x\n", ret32);
        return -1;
    }

    return FH_VB_Exit();
}

FH_UINT32 get_vpu_vi_w(FH_UINT32 grp_id)
{
    FH_SINT32 vi_w = 0;

    if (g_vpu_grp_infos[grp_id].vpu_vi_crop.crop_en)
    {
        vi_w = g_vpu_grp_infos[grp_id].vpu_vi_crop.vpu_crop_area.u32Width;
    }
    else
    {
        vi_w = sample_isp_get_vi_w(grp_id);
    }

    return vi_w;
}

FH_UINT32 get_vpu_vi_h(FH_UINT32 grp_id)
{
    FH_SINT32 vi_h = 0;

    if (g_vpu_grp_infos[grp_id].vpu_vi_crop.crop_en)
    {
        vi_h = g_vpu_grp_infos[grp_id].vpu_vi_crop.vpu_crop_area.u32Height;
    }
    else
    {
        vi_h = sample_isp_get_vi_h(grp_id);
    }

    return vi_h;
}

FH_UINT32 get_vpu_chn_w(FH_UINT32 grp_id, FH_UINT32 chn_id)
{
    FH_SINT32 vo_w = 0;

    if (g_vpu_chn_infos[grp_id][chn_id].chn_out_crop.crop_en)
    {
        vo_w = g_vpu_chn_infos[grp_id][chn_id].chn_out_crop.vpu_crop_area.u32Width;
    }
    else
    {
        vo_w = g_vpu_chn_infos[grp_id][chn_id].width;
    }

    return vo_w;
}

FH_UINT32 get_vpu_chn_h(FH_UINT32 grp_id, FH_UINT32 chn_id)
{
    FH_SINT32 vo_h = 0;

    if (g_vpu_chn_infos[grp_id][chn_id].chn_out_crop.crop_en)
    {
        vo_h = g_vpu_chn_infos[grp_id][chn_id].chn_out_crop.vpu_crop_area.u32Height;
    }
    else
    {
        vo_h = g_vpu_chn_infos[grp_id][chn_id].height;
    }

    return vo_h;
}

FH_SINT32 sample_common_dsp_init(FH_UINT32 grpid)
{
    FH_SINT32 ret;
    FH_VPU_SIZE vi_pic;

    g_vpu_grp_infos[grpid].grp_info.vi_max_size.u32Width = get_vpu_vi_w(grpid);
    g_vpu_grp_infos[grpid].grp_info.vi_max_size.u32Height = get_vpu_vi_h(grpid);

    ret = FH_VPSS_CreateGrp(grpid, &g_vpu_grp_infos[grpid].grp_info);
    if (ret)
    {
        printf("Error(%d - %x): FH_VPSS_CreateGrp (grp):(%d)!\n", ret, ret, grpid);
        return -1;
    }

    vi_pic.vi_size.u32Width = get_vpu_vi_w(grpid);
    vi_pic.vi_size.u32Height = get_vpu_vi_h(grpid);
    vi_pic.crop_area.crop_en = g_vpu_grp_infos[grpid].vpu_vi_crop.crop_en;
    vi_pic.crop_area.vpu_crop_area.u32X = g_vpu_grp_infos[grpid].vpu_vi_crop.vpu_crop_area.u32X;
    vi_pic.crop_area.vpu_crop_area.u32Y = g_vpu_grp_infos[grpid].vpu_vi_crop.vpu_crop_area.u32Y;
    vi_pic.crop_area.vpu_crop_area.u32Width = g_vpu_grp_infos[grpid].vpu_vi_crop.vpu_crop_area.u32Width;
    vi_pic.crop_area.vpu_crop_area.u32Height = g_vpu_grp_infos[grpid].vpu_vi_crop.vpu_crop_area.u32Height;

    ret = FH_VPSS_SetViAttr(grpid, &vi_pic);
    if (ret != 0)
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
    if (ret != 0)
    {
        printf("Error(%d - %x): FH_VPSS_CreateChn (grp-chn):(%d-%d)!\n", ret, ret, grpid, chan_vpu);
        return ret;
    }

    chn_attr.vpu_chn_size.u32Width = width;
    chn_attr.vpu_chn_size.u32Height = height;
    chn_attr.crop_area.crop_en = g_vpu_chn_infos[grpid][chan_vpu].chn_out_crop.crop_en;
    chn_attr.crop_area.vpu_crop_area.u32X = g_vpu_chn_infos[grpid][chan_vpu].chn_out_crop.vpu_crop_area.u32X;
    chn_attr.crop_area.vpu_crop_area.u32Y = g_vpu_chn_infos[grpid][chan_vpu].chn_out_crop.vpu_crop_area.u32Y;
    chn_attr.crop_area.vpu_crop_area.u32Width = g_vpu_chn_infos[grpid][chan_vpu].chn_out_crop.vpu_crop_area.u32Width;
    chn_attr.crop_area.vpu_crop_area.u32Height = g_vpu_chn_infos[grpid][chan_vpu].chn_out_crop.vpu_crop_area.u32Height;
    chn_attr.stride = 0;
    chn_attr.offset = 0;
    chn_attr.depth = 1;
    ret = FH_VPSS_SetChnAttr(grpid, chan_vpu, &chn_attr);
    if (ret != 0)
    {
        printf("Error(%d - %x): FH_VPSS_SetChnAttr (grp-chn):(%d-%d)!\n", ret, ret, grpid, chan_vpu);
        return ret;
    }

    ret = FH_VPSS_SetVOMode(grpid, chan_vpu, yuv_type);
    if (ret != 0)
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
    if (ret != 0)
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
    case FH_RC_MJPEG_CBR:
    {
        sample_common_set_mjpeg_rc_cbr(info, cfg);
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

    cfg_param.chn_attr.enc_type = FH_NORMAL_H264;
    cfg_param.chn_attr.h264_attr.profile = H264_PROFILE_MAIN;
    cfg_param.chn_attr.h264_attr.i_frame_intterval = 50;
    cfg_param.chn_attr.h264_attr.size.u32Width = info->width;
    cfg_param.chn_attr.h264_attr.size.u32Height = info->height;

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

    cfg_param.chn_attr.enc_type = FH_SMART_H264;
    cfg_param.chn_attr.s264_attr.profile = H264_PROFILE_MAIN;
    cfg_param.chn_attr.s264_attr.refresh_frame_intterval = 50;
    cfg_param.chn_attr.s264_attr.size.u32Width = info->width;
    cfg_param.chn_attr.s264_attr.size.u32Height = info->height;
    cfg_param.chn_attr.s264_attr.smart_en = FH_TRUE;
    cfg_param.chn_attr.s264_attr.texture_en = FH_TRUE;
    cfg_param.chn_attr.s264_attr.backgroudmodel_en = FH_TRUE;
    cfg_param.chn_attr.s264_attr.fresh_ltref_en = FH_FALSE;
    cfg_param.chn_attr.s264_attr.bgm_chn = grpid;

    cfg_param.chn_attr.s264_attr.gop_th.GOP_TH_NUM = 4;
    cfg_param.chn_attr.s264_attr.gop_th.TH_VAL[0] = 8;
    cfg_param.chn_attr.s264_attr.gop_th.TH_VAL[1] = 15;
    cfg_param.chn_attr.s264_attr.gop_th.TH_VAL[2] = 25;
    cfg_param.chn_attr.s264_attr.gop_th.TH_VAL[3] = 35;
    cfg_param.chn_attr.s264_attr.gop_th.MIN_GOP[0] = 380;
    cfg_param.chn_attr.s264_attr.gop_th.MIN_GOP[1] = 330;
    cfg_param.chn_attr.s264_attr.gop_th.MIN_GOP[2] = 270;
    cfg_param.chn_attr.s264_attr.gop_th.MIN_GOP[3] = 220;
    cfg_param.chn_attr.s264_attr.gop_th.MIN_GOP[4] = 160;

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

    cfg_param.chn_attr.enc_type = FH_NORMAL_H265;
    cfg_param.chn_attr.h265_attr.profile = H265_PROFILE_MAIN;
    cfg_param.chn_attr.h265_attr.i_frame_intterval = 50;
    cfg_param.chn_attr.h265_attr.size.u32Width = info->width;
    cfg_param.chn_attr.h265_attr.size.u32Height = info->height;

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

    cfg_param.chn_attr.enc_type = FH_SMART_H265;
    cfg_param.chn_attr.s265_attr.profile = H265_PROFILE_MAIN;
    cfg_param.chn_attr.s265_attr.refresh_frame_intterval = 50;
    cfg_param.chn_attr.s265_attr.size.u32Width = info->width;
    cfg_param.chn_attr.s265_attr.size.u32Height = info->height;
    cfg_param.chn_attr.s265_attr.smart_en = FH_TRUE;
    cfg_param.chn_attr.s265_attr.texture_en = FH_TRUE;
    cfg_param.chn_attr.s265_attr.backgroudmodel_en = FH_TRUE;
    cfg_param.chn_attr.s265_attr.fresh_ltref_en = FH_FALSE;
    cfg_param.chn_attr.s265_attr.bgm_chn = grpid;

    cfg_param.chn_attr.s265_attr.gop_th.GOP_TH_NUM = 4;
    cfg_param.chn_attr.s265_attr.gop_th.TH_VAL[0] = 8;
    cfg_param.chn_attr.s265_attr.gop_th.TH_VAL[1] = 15;
    cfg_param.chn_attr.s265_attr.gop_th.TH_VAL[2] = 25;
    cfg_param.chn_attr.s265_attr.gop_th.TH_VAL[3] = 35;
    cfg_param.chn_attr.s265_attr.gop_th.MIN_GOP[0] = 380;
    cfg_param.chn_attr.s265_attr.gop_th.MIN_GOP[1] = 330;
    cfg_param.chn_attr.s265_attr.gop_th.MIN_GOP[2] = 270;
    cfg_param.chn_attr.s265_attr.gop_th.MIN_GOP[3] = 220;
    cfg_param.chn_attr.s265_attr.gop_th.MIN_GOP[4] = 160;

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
    cfg_param.chn_attr.mjpeg_attr.pic_size.u32Width = info->width;
    cfg_param.chn_attr.mjpeg_attr.pic_size.u32Height = info->height;
    cfg_param.chn_attr.mjpeg_attr.rotate = 0;
    cfg_param.chn_attr.mjpeg_attr.encode_speed = 0; /* 0-9 */

    if (sample_common_set_rc_type(info, &cfg_param))
    {
        sample_common_set_mjpeg_rc_fixqp(info, &cfg_param);
    }

    ret = FH_VENC_SetChnAttr(chan, &cfg_param); // 由于Http只开了一个端口，所以固定为30 chn
    if (ret != 0)
    {
        printf("Error: FH_VENC_SetChnAttr failed with %d\n", ret);
        return -1;
    }
    return 0;
}

static FH_SINT32 sample_common_set_jpeg_chan(FH_UINT32 grpid, FH_UINT32 chan, struct enc_channel_info *info)
{
    FH_VENC_CHN_CONFIG enc_cfg_param;

    enc_cfg_param.chn_attr.enc_type = FH_JPEG;
    enc_cfg_param.chn_attr.jpeg_attr.qp = 50;
    enc_cfg_param.chn_attr.jpeg_attr.rotate = 0;
    enc_cfg_param.chn_attr.jpeg_attr.encode_speed = 0; /* 大幅面下编码器性能不够时，设置为0 */

    return FH_VENC_SetChnAttr(grpid * MAX_VPU_CHN_NUM + chan, &enc_cfg_param);
}

FH_SINT32 sample_common_enc_create_chan(FH_UINT32 grpid, FH_UINT32 chan_enc, struct enc_channel_info *enc_info)
{
    FH_VENC_CHN_CAP cfg_vencmem;

    cfg_vencmem.support_type = enc_info->enc_type;
    cfg_vencmem.max_size.u32Width = enc_info->max_width;
    cfg_vencmem.max_size.u32Height = enc_info->max_height;

    return FH_VENC_CreateChn(grpid * MAX_VPU_CHN_NUM + chan_enc, &cfg_vencmem);
}

static FH_SINT32 sample_common_enc_set_breath(FH_UINT32 grpid, FH_UINT32 chan, FH_UINT16 breath_en)
{
#ifdef CONFIG_ARCH_FH8626V100
    return 0;
#else
    FH_DEBREATH breath;

    breath.debreath_en = breath_en;
    breath.debreath_ratio = 16;

    return FH_VENC_SetDeBreathEffect(grpid * MAX_VPU_CHN_NUM + chan, &breath);
#endif
}

FH_SINT32 sample_common_enc_set_chan(FH_UINT32 grpid, FH_UINT32 chan_enc, struct enc_channel_info *info)
{
    FH_SINT32 ret = 0;

    if(g_vpu_chn_infos[grpid][chan_enc].chn_out_crop.crop_en)
    {
        info->width = g_vpu_chn_infos[grpid][chan_enc].chn_out_crop.vpu_crop_area.u32Width;
        info->height = g_vpu_chn_infos[grpid][chan_enc].chn_out_crop.vpu_crop_area.u32Height;
    }

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

FH_SINT32 sample_common_dmc_init(FH_CHAR *dst_ip, FH_UINT32 port)
{
    return sample_dmc_init(dst_ip, port);
}

FH_SINT32 sample_common_start_send_stream(FH_VOID)
{
    int grploop;
    pthread_attr_t attr;
    pthread_t thread_stream;
    struct sched_param param;

    for (grploop = 0; grploop < MAX_GRP_NUM; grploop++)
    {
        if (g_mem_input_info[grploop].enable)
        {
            pthread_attr_init(&attr);
            pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
            pthread_attr_setstacksize(&attr, 3 * 1024);
#ifdef __RTTHREAD_OS__
            param.sched_priority = 130;
#endif
            pthread_attr_setschedparam(&attr, &param);
            pthread_create(&thread_stream, &attr, sample_common_send_stream_proc, &g_mem_input_info[grploop]);
            g_mem_input_info[grploop].bStart = 1;
            pthread_attr_destroy(&attr);
        }
    }
    return 0;
}

FH_SINT32 sample_common_stop_send_stream(FH_VOID)
{
    int grploop;

    for (grploop = 0; grploop < MAX_GRP_NUM; grploop++)
    {
        if (g_mem_input_info[grploop].enable && g_mem_input_info[grploop].bStart)
        {
            g_mem_input_info[grploop].bStop = 1;
        }
    }

    for (grploop = 0; grploop < MAX_GRP_NUM; grploop++)
    {
        while (g_mem_input_info[grploop].bStop != 0)
            usleep(10 * 1000);
    }

    return 0;
}

FH_SINT32 sample_common_start_get_stream(FH_VOID)
{
    FH_SINT32 grploop;
    struct grp_vpu_info grp_info;
    pthread_attr_t attr;
    pthread_t thread_stream;
    struct sched_param param;

    for (grploop = 0; grploop < MAX_GRP_NUM; grploop++)
    {
        sample_common_dsp_get_grp_info(grploop, &grp_info);
        if (grp_info.enable)
        {
            if (!g_get_stream_running)
            {
                g_get_stream_running = 1;
                g_get_stream_stop = 0;

                pthread_attr_init(&attr);
                pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
                pthread_attr_setschedpolicy(&attr, SCHED_RR);

                param.sched_priority = 55;
#ifdef __RTTHREAD_OS__
                pthread_attr_setstacksize(&attr, 10 * 1024);
                param.sched_priority = 130;
#endif
                pthread_attr_setschedparam(&attr, &param);
                pthread_create(&thread_stream, &attr, sample_common_get_stream_proc, &g_get_stream_stop);
                pthread_attr_destroy(&attr);
                break;
            }
        }
    }

    return 0;
}

FH_SINT32 fbv_common_start_get_stream(FH_VOID)
{
    pthread_attr_t attr;
    pthread_t thread_stream;
    struct sched_param param;

    if (!g_get_stream_running)
    {
        g_get_stream_running = 1;
        g_get_stream_stop = 0;

        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
#ifdef __RTTHREAD_OS__
        pthread_attr_setstacksize(&attr, 3 * 1024);
        param.sched_priority = 130;
#endif
        pthread_attr_setschedparam(&attr, &param);
        pthread_create(&thread_stream, &attr, sample_common_get_stream_proc, &g_get_stream_stop);

    }

    return 0;
}

FH_SINT32 sample_common_stop_get_stream(FH_VOID)
{
    if (g_get_stream_running)
    {
        /*让获取码流的线程退出,置退出标记*/
        g_get_stream_stop = 1;

        /*等待获取码流的线程退出*/
        while (g_get_stream_stop != 0)
            usleep(10 * 1000);

        g_get_stream_running = 0;
    }

    return 0;
}

FH_SINT32 sample_common_dmc_deinit(FH_VOID)
{
    return sample_dmc_deinit();
}

FH_SINT32 sample_common_start_vpu(FH_VOID)
{
    int ret;
    int grploop, vpuloop;
    struct vpu_channel_info *pvpu;
    struct grp_vpu_info *pgrp;

    for (grploop = 0; grploop < MAX_GRP_NUM; grploop++)
    {

        pgrp = &g_vpu_grp_infos[grploop];
        if (pgrp->enable)
        {
            ret = sample_common_dsp_init(grploop);
            if (ret != 0)
            {
                printf("Error(%d - %x): sample_common_dsp_init (grp):(%d)!\n", ret, ret, grploop);
                return -1;
            }

            for (vpuloop = 0; vpuloop < MAX_VPU_CHN_NUM; vpuloop++)
            {
                pvpu = &g_vpu_chn_infos[grploop][vpuloop];
                if (pvpu->enable)
                {
                    ret = sample_common_vpu_create_chan(grploop, vpuloop, pvpu->width, pvpu->height, pvpu->yuv_type);
                    if (ret != 0)
                    {
                        printf("Error(%d - %x): sample_common_vpu_create_chan (grp-chn):(%d-%d)!\n", ret, ret, grploop, vpuloop);
                        return -1;
                    }
                }
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
        pgrp = &g_vpu_grp_infos[grploop];
        if (pgrp->enable)
        {
            for (encloop = 0; encloop < MAX_VPU_CHN_NUM; encloop++)
            {
                penc = &g_enc_chn_infos[grploop][encloop];
                if (penc->enable)
                {
                    ret = sample_common_enc_create_chan(grploop, encloop, penc);
                    if (ret != 0)
                    {
                        printf("Error(%d - %x): sample_common_enc_create_chan (grp-chn):(%d-%d)!\n", ret, ret, grploop, encloop);
                        return -1;
                    }

                    ret = sample_common_enc_set_chan(grploop, encloop, penc);
                    if (ret != 0)
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
    int grploop, jpegloop;
    struct enc_channel_info *pjpeg;
    struct vpu_channel_info *pvpu;
    FH_BIND_INFO src, dst;

    for (grploop = 0; grploop < MAX_GRP_NUM; grploop++)
    {
        for (jpegloop = 0; jpegloop < MAX_VPU_CHN_NUM; jpegloop++)
        {
            pjpeg = &g_jpeg_chn_infos[grploop * MAX_VPU_CHN_NUM + jpegloop];
            if (pjpeg->enable)
            {
                pjpeg->channel = g_jpeg_chn;
                ret = sample_common_enc_create_chan(0, g_jpeg_chn, pjpeg);
                if (ret != 0)
                {
                    printf("Error(%d - %x): jpeg sample_common_enc_create_chan (chn):(%d)!\n", ret, ret, g_jpeg_chn);
                    return -1;
                }

                ret = sample_common_enc_set_chan(0, g_jpeg_chn, pjpeg);
                if (ret != 0)
                {
                    printf("Error(%d - %x): jpeg sample_common_enc_set_chan (chn):(%d)!\n", ret, ret, g_jpeg_chn);
                    return -1;
                }

                ret = FH_VENC_StartRecvPic(g_jpeg_chn);
                if (ret != 0)
                {
                    printf("Error(%d - %x): jpeg FH_VENC_StartRecvPic (chn):(%d)!\n", ret, ret, g_jpeg_chn);
                    return -1;
                }

                pvpu = &g_vpu_chn_infos[grploop][jpegloop];
                if (pvpu->enable)
                {
                    src.obj_id = FH_OBJ_VPU_VO;
                    src.dev_id = grploop;
                    src.chn_id = jpegloop;
                    dst.obj_id = FH_OBJ_JPEG;
                    dst.dev_id = 0;
                    dst.chn_id = g_jpeg_chn;
                    ret = FH_SYS_Bind(src, dst);
                    if (ret != 0)
                    {
                        printf("Error(%d - %x): FH_SYS_Bind VPU to JPEG(%d to %d)\n", ret, ret, grploop * MAX_VPU_CHN_NUM + jpegloop, g_jpeg_chn);
                        return -1;
                    }
                    g_jpeg_chn--;
                }
            }
        }
    }
    return 0;
}