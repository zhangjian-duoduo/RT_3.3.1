/*
 * ovos08_mipi.c
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
#include "ovos08_mipi.h"
#include "sensor_std.h"

#define _NAME(n, s) n##_##s
#define NAME(n) _NAME(n, ovos08_mipi)

#ifndef REWRITE_stSnsDefaultCtrlPara
static SNS_CTRL_PARA_S stSnsDefaultCtrlPara = {
    .bInited = 0,
    .u32CurrExposureRatio = 256,  // 默认初始化为16倍
    .u32MinRstLine = 5,
    .u32MinAgain = 0x40,
};
#endif

static int SetIntt(void* pvObj, FH_UINT32 intt, FH_UINT8 fNo);
static int GetIntt(void* pvObj);
static int SetGain(void* pvObj, FH_UINT32 again, FH_UINT8 fNo);
static int GetGain(void* pvObj);
static int SetExposureRatio(void* pvObj, FH_UINT32 exposure_ratio);
static int GetExposureRatio(void* pvObj, FH_UINT32 *exposure_ratio);
/********************sensor flow related start****************************/
static void SensorReset(void *pvObj)
{
    return;
}

static int Sensor_Init(void *pvObj, Sensor_Init_t* stInitConf)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);
    SensorDevice*       pstCci      = _sns_get_cci_(pvObj);
    pstCtrlPara->u8CciDeviceId = stInitConf->u8CciDeviceId;
    pstCtrlPara->u8CsiDeviceId  = stInitConf->u8CsiDeviceId;
    SensorDevice_Init(pstCci, (0x6c >> 1), 2, pstCtrlPara->u8CciDeviceId);
    SensorReset(pvObj);
    pstCtrlPara->bInited = 1;
    return 0;
}

static int Sensor_Isconnect(void *pvObj, FH_UINT8 u8DeviceId)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);
    SensorDevice*       pstCci      = _sns_get_cci_(pvObj);;
    FH_UINT32 u32SnsChipId;
    pstCtrlPara->u8CciDeviceId = u8DeviceId;
    SensorDevice_Init(pstCci, (0x6c>>1), 2, pstCtrlPara->u8CciDeviceId );

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

static int Sensor_DeInit(void *pvObj)
{
    SensorDevice*       pstCci      = _sns_get_cci_(pvObj);;
    SensorReset(pvObj);

    return SensorDevice_Close(pstCci);
}

static int SetLaneNumMax(void *pvObj, FH_UINT32 lane_num)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);
#ifdef FPGA_ENV
    pstCtrlPara->u32MaxLaneNum = 4;
#else
    pstCtrlPara->u32MaxLaneNum = 4;
#endif
    return 0;
}

static SNS_CONFIG_S* getSensorConfType(void *pvObj)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);
    pstCtrlPara->u32CurrConfSize  = sizeof(stSnsConf0) / sizeof(SNS_CONFIG_S);
    return stSnsConf0;
}

static int getSensorConf(void *pvObj)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);
    SNS_CONFIG_S*       pstConfGrp  = getSensorConfType(pvObj);
    int i;

    for (i = 0; i < pstCtrlPara->u32CurrConfSize; i++)
    {
        if (pstCtrlPara->u32SnsFmt == pstConfGrp[i].u32SnsFmt)
        {
            if (pstCtrlPara->u32MaxLaneNum >= pstConfGrp[i].u8MipiLaneNum)
            {
                set_clk_rate(SNS_CLK_NAME(pstCtrlPara->u8CsiDeviceId), pstConfGrp[i].u32SnsClk);

                pstCtrlPara->pstCurrConf = &pstConfGrp[i];

                return FH_RET_SENSOR_OK;
            }
        }
    }

    return FH_RET_SENSOR_NO_SUPPORT_FORMAT;
}

