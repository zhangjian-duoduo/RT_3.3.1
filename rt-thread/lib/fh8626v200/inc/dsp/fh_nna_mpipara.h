#ifndef __FH_NNA_MPIPARA_H__
#define __FH_NNA_MPIPARA_H__
/**|NNA|**/

#include "types/type_def.h"
#include "fh_common.h"
#define FH_MAX_PATH 256
#define FH_MAX_BBOX_NUM 150
#define FH_MAX_LK_NUM 5
#define FH_MAX_LPD_NUM 4
#define FH_LPR_LLEN 8
#define FH_MAX_EMBEDDING_LEN 128
#define FH_FACEKPT_MAX_NUM 68
#define FH_MAX_DIM_NUM 4
#define FH_MAX_OUTPUT_NUM 4
#define FH_HIGH_DETCT_BASE (0x80000000)

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */
#pragma pack(4)

typedef struct
{
	FH_UINT32 base;  // 物理地址 | [ ]
	FH_VOID * vbase; // 虚拟地址 | [ ]
	FH_UINT32 size;  // 内存大小 | [ ]
}FH_USR_MEM_INFO;

typedef enum
{
	FN_ROT_0   = 0, // 旋转0度 | [ ]
	FN_ROT_90  = 1, // 旋转90度 | [ ]
	FN_ROT_180 = 2, // 旋转180度 | [ ]
	FN_ROT_270 = 3  // 旋转270度 | [ ]
}FH_NNA_ROT;

typedef struct
{
	FH_FLOAT x;     //矩形框x坐标 | [0.0 - 1.0]
	FH_FLOAT y;     //矩形框y坐标 | [0.0 - 1.0]
	FH_FLOAT w;     //矩形框宽度 | [0.0 - 1.0]
	FH_FLOAT h;     //矩形框高度 | [0.0 - 1.0]
}FH_RECT_T;

typedef enum
{
	FN_DET_UNDEFINED = 0,      //自定义后处理 | []
	FN_DET_TYPE_PERSON = 1,    //人形检测 | [ ]
	FN_DET_TYPE_FACE = 2,      //人脸检测 | [ ]
	FN_DET_TYPE_C2 = 3,        //人车检测 | [ ]
	FN_DET_KPTDET = 4,         //关键点检测 | [ ]
	FN_DET_FACEDET_FULL = 5,   //人脸抓拍   | [ ]
	FN_DET_NET_LPDET = 6,      //landmark | [ ]
	FN_DET_FACEATTR = 7,        //人脸属性 | [ ]
	FN_DET_FACEREC = 8,        //人脸识别   | [ ]
	FN_DET_LPREC  = 9,         //车牌识别   | [ ]
	FN_DET_FACEKPT = 10,       //人脸68关键点 | [ ]
	FN_DET_TYPE_PERSON_V0 = 11, //非法操作 | [ ]
	FN_DET_GESTUREDET = 12,     //手势检测 | [ ]
	FN_DET_GESTUREREC = 13,     //手势识别 | [ ]
	FN_DET_LIVREC = 14,         //活体检测 | [ ]
	FN_DET_C3DET = 15,          //人/车/火焰检测 | [ ]
	FN_DET_TYPE_C4 = 17,        //4分类检测 | [ ]
	FN_DET_TYPE_DEF_01 = FH_HIGH_DETCT_BASE+1,  //单类型检测 | [ ]
	FN_DET_TYPE_DEF_02,           // 预留暂未定义 | [ ]
	FN_DET_TYPE_DEF_03,           //双类型检测 | [ ]
	FN_DET_TYPE_KPTDET_01,         //特殊类型关键点检测 | [ ]
	FN_DET_TYPE_KPTDET_LANDMARK,   //特殊类型landmark检测 | [ ]
	FN_DET_TYPE_SUBREC = FH_HIGH_DETCT_BASE+8,   //物体识别 | [ ]
	FN_DET_TYPE_ERROR_V0=FH_HIGH_DETCT_BASE+11   //非法检测 | []
}FH_TYPE_E;

typedef struct
{
	FH_UINT16 clsType;   //类别索引 | [ ]
	FH_RECT_T bbox;      //矩形信息 | [ ]
	FH_FLOAT conf;       //置信度 | [0.0 - 1.0]
}FH_DET_BBOX_T;

