

#define FPS(x)  (10000000/x)

#include "rtconfig_app.h"
/**
 *   Note!!!
 *     FPS must set the bigger one in front of the smaller one,
 *     for example: FPS(30) must infront of FPS(25) !!!!
 */
static const struct uvc_frame_info uvc_frames_nv12[] = {
    { 320, 240,   {FPS(30), FPS(25), FPS(15), 0 }, 3},
    { 640, 368,   {FPS(30), FPS(25), FPS(15), 0 }, 2},
    { 640, 480,   {FPS(30), FPS(25), FPS(15), 0 }, 2},
    { 1024, 576,  {FPS(25), FPS(20), FPS(15), 0 }, 1},
    { 1280, 720,  {FPS(15), FPS(10), FPS(5), 0 }, 1},
    { 1920, 1080,  {FPS(5), 0 }, 1},
    { 0, 0, { 0, }, },
};

static const struct uvc_frame_info uvc_frames_yuy2[] = {
    {320, 240,  {FPS(30), FPS(25), FPS(15), 0 }, 3},
    {480, 272,  {FPS(30), FPS(25), FPS(15), 0 }, 2},
    {640, 368,  {FPS(30), FPS(25), FPS(15), 0 }, 2},
    {640, 480,  {FPS(30), FPS(25), FPS(15), 0 }, 1},
    {1024, 576, {FPS(20), FPS(15), FPS(10), 0 }, 1},
    { 1280, 720,  {FPS(10), FPS(5), 0 }, 1},
    { 1920, 1080,  {FPS(5), 0 }, 1},
    { 0, 0, { 0, }, },
};

static const struct uvc_frame_info uvc_frames_mjpeg[] = {
#ifdef FH_USING_OVOS08_MIPI
    {1920, 1080, {FPS(60), 0 }, 1},
    {640, 360,   {FPS(60), 0 }, 5},
    {640, 480,   {FPS(60), 0 }, 4},
    {1024, 576,  {FPS(60), 0 }, 3},
    {1280, 720,  {FPS(60), 0 }, 2},
    {2560, 1440, {FPS(15), 0 }, 1},
    {3840, 2160, {FPS(15), 0 }, 1},
    {1920, 1080, {FPS(60), 0 }, 1},
#elif defined(FH_USING_OVOS02K_MIPI) || defined(FH_USING_OVOS05_MIPI)
    {640, 360,   {FPS(30), FPS(25), FPS(15), 0 }, 5},
    {640, 480,   {FPS(30), FPS(25), FPS(15), 0 }, 4},
    {1024, 576,  {FPS(30), FPS(25), FPS(15), 0 }, 3},
    {1280, 720,  {FPS(30), FPS(25), FPS(15), 0 }, 2},
    {2560, 1440, {FPS(30), FPS(25), FPS(15), 0 }, 1},
    {1920, 1080, {FPS(30), FPS(25), FPS(15), 0 }, 1},
#elif defined RT_USING_HS_CUSTOM_8852V201_GC2083_DZ_20230619
	{640, 480,	 {FPS(30), FPS(25), FPS(15), 0 }, 5},
	{800, 600, {FPS(30), FPS(25), FPS(15), 0 }, 1},
	{848, 480,	 {FPS(30), FPS(25), FPS(15), 0 }, 5},
	{1280, 720,  {FPS(15), 0 }, 2},
	{960, 540,	{FPS(30), FPS(25), FPS(15), 0 }, 2},
	{2560, 1440, {FPS(30), FPS(25), FPS(15), 0 }, 1},
	{1920, 1080, {FPS(30), FPS(25), FPS(15), 0 }, 1},
	{640, 480,	 {FPS(30), FPS(25), FPS(15), 0 }, 5},
#else
    {1920, 1080, {FPS(30), FPS(25), FPS(15), 0 }, 1},
    {640, 360,   {FPS(30), FPS(25), FPS(15), 0 }, 5},
    {640, 480,   {FPS(30), FPS(25), FPS(15), 0 }, 4},
    {1024, 576,  {FPS(30), FPS(25), FPS(15), 0 }, 3},
    {1280, 720,  {FPS(30), FPS(25), FPS(15), 0 }, 2},
    {1920, 1080, {FPS(30), FPS(25), FPS(15), 0 }, 1},
#endif
    { 0, 0, { 0, }, },
};

