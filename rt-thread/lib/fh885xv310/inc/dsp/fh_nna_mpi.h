#ifndef __FH_NNA_MPI_H__
#define __FH_NNA_MPI_H__
/**NNA**/

//#include "fh_common.h"
//#include "media_process.h"
#include "types/type_def.h"
#include "fh_nna_mpipara.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {

#endif
#endif /* __cplusplus */
/*
*   Name: FH_NNA_DET_Init
*            根据输入的网路文件初始化整个网络，并创建文件句柄.
*
*   Parameters:
*
*       [out] FH_VOID **detHandle
*            初始化网络后返回的文件句柄，供后续其他函数调用．
*
*       [in] FH_UINT32 chan
*            指定的通道号
*
*	[in] FH_NN_INIT_PARAM_T *param
*	     网络输入属性
*
*   Return:
*           0(成功)
*          非0(失败，详见错误码)
*   Note:
*          根据输入的NBG文件创建网络的文件句柄，同时会校验该网络的一些数据，如网络支持的宽，高，网络的类型
*
*
*/
FH_SINT32 FH_NNA_DET_Init(FH_VOID **detHandle, FH_UINT32 chan, FH_NN_INIT_PARAM_T *param);
/*
*   Name: FH_NNA_DET_USRALLOC_Init
*            根据输入的网路文件初始化整个网络，并创建文件句柄.
*
*   Parameters:
*
*       [out] FH_VOID **detHandle
*            初始化网络后返回的文件句柄，供后续其他函数调用．
*
*       [in] FH_UINT32 chan
*            指定的通道号
*
*	[in] FH_NN_INIT_USR_PARAM_T *param
*	     网络输入属性
*
*   Return:
*           0(成功)
*          非0(失败，详见错误码)
*   Note:
*         1, 根据输入的NBG文件创建网络的文件句柄，同时会校验该网络的一些数据，如网络支持的宽，高，网络的类型
*         2, 函数使用方法：该函数需要调用两次，第一次需要设置param中的query_mem为FH_TRUE,获取当前模型需要内存
*            大小，然后应用程序分配内存传递给param中的mem_info。此时内存查询完毕，需设置query_mem为FH_FALSE
*            再次调用该函数，完成真正初始化,内存信息描述结构为FH_USR_MEM_INFO.
*
*
*/

FH_SINT32 FH_NNA_DET_USRALLOC_Init(FH_VOID **detHandle, FH_UINT32 chan, FH_NN_INIT_USR_PARAM_T *param);

/*
*   Name: FH_NNA_DET_CUSTOM_OutTensorInfo
*            根据初始化NBG获取输出Tensor信息
*
*   Parameters:
*
*       [in] FH_VOID *detHandle
*            初始化接口根据NBG返回的模型句柄
*
*
*       [out] FH_QTENSOR_ARRAY_T *out
*            网络输入属性
*
*   Return:
*           0(成功)
*          非0(失败，详见错误码)
*   Note:
*          根据输入NBG模型文件信息，获取网络输出tensor信息
*
*
*/

FH_SINT32 FH_NNA_DET_CUSTOM_OutTensorInfo(FH_VOID *detHandle,  FH_QTENSOR_ARRAY_T *out);

/*
*   Name: FH_NNA_DET_Process
*            根据输入的文件句柄，完成网络的前向推理，得到网络检测到的Bbox信息等供应用使用该信息
*
*   Parameters:
*
*       [in] FH_VOID *detHandle
*            上述函数创建的句柄.
*
*       [in] FH_IMAGE_T *src
*            网络输入图像数据信息，包括图像格式，图像的宽高．以及输入的buffer地址，需要物理地址．
*
*       [out] FH_DETECTION_T *out
*            网络前向推理的结果，　包括的得到的矩形信息个数以及具体的坐标及相应帧的时间戳．
*
*   Return:
*           0(成功)
*          非0(失败，详见错误码)
*   Note:
*          前向推理内部会检测输入图像的幅面信息是否跟网络自身检测范围匹配．同时会检测输入的图像格式．
* 　　　　 同时需要知道输入的buffer物理地址．
*
*
*/

