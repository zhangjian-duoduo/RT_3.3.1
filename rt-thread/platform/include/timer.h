/**
 * Copyright (c) 2015-2019 Shanghai Fullhan Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-04-12                  add license Apache-2.0
 */


#ifndef __TIMER_H__
#define __TIMER_H__

#include <rtdef.h>
#include <stdint.h>
#include <fh_arch.h>

static inline unsigned int timern_base(int n)
{
    unsigned int base = 0;
    switch (n)
    {
    case 0:
    default:
        base = TMR_REG_BASE;
        break;
    case 1:
        base = TMR_REG_BASE+0x14;
        break;
    case 2:
        base = TMR_REG_BASE+0x28;
        break;
    case 3:
        base = TMR_REG_BASE+0x3c;
        break;
    }
    return base;
}


#define TIMERN_REG_BASE(n)          (timern_base(n))

#define REG_TIMER_LOADCNT(n)        (timern_base(n) + 0x00)
#define REG_TIMER_CUR_VAL(n)        (timern_base(n) + 0x04)
#define REG_TIMER_CTRL_REG(n)       (timern_base(n) + 0x08)
#define REG_TIMER_EOI_REG(n)        (timern_base(n) + 0x0C)
#define REG_TIMER_INTSTATUS(n)      (timern_base(n) + 0x10)

#define REG_TIMERS_INTSTATUS    (TMR_REG_BASE + 0xa0)

#define TIMER_INTSTS_TIMER(n)     (0x1<<(n))



struct fh_timer
{
    struct rt_object parent; /**< inherit from rt_object */

    rt_list_t list;

    void (*timeout_func)(void *parameter); /**< timeout function */
    void *parameter;                       /**< timeout function's parameter */

    rt_uint32_t init_cycle;   /**< timer inited cycle */
    rt_uint32_t timeout_tick; /**< timeout cycle*/
};

uint64_t read_pts(void);
void rt_hw_timer_init(void);
void clocksource_pts_register(void);
void clockevent_timer0_register(void);
void clocksource_timer1_register(void);
void timer_waitfor_disable(int id);
#endif