typedef struct
{
	FH_UINT64 frame_id;                     //矩形信息对应的帧序 | [ ]
	FH_UINT64 time_stamp;                   //矩形信息对应帧的时间戳 | [ ]
	FH_UINT32 boxNum;                       //矩形个数 | [ ]
	FH_DET_BBOX_T detBBox[FH_MAX_BBOX_NUM]; //具体矩形信息 | [ ]
}FH_DETECTION_T;

typedef struct _point_t {
	FH_FLOAT x; // 坐标x | [ ]
	FH_FLOAT y; // 坐标y | [ ]
}POINT_T;

typedef struct
{
	FH_UINT16  class_id;                //类别索引 | [ ]
	FH_FLOAT   conf;                    //置信度 | [0.0 - 1.0]
	FH_RECT_T  bbox;                    //矩形信息 | [ ]
	POINT_T    landmark[FH_MAX_LK_NUM]; //人脸关键点信息 | [ ]
	FH_FLOAT   occlude[FH_MAX_LK_NUM];  //被遮挡的概率 | [ ]
	FH_FLOAT   quality;                 //人脸质量分数 | [ ]
}FH_DET_BBOX_EXT_T;

typedef struct
{
	FH_UINT16  class_id;   //类别索引 | [ ]
	FH_FLOAT   conf;       //置信度  | [0.0 - 1.0]
	FH_RECT_T  bbox;       //矩形信息  | [ ]
	POINT_T    landmark[FH_MAX_LPD_NUM]; //车牌四个角点坐标 | [ ]
}FH_DET_BBOX_LPD_T;

typedef struct
{
	FH_UINT64 frame_id;                           //矩形信息对应的帧序 | [ ]
	FH_UINT64 time_stamp;                         //矩形信息对应帧的时间戳 | [ ]
	FH_UINT32 boxNum;                             //矩形个数  | [ ]
	FH_DET_BBOX_EXT_T detBBox[FH_MAX_BBOX_NUM];   //具体矩形信息 | [ ]
}FH_DETECTION_EXT_T;

typedef struct
{
	FH_UINT64 frame_id;                           //矩形信息对应的帧序 | [ ]
	FH_UINT64 time_stamp;                         //矩形信息对应帧的时间戳 | [ ]
	FH_UINT32 boxNum;                             //矩形个数  | [ ]
	FH_DET_BBOX_LPD_T detBBox[FH_MAX_BBOX_NUM];   //具体矩形信息 | [ ]
}FH_DETECTION_LPD_T;


typedef struct{
	FH_FLOAT   scale;                     // 量化因子 | [ ]
	FH_SINT32  zero_point;                // 量化因子 | [0 - 255]
	FH_UINT8   val[FH_MAX_EMBEDDING_LEN]; // 特征值 | [ ]
	FH_UINT32  length;                    // 特征值的长度 | [ ]
}FH_EMBEDDING;

typedef struct{
	FH_UINT64 frame_id;      //帧序 | [ ]
	FH_UINT64 time_stamp;    // 时间戳信息 | [ ]
	FH_EMBEDDING embedding_t;// 人脸特征信息 | [ ]
}FH_EMBEDDING_T;

typedef enum {
	FH_ANGRY = 1,     //人脸表型：发怒 | [ ]
	FH_DISGUST,       //人脸表情: 厌烦 | [ ]
	FH_FEAR,          //人脸表情：恐惧 | [ ]
	FH_HAPPY,         //人脸表情: 开心 | [ ]
	FH_SAD,           //人脸表情: 悲伤 | [ ]
	FH_SURPRISE,      //人脸表情: 惊奇 | [ ]
	FH_NEUTRAL        //人脸表情: 中性 | [ ]
}FH_FACE_ATTR_EXPRESSION_T;

typedef enum{
	FH_FEMALE = 1,    //性别判定：男 | [ ]
	FH_MALE           //性别判定：女 | [ ]
}FH_FACE_ATTR_GENDER_T;

typedef enum{
	FH_GLASSES = 1,   //戴眼镜 | [ ]
	FH_NO_GLASSES     //没戴眼睛 | [ ]
}FH_FACE_ATTR_GLASSES_T;

typedef enum {
	FH_MASK = 1,      //戴口罩 | [ ]
	FH_NO_MASK        //没戴口罩 | [ ]
}FH_FACE_ATTR_MASK_T;

