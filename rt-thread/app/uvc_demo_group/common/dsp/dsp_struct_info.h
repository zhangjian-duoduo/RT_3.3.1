#ifndef __DSP_STRUCT_INFO_H__
#define __DSP_STRUCT_INFO_H__

struct grp_vpu_info g_vpu_grp_infos[MAX_GRP_NUM] = {

    {
        .enable = 0,
        .bgm_enable = 0,
#if (FH_GRP_ID == 0)
        .enable = 1,
        .channel = 0,
#ifdef YCMEAN_EN_G0
        .grp_info.ycmean_ds = YCMEAN_DS_G0,
        .grp_info.ycmean_en = 1,
#endif
        .mode = VPU_MODE_ISP,
#ifdef BGM_ENABLE_G0
        .bgm_enable = 1,
        .bgm_ds = BGM_DS_G0,
#ifdef SAD_ENABLE_G0
        .sad_enable = 1,
#endif
#endif
#ifdef CPY_ENABLE_G0
        .cpy_enable = 1,
#endif
#endif /* VIDEO_GRP0 */
    },

    {
        .enable = 0,
        .bgm_enable = 0,
#if (FH_GRP_ID == 1)
        .enable = 1,
        .channel = 1,
#ifdef YCMEAN_EN_G1
        .grp_info.ycmean_ds = YCMEAN_DS_G1,
        .grp_info.ycmean_en = 1,
#endif
        .mode = VPU_MODE_ISP,
#ifdef BGM_ENABLE_G1
        .bgm_enable = 1,
        .bgm_ds = BGM_DS_G1,
#ifdef SAD_ENABLE_G1
        .sad_enable = 1,
#endif
#endif
#ifdef CPY_ENABLE_G1
        .cpy_enable = 1,
#endif
#endif /* VIDEO_GRP1 */
    },

    {
        .enable = 0,
        .bgm_enable = 0,
#if (FH_GRP_ID == 2)
        .enable = 1,
        .channel = 2,
#ifdef YCMEAN_EN_G2
        .grp_info.ycmean_ds = YCMEAN_DS_G2,
        .grp_info.ycmean_en = 1,
#endif
        .mode = VPU_MODE_ISP,
#ifdef BGM_ENABLE_G2
        .bgm_enable = 1,
        .bgm_ds = BGM_DS_G2,
#ifdef SAD_ENABLE_G2
        .sad_enable = 1,
#endif
#endif
#ifdef CPY_ENABLE_G2
        .cpy_enable = 1,
#endif
#endif /* VIDEO_GRP2 */
    },

};

struct vpu_channel_info g_vpu_chn_infos[MAX_GRP_NUM][MAX_VPU_CHN_NUM] = {

    {
        {
            .enable = 0,
#ifdef CH0_ENABLE
            .enable = 1,
            .channel = 0,
            .width = CH0_WIDTH,
            .height = CH0_HEIGHT,
#if (CH0_MAX_WIDTH > CH0_WIDTH)
            .max_width = CH0_MAX_WIDTH,
#else
            .max_width = CH0_WIDTH,
#endif
#if (CH0_MAX_HEIGHT > CH0_HEIGHT)
            .max_height = CH0_MAX_HEIGHT,
#else
            .max_height = CH0_HEIGHT,
#endif
            .yuv_type = CH0_YUV_TYPE,
            .bufnum = 3,
#endif /*CH0_WIDTH*/
        },

        {
            .enable = 0,
#ifdef CH1_ENABLE
            .enable = 1,
            .channel = 1,
            .width = CH1_WIDTH,
            .height = CH1_HEIGHT,
#if (CH1_MAX_WIDTH > CH1_WIDTH)
            .max_width = CH1_MAX_WIDTH,
#else
            .max_width = CH1_WIDTH,
#endif
#if (CH1_MAX_HEIGHT > CH1_HEIGHT)
            .max_height = CH1_MAX_HEIGHT,
#else
            .max_height = CH1_HEIGHT,
#endif
            .yuv_type = CH1_YUV_TYPE,
            .bufnum = 3,
#endif /*CH1_WIDTH*/
        },

        {
            .enable = 0,
#ifdef CH2_ENABLE
            .enable = 1,
            .channel = 2,
            .width = CH2_WIDTH,
            .height = CH2_HEIGHT,
#if (CH2_MAX_WIDTH > CH2_WIDTH)
            .max_width = CH2_MAX_WIDTH,
#else
            .max_width = CH2_WIDTH,
#endif
#if (CH2_MAX_HEIGHT > CH2_HEIGHT)
            .max_height = CH2_MAX_HEIGHT,
#else
            .max_height = CH2_HEIGHT,
#endif
            .yuv_type = CH2_YUV_TYPE,
#if (defined(CONFIG_ARCH_FH885xV310) || defined(CONFIG_ARCH_FH885xV300)) && defined(CH2_ENABLE)
            .bufnum = 3,
#else
            .bufnum = CH2_BUFNUM,
#endif
#endif /*CH2_WIDTH*/
        },

