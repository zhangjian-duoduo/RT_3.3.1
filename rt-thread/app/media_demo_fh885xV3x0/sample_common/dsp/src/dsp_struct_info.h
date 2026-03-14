#ifndef __DSP_STRUCT_INFO_H__
#define __DSP_STRUCT_INFO_H__

struct grp_vpu_info g_vpu_grp_infos[MAX_GRP_NUM] = {

    {
        .enable = 0,
        .bgm_enable = 0,
#if defined(VPU_MODE_MEM_G0) || defined(VPU_MODE_ISP_G0)
        .enable = 1,
        .channel = 0,
#ifdef YCMEAN_EN_G0
        .grp_info.ycmean_ds = YCMEAN_DS_G0,
        .grp_info.ycmean_en = 1,
#endif
        .mode = VPU_INMODE_TYPE_G0,
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
#ifdef VPU_VI_CROP_EN_G0
        .vpu_vi_crop.crop_en = 1,
        .vpu_vi_crop.vpu_crop_area.u32X = VPU_CROP_AREA_X_G0,
        .vpu_vi_crop.vpu_crop_area.u32Y = VPU_VI_CROP_AREA_Y_G0,
        .vpu_vi_crop.vpu_crop_area.u32Width = VPU_VI_CROP_AREA_WIDTH_G0,
        .vpu_vi_crop.vpu_crop_area.u32Height = VPU_VI_CROP_AREA_HEIGHT_G0,
#else
        .vpu_vi_crop.crop_en = 0,
        .vpu_vi_crop.vpu_crop_area.u32X = 0,
        .vpu_vi_crop.vpu_crop_area.u32Y = 0,
        .vpu_vi_crop.vpu_crop_area.u32Width = 0,
        .vpu_vi_crop.vpu_crop_area.u32Height = 0,
#endif
#endif /* VIDEO_GRP0 */
    },

    {
        .enable = 0,
        .bgm_enable = 0,
#if defined(VPU_MODE_MEM_G1) || defined(VPU_MODE_ISP_G1)
        .enable = 1,
        .channel = 1,
#ifdef YCMEAN_EN_G1
        .grp_info.ycmean_ds = YCMEAN_DS_G1,
        .grp_info.ycmean_en = 1,
#endif
        .mode = VPU_INMODE_TYPE_G1,
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
#ifdef VPU_VI_CROP_EN_G1
        .vpu_vi_crop.crop_en = 1,
        .vpu_vi_crop.vpu_crop_area.u32X = VPU_CROP_AREA_X_G1,
        .vpu_vi_crop.vpu_crop_area.u32Y = VPU_VI_CROP_AREA_Y_G1,
        .vpu_vi_crop.vpu_crop_area.u32Width = VPU_VI_CROP_AREA_WIDTH_G1,
        .vpu_vi_crop.vpu_crop_area.u32Height = VPU_VI_CROP_AREA_HEIGHT_G1,
#else
        .vpu_vi_crop.crop_en = 0,
        .vpu_vi_crop.vpu_crop_area.u32X = 0,
        .vpu_vi_crop.vpu_crop_area.u32Y = 0,
        .vpu_vi_crop.vpu_crop_area.u32Width = 0,
        .vpu_vi_crop.vpu_crop_area.u32Height = 0,
#endif
#endif /* VIDEO_GRP1 */
    },

    {
        .enable = 0,
        .bgm_enable = 0,
#if defined(VPU_MODE_MEM_G2) || defined(VPU_MODE_ISP_G2)
        .enable = 1,
        .channel = 2,
#ifdef YCMEAN_EN_G2
        .grp_info.ycmean_ds = YCMEAN_DS_G2,
        .grp_info.ycmean_en = 1,
#endif
        .mode = VPU_INMODE_TYPE_G2,
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
#ifdef CH0_ENABLE_G0
            .enable = 1,
            .channel = 0,
            .width = CH0_WIDTH_G0,
            .height = CH0_HEIGHT_G0,
#if (CH0_MAX_WIDTH_G0 > CH0_WIDTH_G0)
            .max_width = CH0_MAX_WIDTH_G0,
#else
            .max_width = CH0_WIDTH_G0,
#endif
#if (CH0_MAX_HEIGHT_G0 > CH0_HEIGHT_G0)
            .max_height = CH0_MAX_HEIGHT_G0,
#else
            .max_height = CH0_HEIGHT_G0,
#endif
            .yuv_type = CH0_YUV_TYPE_G0,
            .bufnum = CH0_BUFNUM_G0,
#ifdef CH0_CROP_EN_G0
            .chn_out_crop.crop_en = 1,
            .chn_out_crop.vpu_crop_area.u32X = CH0_CROP_AREA_X_G0,
            .chn_out_crop.vpu_crop_area.u32Y = CH0_CROP_AREA_Y_G0,
            .chn_out_crop.vpu_crop_area.u32Width = CH0_CROP_AREA_WIDTH_G0,
            .chn_out_crop.vpu_crop_area.u32Height = CH0_CROP_AREA_HEIGHT_G0,
#else
            .chn_out_crop.crop_en = 0,
            .chn_out_crop.vpu_crop_area.u32X = 0,
            .chn_out_crop.vpu_crop_area.u32Y = 0,
            .chn_out_crop.vpu_crop_area.u32Width = 0,
            .chn_out_crop.vpu_crop_area.u32Height = 0,
#endif
#endif /*CH0_WIDTH_G0*/
        },