static void SetSensorMipiCfg(void* pvObj)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);

    struct mipi_conf config;
    config.frq_range = pstCtrlPara->pstCurrConf->u8MipiClkRate;
    config.raw_type = RAW10;
    config.lane_num = pstCtrlPara->pstCurrConf->u8MipiLaneNum;
    config.mipi_id = pstCtrlPara->u8CsiDeviceId ;

    if (pstCtrlPara->u32WdrFlag == 1)
    {
        config.sensor_mode = NOT_SONY_WDR_USE_WDR;
        config.lf_vc_id = 0;
        config.sf_vc_id = 1;
    }
    else
        config.sensor_mode = NON_WDR;

    mipi_init(&config);
    return;
}

static int GetSensorViAttr(void* pvObj, ISP_VI_ATTR_S *stViAttr)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);
    FH_SINT32 s32Ret;

    if (stViAttr == NULL)
        return FH_RET_SENSOR_NULL_POINTER;

    if (pstCtrlPara->pstCurrConf == NULL)
    {
        s32Ret = getSensorConf(pvObj);

        if (s32Ret != FH_RET_SENSOR_OK)
            return s32Ret;
    }

    stViAttr->u16WndWidth    = pstCtrlPara->pstCurrConf->stSnsAttr->u16WndWidth;
    stViAttr->u16WndHeight   = pstCtrlPara->pstCurrConf->stSnsAttr->u16WndHeight;
    stViAttr->u16InputWidth  = pstCtrlPara->pstCurrConf->stSnsAttr->u16InputWidth;
    stViAttr->u16InputHeight = pstCtrlPara->pstCurrConf->stSnsAttr->u16InputHeight;
    stViAttr->u16PicWidth    = pstCtrlPara->pstCurrConf->stSnsAttr->u16PicWidth;
    stViAttr->u16PicHeight   = pstCtrlPara->pstCurrConf->stSnsAttr->u16PicHeight;
    stViAttr->u16OffsetX     = pstCtrlPara->pstCurrConf->stSnsAttr->u16OffsetX;
    stViAttr->u16OffsetY     = pstCtrlPara->pstCurrConf->stSnsAttr->u16OffsetY;
    stViAttr->enBayerType    = pstCtrlPara->pstCurrConf->stSnsAttr->enBayerType;
    pstCtrlPara->u32CurrHsf  = pstCtrlPara->pstCurrConf->stSnsAttr->u16FrameRate * stViAttr->u16WndHeight;


    if (pstCtrlPara->pstCurrConf->stSnsAttr->u16FrameRate == 12)
        pstCtrlPara->u32CurrHsf = 25 * stViAttr->u16WndHeight / 2;
    else
        pstCtrlPara->u32CurrHsf = pstCtrlPara->pstCurrConf->stSnsAttr->u16FrameRate * stViAttr->u16WndHeight;

    return 0;
}

static int SetSensorFmt(void *pvObj, FH_SINT32 s32Format)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);
    SensorDevice*       pstCci      = _sns_get_cci_(pvObj);;

    SensorDevice_Close(pstCci);
    pstCtrlPara->u32SnsFmt = s32Format;
    pstCtrlPara->u32WdrFlag = (s32Format >> 16) & 1;

    SensorDevice_Init(pstCci, (0x6c>>1), 2, pstCtrlPara->u8CciDeviceId );
    SensorReset(pvObj);

    if (pstCtrlPara->bInited)
    {
        FH_SINT32 i, s32Ret;

        s32Ret = getSensorConf(pvObj);

        if (s32Ret)
            return s32Ret;

        SetSensorMipiCfg(pvObj);

        //统一pll的默认配置
        Sensor_Write(pstCci, 0x0325, 0x48);
        Sensor_Write(pstCci, 0x0328, 0x05);

        for (i = 0; i < pstCtrlPara->pstCurrConf->u32ConfSize; i++)
            Sensor_Write(pstCci, pstCtrlPara->pstCurrConf->pu16CciConf[2 * i],  pstCtrlPara->pstCurrConf->pu16CciConf[2 * i + 1]);

    }
    else
        return -1;

    // pstCtrlPara->u32Vcycle = Sensor_Read(pstCci, 0x380e) << 8;
    // pstCtrlPara->u32Vcycle |= Sensor_Read(pstCci, 0x380f);
    pstCtrlPara->u32Vcycle = pstCtrlPara->pstCurrConf->stSnsAttr->u16WndHeight;
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
/********************sensor flow related end*************************************/

