#ifndef __ISP_CHANNEL_H__
#define __ISP_CHANNEL_H__
#include "rtconfig_app.h"
#include "isp_sensor.h"

struct dev_isp_info g_isp_info[MAX_GRP_NUM] = {

    {
        .enable = 0,
#if (FH_GRP_ID == 0)
        .enable = 1,
        .channel = 0,
        .isp_format = ISP_FORMAT,
        .isp_init_width = VI_INPUT_WIDTH,
        .isp_init_height = VI_INPUT_HEIGHT,
#ifdef MAX_ISP_WIDTH
        .isp_max_width = MAX_ISP_WIDTH > VI_INPUT_WIDTH ? MAX_ISP_WIDTH : VI_INPUT_WIDTH,
        .isp_max_height = MAX_ISP_HEIGHT > VI_INPUT_HEIGHT ? MAX_ISP_HEIGHT : VI_INPUT_HEIGHT,
#else
        .isp_max_width = VI_INPUT_WIDTH,
        .isp_max_height = VI_INPUT_HEIGHT,
#endif
        .sensor_name = SENSOR_NAME,
#endif
    }, /* VPU_MODE_ISP */
    {
        .enable = 0,
#if (FH_GRP_ID == 1)
        .enable = 1,
        .channel = 1,
        .isp_format = ISP_FORMAT,
        .isp_init_width = VI_INPUT_WIDTH,
        .isp_init_height = VI_INPUT_HEIGHT,
#ifdef MAX_ISP_WIDTH
        .isp_max_width = MAX_ISP_WIDTH > VI_INPUT_WIDTH ? MAX_ISP_WIDTH : VI_INPUT_WIDTH,
        .isp_max_height = MAX_ISP_HEIGHT > VI_INPUT_HEIGHT ? MAX_ISP_HEIGHT : VI_INPUT_HEIGHT,
#else
        .isp_max_width = VI_INPUT_WIDTH,
        .isp_max_height = VI_INPUT_HEIGHT,
#endif
        .sensor_name = SENSOR_NAME,
#endif /* VPU_MODE_ISP */
    },
    {
        .enable = 0,

#if (FH_GRP_ID == 2)
        .enable = 1,
        .channel = 2,
        .isp_format = ISP_FORMAT,
        .isp_init_width = VI_INPUT_WIDTH,
        .isp_init_height = VI_INPUT_HEIGHT,
#ifdef MAX_ISP_WIDTH
        .isp_max_width = MAX_ISP_WIDTH > VI_INPUT_WIDTH ? MAX_ISP_WIDTH : VI_INPUT_WIDTH,
        .isp_max_height = MAX_ISP_HEIGHT > VI_INPUT_HEIGHT ? MAX_ISP_HEIGHT : VI_INPUT_HEIGHT,
#else
        .isp_max_width = VI_INPUT_WIDTH,
        .isp_max_height = VI_INPUT_HEIGHT,
#endif
        .sensor_name = SENSOR_NAME,
#endif /* VPU_MODE_ISP */
    },

};

#endif /* __ISP_CHANNEL_H__ */