        {
            .enable = 0,
#ifdef CH3_ENABLE
            .enable = 1,
            .channel = 3,
            .width = CH3_WIDTH,
            .height = CH3_HEIGHT,
#if (CH3_MAX_WIDTH > CH3_WIDTH)
            .max_width = CH3_MAX_WIDTH,
#else
            .max_width = CH3_WIDTH,
#endif
#if (CH3_MAX_HEIGHT > CH3_HEIGHT)
            .max_height = CH3_MAX_HEIGHT,
#else
            .max_height = CH3_HEIGHT,
#endif
            .yuv_type = CH3_YUV_TYPE,
            .bufnum = CH3_BUFNUM,
#endif /*CH3_WIDTH*/
        },
    }, /* group 0 */

    {
        {
            .enable = 0,
#ifdef CH0_ENABLE
            .enable = 1,
            .channel = 0,
            .width = CH0_WIDTH,
            .height = CH0_HEIGHT,
#if (CH0_MAX_WIDTH > CH0_WIDTH)
            .max_width = CH0_MAX_WIDTH,
#else
            .max_width = CH0_WIDTH,
#endif
#if (CH0_MAX_HEIGHT > CH0_HEIGHT)
            .max_height = CH0_MAX_HEIGHT,
#else
            .max_height = CH0_HEIGHT,
#endif
            .yuv_type = CH0_YUV_TYPE,
            .bufnum = 3,
#endif /*CH0_WIDTH*/
        },

        {
            .enable = 0,
#ifdef CH1_ENABLE
            .enable = 1,
            .channel = 1,
            .width = CH1_WIDTH,
            .height = CH1_HEIGHT,
#if (CH1_MAX_WIDTH > CH1_WIDTH)
            .max_width = CH1_MAX_WIDTH,
#else
            .max_width = CH1_WIDTH,
#endif
#if (CH1_MAX_HEIGHT > CH1_HEIGHT)
            .max_height = CH1_MAX_HEIGHT,
#else
            .max_height = CH1_HEIGHT,
#endif
            .yuv_type = CH1_YUV_TYPE,
            .bufnum = 3,
#endif /*CH1_WIDTH*/
        },

        {
            .enable = 0,
#ifdef CH2_ENABLE
            .enable = 1,
            .channel = 2,
            .width = CH2_WIDTH,
            .height = CH2_HEIGHT,
#if (CH2_MAX_WIDTH > CH2_WIDTH)
            .max_width = CH2_MAX_WIDTH,
#else
            .max_width = CH2_WIDTH,
#endif
#if (CH2_MAX_HEIGHT > CH2_HEIGHT)
            .max_height = CH2_MAX_HEIGHT,
#else
            .max_height = CH2_HEIGHT,
#endif
            .yuv_type = CH2_YUV_TYPE,
            .bufnum = 3,
#endif /*CH2_WIDTH*/
        },

        {
            .enable = 0,
#ifdef CH3_ENABLE
            .enable = 1,
            .channel = 3,
            .width = CH3_WIDTH,
            .height = CH3_HEIGHT,
#if (CH3_MAX_WIDTH > CH3_WIDTH)
            .max_width = CH3_MAX_WIDTH,
#else
            .max_width = CH3_WIDTH,
#endif
#if (CH3_MAX_HEIGHT > CH3_HEIGHT)
            .max_height = CH3_MAX_HEIGHT,
#else
            .max_height = CH3_HEIGHT,
#endif
            .yuv_type = CH3_YUV_TYPE,
            .bufnum = 3,
#endif /*CH3_WIDTH*/
        },
    }, /* group 1 */

