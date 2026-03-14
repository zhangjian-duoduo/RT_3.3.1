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
    "ETH", "I2C0", "MIPI", "PWM2", "PWM3", "PWM4", "PWM5",
    "PWM6", "PWM7", "PWM8", "PWM9", "SADC_XAIN0",
    "SADC_XAIN1", "SD0_NO_WP", "SENSOR_CLK", "SSI0_4BIT",
    "UART0", "UART1", "GPIO4", "GPIO13", "GPIO30", "GPIO31",
    "GPIO32", "GPIO43", "GPIO44", "GPIO47"
};

#endif
