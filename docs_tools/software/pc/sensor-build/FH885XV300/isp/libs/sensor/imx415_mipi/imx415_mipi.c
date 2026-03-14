/*
 * imx415_mipi.c
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
#include "imx415_mipi.h"
#include "sensor_std.h"

#define _NAME(n, s) n##_##s
#define NAME(n) _NAME(n, imx415_mipi)

#define CCI_ID (0x34) >> 1
#define CCI_MODE 2
#ifndef REWRITE_stSnsDefaultCtrlPara
static SNS_CTRL_PARA_S stSnsDefaultCtrlPara = {
    .bInited = 0,
    .u32CurrExposureRatio = 256, // 默认初始化为16倍
    .u32MinRstLine = 5,
    .u32MinAgain = 0x40,
};
#endif

static unsigned int RHS1 = 0;
static unsigned int VMAX = 0;
static unsigned int FSC = 0;
// static int err_sum = 0;
extern const unsigned int IMX415RegToGain[241];

static int SetIntt(void *pvObj, FH_UINT32 intt, FH_UINT8 fNo);
static int GetIntt(void *pvObj);
static int SetGain(void *pvObj, FH_UINT32 again, FH_UINT8 fNo);
static int GetGain(void *pvObj);
static int SetExposureRatio(void *pvObj, FH_UINT32 exposure_ratio);
static int GetExposureRatio(void *pvObj, FH_UINT32 *exposure_ratio);
/********************sensor flow related start****************************/
static void SensorReset(void *pvObj)
{
    return;
}

static int Sensor_Init(void *pvObj, Sensor_Init_t *stInitConf)
{
    SNS_CTRL_PARA_S *pstCtrlPara = _sns_get_ctrl_para(pvObj);
    SensorDevice *pstCci = _sns_get_cci_(pvObj);
    pstCtrlPara->u8CciDeviceId = stInitConf->u8CciDeviceId;
    pstCtrlPara->u8CsiDeviceId = stInitConf->u8CsiDeviceId;
    SensorDevice_Init(pstCci, CCI_ID, CCI_MODE, pstCtrlPara->u8CciDeviceId);
    SensorReset(pvObj);
    pstCtrlPara->bInited = 1;
    return 0;
}

static int Sensor_Isconnect(void *pvObj, FH_UINT8 u8DeviceId)
{
    SNS_CTRL_PARA_S *pstCtrlPara = _sns_get_ctrl_para(pvObj);
    SensorDevice *pstCci = _sns_get_cci_(pvObj);
    FH_UINT32 u32SnsChipId;
    pstCtrlPara->u8CciDeviceId = u8DeviceId;
    SensorDevice_Init(pstCci, CCI_ID, CCI_MODE, pstCtrlPara->u8CciDeviceId);

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
    SensorDevice *pstCci = _sns_get_cci_(pvObj);

    SensorReset(pvObj);

    return SensorDevice_Close(pstCci);
}

static int SetLaneNumMax(void *pvObj, FH_UINT32 lane_num)
{
    SNS_CTRL_PARA_S *pstCtrlPara = _sns_get_ctrl_para(pvObj);
#ifdef FPGA_ENV
    pstCtrlPara->u32MaxLaneNum = 4;
#else
    pstCtrlPara->u32MaxLaneNum = 4;
#endif
    return 0;
}

static SNS_CONFIG_S *getSensorConfType(void *pvObj)
{
    SNS_CTRL_PARA_S *pstCtrlPara = _sns_get_ctrl_para(pvObj);
    pstCtrlPara->u32CurrConfSize = sizeof(stSnsConf0) / sizeof(SNS_CONFIG_S);
    return stSnsConf0;
}

