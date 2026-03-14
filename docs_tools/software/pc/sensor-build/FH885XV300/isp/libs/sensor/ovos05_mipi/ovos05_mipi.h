/*
* ovos05_mipi.h
*
*  Created on: Jun 29, 2015
*      Author: duobao
*/

#ifndef OVOS05_MIPI_H_
#define OVOS05_MIPI_H_

#include "cfg/ovos05_mipi_2lane_400w20.h"
#include "cfg/ovos05_mipi_2lane_400w25.h"
#include "cfg/ovos05_mipi_2lane_400w30.h"
#include "cfg/ovos05_mipi_2lane_400w25_wdr.h"
#include "cfg/ovos05_mipi_2lane_400w30_wdr.h"
#include "cfg/ovos05_mipi_2lane_500w12p5.h"
#include "cfg/ovos05_mipi_2lane_500w25.h"
#include "cfg/ovos05_mipi_2lane_2304X1296P25.h"
#include "cfg/ovos05_mipi_2lane_2304X1296P25_wdr.h"
#include "cfg/ovos05_mipi_4lane_1080p20.h"
#include "cfg/ovos05_mipi_4lane_1080p25.h"
#include "cfg/ovos05_mipi_4lane_1536p25_wdr.h"
#include "cfg/ovos05_mipi_4lane_1536p30_wdr.h"
#include "cfg/ovos05_mipi_4lane_1536p40_wdr.h"
#include "cfg/ovos05_mipi_4lane_400w25.h"
#include "cfg/ovos05_mipi_4lane_400w25_wdr.h"
#include "cfg/ovos05_mipi_4lane_400w30.h"
#include "cfg/ovos05_mipi_4lane_400w30_wdr.h"
#include "cfg/ovos05_mipi_4lane_500w12p5_wdr.h"
#include "cfg/ovos05_mipi_4lane_500w15.h"
#include "cfg/ovos05_mipi_4lane_500w15_wdr.h"
#include "cfg/ovos05_mipi_4lane_500w20.h"
#include "cfg/ovos05_mipi_4lane_500w20_wdr.h"
#include "cfg/ovos05_mipi_4lane_500w25_wdr.h"
#include "cfg/ovos05_mipi_4lane_12bit_500w30_wdr.h"
#include "cfg/ovos05_mipi_2lane_2688x1520p30.h"
#include "cfg/ovos05_mipi_2lane_2688x1520p30_wdr.h"
#include "cfg/ovos05_mipi_2lane_2688x1520p25.h"
#include "cfg/ovos05_mipi_2lane_2688x1520p25_wdr.h"
// 用于0x20的sensor的配置
static SNS_CONFIG_S stSnsConf0[]  =
{

    {
        .u32SnsFmt       = FORMAT_WDR_500W12P5,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS05_MIPI_WDR_500W12P5) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS05_MIPI_WDR_500W12P5,
        .u8MipiClkRate  = R_900_999,
        .u8MipiLaneNum  = 4,
        .stSnsAttr       = &attr_4l_wdr_500w12
    },

    {
        .u32SnsFmt       = FORMAT_WDR_1536P25,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS05_MIPI_WDR_1536P25) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS05_MIPI_WDR_1536P25,
        .u8MipiClkRate  = R_900_999,
        .u8MipiLaneNum  = 4,
        .stSnsAttr       = &attr_4l_wdr_1536p25
    },

    {
        .u32SnsFmt       = FORMAT_WDR_1536P30,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS05_MIPI_WDR_1536P30) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS05_MIPI_WDR_1536P30,
        .u8MipiClkRate  = R_900_999,
        .u8MipiLaneNum  = 4,
        .stSnsAttr       = &attr_4l_wdr_1536p30
    },

    {
        .u32SnsFmt       = FORMAT_WDR_1536P40,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS05_MIPI_WDR_1536P40) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS05_MIPI_WDR_1536P40,
        .u8MipiClkRate  = R_900_999,
        .u8MipiLaneNum  = 4,
        .stSnsAttr       = &attr_4l_wdr_1536p40
    },

    {
        .u32SnsFmt       = FORMAT_WDR_400WP25,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS05_MIPI_WDR_400WP25) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS05_MIPI_WDR_400WP25,
        .u8MipiClkRate  = R_900_999,
        .u8MipiLaneNum  = 4,
        .stSnsAttr       = &attr_4l_wdr_400wp25
    },

    {
        .u32SnsFmt       = FORMAT_WDR_400WP30,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS05_MIPI_WDR_400WP30) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS05_MIPI_WDR_400WP30,
        .u8MipiClkRate  = R_900_999,
        .u8MipiLaneNum  = 4,
        .stSnsAttr       = &attr_4l_wdr_400wp30
    },

    {
        .u32SnsFmt       = FORMAT_WDR_500WP15,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS05_MIPI_WDR_500WP15) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS05_MIPI_WDR_500WP15,
        .u8MipiClkRate  = R_900_999,
        .u8MipiLaneNum  = 4,
        .stSnsAttr       = &attr_4l_wdr_500wp15
    },

    {
        .u32SnsFmt       = FORMAT_WDR_500WP20,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS05_MIPI_WDR_500WP20) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS05_MIPI_WDR_500WP20,
        .u8MipiClkRate  = R_900_999,
        .u8MipiLaneNum  = 4,
        .stSnsAttr       = &attr_4l_wdr_500wp20
    },

    {
        .u32SnsFmt       = FORMAT_WDR_500WP25,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS05_MIPI_WDR_500WP25) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS05_MIPI_WDR_500WP25,
        .u8MipiClkRate  = R_900_999,
        .u8MipiLaneNum  = 4,
        .stSnsAttr       = &attr_4l_wdr_500wp25
    },

    {
        .u32SnsFmt       = FORMAT_WDR_500WP30,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS05_MIPI_12BIT_WDR_500WP30) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS05_MIPI_12BIT_WDR_500WP30,
        .u8MipiClkRate  = R_1400_1499,
        .u8MipiLaneNum  = 4,
        .stSnsAttr       = &attr_4l_12bit_wdr_500wp30
    },

    {
        .u32SnsFmt       = FORMAT_400WP25,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS05_MIPI_400WP25) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS05_MIPI_400WP25,
        .u8MipiClkRate  = R_500_599,
        .u8MipiLaneNum  = 4,
        .stSnsAttr       = &attr_4l_400wp25
    },

    {
        .u32SnsFmt       = FORMAT_1080P25,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS05_MIPI_400WP25) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS05_MIPI_400WP25,
        .u8MipiClkRate  = R_500_599,
        .u8MipiLaneNum  = 4,
        .stSnsAttr       = &attr_4l_1080p25
    },

    {
        .u32SnsFmt       = FORMAT_400WP30,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS05_MIPI_400WP30) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS05_MIPI_400WP30,
        .u8MipiClkRate  = R_500_599,
        .u8MipiLaneNum  = 4,
        .stSnsAttr       = &attr_4l_400wp30
    },

    {
        .u32SnsFmt       = FORMAT_500WP15,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS05_MIPI_500WP15) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS05_MIPI_500WP15,
        .u8MipiClkRate  = R_500_599,
        .u8MipiLaneNum  = 4,
        .stSnsAttr       = &attr_4l_500wp15
    },

    {
        .u32SnsFmt       = FORMAT_500WP20,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS05_MIPI_500WP20) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS05_MIPI_500WP20,
        .u8MipiClkRate  = R_500_599,
        .u8MipiLaneNum  = 4,
        .stSnsAttr       = &attr_4l_500wp20
    },

    {
        .u32SnsFmt       = FORMAT_1080P20,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS05_MIPI_500WP20) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS05_MIPI_500WP20,
        .u8MipiClkRate  = R_500_599,
        .u8MipiLaneNum  = 4,
        .stSnsAttr       = &attr_4l_1080p20
    },

    {
        .u32SnsFmt       = FORMAT_WDR_400WP30,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS05_MIPI_2LANE_400WP30_WDR) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS05_MIPI_2LANE_400WP30_WDR,
        .u8MipiClkRate  = R_1400_1499,
        .u8MipiLaneNum  = 2,
        .stSnsAttr       = &attr_2l_wdr_400wp25
    },

    {
        .u32SnsFmt       = FORMAT_WDR_400WP25,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS05_MIPI_2LANE_400WP25_WDR) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS05_MIPI_2LANE_400WP25_WDR,
        .u8MipiClkRate  = R_1400_1499,
        .u8MipiLaneNum  = 2,
        .stSnsAttr       = &attr_2l_wdr_400wp30
    },

    {
        .u32SnsFmt       = FORMAT_2304X1296P25,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS05_MIPI_2LANE_400WP25) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS05_MIPI_2LANE_400WP25,
        .u8MipiClkRate  = R_1400_1499,
        .u8MipiLaneNum  = 2,
        .stSnsAttr       = &attr_2l_2304x1296p25
    },

    {
        .u32SnsFmt       = FORMAT_WDR_2304X1296P25,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS05_MIPI_2LANE_400WP25_WDR) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS05_MIPI_2LANE_400WP25_WDR,
        .u8MipiClkRate  = R_1400_1499,
        .u8MipiLaneNum  = 2,
        .stSnsAttr       = &attr_2l_wdr_2304x1296p25
    },

    {
        .u32SnsFmt       = FORMAT_400WP20,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS05_MIPI_2LANE_400WP20) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS05_MIPI_2LANE_400WP20,
        .u8MipiClkRate  = R_900_999,
        .u8MipiLaneNum  = 2,
        .stSnsAttr       = &attr_2l_400wp20
    },

    {
        .u32SnsFmt       = FORMAT_500W12P5,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS05_MIPI_2LANE_500W12P5) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS05_MIPI_2LANE_500W12P5,
        .u8MipiClkRate  = R_900_999,
        .u8MipiLaneNum  = 2,
        .stSnsAttr       = &attr_2l_500wp12
    },

    {
        .u32SnsFmt       = FORMAT_500WP25,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS05_MIPI_2LANE_500WP25) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS05_MIPI_2LANE_500WP25,
        .u8MipiClkRate  = R_1100_1199,
        .u8MipiLaneNum  = 2,
        .stSnsAttr       = &attr_2l_500wp25
    },

    {
        .u32SnsFmt       = FORMAT_400WP30,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS05_MIPI_2LANE_400WP30) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS05_MIPI_2LANE_400WP30,
        .u8MipiClkRate  = R_1400_1499,
        .u8MipiLaneNum  = 2,
        .stSnsAttr       = &attr_2l_400wp30
    },

    {
        .u32SnsFmt       = FORMAT_400WP25,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS05_MIPI_2LANE_400WP25) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS05_MIPI_2LANE_400WP25,
        .u8MipiClkRate  = R_1400_1499,
        .u8MipiLaneNum  = 2,
        .stSnsAttr       = &attr_2l_400wp25
    },

    {
        .u32SnsFmt       = FORMAT_WDR_2688X1520P30,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS05_MIPI_2LANE_2688X1520P30_WDR) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS05_MIPI_2LANE_2688X1520P30_WDR,
        .u8MipiClkRate  = R_1400_1499,
        .u8MipiLaneNum  = 2,
        .stSnsAttr       = &attr_2l_2688X1520p30_wdr
    },

    {
        .u32SnsFmt       = FORMAT_2688X1520P30,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS05_MIPI_2LANE_2688X1520P30) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS05_MIPI_2LANE_2688X1520P30,
        .u8MipiClkRate  = R_1000_1099,
        .u8MipiLaneNum  = 2,
        .stSnsAttr       = &attr_2l_2688X1520p30
    },

    {
        .u32SnsFmt       = FORMAT_WDR_2688X1520P25,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS05_MIPI_2LANE_2688X1520P25_WDR) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS05_MIPI_2LANE_2688X1520P25_WDR,
        .u8MipiClkRate  = R_1400_1499,
        .u8MipiLaneNum  = 2,
        .stSnsAttr       = &attr_2l_2688X1520p25_wdr
    },

    {
        .u32SnsFmt       = FORMAT_2688X1520P25,
        .u32ConfSize      = sizeof(Sensor_Cfg_OVOS05_MIPI_2LANE_2688X1520P25) / 4,
        .pu16CciConf      = Sensor_Cfg_OVOS05_MIPI_2LANE_2688X1520P25,
        .u8MipiClkRate  = R_1000_1099,
        .u8MipiLaneNum  = 2,
        .stSnsAttr       = &attr_2l_2688X1520p25
    },
};

#endif /* OVOS05_MIPI_H_ */
