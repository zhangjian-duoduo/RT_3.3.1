/*
 * ovos02k_mipi.h
 *
 *  Created on: Aug 11, 2020
 *      Author: hl
 */

#ifndef OVOS02K_MIPI_H_
#define OVOS02K_MIPI_H_

#include "cfg/ovos02k_mipi_2lane_1080p25.h"
#include "cfg/ovos02k_mipi_2lane_720p25.h"
#include "cfg/ovos02k_mipi_2lane_1080p25_wdr.h"
#include "cfg/ovos02k_mipi_2lane_1080p30.h"
#include "cfg/ovos02k_mipi_2lane_1080p30_wdr.h"
#include "cfg/ovos02k_mipi_2lane_1080p30_wdr_R1A.h"
#include "cfg/ovos02k_mipi_4lane_1080p25.h"
#include "cfg/ovos02k_mipi_4lane_1080p25_wdr.h"
#include "cfg/ovos02k_mipi_4lane_1080p30.h"
#include "cfg/ovos02k_mipi_4lane_1080p30_wdr.h"

// 用于0x20的sensor的配置
static SNS_CONFIG_S stSnsConf0[] =
{
    {
        .u32SnsFmt      = FORMAT_1080P25,
        .u32ConfSize    = sizeof(Sensor_Cfg_OVOS02K_MIPI_1080P25) / 4,
        .pu16CciConf    = Sensor_Cfg_OVOS02K_MIPI_1080P25,
        .u8MipiClkRate  = R_900_999,
        .u8MipiLaneNum  = 4,
        .stSnsAttr      = &attr_4l_1080p25
    },

    {
        .u32SnsFmt      = FORMAT_720P25,
        .u32ConfSize    = sizeof(Sensor_Cfg_OVOS02K_MIPI_720P25_2L) / 4,
        .pu16CciConf    = Sensor_Cfg_OVOS02K_MIPI_720P25_2L,
        .u8MipiClkRate  = R_900_999,
        .u8MipiLaneNum  = 2,
        .stSnsAttr      = &attr_2l_720P25
    },

    {
        .u32SnsFmt      = FORMAT_1080P30,
        .u32ConfSize    = sizeof(Sensor_Cfg_OVOS02K_MIPI_1080P30) / 4,
        .pu16CciConf    = Sensor_Cfg_OVOS02K_MIPI_1080P30,
        .u8MipiClkRate  = R_900_999,
        .u8MipiLaneNum  = 4,
        .stSnsAttr      = &attr_4l_1080p30
    },

    {
        .u32SnsFmt      = FORMAT_WDR_1080P25,
        .u32ConfSize    = sizeof(Sensor_Cfg_OVOS02K_MIPI_WDR_1080P25) / 4,
        .pu16CciConf    = Sensor_Cfg_OVOS02K_MIPI_WDR_1080P25,
        .u8MipiClkRate  = R_900_999,
        .u8MipiLaneNum  = 4,
        .stSnsAttr      = &attr_4l_1080p25_wdr
    },

    {
        .u32SnsFmt      = FORMAT_WDR_1080P30,
        .u32ConfSize    = sizeof(Sensor_Cfg_OVOS02K_MIPI_WDR_1080P30) / 4,
        .pu16CciConf    = Sensor_Cfg_OVOS02K_MIPI_WDR_1080P30,
        .u8MipiClkRate  = R_900_999,
        .u8MipiLaneNum  = 4,
        .stSnsAttr      = &attr_4l_1080p30_wdr
    },

    {
        .u32SnsFmt      = FORMAT_1080P25,
        .u32ConfSize    = sizeof(Sensor_Cfg_OVOS02K_MIPI_1080P25_2L) / 4,
        .pu16CciConf    = Sensor_Cfg_OVOS02K_MIPI_1080P25_2L,
        .u8MipiClkRate  = R_900_999,
        .u8MipiLaneNum  = 2,
        .stSnsAttr      = &attr_2l_1080p25
    },

    {
        .u32SnsFmt      = FORMAT_1080P30,
        .u32ConfSize    = sizeof(Sensor_Cfg_OVOS02K_MIPI_1080P30_2L) / 4,
        .pu16CciConf    = Sensor_Cfg_OVOS02K_MIPI_1080P30_2L,
        .u8MipiClkRate  = R_900_999,
        .u8MipiLaneNum  = 2,
        .stSnsAttr      = &attr_2l_1080p30
    },

    {
        .u32SnsFmt      = FORMAT_WDR_1080P25,
        .u32ConfSize    = sizeof(Sensor_Cfg_OVOS02K_MIPI_WDR_1080P25_2L) / 4,
        .pu16CciConf    = Sensor_Cfg_OVOS02K_MIPI_WDR_1080P25_2L,
        .u8MipiClkRate  = R_900_999,
        .u8MipiLaneNum  = 2,
        .stSnsAttr      = &attr_2l_1080p25_wdr
    },

    {
        .u32SnsFmt      = FORMAT_WDR_1080P30,
        .u32ConfSize    = sizeof(Sensor_Cfg_OVOS02K_MIPI_WDR_1080P30_2L) / 4,
        .pu16CciConf    = Sensor_Cfg_OVOS02K_MIPI_WDR_1080P30_2L,
        .u8MipiClkRate  = R_900_999,
        .u8MipiLaneNum  = 2,
        .stSnsAttr      = &attr_2l_1080p30_wdr
    },

};
// 用于0xb0的sensor的配置
static SNS_CONFIG_S stSnsConf1[] =
{
    {
        .u32SnsFmt      = FORMAT_1080P25,
        .u32ConfSize    = sizeof(Sensor_Cfg_OVOS02K_MIPI_1080P25) / 4,
        .pu16CciConf    = Sensor_Cfg_OVOS02K_MIPI_1080P25,
        .u8MipiClkRate  = R_900_999,
        .u8MipiLaneNum  = 4,
        .stSnsAttr      = &attr_4l_1080p25
    },

    {
        .u32SnsFmt      = FORMAT_720P25,
        .u32ConfSize    = sizeof(Sensor_Cfg_OVOS02K_MIPI_1080P25_2L) / 4,
        .pu16CciConf    = Sensor_Cfg_OVOS02K_MIPI_1080P25_2L,
        .u8MipiClkRate  = R_900_999,
        .u8MipiLaneNum  = 2,
        .stSnsAttr      = &attr_2l_720P25
    },

    {
        .u32SnsFmt      = FORMAT_1080P30,
        .u32ConfSize    = sizeof(Sensor_Cfg_OVOS02K_MIPI_1080P30) / 4,
        .pu16CciConf    = Sensor_Cfg_OVOS02K_MIPI_1080P30,
        .u8MipiClkRate  = R_900_999,
        .u8MipiLaneNum  = 4,
        .stSnsAttr      = &attr_4l_1080p30
    },

    {
        .u32SnsFmt      = FORMAT_WDR_1080P25,
        .u32ConfSize    = sizeof(Sensor_Cfg_OVOS02K_MIPI_WDR_1080P25) / 4,
        .pu16CciConf    = Sensor_Cfg_OVOS02K_MIPI_WDR_1080P25,
        .u8MipiClkRate  = R_900_999,
        .u8MipiLaneNum  = 4,
        .stSnsAttr      = &attr_4l_1080p25_wdr
    },

    {
        .u32SnsFmt      = FORMAT_WDR_1080P30,
        .u32ConfSize    = sizeof(Sensor_Cfg_OVOS02K_MIPI_WDR_1080P30) / 4,
        .pu16CciConf    = Sensor_Cfg_OVOS02K_MIPI_WDR_1080P30,
        .u8MipiClkRate  = R_900_999,
        .u8MipiLaneNum  = 4,
        .stSnsAttr      = &attr_4l_1080p30_wdr
    },

    {
        .u32SnsFmt      = FORMAT_1080P25,
        .u32ConfSize    = sizeof(Sensor_Cfg_OVOS02K_MIPI_1080P25_2L) / 4,
        .pu16CciConf    = Sensor_Cfg_OVOS02K_MIPI_1080P25_2L,
        .u8MipiClkRate  = R_900_999,
        .u8MipiLaneNum  = 2,
        .stSnsAttr      = &attr_2l_1080p25
    },

    {
        .u32SnsFmt      = FORMAT_1080P30,
        .u32ConfSize    = sizeof(Sensor_Cfg_OVOS02K_MIPI_1080P30_2L) / 4,
        .pu16CciConf    = Sensor_Cfg_OVOS02K_MIPI_1080P30_2L,
        .u8MipiClkRate  = R_900_999,
        .u8MipiLaneNum  = 2,
        .stSnsAttr      = &attr_2l_1080p30
    },

    {
        .u32SnsFmt      = FORMAT_WDR_1080P25,
        .u32ConfSize    = sizeof(Sensor_Cfg_OVOS02K_MIPI_WDR_1080P25_2L) / 4,
        .pu16CciConf    = Sensor_Cfg_OVOS02K_MIPI_WDR_1080P25_2L,
        .u8MipiClkRate  = R_900_999,
        .u8MipiLaneNum  = 2,
        .stSnsAttr      = &attr_2l_1080p25_wdr
    },

    {
        .u32SnsFmt      = FORMAT_WDR_1080P30,
        .u32ConfSize    = sizeof(Sensor_Cfg_OVOS02K_MIPI_WDR_1080P30_2L_R1A) / 4,
        .pu16CciConf    = Sensor_Cfg_OVOS02K_MIPI_WDR_1080P30_2L_R1A,
        .u8MipiClkRate  = R_900_999,
        .u8MipiLaneNum  = 2,
        .stSnsAttr      = &attr_2l_1080p30_wdr_r1a
    },

};

#endif // OVOS02K_MIPI_H_