        {
            .enable = 0,
#ifdef CH1_ENABLE_G0
            .enable = 1,
            .channel = 1,
            .width = CH1_WIDTH_G0,
            .height = CH1_HEIGHT_G0,
#if (CH1_MAX_WIDTH_G0 > CH1_WIDTH_G0)
            .max_width = CH1_MAX_WIDTH_G0,
#else
            .max_width = CH1_WIDTH_G0,
#endif
#if (CH1_MAX_HEIGHT_G0 > CH1_HEIGHT_G0)
            .max_height = CH1_MAX_HEIGHT_G0,
#else
            .max_height = CH1_HEIGHT_G0,
#endif
            .yuv_type = CH1_YUV_TYPE_G0,
            .bufnum = CH1_BUFNUM_G0,
#ifdef CH1_CROP_EN_G0
            .chn_out_crop.crop_en = 1,
            .chn_out_crop.vpu_crop_area.u32X = CH1_CROP_AREA_X_G0,
            .chn_out_crop.vpu_crop_area.u32Y = CH1_CROP_AREA_Y_G0,
            .chn_out_crop.vpu_crop_area.u32Width = CH1_CROP_AREA_WIDTH_G0,
            .chn_out_crop.vpu_crop_area.u32Height = CH1_CROP_AREA_HEIGHT_G0,
#else
            .chn_out_crop.crop_en = 0,
            .chn_out_crop.vpu_crop_area.u32X = 0,
            .chn_out_crop.vpu_crop_area.u32Y = 0,
            .chn_out_crop.vpu_crop_area.u32Width = 0,
            .chn_out_crop.vpu_crop_area.u32Height = 0,
#endif
#endif /*CH1_WIDTH_G0*/
        },

        {
            .enable = 0,
#ifdef CH2_ENABLE_G0
            .enable = 1,
            .channel = 2,
            .width = CH2_WIDTH_G0,
            .height = CH2_HEIGHT_G0,
#if (CH2_MAX_WIDTH_G0 > CH2_WIDTH_G0)
            .max_width = CH2_MAX_WIDTH_G0,
#else
            .max_width = CH2_WIDTH_G0,
#endif
#if (CH2_MAX_HEIGHT_G0 > CH2_HEIGHT_G0)
            .max_height = CH2_MAX_HEIGHT_G0,
#else
            .max_height = CH2_HEIGHT_G0,
#endif
            .yuv_type = CH2_YUV_TYPE_G0,
            .bufnum = CH2_BUFNUM_G0,
#ifdef CH2_CROP_EN_G0
            .chn_out_crop.crop_en = 1,
            .chn_out_crop.vpu_crop_area.u32X = CH2_CROP_AREA_X_G0,
            .chn_out_crop.vpu_crop_area.u32Y = CH2_CROP_AREA_Y_G0,
            .chn_out_crop.vpu_crop_area.u32Width = CH2_CROP_AREA_WIDTH_G0,
            .chn_out_crop.vpu_crop_area.u32Height = CH2_CROP_AREA_HEIGHT_G0,
#else
            .chn_out_crop.crop_en = 0,
            .chn_out_crop.vpu_crop_area.u32X = 0,
            .chn_out_crop.vpu_crop_area.u32Y = 0,
            .chn_out_crop.vpu_crop_area.u32Width = 0,
            .chn_out_crop.vpu_crop_area.u32Height = 0,
#endif
#endif /*CH2_WIDTH_G0*/
        },

