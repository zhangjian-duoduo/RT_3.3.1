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


#include <rtdevice.h>
#include "fh_arch.h"
#include "fh_timer.h"
#ifdef RT_USING_TIMEKEEPING
#include <timekeeping.h>
#include <hrtimer.h>
#endif

#include "fh_chip.h"
#include "fh_clock.h"
#include "board.h"
#include "delay.h"
#include "timer.h"
#include "fh_pmu.h"


#define PAE_PTS_CLK (1000000)
typedef uint64_t cycle_t;



#ifdef RT_USING_TIMEKEEPING
static cycle_t fh_pts_read(struct clocksource *cs)
{
#else
static cycle_t fh_pts_read(void *cs)
{
#endif
    return fh_get_pts64();
}
uint64_t read_pts(void) {return fh_pts_read(NULL);}

#ifdef RT_USING_TIMEKEEPING
struct clocksource clocksource_pts = {
    .name = "pts", .rating = 299, .read = fh_pts_read, .mask = 0xffffffff,
};

void clocksource_pts_register(void)
{
    struct clk *ptsclk = clk_get(NULL, "pts_clk");

    if (ptsclk != RT_NULL)
        clk_enable(ptsclk);

    fh_pmu_set_user();
    clocksource_register_hz(&clocksource_pts, PAE_PTS_CLK);
}

static cycle_t fh_timer1_read(struct clocksource *cs)
{
    return ~GET_REG(REG_TIMER_CUR_VAL(1));
}

struct clocksource clocksource_timer1 = {
    .name = "timer1", .rating = 300, .read = fh_timer1_read, .mask = 0xffffffff,
};

void timer_waitfor_disable(int id)
{
    int sync_cnt = 0;

    /* zy/ticket/100 : update apb Timer LOADCNT */
    /* CURRENTVALE could,t be start from new LOADCOUNT */
    /* cause is timer clk 1M hz and apb is 150M hz */
    /* check current cnt for it is disabled */
    while (GET_REG(REG_TIMER_CUR_VAL(id)) != 0)
    {
        sync_cnt++;
        if (sync_cnt >= 50)
        {
            /* typical cnt is 5 when in 1M timer clk */
            /* so here use 50 to check whether it is err */
            rt_kprintf("timer %x problem,can't disable\n",id);
        }
    }


}


void clocksource_timer1_register(void)
{
    SET_REG(REG_TIMER_CTRL_REG(1), 0x00);
    timer_waitfor_disable(1);
    SET_REG(REG_TIMER_CTRL_REG(1), 0x5);
    clocksource_register_hz(&clocksource_timer1, TIMER_CLOCK);
}



static int fh_set_next_event(unsigned long cycles,
                             struct clock_event_device *dev)
{

    SET_REG_M(REG_TIMER_CTRL_REG(0), 0x00, 0x1);
    timer_waitfor_disable(0);
    SET_REG(REG_TIMER_LOADCNT(0), cycles);
    SET_REG_M(REG_TIMER_CTRL_REG(0), 0x01, 0x1);

    return 0;
}

static void fh_set_mode(enum clock_event_mode mode,
                        struct clock_event_device *dev)
{

    switch (mode)
    {
    case CLOCK_EVT_MODE_PERIODIC:
    case CLOCK_EVT_MODE_ONESHOT:
        SET_REG(REG_TIMER_CTRL_REG(0), 0);
        timer_waitfor_disable(0);
        SET_REG(REG_TIMER_LOADCNT(0), TIMER_CLOCK / RT_TICK_PER_SECOND);
        SET_REG(REG_TIMER_CTRL_REG(0), 0x3);
        /* pmu reset */
        reset_timer();
        break;        break;
    case CLOCK_EVT_MODE_UNUSED:
    case CLOCK_EVT_MODE_SHUTDOWN:
        SET_REG(REG_TIMER_CTRL_REG(0), 0x0);
        timer_waitfor_disable(0);
        break;
    case CLOCK_EVT_MODE_RESUME:
        SET_REG(REG_TIMER_CTRL_REG(0), 0x3);
        break;
    }
}

static struct clock_event_device clockevent_timer0 = {
    .name           = "fh_clockevent",
    .features       = CLOCK_EVT_FEAT_PERIODIC | CLOCK_EVT_FEAT_ONESHOT,
    .shift          = 32,
    .set_next_event = fh_set_next_event,
    .set_mode       = fh_set_mode,
};

void clockevent_handler(int vector, void *param)
{
    unsigned int int_status;

    int_status = GET_REG(REG_TIMERS_INTSTATUS);

    if (int_status & TIMER_INTSTS_TIMER(0))
    {
        clockevent_timer0.event_handler(&clockevent_timer0);
        GET_REG(REG_TIMER_EOI_REG(0));
    }
}

void clockevent_timer0_register(void)
{
    clockevent_timer0.mult =
        div_sc(TIMER_CLOCK, NSEC_PER_SEC, clockevent_timer0.shift);
    clockevent_timer0.max_delta_ns =10000000;
        /* clockevent_delta2ns(0xffffffff, &clockevent_timer0); */
    clockevent_timer0.min_delta_ns =10000;
       /* clockevent_delta2ns(0xf, &clockevent_timer0);*/
    clockevents_register_device(&clockevent_timer0);

    rt_hw_interrupt_install(TMR0_IRQn, clockevent_handler,
                            (void *)&clockevent_timer0, "sys_tick");


}
#endif

#if 0
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
void rt_hw_timer_init(void)
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
#endif
