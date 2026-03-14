#ifndef __FH_IVS_FUNC_H__
#define __FH_IVS_FUNC_H__

#define MAX_BOUNDARY_POINT_NUM  10   /* 最大坐标数 */
#define FH_MAX_TGT_CNT          20   /*最大跟踪目标数*/

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

/************************************ public struct ************************************/

typedef enum
{
    IVS_DEBUG_LEVEL_OFF    = 0x0, /* 关闭debug打印 */
    IVS_DEBUG_LEVEL_RET    = 0x1, /* 仅打开函数返回报错打印 */
    IVS_DEBUG_LEVEL_BGM    = 0x2, /* 打开BGM相关打印 */
    IVS_DEBUG_LEVEL_CFG    = 0x4, /* 打开规则配置打印 */
    IVS_DEBUG_LEVEL_DETECT = 0x8, /* 打开检测相关打印 */
    IVS_DEBUG_LEVEL_ALL    = 0xf, /* 打开所有打印*/
}FH_IVS_DEBUG_LEVEL;

typedef struct
{
    int x; /* 点的x坐标值 */
    int y; /* 点的y坐标值 */
}FH_POINT_S;

typedef struct
{
    int x; /* 区域定点坐标x */
    int y; /* 区域定点坐标y */
    int w; /* 区域宽度w */
    int h; /* 区域长度h */
}FH_RECT_S;

typedef struct
{
    int        iPointNum;                         /* 组成该区域的顶点的个数 */
    FH_POINT_S astPoint[MAX_BOUNDARY_POINT_NUM];  /* 任意边形坐标点 */
}FH_POLYGON_REGION_S;

typedef struct
{
    int		  Id;  /* 目标标识 */
    FH_RECT_S pos; /* 目标位置 */
} FH_RESULT_S;

typedef struct
{
    int maxTrackNum;  /* 最大追踪数量 */
    int iouThresh;    /* 最小匹配面积 */
    int minHits;      /* 最小命中次数 */
    int maxAge;       /* 最大跟丢次数 */
}FH_IVS_TRACK_CFG_t;

typedef struct
{
    int num;
    FH_RESULT_S astresult[FH_MAX_TGT_CNT];
}FH_IVS_ALL_RESULT_S;

/************************************ private struct ************************************/
/** 周界检测模式 */
typedef enum
{
    FH_PMODE_NO = 0,        /* 无事件 */
    FH_PMODE_EXIT,    	    /* 离开 */
    FH_PMODE_ENTER,         /* 进入 */
    FH_PMODE_INTRUSION      /* 入侵 */
} PERI_MODE_E;

/** 周界检测规则参数 */
typedef struct
{
    int     enable;     /* 规则使能 */
    int		iMode;		/* 周界模式 */
    int		iSizeMin;	/* 最小尺寸(画面宽度的百分比) 取值范围[0, 100] */
    int		iSizeMax;	/* 最大尺寸(画面宽度的百分比) 取值范围[0, 100] */
    int		iTimeMin;	/* 时间取值范围[0, 100]，单位：秒，仅区域入侵有效 */
    int     minHits;    /* 最小触发次数， 建议范围[1-5] */
    FH_POLYGON_REGION_S stBoundary;  /* 周界边界区域 */
} FH_PERI_RULE_PARA_S;

/** 周界检测事件数据结构 */
typedef struct
{
    int					Id;			/* 目标标识 */
    FH_RECT_S		    stRect;	    /* 事件位置 */
    PERI_MODE_E			eMode; 		/* 警戒区模式 */
} FH_PERI_RESULT_S;

typedef struct
{
    int EventNum;                                /* 结果个数 */
    FH_PERI_RESULT_S astresult[FH_MAX_TGT_CNT];  /* 结果数据 */
}FH_PERI_ALL_RESULT_S;

/////////////////////////////////////////////////////////////////////
typedef enum
{
    FH_TMODE_NO = 0,        /* 无绊线 */
    FH_TMODE_LTOR,	        /* 从左到右绊线 */
    FH_TMODE_RTOL,			/* 从右到左绊线 */
} TRIP_MODE_E;