        {
            .enable = 0,
#ifdef CH3_ENABLE_G0
            .enable = 1,
            .channel = 3,
            .width = CH3_WIDTH_G0,
            .height = CH3_HEIGHT_G0,
#if (CH3_MAX_WIDTH_G0 > CH3_WIDTH_G0)
            .max_width = CH3_MAX_WIDTH_G0,
#else
            .max_width = CH3_WIDTH_G0,
#endif
#if (CH3_MAX_HEIGHT_G0 > CH3_HEIGHT_G0)
            .max_height = CH3_MAX_HEIGHT_G0,
#else
            .max_height = CH3_HEIGHT_G0,
#endif
            .yuv_type = CH3_YUV_TYPE_G0,
            .bufnum = CH3_BUFNUM_G0,
#endif /*CH3_WIDTH_G0*/
        },
    }, /* group 0 */

    {
        {
            .enable = 0,
#ifdef CH0_ENABLE_G1
            .enable = 1,
            .channel = 0,
            .width = CH0_WIDTH_G1,
            .height = CH0_HEIGHT_G1,
#if (CH0_MAX_WIDTH_G1 > CH0_WIDTH_G1)
            .max_width = CH0_MAX_WIDTH_G1,
#else
            .max_width = CH0_WIDTH_G1,
#endif
#if (CH0_MAX_HEIGHT_G1 > CH0_HEIGHT_G1)
            .max_height = CH0_MAX_HEIGHT_G1,
#else
            .max_height = CH0_HEIGHT_G1,
#endif
            .yuv_type = CH0_YUV_TYPE_G1,
            .bufnum = CH0_BUFNUM_G1,
#ifdef CH0_CROP_EN_G1
            .chn_out_crop.crop_en = 1,
            .chn_out_crop.vpu_crop_area.u32X = CH0_CROP_AREA_X_G1,
            .chn_out_crop.vpu_crop_area.u32Y = CH0_CROP_AREA_Y_G1,
            .chn_out_crop.vpu_crop_area.u32Width = CH0_CROP_AREA_WIDTH_G1,
            .chn_out_crop.vpu_crop_area.u32Height = CH0_CROP_AREA_HEIGHT_G1,
#else
            .chn_out_crop.crop_en = 0,
            .chn_out_crop.vpu_crop_area.u32X = 0,
            .chn_out_crop.vpu_crop_area.u32Y = 0,
            .chn_out_crop.vpu_crop_area.u32Width = 0,
            .chn_out_crop.vpu_crop_area.u32Height = 0,
#endif
#endif /*CH0_WIDTH_G1*/
        },

        {
            .enable = 0,
#ifdef CH1_ENABLE_G1
            .enable = 1,
            .channel = 1,
            .width = CH1_WIDTH_G1,
            .height = CH1_HEIGHT_G1,
#if (CH1_MAX_WIDTH_G1 > CH1_WIDTH_G1)
            .max_width = CH1_MAX_WIDTH_G1,
#else
            .max_width = CH1_WIDTH_G1,
#endif
#if (CH1_MAX_HEIGHT_G1 > CH1_HEIGHT_G1)
            .max_height = CH1_MAX_HEIGHT_G1,
#else
            .max_height = CH1_HEIGHT_G1,
#endif
            .yuv_type = CH1_YUV_TYPE_G1,
            .bufnum = CH1_BUFNUM_G1,
#ifdef CH1_CROP_EN_G0
            .chn_out_crop.crop_en = 1,
            .chn_out_crop.vpu_crop_area.u32X = CH1_CROP_AREA_X_G1,
            .chn_out_crop.vpu_crop_area.u32Y = CH1_CROP_AREA_Y_G1,
            .chn_out_crop.vpu_crop_area.u32Width = CH1_CROP_AREA_WIDTH_G1,
            .chn_out_crop.vpu_crop_area.u32Height = CH1_CROP_AREA_HEIGHT_G1,
#else
            .chn_out_crop.crop_en = 0,
            .chn_out_crop.vpu_crop_area.u32X = 0,
            .chn_out_crop.vpu_crop_area.u32Y = 0,
            .chn_out_crop.vpu_crop_area.u32Width = 0,
            .chn_out_crop.vpu_crop_area.u32Height = 0,
#endif
#endif /*CH1_WIDTH_G1*/
        },

        {
            .enable = 0,
#ifdef CH2_ENABLE_G1
            .enable = 1,
            .channel = 2,
            .width = CH2_WIDTH_G1,
            .height = CH2_HEIGHT_G1,
#if (CH2_MAX_WIDTH_G1 > CH2_WIDTH_G1)
            .max_width = CH2_MAX_WIDTH_G1,
#else
            .max_width = CH2_WIDTH_G1,
#endif
#if (CH2_MAX_HEIGHT_G1 > CH2_HEIGHT_G1)
            .max_height = CH2_MAX_HEIGHT_G1,
#else
            .max_height = CH2_HEIGHT_G1,
#endif
            .yuv_type = CH2_YUV_TYPE_G1,
            .bufnum = CH2_BUFNUM_G1,
#ifdef CH2_CROP_EN_G1
            .chn_out_crop.crop_en = 1,
            .chn_out_crop.vpu_crop_area.u32X = CH2_CROP_AREA_X_G1,
            .chn_out_crop.vpu_crop_area.u32Y = CH2_CROP_AREA_Y_G1,
            .chn_out_crop.vpu_crop_area.u32Width = CH2_CROP_AREA_WIDTH_G1,
            .chn_out_crop.vpu_crop_area.u32Height = CH2_CROP_AREA_HEIGHT_G1,
#else
            .chn_out_crop.crop_en = 0,
            .chn_out_crop.vpu_crop_area.u32X = 0,
            .chn_out_crop.vpu_crop_area.u32Y = 0,
            .chn_out_crop.vpu_crop_area.u32Width = 0,
            .chn_out_crop.vpu_crop_area.u32Height = 0,
#endif
#endif /*CH2_WIDTH_G1*/
        },

        {
            .enable = 0,
#ifdef CH3_ENABLE_G1
            .enable = 1,
            .channel = 3,
            .width = CH3_WIDTH_G1,
            .height = CH3_HEIGHT_G1,
#if (CH3_MAX_WIDTH_G1 > CH3_WIDTH_G1)
            .max_width = CH3_MAX_WIDTH_G1,
#else
            .max_width = CH3_WIDTH_G1,
#endif
#if (CH3_MAX_HEIGHT_G1 > CH3_HEIGHT_G1)
            .max_height = CH3_MAX_HEIGHT_G1,
#else
            .max_height = CH3_HEIGHT_G1,
#endif
            .yuv_type = CH3_YUV_TYPE_G1,
            .bufnum = CH3_BUFNUM_G1,
#endif /*CH3_WIDTH_G1*/
        },
    }, /* group 1 */

    {
        {
            .enable = 0,
#ifdef CH0_ENABLE_G2
            .enable = 1,
            .channel = 0,
            .width = CH0_WIDTH_G2,
            .height = CH0_HEIGHT_G2,
#if (CH0_MAX_WIDTH_G2 > CH0_WIDTH_G2)
            .max_width = CH0_MAX_WIDTH_G2,
#else
            .max_width = CH0_WIDTH_G2,
#endif
#if (CH0_MAX_HEIGHT_G2 > CH0_HEIGHT_G2)
            .max_height = CH0_MAX_HEIGHT_G2,
#else
            .max_height = CH0_HEIGHT_G2,
#endif
            .yuv_type = CH0_YUV_TYPE_G2,
            .bufnum = CH0_BUFNUM_G2,
#endif /*CH0_WIDTH_G2*/
        },

        {
            .enable = 0,
#ifdef CH1_ENABLE_G2
            .enable = 1,
            .channel = 1,
            .width = CH1_WIDTH_G2,
            .height = CH1_HEIGHT_G2,
#if (CH1_MAX_WIDTH_G2 > CH1_WIDTH_G2)
            .max_width = CH1_MAX_WIDTH_G2,
#else
            .max_width = CH1_WIDTH_G2,
#endif
#if (CH1_MAX_HEIGHT_G2 > CH1_HEIGHT_G2)
            .max_height = CH1_MAX_HEIGHT_G2,
#else
            .max_height = CH1_HEIGHT_G2,
#endif
            .yuv_type = CH1_YUV_TYPE_G2,
            .bufnum = CH1_BUFNUM_G2,
#endif /*CH1_WIDTH_G2*/
        },

        {
            .enable = 0,
#ifdef CH2_ENABLE_G2
            .enable = 1,
            .channel = 2,
            .width = CH2_WIDTH_G2,
            .height = CH2_HEIGHT_G2,
#if (CH2_MAX_WIDTH_G2 > CH2_WIDTH_G2)
            .max_width = CH2_MAX_WIDTH_G2,
#else
            .max_width = CH2_WIDTH_G2,
#endif
#if (CH2_MAX_HEIGHT_G2 > CH2_HEIGHT_G2)
            .max_height = CH2_MAX_HEIGHT_G2,
#else
            .max_height = CH2_HEIGHT_G2,
#endif
            .yuv_type = CH2_YUV_TYPE_G2,
            .bufnum = CH2_BUFNUM_G2,
#endif /*CH2_WIDTH_G2*/
        },

        {
            .enable = 0,
#ifdef CH3_ENABLE_G2
            .enable = 1,
            .channel = 3,
            .width = CH3_WIDTH_G2,
            .height = CH3_HEIGHT_G2,
#if (CH3_MAX_WIDTH_G2 > CH3_WIDTH_G2)
            .max_width = CH3_MAX_WIDTH_G2,
#else
            .max_width = CH3_WIDTH_G2,
#endif
#if (CH3_MAX_HEIGHT_G2 > CH3_HEIGHT_G2)
            .max_height = CH3_MAX_HEIGHT_G2,
#else
            .max_height = CH3_HEIGHT_G2,
#endif
            .yuv_type = CH3_YUV_TYPE_G2,
            .bufnum = CH3_BUFNUM_G2,
#endif /*CH3_WIDTH_G2*/
        },
    }, /* group 2 */
};