    {
        {
            .enable = 0,
#ifdef CH0_ENABLE
            .enable = 1,
            .channel = 0,
            .width = CH0_WIDTH,
            .height = CH0_HEIGHT,
#if (CH0_MAX_WIDTH > CH0_WIDTH)
            .max_width = CH0_MAX_WIDTH,
#else
            .max_width = CH0_WIDTH,
#endif
#if (CH0_MAX_HEIGHT > CH0_HEIGHT)
            .max_height = CH0_MAX_HEIGHT,
#else
            .max_height = CH0_HEIGHT,
#endif
            .yuv_type = CH0_YUV_TYPE,
            .bufnum = 3,
#endif /*CH0_WIDTH*/
        },

        {
            .enable = 0,
#ifdef CH1_ENABLE
            .enable = 1,
            .channel = 1,
            .width = CH1_WIDTH,
            .height = CH1_HEIGHT,
#if (CH1_MAX_WIDTH > CH1_WIDTH)
            .max_width = CH1_MAX_WIDTH,
#else
            .max_width = CH1_WIDTH,
#endif
#if (CH1_MAX_HEIGHT > CH1_HEIGHT)
            .max_height = CH1_MAX_HEIGHT,
#else
            .max_height = CH1_HEIGHT,
#endif
            .yuv_type = CH1_YUV_TYPE,
            .bufnum = 3,
#endif /*CH1_WIDTH*/
        },

        {
            .enable = 0,
#ifdef CH2_ENABLE
            .enable = 1,
            .channel = 2,
            .width = CH2_WIDTH,
            .height = CH2_HEIGHT,
#if (CH2_MAX_WIDTH > CH2_WIDTH)
            .max_width = CH2_MAX_WIDTH,
#else
            .max_width = CH2_WIDTH,
#endif
#if (CH2_MAX_HEIGHT > CH2_HEIGHT)
            .max_height = CH2_MAX_HEIGHT,
#else
            .max_height = CH2_HEIGHT,
#endif
            .yuv_type = CH2_YUV_TYPE,
            .bufnum = 3,
#endif /*CH2_WIDTH*/
        },

        {
            .enable = 0,
#ifdef CH3_ENABLE
            .enable = 1,
            .channel = 3,
            .width = CH3_WIDTH,
            .height = CH3_HEIGHT,
#if (CH3_MAX_WIDTH > CH3_WIDTH)
            .max_width = CH3_MAX_WIDTH,
#else
            .max_width = CH3_WIDTH,
#endif
#if (CH3_MAX_HEIGHT > CH3_HEIGHT)
            .max_height = CH3_MAX_HEIGHT,
#else
            .max_height = CH3_HEIGHT,
#endif
            .yuv_type = CH3_YUV_TYPE,
            .bufnum = 3,
#endif /*CH3_WIDTH*/
        },
    }, /* group 2 */

};

struct enc_channel_info g_enc_chn_infos[MAX_GRP_NUM][MAX_VPU_CHN_NUM] = {
    {
        {
            .enable = 0,
#ifdef CH0_ENABLE
            .enable = 1,
            .channel = 0,
            .width = CH0_WIDTH,
            .height = CH0_HEIGHT,
#if (CH0_MAX_WIDTH > CH0_WIDTH)
            .max_width = CH0_MAX_WIDTH,
#else
            .max_width = CH0_WIDTH,
#endif
#if (CH0_MAX_HEIGHT > CH0_HEIGHT)
            .max_height = CH0_MAX_HEIGHT,
#else
            .max_height = CH0_HEIGHT,
#endif
            .frame_count = CH0_FRAME_COUNT,
            .frame_time = CH0_FRAME_TIME,
            .bps = CH0_BIT_RATE * 1024,
            .enc_type = CH0_ENC_TYPE,
            .rc_type = CH0_RC_TYPE,
            .breath_on = 0,
#endif /*CH0_BIND_ENC_G0*/
        },

        {
            .enable = 0,
#ifdef CH1_ENABLE
            .enable = 1,
            .channel = 1,
            .width = CH1_WIDTH,
            .height = CH1_HEIGHT,
#if (CH1_MAX_WIDTH > CH1_WIDTH)
            .max_width = CH1_MAX_WIDTH,
#else
            .max_width = CH1_WIDTH,
#endif
#if (CH1_MAX_HEIGHT > CH1_HEIGHT)
            .max_height = CH1_MAX_HEIGHT,
#else
            .max_height = CH1_HEIGHT,
#endif
            .frame_count = CH1_FRAME_COUNT,
            .frame_time = CH1_FRAME_TIME,
            .bps = CH1_BIT_RATE * 1024,
            .enc_type = CH1_ENC_TYPE,
            .rc_type = CH1_RC_TYPE,
            .breath_on = 0,
#endif /*CH1_BIND_ENC_G0*/
        },

        {
            .enable = 0,
#if (defined(CONFIG_ARCH_FH885xV310) || defined(CONFIG_ARCH_FH885xV300)) && defined(CH2_ENABLE)
            .enable = 1,
            .channel = 2,
            .width = CH2_WIDTH,
            .height = CH2_HEIGHT,
#if (CH2_MAX_WIDTH > CH2_WIDTH)
            .max_width = CH2_MAX_WIDTH,
#else
            .max_width = CH2_WIDTH,
#endif
#if (CH2_MAX_HEIGHT > CH2_HEIGHT)
            .max_height = CH2_MAX_HEIGHT,
#else
            .max_height = CH2_HEIGHT,
#endif
            .frame_count = CH2_FRAME_COUNT,
            .frame_time = CH2_FRAME_TIME,
            .bps = CH2_BIT_RATE * 1024,
            .enc_type = CH2_ENC_TYPE,
            .rc_type = CH2_RC_TYPE,
            .breath_on = 0,
#endif /*CH2_BIND_ENC_G0*/
        },

