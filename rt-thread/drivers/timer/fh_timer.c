/**
 * Copyright (c) 2015-2019 Shanghai Fullhan Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-04-12               the first version
 *
 */

/*****************************************************************************
 *  Include Section
 *  add all #include here
 *****************************************************************************/
// #include "inc/fh_driverlib.h"
#include <rtthread.h>
#include "fh_timer.h"
#include "fh_arch.h"
#include "board.h"
#include "timer.h"
#include "fh_clock.h"
#include <rthw.h>

/*****************************************************************************
 * Define section
 * add all #define here
 *****************************************************************************/

/****************************************************************************
 * ADT section
 *  add definition of user defined Data Type that only be used in this file here
 ***************************************************************************/

/******************************************************************************
 * Function prototype section
 * add prototypes for all functions called by this file,execepting those
 * declared in header file
 *****************************************************************************/

/*****************************************************************************
 * Global variables section - Exported
 * add declaration of global variables that will be exported here
 * e.g.
 *  int8_t foo;
 ****************************************************************************/

/*****************************************************************************
 * Global variables section - Local
 * define global variables(will be refered only in this file) here,
 * static keyword should be used to limit scope of local variable to this file
 * e.g.
 *  static uint8_t ufoo;
 *****************************************************************************/

/* function body */

/*****************************************************************************
 * Description:
 *      add funtion description here
 * Parameters:
 *      description for each argument, new argument starts at new line
 * Return:
 *      what does this function returned?
 *****************************************************************************/
int timer_init(timer *tim)
{
    tim->TIMER_CTRL_REG = 0;
    return 0;
}

int timer_set_mode(timer *tim, enum timer_mode mode)
{
    switch (mode)
    {
    case TIMER_MODE_PERIODIC:
    case TIMER_MODE_ONESHOT:
        tim->TIMER_CTRL_REG |= TIMER_CTRL_MODE;
        break;
    default:
        rt_kprintf("Not support TIMER mode\n");
        return -1;
        break;
    }

    return 0;
}

void timer_set_period(timer *tim, UINT32 period, UINT32 clock)
{
    tim->TIMER_LOAD_COUNT = clock / period;
}

void timer_enable(timer *tim)
{
    tim->TIMER_CTRL_REG |= TIMER_CTRL_ENABLE;
    /*解决timer跨时钟域问题*/
}
void timer_disable(timer *tim) { tim->TIMER_CTRL_REG &= ~TIMER_CTRL_ENABLE; }
void timer_enable_irq(timer *tim)
{
    tim->TIMER_CTRL_REG &= ~TIMER_CTRL_INTMASK;
}

void timer_disable_irq(timer *tim)
{
    tim->TIMER_CTRL_REG |= TIMER_CTRL_INTMASK;
}

UINT32 timer_get_status(timer *tim) { return tim->TIMER_INT_STATUS; }
UINT32 timer_get_eoi(timer *tim) { return tim->TIMER_EOI; }
UINT32 timer_get_value(timer *tim)
{
    return tim->TIMER_LOAD_COUNT - tim->TIMER_CURRENT_VALUE;
}

void rt_timer_handler(int vector, void *param)
{
    timer *tim = param;
    unsigned int int_status;
    int_status = GET_REG(REG_TIMERS_INTSTATUS);
    if (int_status & TIMER_INTSTS_TIMER(0))
    {
        timer_get_eoi(tim);
        rt_tick_increase();
    }

}

/**
 * This function will init pit for system ticks
 */
void rt_hw_dw_timer_init(void)
{
    timer *tim = (timer *)(TIMERN_REG_BASE(0));
    struct clk *tmrclk = clk_get(NULL, "tmr_clk");

    if (tmrclk != RT_NULL)
    {
        clk_set_rate(tmrclk, TIMER_CLOCK);
        clk_enable(tmrclk);
    }

    timer_init(tim);
    /* install interrupt handler */
    rt_hw_interrupt_install(TMR0_IRQn, rt_timer_handler, (void *)tim,
                            "sys_tick");
    rt_hw_interrupt_umask(TMR0_IRQn);
    timer_enable_irq(tim);
    timer_set_mode(tim, TIMER_MODE_PERIODIC);
    timer_set_period(tim, RT_TICK_PER_SECOND, TIMER_CLOCK);
    /*timer_enable_irq(tim);*/
    timer_enable(tim);
}