struct enc_channel_info g_enc_chn_infos[MAX_GRP_NUM][MAX_VPU_CHN_NUM] = {
    {
        {
            .enable = 0,
#if defined(CH0_BIND_ENC_G0) && defined(CH0_ENC_TYPE_G0)
            .enable = 1,
            .channel = 0,
            .width = CH0_WIDTH_G0,
            .height = CH0_HEIGHT_G0,
#if (CH0_MAX_WIDTH_G0 > CH0_WIDTH_G0)
            .max_width = CH0_MAX_WIDTH_G0,
#else
            .max_width = CH0_WIDTH_G0,
#endif
#if (CH0_MAX_HEIGHT_G0 > CH0_HEIGHT_G0)
            .max_height = CH0_MAX_HEIGHT_G0,
#else
            .max_height = CH0_HEIGHT_G0,
#endif
            .frame_count = CH0_FRAME_COUNT_G0,
            .frame_time = CH0_FRAME_TIME_G0,
            .bps = CH0_BIT_RATE_G0 * 1024,
            .enc_type = CH0_ENC_TYPE_G0,
            .rc_type = CH0_RC_TYPE_G0,
            .breath_on = CH0_BREATH_OPEN_G0,
#endif /*CH0_BIND_ENC_G0*/
        },

        {
            .enable = 0,
#if defined(CH1_BIND_ENC_G0) && defined(CH1_ENC_TYPE_G0)
            .enable = 1,
            .channel = 1,
            .width = CH1_WIDTH_G0,
            .height = CH1_HEIGHT_G0,
#if (CH1_MAX_WIDTH_G0 > CH1_WIDTH_G0)
            .max_width = CH1_MAX_WIDTH_G0,
#else
            .max_width = CH1_WIDTH_G0,
#endif
#if (CH1_MAX_HEIGHT_G0 > CH1_HEIGHT_G0)
            .max_height = CH1_MAX_HEIGHT_G0,
#else
            .max_height = CH1_HEIGHT_G0,
#endif
            .frame_count = CH1_FRAME_COUNT_G0,
            .frame_time = CH1_FRAME_TIME_G0,
            .bps = CH1_BIT_RATE_G0 * 1024,
            .enc_type = CH1_ENC_TYPE_G0,
            .rc_type = CH1_RC_TYPE_G0,
            .breath_on = CH1_BREATH_OPEN_G0,
#endif /*CH1_BIND_ENC_G0*/
        },

        {
            .enable = 0,
#if defined(CH2_BIND_ENC_G0) && defined(CH2_ENC_TYPE_G0)
            .enable = 1,
            .channel = 2,
            .width = CH2_WIDTH_G0,
            .height = CH2_HEIGHT_G0,
#if (CH2_MAX_WIDTH_G0 > CH2_WIDTH_G0)
            .max_width = CH2_MAX_WIDTH_G0,
#else
            .max_width = CH2_WIDTH_G0,
#endif
#if (CH2_MAX_HEIGHT_G0 > CH2_HEIGHT_G0)
            .max_height = CH2_MAX_HEIGHT_G0,
#else
            .max_height = CH2_HEIGHT_G0,
#endif
            .frame_count = CH2_FRAME_COUNT_G0,
            .frame_time = CH2_FRAME_TIME_G0,
            .bps = CH2_BIT_RATE_G0 * 1024,
            .enc_type = CH2_ENC_TYPE_G0,
            .rc_type = CH2_RC_TYPE_G0,
            .breath_on = CH2_BREATH_OPEN_G0,
#endif /*CH2_BIND_ENC_G0*/
        },

