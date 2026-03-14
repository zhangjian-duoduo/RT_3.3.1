#ifndef __chn_yuv_type_h__
#define __chn_yuv_type_h__

/*说明; 此文件的目的是定义这三个宏,CH0_YUV_TYPE, CH1_YUV_TYPE, CH2_YUV_TYPE*/

#ifdef VIDEO_GRP0

#ifdef FH_CH0_USING_SCAN_G0
#define CH0_YUV_TYPE_G0 VPU_VOMODE_SCAN
#endif

#ifdef FH_CH1_USING_SCAN_G0
#define CH1_YUV_TYPE_G0 VPU_VOMODE_SCAN
#endif

#ifdef FH_CH2_USING_RGB888_G0
#define CH2_YUV_TYPE_G0 VPU_VOMODE_RGB888
#endif

#endif /* VIDEO_GRP0 */

#ifdef VIDEO_GRP1

#ifdef FH_CH0_USING_SCAN_G1
#define CH0_YUV_TYPE_G1 VPU_VOMODE_SCAN
#endif

#ifdef FH_CH1_USING_SCAN_G1
#define CH1_YUV_TYPE_G1 VPU_VOMODE_SCAN
#endif

#ifdef FH_CH2_USING_RGB888_G1
#define CH2_YUV_TYPE_G1 VPU_VOMODE_RGB888
#endif

#endif /* VIDEO_GRP1 */

#endif /*__chn_yuv_type_h__*/