FH_SINT32 FH_NNA_DET_Process(FH_VOID *detHandle, FH_IMAGE_T *src,  FH_DETECTION_T *out);
/*
*   Name: FH_NNA_UNIFY_DET_Process
*            根据输入的文件句柄，完成网络的前向推理，得到网络检测到的Bbox信息等供应用使用该信息
*
*   Parameters:
*
*       [in] FH_VOID *detHandle
*            上述函数创建的句柄.
*
*       [in] FH_VIDEO_FRAME *src
*            网络输入图像数据信息，包括图像格式，图像的宽高．以及输入的buffer地址，需要物理地址．
*
*       [out] FH_DETECTION_T *out
*            网络前向推理的结果，　包括的得到的矩形信息个数以及具体的坐标及相应帧的时间戳．
*
*   Return:
*           0(成功)
*          非0(失败，详见错误码)
*   Note:
*          前向推理内部会检测输入图像的幅面信息是否跟网络自身检测范围匹配．同时会检测输入的图像格式．
* 　　　　 同时需要知道输入的buffer物理地址．
*
*
*/

FH_SINT32 FH_NNA_UNIFY_DET_Process(FH_VOID *detHandle, FH_VIDEO_FRAME *src,  FH_DETECTION_T *out);
/*
*   Name: FH_NNA_DET_POINT_Process
*            根据输入的文件句柄，完成网络的前向推理，得到网络检测到的Bbox信息等供应用使用该信息
*
*   Parameters:
*
*       [in] FH_VOID *detHandle
*            上述函数创建的句柄.
*
*       [in] FH_IMAGE_T *src
*            网络输入图像数据信息，包括图像格式，图像的宽高．以及输入的buffer地址，需要物理地址．
*
*       [out] FH_DETECTION_INFO *out
*            网络前向推理的结果，　包括的得到的矩形信息个数以及具体的坐标及相应帧的时间戳．
*
*   Return:
*           0(成功)
*          非0(失败，详见错误码)
*   Note:
*          前向推理内部会检测输入图像的幅面信息是否跟网络自身检测范围匹配．同时会检测输入的图像格式．
* 　　　　 同时需要知道输入的buffer物理地址．
*
*
*/

FH_SINT32 FH_NNA_DET_POINT_Process(FH_VOID *detHandle, FH_IMAGE_T *src,	FH_DETECTION_INFO *out);


/*
*   Name: FH_NNA_UNIFY_DET_POINT_Process
*            根据输入的文件句柄，完成网络的前向推理，得到网络检测到的Bbox信息以及时间戳等供应用使用该信息
*
*   Parameters:
*
*       [in] FH_VOID *detHandle
*            上述函数创建的句柄.
*
*       [in] FH_VIDEO_FRAME *src
*            网络输入图像数据信息，包括图像格式，图像的宽高．以及输入的buffer地址，需要物理地址．
*
*       [out] FH_DETECTION_T *out
*            网络前向推理的结果，　包括的得到的矩形信息个数以及具体的坐标及相应帧的时间戳．
*
*   Return:
*           0(成功)
*          非0(失败，详见错误码)
*   Note:
*/

FH_SINT32 FH_NNA_UNIFY_DET_POINT_Process(FH_VOID *detHandle, FH_VIDEO_FRAME *src, FH_DETECTION_INFO *out);


/*
*   Name: FH_NNA_DET_Forward
*            根据输入的文件句柄，完成网络的前向推理，得到网络检测到的Bbox信息以及时间戳等供应用使用该信息
*
*   Parameters:
*
*       [in] FH_VOID *detHandle
*            上述函数创建的句柄.
*
*       [out] FH_DETECTION_T *out
*            网络前向推理的结果，　包括的得到的矩形信息个数以及具体的坐标及相应帧的时间戳．
*
*   Return:
*           0(成功)
*          非0(失败，详见错误码)
*   Note:
*/

FH_SINT32 FH_NNA_DET_Forward(FH_VOID *detHandle, FH_DETECTION_T *out);
/*
*   Name: FH_NNA_DET_POINT_Forward
*            根据输入的文件句柄，完成网络的前向推理，得到网络检测到的Bbox信息以及时间戳等供应用使用该信息
*
*   Parameters:
*
*       [in] FH_VOID *detHandle
*            上述函数创建的句柄.
*
*       [out] FH_DETECTION_INFO *out
*            网络前向推理的结果，　包括的得到的矩形信息个数以及具体的坐标及相应帧的时间戳．
*
*   Return:
*           0(成功)
*          非0(失败，详见错误码)
*   Note:
*/

FH_SINT32 FH_NNA_DET_POINT_Forward(FH_VOID *detHandle, FH_DETECTION_INFO *out);

