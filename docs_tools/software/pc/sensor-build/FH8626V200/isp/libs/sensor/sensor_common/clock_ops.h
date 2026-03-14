#ifndef __CLOCK_OPS_H
#define __CLOCK_OPS_H

#ifdef OS_LINUX
#include <sys/ioctl.h>
#define CLK_IOCTL_MAGIC     'c'
#define ENABLE_CLK          _IOWR(CLK_IOCTL_MAGIC, 0, unsigned int)
#define DISABLE_CLK         _IOWR(CLK_IOCTL_MAGIC, 1, unsigned int)
#define SET_CLK_RATE        _IOWR(CLK_IOCTL_MAGIC, 2, unsigned int)
#define GET_CLK_RATE        _IOWR(CLK_IOCTL_MAGIC, 3, unsigned int)
#define SET_PMU             _IOWR(CLK_IOCTL_MAGIC, 4, unsigned int)
#define GET_PMU             _IOWR(CLK_IOCTL_MAGIC, 5, unsigned int)

#define CLK_IOCTL_MAXNR     6
#else
#define ENABLE_CLK          0
#define DISABLE_CLK         1
#define SET_CLK_RATE        2
#define GET_CLK_RATE        3
#define SET_PMU             4
#define GET_PMU             5
#endif


struct clk_usr
{
	char			*name;
	unsigned long       	frequency;
};

int set_clk_rate(int clock_type,unsigned long rate);
int set_clk_enable(int clock_type, int enable);

enum CLK_TYPE
{
    isp_clk = 0,
    CLK_SNS0_CLK = 1,
    CLK_SNS1_CLK = 2,
    CLK_SNS2_CLK = 3,
};

enum CLK_RATE
{
	CLK_12M  = 12000000,
	CLK_18M  = 18000000,
	CLK_20M  = 20000000,
	CLK_22M5 = 22500000,
	CLK_24M  = 24000000,
	CLK_27M  = 27000000,
	CLK_30M  = 30000000,
	CLK_36M  = 36000000,
	CLK_37M  = 37125000,
	CLK_45M  = 45000000,
	CLK_96M  = 96000000,
	CLK_120M = 120000000,
	CLK_160M = 160000000,
};
#if defined(XGM) || defined(XGMPLUS)
#define SNS_CLK_NAME(csiId) \
({\
    FH_UINT32 u32SnsId;\
    switch (csiId)\
    {\
    case 0:\
        u32SnsId = CLK_SNS0_CLK;\
        break;\
    case 1:\
        u32SnsId = CLK_SNS1_CLK;\
        break;\
    case 2:\
        u32SnsId = CLK_SNS1_CLK;\
        break;\
    default:\
        u32SnsId = CLK_SNS1_CLK;\
        break;\
    }\
    u32SnsId;\
})
#elif defined(XGM2)
#define SNS_CLK_NAME(csiId) \
({\
    FH_UINT32 u32SnsId;\
    switch (csiId)\
    {\
    case 0:\
        u32SnsId = CLK_SNS0_CLK;\
        break;\
    case 1:\
        u32SnsId = CLK_SNS0_CLK;\
        break;\
    default:\
        u32SnsId = CLK_SNS0_CLK;\
        break;\
    }\
    u32SnsId;\
})
#elif defined(CH2)
#define SNS_CLK_NAME(csiId) \
({\
    FH_UINT32 u32SnsId;\
    switch (csiId)\
    {\
    case 0:\
        u32SnsId = CLK_SNS0_CLK;\
        break;\
    case 1:\
        u32SnsId = CLK_SNS0_CLK;\
        break;\
    case 2:\
        u32SnsId = CLK_SNS0_CLK;\
        break;\
    default:\
        u32SnsId = CLK_SNS0_CLK;\
        break;\
    }\
    u32SnsId;\
})
#else
#define SNS_CLK_NAME(csiId) \
({\
    FH_UINT32 u32SnsId;\
    switch (csiId)\
    {\
    case 0:\
        u32SnsId = CLK_SNS1_CLK;\
        break;\
    case 1:\
        u32SnsId = CLK_SNS0_CLK;\
        break;\
    case 2:\
        u32SnsId = CLK_SNS2_CLK;\
        break;\
    default:\
        u32SnsId = CLK_SNS1_CLK;\
        break;\
    }\
    u32SnsId;\
})
#endif
#endif