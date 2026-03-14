#ifndef __FASTVIDEO_CONFIG_H__
#define __FASTVIDEO_CONFIG_H__

#define RT_USING_SC3338_MIPI

#define NN_DETECT_WIDTH     (512)
#define NN_DETECT_HEIGHT    (288)
#define NN_DETECT_CHAN      (2)
#define NN_DETECT_FRAMES    (3)

#define CONFIG_FAST_VIDEO_RESIZE

#define ISP_FORMAT_PRE      FORMAT_640X360P100
#define ISP_WIDTH_PRE       640
#define ISP_HEIGHT_PRE      360
#define VIDEO_WIDTH_PRE     ISP_WIDTH_PRE
#define VIDEO_HEIGHT_PRE    ISP_HEIGHT_PRE
#define CH0_WIDTH_PRE       ISP_WIDTH_PRE
#define CH0_HEIGHT_PRE      ISP_HEIGHT_PRE
#define CH0_FRAME_PRE       100

#define JPEG_ENC_CHNUM      (4)

/* isp configuration */
#define ISP_FORMAT          FORMAT_2304X1296P15
#define ISP_INIT_WIDTH      2304
#define ISP_INIT_HEIGHT     1296

/* dsp configuration */
#define VIDEO_INPUT_WIDTH   2304
#define VIDEO_INPUT_HEIGHT  1296

/* channel 0 configuration */
#define CH0_WIDTH           2304
#define CH0_HEIGHT          1296
#define CH0_BIT_RATE        (2048 * 1024)
#define CH0_FRAME_COUNT     15
#define CH0_FRAME_TIME      1

/* channel 1 configuration */
#define CH1_WIDTH           720
#define CH1_HEIGHT          576
#define CH1_BIT_RATE        (1024 * 1024)
#define CH1_FRAME_COUNT     25
#define CH1_FRAME_TIME      1

/* channel 2 configuration */
#define CH2_WIDTH           352
#define CH2_HEIGHT          288
#define CH2_BIT_RATE        (512 * 1024)
#define CH2_FRAME_COUNT     25
#define CH2_FRAME_TIME      1

#define JPEG_INIT_WIDTH     (CH0_WIDTH)
#define JPEG_INIT_HEIGHT    (CH0_HEIGHT)

#endif