/*
*   Name: FH_NNA_DET_CUSTOM_Process
*            根据输入的文件句柄，完成网络的前向推理，得到网络检测到的Bbox信息等供应用使用该信息
*
*   Parameters:
*
*       [in] FH_VOID *detHandle
*            上述函数创建的句柄.
*
*       [in] FH_IMAGE *src
*            网络输入图像数据信息，包括图像格式，图像的宽高．以及输入的buffer地址，需要物理地址．
*
*       [out] FH_QTENSOR_ARRAY_T *out
*            网络前向推理的结果，　包括的得到网络的输出Tensor信息．
*
*   Return:
*           0(成功)
*          非0(失败，详见错误码)
*   Note:
*          前向推理内部会检测输入图像的幅面信息是否跟网络自身检测范围匹配．同时会检测输入的图像格式．
* 　　　　 同时需要知道输入的buffer物理地址．
*          FH_NNA_DET_POST_Process 此函数返回的是网络推理的结果，没有经过后处理，需要用户自定义后处理
*          后出完成后，需要调用FH_NNA_DET_CUSTOM_Release 释放资源.
*
*
*/

FH_SINT32 FH_NNA_DET_CUSTOM_Process(FH_VOID *detHandle, FH_IMAGE_T *src,  FH_QTENSOR_ARRAY_T *out);

/*
*   Name: FH_NNA_DET_CUSTOM_POST_Process
*            根据输入的文件句柄，完成网络的前向推理，得到网络检测到的Bbox信息等供应用使用该信息
*
*   Parameters:
*
*       [in] FH_VOID *detHandle
*            上述函数创建的句柄.
*
*       [in] FH_QTENSOR_ARRAY_T *in
*            网络推理输出的数据，不包括后处理数据信息.
*
*       [out] FH_DETECTION_T *out
*            网络前向推理的结果，　包括的得到网络的输出检测框信息
*
*   Return:
*           0(成功)
*          非0(失败，详见错误码)
*   Note:
*          前向推理内部会检测输入图像的幅面信息是否跟网络自身检测范围匹配．同时会检测输入的图像格式．
* 　　　　 同时需要知道输入的buffer物理地址．
*          FH_NNA_DET_POST_Process 此函数返回的是网络推理的结果，没有经过后处理，需要用户自定义后处理
*          后出完成后，需要调用FH_NNA_DET_CUSTOM_Release 释放资源.
*
*
*/

FH_SINT32 FH_NNA_DET_CUSTOM_POST_Process(FH_VOID *detHandle,  FH_QTENSOR_ARRAY_T *in, FH_DETECTION_T *out);

/*
*   Name: FH_NNA_DET_CUSTOM_Forward
*            根据输入的文件句柄，完成网络的前向推理，得到网络检测到的Bbox信息以及时间戳等供应用使用该信息
*
*   Parameters:
*
*       [in] FH_VOID *detHandle
*            上述函数创建的句柄.
*
*       [out] FH_QTENSOR_ARRAY_T *out
*            网络前向推理的结过，得到输出Tensor信息.
*
*   Return:
*           0(成功)
*          非0(失败，详见错误码)
*   Note: 此接口属于新增加接口，输出时推理的数据地址以及大小，应用需自定义后处理
*         后处理结束后需要调用FH_NNA_DET_CUSTOM_Release释放资源
*/

FH_SINT32 FH_NNA_DET_CUSTOM_Forward(FH_VOID *detHandle, FH_QTENSOR_ARRAY_T *out);

/*
*   Name: FH_NNA_DET_CUSTOM_Release
*            网络资源的销毁
*
*   Parameters:
*
*       [in] FH_VOID *detHandle
*            创建的网络句柄.
*
*
*   Return:
*           0(成功)
*          非0(失败，详见错误码)
*   Note:
*          释放相应的资源(包括及后处理buffer).
*          配合FH_NNA_DET_CUSTOM_Forward，FH_NNA_DET_CUSTOM_Process使用，释放资源.
*
*
*/

FH_SINT32 FH_NNA_DET_CUSTOM_Release(FH_VOID *detHandle);

/*
*   Name: FH_NNA_DET_SetParam
*            设置网络的检测阈值，前向推理过程种可以动态设置检测阈值．
*
*   Parameters:
*
*       [in] FH_VOID *detHandle
*            创建的网络句柄.
*
*       [in] FH_DET_SETPARAM_T *setParams
*            待设置的检测阈值
*
*   Return:
*           0(成功)
*          非0(失败，详见错误码)
*   Note:
*          在推理前后帧中间，根据需求可动态修改检测阈值，到达理想的检测精度．
*
*
*/

FH_SINT32 FH_NNA_DET_SetParam(FH_VOID *detHandle, FH_DET_SETPARAM_T *setParams);

