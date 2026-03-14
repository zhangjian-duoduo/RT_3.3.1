#ifndef __CONFIG_H__
#define __CONFIG_H__

#define MAX_VPU_CHN_NUM 4
#define MAX_GRP_NUM 4
#define JPEG_CHN_OFFSET 3
/*Automatically generated make config: don't edit */
/*Linux Kernel Configuration */
/*Wed Sep 21 19:49:27 2022 */

/*EMU is not set */
/*NN_DRAW_BOX is not set */
/*CONFIG_ISP_OFFLINE_MODE is not set */

/*APP config */

#define FH_APP_GRP_ID 0
/*FH_APP_OPEN_OVERLAY is not set */
/*FH_APP_OPEN_VENC is not set */
/*FH_APP_OPEN_ISP_STRATEGY_DEMO is not set */
/*FH_APP_OPEN_MOTION_DETECT is not set */
/*FH_APP_OPEN_RECT_MOTION_DETECT is not set */
/*FH_APP_OPEN_COVER_DETECT is not set */
/*FH_APP_OPEN_AF is not set */
/*FH_APP_OPEN_IVS is not set */
/*FH_APP_OPEN_ABANDON_DETECT is not set */
#define VIDEO_GRP0
/*VPU_MODE_MEM_G0 is not set */
/*VDEC_SEND_G0 is not set */
#define VPU_MODE_ISP_G0

/*ISP Config */

/*FH_APP_USING_IRCUT_G0 is not set */
#define MAX_ISP_WIDTH_G0 0
#define MAX_ISP_HEIGHT_G0 0
#define FH_USING_OVOS02K_MIPI_G0
/*FH_USING_OVOS02D_MIPI_G0 is not set */
/*FH_USING_OVOS04C10_MIPI_G0 is not set */
/*FH_USING_DUMMY_SENSOR_G0 is not set */
/*FH_USING_OVOS05_MIPI_G0 is not set */
/*FH_USING_IMX415_MIPI_G0 is not set */
/*FH_USING_OVOS08_MIPI_G0 is not set */
#define FH_APP_USING_FORMAT_1080P25_G0
/*FH_APP_USING_FORMAT_1080P30_G0 is not set */
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
#define CH0_WIDTH_G0 1920
#define CH0_HEIGHT_G0 1088
/*FH_CH0_USING_SCAN_G0 is not set */
/*FH_CH0_USING_BLK_G0 is not set */
/*FH_CH0_USING_TILE192_G0 is not set */
#define FH_CH0_USING_TILE224_G0
/*FH_CH0_USING_TILE256_G0 is not set */
/*FH_CH0_USING_YUYV_G0 is not set */
#define CH0_BUFNUM_G0 3
#define CH0_BIND_ENC_G0
/*CH0_BIND_NONE_G0 is not set */
/*CH0_MJPEG_G0 is not set */

/*VENC */

#define CH0_BIT_RATE_G0 2048
#define CH0_FRAME_COUNT_G0 25
#define CH0_FRAME_TIME_G0 1
/*FH_CH0_USING_SAMPLE_H264_G0 is not set */
/*FH_CH0_USING_SAMPLE_S264_G0 is not set */
/*FH_CH0_USING_SAMPLE_H265_G0 is not set */
#define FH_CH0_USING_SAMPLE_S265_G0
/*FH_CH0_USING_SAMPLE_NONE_G0 is not set */
/*FH_CH0_OPEN_BREATH_EFFECT_G0 is not set */
#define FH_CH0_USING_SAMPLE_H265_VBR_G0
/*FH_CH0_USING_SAMPLE_H265_FIXQP_G0 is not set */
/*FH_CH0_USING_SAMPLE_H265_CBR_G0 is not set */
/*FH_CH0_USING_SAMPLE_H265_AVBR_G0 is not set */
/*FH_CH0_USING_SAMPLE_H265_CVBR_G0 is not set */
/*FH_CH0_USING_SAMPLE_H265_QVBR_G0 is not set */
/*CH1_ENABLE_G0 is not set */
/*CH2_ENABLE_G0 is not set */
/*CH3_ENABLE_G0 is not set */

/*Network Protocol for sending stream */

#define FH_APP_USING_PES_G0
#define FH_APP_USING_UDP_G0
/*FH_APP_USING_SRT_G0 is not set */
/*FH_APP_USING_RTSP_G0 is not set */
/*FH_APP_RECORD_RAW_STREAM_G0 is not set */
/*VIDEO_GRP1 is not set */
/*VIDEO_GRP2 is not set */
/*FH_APP_USING_COOLVIEW is not set */

#endif