typedef struct{
	FH_SINT32 attr;                  //e.g. age, gender, glasses, mask, expression | [ ]
	FH_FLOAT  prob;                  //confidence for the attribute | [ ]
} FH_FACE_ATTR_COMMON_T;

typedef enum{
	FH_PP_DATA_TYPE_FP32 = 0, // 输出数据类型 | [ ]
	FH_PP_DATA_TYPE_UINT8= 1   // 输出数据类型 | [ ]
}FH_PP_DATA_TYPE_E;

typedef struct {
	FH_UINT8          *data;              // 输出buffer地址 | [ ]
	FH_PP_DATA_TYPE_E  type;              // 输出数据类型 | [ ]
	FH_UINT16          dims[FH_MAX_DIM_NUM];    // 输出数据维度信息 | [ ]
	FH_UINT16          pix_stride;        // 输出数据像素stride | [ ]
	FH_UINT16          line_stride;       // 输出数据行stride | [ ]
	FH_SINT32          zero_point;        // 量化因子 | [ ]
	FH_FLOAT           scale;             //量化因子 | [ ]
} FH_QTENSOR_T;

typedef struct {
	FH_QTENSOR_T data[FH_MAX_OUTPUT_NUM];  //输出 tensor信息 | [ ]
	FH_UINT32 size;                        //输出有效的tensor个数 | [ ]
	FH_UINT64 time_stamp;                  // 时间戳信息 | [ ]
} FH_QTENSOR_ARRAY_T;

typedef enum
{
	FH_GESTURE_UNDEFINED = 0,  //未定义　| [ ]
	FH_GESTURE_FIST = 1,       //握拳 | [ ]
	FH_GESTURE_FOUR = 2,      //四　| [ ]
	FH_GESTURE_OK = 3,        //OK | [ ]
	FH_GESTURE_PEACE = 4,     //peace | [ ]
	FH_GESTURE_STOP = 5,      //stop | [ ]
}FH_GUESTURE_TYPE_E;


typedef struct  {
	FH_UINT64 frame_id;                //帧序 | [ ]
	FH_UINT64 time_stamp;              //时间戳信息 | [ ]
	FH_UINT8  type_id;                 //guesture idx | [0-5]
	FH_FLOAT  prob;                    //probability for each decoded guesture | [0.0-1.0]
}FH_GUESTURE_OUT_T;

typedef struct {
	FH_UINT64   frame_id;                 //帧序 | [ ]
	FH_UINT64   time_stamp;             //时间戳信息 | [ ]
	FH_FLOAT    prob;                  //probability for actual face,[0.0-1.0]
}FH_LIV_OUT_T;

typedef struct{
	FH_UINT64 frame_id;                //帧序 | [ ]
	FH_UINT64 time_stamp;              //时间戳信息 | [ ]
	FH_FLOAT  age;                     //年龄 | [0, 90]
	FH_FACE_ATTR_COMMON_T gender;      //gender: male/female | [ ]
	FH_FACE_ATTR_COMMON_T glasses;     //glasses: glasses/no-glasses | [ ]
	FH_FACE_ATTR_COMMON_T mask;        //mask: mask/no-mask	| [ ]
	FH_FACE_ATTR_COMMON_T expr;        //"angry", "disgust", "fear", "happy", "sad", "surprise", "neutral" | [ ]
}FH_FACE_ATTR_T;

typedef struct {
	FH_UINT64 time_stamp;                // 时间戳信息 | [ ]
	FH_UINT32  points_num;                // 关键点数量 | [ ];
	POINT_T points[FH_FACEKPT_MAX_NUM];  //人脸68点关键点坐标
}FH_FACEKPT_T;

typedef struct{
	FH_UINT64 time_stamp;            // 时间戳信息 | [ ]
	FH_UINT8  val_dec[FH_LPR_LLEN];  //解码车牌字符索引 | [ ]
	FH_FLOAT  prob_dec[FH_LPR_LLEN]; //每个解码字符串概率| [0.0 - 1.0]
	FH_UINT8  plate_len;             //解码车牌字符串长度 | [ ]
}FH_LPR_T;

