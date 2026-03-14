/*
 * OVOS04C10_mipi.h
 *
 *  Created on: Jun 29, 2015
 *      Author: duobao
 */

#ifndef OVOS04C10_MIPI_H_
#define OVOS04C10_MIPI_H_

#include "cfg/ovos04c10_mipi_2lane_400w25.h"
#include "cfg/ovos04c10_mipi_2lane_400w30.h"
#include "cfg/ovos04c10_mipi_2lane_400w25_wdr.h"
#include "cfg/ovos04c10_mipi_2lane_400w30_wdr.h"
#include "cfg/ovos04c10_mipi_2lane_2688x1520p25.h"
#include "cfg/ovos04c10_mipi_2lane_2688x1520p25_wdr.h"
#include "cfg/ovos04c10_mipi_2lane_2688x1520p30.h"
#include "cfg/ovos04c10_mipi_2lane_2688x1520p30_wdr.h"
#include "cfg/ovos04c10_mipi_4lane_400w25.h"
#include "cfg/ovos04c10_mipi_4lane_400w25_wdr.h"
#include "cfg/ovos04c10_mipi_4lane_400w30.h"
#include "cfg/ovos04c10_mipi_4lane_400w30_wdr.h"
#include "cfg/ovos04c10_mipi_4lane_2688x1520p30.h"
#include "cfg/ovos04c10_mipi_4lane_2688x1520p30_wdr.h"
#include "cfg/ovos04c10_mipi_4lane_2688x1520p25.h"
#include "cfg/ovos04c10_mipi_4lane_2688x1520p25_wdr.h"

// 用于0x20的sensor的配置
static SNS_CONFIG_S stSnsConf0[]  =
{
    {
        .u32SnsFmt       = FORMAT_400WP25,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS04C10_MIPI_2688X1520P25_2LANE) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS04C10_MIPI_2688X1520P25_2LANE,
        .u8MipiClkRate  = R_1400_1499,
        .u8MipiLaneNum  = 2,
        .stSnsAttr       = &attr_2l_400w25,
    },

    {
        .u32SnsFmt       = FORMAT_WDR_400WP25,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS04C10_MIPI_2688X1520P25_WDR_2LANE) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS04C10_MIPI_2688X1520P25_WDR_2LANE,
        .u8MipiClkRate  = R_1400_1499,
        .u8MipiLaneNum  = 2,
        .stSnsAttr       = &attr_2l_400w25_wdr,
    },

    {
        .u32SnsFmt       = FORMAT_400WP30,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS04C10_MIPI_2688X1520P30_2LANE) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS04C10_MIPI_2688X1520P30_2LANE,
        .u8MipiClkRate  = R_1400_1499,
        .u8MipiLaneNum  = 2,
        .stSnsAttr       = &attr_2l_400w30,
    },

    {
        .u32SnsFmt       = FORMAT_WDR_400WP30,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS04C10_MIPI_2688X1520P30_WDR_2LANE) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS04C10_MIPI_2688X1520P30_WDR_2LANE,
        .u8MipiClkRate  = R_1400_1499,
        .u8MipiLaneNum  = 2,
        .stSnsAttr       = &attr_2l_400w30_wdr,
    },

    {
        .u32SnsFmt       = FORMAT_2688X1520P25,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS04C10_MIPI_2688X1520P25_2LANE) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS04C10_MIPI_2688X1520P25_2LANE,
        .u8MipiClkRate  = R_1400_1499,
        .u8MipiLaneNum  = 2,
        .stSnsAttr       = &attr_2l_2688x1520p25,
    },

    {
        .u32SnsFmt       = FORMAT_WDR_2688X1520P25,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS04C10_MIPI_2688X1520P25_WDR_2LANE) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS04C10_MIPI_2688X1520P25_WDR_2LANE,
        .u8MipiClkRate  = R_1400_1499,
        .u8MipiLaneNum  = 2,
        .stSnsAttr       = &attr_2l_2688x1520p25_wdr,
    },

    {
        .u32SnsFmt       = FORMAT_2688X1520P30,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS04C10_MIPI_2688X1520P30_2LANE) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS04C10_MIPI_2688X1520P30_2LANE,
        .u8MipiClkRate  = R_1400_1499,
        .u8MipiLaneNum  = 2,
        .stSnsAttr       = &attr_2l_2688x1520p30,
    },

    {
        .u32SnsFmt       = FORMAT_WDR_2688X1520P30,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS04C10_MIPI_2688X1520P30_WDR_2LANE) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS04C10_MIPI_2688X1520P30_WDR_2LANE,
        .u8MipiClkRate  = R_1400_1499,
        .u8MipiLaneNum  = 2,
        .stSnsAttr       = &attr_2l_2688x1520p30_wdr,
    },


    {
        .u32SnsFmt       = FORMAT_400WP30,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS04C10_MIPI_400WP30) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS04C10_MIPI_400WP30,
        .u8MipiClkRate  = R_900_999,
        .u8MipiLaneNum  = 4,
        .stSnsAttr       = &attr_4l_400w30,
    },

    {
        .u32SnsFmt       = FORMAT_WDR_400WP30,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS04C10_MIPI_WDR_400WP30) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS04C10_MIPI_WDR_400WP30,
        .u8MipiClkRate  = R_900_999,
        .u8MipiLaneNum  = 4,
        .stSnsAttr       = &attr_4l_400w30_wdr,
    },

    {
        .u32SnsFmt       = FORMAT_400WP25,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS04C10_MIPI_400WP25) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS04C10_MIPI_400WP25,
        .u8MipiClkRate  = R_900_999,
        .u8MipiLaneNum  = 4,
        .stSnsAttr       = &attr_4l_400w25,
    },

    {
        .u32SnsFmt       = FORMAT_WDR_400WP25,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS04C10_MIPI_WDR_400WP25) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS04C10_MIPI_WDR_400WP25,
        .u8MipiClkRate  = R_900_999,
        .u8MipiLaneNum  = 4,
        .stSnsAttr       = &attr_4l_400w25_wdr,
    },

    {
        .u32SnsFmt       = FORMAT_2688X1520P30,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS04C10_MIPI_2688X1520P30) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS04C10_MIPI_2688X1520P30,
        .u8MipiClkRate  = R_700_799,
        .u8MipiLaneNum  = 4,
        .stSnsAttr       = &attr_4l_2688x1520p30,
    },

    {
        .u32SnsFmt       = FORMAT_WDR_2688X1520P30,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS04C10_MIPI_2688X1520P30_WDR) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS04C10_MIPI_2688X1520P30_WDR,
        .u8MipiClkRate  = R_700_799,
        .u8MipiLaneNum  = 4,
        .stSnsAttr       = &attr_4l_2688x1520p30_wdr,
    },

    {
        .u32SnsFmt       = FORMAT_2688X1520P25,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS04C10_MIPI_2688X1520P25) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS04C10_MIPI_2688X1520P25,
        .u8MipiClkRate  = R_700_799,
        .u8MipiLaneNum  = 4,
        .stSnsAttr       = &attr_4l_2688x1520p25,
    },

    {
        .u32SnsFmt       = FORMAT_WDR_2688X1520P25,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS04C10_MIPI_2688X1520P25_WDR) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS04C10_MIPI_2688X1520P25_WDR,
        .u8MipiClkRate  = R_700_799,
        .u8MipiLaneNum  = 4,
        .stSnsAttr       = &attr_4l_2688x1520p25_wdr,
    },
};

#endif /* OVOS04C10_MIPI_H_ */
