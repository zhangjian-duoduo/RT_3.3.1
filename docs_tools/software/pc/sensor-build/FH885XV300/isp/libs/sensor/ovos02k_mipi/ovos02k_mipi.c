/*
 * ovos02k_mipi.c
 *
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "isp_sensor_if.h"
#include "sensor_ops.h"
#include "clock_ops.h"
#include "isp_enum.h"
#include "mipi_api.h"
#include "sensor.h"
#include "ovos02k_mipi.h"
#include "sensor_std.h"

#define _NAME(n, s) n##_##s
#define NAME(n) _NAME(n, ovos02k_mipi)
#define CCI_ID      (0x6c >> 1)
#define CCI_MODE    2

#define REWRITE_getSensorConfType
#define REWRITE_CalcSnsValidGain
#define REWRITE_Sensor_Isconnect
#define FUN_EN_Sensor_WriteEx
#ifdef STITCH_SUPPORT
#define REWRITE_SensorCommonIf
#define FUN_EN_CommonIfGetRegsInfo
#endif

static int Sensor_Isconnect(void *pvObj, FH_UINT8 u8DeviceId)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);
    SensorDevice*       pstCci      = _sns_get_cci_(pvObj);
    FH_UINT32 u32SnsChipId;

    pstCtrlPara->u8CciDeviceId = u8DeviceId;
    SensorDevice_Init(pstCci, CCI_ID, CCI_MODE, pstCtrlPara->u8CciDeviceId);

    u32SnsChipId = Sensor_Read(pstCci, 0x300c);
    u32SnsChipId |= Sensor_Read(pstCci, 0x300b) << 8;
    u32SnsChipId |= Sensor_Read(pstCci, 0x300a) << 16;

    SensorDevice_Close(pstCci);

    if (u32SnsChipId == 0x530243)
        return 1;
    else
    {
        return 0;
    }
}

static SNS_CONFIG_S* getSensorConfType(void *pvObj)
{
#define SNS_CHIP_VER0 0x20
#define SNS_CHIP_VER1 0xb0

    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);
    SensorDevice*       pstCci      = _sns_get_cci_(pvObj);

    FH_UINT32 u32SnsChipId = Sensor_Read(pstCci, 0x302a);

    if (u32SnsChipId == SNS_CHIP_VER0)
    {
        pstCtrlPara->u32CurrConfSize  = sizeof(stSnsConf0) / sizeof(SNS_CONFIG_S);
        return stSnsConf0;
    }
    else if (u32SnsChipId == SNS_CHIP_VER1)
    {
        pstCtrlPara->u32CurrConfSize  = sizeof(stSnsConf1) / sizeof(SNS_CONFIG_S);
        return stSnsConf1;
    }
    else
    {
        pstCtrlPara->u32CurrConfSize  = sizeof(stSnsConf0) / sizeof(SNS_CONFIG_S);
        return stSnsConf0;
    }
}

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

//可根据sns_cfg->max_lane_num确定当前使用2lane配置还是4lane配置
static int SetSensorFmt(void *pvObj, FH_SINT32 s32Format)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);
    SensorDevice*       pstCci      = _sns_get_cci_(pvObj);
    FH_SINT32 s32Ret = SnsCom_SetFmt(pvObj, s32Format, CCI_ID, CCI_MODE);

    if (s32Ret)
        return s32Ret;

    pstCtrlPara->u32Vcycle = Sensor_Read(pstCci, 0x380e) << 8;
    pstCtrlPara->u32Vcycle |= Sensor_Read(pstCci, 0x380f);
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

static int GetAEDefault(void *pvObj, Sensor_AE_Default_S *stSnsAEDefault)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);

    stSnsAEDefault->minIntt         = 1;
    stSnsAEDefault->maxIntt         = pstCtrlPara->u32MaxIntt;
    stSnsAEDefault->minAGain        = pstCtrlPara->u32MinAgain;
    stSnsAEDefault->maxAGain        = 0xffff;
    stSnsAEDefault->fullLineStd     = pstCtrlPara->u32FullLineStd;
    stSnsAEDefault->MinRstLine      = pstCtrlPara->u32MinRstLine;

    return 0;
}

// GetGain仅在ae refresh时会重新获取,所以尽量直接从sensor端获取,保证intt的正确性
static int GetGain(void *pvObj)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);

    return pstCtrlPara->u32CurrSnsGain;
}

static int CalcSnsValidGain(void *pvObj, FH_UINT32 *u32Gain)
{
    *u32Gain = (*u32Gain >> 2) << 2;  // U.6->U.4->U.6

    return 0;
}

static int SetGain(void *pvObj, FH_UINT32 again, FH_UINT8 fNo)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);
    SensorDevice*       pstCci      = _sns_get_cci_(pvObj);
    // gain U10.6
    FH_UINT8 Reg3508, Reg3509, Reg350a, Reg350b, Reg350c;
    FH_UINT8 Reg376c, Reg3c55, Reg73fe, Reg73ff;
    FH_UINT32 d_gain, ac_gain, cv_gain, gain_thresh;
    // printf(" set SF Again = %d fNo=%d, u32WdrFlag=%d\n", again, fNo, u32WdrFlag);
    again = again >> 2; // U.6 -> U.4
    pstCtrlPara->u32CurrSnsGain = again << 2;

    if (fNo == 0)
    {
        if (pstCtrlPara->u32WdrFlag == 1)
        {
            //wdr mode 固定HCG模式由于：1.LCG/HCG切换闪烁;2.Group写入方式长帧会很暗。
            if (again <= 0x10)
            { // 1x
                Reg3508 = 0x01;
                Reg3509 = 0x00;
                Reg350a = 0x01;
                Reg350b = 0x00;
                Reg350c = 0x00;
                Reg376c = 0x00;
                Reg3c55 = 0xcb;
            }
            else if (again <= 0xf8)
            { // 1X~15.5X a_gain
                Reg3508 = (again >> 4) & 0x0f;
                Reg3509 = (again & 0x0f) << 4;
                Reg350a = 0x01;
                Reg350b = 0x00;
                Reg350c = 0x00;
                Reg376c = 0x00;
                Reg3c55 = 0xcb;
            }
            else if (again > 0xf8)
            { // 1X~248X a_gain
                Reg3508 = 0x0f;
                Reg3509 = 0x80;
                Reg376c = 0x00;
                Reg3c55 = 0xcb;
                d_gain = (again << 10) / 0xf8; // U.4~U.10
                if (d_gain > 0x3fff)
                {
                    d_gain = 0x3fff;
                }
                Reg350a = (d_gain >> 10) & 0x0f;
                Reg350b = (d_gain >> 2) & 0xff;
                Reg350c = (d_gain & 0x03) << 6;
            }
            // Sensor_Write(pstCci, 0x3208, 0x00); // group 0 hold start
            Sensor_WriteEx(pvObj, 0x3508, Reg3508, SNS_INT_POS_START);
            Sensor_WriteEx(pvObj, 0x3509, Reg3509, SNS_INT_POS_START);
            Sensor_WriteEx(pvObj, 0x350a, Reg350a, SNS_INT_POS_START);
            Sensor_WriteEx(pvObj, 0x350b, Reg350b, SNS_INT_POS_START);
            Sensor_WriteEx(pvObj, 0x350c, Reg350c, SNS_INT_POS_START);
            Sensor_WriteEx(pvObj, 0x3548, Reg3508, SNS_INT_POS_START);
            Sensor_WriteEx(pvObj, 0x3549, Reg3509, SNS_INT_POS_START);
            Sensor_WriteEx(pvObj, 0x354a, Reg350a, SNS_INT_POS_START);
            Sensor_WriteEx(pvObj, 0x354b, Reg350b, SNS_INT_POS_START);
            Sensor_WriteEx(pvObj, 0x354c, Reg350c, SNS_INT_POS_START);
            Sensor_WriteEx(pvObj, 0x376c, Reg376c, SNS_INT_POS_START);
            Sensor_WriteEx(pvObj, 0x3c55, Reg3c55, SNS_INT_POS_START);
            // Sensor_Write(pstCci, 0x3208, 0x10); // group 0 hold end
            // Sensor_Write(pstCci, 0x320d, 0x00); // manual launch on
            // Sensor_Write(pstCci, 0x3208, 0xe0); // launch group 1
        }
        else
        {
            //线性模式下可以通过group写入方式切换LCG/HCG，为了解决切换时的闪烁ae必须隔帧做
            Reg73fe = Sensor_Read(pstCci, 0x73fe);
            Reg73ff = Sensor_Read(pstCci, 0x73ff);
            cv_gain = (Reg73fe << 8) | Reg73ff;
            gain_thresh = (0xf8 * (cv_gain >> 4)) >> 4;
            if (again <= 0x10)
            { // 1x
                Reg3508 = 0x01;
                Reg3509 = 0x00;
                Reg350a = 0x01;
                Reg350b = 0x00;
                Reg350c = 0x00;
                Reg376c = 0x30;
                Reg3c55 = 0x08;
            }
            else if (again <= 0x60)
            { // 1X~15.5X a_gain
                Reg3508 = (again >> 4) & 0x0f;
                Reg3509 = (again & 0x0f) << 4;
                Reg350a = 0x01;
                Reg350b = 0x00;
                Reg350c = 0x00;
                Reg376c = 0x30;
                Reg3c55 = 0x08;
            }
            else if (again <= gain_thresh)
            { // 4X~65X a_gain
                Reg376c = 0x00; //LCG:1x -> HCG:4.207x
                Reg3c55 = 0xcb;
                ac_gain = (again << 8) / MAX(cv_gain, 0x400); //again/4x
                Reg3508 = (ac_gain >> 4) & 0x0f;
                Reg3509 = (ac_gain & 0x0f) << 4;
                Reg350a = 0x01;
                Reg350b = 0x00;
                Reg350c = 0x00;
            }
            else if (again > gain_thresh)
            { // 65X~1028X a_gain
                Reg3508 = 0x0f;
                Reg3509 = 0x80;
                Reg376c = 0x00;
                Reg3c55 = 0xcb;
                d_gain = (again << 10) / MAX(gain_thresh, 0x400); // U.4~U.10
                if (d_gain > 0x3fff)
                {
                    d_gain = 0x3fff;
                }
                Reg350a = (d_gain >> 10) & 0x0f;
                Reg350b = (d_gain >> 2) & 0xff;
                Reg350c = (d_gain & 0x03) << 6;
            }
            Sensor_WriteEx(pvObj, 0x3208, 0x00   , SNS_INT_POS_START); // group 0 hold start
            Sensor_WriteEx(pvObj, 0x3508, Reg3508, SNS_INT_POS_START);
            Sensor_WriteEx(pvObj, 0x3509, Reg3509, SNS_INT_POS_START);
            Sensor_WriteEx(pvObj, 0x350a, Reg350a, SNS_INT_POS_START);
            Sensor_WriteEx(pvObj, 0x350b, Reg350b, SNS_INT_POS_START);
            Sensor_WriteEx(pvObj, 0x350c, Reg350c, SNS_INT_POS_START);
            Sensor_WriteEx(pvObj, 0x3208, 0x10   , SNS_INT_POS_START); // group 0 hold end
            Sensor_WriteEx(pvObj, 0x3208, 0x01   , SNS_INT_POS_START); // group 1 hold start
            Sensor_WriteEx(pvObj, 0x376c, Reg376c, SNS_INT_POS_START);
            Sensor_WriteEx(pvObj, 0x3c55, Reg3c55, SNS_INT_POS_START);
            Sensor_WriteEx(pvObj, 0x3208, 0x11   , SNS_INT_POS_START); // group 1 hold end
            Sensor_WriteEx(pvObj, 0x3209, 0x01   , SNS_INT_POS_START); // saty in group 0 for 1 frame
            Sensor_WriteEx(pvObj, 0x320a, 0x01   , SNS_INT_POS_START); // saty in group 1 for 1 frame
            Sensor_WriteEx(pvObj, 0x320d, 0x05   , SNS_INT_POS_START); // Bit[1:0]:1, return to group 1, Bit[2]:context switch enable; [7:4]:0, loop number is 1
            Sensor_WriteEx(pvObj, 0x320e, 0xa0   , SNS_INT_POS_START); //delay launch group 0, and auto switch to group 1
        }
    }

    return 0;
}

// GetIntt仅在ae refresh时会重新获取,所以尽量直接从sensor端获取,保证intt的正确性
static int GetIntt(void *pvObj)
{
    SensorDevice*       pstCci      = _sns_get_cci_(pvObj);
    int intt, Reg3501, Reg3502;

    Reg3501 = Sensor_Read(pstCci, 0x3501) & 0xff;
    Reg3502 = Sensor_Read(pstCci, 0x3502) & 0xff;
    intt = (Reg3501<<8)|Reg3502;

    return intt;
}

static int Set_short_exp_intt(void *pvObj, FH_UINT32 intt)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);
    FH_UINT32 intt_h, intt_l;

    intt_h = (intt >> 8) & 0xff;
    intt_l = intt & 0xff;

    Sensor_WriteEx(pvObj, 0x3541, intt_h, SNS_INT_POS_START);
    Sensor_WriteEx(pvObj, 0x3542, intt_l, SNS_INT_POS_START);
    pstCtrlPara->u32CurrSnsShortIntt = intt;

    return 0;
}

static int SetIntt(void *pvObj, FH_UINT32 intt, FH_UINT8 fNo)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);
    /****** Set Intt_LEF ******/
    FH_UINT32 Reg3501, Reg3502;
    intt = MAX(1, intt);
    if (fNo == 0)
    {
        Reg3501 = (intt >> 8) & 0xff;
        Reg3502 = intt & 0xff;
        Sensor_WriteEx(pvObj, 0x3501, Reg3501, SNS_INT_POS_START);
        Sensor_WriteEx(pvObj, 0x3502, Reg3502, SNS_INT_POS_START);
        pstCtrlPara->u32CurrSnsIntt = intt;
        if(pstCtrlPara->u32WdrFlag)
        {
            FH_UINT32 short_exp_intt;
            short_exp_intt = MAX(1, ((MIN(intt, pstCtrlPara->u32MaxIntt) << 4)) / pstCtrlPara->u32CurrExposureRatio);
            Set_short_exp_intt(pvObj, short_exp_intt);
        }
    }
    return 0;
}

