#include "sensor_std.h"

static int SetIntt(void* pvObj, FH_UINT32 intt, FH_UINT8 fNo);
static int GetIntt(void* pvObj);
static int SetGain(void* pvObj, FH_UINT32 again, FH_UINT8 fNo);
static int GetGain(void* pvObj);
static int SetExposureRatio(void* pvObj, FH_UINT32 exposure_ratio);
static int GetExposureRatio(void* pvObj, FH_UINT32 *exposure_ratio);

#ifndef REWRITE_stSnsDefaultCtrlPara
static SNS_CTRL_PARA_S stSnsDefaultCtrlPara = {
    .bInited = 0,
    .u32CurrExposureRatio = 256,  // 默认初始化为16倍
    .u32MinRstLine = 5,
    .u32MinAgain = 0x40,
};
#endif

#ifndef REWRITE_Sensor_Isconnect
static int Sensor_Isconnect(void *pvObj, FH_UINT8 u8DeviceId){return 0;}
#endif

#ifndef REWRITE_SensorReset
static void SensorReset(void *pvObj)
{
    return;
}
#endif

#ifndef REWRITE_Sensor_DeInit
static int Sensor_DeInit(void *pvObj)
{
    SensorDevice*       pstCci      = _sns_get_cci_(pvObj);

    SensorReset(pvObj);

    return SensorDevice_Close(pstCci);
}
#endif

#ifndef REWRITE_Sensor_Init
static int Sensor_Init(void *pvObj, Sensor_Init_t* stInitConf)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);
    SensorDevice*       pstCci      = _sns_get_cci_(pvObj);

    pstCtrlPara->u8CciDeviceId  = stInitConf->u8CciDeviceId;
    pstCtrlPara->u8CsiDeviceId  = stInitConf->u8CsiDeviceId;
#ifdef STITCH_SUPPORT
    pstCtrlPara->bGrpSync  = stInitConf->bGrpSync;
#endif

    SensorDevice_Init(pstCci, CCI_ID, CCI_MODE, pstCtrlPara->u8CciDeviceId);
    SensorReset(pvObj);

    pstCtrlPara->bInited = 1;

    return 0;
}
#endif

#ifndef REWRITE_SetLaneNumMax
static int SetLaneNumMax(void *pvObj, FH_UINT32 lane_num)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);
    pstCtrlPara->u32MaxLaneNum = lane_num;
    return 0;
}
#endif

#ifndef REWRITE_SetSensorMipiCfg
static void SetSensorMipiCfg(void *pvObj)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);

    struct mipi_conf config;

    config.frq_range = pstCtrlPara->pstCurrConf->u8MipiClkRate;
    config.raw_type  = RAW10;
    config.lane_num  = pstCtrlPara->pstCurrConf->u8MipiLaneNum;
    config.mipi_id   = pstCtrlPara->u8CsiDeviceId;

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
#endif

#ifndef REWRITE_getSensorConfType
static SNS_CONFIG_S* getSensorConfType(void *pvObj)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);

    pstCtrlPara->u32CurrConfSize  = sizeof(stSnsConf0) / sizeof(SNS_CONFIG_S);

    return stSnsConf0;
}
#endif

#ifndef REWRITE_getSensorConf
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
                if (pstConfGrp[i].u32SnsClk != 0)
                    set_clk_rate(SNS_CLK_NAME(pstCtrlPara->u8CsiDeviceId), pstConfGrp[i].u32SnsClk);

                pstCtrlPara->pstCurrConf = &pstConfGrp[i];

                return FH_RET_SENSOR_OK;
            }
        }
    }

    return FH_RET_SENSOR_NO_SUPPORT_FORMAT;
}
#endif