        {
            .enable = 0,
#if defined(CH3_BIND_ENC_G0) && defined(CH3_ENC_TYPE_G0)
            .enable = 1,
            .channel = 3,
            .width = CH3_WIDTH_G0,
            .height = CH3_HEIGHT_G0,
#if (CH3_MAX_WIDTH_G0 > CH3_WIDTH_G0)
            .max_width = CH3_MAX_WIDTH_G0,
#else
            .max_width = CH3_WIDTH_G0,
#endif
#if (CH3_MAX_HEIGHT_G0 > CH3_HEIGHT_G0)
            .max_height = CH3_MAX_HEIGHT_G0,
#else
            .max_height = CH3_HEIGHT_G0,
#endif
            .frame_count = CH3_FRAME_COUNT_G0,
            .frame_time = CH3_FRAME_TIME_G0,
            .bps = CH3_BIT_RATE_G0 * 1024,
            .enc_type = CH3_ENC_TYPE_G0,
            .rc_type = CH3_RC_TYPE_G0,
            .breath_on = CH3_BREATH_OPEN_G0,
#endif /*CH3_BIND_ENC_G0*/
        },
    }, /* G0 */
    {
        {
            .enable = 0,
#if defined(CH0_BIND_ENC_G1) && defined(CH0_ENC_TYPE_G1)
            .enable = 1,
            .channel = 0,
            .width = CH0_WIDTH_G1,
            .height = CH0_HEIGHT_G1,
#if (CH0_MAX_WIDTH_G1 > CH0_WIDTH_G1)
            .max_width = CH0_MAX_WIDTH_G1,
#else
            .max_width = CH0_WIDTH_G1,
#endif
#if (CH0_MAX_HEIGHT_G1 > CH0_HEIGHT_G1)
            .max_height = CH0_MAX_HEIGHT_G1,
#else
            .max_height = CH0_HEIGHT_G1,
#endif
            .frame_count = CH0_FRAME_COUNT_G1,
            .frame_time = CH0_FRAME_TIME_G1,
            .bps = CH0_BIT_RATE_G1 * 1024,
            .enc_type = CH0_ENC_TYPE_G1,
            .rc_type = CH0_RC_TYPE_G1,
            .breath_on = CH0_BREATH_OPEN_G1,
#endif /*CH0_BIND_ENC_G1*/
        },

        {
            .enable = 0,
#if defined(CH1_BIND_ENC_G1) && defined(CH1_ENC_TYPE_G1)
            .enable = 1,
            .channel = 1,
            .width = CH1_WIDTH_G1,
            .height = CH1_HEIGHT_G1,
#if (CH1_MAX_WIDTH_G1 > CH1_WIDTH_G1)
            .max_width = CH1_MAX_WIDTH_G1,
#else
            .max_width = CH1_WIDTH_G1,
#endif
#if (CH1_MAX_HEIGHT_G1 > CH1_HEIGHT_G1)
            .max_height = CH1_MAX_HEIGHT_G1,
#else
            .max_height = CH1_HEIGHT_G1,
#endif
            .frame_count = CH1_FRAME_COUNT_G1,
            .frame_time = CH1_FRAME_TIME_G1,
            .bps = CH1_BIT_RATE_G1 * 1024,
            .enc_type = CH1_ENC_TYPE_G1,
            .rc_type = CH1_RC_TYPE_G1,
            .breath_on = CH1_BREATH_OPEN_G1,
#endif /*CH1_BIND_ENC_G1*/
        },

        {
            .enable = 0,
#if defined(CH2_BIND_ENC_G1) && defined(CH2_ENC_TYPE_G1)
            .enable = 1,
            .channel = 2,
            .width = CH2_WIDTH_G1,
            .height = CH2_HEIGHT_G1,
#if (CH2_MAX_WIDTH_G1 > CH2_WIDTH_G1)
            .max_width = CH2_MAX_WIDTH_G1,
#else
            .max_width = CH2_WIDTH_G1,
#endif
#if (CH2_MAX_HEIGHT_G1 > CH2_HEIGHT_G1)
            .max_height = CH2_MAX_HEIGHT_G1,
#else
            .max_height = CH2_HEIGHT_G1,
#endif
            .frame_count = CH2_FRAME_COUNT_G1,
            .frame_time = CH2_FRAME_TIME_G1,
            .bps = CH2_BIT_RATE_G1 * 1024,
            .enc_type = CH2_ENC_TYPE_G1,
            .rc_type = CH2_RC_TYPE_G1,
            .breath_on = CH2_BREATH_OPEN_G1,
#endif /*CH2_BIND_ENC_G1*/
        },

        {
            .enable = 0,
#if defined(CH3_BIND_ENC_G1) && defined(CH3_ENC_TYPE_G1)
            .enable = 1,
            .channel = 3,
            .width = CH3_WIDTH_G1,
            .height = CH3_HEIGHT_G1,
#if (CH3_MAX_WIDTH_G1 > CH3_WIDTH_G1)
            .max_width = CH3_MAX_WIDTH_G1,
#else
            .max_width = CH3_WIDTH_G1,
#endif
#if (CH3_MAX_HEIGHT_G1 > CH3_HEIGHT_G1)
            .max_height = CH3_MAX_HEIGHT_G1,
#else
            .max_height = CH3_HEIGHT_G1,
#endif
            .frame_count = CH3_FRAME_COUNT_G1,
            .frame_time = CH3_FRAME_TIME_G1,
            .bps = CH3_BIT_RATE_G1 * 1024,
            .enc_type = CH3_ENC_TYPE_G1,
            .rc_type = CH3_RC_TYPE_G1,
            .breath_on = CH3_BREATH_OPEN_G1,
#endif /*CH3_BIND_ENC_G1*/
        },
    }, /* G1 */
    {
        {
            .enable = 0,
#if defined(CH0_BIND_ENC_G2) && defined(CH0_ENC_TYPE_G2)
            .enable = 1,
            .channel = 0,
            .width = CH0_WIDTH_G2,
            .height = CH0_HEIGHT_G2,
#if (CH0_MAX_WIDTH_G2 > CH0_WIDTH_G2)
            .max_width = CH0_MAX_WIDTH_G2,
#else
            .max_width = CH0_WIDTH_G2,
#endif
#if (CH0_MAX_HEIGHT_G2 > CH0_HEIGHT_G2)
            .max_height = CH0_MAX_HEIGHT_G2,
#else
            .max_height = CH0_HEIGHT_G2,
#endif
            .frame_count = CH0_FRAME_COUNT_G2,
            .frame_time = CH0_FRAME_TIME_G2,
            .bps = CH0_BIT_RATE_G2 * 1024,
            .enc_type = CH0_ENC_TYPE_G2,
            .rc_type = CH0_RC_TYPE_G2,
            .breath_on = CH0_BREATH_OPEN_G2,
#endif /*CH0_BIND_ENC_G2*/
        },

        {
            .enable = 0,
#if defined(CH1_BIND_ENC_G2) && defined(CH1_ENC_TYPE_G2)
            .enable = 1,
            .channel = 1,
            .width = CH1_WIDTH_G2,
            .height = CH1_HEIGHT_G2,
#if (CH1_MAX_WIDTH_G2 > CH1_WIDTH_G2)
            .max_width = CH1_MAX_WIDTH_G2,
#else
            .max_width = CH1_WIDTH_G2,
#endif
#if (CH1_MAX_HEIGHT_G2 > CH1_HEIGHT_G2)
            .max_height = CH1_MAX_HEIGHT_G2,
#else
            .max_height = CH1_HEIGHT_G2,
#endif
            .frame_count = CH1_FRAME_COUNT_G2,
            .frame_time = CH1_FRAME_TIME_G2,
            .bps = CH1_BIT_RATE_G2 * 1024,
            .enc_type = CH1_ENC_TYPE_G2,
            .rc_type = CH1_RC_TYPE_G2,
            .breath_on = CH1_BREATH_OPEN_G2,
#endif /*CH1_BIND_ENC_G2*/
        },

        {
            .enable = 0,
#if defined(CH2_BIND_ENC_G2) && defined(CH2_ENC_TYPE_G2)
            .enable = 1,
            .channel = 2,
            .width = CH2_WIDTH_G2,
            .height = CH2_HEIGHT_G2,
#if (CH2_MAX_WIDTH_G2 > CH2_WIDTH_G2)
            .max_width = CH2_MAX_WIDTH_G2,
#else
            .max_width = CH2_WIDTH_G2,
#endif
#if (CH2_MAX_HEIGHT_G2 > CH2_HEIGHT_G2)
            .max_height = CH2_MAX_HEIGHT_G2,
#else
            .max_height = CH2_HEIGHT_G2,
#endif
            .frame_count = CH2_FRAME_COUNT_G2,
            .frame_time = CH2_FRAME_TIME_G2,
            .bps = CH2_BIT_RATE_G2 * 1024,
            .enc_type = CH2_ENC_TYPE_G2,
            .rc_type = CH2_RC_TYPE_G2,
            .breath_on = CH2_BREATH_OPEN_G2,
#endif /*CH2_BIND_ENC_G2*/
        },

        {
            .enable = 0,
#if defined(CH3_BIND_ENC_G2) && defined(CH3_ENC_TYPE_G2)
            .enable = 1,
            .channel = 3,
            .width = CH3_WIDTH_G2,
            .height = CH3_HEIGHT_G2,
#if (CH3_MAX_WIDTH_G2 > CH3_WIDTH_G2)
            .max_width = CH3_MAX_WIDTH_G2,
#else
            .max_width = CH3_WIDTH_G2,
#endif
#if (CH3_MAX_HEIGHT_G2 > CH3_HEIGHT_G2)
            .max_height = CH3_MAX_HEIGHT_G2,
#else
            .max_height = CH3_HEIGHT_G2,
#endif
            .frame_count = CH3_FRAME_COUNT_G2,
            .frame_time = CH3_FRAME_TIME_G2,
            .bps = CH3_BIT_RATE_G2 * 1024,
            .enc_type = CH3_ENC_TYPE_G2,
            .rc_type = CH3_RC_TYPE_G2,
            .breath_on = CH3_BREATH_OPEN_G2,
#endif /*CH3_BIND_ENC_G2*/
        },
    }, /* G2 */
};

