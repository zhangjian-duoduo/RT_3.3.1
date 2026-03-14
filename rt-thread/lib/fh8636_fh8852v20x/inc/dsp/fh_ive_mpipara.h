/*#TAG_RELEASE_FH865X#*/
#ifndef __FH_IVE_MPIPARA_H__
#define __FH_IVE_MPIPARA_H__
/**|IVE|**/

#include "types/type_def.h"
#include "fh_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */
#pragma pack(4)

typedef struct {
	FH_UINT64 handle_idx;      // 任务标记 | [ ]
	FH_UINT32 module_idx;      // 硬件模块标记 | [ ]
} IVE_HANDLE;

typedef struct
{
	FH_SIZE     src_size;      // 输入图像幅面 | [ ]
	FH_SIZE     dst_size;      // 输出图像幅面 | [ ]
	FH_MEM_INFO src_addr;      // 输入图像地址 | [ ]
	FH_MEM_INFO dst_addr;      // 输出图像地址 | [ ]
	FH_UINT8    data_clip_min; // 输出数据最小值限制 | [0-255]
	FH_UINT8    data_clip_max; // 输出数据最大值限制 | [0-255]
} IVE_V1_RESIZE_CTRL;

typedef struct
{
	FH_SIZE      image_size;    // 输入图像幅面 | [ ]
	FH_UINT32    filter_mode;   // 滤波模式0:高斯滤波 1:同态滤波 | [0-1]
	FH_UINT16    gau_coeff0;    // 高斯滤波器系数 | [0-511]
	FH_UINT8     gau_coeff1;    // 高斯滤波器系数 | [0-63]
	FH_UINT8     gau_coeff2;    // 高斯滤波器系数 | [0-63]
	FH_UINT8     gau_coeff3;    // 高斯滤波器系数 | [0-63]
	FH_UINT8     gau_coeff4;    // 高斯滤波器系数 | [0-63]
	FH_UINT8     gau_coeff5;    // 高斯滤波器系数 | [0-63]
	FH_UINT8     data_clip_min; // 输出数据最小值限制 | [0-255]
	FH_UINT8     data_clip_max; // 输出数据最大值限制 | [0-255]
	FH_MEM_INFO  src_addr;      // 输入图像地址 | [ ]
	FH_MEM_INFO  dst_addr;      // 输出图像地址 | [ ]
} IVE_V1_FILTER_CTRL;

typedef struct
{
	FH_SIZE      image_size;    // 输入图像幅面 | [ ]
	FH_UINT8     detect_th;     // 二值化门限,推荐值100 | [0-255]
	FH_UINT16    gau_coeff0;    // 高斯滤波器系数,推荐值10 | [0-511]
	FH_UINT8     gau_coeff1;    // 高斯滤波器系数,推荐值10 | [0-63]
	FH_UINT8     gau_coeff2;    // 高斯滤波器系数,推荐值10 | [0-63]
	FH_UINT8     gau_coeff3;    // 高斯滤波器系数,推荐值10 | [0-63]
	FH_UINT8     gau_coeff4;    // 高斯滤波器系数,推荐值10 | [0-63]
	FH_UINT8     gau_coeff5;    // 高斯滤波器系数,推荐值10 | [0-63]
	FH_UINT8     data_clip_min; // 输出数据最小值限制,推荐值0 | [0-255]
	FH_UINT8     data_clip_max; // 输出数据最大值限制,推荐值255 | [0-255]
	FH_MEM_INFO  cur_image;     // 当前帧图像地址 | [ ]
	FH_MEM_INFO  bkg_image;     // 背景帧图像地址 | [ ]
	FH_MEM_INFO  mv_det_data;   // 输出运动二值图地址 | [ ]
} IVE_V1_MV_DETECT_CTRL;

#pragma pack()
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

