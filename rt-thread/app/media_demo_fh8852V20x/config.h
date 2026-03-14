#ifndef __CONFIG_H__
#define __CONFIG_H__

#define MAX_VPU_CHN_NUM 3
#define MAX_GRP_NUM 2
#define JPEG_CHN_OFFSET 3
/*Automatically generated make config: don't edit */
/*Linux Kernel Configuration */
/*Mon Jun 26 11:12:26 2023 */

/*EMU is not set */
/*NN_DRAW_BOX is not set */

/*APP config */

#define FH_APP_GRP_ID 0
#define FH_APP_OPEN_OVERLAY
#define FH_APP_OVERLAY_LOGO
#define FH_APP_OVERLAY_TOSD
/*FH_APP_OPEN_VENC is not set */
/*FH_APP_OPEN_ISP_STRATEGY_DEMO is not set */
/*FH_APP_OPEN_MOTION_DETECT is not set */
/*FH_APP_OPEN_RECT_MOTION_DETECT is not set */
/*FH_APP_OPEN_COVER_DETECT is not set */
/*FH_APP_OPEN_AF is not set */
/*FH_APP_OPEN_IVS is not set */
/*FH_APP_OPEN_ABANDON_DETECT is not set */
#define VIDEO_GRP0
#define VPU_MODE_ISP_G0

/*ISP Config */

#define FH_APP_USING_IRCUT_G0
#define MAX_ISP_WIDTH_G0 0
#define MAX_ISP_HEIGHT_G0 0
#define FH_USING_OVOS02K_MIPI_G0
/*FH_USING_SC3335_MIPI_G0 is not set */
/*FH_APP_USING_FORMAT_720P25_G0 is not set */
/*FH_APP_USING_FORMAT_1080P25_G0 is not set */
#define FH_APP_USING_FORMAT_1080P30_G0
/*FH_APP_USING_FORMAT_1080P25_WDR_G0 is not set */
/*FH_APP_USING_FORMAT_1080P30_WDR_G0 is not set */

/*VPU Config */

#define CH0_ENABLE_G0
#define YCMEAN_EN_G0
#define YCMEAN_DS_G0 16
#define BGM_ENABLE_G0
#define BGM_DS_G0 8
#define CPY_ENABLE_G0
#define SAD_ENABLE_G0
#define CH0_MAX_WIDTH_G0 0
#define CH0_MAX_HEIGHT_G0 0
#define CH0_WIDTH_G0 640
#define CH0_HEIGHT_G0 480
#define FH_CH0_USING_SCAN_G0
#define CH0_BUFNUM_G0 3
#define CH0_BIND_ENC_G0
/*CH0_MJPEG_G0 is not set */

/*VENC */

#define CH0_BIT_RATE_G0 2048
#define CH0_FRAME_COUNT_G0 25
#define CH0_FRAME_TIME_G0 1
/*FH_CH0_USING_SAMPLE_H264_G0 is not set */
/*FH_CH0_USING_SAMPLE_S264_G0 is not set */
/*FH_CH0_USING_SAMPLE_H265_G0 is not set */
#define FH_CH0_USING_SAMPLE_S265_G0
/*FH_CH0_OPEN_BREATH_EFFECT_G0 is not set */
#define FH_CH0_USING_SAMPLE_H265_VBR_G0
/*FH_CH0_USING_SAMPLE_H265_FIXQP_G0 is not set */
/*FH_CH0_USING_SAMPLE_H265_CBR_G0 is not set */
/*FH_CH0_USING_SAMPLE_H265_AVBR_G0 is not set */
/*FH_CH0_USING_SAMPLE_H265_CVBR_G0 is not set */
/*FH_CH0_USING_SAMPLE_H265_QVBR_G0 is not set */
#define CH1_ENABLE_G0
#define CH1_MAX_WIDTH_G0 0
#define CH1_MAX_HEIGHT_G0 0
#define CH1_WIDTH_G0 720
#define CH1_HEIGHT_G0 576
#define FH_CH1_USING_SCAN_G0
#define CH1_BUFNUM_G0 3
#define CH1_BIND_ENC_G0
/*CH1_MJPEG_G0 is not set */

/*VENC */

#define CH1_BIT_RATE_G0 1024
#define CH1_FRAME_COUNT_G0 25
#define CH1_FRAME_TIME_G0 1
#define FH_CH1_USING_SAMPLE_H264_G0
/*FH_CH1_USING_SAMPLE_S264_G0 is not set */
/*FH_CH1_USING_SAMPLE_H265_G0 is not set */
/*FH_CH1_USING_SAMPLE_S265_G0 is not set */
/*FH_CH1_USING_SAMPLE_MJPEG_G0 is not set */
/*FH_CH1_OPEN_BREATH_EFFECT_G0 is not set */
#define FH_CH1_USING_SAMPLE_H264_VBR_G0
/*FH_CH1_USING_SAMPLE_H264_FIXQP_G0 is not set */
/*FH_CH1_USING_SAMPLE_H264_CBR_G0 is not set */
/*FH_CH1_USING_SAMPLE_H264_AVBR_G0 is not set */
/*FH_CH1_USING_SAMPLE_H264_CVBR_G0 is not set */
/*FH_CH1_USING_SAMPLE_H264_QVBR_G0 is not set */
/*CH2_ENABLE_G0 is not set */

/*Network Protocol for sending stream */

#define FH_APP_USING_PES_G0
#define FH_APP_USING_UDP_G0
/*FH_APP_USING_SRT_G0 is not set */
/*FH_APP_USING_RTSP_G0 is not set */
/*FH_APP_RECORD_RAW_STREAM_G0 is not set */
/*VIDEO_GRP1 is not set */
/*FH_APP_USING_COOLVIEW is not set */

#endif
