/**
 * @file     motionDetect.h
 * @brief    motion detect module interface
 * @version  V1.0.0
 * @date     12-Dec-2018
 * @author   Software Team
 *
 * @note
 * Copyright (C) 2018 Shanghai Fullhan Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * @par
 * Fullhan is supplying this software which provides customers with programming
 * information regarding the products. Fullhan has no responsibility or
 * liability for the use of the software. Fullhan not guarantee the correctness
 * of this software. Fullhan reserves the right to make changes in the software
 * without notification.
 *
 */

#ifndef MOTIONDETECT_H_
#define MOTIONDETECT_H_

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <time.h>

#include "FH_typedef.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define MAX(a, b)                    ((a) > (b) ? (a) : (b)) // Macro returning max value
#define MIN(a, b)                    ((a) < (b) ? (a) : (b)) // Macro returning min value
#define CLIP(val, min, max)	        (((val)<(min)) ? (min):(((val)>(max))? (max):(val)))

#define MAX_RECTS 5
#define nMaxResults 1000
#define areaTh 20

typedef struct
{
	unsigned int u32X;
	unsigned int u32Y;
	unsigned int u32Width;
	unsigned int u32Height;
} MOtion_ROI;

typedef struct
{
	unsigned int base_w;
	unsigned int base_h;
	unsigned int rect_num;
	MOtion_ROI rect[MAX_RECTS];
} MOtion_BGM_RUNTB_RECT;

typedef struct
{
	unsigned short x1;
	unsigned short x2;
	unsigned short y1;
	unsigned short y2;
	unsigned short root;
	//int fgCnt;
} ORD_INFO;

typedef struct
{
	unsigned short frameW;
	unsigned short frameH;
	unsigned short zoomLen;
	int *gx;
	int *gy;
	int *trace;
	unsigned char *resizeImg;
} MotionParam;

int initMDDetect(MotionParam *inParam, unsigned short zoomLen);
void MotionDetect(unsigned char *src, MotionParam *inParam, unsigned char *foreground);
void getOrdFromGau(unsigned char *foreground, int confindence, MOtion_BGM_RUNTB_RECT *run);
void destoryParam(MotionParam *inParam, unsigned short zoomLen);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  /* !MOTIONDETECT_H_*/
