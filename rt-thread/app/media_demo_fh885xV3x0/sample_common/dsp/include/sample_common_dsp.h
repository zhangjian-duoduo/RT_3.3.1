#ifndef __SAMPLE_COMMON_DSP_H__
#define __SAMPLE_COMMON_DSP_H__

#include "dsp/fh_system_mpi.h"
#include "dsp/fh_vpu_mpi.h"
#include "dsp/fh_venc_mpi.h"
#include "mpp/fh_vb_mpi.h"
#include "mpp/fh_vb_mpipara.h"

#define ALIGNTO(addr, edge) ((addr + edge - 1) & ~(edge - 1)) /* 数据结构对齐定义 */

struct grp_vpu_info
{
    FH_SINT32 channel;
    FH_SINT32 enable;
    FH_VPU_SET_GRP_INFO grp_info;
    FH_VPU_VI_MODE mode;
    FH_UINT32 bgm_enable;
    FH_UINT32 cpy_enable;
    FH_UINT32 sad_enable;
    FH_UINT32 bgm_ds;
    FH_VPU_CROP vpu_vi_crop;

};

struct vpu_channel_info
{
    FH_SINT32 channel;
    FH_SINT32 enable;
    FH_UINT32 width;
    FH_UINT32 height;
    FH_UINT32 max_width;
    FH_UINT32 max_height;
    FH_UINT32 yuv_type;
    FH_UINT32 bufnum;
    FH_VPU_CROP chn_out_crop;
};

struct enc_channel_info
{
    FH_SINT32 channel;
    FH_SINT32 enable;
    FH_UINT32 width;
    FH_UINT32 height;
    FH_UINT32 max_width;
    FH_UINT32 max_height;
    FH_UINT8 frame_count;
    FH_UINT8 frame_time;
    FH_UINT32 bps;
    FH_UINT32 enc_type;
    FH_UINT32 rc_type;
    FH_UINT16 breath_on;
};

struct vpu_send_frm_info
{
    FH_UINT32 u32Width;
    FH_UINT32 u32Height;
    FH_UINT32 u32StrideY;
    FH_UINT32 u32StrideUV;
    FH_CHAR *yfile;
    FH_CHAR *uvfile;
    FH_CHAR *yuvfile;
};

struct vpu_send_param
{
    FH_BOOL bStop;
    FH_BOOL bStart;
    FH_BOOL enable;
    FH_UINT32 u32Grp;
    FH_UINT64 time_stamp;       // 时间戳 | [ ]
    FH_VPU_VO_MODE data_format; // DDR输入格式,支持格式参考开发手册 | [ ]
    struct vpu_send_frm_info SendFrameInfo;
};

extern FH_VOID sample_common_dsp_get_grp_info(FH_UINT32 chan_vpu, struct grp_vpu_info *info);
extern FH_VOID sample_common_dsp_get_vpu_chn_info(FH_UINT32 grpid, FH_UINT32 chan_vpu, struct vpu_channel_info *info);
extern FH_VOID sample_common_dsp_get_enc_chn_info(FH_UINT32 grpid, FH_UINT32 chan_vpu, struct enc_channel_info *info);

extern FH_VOID sample_common_media_driver_config(FH_UINT32 grpid);
extern FH_SINT32 sample_common_media_sys_init(FH_VOID);
extern FH_SINT32 sample_common_media_sys_exit(FH_VOID);
extern FH_SINT32 sample_common_dsp_init(FH_UINT32 grpid);
extern FH_SINT32 sample_common_vpu_create_chan(FH_UINT32 grpid, FH_UINT32 chan_vpu, FH_UINT32 width, FH_UINT32 height, FH_UINT32 yuv_type);
extern FH_SINT32 sample_common_vpu_bind_to_enc(FH_UINT32 grpid, FH_UINT32 chan_vpu, FH_UINT32 chan_enc);

extern FH_SINT32 sample_common_enc_create_chan(FH_UINT32 grpid, FH_UINT32 chan_enc, struct enc_channel_info *info);
extern FH_SINT32 sample_common_enc_set_chan(FH_UINT32 grpid, FH_UINT32 chan_enc, struct enc_channel_info *info);

extern FH_SINT32 sample_common_start_capture_jpeg(FH_VOID);
extern FH_SINT32 sample_common_stop_capture_jpeg(FH_VOID);

extern FH_SINT32 sample_common_start_get_stream(FH_VOID);
extern FH_SINT32 sample_common_stop_get_stream(FH_VOID);

extern FH_SINT32 sample_common_start_send_stream(FH_VOID);
extern FH_SINT32 sample_common_stop_send_stream(FH_VOID);

extern FH_SINT32 sample_common_dmc_init(FH_CHAR *dst_ip, FH_UINT32 port);
extern FH_SINT32 sample_common_dmc_deinit(FH_VOID);

extern FH_SINT32 sample_common_start_enc(FH_VOID);
extern FH_SINT32 sample_common_start_vpu(FH_VOID);
extern FH_SINT32 sample_common_start_mjpeg(FH_VOID);

extern FH_SINT32 sample_common_set_merge_attr(FH_UINT32 grpid);
extern FH_SINT32 sample_common_init_brc_chn(FH_SINT32 grpid);

extern FH_UINT32 get_dual_camera_ll_vi_w(FH_VOID);
extern FH_UINT32 get_dual_camera_rr_vi_w(FH_VOID);
extern FH_UINT32 get_dual_camera_ll_rr_vi_h(FH_VOID);

extern FH_UINT32 get_vpu_vi_w(FH_UINT32 grp_id);
extern FH_UINT32 get_vpu_vi_h(FH_UINT32 grp_id);

extern FH_UINT32 get_vpu_chn_w(FH_UINT32 grp_id, FH_UINT32 chn_id);
extern FH_UINT32 get_vpu_chn_h(FH_UINT32 grp_id, FH_UINT32 chn_id);

#endif /*__SAMPLE_COMMON_DSP_H__*/
