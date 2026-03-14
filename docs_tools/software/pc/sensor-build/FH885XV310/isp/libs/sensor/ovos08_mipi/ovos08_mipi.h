/*
 * ovos08_mipi.h
 *
 *  Created on: Oct 30, 2018
 *      Author: zy
 */

#ifndef OVOS08_MIPI_H_
#define OVOS08_MIPI_H_

//---------------------------800WP15-------------------------------------------------------

#include "cfg/ovos08_mipi_4lane_1080p60.h"
#include "cfg/ovos08_mipi_4lane_800w15.h"
//#include "cfg/ovos08_mipi_4lane_800w25.h"
#include "cfg/ovos08_mipi_4lane_12bit_800w25.h"
#include "cfg/ovos08_mipi_4lane_12bit_800w30.h"
#include "cfg/ovos08_mipi_4lane_800w15_wdr.h"
#include "cfg/ovos08_mipi_4lane_800w25_wdr.h"
// #include "cfg/ovos08_mipi_4lane_800w30.h"
#include "cfg/ovos08_mipi_4lane_2688X1520p30.h"
#include "cfg/ovos08_mipi_4lane_12bit_2688X1520P30_wdr.h"
#include "cfg/ovos08_mipi_4lane_2592X1952p30_wdr.h"
#include "cfg/ovos08_mipi_4lane_12bit_3072X1728P30_wdr.h"
#include "cfg/ovos08_mipi_4lane_600w20_wdr.h"
#include "cfg/ovos08_mipi_2lane_3072x1728p25.h"
#include "cfg/ovos08_mipi_2lane_2304x1296p25_wdr.h"

static SNS_CONFIG_S stSnsConf0[]  =
{
    {
        .u32SnsFmt       = FORMAT_WDR_2304X1296P25,
        .u32ConfSize     = sizeof(Sensor_Cfg_OVOS08_MIPI_2LANE_2304X1296P25) / 4,
        .pu16CciConf     = Sensor_Cfg_OVOS08_MIPI_2LANE_2304X1296P25,
        .u8MipiClkRate   = R_1400_1499,
        .u8MipiLaneNum   = 2,
        .stSnsAttr       = &attr_2l_2304X1296p25,
        .u32SnsClk       = 24000000,
    },

    {
        .u32SnsFmt       = FORMAT_800WP15,
        .u32ConfSize     = sizeof(Sensor_Cfg_OVOS08_MIPI_4LANE_800W15) / 4,
        .pu16CciConf     = Sensor_Cfg_OVOS08_MIPI_4LANE_800W15,
        .u8MipiClkRate   = R_800_899,
        .u8MipiLaneNum   = 4,
        .stSnsAttr       = &attr_4l_800w15,
        .u32SnsClk       = 27000000,
    },

    {
        .u32SnsFmt        = FORMAT_1080P60,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS08_MIPI_1080P60) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS08_MIPI_1080P60,
        .u8MipiClkRate    = R_800_899,
        .u8MipiLaneNum    = 4,
        .stSnsAttr        = &attr_4l_1080P60,
        .u32SnsClk        = 24000000,
    },

    {
        .u32SnsFmt        = FORMAT_WDR_800WP15,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS08_MIPI_WDR_800WP15) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS08_MIPI_WDR_800WP15,
        .u8MipiClkRate    = R_1000_1099,
        .u8MipiLaneNum    = 4,
        .stSnsAttr        = &attr_wdr_4l_800w15,
        .u32SnsClk        = 27000000,
    },

    {
        .u32SnsFmt        = FORMAT_WDR_800WP25,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS08_MIPI_WDR_800WP25) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS08_MIPI_WDR_800WP25,
        .u8MipiClkRate    = R_1400_1499,
        .u8MipiLaneNum    = 4,
        .stSnsAttr        = &attr_wdr_4l_800w25,
        .u32SnsClk        = 24000000,
    },

    {
        .u32SnsFmt        = FORMAT_800WP25,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS08_MIPI_4LANE_12BIT_800W25) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS08_MIPI_4LANE_12BIT_800W25,
        .u8MipiClkRate    = R_1400_1499,
        .u8MipiLaneNum    = 4,
        .u32SnsClk        = 24000000,
        .stSnsAttr        = &attr_4l_12bit_800w25,
    },

    {
        .u32SnsFmt        = FORMAT_800WP30,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS08_MIPI_4LANE_12BIT_800WP30) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS08_MIPI_4LANE_12BIT_800WP30,
        .u8MipiClkRate    = R_1400_1499,
        .u8MipiLaneNum    = 4,
        .stSnsAttr        = &attr_4l_12bit_800w30,
        .u32SnsClk        = 24000000,
    },

    {
        .u32SnsFmt        = FORMAT_WDR_2688X1520P30,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS08_MIPI_4LANE_12BIT_WDR_3072X1728P30) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS08_MIPI_4LANE_12BIT_WDR_3072X1728P30,
        .u8MipiClkRate    = R_1400_1499,
        .u8MipiLaneNum    = 4,
        .stSnsAttr        = &attr_wdr_4l_12bit_2688X1520P30,
        .u32SnsClk        = 24000000,
    },

    {
        .u32SnsFmt        = FORMAT_WDR_3072X1728P30,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS08_MIPI_4LANE_12BIT_WDR_3072X1728P30) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS08_MIPI_4LANE_12BIT_WDR_3072X1728P30,
        .u8MipiClkRate    = R_1400_1499,
        .u8MipiLaneNum    = 4,
        .stSnsAttr        = &attr_wdr_4l_12bit_3072X1728P30,
        .u32SnsClk        = 24000000,
    },

    {
        .u32SnsFmt        = FORMAT_2688X1520P30,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS08_MIPI_4LANE_12BIT_800WP30) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS08_MIPI_4LANE_12BIT_800WP30,
        .u8MipiClkRate    = R_1400_1499,
        .u8MipiLaneNum    = 4,
        .stSnsAttr        = &attr_4l_2688X1520P30,
        .u32SnsClk        = 24000000,
    },

    {
        .u32SnsFmt        = FORMAT_3072X1728P25,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS08_MIPI_2LANE_3072X1728P25) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS08_MIPI_2LANE_3072X1728P25,
        .u8MipiClkRate    = R_1400_1499,
        .u8MipiLaneNum    = 2,
        .stSnsAttr       = &attr_2l_3072X1728p25,
        .u32SnsClk        = 24000000,
    },

    {
        .u32SnsFmt        = FORMAT_WDR_2592X1952P30,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS08_MIPI_WDR_2592X1952P30) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS08_MIPI_WDR_2592X1952P30,
        .u8MipiClkRate    = R_1400_1499,
        .u8MipiLaneNum    = 4,
        .stSnsAttr        = &attr_4l_2592X1952P30,
        .u32SnsClk        = 27000000,
    },

    {
        .u32SnsFmt        = FORMAT_WDR_600WP20,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS08_MIPI_4LANE_WDR_600WP20) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS08_MIPI_4LANE_WDR_600WP20,
        .u8MipiClkRate    = R_1400_1499,
        .u8MipiLaneNum    = 4,
        .stSnsAttr        = &attr_4l_wdr_600wp20,
        .u32SnsClk        = 24000000,
    },
};
#endif /* OVOS08_MIPI_H_ */