/************************AE&WDR Related start************************************/

static int GetAEInfo(void* pvObj, Sensor_AE_INFO_S *stSnsAEInfo)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);

    stSnsAEInfo->currIntt = GetIntt(pvObj);
    stSnsAEInfo->currAGain = GetGain(pvObj);
    stSnsAEInfo->currHsf = pstCtrlPara->u32CurrHsf;
    stSnsAEInfo->currFrameH = pstCtrlPara->u32Vcycle;
    return 0;
}

static int GetAEDefault(void* pvObj, Sensor_AE_Default_S *sensAEDefault)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);

    sensAEDefault->minIntt = 2;
    sensAEDefault->maxIntt = pstCtrlPara->u32MaxIntt;
    sensAEDefault->minAGain = pstCtrlPara->u32MinAgain;
    sensAEDefault->maxAGain = 0x3dff;
    sensAEDefault->fullLineStd = pstCtrlPara->u32FullLineStd;
    sensAEDefault->MinRstLine = pstCtrlPara->u32MinRstLine;

    return 0;
}

// GetGain仅在ae refresh时会重新获取,所以尽量直接从sensor端获取,保证intt的正确性
static int GetGain(void* pvObj)
{
    SensorDevice*       pstCci      = _sns_get_cci_(pvObj);;
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

static int CalcSnsValidGain(void* pvObj, FH_UINT32 *u32Gain)
{
    return 0;
}


static int SetGain(void* pvObj, FH_UINT32 again, FH_UINT8 fNo) {
    FH_UINT8  Reg3508, Reg3509, Reg350a, Reg350b;
    FH_UINT32  d_gain;
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);
    SensorDevice*       pstCci      = _sns_get_cci_(pvObj);;
    //*************************************************************************
    //  for BLC
    // if (cnt_flag == 0)
    // {
    //     last_gain = gain;
    //     cnt_flag  = 1;
    // }
    // else if (cnt_flag == 1)
    // {
    //     cur_gain = gain;

    //     flag_d = (cur_gain > last_gain) ? 1 : 0;
    //     if ((flag_d == 1) && (gain > 0x1000)) // gain UP and gain>64x
    //     {
    //         reg_ar_wr(0x4000, 0x78);
    //     }
    //     else if ((flag_d == 0) && (gain < 0xf80))
    //     {
    //         reg_ar_wr(0x4000, 0xF8);
    //     }
    //     cnt_flag = 0;
    // }
    //**********************************************************************
    again = again << 1; // U.6 -> U.7
    if (again <= 0x80)
    { // 1x
        Reg3508 = 0x00;
        Reg3509 = 0x80;
        Reg350a = 0x04;
        Reg350b = 0x00;
    }
///////////////////////////////////////Again 不连续，用dgain内插
    else if (again <= 0x100)
    { // 1X~2X a_gain
        Reg3508 = (again >> 8) & 0xff;
        Reg3509 = again & 0xf8;
        d_gain = (again << 10) / ((Reg3508 << 8) | Reg3509);
        Reg350a = (d_gain >> 8) & 0xff;
        Reg350b = d_gain & 0xff;
    }
    else if (again <= 0x200)
    { // 2X~4X a_gain
        Reg3508 = (again >> 8) & 0xff;
        Reg3509 = again & 0xf0;
        d_gain = (again << 10) / ((Reg3508 << 8) | Reg3509);
        Reg350a = (d_gain >> 8) & 0xff;
        Reg350b = d_gain & 0xff;
    }
    else if (again <= 0x400)
    { // 4X~8X a_gain
        Reg3508 = (again >> 8) & 0xff;
        Reg3509 = again & 0xe0;
        d_gain = (again << 10) / ((Reg3508 << 8) | Reg3509);
        Reg350a = (d_gain >> 8) & 0xff;
        Reg350b = d_gain & 0xff;
    }
    else if (again <= 0x7c0)
    { // 8X~15.5X a_gain
        Reg3508 = (again >> 8) & 0xff;
        Reg3509 = again & 0xc0;
        d_gain = (again << 10) / ((Reg3508 << 8) | Reg3509);
        Reg350a = (d_gain >> 8) & 0xff;
        Reg350b = d_gain & 0xff;
    }
    else
    { //15.5X~247.9845X again*dgain
        Reg3508 = 0x07;
        Reg3509 = 0xc0;
        d_gain = (again << 10) / 0x7c0; // U.7~U.10
        if (d_gain > 0x3fff)
        {
            d_gain = 0x3fff;
        }
        Reg350a = (d_gain >> 8) & 0xff;
        Reg350b = d_gain & 0xff;
    }

    if(fNo == 0)
    {
        Sensor_Write(pstCci, 0x3208, 0x00); // group 0 hold start
        Sensor_Write(pstCci, 0x3508, Reg3508);
        Sensor_Write(pstCci, 0x3509, Reg3509);
        Sensor_Write(pstCci, 0x350a, Reg350a);
        Sensor_Write(pstCci, 0x350b, Reg350b);
        if (pstCtrlPara->u32WdrFlag ) {
            Sensor_Write(pstCci, 0x350c, Reg3508);
            Sensor_Write(pstCci, 0x350d, Reg3509);
            Sensor_Write(pstCci, 0x350e, Reg350a);
            Sensor_Write(pstCci, 0x350f, Reg350b);
        }
        Sensor_Write(pstCci, 0x3208, 0x10); // group 0 hold end
        // Sensor_Write(pstCci, 0x320d, 0x00); // manual launch on
        Sensor_Write(pstCci, 0x3208, 0xa0); // launch group 1
    }

    return 0;
}

