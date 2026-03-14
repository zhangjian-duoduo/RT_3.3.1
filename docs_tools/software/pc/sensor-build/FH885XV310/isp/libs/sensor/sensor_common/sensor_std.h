#ifndef __SENSOR_STD_H__
#define __SENSOR_STD_H_

#define FH_RET_SENSOR_OK                        0
#define FH_RET_SENSOR_NULL_POINTER             -8001
#define FH_RET_SENSOR_NO_SUPPORT_FORMAT        -8002
#define FH_RET_SENSOR_NOT_INIT                 -8003

#define FH_ERR_NULL_POINTER            FH_RET_SENSOR_NULL_POINTER
#define FH_ERR_ISP_INIT                FH_RET_SENSOR_NOT_INIT
#define FH_ERR_ISP_NO_SUPPORT_FORMAT   FH_RET_SENSOR_NO_SUPPORT_FORMAT

#endif