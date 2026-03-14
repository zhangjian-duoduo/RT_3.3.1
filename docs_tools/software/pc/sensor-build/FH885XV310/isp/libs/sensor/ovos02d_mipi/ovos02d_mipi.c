/*
 * ovos02d_mipi.c
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
#include "ovos02d_mipi.h"
#include "sensor_std.h"

#define _NAME(n, s) n##_##s
#define NAME(n) _NAME(n, ovos02d_mipi)

#define CCI_ID (0x78 >> 1)
#define CCI_MODE 0

#define REWRITE_CalcSnsValidGain
#define REWRITE_Sensor_Isconnect
#include "sns_com.h"

struct privateParaT{
    FH_UINT32 min_Vblank_adjust;
};

static int Sensor_Isconnect(void *pvObj, FH_UINT8 u8DeviceId)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);
    SensorDevice*       pstCci      = _sns_get_cci_(pvObj);

    pstCtrlPara->u8CciDeviceId = u8DeviceId;
    SensorDevice_Init(pstCci, CCI_ID, CCI_MODE, pstCtrlPara->u8CciDeviceId );
    SensorDevice_Close(pstCci);

    return 0;
}

//可根据sns_cfg->max_lane_num确定当前使用2lane配置还是4lane配置
static int SetSensorFmt(void *pvObj, FH_SINT32 s32Format)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);
    SensorDevice*       pstCci      = _sns_get_cci_(pvObj);
    FH_SINT32 s32Ret = SnsCom_SetFmt(pvObj, s32Format, CCI_ID, CCI_MODE);

    if (s32Ret)
        return s32Ret;

    // Sensor_Write(pstCci, 0xfd, 0x01);
    // pstCtrlPara->u32Vcycle = Sensor_Read(pstCci, 0x4e) << 8;
    // pstCtrlPara->u32Vcycle |= Sensor_Read(pstCci, 0x4f);
    // pstCtrlPara->u32FullLineStd = pstCtrlPara->u32Vcycle;

    pstCtrlPara->u32Vcycle = pstCtrlPara->pstCurrConf->stSnsAttr->u16WndHeight; // 此时如果采用读取寄存器方式获取不到实际配置的值
    pstCtrlPara->u32FullLineStd = pstCtrlPara->u32Vcycle;

    ((struct privateParaT *)pstCtrlPara->pvPrivatePara)->min_Vblank_adjust = Sensor_Read(pstCci, 0x05) << 8;
    ((struct privateParaT *)pstCtrlPara->pvPrivatePara)->min_Vblank_adjust |= Sensor_Read(pstCci, 0x06);

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

static int GetAEDefault(void *pvObj, Sensor_AE_Default_S *sensAEDefault)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);
    sensAEDefault->minIntt = 1;
    sensAEDefault->maxIntt = pstCtrlPara->u32MaxIntt;
    sensAEDefault->minAGain = pstCtrlPara->u32MinAgain;
    sensAEDefault->maxAGain = 0xfe80;
    sensAEDefault->fullLineStd = pstCtrlPara->u32FullLineStd;
    sensAEDefault->MinRstLine = pstCtrlPara->u32MinRstLine;

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
	// TODO:check againmax (32x 15.5x)
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);
    SensorDevice*       pstCci      = _sns_get_cci_(pvObj);
    FH_UINT32 again_h, again_l, dgain;

    pstCtrlPara->u32CurrSnsGain = again;
    // sensor again的精度是U5.4精度
    again >>= 2;  // U.6 -> U.4
    if(again <= 0x10)
    { // 1x
        again_l = 0x10;
        again_h = 0x0;
        dgain = 0x08;
    }
    else if (again <= 0x1ff)
    { // 1x ~ 31x
        again_h = (again >> 8) & 0x1;
        again_l = again & 0xff;
        dgain = 0x08;
    }
    else
    { // 31x ~ 512x sensor dgain的精度是U5.3精度
       again_h = 0x1;
       again_l = 0xff;
       dgain = MIN((again << 3) / 0x1ff, 0xff);
    }

    if (fNo == 0)
    {
        Sensor_Write(pstCci, 0xfd, 0x01);
        Sensor_Write(pstCci, 0x38, again_h);
        Sensor_Write(pstCci, 0x24, again_l);
        Sensor_Write(pstCci, 0x39, dgain);
        if (pstCtrlPara->u32WdrFlag == 1)
        {
            Sensor_Write(pstCci, 0x41, again_h);
            Sensor_Write(pstCci, 0x42, again_l);
            Sensor_Write(pstCci, 0x40, dgain);
        }
        Sensor_Write(pstCci, 0x01, 0x01);
    }

    return 0;
}

// GetIntt仅在ae refresh时会重新获取,所以尽量直接从sensor端获取,保证intt的正确性
static int GetIntt(void *pvObj)
{
    SensorDevice*       pstCci      = _sns_get_cci_(pvObj);
    int intt_h, intt_l, intt;

    Sensor_Write(pstCci, 0xfd, 0x01);

    intt_h = Sensor_Read(pstCci, 0x03);
    intt_l = Sensor_Read(pstCci, 0x04);
    intt = (intt_h << 8) | intt_l;

    return intt;
}

static int Set_short_exp_intt(void *pvObj, FH_UINT32 intt)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);
    SensorDevice*       pstCci      = _sns_get_cci_(pvObj);
    FH_UINT32 intt_h, intt_l;

    intt_h = (intt >> 8) & 0xff;
    intt_l = intt & 0xff;

    Sensor_Write(pstCci, 0xfd, 0x01);
    Sensor_Write(pstCci, 0x2f, intt_h);
    Sensor_Write(pstCci, 0x30, intt_l);
    Sensor_Write(pstCci, 0x01, 0x01);

    pstCtrlPara->u32CurrSnsShortIntt = intt;

    return 0;
}

static int SetIntt(void *pvObj, FH_UINT32 intt, FH_UINT8 fNo)
{
    SNS_CTRL_PARA_S*    pstCtrlPara = _sns_get_ctrl_para(pvObj);
    SensorDevice*       pstCci      = _sns_get_cci_(pvObj);
    FH_UINT32 intt_h, intt_l;

    intt = MAX(1, intt);

    if (fNo == 0)
    {

        intt_h = (intt >> 8) & 0xff;
        intt_l = intt & 0xff;

        Sensor_Write(pstCci, 0xfd, 0x01);
        Sensor_Write(pstCci, 0x03, intt_h);
        Sensor_Write(pstCci, 0x04, intt_l);
        Sensor_Write(pstCci, 0x01, 0x01);

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
    FH_UINT32 vblank, vblank_h, vblank_l;

    Sensor_Write(pstCci, 0xfd, 0x01);
    pstCtrlPara->u32Vcycle = Sensor_Read(pstCci, 0x4e) << 8;
    pstCtrlPara->u32Vcycle |= Sensor_Read(pstCci, 0x4f);
    vblank = Sensor_Read(pstCci, 0x05) << 8;
    vblank |= Sensor_Read(pstCci, 0x06);

    vblank = MAX(((struct privateParaT *)pstCtrlPara->pvPrivatePara)->min_Vblank_adjust, (vblank + frameH - pstCtrlPara->u32Vcycle));

    vblank_h = (vblank >> 8) & 0xff;
    vblank_l = vblank & 0xff;

    Sensor_Write(pstCci, 0xfd, 0x01);
    Sensor_Write(pstCci, 0x05, vblank_h);
    Sensor_Write(pstCci, 0x06, vblank_l);
    Sensor_Write(pstCci, 0x01, 0x01);

    pstCtrlPara->u32Vcycle = frameH;

    return 0;
}

static int SetSensorFlipMirror(void *pvObj, FH_UINT32 sensor_en_stat)
{
    SensorDevice*       pstCci      = _sns_get_cci_(pvObj);
    FH_UINT8 tmp1;
    FH_UINT8 flip, mirror;

    flip = sensor_en_stat & 0x1;
    mirror = (sensor_en_stat >> 1) & 0x1;

    Sensor_Write(pstCci, 0xfd, 0x01);
    tmp1 = Sensor_Read(pstCci, 0x3f);  // bit[0]:mirror  bit[1]:flip
    tmp1 = (tmp1&0xfc) | (1-mirror) | (flip << 1);

    Sensor_Write(pstCci, 0xfd, 0x01);
    Sensor_Write(pstCci, 0x3f, tmp1);
    Sensor_Write(pstCci, 0x01, 0x01);

    return 0;
}

static int GetSensorFlipMirror(void *pvObj, FH_UINT32 *sensor_en_stat)
{
    SensorDevice* pstCci = _sns_get_cci_(pvObj);
    FH_UINT8 mirror, flip;

    Sensor_Write(pstCci, 0xfd, 0x01);
    mirror = 1 - (Sensor_Read(pstCci, 0x3f) & 0x1);
    Sensor_Write(pstCci, 0xfd, 0x01);
    flip = (Sensor_Read(pstCci, 0x3f)>>1) & 0x1;
    *sensor_en_stat = (mirror << 1) | flip;

    return 0;
}

#define SENSOR_OVOS02d_MIPI "ovos02d_mipi"
struct isp_sensor_if ovos02d_mipi_sensor_if_default = {
    .name = SENSOR_OVOS02d_MIPI,
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
        .pstSnsIf       = &ovos02d_mipi_sensor_if_default,
        .pstSnsCtrlPara = &stSnsDefaultCtrlPara,
        .pVer           = SENSOR_GIT_VERSION,
    };

    struct isp_sensor_if *s_if = Sensor_Create_Common(&info);

    struct privateParaT *privPara = (struct privateParaT *)malloc(sizeof(struct privateParaT));
    ((SNS_PARA_S *)s_if->para)->stCtrlPara.pvPrivatePara = (void *)privPara;

    return s_if;
}

void NAME(Sensor_Destroy)(struct isp_sensor_if *s_if)
{
    free(((SNS_PARA_S *)s_if->para)->stCtrlPara.pvPrivatePara);
    ((SNS_PARA_S *)s_if->para)->stCtrlPara.pvPrivatePara = 0;

    Sensor_Destroy_Common(s_if);
}