#ifndef REWRITE_SnsCom_SetFmt
static FH_SINT32 SnsCom_SetFmt(FH_VOID* pvObj, FH_SINT32 s32Format, FH_UINT16 u16CciId, FH_UINT16 u8CsiMode)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);
    SensorDevice*       pstCci      = _sns_get_cci_(pvObj);

    SensorDevice_Close(pstCci);
    pstCtrlPara->u32SnsFmt  = s32Format;
    pstCtrlPara->u32WdrFlag = (s32Format >> 16) & 1;

    SensorDevice_Init(pstCci, u16CciId, u8CsiMode, pstCtrlPara->u8CciDeviceId);
    SensorReset(pvObj);

    if (pstCtrlPara->bInited)
    {
        FH_SINT32 i, s32Ret;

        s32Ret = getSensorConf(pvObj);

        if (s32Ret)
            return s32Ret;

        // 先初始化好mipi再初始化sensor
        SetSensorMipiCfg(pvObj);

        for (i = 0; i < pstCtrlPara->pstCurrConf->u32ConfSize; i++)
        {
            if (pstCtrlPara->pstCurrConf->pu16CciConf[2 * i] == 0xffff && pstCtrlPara->pstCurrConf->pu16CciConf[2 * i + 1] == 0xffff)
            {
                // sony sensor 需求等待30ms
                usleep(5000*6);
            }
            else
                Sensor_Write(pstCci, pstCtrlPara->pstCurrConf->pu16CciConf[2 * i],  pstCtrlPara->pstCurrConf->pu16CciConf[2 * i + 1]);
        }

    }
    else
        return FH_RET_SENSOR_NOT_INIT;

    return FH_RET_SENSOR_OK;
}
#endif

#ifndef REWRITE_GetSensorViAttr
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
    stViAttr->u16FrameRate   = pstCtrlPara->pstCurrConf->stSnsAttr->u16FrameRate;
    pstCtrlPara->u32CurrHsf  = pstCtrlPara->pstCurrConf->stSnsAttr->u16FrameRate * stViAttr->u16WndHeight;


    if (pstCtrlPara->pstCurrConf->stSnsAttr->u16FrameRate == 12)
        pstCtrlPara->u32CurrHsf = 25 * stViAttr->u16WndHeight / 2;
    else
        pstCtrlPara->u32CurrHsf = pstCtrlPara->pstCurrConf->stSnsAttr->u16FrameRate * stViAttr->u16WndHeight;

    return 0;
}
#endif

#ifndef REWRITE_GetAEInfo
static int GetAEInfo(void* pvObj, Sensor_AE_INFO_S *stSnsAEInfo)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);

    stSnsAEInfo->currIntt = GetIntt(pvObj);
    stSnsAEInfo->currAGain = GetGain(pvObj);
    stSnsAEInfo->currHsf = pstCtrlPara->u32CurrHsf;
    stSnsAEInfo->currFrameH = pstCtrlPara->u32Vcycle;
    return 0;
}
#endif

#ifndef REWRITE_SetSensorReg
static int SetSensorReg(void* pvObj, unsigned short addr, unsigned short data)
{
    SensorDevice*       pstCci      = _sns_get_cci_(pvObj);

    Sensor_Write(pstCci, addr, data);

    return 0;
}
#endif

#ifndef REWRITE_GetSensorReg
static int GetSensorReg(void* pvObj, unsigned short addr, unsigned short *data)
{
    SensorDevice*       pstCci      = _sns_get_cci_(pvObj);

    *data = Sensor_Read(pstCci, addr);

    return 0;
}
#endif

#ifndef REWRITE_CalcSnsValidGain
static int CalcSnsValidGain(void* pvObj, FH_UINT32 *u32Gain)
{
    return 0;
}
#endif

#ifndef REWRITE_CalcSnsValidIntt
static int CalcSnsValidIntt(void* obj, FH_UINT32 *u32Intt)
{
    return 0;
}
#endif

#ifndef REWRITE_SetExposureRatio
static int SetExposureRatio(void* pvObj, FH_UINT32 exposure_ratio)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);

    pstCtrlPara->u32CurrExposureRatio = exposure_ratio;

    return 0;
}
#endif

#ifndef REWRITE_Sensor_Kick
static int Sensor_Kick(void* pvObj)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);

    if (pstCtrlPara->bInited)
    {
        // enable sensor output
    }
    return 0;
}
#endif

#ifndef REWRITE_GetUserSensorAwbGain
static FH_UINT32 *GetUserSensorAwbGain(void *pvObj, FH_UINT32 idx)
{
    return 0;
}
#endif

#ifndef REWRITE_GetSensorLtmCurve
static FH_UINT32 *GetSensorLtmCurve(void *pvObj, FH_UINT32 idx)
{
    return 0;
}
#endif

#ifndef REWRITE_SetSensorAwbGain
static int SetSensorAwbGain(void* pvObj, FH_UINT32 *awb_gain)
{
    return 0;
}
#endif

#ifndef REWRITE_GetSensorAwbGain
static int GetSensorAwbGain(void* pvObj, FH_UINT32 *awb_gain)
{
    return 0;
}
#endif

