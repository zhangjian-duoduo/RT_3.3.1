/**
 * @file uvc_crosshair.c
 * @brief UVC Crosshair Implementation
 * @note Uses GBOX to draw crosshair (two rectangles: horizontal and vertical lines)
 */
#include <stdio.h>
#include <string.h>
#include "uvc_crosshair_config.h"
#include "advapi/FHAdv_OSD_mpi.h"

#if UVC_ENABLE_CROSSHAIR

#define CROSSHAIR_VPU_ID       0  /* VPU ID */
#define CROSSHAIR_CHN          0  /* Channel ID */
#define CROSSHAIR_H_LINE_ID    0  /* Horizontal line GBOX ID */
#define CROSSHAIR_V_LINE_ID    1  /* Vertical line GBOX ID */

static FH_BOOL g_crosshair_init = FH_FALSE;
static FH_SINT32 g_crosshair_width = 0;
static FH_SINT32 g_crosshair_height = 0;

/**
 * @brief Initialize crosshair feature
 * @return 0 on success, error code on failure
 */
FH_SINT32 uvc_crosshair_init(FH_VOID)
{
    FH_SINT32 ret;

    if (g_crosshair_init)
    {
        return 0;
    }

    /* Initialize GBOX pixel config */
    ret = FHAdv_Osd_Config_GboxPixel(CROSSHAIR_VPU_ID, 2, FHTEN_OSD_GBOX_Pixel);
    if (ret != FH_SUCCESS)
    {
        printf("FHAdv_Osd_Config_GboxPixel failed! ret=0x%x\n", ret);
        return ret;
    }

    g_crosshair_init = FH_TRUE;
    printf("Crosshair init done\n");

    return 0;
}

/**
 * @brief Update crosshair position based on resolution
 * @param width frame width
 * @param height frame height
 * @return 0 on success
 */
FH_SINT32 uvc_crosshair_update(FH_SINT32 width, FH_SINT32 height)
{
    FH_SINT32 ret;
    FHT_OSD_Gbox_t gbox_h, gbox_v;
    FH_SINT32 center_x, center_y;
    FH_SINT32 line_width = UVC_CROSSHAIR_LINE_WIDTH;

    if (!g_crosshair_init)
    {
        printf("Crosshair not initialized!\n");
        return -1;
    }

    /* Same resolution, no need to update */
    if (g_crosshair_width == width && g_crosshair_height == height)
    {
        return 0;
    }

    center_x = width / 2;
    center_y = height / 2;

    /* Horizontal line - from left edge to right edge, centered vertically */
    memset(&gbox_h, 0, sizeof(gbox_h));
    gbox_h.enable = 1;
    gbox_h.gboxId = CROSSHAIR_H_LINE_ID;
    gbox_h.rotate = 0;
    gbox_h.gboxLineWidth = line_width;
    gbox_h.area.fTopLeftX = 0;
    gbox_h.area.fTopLeftY = center_y - line_width / 2;
    gbox_h.area.fWidth = width;
    gbox_h.area.fHeigh = line_width;
    gbox_h.osdColor.fRed = UVC_CROSSHAIR_COLOR_R;
    gbox_h.osdColor.fGreen = UVC_CROSSHAIR_COLOR_G;
    gbox_h.osdColor.fBlue = UVC_CROSSHAIR_COLOR_B;
    gbox_h.osdColor.fAlpha = UVC_CROSSHAIR_COLOR_A;
    gbox_h.max_frame_w = width;
    gbox_h.max_frame_h = height;

    /* Vertical line - from top edge to bottom edge, centered horizontally */
    memset(&gbox_v, 0, sizeof(gbox_v));
    gbox_v.enable = 1;
    gbox_v.gboxId = CROSSHAIR_V_LINE_ID;
    gbox_v.rotate = 0;
    gbox_v.gboxLineWidth = line_width;
    gbox_v.area.fTopLeftX = center_x - line_width / 2;
    gbox_v.area.fTopLeftY = 0;
    gbox_v.area.fWidth = line_width;
    gbox_v.area.fHeigh = height;
    gbox_v.osdColor.fRed = UVC_CROSSHAIR_COLOR_R;
    gbox_v.osdColor.fGreen = UVC_CROSSHAIR_COLOR_G;
    gbox_v.osdColor.fBlue = UVC_CROSSHAIR_COLOR_B;
    gbox_v.osdColor.fAlpha = UVC_CROSSHAIR_COLOR_A;
    gbox_v.max_frame_w = width;
    gbox_v.max_frame_h = height;

    /* Draw horizontal line */
    ret = FHAdv_Osd_Ex_SetGbox(CROSSHAIR_VPU_ID, CROSSHAIR_CHN, &gbox_h);
    if (ret != FH_SUCCESS)
    {
        printf("FHAdv_Osd_Ex_SetGbox (h) failed! ret=0x%x\n", ret);
        return ret;
    }

    /* Draw vertical line */
    ret = FHAdv_Osd_Ex_SetGbox(CROSSHAIR_VPU_ID, CROSSHAIR_CHN, &gbox_v);
    if (ret != FH_SUCCESS)
    {
        printf("FHAdv_Osd_Ex_SetGbox (v) failed! ret=0x%x\n", ret);
        return ret;
    }

    g_crosshair_width = width;
    g_crosshair_height = height;
    printf("Crosshair updated: %dx%d\n", width, height);

    return 0;
}

/**
 * @brief Disable crosshair
 * @return 0 on success
 */
FH_SINT32 uvc_crosshair_disable(FH_VOID)
{
    FH_SINT32 ret;
    FHT_OSD_Gbox_t gbox_cfg;

    if (!g_crosshair_init)
    {
        return 0;
    }

    /* Disable horizontal line */
    memset(&gbox_cfg, 0, sizeof(gbox_cfg));
    gbox_cfg.enable = 0;
    gbox_cfg.gboxId = CROSSHAIR_H_LINE_ID;
    gbox_cfg.gboxLineWidth = 2;
    gbox_cfg.area.fTopLeftX = 0;
    gbox_cfg.area.fTopLeftY = 0;
    gbox_cfg.area.fWidth = 16;
    gbox_cfg.area.fHeigh = 16;
    gbox_cfg.osdColor.fAlpha = 255;
    gbox_cfg.osdColor.fRed = 0;
    gbox_cfg.osdColor.fGreen = 0;
    gbox_cfg.osdColor.fBlue = 0;

    ret = FHAdv_Osd_Ex_SetGbox(CROSSHAIR_VPU_ID, CROSSHAIR_CHN, &gbox_cfg);
    if (ret != FH_SUCCESS)
    {
        printf("FHAdv_Osd_Ex_SetGbox disable (h) failed! ret=0x%x\n", ret);
    }

    /* Disable vertical line */
    gbox_cfg.gboxId = CROSSHAIR_V_LINE_ID;
    ret = FHAdv_Osd_Ex_SetGbox(CROSSHAIR_VPU_ID, CROSSHAIR_CHN, &gbox_cfg);
    if (ret != FH_SUCCESS)
    {
        printf("FHAdv_Osd_Ex_SetGbox disable (v) failed! ret=0x%x\n", ret);
    }

    g_crosshair_width = 0;
    g_crosshair_height = 0;

    return 0;
}

#endif /* UVC_ENABLE_CROSSHAIR */