static const struct uvc_frame_info uvc_frames_h264[] = {
#ifdef FH_USING_OVOS08_MIPI
    {640, 360,   {FPS(60), 0 }, 7},
    {640, 480,   {FPS(60), 0 }, 6},
    {1024, 576,  {FPS(60), 0 }, 5},
    {1280, 720,  {FPS(60), 0 }, 5},
    {2560, 1440, {FPS(15), 0 }, 3},
    {3840, 2160, {FPS(15), 0 }, 3},
    {1920, 1080, {FPS(60), 0 }, 4},
#elif defined(FH_USING_OVOS02K_MIPI) || defined(FH_USING_OVOS05_MIPI)
    {640, 360,   {FPS(30), FPS(25), FPS(15), 0 }, 7},
    {640, 480,   {FPS(30), FPS(25), FPS(15), 0 }, 6},
    {1024, 576,  {FPS(30), FPS(25), FPS(15), 0 }, 5},
    {1280, 720,  {FPS(30), FPS(25), FPS(15), 0 }, 5},
    {2560, 1440, {FPS(30), FPS(25), FPS(15), 0 }, 4},
    {1920, 1080, {FPS(30), FPS(25), FPS(15), 0 }, 4},
#elif defined RT_USING_HS_CUSTOM_8852V201_GC2083_DZ_20230619
	{640, 480,	 {FPS(30), FPS(25), FPS(15), 0 }, 5},
	{800, 600, {FPS(30), FPS(25), FPS(15), 0 }, 1},
	{848, 480,	 {FPS(30), FPS(25), FPS(15), 0 }, 5},
	{1280, 720,  {FPS(15), 0 }, 2},
	{960, 540,	{FPS(30), FPS(25), FPS(15), 0 }, 2},
	{2560, 1440, {FPS(30), FPS(25), FPS(15), 0 }, 1},
	{1920, 1080, {FPS(30), FPS(25), FPS(15), 0 }, 1},
	{640, 480,	 {FPS(30), FPS(25), FPS(15), 0 }, 5},
#else
    {640, 360,   {FPS(30), FPS(25), FPS(15), 0 }, 7},
    {640, 480,   {FPS(30), FPS(25), FPS(15), 0 }, 6},
    {1024, 576,  {FPS(30), FPS(25), FPS(15), 0 }, 5},
    {1280, 720,  {FPS(30), FPS(25), FPS(15), 0 }, 5},
    {1920, 1080, {FPS(30), FPS(25), FPS(15), 0 }, 4},
#endif
    { 0, 0, { 0, }, },
};

static const struct uvc_frame_info uvc_frames_h265[] = {
#ifdef FH_USING_OVOS08_MIPI
    {640, 360,   {FPS(60), 0 }, 7},
    {640, 480,   {FPS(60), 0 }, 6},
    {1024, 576,  {FPS(60), 0 }, 5},
    {1280, 720,  {FPS(60), 0 }, 5},
    {2560, 1440, {FPS(15), 0 }, 3},
    {3840, 2160, {FPS(15), 0 }, 3},
    {1920, 1080, {FPS(60), 0 }, 4},
#elif defined(FH_USING_OVOS02K_MIPI) || defined(FH_USING_OVOS05_MIPI)
    {640, 360,   {FPS(30), FPS(25), FPS(15), 0 }, 7},
    {640, 480,   {FPS(30), FPS(25), FPS(15), 0 }, 6},
    {1024, 576,  {FPS(30), FPS(25), FPS(15), 0 }, 5},
    {1280, 720,  {FPS(30), FPS(25), FPS(15), 0 }, 4},
    {2560, 1440, {FPS(30), FPS(25), FPS(15), 0 }, 3},
    {1920, 1080, {FPS(30), FPS(25), FPS(15), 0 }, 4},
#elif defined RT_USING_HS_CUSTOM_8852V201_GC2083_DZ_20230619
	{640, 480,	 {FPS(30), FPS(25), FPS(15), 0 }, 5},
	{800, 600, {FPS(30), FPS(25), FPS(15), 0 }, 1},
	{848, 480,	 {FPS(30), FPS(25), FPS(15), 0 }, 5},
	{1280, 720,  {FPS(15), 0 }, 2},
	{960, 540,	{FPS(30), FPS(25), FPS(15), 0 }, 2},
	{2560, 1440, {FPS(30), FPS(25), FPS(15), 0 }, 1},
	{1920, 1080, {FPS(30), FPS(25), FPS(15), 0 }, 1},
	{640, 480,	 {FPS(30), FPS(25), FPS(15), 0 }, 5},
#else
    {640, 360,   {FPS(30), FPS(25), FPS(15), 0 }, 7},
    {640, 480,   {FPS(30), FPS(25), FPS(15), 0 }, 6},
    {1024, 576,  {FPS(30), FPS(25), FPS(15), 0 }, 5},
    {1280, 720,  {FPS(30), FPS(25), FPS(15), 0 }, 5},
    {1920, 1080, {FPS(30), FPS(25), FPS(15), 0 }, 4},
#endif
    { 0, 0, { 0, }, 0},
};

