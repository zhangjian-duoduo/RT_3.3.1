#ifndef __OVOS05_MIPI_4LANE_1080P20_H__
#define __OVOS05_MIPI_4LANE_1080P20_H__

#define FRAM_W_1080P20              3700
#define FRAM_H_1080P20              2700
#define ACTIVE_W_1080P20            2560
#define ACTIVE_H_1080P20            1944
#define PIC_IN_W_1080P20            1920
#define PIC_IN_H_1080P20            1080
#define OFFSET_X_1080P20            0
#define OFFSET_Y_1080P20            0

static ISP_VI_ATTR_S attr_4l_1080p20 = {
    .u16WndHeight   = FRAM_H_1080P20,
    .u16WndWidth    = FRAM_W_1080P20,
    .u16InputHeight = ACTIVE_H_1080P20,
    .u16InputWidth  = ACTIVE_W_1080P20,
    .u16OffsetX     = OFFSET_X_1080P20,
    .u16OffsetY     = OFFSET_Y_1080P20,
    .u16PicHeight   = PIC_IN_H_1080P20,
    .u16PicWidth    = PIC_IN_W_1080P20,
    .u16FrameRate   = 20,
    .enBayerType    = BAYER_BGGR
};

#endif // __OVOS05_MIPI_4LANE_1080P20_H__
