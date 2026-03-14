#ifndef __CHN_BIND_TYPE_H__
#define __CHN_BIND_TYPE_H__

/*****************************VPU********************************/

#ifdef FH_CH0_USING_SAMPLE_MJPEG
#define CH0_BIND_TYPE FH_OBJ_JPEG
#else
#define CH0_BIND_TYPE FH_OBJ_ENC
#endif

#ifdef FH_CH1_USING_SAMPLE_MJPEG
#define CH1_BIND_TYPE FH_OBJ_JPEG
#else
#define CH1_BIND_TYPE FH_OBJ_ENC
#endif

#ifdef FH_CH2_USING_SAMPLE_MJPEG
#define CH2_BIND_TYPE FH_OBJ_JPEG
#else
#define CH2_BIND_TYPE FH_OBJ_ENC
#endif

#ifdef FH_CH3_USING_SAMPLE_MJPEG
#define CH3_BIND_TYPE FH_OBJ_JPEG
#else
#define CH3_BIND_TYPE FH_OBJ_ENC
#endif

/*****************************ENC********************************/

#ifndef CH0_ENC_BIND_TYPE
#define CH0_ENC_BIND_TYPE FH_OBJ_NONE
#endif

#ifndef CH1_ENC_BIND_TYPE
#define CH1_ENC_BIND_TYPE FH_OBJ_NONE
#endif

#ifndef CH2_ENC_BIND_TYPE
#define CH2_ENC_BIND_TYPE FH_OBJ_NONE
#endif

#ifndef CH3_ENC_BIND_TYPE
#define CH3_ENC_BIND_TYPE FH_OBJ_NONE
#endif

/*****************************VGS********************************/

#ifdef CH0_VGS_BIND_HD
#define CH0_VGS_BIND_TYPE VO_LAYER_VHD0
#endif

#ifdef CH0_VGS_BIND_SD
#define CH0_VGS_BIND_TYPE VO_LAYER_VSD0
#endif

#ifdef CH0_VGS_BIND_PIP
#define CH0_VGS_BIND_TYPE VO_LAYER_VPIP
#endif

#ifndef CH0_VGS_BIND_TYPE
#define CH0_VGS_BIND_TYPE FH_OBJ_NONE
#endif

#ifdef CH1_VGS_BIND_HD
#define CH1_VGS_BIND_TYPE VO_LAYER_VHD0
#endif

#ifdef CH1_VGS_BIND_SD
#define CH1_VGS_BIND_TYPE VO_LAYER_VSD0
#endif

#ifdef CH1_VGS_BIND_PIP
#define CH1_VGS_BIND_TYPE VO_LAYER_VPIP
#endif

#ifndef CH1_VGS_BIND_TYPE
#define CH1_VGS_BIND_TYPE FH_OBJ_NONE
#endif

#ifdef CH2_VGS_BIND_HD
#define CH2_VGS_BIND_TYPE VO_LAYER_VHD0
#endif

#ifdef CH2_VGS_BIND_SD
#define CH2_VGS_BIND_TYPE VO_LAYER_VSD0
#endif

#ifdef CH2_VGS_BIND_PIP
#define CH2_VGS_BIND_TYPE VO_LAYER_VPIP
#endif

#ifndef CH2_VGS_BIND_TYPE
#define CH2_VGS_BIND_TYPE FH_OBJ_NONE
#endif

#ifdef CH3_VGS_BIND_HD
#define CH3_VGS_BIND_TYPE VO_LAYER_VHD0
#endif

#ifdef CH3_VGS_BIND_SD
#define CH3_VGS_BIND_TYPE VO_LAYER_VSD0
#endif

#ifdef CH3_VGS_BIND_PIP
#define CH3_VGS_BIND_TYPE VO_LAYER_VPIP
#endif

#ifndef CH3_VGS_BIND_TYPE
#define CH3_VGS_BIND_TYPE FH_OBJ_NONE
#endif

/*****************************VDEC********************************/

#ifdef CH0_VDEC_BIND_VGS
#define CH0_VDEC_BIND_TYPE FH_OBJ_VPPU
#endif

#ifdef CH0_VDEC_BIND_HD
#define CH0_VDEC_BIND_TYPE VO_LAYER_VHD0
#endif

#ifdef CH0_VDEC_BIND_SD
#define CH0_VDEC_BIND_TYPE VO_LAYER_VSD0
#endif

#ifdef CH0_VDEC_BIND_PIP
#define CH0_VDEC_BIND_TYPE VO_LAYER_VPIP
#endif

#ifndef CH0_VDEC_BIND_TYPE
#define CH0_VDEC_BIND_TYPE FH_OBJ_NONE
#endif

#ifdef CH1_VDEC_BIND_VGS
#define CH1_VDEC_BIND_TYPE FH_OBJ_VPPU
#endif

#ifdef CH1_VDEC_BIND_HD
#define CH1_VDEC_BIND_TYPE VO_LAYER_VHD0
#endif

#ifdef CH1_VDEC_BIND_SD
#define CH1_VDEC_BIND_TYPE VO_LAYER_VSD0
#endif

#ifdef CH1_VDEC_BIND_PIP
#define CH1_VDEC_BIND_TYPE VO_LAYER_VPIP
#endif

#ifndef CH1_VDEC_BIND_TYPE
#define CH1_VDEC_BIND_TYPE FH_OBJ_NONE
#endif

#ifdef CH2_VDEC_BIND_VGS
#define CH2_VDEC_BIND_TYPE FH_OBJ_VPPU
#endif

#ifdef CH2_VDEC_BIND_HD
#define CH2_VDEC_BIND_TYPE VO_LAYER_VHD0
#endif

#ifdef CH2_VDEC_BIND_SD
#define CH2_VDEC_BIND_TYPE VO_LAYER_VSD0
#endif

#ifdef CH2_VDEC_BIND_PIP
#define CH2_VDEC_BIND_TYPE VO_LAYER_VPIP
#endif

#ifndef CH2_VDEC_BIND_TYPE
#define CH2_VDEC_BIND_TYPE FH_OBJ_NONE
#endif

#ifdef CH3_VDEC_BIND_VGS
#define CH3_VDEC_BIND_TYPE FH_OBJ_VPPU
#endif

#ifdef CH3_VDEC_BIND_HD
#define CH3_VDEC_BIND_TYPE VO_LAYER_VHD0
#endif

#ifdef CH3_VDEC_BIND_SD
#define CH3_VDEC_BIND_TYPE VO_LAYER_VSD0
#endif

#ifdef CH3_VDEC_BIND_PIP
#define CH3_VDEC_BIND_TYPE VO_LAYER_VPIP
#endif

#ifndef CH3_VDEC_BIND_TYPE
#define CH3_VDEC_BIND_TYPE FH_OBJ_NONE
#endif

#endif /* __CHN_BIND_TYPE_H__ */