        {
            .enable = 0,
#if 0
            .enable = 1,
            .channel = 3,
            .width = CH3_WIDTH,
            .height = CH3_HEIGHT,
#if (CH3_MAX_WIDTH > CH3_WIDTH)
            .max_width = CH3_MAX_WIDTH,
#else
            .max_width = CH3_WIDTH,
#endif
#if (CH3_MAX_HEIGHT > CH3_HEIGHT)
            .max_height = CH3_MAX_HEIGHT,
#else
            .max_height = CH3_HEIGHT,
#endif
            .frame_count = CH3_FRAME_COUNT,
            .frame_time = CH3_FRAME_TIME,
            .bps = CH3_BIT_RATE * 1024,
            .enc_type = CH3_ENC_TYPE,
            .rc_type = CH3_RC_TYPE,
            .breath_on = 0,
#endif /*CH3_BIND_ENC_G0*/
        },
    }, /* G0 */
    {
        {
            .enable = 0,
#ifdef CH0_ENABLE
            .enable = 1,
            .channel = 0,
            .width = CH0_WIDTH,
            .height = CH0_HEIGHT,
#if (CH0_MAX_WIDTH > CH0_WIDTH)
            .max_width = CH0_MAX_WIDTH,
#else
            .max_width = CH0_WIDTH,
#endif
#if (CH0_MAX_HEIGHT > CH0_HEIGHT)
            .max_height = CH0_MAX_HEIGHT,
#else
            .max_height = CH0_HEIGHT,
#endif
            .frame_count = CH0_FRAME_COUNT,
            .frame_time = CH0_FRAME_TIME,
            .bps = CH0_BIT_RATE * 1024,
            .enc_type = CH0_ENC_TYPE,
            .rc_type = CH0_RC_TYPE,
            .breath_on = 0,
#endif /*CH0_BIND_ENC_G1*/
        },

        {
            .enable = 0,
#ifdef CH1_ENABLE
            .enable = 1,
            .channel = 1,
            .width = CH1_WIDTH,
            .height = CH1_HEIGHT,
#if (CH1_MAX_WIDTH > CH1_WIDTH)
            .max_width = CH1_MAX_WIDTH,
#else
            .max_width = CH1_WIDTH,
#endif
#if (CH1_MAX_HEIGHT > CH1_HEIGHT)
            .max_height = CH1_MAX_HEIGHT,
#else
            .max_height = CH1_HEIGHT,
#endif
            .frame_count = CH1_FRAME_COUNT,
            .frame_time = CH1_FRAME_TIME,
            .bps = CH1_BIT_RATE * 1024,
            .enc_type = CH1_ENC_TYPE,
            .rc_type = CH1_RC_TYPE,
            .breath_on = 0,
#endif /*CH1_BIND_ENC_G1*/
        },

        {
            .enable = 0,
#if (defined(CONFIG_ARCH_FH885xV310) || defined(CONFIG_ARCH_FH885xV300)) && defined(CH2_ENABLE)
            .enable = 1,
            .channel = 2,
            .width = CH2_WIDTH,
            .height = CH2_HEIGHT,
#if (CH2_MAX_WIDTH > CH2_WIDTH)
            .max_width = CH2_MAX_WIDTH,
#else
            .max_width = CH2_WIDTH,
#endif
#if (CH2_MAX_HEIGHT > CH2_HEIGHT)
            .max_height = CH2_MAX_HEIGHT,
#else
            .max_height = CH2_HEIGHT,
#endif
            .frame_count = CH2_FRAME_COUNT,
            .frame_time = CH2_FRAME_TIME,
            .bps = CH2_BIT_RATE * 1024,
            .enc_type = CH2_ENC_TYPE,
            .rc_type = CH2_RC_TYPE,
            .breath_on = 0,
#endif /*CH2_BIND_ENC_G1*/
        },

        {
            .enable = 0,
#if 0
            .enable = 1,
            .channel = 3,
            .width = CH3_WIDTH,
            .height = CH3_HEIGHT,
#if (CH3_MAX_WIDTH > CH3_WIDTH)
            .max_width = CH3_MAX_WIDTH,
#else
            .max_width = CH3_WIDTH,
#endif
#if (CH3_MAX_HEIGHT > CH3_HEIGHT)
            .max_height = CH3_MAX_HEIGHT,
#else
            .max_height = CH3_HEIGHT,
#endif
            .frame_count = CH3_FRAME_COUNT,
            .frame_time = CH3_FRAME_TIME,
            .bps = CH3_BIT_RATE * 1024,
            .enc_type = CH3_ENC_TYPE,
            .rc_type = CH3_RC_TYPE,
            .breath_on = 0,
#endif /*CH3_BIND_ENC_G1*/
        },
    }, /* G1 */
    {
        {
            .enable = 0,
#ifdef CH0_ENABLE
            .enable = 1,
            .channel = 0,
            .width = CH0_WIDTH,
            .height = CH0_HEIGHT,
#if (CH0_MAX_WIDTH > CH0_WIDTH)
            .max_width = CH0_MAX_WIDTH,
#else
            .max_width = CH0_WIDTH,
#endif
#if (CH0_MAX_HEIGHT > CH0_HEIGHT)
            .max_height = CH0_MAX_HEIGHT,
#else
            .max_height = CH0_HEIGHT,
#endif
            .frame_count = CH0_FRAME_COUNT,
            .frame_time = CH0_FRAME_TIME,
            .bps = CH0_BIT_RATE * 1024,
            .enc_type = CH0_ENC_TYPE,
            .rc_type = CH0_RC_TYPE,
            .breath_on = 0,
#endif /*CH0_BIND_ENC_G2*/
        },

        {
            .enable = 0,
#ifdef CH1_ENABLE
            .enable = 1,
            .channel = 1,
            .width = CH1_WIDTH,
            .height = CH1_HEIGHT,
#if (CH1_MAX_WIDTH > CH1_WIDTH)
            .max_width = CH1_MAX_WIDTH,
#else
            .max_width = CH1_WIDTH,
#endif
#if (CH1_MAX_HEIGHT > CH1_HEIGHT)
            .max_height = CH1_MAX_HEIGHT,
#else
            .max_height = CH1_HEIGHT,
#endif
            .frame_count = CH1_FRAME_COUNT,
            .frame_time = CH1_FRAME_TIME,
            .bps = CH1_BIT_RATE * 1024,
            .enc_type = CH1_ENC_TYPE,
            .rc_type = CH1_RC_TYPE,
            .breath_on = 0,
#endif /*CH1_BIND_ENC_G2*/
        },

        {
            .enable = 0,
#if (defined(CONFIG_ARCH_FH885xV310) || defined(CONFIG_ARCH_FH885xV300)) && defined(CH2_ENABLE)
            .enable = 1,
            .channel = 2,
            .width = CH2_WIDTH,
            .height = CH2_HEIGHT,
#if (CH2_MAX_WIDTH > CH2_WIDTH)
            .max_width = CH2_MAX_WIDTH,
#else
            .max_width = CH2_WIDTH,
#endif
#if (CH2_MAX_HEIGHT > CH2_HEIGHT)
            .max_height = CH2_MAX_HEIGHT,
#else
            .max_height = CH2_HEIGHT,
#endif
            .frame_count = CH2_FRAME_COUNT,
            .frame_time = CH2_FRAME_TIME,
            .bps = CH2_BIT_RATE * 1024,
            .enc_type = CH2_ENC_TYPE,
            .rc_type = CH2_RC_TYPE,
            .breath_on = 0,
#endif /*CH2_BIND_ENC_G2*/
        },

        {
            .enable = 0,
#if 0
            .enable = 1,
            .channel = 3,
            .width = CH3_WIDTH,
            .height = CH3_HEIGHT,
#if (CH3_MAX_WIDTH > CH3_WIDTH)
            .max_width = CH3_MAX_WIDTH,
#else
            .max_width = CH3_WIDTH,
#endif
#if (CH3_MAX_HEIGHT > CH3_HEIGHT)
            .max_height = CH3_MAX_HEIGHT,
#else
            .max_height = CH3_HEIGHT,
#endif
            .frame_count = CH3_FRAME_COUNT,
            .frame_time = CH3_FRAME_TIME,
            .bps = CH3_BIT_RATE * 1024,
            .enc_type = CH3_ENC_TYPE,
            .rc_type = CH3_RC_TYPE,
            .breath_on = 0,
#endif /*CH3_BIND_ENC_G2*/
        },
    }, /* G2 */
};

