#ifndef __SENSOR_H__
#define __SENSOR_H__
#include "sensor_ver.h"

typedef enum
{
    SNS_INT_POS_START    = 0,
    SNS_INT_POS_END      = 1,
}SNS_INT_POS_E;

#ifdef STITCH_SUPPORT
#define SNS_REGS_DATA_MAX_NUM                (32)

//单个sensor寄存器配置
typedef struct
{
    FH_UINT32       u32Addr;
    FH_UINT32       u32Data;
    FH_BOOL         bNeedUpdate;
    SNS_INT_POS_E   u8IntPos;//配置时机点
} SNS_REG_DATA_S;

//传递给isp的sensor寄存器配置
typedef struct {
    FH_UINT32        u32Id;//通道道号
    FH_BOOL          bNow; //立即配sensor
    FH_UINT8         u8Ccid;
    FH_UINT32        u32DevAddr;
    FH_UINT8         mode;
    FH_UINT32        u32RegNum;
    SNS_REG_DATA_S   astRegData[SNS_REGS_DATA_MAX_NUM];
    FH_BOOL          bValid;//数据有效，可读
} SNS_REGS_INFO_S;
#endif

// 用于控制下载sensor配置的结构体
typedef struct
{
    FH_UINT32       u32SnsFmt;  // 该配置的制式
    FH_UINT32       u32ConfSize;  // 该配置的大小
    const FH_UINT16 *pu16CciConf; // 配置指向的初始化数组
    FH_UINT8        u8MipiClkRate;  // 该配置的mipi时钟频率
    FH_UINT8        u8MipiLaneNum;  // 该配置的mipilane数
    ISP_VI_ATTR_S   *stSnsAttr;  // 该配置的幅面相关的配置
    FH_UINT32       u32SnsClk;
}SNS_CONFIG_S;

// 用于sensor库函数实现需要的变量
typedef struct
{
    FH_BOOL     bInited;        // sensor初始化
    FH_UINT8    u8CciDeviceId;   // 当前使用几号设备来初始化sensor
    FH_UINT8    u8CsiDeviceId;   // 当前使用几号mipi
    FH_UINT32   u32MaxLaneNum;  // sensor的mipi配置不能超过最大支持的lane数目
    FH_UINT32   u32SnsFmt;  // sensor输出制式
    FH_UINT32   u32Vcycle;     // sensor框架高
    FH_UINT32   u32WdrFlag;    // sensor是否是wdr输出
    FH_UINT32   u32CurrSnsGain;        // 当前sensor增益
    FH_UINT32   u32CurrSnsIntt;        // 当前sensor长曝光时间
    FH_UINT32   u32CurrSnsShortIntt;  // 当前sensor短曝光时间
    FH_UINT32   u32CurrExposureRatio;     // 当前sensor曝光比
    FH_UINT32   u32CurrHsf;    // 当前sensor行频
    FH_UINT32   u32MaxIntt;    // sensor最大曝光时间(非降帧时)
    FH_UINT32   u32MinAgain;    // 最小曝光增益,U.6精度
    FH_UINT32   u32FullLineStd; // sensor框架高度(非降帧时)
    FH_UINT32   u32MinRstLine;  // 计算最大曝光时间时的最小复位行数
    void*       pvPrivatePara;
    FH_UINT32   u32CurrConfSize;
    SNS_CONFIG_S*    pstCurrConf;
#ifdef STITCH_SUPPORT
    FH_BOOL         bGrpSync;//是否需要多sensor同步,
    SNS_REG_DATA_S  stRegsBuf[256];
    FH_UINT8        u8RegsSpWr;
    FH_UINT8        u8RegsSpRd;
#endif
}SNS_CTRL_PARA_S;

// sensor结构需要传递的参数集合
typedef struct
{
    SNS_CTRL_PARA_S  stCtrlPara;
    SensorDevice     stCci;
}SNS_PARA_S;

#define _sns_get_ctrl_para(obj)     (SNS_CTRL_PARA_S*)&(((SNS_PARA_S*)(((struct isp_sensor_if *)obj)->para))->stCtrlPara)
#define _sns_get_cci_(obj)          (SensorDevice*)&(((SNS_PARA_S*)(((struct isp_sensor_if *)obj)->para))->stCci)

typedef struct
{
    struct isp_sensor_if *pstSnsIf;
    SNS_CTRL_PARA_S      *pstSnsCtrlPara;
    char                 pVer[32];
}SensorCreateInfo;

struct isp_sensor_if *Sensor_Create_Common(SensorCreateInfo *info);
void Sensor_Destroy_Common(struct isp_sensor_if *s_if);
#endif // __SENSOR_H__
