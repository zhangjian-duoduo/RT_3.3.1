#ifndef __CHN_SET_RC_H__
#define __CHN_SET_RC_H__

FH_VOID sample_common_set_h264_rc_fixqp(struct enc_channel_info *info, FH_VENC_CHN_CONFIG *cfg);
FH_VOID sample_common_set_h264_rc_cbr(struct enc_channel_info *info, FH_VENC_CHN_CONFIG *cfg);
FH_VOID sample_common_set_h264_rc_vbr(struct enc_channel_info *info, FH_VENC_CHN_CONFIG *cfg);
FH_VOID sample_common_set_h264_rc_avbr(struct enc_channel_info *info, FH_VENC_CHN_CONFIG *cfg);
FH_VOID sample_common_set_h264_rc_cvbr(struct enc_channel_info *info, FH_VENC_CHN_CONFIG *cfg);

FH_VOID sample_common_set_h265_rc_fixqp(struct enc_channel_info *info, FH_VENC_CHN_CONFIG *cfg);
FH_VOID sample_common_set_h265_rc_cbr(struct enc_channel_info *info, FH_VENC_CHN_CONFIG *cfg);
FH_VOID sample_common_set_h265_rc_vbr(struct enc_channel_info *info, FH_VENC_CHN_CONFIG *cfg);
FH_VOID sample_common_set_h265_rc_avbr(struct enc_channel_info *info, FH_VENC_CHN_CONFIG *cfg);
FH_VOID sample_common_set_h265_rc_cvbr(struct enc_channel_info *info, FH_VENC_CHN_CONFIG *cfg);

FH_VOID sample_common_set_h264_rc_qvbr(struct enc_channel_info *info, FH_VENC_CHN_CONFIG *cfg);
FH_VOID sample_common_set_h265_rc_qvbr(struct enc_channel_info *info, FH_VENC_CHN_CONFIG *cfg);

FH_VOID sample_common_set_mjpeg_rc_fixqp(struct enc_channel_info *info, FH_VENC_CHN_CONFIG *cfg);
FH_VOID sample_common_set_mjpeg_rc_cbr(struct enc_channel_info *info, FH_VENC_CHN_CONFIG *cfg);
FH_VOID sample_common_set_mjpeg_rc_vbr(struct enc_channel_info *info, FH_VENC_CHN_CONFIG *cfg);

#endif /* __CHN_SET_RC_H__ */