struct enc_channel_info g_jpeg_chn_infos[MAX_GRP_NUM * MAX_VPU_CHN_NUM] = {
    {
        .enable = 0,
#if defined(CH0_MJPEG_G0)
        .enable = 1,
        .channel = 0,
        .width = CH0_WIDTH_G0,
        .height = CH0_HEIGHT_G0,
#if (CH0_MAX_WIDTH_G0 > CH0_WIDTH_G0)
        .max_width = CH0_MAX_WIDTH_G0,
#else
        .max_width = CH0_WIDTH_G0,
#endif
#if (CH0_MAX_HEIGHT_G0 > CH0_HEIGHT_G0)
        .max_height = CH0_MAX_HEIGHT_G0,
#else
        .max_height = CH0_HEIGHT_G0,
#endif
        .frame_count = CH0_MJPEG_FRAME_COUNT_G0,
        .frame_time = CH0_MJPEG_FRAME_TIME_G0,
        .bps = CH0_MJPEG_BIT_RATE_G0 * 1024,
        .enc_type = FH_MJPEG,
        .rc_type = CH0_MJPEG_RC_TYPE_G0,
        .breath_on = 0,
#endif /*CH0_MJPEG_G0*/
    },
    {
        .enable = 0,
#if defined(CH1_MJPEG_G0)
        .enable = 1,
        .channel = 0,
        .width = CH1_WIDTH_G0,
        .height = CH1_HEIGHT_G0,
#if (CH1_MAX_WIDTH_G0 > CH1_WIDTH_G0)
        .max_width = CH1_MAX_WIDTH_G0,
#else
        .max_width = CH1_WIDTH_G0,
#endif
#if (CH1_MAX_HEIGHT_G0 > CH1_HEIGHT_G0)
        .max_height = CH1_MAX_HEIGHT_G0,
#else
        .max_height = CH1_HEIGHT_G0,
#endif
        .frame_count = CH1_MJPEG_FRAME_COUNT_G0,
        .frame_time = CH1_MJPEG_FRAME_TIME_G0,
        .bps = CH1_MJPEG_BIT_RATE_G0 * 1024,
        .enc_type = FH_MJPEG,
        .rc_type = CH1_MJPEG_RC_TYPE_G0,
        .breath_on = 0,
#endif /*CH1_MJPEG_G0*/
    },
    {
        .enable = 0,
#if defined(CH2_MJPEG_G0)
        .enable = 1,
        .channel = 0,
        .width = CH2_WIDTH_G0,
        .height = CH2_HEIGHT_G0,
#if (CH2_MAX_WIDTH_G0 > CH2_WIDTH_G0)
        .max_width = CH2_MAX_WIDTH_G0,
#else
        .max_width = CH2_WIDTH_G0,
#endif
#if (CH2_MAX_HEIGHT_G0 > CH2_HEIGHT_G0)
        .max_height = CH2_MAX_HEIGHT_G0,
#else
        .max_height = CH2_HEIGHT_G0,
#endif
        .frame_count = CH2_MJPEG_FRAME_COUNT_G0,
        .frame_time = CH2_MJPEG_FRAME_TIME_G0,
        .bps = CH2_MJPEG_BIT_RATE_G0 * 1024,
        .enc_type = FH_MJPEG,
        .rc_type = CH2_MJPEG_RC_TYPE_G0,
        .breath_on = 0,
#endif /*CH2_MJPEG_G0*/
    },
    {
        .enable = 0,
#if defined(CH3_MJPEG_G0)
        .enable = 1,
        .channel = 0,
        .width = CH3_WIDTH_G0,
        .height = CH3_HEIGHT_G0,
#if (CH3_MAX_WIDTH_G0 > CH3_WIDTH_G0)
        .max_width = CH3_MAX_WIDTH_G0,
#else
        .max_width = CH3_WIDTH_G0,
#endif
#if (CH3_MAX_HEIGHT_G0 > CH3_HEIGHT_G0)
        .max_height = CH3_MAX_HEIGHT_G0,
#else
        .max_height = CH3_HEIGHT_G0,
#endif
        .frame_count = CH3_MJPEG_FRAME_COUNT_G0,
        .frame_time = CH3_MJPEG_FRAME_TIME_G0,
        .bps = CH3_MJPEG_BIT_RATE_G0 * 1024,
        .enc_type = FH_MJPEG,
        .rc_type = CH3_MJPEG_RC_TYPE_G0,
        .breath_on = 0,
#endif /*CH3_MJPEG_G0*/
    },
    /*****************************G0*****************************/

