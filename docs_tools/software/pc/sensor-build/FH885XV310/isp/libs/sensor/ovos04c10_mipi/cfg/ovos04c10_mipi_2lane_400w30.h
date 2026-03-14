#ifndef __OVOS04C10_MIPI_2LANE_400W30_H__
#define __OVOS04C10_MIPI_2LANE_400W30_H__

#define FRAM_H_400WP30_2LANE              1640
#define FRAM_W_400WP30_2LANE              2054
#define ACTIVE_H_400WP30_2LANE            1520
#define ACTIVE_W_400WP30_2LANE            2688
#define OFFSET_X_400WP30_2LANE            0
#define OFFSET_Y_400WP30_2LANE            0
#define PIC_IN_H_400WP30_2LANE            1440
#define PIC_IN_W_400WP30_2LANE            2560

static ISP_VI_ATTR_S attr_2l_400w30 = {
    .u16WndHeight   =   FRAM_H_400WP30_2LANE,
    .u16WndWidth    =   FRAM_W_400WP30_2LANE,
    .u16InputHeight =   ACTIVE_H_400WP30_2LANE,
    .u16InputWidth  =   ACTIVE_W_400WP30_2LANE,
    .u16OffsetX     =   OFFSET_X_400WP30_2LANE,
    .u16OffsetY     =   OFFSET_Y_400WP30_2LANE,
    .u16PicHeight   =   PIC_IN_H_400WP30_2LANE,
    .u16PicWidth    =   PIC_IN_W_400WP30_2LANE,
    .u16FrameRate   =   30,
    .enBayerType    =   BAYER_BGGR
};

#endif // __OVOS04C10_MIPI_2LANE_400W30_H__