#ifndef REWRITE_SetSensorIris
static int SetSensorIris(void* pvObj, FH_UINT32 iris)
{
    return 0;
}
#endif

#ifndef REWRITE_GetMirrorFlipBayerFormat
static FH_UINT32* GetMirrorFlipBayerFormat(void* pvObj)
{
    return 0;
}
#endif

#ifndef REWRITE_SensorCommonIf
static int SensorCommonIf(void* pvObj, FH_UINT32 cmd, ISP_SENSOR_COMMON_CMD_DATA0* data0, ISP_SENSOR_COMMON_CMD_DATA1* data1)
{
    int ret = -1;
    return ret;
}
#endif

#ifndef REWRITE_GetExposureRatio

static int GetExposureRatio(void *pvObj, FH_UINT32 *exposure_ratio)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);
    *exposure_ratio = ((pstCtrlPara->u32CurrSnsIntt  << 4)+ MAX(1, pstCtrlPara->u32CurrSnsShortIntt)-1)/ MAX(1, pstCtrlPara->u32CurrSnsShortIntt);
    return 0;
}

#endif

#ifndef REWRITE_GetSensorAttribute

static int GetSensorAttribute(void* pvObj, char * name, FH_UINT32 *value)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);

    if (strcmp(name, "WDR") == 0) {
        *value = pstCtrlPara->u32WdrFlag;
        return 0;
    }

    return -1;
}

#endif

#ifdef FUN_EN_Sensor_WriteEx
static int Sensor_WriteEx(void *pvObj, FH_UINT32 addr, FH_UINT32 data, SNS_INT_POS_E pos)
{
    SensorDevice*       pstCci      = _sns_get_cci_(pvObj);

#ifdef STITCH_SUPPORT
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);
    if(pstCtrlPara->bGrpSync)
    {
        pstCtrlPara->stRegsBuf[pstCtrlPara->u8RegsSpWr].u32Addr     = addr;
        pstCtrlPara->stRegsBuf[pstCtrlPara->u8RegsSpWr].u32Data     = data;
        pstCtrlPara->stRegsBuf[pstCtrlPara->u8RegsSpWr].bNeedUpdate = FH_TRUE;
        pstCtrlPara->stRegsBuf[pstCtrlPara->u8RegsSpWr].u8IntPos    = pos;
        pstCtrlPara->u8RegsSpWr++;
        if(pstCtrlPara->u8RegsSpWr == pstCtrlPara->u8RegsSpRd)
        {
            printf("u8RegsSpWr overflow !\n");
            pstCtrlPara->u8RegsSpRd++;
        }
    }
    else
    {
        Sensor_Write(pstCci, addr, data);
    }
#else
    Sensor_Write(pstCci, addr, data);
#endif
    return 0;
}
#endif

#ifdef FUN_EN_CommonIfGetRegsInfo
static int CommonIfGetRegsInfo(void* pvObj, ISP_SENSOR_COMMON_CMD_DATA0* data0, ISP_SENSOR_COMMON_CMD_DATA1* data1)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);
    SensorDevice*       pstCci      = _sns_get_cci_(pvObj);
    FH_UINT8 i;
    FH_UINT8 num;
    FH_UINT32 u32DatSize;

    SNS_REGS_INFO_S *pstSnsRegInfo = (SNS_REGS_INFO_S *)data0;
    u32DatSize = *((FH_UINT32*)data1);
    if(u32DatSize < sizeof(SNS_REGS_INFO_S))
    {
        printf("data0 buf size(%d<%d) err !\n", u32DatSize , sizeof(SNS_REGS_INFO_S));
        return -1;
    }
    pstSnsRegInfo->u8Ccid     = pstCtrlPara->u8CciDeviceId;
    pstSnsRegInfo->u32DevAddr = pstCci->i2c_devid;
    pstSnsRegInfo->mode       = pstCci->mode;
    num = pstCtrlPara->u8RegsSpWr - pstCtrlPara->u8RegsSpRd;
    if(num > SNS_REGS_DATA_MAX_NUM)
        num = SNS_REGS_DATA_MAX_NUM;
    for(i=0; i<num; i++)
        pstSnsRegInfo->astRegData[i] = pstCtrlPara->stRegsBuf[pstCtrlPara->u8RegsSpRd++];
    pstSnsRegInfo->u32RegNum = num;

    return 0;
}
#endif