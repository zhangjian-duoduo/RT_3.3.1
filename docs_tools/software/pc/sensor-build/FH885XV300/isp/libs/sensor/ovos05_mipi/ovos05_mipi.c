/*
 * ovos05_mipi.c
 *
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "clock_ops.h"

#include "isp_sensor_if.h"
#include "sensor_ops.h"

#include "isp_enum.h"
#include "mipi_api.h"
#include "ovos05_mipi_param.h"
#include "ovos05_mipi_wdr_param.h"
#include "sensor.h"
#include "ovos05_mipi.h"
#include "sensor_std.h"

#define _NAME(n, s) n##_##s
#define NAME(n) _NAME(n, ovos05_mipi)
#define CCI_ID (0x6c >> 1)
#define CCI_MODE 2

#define REWRITE_GetSensorLtmCurve
#define REWRITE_GetUserSensorAwbGain
#define REWRITE_Sensor_Isconnect
#define FUN_EN_Sensor_WriteEx
#ifdef STITCH_SUPPORT
#define REWRITE_SensorCommonIf
#define FUN_EN_CommonIfGetRegsInfo
#endif
#include "sns_com.h"

#ifdef STITCH_SUPPORT
static int SensorCommonIf(void* pvObj, FH_UINT32 cmd, ISP_SENSOR_COMMON_CMD_DATA0* data0, ISP_SENSOR_COMMON_CMD_DATA1* data1)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);
    int ret = -1;
    switch (cmd)
    {
        case CMD_GET_REGS_INFO:
            if(!pstCtrlPara->bGrpSync)
            {
                printf("not support CMD_GET_REGS_INFO \n");
                break;
            }
            ret = CommonIfGetRegsInfo(pvObj, data0, data1);
            break;
        default:
            ret = -1;
            break;
    }
    return ret;
}
#endif

static int Sensor_Isconnect(void *pvObj, FH_UINT8 u8DeviceId)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);
    SensorDevice*       pstCci      = _sns_get_cci_(pvObj);

    FH_UINT32 u32SnsChipId;

    pstCtrlPara->u8CciDeviceId = u8DeviceId;

    SensorDevice_Init(pstCci, CCI_ID, CCI_MODE, pstCtrlPara->u8CciDeviceId );

    u32SnsChipId = Sensor_Read(pstCci, 0x300c);
    u32SnsChipId |= Sensor_Read(pstCci, 0x300b) << 8;
    u32SnsChipId |= Sensor_Read(pstCci, 0x300a) << 16;

    SensorDevice_Close(pstCci);

    if (u32SnsChipId == 0x530541)
        return 1;
    else
    {
        return 0;
    }
}

//可根据sns_cfg->max_lane_num确定当前使用2lane配置还是4lane配置
static int SetSensorFmt(void *pvObj, FH_SINT32 s32Format)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);
    SensorDevice*       pstCci      = _sns_get_cci_(pvObj);

    FH_SINT32 s32Ret = SnsCom_SetFmt(pvObj, s32Format, CCI_ID, CCI_MODE);

    if (s32Ret)
        return s32Ret;

    pstCtrlPara->u32Vcycle = (Sensor_Read(pstCci, 0x380e) << 8) | (Sensor_Read(pstCci, 0x380f));
    pstCtrlPara->u32FullLineStd = pstCtrlPara->u32Vcycle;

    if (pstCtrlPara->u32WdrFlag == 0)
    {
        pstCtrlPara->u32MinRstLine = 5;
    }
    else
    {
        pstCtrlPara->u32MinRstLine = pstCtrlPara->u32Vcycle / 7 + 5;
    }
    pstCtrlPara->u32MaxIntt = pstCtrlPara->u32Vcycle - pstCtrlPara->u32MinRstLine;
    SetIntt(pvObj, pstCtrlPara->u32MaxIntt, 0);
    SetExposureRatio(pvObj, pstCtrlPara->u32CurrExposureRatio);
    SetGain(pvObj, pstCtrlPara->u32MinAgain, 0);
    pstCtrlPara->u32CurrSnsGain = pstCtrlPara->u32MinAgain;

    return 0;
}

static int GetAEDefault(void* pvObj, Sensor_AE_Default_S *sensAEDefault)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);

    sensAEDefault->minIntt = 1;
    sensAEDefault->maxIntt = pstCtrlPara->u32MaxIntt;
    sensAEDefault->minAGain = pstCtrlPara->u32MinAgain;
    sensAEDefault->maxAGain = 0x3ff7;
    sensAEDefault->fullLineStd = pstCtrlPara->u32FullLineStd;
    sensAEDefault->MinRstLine = pstCtrlPara->u32MinRstLine;

    return 0;
}

// GetGain仅在ae refresh时会重新获取,所以尽量直接从sensor端获取,保证intt的正确性
static int GetGain(void* pvObj)
{
    SensorDevice*       pstCci      = _sns_get_cci_(pvObj);

    int gain, again, dgain, Reg3508, Reg3509, Reg350a, Reg350b;
    Reg3508 = Sensor_Read(pstCci, 0x3508) & 0xff;
    Reg3509 = Sensor_Read(pstCci, 0x3509) & 0xff;
    Reg350a = Sensor_Read(pstCci, 0x350a) & 0xff;
    Reg350b = Sensor_Read(pstCci, 0x350b) & 0xff;

    dgain = ((Reg350a << 8) | Reg350b);  // U.10
    again = ((Reg3508 << 8) | Reg3509);  // U.7

    gain = (dgain * again) >> 11;  // U.17->U.6

    return gain;
}

static int SetGain(void* pvObj, FH_UINT32 again, FH_UINT8 fNo) {
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);
    FH_UINT8  Reg3508, Reg3509, Reg350a, Reg350b;
    FH_UINT32  d_gain;

    again = again << 1;  // U.6 -> U.7
    if (again <= 0x80) {  // 1x
        Reg3508 = 0x00;
        Reg3509 = 0x80;
        Reg350a = 0x04;
        Reg350b = 0x00;
    }
    else if (again <= 0x100) { // 1X~2X a_gain
        Reg3508 = (again >> 8) & 0xff;
        Reg3509 = again & 0xf8;
        d_gain = (again << 10) / ((Reg3508 << 8) | Reg3509);
        Reg350a = (d_gain >> 8) & 0xff;
        Reg350b = d_gain & 0xff;
    }
    else if (again <= 0x200) { // 2X~4X a_gain
        Reg3508 = (again >> 8) & 0xff;
        Reg3509 = again & 0xf0;
        d_gain = (again << 10) / ((Reg3508 << 8) | Reg3509);
        Reg350a = (d_gain >> 8) & 0xff;
        Reg350b = d_gain & 0xff;
    }
    else if (again <= 0x400) { // 4X~8X a_gain
        Reg3508 = (again >> 8) & 0xff;
        Reg3509 = again & 0xe0;
        d_gain = (again << 10) / ((Reg3508 << 8) | Reg3509);
        Reg350a = (d_gain >> 8) & 0xff;
        Reg350b = d_gain & 0xff;
    }
    else if (again < 0x7ff) { // 8X~15.99X a_gain
        Reg3508 = (again >> 8) & 0xff;
        Reg3509 = again & 0xc0;
        d_gain = (again << 10) / ((Reg3508 << 8) | Reg3509);
        Reg350a = (d_gain >> 8) & 0xff;
        Reg350b = d_gain & 0xff;
    }
    else { //15.992X~255.859X again*dgain
        Reg3508 = 0x07;
        Reg3509 = 0xff;
        d_gain = (again << 10) / 0x7ff;  // U.7~U.10
        if (d_gain > 0x3fff) {
            d_gain = 0x3fff;
        }
        Reg350a = (d_gain >> 8) & 0xff;
        Reg350b = d_gain & 0xff;
    }

    if (fNo == 0) {
        Sensor_WriteEx(pvObj, 0x3508, Reg3508, SNS_INT_POS_START);
        Sensor_WriteEx(pvObj, 0x3509, Reg3509, SNS_INT_POS_START);
        Sensor_WriteEx(pvObj, 0x350a, Reg350a, SNS_INT_POS_START);
        Sensor_WriteEx(pvObj, 0x350b, Reg350b, SNS_INT_POS_START);
        if (pstCtrlPara->u32WdrFlag ) {
            Sensor_WriteEx(pvObj, 0x350c, Reg3508, SNS_INT_POS_START);
            Sensor_WriteEx(pvObj, 0x350d, Reg3509, SNS_INT_POS_START);
            Sensor_WriteEx(pvObj, 0x350e, Reg350a, SNS_INT_POS_START);
            Sensor_WriteEx(pvObj, 0x350f, Reg350b, SNS_INT_POS_START);
        }
    }

    return 0;
}

// GetIntt仅在ae refresh时会重新获取,所以尽量直接从sensor端获取,保证intt的正确性
static int GetIntt(void* pvObj)
{
    SensorDevice*       pstCci      = _sns_get_cci_(pvObj);

    int intt, Reg3501, Reg3502;
    Reg3501 = Sensor_Read(pstCci, 0x3501) & 0xff;
    Reg3502 = Sensor_Read(pstCci, 0x3502) & 0xff;
    intt = (Reg3501<<8)|Reg3502;
    return intt;
}

static int Set_short_exp_intt(void* pvObj, FH_UINT32 intt)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);

    FH_UINT32 intt_h, intt_l;

    intt_h = (intt >> 8) & 0xff;
    intt_l = intt & 0xff;

    Sensor_WriteEx(pvObj, 0x3511, intt_h, SNS_INT_POS_START);
    Sensor_WriteEx(pvObj, 0x3512, intt_l, SNS_INT_POS_START);
    pstCtrlPara->u32CurrSnsShortIntt = intt;
    return 0;
}

static int SetIntt(void* pvObj, FH_UINT32 intt, FH_UINT8 fNo)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);

    if (fNo == 0) {  // first frame
        FH_UINT32 intt_h, intt_l;

        intt_h = (intt >> 8) & 0xff;
        intt_l = intt & 0xff;
        Sensor_WriteEx(pvObj, 0x3501, intt_h, SNS_INT_POS_START);
        Sensor_WriteEx(pvObj, 0x3502, intt_l, SNS_INT_POS_START);
        pstCtrlPara->u32CurrSnsIntt = intt;
        if(pstCtrlPara->u32WdrFlag)
        {
            FH_UINT32 short_exp_intt;
            short_exp_intt = MAX(1, ((MIN(intt, pstCtrlPara->u32MaxIntt) << 4) + (pstCtrlPara->u32CurrExposureRatio >> 1)) / pstCtrlPara->u32CurrExposureRatio);
            Set_short_exp_intt(pvObj, short_exp_intt);
        }
    }
    return 0;
}

static int SetSnsFrameH(void* pvObj, FH_UINT32 frameH) {
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);
    SensorDevice*       pstCci      = _sns_get_cci_(pvObj);

    Sensor_Write(pstCci, 0x380e, frameH>>8);
    Sensor_Write(pstCci, 0x380f, frameH&0xff);

    pstCtrlPara->u32Vcycle = frameH;

    return 0;
}

static int SetSensorFlipMirror(void* pvObj, FH_UINT32 sensor_en_stat)
{
    SensorDevice*       pstCci      = _sns_get_cci_(pvObj);

    FH_UINT8 tmp1, tmp2;
    FH_UINT8 flip, mirror;

    flip = sensor_en_stat & 0x1;
    mirror = !(((sensor_en_stat >> 1)) & 0x1);

    tmp1 = Sensor_Read(pstCci, 0x3820);
    tmp2 = Sensor_Read(pstCci, 0x3821);

    tmp1 = (tmp1&0xfb) | (flip << 2);
    tmp2 = (tmp2&0xfb) | (mirror << 2);

    Sensor_Write(pstCci, 0x3820, tmp1);
    Sensor_Write(pstCci, 0x3821, tmp2);

    return 0;
}

static int GetSensorFlipMirror(void* pvObj, FH_UINT32 *sensor_en_stat)
{
    SensorDevice*       pstCci      = _sns_get_cci_(pvObj);

    FH_UINT8 mirror, flip;

    flip = (Sensor_Read(pstCci, 0x3820))>>2 & 0x1;
    mirror = !((Sensor_Read(pstCci, 0x3821))>>2 & 0x1);
    *sensor_en_stat = (mirror << 1) | flip;

    return 0;
}

static FH_UINT32* GetUserSensorAwbGain(void* pvObj, FH_UINT32 idx)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);

    if(pstCtrlPara->u32WdrFlag)
        return (AwbGainWdr)[idx];
    else
        return (AwbGain)[idx];
}

static FH_UINT32* GetSensorLtmCurve(void* pvObj, FH_UINT32 idx)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);

    if (!pstCtrlPara->u32WdrFlag)
    {
        switch (idx)
        {
        case 0:
            return (LtmMapping00);
            break;
        case 1:
            return (LtmMapping01);
            break;
        case 2:
            return (LtmMapping02);
            break;
        case 3:
            return (LtmMapping03);
            break;
        case 4:
            return (LtmMapping04);
            break;
        case 5:
            return (LtmMapping05);
            break;
        case 6:
            return (LtmMapping06);
            break;
        case 7:
            return (LtmMapping07);
            break;
        case 8:
            return (LtmMapping08);
            break;
        case 9:
            return (LtmMapping09);
            break;
        case 10:
            return (LtmMapping10);
        }
        return (LtmMapping00);
    }
    return 0;
}

#define SENSOR_OVOS05_MIPI    "ovos05_mipi"
struct isp_sensor_if ovos05_mipi_sensor_if = {
    .name = SENSOR_OVOS05_MIPI,
    .get_vi_attr = GetSensorViAttr,
    .set_flipmirror = SetSensorFlipMirror,
    .get_flipmirror = GetSensorFlipMirror,
    .set_iris = SetSensorIris,
    .init = Sensor_Init,
    .reset = SensorReset,
    .deinit = Sensor_DeInit,
    .set_sns_fmt = SetSensorFmt,
    .kick = Sensor_Kick,
    .set_sns_reg = SetSensorReg,
    .set_exposure_ratio = SetExposureRatio,
    .get_exposure_ratio = GetExposureRatio,
    .get_sensor_attribute = GetSensorAttribute,
    .set_lane_num_max = SetLaneNumMax,
    .get_sns_reg = GetSensorReg,
    .get_awb_gain = GetSensorAwbGain,
    .set_awb_gain = SetSensorAwbGain,
    .data = 0,
    .common_if = SensorCommonIf,
    .get_sns_ae_default = GetAEDefault,
    .get_sns_ae_info = GetAEInfo,
    .set_sns_intt  = SetIntt,
    .calc_sns_valid_intt = CalcSnsValidIntt,
    .set_sns_gain = SetGain,
    .calc_sns_valid_gain = CalcSnsValidGain,
    .set_sns_frame_height = SetSnsFrameH,
    .get_sensor_mirror_flip_bayer_format = GetMirrorFlipBayerFormat,
    .get_user_awb_gain = GetUserSensorAwbGain,
    .get_user_ltm_curve = GetSensorLtmCurve,
    .is_sensor_connect = Sensor_Isconnect,
};

struct isp_sensor_if *NAME(Sensor_Create)()
{
    SensorCreateInfo info = {
        .pstSnsIf       = &ovos05_mipi_sensor_if,
        .pstSnsCtrlPara = &stSnsDefaultCtrlPara,
        .pVer           = SENSOR_GIT_VERSION,
    };

    return Sensor_Create_Common(&info);
}

void NAME(Sensor_Destroy)(struct isp_sensor_if *s_if)
{
    Sensor_Destroy_Common(s_if);
}
