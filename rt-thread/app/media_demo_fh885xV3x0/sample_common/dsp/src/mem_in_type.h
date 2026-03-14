#ifndef __mem_in_type_h__
#define __mem_in_type_h__

#ifdef VIDEO_GRP0

#ifdef MEM_IN_MODE_NV16_G0
#define MEM_IN_TYPE_G0 VPU_VOMODE_NV16
#endif

#ifdef MEM_IN_MODE_YUYV_G0
#define MEM_IN_TYPE_G0 VPU_VOMODE_YUYV
#endif

#ifdef MEM_IN_MODE_UYVY_G0
#define MEM_IN_TYPE_G0 VPU_VOMODE_UYVY
#endif

#ifdef MEM_IN_MODE_NV12_10BIT_G0
#define MEM_IN_TYPE_G0 VPU_VOMODE_10BIT_NV12
#endif

#endif /* VIDEO_GRP0 */
#ifdef VIDEO_GRP1

#ifdef MEM_IN_MODE_NV16_G1
#define MEM_IN_TYPE_G1 VPU_VOMODE_NV16
#endif

#ifdef MEM_IN_MODE_YUYV_G1
#define MEM_IN_TYPE_G1 VPU_VOMODE_YUYV
#endif

#ifdef MEM_IN_MODE_UYVY_G1
#define MEM_IN_TYPE_G1 VPU_VOMODE_UYVY
#endif

#ifdef MEM_IN_MODE_NV12_10BIT_G1
#define MEM_IN_TYPE_G1 VPU_VOMODE_10BIT_NV12
#endif

#endif /* VIDEO_GRP1 */
#ifdef VIDEO_GRP2

#ifdef MEM_IN_MODE_NV16_G2
#define MEM_IN_TYPE_G2 VPU_VOMODE_NV16
#endif

#ifdef MEM_IN_MODE_YUYV_G2
#define MEM_IN_TYPE_G2 VPU_VOMODE_YUYV
#endif

#ifdef MEM_IN_MODE_UYVY_G2
#define MEM_IN_TYPE_G2 VPU_VOMODE_UYVY
#endif

#ifdef MEM_IN_MODE_NV12_10BIT_G2
#define MEM_IN_TYPE_G2 VPU_VOMODE_10BIT_NV12
#endif

#endif /* VIDEO_GRP2 */

#endif /* __chn_yuv_type_h__ */
