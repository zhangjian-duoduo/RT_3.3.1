/**
 * Copyright (c) 2015-2019 Shanghai Fullhan Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-01-08     wangyl307    the first version, for fh8856&zy2
 * 2019-01-14     wangyl307    add pads fix funcs and defs for zy
 *
 */

#ifndef __APP_IOPAD_H__
#define __APP_IOPAD_H__

char *fh_pinctrl_selected_devices[] = {
    "I2C0", "MIPI", "SADC_XAIN0", "SADC_XAIN1",
    "SENSOR_CLK", "SSI0_4BIT", "STM0", "STM1",
    "UART0", "UART1", "USB", "GPIO4", "GPIO13", "GPIO30",
    "GPIO31", "GPIO32", "GPIO43", "GPIO44",

#ifdef WIFI_USING_SDIOWIFI
#if (WIFI_SDIO == 0)
    "SD0_WIFI", "GPIO17", "GPIO28", "GPIO29"
#elif (WIFI_SDIO == 1)
#error "fh8626v200 fast_board hardware not support sd1 wifi"
#endif
#else
    "SD0_NO_WP", "ETH"
#endif
};

#endif
