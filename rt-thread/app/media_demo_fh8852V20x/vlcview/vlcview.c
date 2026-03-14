/*
 * File      : sample_vlcview.c
 * This file is part of SOCs BSP for RT-Thread distribution.
 *
 * Copyright (c) 2017 Shanghai Fullhan Microelectronics Co., Ltd.
 * All rights reserved
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *  Visit http://www.fullhan.com to get contact with Fullhan.
 *
 * Change Logs:
 * Date           Author       Notes
 */
#include <sample_common.h>
#include "vlcview.h"

static FH_SINT32 g_sample_running = 0;

FH_SINT32 _vlcview_exit(FH_VOID)
{
    if (!g_sample_running)
    {
        printf("vlcview is not running!\n");
        return 0;
    }
    printf("=====vlc start exit=====\n");

#ifdef FH_APP_OPEN_IVS
    sample_ivs_stop();
    printf("=====sample_ivs_stop=====\n");
#endif

#ifdef FH_APP_OPEN_AF
    sample_af_stop();
    printf("=====sample_af_stop=====\n");
#endif

#ifdef FH_APP_OPEN_VENC
    sample_venc_stop();
    printf("=====sample_venc_stop=====\n");
#endif

#if defined(FH_APP_OPEN_MOTION_DETECT) ||      \
    defined(FH_APP_OPEN_RECT_MOTION_DETECT) || \
    defined(FH_APP_OPEN_COVER_DETECT)
    sample_md_cd_stop();
    printf("=====sample_md_cd_stop=====\n");
#endif

#if (defined(NN_ENABLE_G0) || defined(NN_ENABLE_G1)) && defined(FH_NNA_APP_SELECT)
    sample_fh_nn_obj_detect_stop();
    printf("=====sample_fh_nn_obj_detect_stop=====\n");
#endif

#ifdef FH_APP_OPEN_OVERLAY
    sample_overlay_stop();
    printf("=====sample_overlay_stop=====\n");
#endif

#ifdef FH_APP_USING_COOLVIEW
    sample_common_stop_coolview(0);
    printf("=====sample_common_stop_coolview=====\n");
#endif

#ifdef FH_APP_OPEN_ISP_STRATEGY_DEMO
    sample_isp_stop();
    printf("=====sample_isp_stop=====\n");
#endif
    sample_common_stop_get_stream();
    printf("=====sample_common_stop_get_stream=====\n");
    sample_common_bgm_exit();
    printf("=====sample_common_bgm_exit=====\n");
    sample_common_dmc_deinit();
    printf("=====sample_common_dmc_deinit=====\n");
    sample_common_isp_stop();
    printf("=====sample_common_isp_stop=====\n");
    sample_common_media_sys_exit();
    printf("=====sample_common_media_sys_exit=====\n");

    g_sample_running = 0;

    return 0;
}

FH_SINT32 _vlcview(FH_CHAR *dst_ip, FH_UINT32 port)
{
    FH_SINT32 ret;

    if (g_sample_running)
    {
        printf("vlcview is running!\n");
        return 0;
    }

    g_sample_running = 1;
    /******************************************
      step  1: init media platform
     ******************************************/
    ret = sample_common_media_sys_init();
    if (ret != 0)
    {
        printf("Error(%d - %x): sample_common_media_sys_init!\n", ret, ret);
        goto err_exit;
    }
    /******************************************
      step  2: init vpu vi and open vpu
     ******************************************/
    ret = sample_common_start_vpu();
    if (ret != 0)
    {
        printf("Error(%d - %x): sample_common_start_vpu!\n", ret, ret);
        goto err_exit;
    }
    /******************************************
      step  3: init enc
     ******************************************/
    ret = sample_common_start_enc();
    if (ret != 0)
    {
        printf("Error(%d - %x): sample_common_start_enc!\n", ret, ret);
        goto err_exit;
    }
    /************************************************
     step  4: if use smart encode type, open bgm
    ************************************************/
    ret = sample_common_bgm_init();
    if (ret != 0)
    {
        printf("Error(%d - %x): sample_common_bgm_init!\n", ret, ret);
        goto err_exit;
    }
    /************************************************
        step  8: start isp
    ************************************************/
    ret = sample_common_start_isp();
    if (ret)
    {
        printf("Error(%d - %x): sample_common_start_isp!\n", ret, ret);
        goto err_exit;
    }
    /******************************************
      step  9: start init mjpeg
    ******************************************/
    ret = sample_common_start_mjpeg();
    if (ret != 0)
    {
        printf("Error(%d - %x): sample_common_start_mjpeg!\n", ret, ret);
        goto err_exit;
    }
    /************************************************
        step  10: start bind
    ************************************************/
    ret = sample_common_start_bind();
    if (ret)
    {
        goto err_exit;
    }
    /******************************************
      step  11: isp send stream
     ******************************************/
    sample_common_isp_run();
    /************************************************
      step  12: init data-manage-center which is used to dispatch stream...
     ************************************************/
    sample_common_dmc_init(dst_ip, port);
    /******************************************
      step  13: start get stream
     ******************************************/
    sample_common_start_get_stream();
#ifdef FH_APP_USING_COOLVIEW
    /******************************************
      step  16: open coolview
     ******************************************/
    sample_common_start_coolview(NULL);
#endif
    /******************************************
      step  17: do nn human detect
     ******************************************/
#if (defined(NN_ENABLE_G0) || defined(NN_ENABLE_G1)) && defined(FH_NNA_APP_SELECT)
    sample_fh_nn_obj_detect_start();
#endif

#ifdef FH_APP_OPEN_IVS
    sample_ivs_start();
#endif

#ifdef FH_APP_OPEN_OVERLAY
    /******************************************
      step  14: add overlayer
     ******************************************/
    sample_overlay_start();
#endif

#if defined(FH_APP_OPEN_MOTION_DETECT) ||      \
    defined(FH_APP_OPEN_RECT_MOTION_DETECT) || \
    defined(FH_APP_OPEN_COVER_DETECT)

    /******************************************
      step  17: do motion detect and cover detect
     ******************************************/
    sample_md_cd_start();
#endif

#ifdef FH_APP_OPEN_VENC
    /******************************************
      step  18: open venc functions
     ******************************************/
    sample_venc_start();
#endif

#ifdef FH_APP_OPEN_ISP_STRATEGY_DEMO
    /******************************************
      step  18: open isp strategy functions
     ******************************************/
    sample_isp_start();
#endif

#ifdef FH_APP_OPEN_AF
    sample_af_start();
#endif

    printf("[vlcview] start successful! ------build at(%s-%s)\n", __DATE__, __TIME__);

    return 0;

err_exit:
    _vlcview_exit();
    return -1;
}