// GetIntt仅在ae refresh时会重新获取,所以尽量直接从sensor端获取,保证intt的正确性
static int GetIntt(void* pvObj)
{
    SensorDevice*       pstCci      = _sns_get_cci_(pvObj);;
    int intt, Reg3501, Reg3502;
    Reg3501 = Sensor_Read(pstCci, 0x3501) & 0xff;
    Reg3502 = Sensor_Read(pstCci, 0x3502) & 0xff;
    intt = (Reg3501<<8)|Reg3502;
    return intt;
}

static int CalcSnsValidIntt(void* pvObj, FH_UINT32 *u32Intt)
{
    return 0;
}

static int Set_short_exp_intt(void* pvObj, FH_UINT32 intt)
{
    FH_UINT32 intt_h, intt_l;
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);
    SensorDevice*       pstCci      = _sns_get_cci_(pvObj);;

    intt_h = (intt >> 8) & 0xff;
    intt_l = intt & 0xff;

    Sensor_Write(pstCci, 0x3511, intt_h);
    Sensor_Write(pstCci, 0x3512, intt_l);
    pstCtrlPara->u32CurrSnsShortIntt = intt;
    return 0;
}

static int SetIntt(void* pvObj, FH_UINT32 intt, FH_UINT8 fNo)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);
    SensorDevice*       pstCci      = _sns_get_cci_(pvObj);;

    if (fNo == 0) {  // first frame
        FH_UINT32 intt_h, intt_l;

        intt_h = (intt >> 8) & 0xff;
        intt_l = intt & 0xff;
        Sensor_Write(pstCci, 0x3501, intt_h);
        Sensor_Write(pstCci, 0x3502, intt_l);
        pstCtrlPara->u32CurrSnsIntt = intt;
        if(pstCtrlPara->u32WdrFlag)
        {
            FH_UINT32 short_exp_intt;
            short_exp_intt = MAX(2, ((MIN(intt, pstCtrlPara->u32MaxIntt) << 4) + (pstCtrlPara->u32CurrExposureRatio >> 1)) / pstCtrlPara->u32CurrExposureRatio);
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

static int SetExposureRatio(void* pvObj, FH_UINT32 exposure_ratio)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);

    pstCtrlPara->u32CurrExposureRatio = exposure_ratio;
    return 0;
}