/*
*   Name: FH_NNA_DET_SetParam_EXT
*            按检测类型设置网络的检测阈值，前向推理过程种可以动态设置检测阈值．
*
*   Parameters:
*
*       [in] FH_VOID *detHandle
*            创建的网络句柄.
*
*       [in] FH_DET_SETPARAM_EXT_T *setParams
*            待设置的检测类型和检测阈值
*
*   Return:
*           0(成功)
*          非0(失败，详见错误码)
*   Note:
*          在推理前后帧中间，根据需求可动态修改检测阈值，到达理想的检测精度．
*
*
*/

FH_SINT32 FH_NNA_DET_SetParam_EXT(FH_VOID *detHandle, FH_DET_SETPARAM_EXT_T *setParams);

/*
*   Name: FH_NNA_DET_SetNmsThresh
*            设置网络的清理检测阈值.
*
*   Parameters:
*
*       [in] FH_VOID *detHandle
*            创建的网络句柄.
*
*       [in] FH_FLOAT *nms_thresh
*            清理检测阈值
*
*   Return:
*           0(成功)
*          非0(失败，详见错误码)
*   Note:
*          该阈值用于判断多个检测结果是否是对同一目标的检测.从而决定是否将他们合并.
*          nms_thresh越低,框个数越少.
*
*/
FH_SINT32 FH_NNA_DET_SetNmsThresh(FH_VOID *detHandle, FH_FLOAT nms_thresh);

/*
*   Name: FH_NNA_DET_Exit
*            网络资源的销毁
*
*   Parameters:
*
*       [in] FH_VOID *detHandle
*            创建的网络句柄.
*
*
*   Return:
*           0(成功)
*          非0(失败，详见错误码)
*   Note:
*          销毁相应的资源(包括网络资源及进程资源).
*
*
*/

FH_SINT32 FH_NNA_DET_Exit(FH_VOID *detHandle);
/*
*   Name: FH_NNA_GetNbgVer
*            获取nbg版本号
*
*   Parameters:
*
*       [in] FH_VOID *detHandle
*            创建的网络句柄.
*
*       [out] FH_UINT32 *netVersion
*            网络模型版本号
*
*   Return:
*           0(成功)
*          非0(失败，详见错误码)
*/
FH_SINT32 FH_NNA_GetNbgVer(FH_VOID *detHandle, FH_UINT32 *netVersion);

/*
*   Name: FH_NNA_UpdateUsrFmBuff
*            更新用户分配的Fm Buff, FH_USER_ALLOC_FM_BUF有效
*
*   Parameters:
*
*       [in] FH_USR_MEM_INFO *usr_mem
*            用户分配的fm buf
*   Return:
*           0(成功)
*          非0(失败，详见错误码)
*/
FH_SINT32 FH_NNA_UpdateUsrFmBuff(FH_USR_MEM_INFO *usr_mem);

/*
*   Name: FH_NNA_UpdateFmBuff
*            更新Fm Buff, FH_MANUAL_MODE有效
*
*   Parameters:
*
*       [in] FH_VOID
*
*   Return:
*           0(成功)
*          非0(失败，详见错误码)
*/
FH_SINT32 FH_NNA_UpdateFmBuff(FH_VOID);

/*
*   Name: FH_NNA_OptimizePara
*            设置NNA优化参数,必须在所有通道初始化之前设置,否则报错
*
*   Parameters:
*
*       [in] FH_NN_OPTIMIZE_PARA_T *para
*            优化参数.
*
*   Return:
*           0(成功)
*          非0(失败，详见错误码)
*/
FH_SINT32 FH_NNA_OptimizePara(FH_NN_OPTIMIZE_PARA_T *para);

/*
*   Name: FH_NNA_GetMaxFmSize()
*            获取当前所有通道的fm size的最大值
*
*   Parameters:
*
*       [out] FH_UINT32 *size
*             当前所有通道的fm size的最大值
*
*   Return:
*           0(成功)
*          非0(失败，详见错误码)
*/
FH_SINT32 FH_NNA_GetMaxFmSize(FH_UINT32 *size);

/*
*   Name: FH_NNA_SetRotate
*            设置NN网络旋转角度
*
*   Parameters:
*
*       [in] FH_VOID *detHandle
*            创建的网络句柄.
*
*       [in] FH_NNA_ROT rotate
*            旋转角度
*
*   Return:
*           0(成功)
*          非0(失败，详见错误码)
*/
FH_SINT32 FH_NNA_SetRotate(FH_VOID *detHandle, FH_NNA_ROT rotate);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