typedef struct{
	FH_TYPE_E net_type;
	union{
		FH_DETECTION_T     common_t;           //其他检测网络结果 | [ ]
		FH_DETECTION_EXT_T face_detect_t;      //人脸关键点结果 | [ ]
		FH_DETECTION_LPD_T lpd_detect_t;       //车牌关键点 | [ ]
		FH_EMBEDDING_T     face_recognition_t; //人脸识别 | [ ]
		FH_FACE_ATTR_T     face_attr;          //人脸属性 | [ ]
		FH_LPR_T           lpr_t;              //车牌识别 | [ ]
		FH_FACEKPT_T       facekpt_t;          //人脸68关键点 | [ ]
		FH_GUESTURE_OUT_T  guesture_t;         //手势识别 | [ ]
		FH_LIV_OUT_T       liv_out_t;          //活体检测 | [ ]
	};
}FH_DETECTION_INFO;

typedef enum
{
	FH_IMAGE_FORMAT_RGB888 = (1 << 1) //输入图像的颜色格式 | [ ]
}FH_IMAGE_TYPE_E;

typedef struct
{
	FH_UINT8 *nbg_data;
	FH_UINT32 type;             //网络检测类型 | [1 - 3]
	FH_UINT32 src_w_in;         //网络检测宽度 | [ ]
	FH_UINT32 src_h_in;         //网络检测高度 | [ ]
	FH_UINT32 src_c_in;         //网络检测深度 | [ ]
	FH_FLOAT conf_thr;          //网络检测阈值 | [ ]
	FH_NNA_ROT rotate;          //旋转角度 | [ ]
	FH_VOID *reserved;          //后续兼容使用 | [ ]
}FH_NN_INIT_PARAM_T;

typedef struct
{
	FH_BOOL query_mem;             //查询内存 | [ ]
	FH_UINT32 mem_size;            //获取总memory大小 | [ ]
	FH_UINT32 cfg_size;            //获取cfg memory大小 | [ ]
	FH_UINT32 fm_size;            //获取fm memory大小 | [ ]
	FH_USR_MEM_INFO mem_info;      //具体memory信息 | [ ]
	FH_USR_MEM_INFO fm_mem_info;      //具体fm memory信息 | [ ]
	FH_NN_INIT_PARAM_T init_param; //网络配置参数 | [ ]
}FH_NN_INIT_USR_PARAM_T;

typedef struct
{
	FH_UINT32 width;           //输入图像数据的宽 | [ ]
	FH_UINT32 height;          //输入图像数据的高 | [ ]
	FH_UINT32 stride;          //输入图像的stride | [ ]
	FH_UINT64 timestamp;       //输入图像时间戳 | [ ]
	FH_UINT64 frame_id;        //帧序 | [ ]
	FH_IMAGE_TYPE_E imageType; //输入图像的像素格式 | [RGB888]
	FH_MEM_INFO src_data;      //输入图像的地址信息 | [ ]
	FH_UINT32 pool_id;         //vb pool id(非VB模式建议配置为-1UL) | []
}FH_IMAGE_T;

typedef struct
{
	FH_FLOAT conf_thr; //网络检测阈值 | [0.0 - 1.0]
}FH_DET_SETPARAM_T;

typedef struct
{
    FH_UINT32 cls_id;   ///检测类型
	FH_FLOAT conf_thr; //网络检测阈值 | [0.0 - 1.0]
}FH_DET_SETPARAM_EXT_T;

typedef enum
{
    FH_NO_MULTIPLEX = 0,                    //默认不做内存优化，软件行为保持和之前一致 | [ ]
    FH_AUTO_MODE,                              //自动模式，各通道做buffer复用，新增通道时，会自动判断buffer是否够用，不够则重新申请 | [ ]
    FH_MANUAL_MODE,                            //手动模式，各通道做buffer复用，但用户需要调用buffer分配函数来进行buffer更新 | [ ]
    FH_USER_ALLOC_FM_BUF = 0x10,               //用户自己做buffer管理 | [ ]
}FH_NN_BUF_MULTIPLEX_MODE_E;

typedef struct
{
    FH_NN_BUF_MULTIPLEX_MODE_E multiplex_mode;  //不依赖于网络id比对的内存优化模式选择,默认不开启，如需开启，需要在所有通道初始化前配置 | [ ]
    FH_UINT32 nbg_id_cmp_bypass;    //依赖于网络id比对的内存优化bypass，默认开启优化 | [ ]
} FH_NN_OPTIMIZE_PARA_T;

#pragma pack()
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