static int GetExposureRatio(void* pvObj, FH_UINT32 *exposure_ratio)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);

    *exposure_ratio = (pstCtrlPara->u32CurrSnsIntt  << 4) / MAX(1, pstCtrlPara->u32CurrSnsShortIntt);
    return 0;
}
/************************AE&WDR Related End************************************/

/************************sensor attr related start*****************************/
static int GetSensorAttribute(void* pvObj, char * name, FH_UINT32 *value)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);

    if (strcmp(name, "WDR") == 0) {
        *value = pstCtrlPara->u32WdrFlag;
        return 0;
    }

    return -1;
}
/************************sensor attr related end*******************************/

/***********************fastboot related start*********************************/
static int Sensor_Kick(void* pvObj)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);

    if (pstCtrlPara->bInited)
    {
        // enable sensor output
    }
    return 0;
}

/************************sensor func related start****************************/
static int SetSensorReg(void* pvObj, unsigned short addr, unsigned short data)
{
    SensorDevice*       pstCci      = _sns_get_cci_(pvObj);;
    Sensor_Write(pstCci, addr, data);
    return 0;
}

static int GetSensorReg(void* pvObj, unsigned short addr, unsigned short *data)
{
    SensorDevice*       pstCci      = _sns_get_cci_(pvObj);;
    *data = Sensor_Read(pstCci, addr);
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

    tmp1 = (tmp1 & 0xfb) | (flip << 2);
    tmp2 = (tmp2 & 0xfb) | (mirror << 2);

    Sensor_Write(pstCci, 0x3820, tmp1);
    Sensor_Write(pstCci, 0x3821, tmp2);

    return 0;
}

static int GetSensorFlipMirror(void* pvObj, FH_UINT32 *sensor_en_stat)
{
    SensorDevice*       pstCci      = _sns_get_cci_(pvObj);
    FH_UINT8 mirror, flip;

    flip = (Sensor_Read(pstCci, 0x3820)) >> 2 & 0x1;
    mirror = !((Sensor_Read(pstCci, 0x3821)) >> 2 & 0x1);
    *sensor_en_stat = (mirror << 1) | flip;

    return 0;
}

static int SensorCommonIf(void* pvObj, FH_UINT32 cmd, ISP_SENSOR_COMMON_CMD_DATA0* data0, ISP_SENSOR_COMMON_CMD_DATA1* data1)
{
    int ret = -1;
    return ret;
}
/***********************sensor func related end**********************************/

/***********************adv api related start************************************/
static FH_UINT32* GetMirrorFlipBayerFormat(void* pvObj){return 0;}

static FH_UINT32* GetUserSensorAwbGain(void* pvObj, FH_UINT32 idx){return 0;}
static FH_UINT32* GetSensorLtmCurve(void* pvObj, FH_UINT32 idx){return 0;}
/***********************adv api related end*************************************/

/***********************not used for now start**********************************/
static int SetSensorAwbGain(void* pvObj, FH_UINT32 *awb_gain){return 0;}
static int GetSensorAwbGain(void* pvObj, FH_UINT32 *awb_gain){return 0;}
static int SetSensorIris(void* pvObj, FH_UINT32 iris){return 0;}
/***********************not used for now end************************************/

#define SENSOR_OVOS08_MIPI "ovos08_mipi"
struct isp_sensor_if ovos08_mipi_sensor_if = {
    .name = SENSOR_OVOS08_MIPI,
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
        .pstSnsIf       = &ovos08_mipi_sensor_if,
        .pstSnsCtrlPara = &stSnsDefaultCtrlPara,
        .pVer           = SENSOR_GIT_VERSION,
    };

    return Sensor_Create_Common(&info);
}

void NAME(Sensor_Destroy)(struct isp_sensor_if *s_if)
{
    Sensor_Destroy_Common(s_if);
}