struct enc_channel_info g_jpeg_chn_infos[MAX_GRP_NUM * MAX_VPU_CHN_NUM] = {
    {
        .enable = 0,
#if defined(CH0_ENABLE)
        .enable = 1,
        .channel = 0,
        .width = CH0_WIDTH,
        .height = CH0_HEIGHT,
#if (CH0_MAX_WIDTH > CH0_WIDTH)
        .max_width = CH0_MAX_WIDTH,
#else
        .max_width = CH0_WIDTH,
#endif
#if (CH0_MAX_HEIGHT > CH0_HEIGHT)
        .max_height = CH0_MAX_HEIGHT,
#else
        .max_height = CH0_HEIGHT,
#endif
        .frame_count = CH0_FRAME_COUNT,
        .frame_time = CH0_FRAME_TIME,
        .bps = CH0_BIT_RATE * 1024,
        .enc_type = FH_MJPEG,
        .rc_type = CH0_RC_TYPE,
        .breath_on = 0,
#endif /*CH0_ENABLE*/
    },
    {
        .enable = 0,
#if defined(CH1_ENABLE)
        .enable = 1,
        .channel = 0,
        .width = CH1_WIDTH,
        .height = CH1_HEIGHT,
#if (CH1_MAX_WIDTH > CH1_WIDTH)
        .max_width = CH1_MAX_WIDTH,
#else
        .max_width = CH1_WIDTH,
#endif
#if (CH1_MAX_HEIGHT > CH1_HEIGHT)
        .max_height = CH1_MAX_HEIGHT,
#else
        .max_height = CH1_HEIGHT,
#endif
        .frame_count = CH1_FRAME_COUNT,
        .frame_time = CH1_FRAME_TIME,
        .bps = CH1_BIT_RATE * 1024,
        .enc_type = FH_MJPEG,
        .rc_type = CH1_RC_TYPE,
        .breath_on = 0,
#endif /*CH1_ENABLE*/
    },
    {
        .enable = 0,
#if (defined(CONFIG_ARCH_FH885xV310) || defined(CONFIG_ARCH_FH885xV300)) && defined(CH2_ENABLE)
        .enable = 1,
        .channel = 0,
        .width = CH2_WIDTH,
        .height = CH2_HEIGHT,
#if (CH2_MAX_WIDTH > CH2_WIDTH)
        .max_width = CH2_MAX_WIDTH,
#else
        .max_width = CH2_WIDTH,
#endif
#if (CH2_MAX_HEIGHT > CH2_HEIGHT)
        .max_height = CH2_MAX_HEIGHT,
#else
        .max_height = CH2_HEIGHT,
#endif
        .frame_count = CH2_FRAME_COUNT,
        .frame_time = CH2_FRAME_TIME,
        .bps = CH2_BIT_RATE * 1024,
        .enc_type = FH_MJPEG,
        .rc_type = CH2_RC_TYPE,
        .breath_on = 0,
#endif /*CH2_ENABLE*/
    },
    {
        .enable = 0,
#if 0
        .enable = 1,
        .channel = 0,
        .width = CH3_WIDTH,
        .height = CH3_HEIGHT,
#if (CH3_MAX_WIDTH > CH3_WIDTH)
        .max_width = CH3_MAX_WIDTH,
#else
        .max_width = CH3_WIDTH,
#endif
#if (CH3_MAX_HEIGHT > CH3_HEIGHT)
        .max_height = CH3_MAX_HEIGHT,
#else
        .max_height = CH3_HEIGHT,
#endif
        .frame_count = CH3_FRAME_COUNT,
        .frame_time = CH3_FRAME_TIME,
        .bps = CH3_BIT_RATE * 1024,
        .enc_type = FH_MJPEG,
        .rc_type = CH3_RC_TYPE,
        .breath_on = 0,
#endif /*CH3_MJPEG_G0*/
    },
    /*****************************G0*****************************/

