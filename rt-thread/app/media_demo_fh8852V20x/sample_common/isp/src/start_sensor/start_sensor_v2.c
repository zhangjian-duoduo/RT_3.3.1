#include <isp/isp_sensor_if.h>
#include "sample_common.h"
#include "FHAdv_Isp_mpi_v3.h"

#define SENSOR_CREATE_MULTI(n) Sensor_Create##_##n

#if defined(FH_USING_OVOS02K_MIPI_G0) || defined(FH_USING_OVOS02K_MIPI_G1)
extern struct isp_sensor_if *SENSOR_CREATE_MULTI(ovos02k_mipi)();
#endif

#if defined(FH_USING_OVOS02D_MIPI_G0) || defined(FH_USING_OVOS02D_MIPI_G1)
extern struct isp_sensor_if *SENSOR_CREATE_MULTI(ovos02d_mipi)();
#endif

#if defined(FH_USING_OV2735_MIPI_G0) || defined(FH_USING_OV2735_MIPI_G1)
extern struct isp_sensor_if *SENSOR_CREATE_MULTI(ov2735_mipi)();
#endif

#if defined(FH_USING_SC3335_MIPI_G0) || defined(FH_USING_SC3335_MIPI_G1)
extern struct isp_sensor_if *SENSOR_CREATE_MULTI(sc3335_mipi)();
#endif

#if defined(FH_USING_SC3338_MIPI_G0) || defined(FH_USING_SC3338_MIPI_G1)
extern struct isp_sensor_if *SENSOR_CREATE_MULTI(sc3338_mipi)();
#endif

#if defined(FH_USING_GC2083_MIPI_G0) || defined(FH_USING_GC2083_MIPI_G1)
extern struct isp_sensor_if *SENSOR_CREATE_MULTI(gc2083_mipi)();
#endif

#if defined(FH_USING_DUMMY_SENSOR_G0) || defined(FH_USING_DUMMY_SENSOR_G1)
extern struct isp_sensor_if *SENSOR_CREATE_MULTI(dummy_sensor)();
#endif

struct isp_sensor_if *start_sensor(char *SensorName, int grpid)
{
    int ret;
    struct isp_sensor_if *sensor = NULL;

#if defined(FH_USING_OVOS02K_MIPI_G0) || defined(FH_USING_OVOS02K_MIPI_G1)
    if (!strcmp(SensorName, "ovos02k_mipi"))
    {

        sensor = SENSOR_CREATE_MULTI(ovos02k_mipi)();
    }
#endif

#if defined(FH_USING_OVOS02D_MIPI_G0) || defined(FH_USING_OVOS02D_MIPI_G1)
    if (!strcmp(SensorName, "ovos02d_mipi"))
    {

        sensor = SENSOR_CREATE_MULTI(ovos02d_mipi)();
    }
#endif

#if defined(FH_USING_OV2735_MIPI_G0) || defined(FH_USING_OV2735_MIPI_G1)
    if (!strcmp(SensorName, "ov2735_mipi"))
    {

        sensor = SENSOR_CREATE_MULTI(ov2735_mipi)();
    }
#endif

#if defined(FH_USING_SC3335_MIPI_G0) || defined(FH_USING_SC3335_MIPI_G1)
    if (!strcmp(SensorName, "sc3335_mipi"))
    {

        sensor = SENSOR_CREATE_MULTI(sc3335_mipi)();
    }
#endif

#if defined(FH_USING_SC3338_MIPI_G0) || defined(FH_USING_SC3338_MIPI_G1)
    if (!strcmp(SensorName, "sc3338_mipi"))
    {

        sensor = SENSOR_CREATE_MULTI(sc3338_mipi)();
    }
#endif

#if defined(FH_USING_GC2083_MIPI_G0) || defined(FH_USING_GC2083_MIPI_G1)
    if (!strcmp(SensorName, "gc2083_mipi"))
    {
        sensor = SENSOR_CREATE_MULTI(gc2083_mipi)();
    }
#endif

#if defined(FH_USING_DUMMY_SENSOR_G0) || defined(FH_USING_DUMMY_SENSOR_G1)
    if (!strcmp(SensorName, "dummy_sensor"))
    {
        sensor = SENSOR_CREATE_MULTI(dummy_sensor)();
    }
#endif

    if (sensor && grpid == FH_APP_GRP_ID)
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
