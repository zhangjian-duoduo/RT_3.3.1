/*************************************************************************
    > File Name: isp_sensor.h
    > Author: dongky341
    > Mail: dongky341@fullhan.com
    > Created Time: 2021年09月07日 星期二 20时00分36秒
 ************************************************************************/
#ifndef __ISP_SENSOR_H__
#define __ISP_SENSOR_H__

#ifdef FH_USING_OVOS02D_MIPI
#define SENSOR_NAME "ovos02d_mipi"
#endif

#ifdef FH_USING_OVOS02K_MIPI
#define SENSOR_NAME "ovos02k_mipi"
#endif

#ifdef FH_USING_GC2083_MIPI
#define SENSOR_NAME "gc2083_mipi"
#endif

#ifdef FH_USING_OVOS04C10_MIPI
#define SENSOR_NAME "ovos04c10_mipi"
#endif

#ifdef FH_USING_DUMMY_SENSOR
#define SENSOR_NAME "dummy_sensor"
#endif

#ifdef FH_USING_OVOS05_MIPI
#define SENSOR_NAME "ovos05_mipi"
#endif

#ifdef FH_USING_IMX415_MIPI
#define SENSOR_NAME "imx415_mipi"
#endif

#ifdef FH_USING_OVOS08_MIPI
#define SENSOR_NAME "ovos08_mipi"
#endif

#endif /* __ISP_SENSOR_H__ */