static const struct uvc_format_info formats[] = {
#if defined RT_USING_HS_CUSTOM_8852V201_GC2083_DZ_20230619
	{V4L2_PIX_FMT_MJPEG, uvc_frames_mjpeg, 6},
	{V4L2_PIX_FMT_YUY2, uvc_frames_yuy2, 5},
	{V4L2_PIX_FMT_H265, uvc_frames_h265, 5},
	{V4L2_PIX_FMT_NV12, uvc_frames_nv12, 5},
	{V4L2_PIX_FMT_H264, uvc_frames_h264, 5},
#else
    {V4L2_PIX_FMT_MJPEG, uvc_frames_mjpeg, 6},
    {V4L2_PIX_FMT_YUY2, uvc_frames_yuy2, 5},
#ifndef UVC_SUPPORT_WINDOWS_HELLO_FACE
    {V4L2_PIX_FMT_NV12, uvc_frames_nv12, 5},
    {V4L2_PIX_FMT_H264, uvc_frames_h264, 5},
#if !defined(CONFIG_ARCH_FH8626V200)
    {V4L2_PIX_FMT_H265, uvc_frames_h265, 5},
#endif
#endif

#endif
};

#ifdef UVC_DOUBLE_STREAM

static const struct uvc_frame_info uvc_frames2_mjpeg[] = {

    {320, 240, {FPS(30), FPS(25), 0 }, },
    {640, 360, {FPS(30), FPS(25), 0 }, },
    {640, 480, {FPS(30), FPS(25), 0 }, },
    {720, 576, {FPS(30), FPS(25), 0 }, },
    { 0, 0, { 0, }, },
};

static const struct uvc_frame_info uvc_frames2_h264[] = {
    {320, 240, {FPS(30), FPS(25), 0 }, },
    {640, 360, {FPS(30), FPS(25), 0 }, },
    {640, 480, {FPS(30), FPS(25), 0 }, },
    {720, 576, {FPS(30), FPS(25), 0 }, },
    { 0, 0, { 0, }, },
};

static const struct uvc_frame_info uvc_frames2_h265[] = {
    {320, 240, {FPS(30), FPS(25), 0 }, },
    {640, 360, {FPS(30), FPS(25), 0 }, },
    {640, 480, {FPS(30), FPS(25), 0 }, },
    {720, 576, {FPS(30), FPS(25), 0 }, },
    { 0, 0, { 0, }, },
};

static const struct uvc_frame_info uvc_frames2_ir[] = {
    {624, 352, {FPS(30), 0 }, },
    { 0, 0, { 0, }, },
};
static const struct uvc_format_info formats2[] = {
#ifdef UVC_SUPPORT_WINDOWS_HELLO_FACE
    {V4L2_PIX_FMT_IR, uvc_frames2_ir, 1},
#else
    {V4L2_PIX_FMT_H264, uvc_frames2_h264, 1},
    {V4L2_PIX_FMT_MJPEG, uvc_frames2_mjpeg, 1},
#if !defined(CONFIG_ARCH_FH8626V200)
    {V4L2_PIX_FMT_H265, uvc_frames2_h265, 1},
#endif
#endif
};
#endif


