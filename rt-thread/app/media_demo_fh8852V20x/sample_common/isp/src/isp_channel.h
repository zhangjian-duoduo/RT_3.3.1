#ifndef __ISP_CHANNEL_H__
#define __ISP_CHANNEL_H__

struct dev_isp_info g_isp_info[MAX_GRP_NUM] = {

    {
        .enable          = 0,
#if defined (VPU_MODE_ISP_G0) 
        .enable          = 1,
        .channel         = 0,
        .isp_format      = ISP_FORMAT_G0,
        .isp_init_width  = VI_INPUT_WIDTH_G0,
        .isp_init_height = VI_INPUT_HEIGHT_G0,
#ifdef MAX_ISP_WIDTH_G0
        .isp_max_width   = MAX_ISP_WIDTH_G0 > VI_INPUT_WIDTH_G0 ? MAX_ISP_WIDTH_G0 : VI_INPUT_WIDTH_G0,
        .isp_max_height  = MAX_ISP_HEIGHT_G0 > VI_INPUT_HEIGHT_G0 ? MAX_ISP_HEIGHT_G0 : VI_INPUT_HEIGHT_G0,
#else
        .isp_max_width   = VI_INPUT_WIDTH_G0,
        .isp_max_height  = VI_INPUT_HEIGHT_G0,
#endif
        .sensor_name     = SENSOR_NAME_G0,
        .running = 0,
#endif /* #if defined VPU_MODE_ISP_G0 */
    },
    {
        .enable          = 0,
#if defined (VPU_MODE_ISP_G1)
        .enable          = 1,
        .channel         = 1,
        .isp_format      = ISP_FORMAT_G1,
        .isp_init_width  = VI_INPUT_WIDTH_G1,
        .isp_init_height = VI_INPUT_HEIGHT_G1,
#ifdef MAX_ISP_WIDTH_G1
        .isp_max_width   = MAX_ISP_WIDTH_G1 > VI_INPUT_WIDTH_G1 ? MAX_ISP_WIDTH_G1 : VI_INPUT_WIDTH_G1,
        .isp_max_height  = MAX_ISP_HEIGHT_G1 > VI_INPUT_HEIGHT_G1 ? MAX_ISP_HEIGHT_G1 : VI_INPUT_HEIGHT_G1,
#else
        .isp_max_width   = VI_INPUT_WIDTH_G1,
        .isp_max_height  = VI_INPUT_HEIGHT_G1,
#endif
        .sensor_name     = SENSOR_NAME_G1,
        .running = 0,
#endif /* VPU_MODE_ISP_G1 */
    },
};

#endif /* __ISP_CHANNEL_H__ */