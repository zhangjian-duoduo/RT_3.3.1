/*#TAG_RELEASE_FH865X#*/
#ifndef __FH_IVE_MPI_H__
#define __FH_IVE_MPI_H__
/**IVE**/

#include "types/type_def.h"
#include "fh_common.h"
#include "fh_ive_mpipara.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

/*
*   Name: FH_IVE_V1_Resize
*            创建图像缩放任务
*
*   Parameters:
*
*       [out] IVE_HANDLE *handle
*            IVE任务句柄
*
*       [in] IVE_V1_RESIZE_CTRL *ctrl
*            缩放任务控制参数
*
*       [in] FH_BOOL instant
*            接口是否阻塞
*
*   Return:
*           0(成功)
*          非0(失败，详见错误码)
*
*   Note:
*          仅FH8652 & FH8656 & FH8658支持,需加载BGM驱动
*/
FH_SINT32 FH_IVE_V1_Resize(IVE_HANDLE *handle,IVE_V1_RESIZE_CTRL *ctrl,FH_BOOL instant);

/*
*   Name: FH_IVE_V1_Filter
*            创建图像滤波任务
*
*   Parameters:
*
*       [out] IVE_HANDLE *handle
*            IVE任务句柄
*
*       [in] IVE_V1_FILTER_CTRL *ctrl
*            滤波任务控制参数
*
*       [in] FH_BOOL instant
*            接口是否阻塞
*
*   Return:
*           0(成功)
*          非0(失败，详见错误码)
*
*   Note:
*          仅FH8652 & FH8656 & FH8658支持,需加载BGM驱动
*/
FH_SINT32 FH_IVE_V1_Filter(IVE_HANDLE *handle,IVE_V1_FILTER_CTRL *ctrl,FH_BOOL instant);

/*
*   Name: FH_IVE_V1_MvDetect
*            创建运动检测任务
*
*   Parameters:
*
*       [out] IVE_HANDLE *handle
*            IVE任务句柄
*
*       [in] IVE_V1_MV_DETECT_CTRL *ctrl
*            运动检测任务控制参数
*
*       [in] FH_BOOL instant
*            接口是否阻塞
*
*   Return:
*           0(成功)
*          非0(失败，详见错误码)
*
*   Note:
*          仅FH8652 & FH8656 & FH8658支持,需加载BGM驱动
*/
FH_SINT32 FH_IVE_V1_MvDetect(IVE_HANDLE *handle,IVE_V1_MV_DETECT_CTRL *ctrl,FH_BOOL instant);

/*
*   Name: FH_IVE_Query
*            查询IVE任务是否完成
*
*   Parameters:
*
*       [in] IVE_HANDLE handle
*            IVE任务句柄
*
*       [out] FH_BOOL *isfinish
*            IVE任务是否完成标志
*
*       [in] FH_BOOL isblock
*            查询接口是否阻塞标志
*
*   Return:
*           0(成功)
*          非0(失败，详见错误码)
*
*   Note:
*          当句柄中硬件模块标记相同时，如果任务标记大的任务已完成，代表比其小的任务也都已经完成。
*          硬件模块标记不同时，２个任务标记相互独立，不能推导出另外不同模块标记之间任务是否完成。
*/
FH_SINT32 FH_IVE_Query(IVE_HANDLE handle,FH_BOOL *isfinish,FH_BOOL isblock);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /*__MPI_VO_H__ */
