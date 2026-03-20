#include <isp/isp_sensor_if.h>
#include "sample_common.h"

#include "FHAdv_Isp_mpi_v3.h"

#define SENSOR_CREATE_MULTI(n) Sensor_Create##_##n

#if defined (FH_USING_OVOS02D_MIPI) || defined (FH_USING_OVOS02D_MIPI)
extern struct isp_sensor_if *SENSOR_CREATE_MULTI(ovos02d_mipi)();
#endif

#if defined (FH_USING_OVOS02K_MIPI) || defined (FH_USING_OVOS02K_MIPI)
extern struct isp_sensor_if *SENSOR_CREATE_MULTI(ovos02k_mipi)();
#endif

#if defined (FH_USING_GC2083_MIPI) || defined (FH_USING_GC2083_MIPI)
extern struct isp_sensor_if *SENSOR_CREATE_MULTI(GC2083_mipi)();
#endif

#if defined (FH_USING_OVOS04C10_MIPI) || defined (FH_USING_OVOS04C10_MIPI)
extern struct isp_sensor_if *SENSOR_CREATE_MULTI(ovos04c10_mipi)();
#endif

#if defined (FH_USING_DUMMY_SENSOR) || defined (FH_USING_DUMMY_SENSOR) || defined (FH_USING_DUMMY_SENSOR)
extern struct isp_sensor_if *SENSOR_CREATE_MULTI(dummy_sensor)();
#endif

#if defined (FH_USING_JXF23_DVP)
extern struct isp_sensor_if *SENSOR_CREATE_MULTI(jxf23_dvp)();
#endif

#if defined (FH_USING_OVOS05_MIPI) || defined (FH_USING_OVOS05_MIPI)
extern struct isp_sensor_if *SENSOR_CREATE_MULTI(ovos05_mipi)();
#endif

#if defined (FH_USING_OVOS08_MIPI) || defined (FH_USING_OVOS08_MIPI)
extern struct isp_sensor_if *SENSOR_CREATE_MULTI(ovos08_mipi)();
#endif

#if defined (FH_USING_IMX415_MIPI) || defined (FH_USING_IMX415_MIPI)
extern struct isp_sensor_if *SENSOR_CREATE_MULTI(imx415_mipi)();
#endif


struct isp_sensor_if *start_sensor(char *SensorName, int grpid)
{
    int ret;
    struct isp_sensor_if *sensor = NULL;

#if defined (FH_USING_OVOS02D_MIPI) || defined (FH_USING_OVOS02D_MIPI)
    ret = strcmp(SensorName, "ovos02d_mipi");
    if (!ret)
    {
        sensor = SENSOR_CREATE_MULTI(ovos02d_mipi)();
    }
#endif
#if defined (FH_USING_OVOS02K_MIPI) || defined (FH_USING_OVOS02K_MIPI)
    ret = strcmp(SensorName, "ovos02k_mipi");
    if (!ret)
    {
        sensor = SENSOR_CREATE_MULTI(ovos02k_mipi)();
    }
#endif
#if defined (FH_USING_GC2083_MIPI) || defined (FH_USING_GC2083_MIPI)
    ret = strcmp(SensorName, "gc2083_mipi");
    if (!ret)
    {
        sensor = SENSOR_CREATE_MULTI(gc2083_mipi)();
    }
#endif

#if defined (FH_USING_OVOS04C10_MIPI) || defined (FH_USING_OVOS04C10_MIPI)
    ret = strcmp(SensorName, "ovos04c10_mipi");
    if (!ret)
    {
        sensor = SENSOR_CREATE_MULTI(ovos04c10_mipi)();
    }
#endif
#if defined (FH_USING_DUMMY_SENSOR) || defined (FH_USING_DUMMY_SENSOR) || defined (FH_USING_DUMMY_SENSOR)
    ret = strcmp(SensorName, "dummy_sensor");
    if (!ret)
    {
        sensor = SENSOR_CREATE_MULTI(dummy_sensor)();
    }
#endif
#if defined (FH_USING_JXF23_DVP)
    ret = strcmp(SensorName, "jxf23_dvp");
    if (!ret)
    {
        sensor = SENSOR_CREATE_MULTI(jxf23_dvp)();
    }
#endif

#if defined (FH_USING_OVOS05_MIPI) || defined (FH_USING_OVOS05_MIPI)
    ret = strcmp(SensorName, "ovos05_mipi");
    if (!ret)
    {
        sensor = SENSOR_CREATE_MULTI(ovos05_mipi)();
    }
#endif

#if defined (FH_USING_OVOS08_MIPI) || defined (FH_USING_OVOS08_MIPI)
    ret = strcmp(SensorName, "ovos08_mipi");
    if (!ret)
    {
        sensor = SENSOR_CREATE_MULTI(ovos08_mipi)();
    }
#endif

#if defined (FH_USING_IMX415_MIPI) || defined (FH_USING_IMX415_MIPI)
    ret = strcmp(SensorName, "imx415_mipi");
    if (!ret)
    {
        sensor = SENSOR_CREATE_MULTI(imx415_mipi)();
    }
#endif

     if (sensor && grpid == FH_GRP_ID)
     {
         ret = FHAdv_Isp_SensorInit(grpid, sensor);
         if (ret)
         {
             printf("Error: FHAdv_Isp_SensorInit failed, ret = %d\n", ret);
         }
     }

    if (sensor == NULL)
        printf("sensor %s not support yet!\n", SensorName);

    return sensor;
}
