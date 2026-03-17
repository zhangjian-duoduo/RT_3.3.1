/**
 * @file uvc_crosshair.h
 * @brief UVC Crosshair Header
 */
#ifndef __UVC_CROSSHAIR_H__
#define __UVC_CROSSHAIR_H__

#include "types/type_def.h"

#if UVC_ENABLE_CROSSHAIR

/**
 * @brief Initialize crosshair feature
 * @return 0 on success
 */
FH_SINT32 uvc_crosshair_init(FH_VOID);

/**
 * @brief Update crosshair position based on resolution
 * @param width frame width
 * @param height frame height
 * @return 0 on success
 */
FH_SINT32 uvc_crosshair_update(FH_SINT32 width, FH_SINT32 height);

/**
 * @brief Disable crosshair
 * @return 0 on success
 */
FH_SINT32 uvc_crosshair_disable(FH_VOID);

#else

/* Dummy functions when feature is disabled */
#define uvc_crosshair_init()     (0)
#define uvc_crosshair_update(w, h)   (0)
#define uvc_crosshair_disable()   (0)

#endif /* UVC_ENABLE_CROSSHAIR */

#endif /* __UVC_CROSSHAIR_H__ */