/** 绊线检测规则参数 */
typedef struct
{
    int         enable;
	int 	    bDobDir;		/* 表示该绊线是否为双向绊线(0: 否, 1: 是) */
	TRIP_MODE_E	iForDir;		/* 单绊线禁止方向 */
	int		    iSizeMin;		/* 最小尺寸(画面宽度的百分比) 取值范围[0, 100] */
	int		    iSizeMax;		/* 最大尺寸(画面宽度的百分比) 取值范围[0, 100] */
    int         minHits;        /* 最小触发次数， 建议范围[1-5] */
    FH_POINT_S	startPoint;	    /* 规则线起点 */
    FH_POINT_S	endPoint;	    /* 规则线终点 */
} FH_TRIP_RULE_PARA_S;

/** 检测事件数据结构 */
typedef struct
{
	int				 Id;			/* 目标标识 */
	FH_RECT_S		 stRect;		/* 事件位置 */
	TRIP_MODE_E		 tmode;	        /* 绊线禁止方向 */
} FH_TRIP_RESULT_S;

/* 检测事件数据结构 */
typedef struct
{
    int EventNum;                                /* 结果个数 */
    FH_TRIP_RESULT_S astresult[FH_MAX_TGT_CNT];  /* 结果数据 */
} FH_TRIP_ALL_RESULT_S;

/*
*@ 打印回调函数注册，在初始化前调用
*@param [in] callback
*/
void FH_IVS_Register(void (*callback)(int level, char* format, ...));

/*
*@ 初始化，仅调用一次，传入NULL运行默认参数(建议使用默认参数)
*@param [in] ptrackcfg
*@return 0: 成功 非0：失败
*/
int FH_IVS_Init(FH_IVS_TRACK_CFG_t* ptrackcfg);

/*
*@ 绊线检测检测幅面配置, 应为CH0的幅面宽高
*@param [in] width， height
*/
void FH_IVS_SetFrameSize(int width, int height);

/*
*@ 获取绊线检测检测幅面配置
*@param [in] width， height
*/
void FH_IVS_GetFrameSize(int *width, int *height);

/*
*@ 区域入侵检测参数配置，id范围:[0-11]
*@param [in] id， ppericfg
*@return 0: 成功 非0：失败
*/
int FH_IVS_SetPeriRule(int id, FH_PERI_RULE_PARA_S* ppericfg);

/*
*@ 区域入侵获取检测参数配置, id范围:[0-11]
*@param [in] id， [out]ppericfg
*@return 0: 成功 非0：失败
*/
int FH_IVS_GetPeriRule(int id, FH_PERI_RULE_PARA_S* ppericfg);

/*
*@ 绊线检测检测参数配置, id范围:[0-3]
*@param [in] id， ptripcfg
*@return 0: 成功 非0：失败
*/
int FH_IVS_SetTripRule(int id, FH_TRIP_RULE_PARA_S* ptripcfg);

/*
*@ 绊线检测获取检测参数配置, id范围:[0-3]
*@param [in] id， [out]ptripcfg
*@return 0: 成功 非0：失败
*/
int FH_IVS_GetTripRule(int id, FH_TRIP_RULE_PARA_S* ptripcfg);

/*
*@ 运行检测，将所有检测区域保存在result中
*@param [out] result
*/
void FH_IVS_Process(FH_IVS_ALL_RESULT_S *result);

/*
*@ 获取区域入侵检测结果, id范围:[0-11]
*@param [in] id， [out]presult
*@return 0: 成功 非0：失败
*/
int FH_IVS_GetPeriResults(int id, FH_PERI_ALL_RESULT_S *presult);

/*
*@ 获取绊线检测检测结果, id目前只能为0
*@param [in] id， [out]tresult
*@return 0: 成功 非0：失败
*/
int FH_IVS_GetTripResults(int id, FH_TRIP_ALL_RESULT_S *tresult);

/*
*@ 释放资源
*@param
*/
int FH_IVS_Release();

/*
*@ 获取版本信息，如果传入libinfo，则将版本信息保存在libinfo中，传入NULL则打印版本信息
*@param [in] libinfo
*/
void FH_IVS_GetLibInfo();

/*
*@ 配置debug等级，详见FH_IVS_DEBUG_LEVEL结构体
*@param [in] level
*/
void FH_IVS_SetDebugLevel(FH_IVS_DEBUG_LEVEL level);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __FH_IVS_FUNC_H__ */