static int SetSnsFrameH(void *pvObj, FH_UINT32 frameH)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);
    SensorDevice*       pstCci      = _sns_get_cci_(pvObj);
    Sensor_Write(pstCci, 0x380e, frameH >> 8);
    Sensor_Write(pstCci, 0x380f, frameH & 0xff);
    pstCtrlPara->u32Vcycle = frameH;

    return 0;
}

static int SetSensorFlipMirror(void *pvObj, FH_UINT32 sensor_en_stat)
{
    SensorDevice*       pstCci      = _sns_get_cci_(pvObj);
    FH_UINT8 tmp1;
    FH_UINT8 flip, mirror;

    Sensor_Write(pstCci, 0x100, 0x0);  // stream off
    flip = sensor_en_stat & 0x1;
    mirror = (sensor_en_stat >> 1) & 0x1;

    tmp1 = Sensor_Read(pstCci, 0x3820);  // bit[1]:mirror  bit[2]:flip
    tmp1 = (tmp1&0xf9) | ((1-mirror) << 1) | (flip << 2);

    Sensor_Write(pstCci, 0x3820, tmp1);
    Sensor_Write(pstCci, 0x100, 0x1);  // stream on

    return 0;
}

static int GetSensorFlipMirror(void *pvObj, FH_UINT32 *sensor_en_stat)
{
    SensorDevice* pstCci = _sns_get_cci_(pvObj);
    FH_UINT8 mirror, flip;

    mirror = 1 - ((Sensor_Read(pstCci, 0x3820)>>1) & 0x1);
    flip = (Sensor_Read(pstCci, 0x3820)>>2) & 0x1;
    *sensor_en_stat = (mirror << 1) | flip;

    return 0;
}

#define SENSOR_OVOS02K_MIPI "ovos02k_mipi"
struct isp_sensor_if ovos02k_mipi_sensor_if_default = {
    .name = SENSOR_OVOS02K_MIPI,
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
    .para = 0,
    .common_if = SensorCommonIf,
    .get_sns_ae_default = GetAEDefault,
    .get_sns_ae_info = GetAEInfo,
    .set_sns_intt = SetIntt,
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
        .pstSnsIf       = &ovos02k_mipi_sensor_if_default,
        .pstSnsCtrlPara = &stSnsDefaultCtrlPara,
        .pVer           = SENSOR_GIT_VERSION,
    };

    return Sensor_Create_Common(&info);
}

void NAME(Sensor_Destroy)(struct isp_sensor_if *s_if)
{
    Sensor_Destroy_Common(s_if);
}