    {
        .enable = 0,
#if defined(CH0_ENABLE)
        .enable = 1,
        .channel = 0,
        .width = CH0_WIDTH,
        .height = CH0_HEIGHT,
#if (CH0_MAX_WIDTH > CH0_WIDTH)
        .max_width = CH0_MAX_WIDTH,
#else
        .max_width = CH0_WIDTH,
#endif
#if (CH0_MAX_HEIGHT > CH0_HEIGHT)
        .max_height = CH0_MAX_HEIGHT,
#else
        .max_height = CH0_HEIGHT,
#endif
        .frame_count = CH0_FRAME_COUNT,
        .frame_time = CH0_FRAME_TIME,
        .bps = CH0_BIT_RATE * 1024,
        .enc_type = FH_MJPEG,
        .rc_type = CH0_RC_TYPE,
        .breath_on = 0,
#endif /*CH0_MJPEG_G1*/
    },
    {
        .enable = 0,
#if defined(CH1_ENABLE)
        .enable = 1,
        .channel = 0,
        .width = CH1_WIDTH,
        .height = CH1_HEIGHT,
#if (CH1_MAX_WIDTH > CH1_WIDTH)
        .max_width = CH1_MAX_WIDTH,
#else
        .max_width = CH1_WIDTH,
#endif
#if (CH1_MAX_HEIGHT > CH1_HEIGHT)
        .max_height = CH1_MAX_HEIGHT,
#else
        .max_height = CH1_HEIGHT,
#endif
        .frame_count = CH1_FRAME_COUNT,
        .frame_time = CH1_FRAME_TIME,
        .bps = CH1_BIT_RATE * 1024,
        .enc_type = FH_MJPEG,
        .rc_type = CH1_RC_TYPE,
        .breath_on = 0,
#endif /*CH1_MJPEG_G1*/
    },
    {
        .enable = 0,
#if (defined(CONFIG_ARCH_FH885xV310) || defined(CONFIG_ARCH_FH885xV300)) && defined(CH2_ENABLE)
        .enable = 1,
        .channel = 0,
        .width = CH2_WIDTH,
        .height = CH2_HEIGHT,
#if (CH2_MAX_WIDTH > CH2_WIDTH)
        .max_width = CH2_MAX_WIDTH,
#else
        .max_width = CH2_WIDTH,
#endif
#if (CH2_MAX_HEIGHT > CH2_HEIGHT)
        .max_height = CH2_MAX_HEIGHT,
#else
        .max_height = CH2_HEIGHT,
#endif
        .frame_count = CH2_FRAME_COUNT,
        .frame_time = CH2_FRAME_TIME,
        .bps = CH2_BIT_RATE * 1024,
        .enc_type = FH_MJPEG,
        .rc_type = CH2_RC_TYPE,
        .breath_on = 0,
#endif /*CH2_MJPEG_G1*/
    },
    {
        .enable = 0,
#if 0
        .enable = 1,
        .channel = 0,
        .width = CH3_WIDTH,
        .height = CH3_HEIGHT,
#if (CH3_MAX_WIDTH > CH3_WIDTH)
        .max_width = CH3_MAX_WIDTH,
#else
        .max_width = CH3_WIDTH,
#endif
#if (CH3_MAX_HEIGHT > CH3_HEIGHT)
        .max_height = CH3_MAX_HEIGHT,
#else
        .max_height = CH3_HEIGHT,
#endif
        .frame_count = CH3_FRAME_COUNT,
        .frame_time = CH3_FRAME_TIME,
        .bps = CH3_BIT_RATE * 1024,
        .enc_type = FH_MJPEG,
        .rc_type = CH3_RC_TYPE,
        .breath_on = 0,
#endif /*CH3_MJPEG_G1*/
    },
    /*****************************G1*****************************/