    {
        .enable = 0,
#if defined(CH0_MJPEG_G1)
        .enable = 1,
        .channel = 0,
        .width = CH0_WIDTH_G1,
        .height = CH0_HEIGHT_G1,
#if (CH0_MAX_WIDTH_G1 > CH0_WIDTH_G1)
        .max_width = CH0_MAX_WIDTH_G1,
#else
        .max_width = CH0_WIDTH_G1,
#endif
#if (CH0_MAX_HEIGHT_G1 > CH0_HEIGHT_G1)
        .max_height = CH0_MAX_HEIGHT_G1,
#else
        .max_height = CH0_HEIGHT_G1,
#endif
        .frame_count = CH0_MJPEG_FRAME_COUNT_G1,
        .frame_time = CH0_MJPEG_FRAME_TIME_G1,
        .bps = CH0_MJPEG_BIT_RATE_G1 * 1024,
        .enc_type = FH_MJPEG,
        .rc_type = CH0_MJPEG_RC_TYPE_G1,
        .breath_on = 0,
#endif /*CH0_MJPEG_G1*/
    },
    {
        .enable = 0,
#if defined(CH1_MJPEG_G1)
        .enable = 1,
        .channel = 0,
        .width = CH1_WIDTH_G1,
        .height = CH1_HEIGHT_G1,
#if (CH1_MAX_WIDTH_G1 > CH1_WIDTH_G1)
        .max_width = CH1_MAX_WIDTH_G1,
#else
        .max_width = CH1_WIDTH_G1,
#endif
#if (CH1_MAX_HEIGHT_G1 > CH1_HEIGHT_G1)
        .max_height = CH1_MAX_HEIGHT_G1,
#else
        .max_height = CH1_HEIGHT_G1,
#endif
        .frame_count = CH1_MJPEG_FRAME_COUNT_G1,
        .frame_time = CH1_MJPEG_FRAME_TIME_G1,
        .bps = CH1_MJPEG_BIT_RATE_G1 * 1024,
        .enc_type = FH_MJPEG,
        .rc_type = CH1_MJPEG_RC_TYPE_G1,
        .breath_on = 0,
#endif /*CH1_MJPEG_G1*/
    },
    {
        .enable = 0,
#if defined(CH2_MJPEG_G1)
        .enable = 1,
        .channel = 0,
        .width = CH2_WIDTH_G1,
        .height = CH2_HEIGHT_G1,
#if (CH2_MAX_WIDTH_G1 > CH2_WIDTH_G1)
        .max_width = CH2_MAX_WIDTH_G1,
#else
        .max_width = CH2_WIDTH_G1,
#endif
#if (CH2_MAX_HEIGHT_G1 > CH2_HEIGHT_G1)
        .max_height = CH2_MAX_HEIGHT_G1,
#else
        .max_height = CH2_HEIGHT_G1,
#endif
        .frame_count = CH2_MJPEG_FRAME_COUNT_G1,
        .frame_time = CH2_MJPEG_FRAME_TIME_G1,
        .bps = CH2_MJPEG_BIT_RATE_G1 * 1024,
        .enc_type = FH_MJPEG,
        .rc_type = CH2_MJPEG_RC_TYPE_G1,
        .breath_on = 0,
#endif /*CH2_MJPEG_G1*/
    },
    {
        .enable = 0,
#if defined(CH3_MJPEG_G1)
        .enable = 1,
        .channel = 0,
        .width = CH3_WIDTH_G1,
        .height = CH3_HEIGHT_G1,
#if (CH3_MAX_WIDTH_G1 > CH3_WIDTH_G1)
        .max_width = CH3_MAX_WIDTH_G1,
#else
        .max_width = CH3_WIDTH_G1,
#endif
#if (CH3_MAX_HEIGHT_G1 > CH3_HEIGHT_G1)
        .max_height = CH3_MAX_HEIGHT_G1,
#else
        .max_height = CH3_HEIGHT_G1,
#endif
        .frame_count = CH3_MJPEG_FRAME_COUNT_G1,
        .frame_time = CH3_MJPEG_FRAME_TIME_G1,
        .bps = CH3_MJPEG_BIT_RATE_G1 * 1024,
        .enc_type = FH_MJPEG,
        .rc_type = CH3_MJPEG_RC_TYPE_G1,
        .breath_on = 0,
#endif /*CH3_MJPEG_G1*/
    },
    /*****************************G1*****************************/