static int getSensorConf(void *pvObj)
{
    SNS_CTRL_PARA_S *pstCtrlPara = _sns_get_ctrl_para(pvObj);
    SNS_CONFIG_S *pstConfGrp = getSensorConfType(pvObj);
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

static void SetSensorMipiCfg(void *pvObj)
{
    SNS_CTRL_PARA_S *pstCtrlPara = _sns_get_ctrl_para(pvObj);

    struct mipi_conf config;
    config.frq_range = pstCtrlPara->pstCurrConf->u8MipiClkRate;
    config.raw_type = RAW12;
    config.lane_num = pstCtrlPara->pstCurrConf->u8MipiLaneNum;
    config.mipi_id = pstCtrlPara->u8CsiDeviceId;

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

static int GetSensorViAttr(void *pvObj, ISP_VI_ATTR_S *stViAttr)
{
    SNS_CTRL_PARA_S *pstCtrlPara = _sns_get_ctrl_para(pvObj);
    FH_SINT32 s32Ret;

    if (stViAttr == NULL)
        return FH_RET_SENSOR_NULL_POINTER;

    if (pstCtrlPara->pstCurrConf == NULL)
    {
        s32Ret = getSensorConf(pvObj);

        if (s32Ret != FH_RET_SENSOR_OK)
            return s32Ret;
    }

    stViAttr->u16WndWidth = pstCtrlPara->pstCurrConf->stSnsAttr->u16WndWidth;
    stViAttr->u16WndHeight = pstCtrlPara->pstCurrConf->stSnsAttr->u16WndHeight;
    stViAttr->u16InputWidth = pstCtrlPara->pstCurrConf->stSnsAttr->u16InputWidth;
    stViAttr->u16InputHeight = pstCtrlPara->pstCurrConf->stSnsAttr->u16InputHeight;
    stViAttr->u16PicWidth = pstCtrlPara->pstCurrConf->stSnsAttr->u16PicWidth;
    stViAttr->u16PicHeight = pstCtrlPara->pstCurrConf->stSnsAttr->u16PicHeight;
    stViAttr->u16OffsetX = pstCtrlPara->pstCurrConf->stSnsAttr->u16OffsetX;
    stViAttr->u16OffsetY = pstCtrlPara->pstCurrConf->stSnsAttr->u16OffsetY;
    stViAttr->enBayerType = pstCtrlPara->pstCurrConf->stSnsAttr->enBayerType;
    pstCtrlPara->u32CurrHsf = pstCtrlPara->pstCurrConf->stSnsAttr->u16FrameRate * stViAttr->u16WndHeight;

    if (pstCtrlPara->pstCurrConf->stSnsAttr->u16FrameRate == 12)
        pstCtrlPara->u32CurrHsf = 25 * stViAttr->u16WndHeight / 2;
    else
        pstCtrlPara->u32CurrHsf = pstCtrlPara->pstCurrConf->stSnsAttr->u16FrameRate * stViAttr->u16WndHeight;

    return 0;
}

static int SetSensorFmt(void *pvObj, FH_SINT32 s32Format)
{
    SNS_CTRL_PARA_S *pstCtrlPara = _sns_get_ctrl_para(pvObj);
    SensorDevice *pstCci = _sns_get_cci_(pvObj);

    SensorDevice_Close(pstCci);
    pstCtrlPara->u32SnsFmt = s32Format;
    pstCtrlPara->u32WdrFlag = (s32Format >> 16) & 1;

    SensorDevice_Init(pstCci, CCI_ID, CCI_MODE, pstCtrlPara->u8CciDeviceId);
    SensorReset(pvObj);

    if (pstCtrlPara->bInited)
    {
        FH_SINT32 i, s32Ret;

        s32Ret = getSensorConf(pvObj);

        if (s32Ret)
            return s32Ret;

        SetSensorMipiCfg(pvObj);

        for (i = 0; i < pstCtrlPara->pstCurrConf->u32ConfSize; i++)
            Sensor_Write(pstCci, pstCtrlPara->pstCurrConf->pu16CciConf[2 * i], pstCtrlPara->pstCurrConf->pu16CciConf[2 * i + 1]);
    }
    else
        return -1;

    VMAX = Sensor_Read(pstCci, 0x3024);
    VMAX |= Sensor_Read(pstCci, 0x3025) << 8;
    VMAX |= (Sensor_Read(pstCci, 0x3026) & 0x0f) << 16;
    pstCtrlPara->u32Vcycle = VMAX;
    pstCtrlPara->u32FullLineStd = pstCtrlPara->u32Vcycle;

    if (pstCtrlPara->u32WdrFlag == 0)
    {
        pstCtrlPara->u32MinRstLine = 8;
        FSC = VMAX;
        pstCtrlPara->u32MaxIntt = FSC - pstCtrlPara->u32MinRstLine;
    }
    else
    {
        //change by miaomj for short exposure orig:0x4d
        RHS1 = Sensor_Read(pstCci, 0x3060);
        RHS1 |= Sensor_Read(pstCci, 0x3061) << 8;
        RHS1 |= (Sensor_Read(pstCci, 0x3062) & 0x0f) << 16;
        pstCtrlPara->u32MinRstLine = RHS1 + 9;
        FSC = 2 * VMAX;
        pstCtrlPara->u32MaxIntt = (FSC - pstCtrlPara->u32MinRstLine) >> 1;
    }

    SetIntt(pvObj, pstCtrlPara->u32MaxIntt, 0);
    SetExposureRatio(pvObj, pstCtrlPara->u32CurrExposureRatio);
    SetGain(pvObj, pstCtrlPara->u32MinAgain, 0);
    pstCtrlPara->u32CurrSnsGain = pstCtrlPara->u32MinAgain;

    return 0;
}
/********************sensor flow related end*************************************/

/************************AE&WDR Related start************************************/

static int GetAEInfo(void *pvObj, Sensor_AE_INFO_S *stSnsAEInfo)
{
    SNS_CTRL_PARA_S *pstCtrlPara = _sns_get_ctrl_para(pvObj);

    stSnsAEInfo->currIntt = GetIntt(pvObj);
    stSnsAEInfo->currAGain = GetGain(pvObj);
    stSnsAEInfo->currHsf = pstCtrlPara->u32CurrHsf;
    stSnsAEInfo->currFrameH = pstCtrlPara->u32Vcycle;
    return 0;
}

static int GetAEDefault(void *pvObj, Sensor_AE_Default_S *sensAEDefault)
{
    SNS_CTRL_PARA_S *pstCtrlPara = _sns_get_ctrl_para(pvObj);

    if (pstCtrlPara->u32WdrFlag == 0)
    {
        sensAEDefault->minIntt = 2;
    }
    else
    {
        sensAEDefault->minIntt = 8;
    }

    sensAEDefault->maxIntt = pstCtrlPara->u32MaxIntt;
    sensAEDefault->minAGain = pstCtrlPara->u32MinAgain;
    sensAEDefault->maxAGain = 254789;
    sensAEDefault->fullLineStd = pstCtrlPara->u32FullLineStd;
    sensAEDefault->MinRstLine = pstCtrlPara->u32MinRstLine;

    return 0;
}

// GetGain仅在ae refresh时会重新获取,所以尽量直接从sensor端获取,保证intt的正确性
static int GetGain(void *pvObj)
{
    SNS_CTRL_PARA_S *pstCtrlPara = _sns_get_ctrl_para(pvObj);

    return pstCtrlPara->u32CurrSnsGain;
}

static int CalcSnsValidGain(void *pvObj, FH_UINT32 *u32Gain)
{
    FH_UINT32 gain_val;

    int i;

    for (i = 0; i < 241; i++)
    {
        if (IMX415RegToGain[i] > *u32Gain)
            break;
    }
    gain_val = (i - 1);

    *u32Gain = IMX415RegToGain[gain_val];

    return 0;
}

static int SetGain(void *pvObj, FH_UINT32 again, FH_UINT8 fNo)
{
    FH_UINT32 gain_val;
    SensorDevice *pstCci = _sns_get_cci_(pvObj);
    SNS_CTRL_PARA_S *pstCtrlPara = _sns_get_ctrl_para(pvObj);
    int i;
    // again += err_sum;

    for (i = 0; i < 241; i++)
    {
        if (IMX415RegToGain[i] > again)
            break;
    }
    gain_val = (i - 1);

    // err_sum = again - IMX485RegToGain[gain_val];
    pstCtrlPara->u32CurrSnsGain = IMX415RegToGain[gain_val];

    Sensor_Write(pstCci, 0x3090, (gain_val & 0xff));
    Sensor_Write(pstCci, 0x3091, ((gain_val >> 8) & 0x7));
    if (pstCtrlPara->u32WdrFlag)
    {
        Sensor_Write(pstCci, 0x3092, (gain_val & 0xff));
        Sensor_Write(pstCci, 0x3093, ((gain_val >> 8) & 0x7));
    }

    return 0;
}

// GetIntt仅在ae refresh时会重新获取,所以尽量直接从sensor端获取,保证intt的正确性
static int GetIntt(void *pvObj)
{
    SNS_CTRL_PARA_S *pstCtrlPara = _sns_get_ctrl_para(pvObj);

    return pstCtrlPara->u32CurrSnsIntt;
}

static int CalcSnsValidIntt(void *pvObj, FH_UINT32 *u32Intt)
{
    return 0;
}

static int Set_short_exp_intt(void *pvObj, FH_UINT32 intt)
{
    SensorDevice *pstCci = _sns_get_cci_(pvObj);
    SNS_CTRL_PARA_S *pstCtrlPara = _sns_get_ctrl_para(pvObj);

    FH_UINT32 SHR1, SHR1_l, SHR1_h, SHR1_hh;
    int m;
    SHR1 = RHS1 - (intt << 1);
    m = (SHR1 - 1) >> 1;
    if (((SHR1 - 1) % 2) != 0)
        SHR1 = 2 * m + 1;
    if (SHR1 < 9)
    {
        SHR1 = 9;
    }
    else if (SHR1 > (RHS1 - 8))
    {
        SHR1 = RHS1 - 8;
    }
    SHR1_l = SHR1 & 0xff;
    SHR1_h = (SHR1 >> 8) & 0xff;
    SHR1_hh = (SHR1 >> 16) & 0x0f;
    // short exposure
    Sensor_Write(pstCci, 0x3054, SHR1_l);
    Sensor_Write(pstCci, 0x3055, SHR1_h);
    Sensor_Write(pstCci, 0x3056, SHR1_hh);
    pstCtrlPara->u32CurrSnsShortIntt = (RHS1 - SHR1) >> 1;
    return 0;
}

static int SetIntt(void *pvObj, FH_UINT32 intt, FH_UINT8 fNo)
{
    SNS_CTRL_PARA_S *pstCtrlPara = _sns_get_ctrl_para(pvObj);
    SensorDevice *pstCci = _sns_get_cci_(pvObj);

    pstCtrlPara->u32CurrSnsIntt = intt;

    if (pstCtrlPara->u32WdrFlag)
    {
        FH_UINT32 short_exp_intt;
        /**/
        FH_UINT32 SHR0, SHR0_l, SHR0_h, SHR0_hh;

        intt = intt << 1;
        SHR0 = ((FSC - intt + 1) >> 1) << 1;

        if (SHR0 < (RHS1 + 9))
        {
            SHR0 = RHS1 + 9;
        }
        else if (SHR0 > (FSC - 8))
        {
            SHR0 = FSC - 8;
        }
        SHR0_l = SHR0 & 0xff;
        SHR0_h = (SHR0 >> 8) & 0xff;
        SHR0_hh = (SHR0 >> 16) & 0xf;

        // long exposure
        pstCtrlPara->u32CurrSnsIntt = (FSC - SHR0) >> 1;
        Sensor_Write(pstCci, 0x3050, SHR0_l);
        Sensor_Write(pstCci, 0x3051, SHR0_h);
        Sensor_Write(pstCci, 0x3052, SHR0_hh);

        // usleep(1000);
        short_exp_intt = MAX(4, ((MIN(pstCtrlPara->u32CurrSnsIntt, pstCtrlPara->u32MaxIntt) << 4) + (pstCtrlPara->u32CurrExposureRatio >> 1)) / pstCtrlPara->u32CurrExposureRatio);
        Set_short_exp_intt(pvObj, short_exp_intt);
    }
    else
    {
        FH_UINT32 intt_invert = 0;
        FH_UINT16 intt_low, intt_high, intt_hh;
        intt_invert = CLIP((VMAX - intt), 8, (VMAX - 4));

        intt_low = (FH_UINT16)(intt_invert & 0x000000ff);
        intt_high = (FH_UINT16)((intt_invert >> 8) & 0x000000ff);
        intt_hh = (FH_UINT16)((intt_invert >> 16) & 0x000000ff);

        Sensor_Write(pstCci, 0x3050, intt_low);
        Sensor_Write(pstCci, 0x3051, intt_high);
        Sensor_Write(pstCci, 0x3052, intt_hh);
    }

    return 0;
}

static int SetSnsFrameH(void *pvObj, FH_UINT32 frameH)
{
    // SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);
    // SensorDevice*       pstCci      = _sns_get_cci_(pvObj);

    // Sensor_Write(pstCci, 0x3025, frameH>>8);
    // Sensor_Write(pstCci, 0x3024, frameH&0xff);
    // pstCtrlPara->u32Vcycle = frameH;
    // VMAX = frameH;
    return 0;
}

static int SetExposureRatio(void *pvObj, FH_UINT32 exposure_ratio)
{
    SNS_CTRL_PARA_S *pstCtrlPara = _sns_get_ctrl_para(pvObj);

    pstCtrlPara->u32CurrExposureRatio = exposure_ratio;
    return 0;
}

static int GetExposureRatio(void *pvObj, FH_UINT32 *exposure_ratio)
{
    SNS_CTRL_PARA_S *pstCtrlPara = _sns_get_ctrl_para(pvObj);

    *exposure_ratio = (pstCtrlPara->u32CurrSnsIntt << 4) / MAX(1, pstCtrlPara->u32CurrSnsShortIntt);
    return 0;
}
/************************AE&WDR Related End************************************/

/************************sensor attr related start*****************************/
static int GetSensorAttribute(void *pvObj, char *name, FH_UINT32 *value)
{
    SNS_CTRL_PARA_S *pstCtrlPara = _sns_get_ctrl_para(pvObj);

    if (strcmp(name, "WDR") == 0)
    {
        *value = pstCtrlPara->u32WdrFlag;
        return 0;
    }

    return -1;
}
/************************sensor attr related end*******************************/

/***********************fastboot related start*********************************/
static int Sensor_Kick(void *pvObj)
{
    SNS_CTRL_PARA_S *pstCtrlPara = _sns_get_ctrl_para(pvObj);

    if (pstCtrlPara->bInited)
    {
        // enable sensor output
    }
    return 0;
}

/************************sensor func related start****************************/
static int SetSensorReg(void *pvObj, unsigned short addr, unsigned short data)
{
    SensorDevice *pstCci = _sns_get_cci_(pvObj);
    Sensor_Write(pstCci, addr, data);
    return 0;
}

static int GetSensorReg(void *pvObj, unsigned short addr, unsigned short *data)
{
    SensorDevice *pstCci = _sns_get_cci_(pvObj);
    *data = Sensor_Read(pstCci, addr);
    return 0;
}

static int SetSensorFlipMirror(void *pvObj, FH_UINT32 sensor_en_stat)
{
    //SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);
    SensorDevice *pstCci = _sns_get_cci_(pvObj);
    Sensor_Write(pstCci, 0x3030, sensor_en_stat);
    return 0;
}

static int GetSensorFlipMirror(void *pvObj, FH_UINT32 *sensor_en_stat)
{
    //SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);
    SensorDevice *pstCci = _sns_get_cci_(pvObj);
    *sensor_en_stat = Sensor_Read(pstCci, 0x3030);
    return 0;
}

static int SensorCommonIf(void *pvObj, FH_UINT32 cmd, ISP_SENSOR_COMMON_CMD_DATA0 *data0, ISP_SENSOR_COMMON_CMD_DATA1 *data1)
{
    int ret = -1;
    return ret;
}
/***********************sensor func related end**********************************/

/***********************adv api related start************************************/
static FH_UINT32 *GetMirrorFlipBayerFormat(void *pvObj) { return 0; }

static FH_UINT32 *GetUserSensorAwbGain(void *pvObj, FH_UINT32 idx) { return 0; }
static FH_UINT32 *GetSensorLtmCurve(void *pvObj, FH_UINT32 idx) { return 0; }
/***********************adv api related end*************************************/

/***********************not used for now start**********************************/
static int SetSensorAwbGain(void *pvObj, FH_UINT32 *awb_gain) { return 0; }
static int GetSensorAwbGain(void *pvObj, FH_UINT32 *awb_gain) { return 0; }
static int SetSensorIris(void *pvObj, FH_UINT32 iris) { return 0; }
/***********************not used for now end************************************/

#define SENSOR_IMX415_MIPI "imx415_mipi"
struct isp_sensor_if imx415_mipi_sensor_if = {
    .name = SENSOR_IMX415_MIPI,
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
        .pstSnsIf = &imx415_mipi_sensor_if,
        .pstSnsCtrlPara = &stSnsDefaultCtrlPara,
        .pVer = SENSOR_GIT_VERSION,
    };

    return Sensor_Create_Common(&info);
}

void NAME(Sensor_Destroy)(struct isp_sensor_if *s_if)
{
    Sensor_Destroy_Common(s_if);
}