    {
        .enable = 0,
#if defined(CH0_ENABLE)
        .enable = 1,
        .channel = 0,
        .width = CH0_WIDTH,
        .height = CH0_HEIGHT,
#if (CH0_MAX_WIDTH > CH0_WIDTH)
        .max_width = CH0_MAX_WIDTH,
#else
        .max_width = CH0_WIDTH,
#endif
#if (CH0_MAX_HEIGHT > CH0_HEIGHT)
        .max_height = CH0_MAX_HEIGHT,
#else
        .max_height = CH0_HEIGHT,
#endif
        .frame_count = CH0_FRAME_COUNT,
        .frame_time = CH0_FRAME_TIME,
        .bps = CH0_BIT_RATE * 1024,
        .enc_type = FH_MJPEG,
        .rc_type = CH0_RC_TYPE,
        .breath_on = 0,
#endif /*CH0_MJPEG_G2*/
    },
    {
        .enable = 0,
#if defined(CH1_ENABLE)
        .enable = 1,
        .channel = 0,
        .width = CH1_WIDTH,
        .height = CH1_HEIGHT,
#if (CH1_MAX_WIDTH > CH1_WIDTH)
        .max_width = CH1_MAX_WIDTH,
#else
        .max_width = CH1_WIDTH,
#endif
#if (CH1_MAX_HEIGHT > CH1_HEIGHT)
        .max_height = CH1_MAX_HEIGHT,
#else
        .max_height = CH1_HEIGHT,
#endif
        .frame_count = CH1_FRAME_COUNT,
        .frame_time = CH1_FRAME_TIME,
        .bps = CH1_BIT_RATE * 1024,
        .enc_type = FH_MJPEG,
        .rc_type = CH1_RC_TYPE,
        .breath_on = 0,
#endif /*CH1_MJPEG_G2*/
    },
    {
        .enable = 0,
#if (defined(CONFIG_ARCH_FH885xV310) || defined(CONFIG_ARCH_FH885xV300)) && defined(CH2_ENABLE)
        .enable = 1,
        .channel = 0,
        .width = CH2_WIDTH,
        .height = CH2_HEIGHT,
#if (CH2_MAX_WIDTH > CH2_WIDTH)
        .max_width = CH2_MAX_WIDTH,
#else
        .max_width = CH2_WIDTH,
#endif
#if (CH2_MAX_HEIGHT > CH2_HEIGHT)
        .max_height = CH2_MAX_HEIGHT,
#else
        .max_height = CH2_HEIGHT,
#endif
        .frame_count = CH2_FRAME_COUNT,
        .frame_time = CH2_FRAME_TIME,
        .bps = CH2_BIT_RATE * 1024,
        .enc_type = FH_MJPEG,
        .rc_type = CH2_RC_TYPE,
        .breath_on = 0,
#endif /*CH2_MJPEG_G2*/
    },
    {
        .enable = 0,
#if 0
        .enable = 1,
        .channel = 0,
        .width = CH3_WIDTH,
        .height = CH3_HEIGHT,
#if (CH3_MAX_WIDTH > CH3_WIDTH)
        .max_width = CH3_MAX_WIDTH,
#else
        .max_width = CH3_WIDTH,
#endif
#if (CH3_MAX_HEIGHT > CH3_HEIGHT)
        .max_height = CH3_MAX_HEIGHT,
#else
        .max_height = CH3_HEIGHT,
#endif
        .frame_count = CH3_FRAME_COUNT,
        .frame_time = CH3_FRAME_TIME,
        .bps = CH3_BIT_RATE * 1024,
        .enc_type = FH_MJPEG,
        .rc_type = CH3_RC_TYPE,
        .breath_on = 0,
#endif /*CH3_MJPEG_G2*/
    },
    /*****************************G2*****************************/
};