    {
        .enable = 0,
#if defined(CH0_MJPEG_G2)
        .enable = 1,
        .channel = 0,
        .width = CH0_WIDTH_G2,
        .height = CH0_HEIGHT_G2,
#if (CH0_MAX_WIDTH_G2 > CH0_WIDTH_G2)
        .max_width = CH0_MAX_WIDTH_G2,
#else
        .max_width = CH0_WIDTH_G2,
#endif
#if (CH0_MAX_HEIGHT_G2 > CH0_HEIGHT_G2)
        .max_height = CH0_MAX_HEIGHT_G2,
#else
        .max_height = CH0_HEIGHT_G2,
#endif
        .frame_count = CH0_MJPEG_FRAME_COUNT_G2,
        .frame_time = CH0_MJPEG_FRAME_TIME_G2,
        .bps = CH0_MJPEG_BIT_RATE_G2 * 1024,
        .enc_type = FH_MJPEG,
        .rc_type = CH0_MJPEG_RC_TYPE_G2,
        .breath_on = 0,
#endif /*CH0_MJPEG_G2*/
    },
    {
        .enable = 0,
#if defined(CH1_MJPEG_G2)
        .enable = 1,
        .channel = 0,
        .width = CH1_WIDTH_G2,
        .height = CH1_HEIGHT_G2,
#if (CH1_MAX_WIDTH_G2 > CH1_WIDTH_G2)
        .max_width = CH1_MAX_WIDTH_G2,
#else
        .max_width = CH1_WIDTH_G2,
#endif
#if (CH1_MAX_HEIGHT_G2 > CH1_HEIGHT_G2)
        .max_height = CH1_MAX_HEIGHT_G2,
#else
        .max_height = CH1_HEIGHT_G2,
#endif
        .frame_count = CH1_MJPEG_FRAME_COUNT_G2,
        .frame_time = CH1_MJPEG_FRAME_TIME_G2,
        .bps = CH1_MJPEG_BIT_RATE_G2 * 1024,
        .enc_type = FH_MJPEG,
        .rc_type = CH1_MJPEG_RC_TYPE_G2,
        .breath_on = 0,
#endif /*CH1_MJPEG_G2*/
    },
    {
        .enable = 0,
#if defined(CH2_MJPEG_G2)
        .enable = 1,
        .channel = 0,
        .width = CH2_WIDTH_G2,
        .height = CH2_HEIGHT_G2,
#if (CH2_MAX_WIDTH_G2 > CH2_WIDTH_G2)
        .max_width = CH2_MAX_WIDTH_G2,
#else
        .max_width = CH2_WIDTH_G2,
#endif
#if (CH2_MAX_HEIGHT_G2 > CH2_HEIGHT_G2)
        .max_height = CH2_MAX_HEIGHT_G2,
#else
        .max_height = CH2_HEIGHT_G2,
#endif
        .frame_count = CH2_MJPEG_FRAME_COUNT_G2,
        .frame_time = CH2_MJPEG_FRAME_TIME_G2,
        .bps = CH2_MJPEG_BIT_RATE_G2 * 1024,
        .enc_type = FH_MJPEG,
        .rc_type = CH2_MJPEG_RC_TYPE_G2,
        .breath_on = 0,
#endif /*CH2_MJPEG_G2*/
    },
    {
        .enable = 0,
#if defined(CH3_MJPEG_G2)
        .enable = 1,
        .channel = 0,
        .width = CH3_WIDTH_G2,
        .height = CH3_HEIGHT_G2,
#if (CH3_MAX_WIDTH_G2 > CH3_WIDTH_G2)
        .max_width = CH3_MAX_WIDTH_G2,
#else
        .max_width = CH3_WIDTH_G2,
#endif
#if (CH3_MAX_HEIGHT_G2 > CH3_HEIGHT_G2)
        .max_height = CH3_MAX_HEIGHT_G2,
#else
        .max_height = CH3_HEIGHT_G2,
#endif
        .frame_count = CH3_MJPEG_FRAME_COUNT_G2,
        .frame_time = CH3_MJPEG_FRAME_TIME_G2,
        .bps = CH3_MJPEG_BIT_RATE_G2 * 1024,
        .enc_type = FH_MJPEG,
        .rc_type = CH3_MJPEG_RC_TYPE_G2,
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
#if defined(VPU_MODE_MEM_G0)
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
#if defined(VPU_MODE_MEM_G1)
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
#if defined(VPU_MODE_MEM_G2)
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