struct vpu_send_param g_mem_input_info[MAX_GRP_NUM] = {
    {
        .enable = 0,
        .bStop = 0,
        .bStart = 0,
#ifdef VPU_MODE_MEM_G0
        .enable = 1,
        .u32Grp = 0,
        .SendFrameInfo.u32Width = MEM_IN_WIDTH_G0,
        .SendFrameInfo.u32Height = MEM_IN_HEIGHT_G0,
        .SendFrameInfo.u32StrideY = MEM_IN_WIDTH_G0,
        .SendFrameInfo.u32StrideUV = MEM_IN_HEIGHT_G0,
        .data_format = MEM_IN_TYPE_G0,
        .time_stamp = MEM_IN_TIMESTAMP_G0,
#ifdef MEM_IN_YUVFILE_G0
        .SendFrameInfo.yuvfile = MEM_IN_YUVFILE_G0,
        .SendFrameInfo.yfile = NULL,
        .SendFrameInfo.uvfile = NULL,
#else
        .SendFrameInfo.yuvfile = NULL,
        .SendFrameInfo.yfile = MEM_IN_YFILE_G0,
        .SendFrameInfo.uvfile = MEM_IN_UVFILE_G0,
#endif
#endif
    },

    {
        .enable = 0,
        .bStop = 0,
        .bStart = 0,
#ifdef VPU_MODE_MEM_G1
        .enable = 1,
        .u32Grp = 1,
        .SendFrameInfo.u32Width = MEM_IN_WIDTH_G1,
        .SendFrameInfo.u32Height = MEM_IN_HEIGHT_G1,
        .SendFrameInfo.u32StrideY = MEM_IN_WIDTH_G1,
        .data_format = MEM_IN_TYPE_G1,
        .time_stamp = MEM_IN_TIMESTAMP_G1,
#ifdef MEM_IN_YUVFILE_G1
        .SendFrameInfo.yuvfile = MEM_IN_YUVFILE_G1,
        .SendFrameInfo.yfile = NULL,
        .SendFrameInfo.uvfile = NULL,
#else
        .SendFrameInfo.yuvfile = NULL,
        .SendFrameInfo.yfile = MEM_IN_YFILE_G1,
        .SendFrameInfo.uvfile = MEM_IN_UVFILE_G1,
#endif
#endif
    },

    {
        .enable = 0,
        .bStop = 0,
        .bStart = 0,
#ifdef VPU_MODE_MEM_G2
        .enable = 1,
        .u32Grp = 2,
        .SendFrameInfo.u32Width = MEM_IN_WIDTH_G2,
        .SendFrameInfo.u32Height = MEM_IN_HEIGHT_G2,
        .SendFrameInfo.u32StrideY = MEM_IN_WIDTH_G2,
        .data_format = MEM_IN_TYPE_G2,
        .time_stamp = MEM_IN_TIMESTAMP_G2,
#ifdef MEM_IN_YUVFILE_G2
        .SendFrameInfo.yuvfile = MEM_IN_YUVFILE_G2,
        .SendFrameInfo.yfile = NULL,
        .SendFrameInfo.uvfile = NULL,
#else
        .SendFrameInfo.yuvfile = NULL,
        .SendFrameInfo.yfile = MEM_IN_YFILE_G2,
        .SendFrameInfo.uvfile = MEM_IN_UVFILE_G2,
#endif
#endif
    },
};

#endif /* __DSP_STRUCT_INFO_H__ */
