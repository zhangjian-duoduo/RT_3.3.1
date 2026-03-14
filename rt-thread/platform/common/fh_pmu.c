/**
 * Copyright (c) 2015-2019 Shanghai Fullhan Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-04-12     wangyl       add license Apache-2.0
 */

#include "rtthread.h"
#include "rtdebug.h"
#include "fh_chip.h"
#include "fh_pmu.h"
#include "fh_def.h"
#include <delay.h>
#include <linux/delay.h>
#include <rthw.h>

#ifndef INTERNAL_PHY
#define INTERNAL_PHY    0x55
#endif
#ifndef EXTERNAL_PHY
#define EXTERNAL_PHY    0xaa
#endif

#ifdef CONFIG_CHIP_FH8862
#define PMU_REG_BASE        (0)
#endif

#define FH_PMU_WRITEL(base, value) SET_REG(PMU_REG_BASE + base, value)
#define FH_PMU_WRITEL_MASK(base, value, mask) \
    SET_REG_M(PMU_REG_BASE + base, value, mask)
#define FH_PMU_READL(base) GET_REG(PMU_REG_BASE + base)


#if defined CONFIG_ARCH_FH885xV300 || defined CONFIG_ARCH_FH885xV310
static unsigned long level;

static inline void fh_set_scu_en(void)
{
    level = rt_hw_interrupt_disable();
    FH_PMU_WRITEL(REG_PMU_SCU_PLL_WREN, 0x706c6c63);
}

static inline void fh_set_scu_di(void)
{
    FH_PMU_WRITEL(REG_PMU_SCU_PLL_WREN, 0x0);
    rt_hw_interrupt_enable(level);
}
void fh_pmu_set_reg(unsigned int offset, unsigned int data)
{
    RT_ASSERT(offset < PMU_OFFSET_MAX);
    fh_set_scu_en();
    FH_PMU_WRITEL(offset, data);
    fh_set_scu_di();
    RT_ASSERT(offset < PMU_OFFSET_MAX);
}

unsigned int fh_pmu_get_reg(unsigned int offset)
{
    RT_ASSERT(offset < PMU_OFFSET_MAX);
    return FH_PMU_READL(offset);
}

void fh_pmu_set_reg_m(unsigned int offset, unsigned int data, unsigned int mask)
{
    RT_ASSERT(offset < PMU_OFFSET_MAX);
    fh_pmu_set_reg(offset, (fh_pmu_get_reg(offset) & (~(mask))) |
                      ((data) & (mask)));
}
#elif defined(CONFIG_ARCH_FH8862)
static unsigned long level;

static inline void fh_set_scu_en(void)
{
    level = rt_hw_local_irq_disable();
    FH_PMU_WRITEL(REG_PMU_SCU_PLL_WREN, 0x706c6c63);
    FH_PMU_WRITEL(REG_PMU_DDR_SCU_WREN, 0x706c6c63);
}

static inline void fh_set_scu_di(void)
{
    FH_PMU_WRITEL(REG_PMU_SCU_PLL_WREN, 0x0);
    FH_PMU_WRITEL(REG_PMU_DDR_SCU_WREN, 0x0);
    rt_hw_local_irq_enable(level);
}
void fh_pmu_set_reg(unsigned int base, unsigned int data)
{
    fh_set_scu_en();
    FH_PMU_WRITEL(base, data);
    fh_set_scu_di();
}

unsigned int fh_pmu_get_reg(unsigned int base)
{
    return FH_PMU_READL(base);
}
void fh_pmu_set_reg_m(unsigned int base, unsigned int data, unsigned int mask)
{
    fh_pmu_set_reg(base, (fh_pmu_get_reg(base) & (~(mask))) |
                      ((data) & (mask)));
}
#else
void fh_pmu_set_reg(unsigned int base, unsigned int data)
{
    FH_PMU_WRITEL(base, data);
}

unsigned int fh_pmu_get_reg(unsigned int base)
{
    return FH_PMU_READL(base);
}

void fh_pmu_set_reg_m(unsigned int base, unsigned int data, unsigned int mask)
{
    FH_PMU_WRITEL_MASK(base, data, mask);
}
#endif

void fh_get_chipid(unsigned int *plat_id, unsigned int *chip_id)
{
	unsigned int _plat_id = 0;

	_plat_id = fh_pmu_get_reg(REG_PMU_CHIP_ID);
	if (plat_id != NULL)
		*plat_id = _plat_id;

	if (chip_id != NULL)
		*chip_id = fh_pmu_get_reg(REG_PMU_IP_VER);
}

unsigned int fh_pmu_get_ptsl(void)
{
	fh_pmu_set_reg(REG_PMU_PTSLO, 0x01);
	return fh_pmu_get_reg(REG_PMU_PTSLO);
}

unsigned int fh_pmu_get_ptsh(void)
{
	fh_pmu_set_reg(REG_PMU_PTSLO, 0x01);
	return fh_pmu_get_reg(REG_PMU_PTSHI);
}

unsigned long long fh_get_pts64(void)
{
	unsigned int high, low;
	unsigned long long pts64;

	fh_pmu_set_reg(REG_PMU_PTSLO, 0x01);
	high = fh_pmu_get_reg(REG_PMU_PTSHI);
	low = fh_pmu_get_reg(REG_PMU_PTSLO);
	pts64 = (((unsigned long long)high)<<32)|((unsigned long long)low);
	return pts64;
}

void fh_pmu_wdt_pause(void)
{
	unsigned int reg;

	reg = fh_pmu_get_reg(REG_PMU_WDT_CTRL);
	reg |= 0x100;
	fh_pmu_set_reg(REG_PMU_WDT_CTRL, reg);
}

void fh_pmu_wdt_resume(void)
{
	unsigned int reg;

	reg = fh_pmu_get_reg(REG_PMU_WDT_CTRL);
	reg &= ~(0x100);
	fh_pmu_set_reg(REG_PMU_WDT_CTRL, reg);
}

void fh_pmu_usb_utmi_rst(void)
{
	unsigned int pmu_reg;

	pmu_reg = fh_pmu_get_reg(REG_PMU_SWRST_MAIN_CTRL);
	pmu_reg &= ~(USB_UTMI_RST_BIT);
	fh_pmu_set_reg(REG_PMU_SWRST_MAIN_CTRL, pmu_reg);
	pmu_reg = fh_pmu_get_reg(REG_PMU_SWRST_MAIN_CTRL);
	udelay(1000);
	pmu_reg |= USB_UTMI_RST_BIT;
	fh_pmu_set_reg(REG_PMU_SWRST_MAIN_CTRL, pmu_reg);
	pmu_reg = fh_pmu_get_reg(REG_PMU_SWRST_MAIN_CTRL);
	udelay(1000);
}

void fh_pmu_usb_phy_rst(void)
{
	unsigned int pmu_reg;

	pmu_reg = fh_pmu_get_reg(REG_PMU_USB_SYS);
	pmu_reg |= (USB_PHY_RST_BIT);
	fh_pmu_set_reg(REG_PMU_USB_SYS, pmu_reg);
	udelay(1000);
	pmu_reg = fh_pmu_get_reg(REG_PMU_USB_SYS);
	pmu_reg &= (~USB_PHY_RST_BIT);
	fh_pmu_set_reg(REG_PMU_USB_SYS, pmu_reg);
}

void fh_pmu_usb_pwr_on(void)
{
    unsigned int pmu_reg;

    pmu_reg = fh_pmu_get_reg(REG_PMU_USB_SYS1);
    pmu_reg &= (~USB_IDDQ_PWR_BIT);
    fh_pmu_set_reg(REG_PMU_USB_SYS1, pmu_reg);
    udelay(1000);
}

void fh_pmu_usb_resume(void)
{
	unsigned int pmu_reg;

	pmu_reg = fh_pmu_get_reg(REG_PMU_USB_SYS);
	pmu_reg |= (USB_SLEEP_MODE_BIT);
	fh_pmu_set_reg(REG_PMU_USB_SYS, pmu_reg);
	udelay(1000);
}

void fh_pmu_usb_tune(void)
{
    unsigned int pmu_reg = 0x76203344;

    fh_pmu_set_reg(REG_PMU_USB_TUNE, pmu_reg);
}

void _pmu_main_reset(unsigned int reg, unsigned int retry, unsigned int udelay)
{
	fh_pmu_set_reg(REG_PMU_SWRST_MAIN_CTRL, reg);

	while (fh_pmu_get_reg(REG_PMU_SWRST_MAIN_CTRL) != 0xffffffff) {
		if (retry-- <= 0)
			return;

		udelay(udelay);
	}
}

void fh_pmu_timer_reset(void)
{
    unsigned int reg = 0;
	reg = ~(1 << TMR_RSTN_BIT);
    _pmu_main_reset(reg, 1000, 1);
}

void _pmu_ahb_reset(unsigned int reg, unsigned int retry, unsigned int udelay)
{
	fh_pmu_set_reg(REG_PMU_SWRST_AHB_CTRL, reg);

	while (fh_pmu_get_reg(REG_PMU_SWRST_AHB_CTRL) != 0xffffffff) {
		if (retry-- <= 0)
			return;

		udelay(udelay);
	}
}

void _pmu_apb_reset(unsigned int reg, unsigned int retry, unsigned int udelay)
{
	fh_pmu_set_reg(REG_PMU_SWRST_APB_CTRL, reg);

	while (fh_pmu_get_reg(REG_PMU_SWRST_APB_CTRL) != 0xffffffff) {
		if (retry-- <= 0)
			return;

		udelay(udelay);
	}
}

void _pmu_nsr_reset(unsigned int reg, unsigned int reset_time)
{
	fh_pmu_set_reg(REG_PMU_SWRSTN_NSR, reg);
	udelay(reset_time);
	fh_pmu_set_reg(REG_PMU_SWRSTN_NSR, 0xFFFFFFFF);
}

void fh_pmu_sdc_reset(int slot_id)
{
#ifndef CONFIG_ARCH_FH8862
	unsigned int reg = 0;

	if (slot_id == 1)
		reg = ~(1 << SDC1_HRSTN_BIT);
	else if (slot_id == 0)
		reg = ~(1 << SDC0_HRSTN_BIT);
	else
		rt_kprintf("%s slot id error:%d\n",__func__, slot_id);

	_pmu_ahb_reset(reg, 1000, 1);
#endif
}

void fh_pmu_enc_reset(void)
{
#if !defined(CONFIG_ARCH_FH8626V100) && !defined(CONFIG_ARCH_FH8636_FH8852V20X)
	_pmu_ahb_reset(~(1 << VCU_HRSTN_BIT), 100, 10);
#endif
}

#if !defined(CONFIG_ARCH_FH8636_FH8852V20X)
void fh_pmu_dwi2s_set_clk(unsigned int div_i2s, unsigned int div_mclk)
{
	unsigned int reg;

	reg = fh_pmu_get_reg(PMU_DWI2S_CLK_DIV_REG);
#ifdef CONFIG_ARCH_FH8862
    reg &= ~(0xfff << PMU_DWI2S_CLK_DIV_SHIFT);
    reg |= ((div_i2s-1) << 6 | (div_mclk-1)) << PMU_DWI2S_CLK_DIV_SHIFT;
#else
	reg &= ~(0xffff << PMU_DWI2S_CLK_DIV_SHIFT);
	reg |= ((div_i2s-1) << 8 | (div_mclk-1)) << PMU_DWI2S_CLK_DIV_SHIFT;
#endif
	fh_pmu_set_reg(PMU_DWI2S_CLK_DIV_REG, reg);

	/* i2s_clk switch to PLLVCO */
	reg = fh_pmu_get_reg(PMU_DWI2S_CLK_SEL_REG);
	reg &= ~(1 << PMU_DWI2S_CLK_SEL_SHIFT);
	reg |= 1 << PMU_DWI2S_CLK_SEL_SHIFT;
	fh_pmu_set_reg(PMU_DWI2S_CLK_SEL_REG, reg);
}
#endif

void fh_pmu_eth_set_speed(unsigned int speed)
{
    unsigned int reg;

    reg = fh_pmu_get_reg(PMU_RMII_SPEED_MODE);
    if (speed == 10)
        reg &= ~(FH_GMAC_SPEED_100M);
    else if (speed == 100)
        reg |= FH_GMAC_SPEED_100M;
    else
        rt_kprintf("ERROR: wrong param for emac set speed, %d\n",
        speed);

    fh_pmu_set_reg(PMU_RMII_SPEED_MODE, reg);
}

void fh_pmu_eth_reset(void)
{
    _pmu_ahb_reset(~(1 << EMAC_HRSTN_BIT), 1000, 1);
}

void fh_pmu_restart(void)
{
	fh_pmu_set_reg(REG_PMU_SWRST_MAIN_CTRL, 0x7fffffff);
}

unsigned int fh_pmu_get_tsensor_init_data(void)
{
#ifdef REG_PMU_RTC_PARAM
    return fh_pmu_get_reg(REG_PMU_RTC_PARAM);
#else
    return 0;
#endif
}
#if defined CONFIG_ARCH_FH8636_FH8852V20X || defined CONFIG_ARCH_FH885xV300 || defined CONFIG_ARCH_FH885xV310
/* add gmac plat */
void fh_pmu_ephy_set_reg(unsigned int offset, unsigned int data)
{
    SET_REG(REG_EPHY_BASE + offset, data);
}

void fh_pmu_ephy_set_m_reg(unsigned int offset,
unsigned int data, unsigned int mask)
{
    unsigned int ret;

    ret = GET_REG(REG_EPHY_BASE + offset);
    ret &= ~(mask);
    ret |= (data & mask);
    SET_REG(REG_EPHY_BASE + offset, ret);
}

struct s_train_val
{
    char *name;
    unsigned int src_add;
    unsigned int src_mask;
    unsigned int src_valid_index;
    unsigned char *dst_base_name;
    unsigned int dst_add;
    unsigned int dst_valid_index;
    unsigned int *bind_train_array;
    unsigned int bind_train_size;
    int usr_train_offset;
};

enum
{
    CP_VAL_0,
    CP_VAL_1,
    VAL_100M_0,
    VAL_100M_1,
    VAL_10M_0,
    VAL_10M_1,
    TRAIN_MAX_SIZE,
};


#define BITS_PER_LONG 32
#define BITMAP_LAST_WORD_MASK(nbits) (~0UL >> (-(nbits) & (BITS_PER_LONG - 1)))
/**
 * __ffs - find first bit in word.
 * @word: The word to search
 *
 * Undefined if no bit exists, so code should check against 0 first.
 */
static  unsigned long __ffs(unsigned long word)
{
    int num = 0;

#if BITS_PER_LONG == 64
    if ((word & 0xffffffff) == 0)
    {
        num += 32;
        word >>= 32;
    }
#endif
    if ((word & 0xffff) == 0)
    {
        num += 16;
        word >>= 16;
    }
    if ((word & 0xff) == 0)
    {
        num += 8;
        word >>= 8;
    }
    if ((word & 0xf) == 0)
    {
        num += 4;
        word >>= 4;
    }
    if ((word & 0x3) == 0)
    {
        num += 2;
        word >>= 2;
    }
    if ((word & 0x1) == 0)
        num += 1;
    return num;
}

/**
 * __fls - find last (most-significant) set bit in a long word
 * @word: the word to search
 *
 * Undefined if no set bit exists, so code should check against 0 first.
 */
static  unsigned long __fls(unsigned long word)
{
    int num = BITS_PER_LONG - 1;

#if BITS_PER_LONG == 64
    if (!(word & (~0ul << 32)))
    {
        num -= 32;
        word <<= 32;
    }
#endif
    if (!(word & (~0ul << (BITS_PER_LONG-16))))
    {
        num -= 16;
        word <<= 16;
    }
    if (!(word & (~0ul << (BITS_PER_LONG-8))))
    {
        num -= 8;
        word <<= 8;
    }
    if (!(word & (~0ul << (BITS_PER_LONG-4))))
    {
        num -= 4;
        word <<= 4;
    }
    if (!(word & (~0ul << (BITS_PER_LONG-2))))
    {
        num -= 2;
        word <<= 2;
    }
    if (!(word & (~0ul << (BITS_PER_LONG-1))))
        num -= 1;
    return num;
}
/*
 * Find the first set bit in a memory region.
 */
static inline unsigned long find_first_bit(const unsigned long *addr, unsigned long size)
{
    unsigned long idx;

    for (idx = 0; idx * BITS_PER_LONG < size; idx++)
    {
        if (addr[idx])
            return MIN(idx * BITS_PER_LONG + __ffs(addr[idx]), size);
    }

    return size;
}


unsigned long find_last_bit(const unsigned long *addr, unsigned long size)
{
    if (size)
    {
        unsigned long val = BITMAP_LAST_WORD_MASK(size);
        unsigned long idx = (size-1) / BITS_PER_LONG;

        do
        {
            val &= addr[idx];
            if (val)
                return idx * BITS_PER_LONG + __fls(val);

            val = ~0ul;
        } while (idx--);
    }
    return size;
}

static unsigned int train_100_array[] = {
    31, 30, 29, 28,
    27, 26, 25, 24,
    23, 22, 21, 20,
    19, 18, 17, 16,
    0,  1,  2,  3,
    4,  5,  6,  7,
    8,  9,  10, 11,
    12, 13, 14, 15,
};

static unsigned int train_10_array[] = {
    31, 30, 29, 28,
    27, 26, 25, 24,
    23, 22, 21, 20,
    19, 18, 17, 16,
    0,  1,  2,  3,
    4,  5,  6,  7,
    8,  9,  10, 11,
    12, 13, 14, 15,
};

static unsigned int train_cp_array[] = {
    0, 1, 2, 3,
};
#define REFIX_CP_TRAIN_DATA_OFFSET    (0)
#define REFIX_100M_TRAIN_DATA_OFFSET    (0)
#define REFIX_10M_TRAIN_DATA_OFFSET    (0)

struct s_train_val train_array[TRAIN_MAX_SIZE] = {

    [CP_VAL_0].name = "cp_val_0",
    [CP_VAL_0].src_add = 0x340,
    [CP_VAL_0].src_mask = 0xc0,
    [CP_VAL_0].src_valid_index = 15,
    [CP_VAL_0].dst_add = 0x18,
    [CP_VAL_0].dst_valid_index = 22,
    [CP_VAL_0].bind_train_array = train_cp_array,
    [CP_VAL_0].bind_train_size = ARRAY_SIZE(train_cp_array),
    [CP_VAL_0].usr_train_offset =
    REFIX_CP_TRAIN_DATA_OFFSET,

    [CP_VAL_1].name = "cp_val_1",
    [CP_VAL_1].src_add = 0x340,
    [CP_VAL_1].src_mask = 0xc0,
    [CP_VAL_1].src_valid_index = 15,
    [CP_VAL_1].dst_add = 0xb7c,
    [CP_VAL_1].dst_valid_index = 6,
    [CP_VAL_1].bind_train_array = train_cp_array,
    [CP_VAL_1].bind_train_size = ARRAY_SIZE(train_cp_array),
    [CP_VAL_1].usr_train_offset =
    REFIX_CP_TRAIN_DATA_OFFSET,

    [VAL_100M_0].name = "val_100M_0",
    [VAL_100M_0].src_add = 0x340,
    [VAL_100M_0].src_mask = 0x1f,
    [VAL_100M_0].src_valid_index = 5,
    [VAL_100M_0].dst_add = 0x24,
    [VAL_100M_0].dst_valid_index = 8,
    [VAL_100M_0].bind_train_array = train_100_array,
    [VAL_100M_0].bind_train_size = ARRAY_SIZE(train_100_array),
    [VAL_100M_0].usr_train_offset =
    REFIX_100M_TRAIN_DATA_OFFSET,

    [VAL_100M_1].name = "val_100M_1",
    [VAL_100M_1].src_add = 0x340,
    [VAL_100M_1].src_mask = 0x1f,
    [VAL_100M_1].src_valid_index = 5,
    [VAL_100M_1].dst_add = 0x1308,
    [VAL_100M_1].dst_valid_index = 8,
    [VAL_100M_1].bind_train_array = train_100_array,
    [VAL_100M_1].bind_train_size = ARRAY_SIZE(train_100_array),
    [VAL_100M_1].usr_train_offset =
    REFIX_100M_TRAIN_DATA_OFFSET,

    [VAL_10M_0].name = "val_10M_0",
    [VAL_10M_0].src_add = 0x340,
    [VAL_10M_0].src_mask = 0x1f00,
    [VAL_10M_0].src_valid_index = 13,
    [VAL_10M_0].dst_add = 0x24,
    [VAL_10M_0].dst_valid_index = 16,
    [VAL_10M_0].bind_train_array = train_10_array,
    [VAL_10M_0].bind_train_size = ARRAY_SIZE(train_10_array),
    [VAL_10M_0].usr_train_offset =
    REFIX_10M_TRAIN_DATA_OFFSET,

    [VAL_10M_1].name = "val_10M_1",
    [VAL_10M_1].src_add = 0x340,
    [VAL_10M_1].src_mask = 0x1f00,
    [VAL_10M_1].src_valid_index = 13,
    [VAL_10M_1].dst_add = 0x1308,
    [VAL_10M_1].dst_valid_index = 0,
    [VAL_10M_1].bind_train_array = train_10_array,
    [VAL_10M_1].bind_train_size = ARRAY_SIZE(train_10_array),
    [VAL_10M_1].usr_train_offset =
    REFIX_10M_TRAIN_DATA_OFFSET,
};

void dump_phy_train_val(struct s_train_val *p_train)
{
    rt_kprintf("------------GAP_LINE---------------\n");
    rt_kprintf("name            : %s\n", p_train->name);
    rt_kprintf("src_add         : %08x\n", p_train->src_add);
    rt_kprintf("src_mask        : %08x\n", p_train->src_mask);
    rt_kprintf("src_valid_index : %08x\n", p_train->src_valid_index);
    rt_kprintf("dst_add         : %08x\n", p_train->dst_add);
    rt_kprintf("dst_valid_index : %08x\n", p_train->dst_valid_index);
    rt_kprintf("usr_train_offset: %d\n", p_train->usr_train_offset);
}


void parse_train_data(struct s_train_val *p_train)
{
    unsigned long test_data;
    int i;
    unsigned int temp_data;
    unsigned int valid_bit_pos;
    int max_offset = 0;
    int low_offset = 0;
    unsigned int *array;
    unsigned int size;

    array = p_train->bind_train_array;
    size = p_train->bind_train_size;
    test_data = p_train->src_mask;
    valid_bit_pos = find_first_bit(&test_data, 32);
    if ((!array) ||
        ((p_train->src_mask >> valid_bit_pos) != (size - 1)))
    {
        rt_kprintf("para error..\n");
        return;
    }

    temp_data = fh_pmu_get_reg(p_train->src_add);
    if (temp_data & (1 << p_train->src_valid_index))
    {
        /*got valid val*/
        temp_data &= p_train->src_mask;
        temp_data >>= valid_bit_pos;
        rt_kprintf("[%s] :: train val %d\n", p_train->name, temp_data);
        /*find index*/
        for (i = 0; i < size; i++)
        {
            if (temp_data == array[i])
            {
                max_offset = (size - 1) - i;
                low_offset = 0 - i;
                break;
            }
        }

        if ((p_train->usr_train_offset < low_offset) ||
        (p_train->usr_train_offset > max_offset)) {
            rt_kprintf("offset [%d] should limit in [%d : %d]\n",
            p_train->usr_train_offset, low_offset, max_offset);
            return;
        }
        i += p_train->usr_train_offset;
        temp_data = array[i];
        /*rt_kprintf("fix idx :: [%d] = %d\n", i, temp_data);*/
    }
    else
    {
        temp_data = 0;
        /*rt_kprintf("[%s] :: use default train data %d\n",*/
        /*p_train->name, temp_data);*/
    }
    temp_data &= (p_train->src_mask >> valid_bit_pos);
    fh_pmu_ephy_set_m_reg(p_train->dst_add,
    temp_data << p_train->dst_valid_index,
    (p_train->src_mask >> valid_bit_pos) << p_train->dst_valid_index);
}

void fh_internal_ephy_v2_reset(void)
{
    int idx;

#ifndef CONFIG_ARCH_FH885xV310
    fh_pmu_ephy_set_m_reg(0, 0, 0x3<<7);
    fh_pmu_ephy_set_reg(0x28, 0x00247875);
    fh_pmu_ephy_set_reg(0x1274, 0x3c3);

    fh_pmu_set_reg_m(REG_PMU_CLK_GATE1, 0, 1<<0);
    fh_pmu_set_reg(REG_PMU_SWRSTN_NSR, 0xbfffffff);
    udelay(200);
    fh_pmu_set_reg(REG_PMU_SWRSTN_NSR, 0xffffffff);
    fh_pmu_ephy_set_m_reg(0x1370, 0x0, 1<<3);
    fh_pmu_ephy_set_reg(0x1274, 0x3c3);
    fh_pmu_ephy_set_reg(0x0b6C, 0xf04);
    fh_pmu_ephy_set_reg(0x1340, 0x0000);
    fh_pmu_ephy_set_reg(0x0bA8, 0x20);
    fh_pmu_ephy_set_reg(0x0018, 0x00c02800);
    fh_pmu_ephy_set_reg(0x001c, 0x00810000);
    fh_pmu_ephy_set_reg(0x0020, 0x0000340c);
    fh_pmu_ephy_set_reg(0x0024, 0x10131500);
    fh_pmu_ephy_set_reg(0x0028, 0x01402800);
    fh_pmu_ephy_set_reg(0x001c, 0xa1800000);
    fh_pmu_ephy_set_reg(0x0028, 0x00402800);
    fh_pmu_ephy_set_reg(0x0014, 0x06000000);
    fh_pmu_ephy_set_reg(0x0020, 0x2340c);
    fh_pmu_ephy_set_reg(0x000c, 0xba0edfff);
    fh_pmu_ephy_set_reg(0x0010, 0x7fcbf9de);
    fh_pmu_ephy_set_reg(0x0b98, 0xd150);
    fh_pmu_ephy_set_reg(0x1224, 0);
    fh_pmu_ephy_set_reg(0x12f0, 0);
    fh_pmu_ephy_set_reg(0x0bc0, 0x7de);
    fh_pmu_ephy_set_reg(0x0bc8, 0x09de);
    fh_pmu_ephy_set_reg(0x070c, 0x0401);
    fh_pmu_ephy_set_reg(0x0a84, 0x1d20);
    fh_pmu_ephy_set_reg(0x06d4, 0x3130);
    fh_pmu_ephy_set_reg(0x0a6c, 0x0818);
    fh_pmu_ephy_set_reg(0x0aec, 0x1000);
    fh_pmu_ephy_set_reg(0x0658, 0x1c00);
    fh_pmu_ephy_set_reg(0x07d4, 0x6900);
    fh_pmu_ephy_set_reg(0x06b8, 0x4);
    fh_pmu_ephy_set_reg(0x06E4, 0x04a0);
    fh_pmu_ephy_set_reg(0x0BBC, 0x1759);
    fh_pmu_ephy_set_reg(0x0bf8, 0x0100);
    fh_pmu_ephy_set_reg(0x0794, 0x9000);
    fh_pmu_ephy_set_reg(0x0788, 0xf);
    fh_pmu_ephy_set_reg(0x1378, 0x1823);
    fh_pmu_ephy_set_m_reg(0x0, 0x23<<24, 0xff<<24);
    fh_pmu_ephy_set_m_reg(0x4, 0x18, 0xff);
    fh_pmu_ephy_set_reg(0x0680, 0x0552);
    fh_pmu_ephy_set_reg(0x1274, 0);
    fh_pmu_ephy_set_reg(0x0b6C, 0x704);
    fh_pmu_ephy_set_reg(0x1340, 0);
    fh_pmu_ephy_set_reg(0x0ba8, 0);
    fh_pmu_ephy_set_reg(0x06c8, 0xc000);
    fh_pmu_ephy_set_reg(0x06e0, 0x8508);
    fh_pmu_ephy_set_reg(0x1360, 0x0137);
    fh_pmu_ephy_set_reg(0x076c, 0x0430);
    fh_pmu_ephy_set_reg(0x06b4, 0xfd00);
    fh_pmu_ephy_set_m_reg(0x0778, 0x24 << 8, 0xff << 8);
    fh_pmu_ephy_set_m_reg(0x077c, 0x24, 0xff);
    /*led setting*/
    fh_pmu_ephy_set_reg(0x2c, 0x30);
    fh_pmu_ephy_set_m_reg(0x07f8, 0x3<<8, 0x3<<8);
#else
    rt_kprintf("fh internal phy reset go...\n ");
    fh_pmu_ephy_set_m_reg(0, 0, 0x3<<7);
    fh_pmu_ephy_set_reg(0x28, 0x00247875);
    fh_pmu_ephy_set_reg(0x1274, 0x3c3);

    fh_pmu_set_reg_m(REG_PMU_CLK_GATE1, 0, 1<<0);
    fh_pmu_set_reg(REG_PMU_SWRSTN_NSR, 0xbfffffff);
    udelay(200);
    fh_pmu_set_reg(REG_PMU_SWRSTN_NSR, 0xffffffff);

    fh_pmu_ephy_set_reg(0x1370, 0x1006);
    fh_pmu_ephy_set_reg(0x0b84, 0x0600);
    fh_pmu_ephy_set_reg(0x1270, 0x1900);
    fh_pmu_ephy_set_reg(0x0b98, 0xd150);
    fh_pmu_ephy_set_reg(0x1274, 0x0000);
    fh_pmu_ephy_set_reg(0x1378, 0x1820);
    fh_pmu_ephy_set_reg(0x0bf8, 0x0130);
    fh_pmu_ephy_set_reg(0x1374, 0x0009);
    fh_pmu_ephy_set_reg(0x0b6c, 0x0704);
    fh_pmu_ephy_set_reg(0x1340, 0x0000);
    fh_pmu_ephy_set_reg(0x0ba8, 0x0000);
    fh_pmu_ephy_set_reg(0x0b7c, 0x10c0);
    fh_pmu_ephy_set_reg(0x1308, 0x0000);
    fh_pmu_ephy_set_reg(0x1378, 0x1820);
    fh_pmu_ephy_set_reg(0x0000, 0x20000000);
    fh_pmu_ephy_set_reg(0x0004, 0x00000018);
    fh_pmu_ephy_set_reg(0x0014, 0x06000000);
    fh_pmu_ephy_set_reg(0x0018, 0x00c00040);
    fh_pmu_ephy_set_reg(0x001c, 0xa1800000);
    fh_pmu_ephy_set_reg(0x0020, 0x0002340c);
    fh_pmu_ephy_set_reg(0x0024, 0x10000000);
    fh_pmu_ephy_set_reg(0x0028, 0x00402800);
    fh_pmu_ephy_set_reg(0x000c, 0xba0edfff);
    fh_pmu_ephy_set_reg(0x0010, 0x7fcbf9de);
    fh_pmu_ephy_set_reg(0x1224, 0x0000);
    fh_pmu_ephy_set_reg(0x12f0, 0x0000);
    fh_pmu_ephy_set_reg(0x0bc0, 0x07de);
    fh_pmu_ephy_set_reg(0x0bc8, 0x09de);
    fh_pmu_ephy_set_reg(0x070c, 0x0401);
    fh_pmu_ephy_set_reg(0x06b4, 0xfd00);
    fh_pmu_ephy_set_reg(0x0a84, 0x1d20);
    fh_pmu_ephy_set_reg(0x06d4, 0x3130);
    fh_pmu_ephy_set_reg(0x0a6c, 0x0818);
    fh_pmu_ephy_set_reg(0x0aec, 0x1000);
    fh_pmu_ephy_set_reg(0x0658, 0x1c00);
    fh_pmu_ephy_set_reg(0x07d4, 0x6900);
    fh_pmu_ephy_set_reg(0x06b8, 0x0004);
    fh_pmu_ephy_set_reg(0x06e4, 0x04a0);
    fh_pmu_ephy_set_reg(0x0bbc, 0x1759);
    fh_pmu_ephy_set_reg(0x0788, 0x000f);
    fh_pmu_ephy_set_reg(0x0680, 0x0552);
    fh_pmu_ephy_set_reg(0x06c8, 0xc000);
    fh_pmu_ephy_set_reg(0x06e0, 0x8508);
    fh_pmu_ephy_set_reg(0x1360, 0xff37);
    fh_pmu_ephy_set_reg(0x076c, 0x0430);
    fh_pmu_ephy_set_reg(0x0794, 0x9000);

    fh_pmu_ephy_set_reg(0x2c, 0x13);
    fh_pmu_ephy_set_reg(0x07f8, 0x325);
#endif
    for (idx = 0; idx < ARRAY_SIZE(train_array); idx++)
    {
        /*dump_phy_train_val(&train_array[idx]);*/
        parse_train_data(&train_array[idx]);
    }
}
#endif

int fh_pmu_internal_ephy_reset(void)
{

#if defined CONFIG_ARCH_FH8636_FH8852V20X || defined CONFIG_ARCH_FH885xV300 || defined CONFIG_ARCH_FH885xV310
    fh_internal_ephy_v2_reset();
#else

#if defined(CONFIG_ARCH_FH885xV200) || \
    defined(CONFIG_ARCH_FH865x)

    unsigned char train_data;
    /*reset sunplus phy */
    /*
     * ephy shutdown: ---____________________________
     * ephy rst         : ___|<=L_10ms=>|--|<=H_12us=>|--
     * ephy clkin       : _-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
     */
    /* pull up shut down first.. */
    fh_pmu_set_reg_m(REG_PMU_ETHPHY_REG0,
    1 << INSIDE_PHY_SHUTDOWN_BIT_POS,
    1 << INSIDE_PHY_SHUTDOWN_BIT_POS);

    udelay(5);
    /* pull down shut down and rst for 10ms. */
    fh_pmu_set_reg_m(REG_PMU_ETHPHY_REG0, 0,
    1 << INSIDE_PHY_SHUTDOWN_BIT_POS);

    fh_pmu_set_reg_m(REG_PMU_SWRSTN_NSR, 0,
    1 << INSIDE_PHY_RST_BIT_POS);
    msleep(10);
    /* pull up rst for 12us. */
    fh_pmu_set_reg_m(REG_PMU_SWRSTN_NSR,
    1 << INSIDE_PHY_RST_BIT_POS,
    1 << INSIDE_PHY_RST_BIT_POS);

    udelay(12);

    train_data = fh_pmu_get_reg(REG_PMU_EPHY_PARAM);
    if (train_data & (1 << TRAINING_EFUSE_ACTIVE_BIT_POS))
    {
        /* train_data */
        rt_kprintf("ephy: training data is :%x\n", train_data);
        fh_pmu_set_reg_m(REG_PMU_ETHPHY_REG0,
        (train_data & INSIDE_PHY_TRAINING_MASK)
        << INSIDE_PHY_TRAINING_BIT_POS,
        INSIDE_PHY_TRAINING_MASK << INSIDE_PHY_TRAINING_BIT_POS);
    }
    else
    {
        /* todo set default value...*/
        train_data = 0x0;
        rt_kprintf("ephy: no training data, use default:%x\n",
            train_data);
        /* train_data */
        fh_pmu_set_reg_m(REG_PMU_ETHPHY_REG0,
        train_data << INSIDE_PHY_TRAINING_BIT_POS,
        INSIDE_PHY_TRAINING_MASK << INSIDE_PHY_TRAINING_BIT_POS);
    }
#endif
#endif
    return 0;
}

#if defined CONFIG_ARCH_FH885xV310
int fh_pmu_inf_set(unsigned int phy_sel)
{
    /*bit 100 : rmii; bit 000 : mii*/
    if (phy_sel == EXTERNAL_PHY)
    {
        fh_pmu_set_reg_m(REG_PMU_ETHPHY_REG0, 4, 7);
    }
    else
    {
        fh_pmu_set_reg_m(REG_PMU_ETHPHY_REG0, 0, 7);
    }
    return 0;
}
#else
int fh_pmu_inf_set(unsigned int phy_sel)
{
    return 0;
}
#endif

void fh_pmu_ephy_sel(unsigned int phy_sel)
{

#if defined CONFIG_ARCH_FH8636_FH8852V20X

#else

#if defined(CONFIG_ARCH_FH885xV200) || \
defined(CONFIG_ARCH_FH865x) || defined CONFIG_ARCH_FH885xV300 || defined CONFIG_ARCH_FH885xV310
    fh_pmu_set_reg_m(REG_PMU_CLK_SEL,
    1 << CLK_SCAN_BIT_POS, 1 << CLK_SCAN_BIT_POS);

    if (phy_sel == EXTERNAL_PHY)
    {
        /* close inside phy */
        fh_pmu_set_reg_m(REG_PMU_ETHPHY_REG0,
        0 << INSIDE_PHY_ENABLE_BIT_POS, 1 << INSIDE_PHY_ENABLE_BIT_POS);
        /* gate inside phy */
        fh_pmu_set_reg_m(REG_PMU_CLK_GATE1,
        1 << INSIDE_CLK_GATE_BIT_POS,
        1 << INSIDE_CLK_GATE_BIT_POS);
        /* set div */
        fh_pmu_set_reg_m(REG_PMU_CLK_DIV6,
        1 << MAC_REF_CLK_DIV_BIT_POS,
        MAC_REF_CLK_DIV_MASK << MAC_REF_CLK_DIV_BIT_POS);
        /* set pad */
        /* eth ref clk out Ungate */
        fh_pmu_set_reg_m(REG_PMU_CLK_GATE,
        0 << ETH_REF_CLK_OUT_GATE_BIT_POS,
        1 << ETH_REF_CLK_OUT_GATE_BIT_POS);
        /* eth rmii clk Ungate */
        fh_pmu_set_reg_m(REG_PMU_CLK_GATE,
        0 << ETH_RMII_CLK_OUT_GATE_BIT_POS,
        1 << ETH_RMII_CLK_OUT_GATE_BIT_POS);
        /* switch mac clk in */
        fh_pmu_set_reg_m(REG_PMU_CLK_SEL,
        1 << IN_OR_OUT_PHY_SEL_BIT_POS,
        1 << IN_OR_OUT_PHY_SEL_BIT_POS);
    }
    else
    {
        /* inside phy enable */
        fh_pmu_set_reg_m(REG_PMU_ETHPHY_REG0,
        1 << INSIDE_PHY_ENABLE_BIT_POS,
        1 << INSIDE_PHY_ENABLE_BIT_POS);
        /* set pad no need */
        /* eth ref clk out gate */
        fh_pmu_set_reg_m(REG_PMU_CLK_GATE,
        1 << ETH_REF_CLK_OUT_GATE_BIT_POS,
        1 << ETH_REF_CLK_OUT_GATE_BIT_POS);
        /* eth rmii clk gate */
        fh_pmu_set_reg_m(REG_PMU_CLK_GATE,
        1 << ETH_RMII_CLK_OUT_GATE_BIT_POS,
        1 << ETH_RMII_CLK_OUT_GATE_BIT_POS);
        /* inside phy clk Ungate */
        fh_pmu_set_reg_m(REG_PMU_CLK_GATE1,
        0 << INSIDE_CLK_GATE_BIT_POS,
        1 << INSIDE_CLK_GATE_BIT_POS);
        /* int rmii refclk mux */
        fh_pmu_set_reg_m(REG_PMU_CLK_SEL,
        0 << IN_OR_OUT_PHY_SEL_BIT_POS,
        1 << IN_OR_OUT_PHY_SEL_BIT_POS);
    }
#else
    return;
#endif

#endif
}
void fh_pmu_set_sdc1_funcsel(unsigned int val)
{
#ifdef REG_PMU_SD1_FUNC_SEL
#ifdef CONFIG_ARCH_FH8636_FH8852V20X
    static int sd1_func_map[] = {1, 0, 2}; /* func sel map */
#elif defined(CONFIG_ARCH_FH865x) || defined(CONFIG_ARCH_FH885xV200)
    static int sd1_func_map[] = {0, 1, 2, 3}; /* func sel map */
#elif defined(CONFIG_ARCH_FH8862)
    static int sd1_func_map[] = {3, 0, 2, 1}; /* func sel map */
#else
    static int sd1_func_map[] = SD1_FUNC_SEL_MAP; /* func sel map */
#endif

    RT_ASSERT(val < ARRAY_SIZE(sd1_func_map));
#ifdef CONFIG_ARCH_FH8862
    fh_pmu_set_reg_m(REG_PMU_SDC_MISC, sd1_func_map[val] << 30, 0xc0000000);
#else
    fh_pmu_set_reg(REG_PMU_SD1_FUNC_SEL, sd1_func_map[val]);
#endif
#endif
}

void fh_pmu_arxc_write_A625_INT_RAWSTAT(unsigned int val)
{
	fh_pmu_set_reg(PMU_A625_INT_RAWSTAT, val);
}

unsigned int fh_pmu_arxc_read_ARM_INT_RAWSTAT(void)
{
	return fh_pmu_get_reg(PMU_ARM_INT_RAWSTAT);
}

void fh_pmu_arxc_write_ARM_INT_RAWSTAT(unsigned int val)
{
	fh_pmu_set_reg(PMU_ARM_INT_RAWSTAT, val);
}

unsigned int fh_pmu_arxc_read_ARM_INT_STAT(void)
{
	return fh_pmu_get_reg(PMU_ARM_INT_STAT);
}

void fh_pmu_arxc_reset(unsigned long phy_addr)
{
	unsigned int arc_addr;

	/*ARC Reset*/
	fh_pmu_set_reg(REG_PMU_SWRSTN_NSR, ~(1<<ARC_RSTN_BIT));

	arc_addr = ((phy_addr & 0xffff) << 16) | (phy_addr >> 16);

	fh_pmu_set_reg(REG_PMU_A625BOOT0, 0x7940266B);
	/* Configure ARC Bootcode start address */
	fh_pmu_set_reg(REG_PMU_A625BOOT1, arc_addr);
	fh_pmu_set_reg(REG_PMU_A625BOOT2, 0x0F802020);
	fh_pmu_set_reg(REG_PMU_A625BOOT3, arc_addr);

	/*clear ARC ready flag*/
	fh_pmu_arxc_write_ARM_INT_RAWSTAT(0);

	/* don't let ARC run when release ARC */
	fh_pmu_set_reg(REG_PMU_A625_START_CTRL, 0);
	udelay(2);

	/* ARC reset released */
	fh_pmu_set_reg(REG_PMU_SWRSTN_NSR, 0xFFFFFFFF);
}

void fh_pmu_arxc_kickoff(void)
{
	//start ARC625
	fh_pmu_set_reg(REG_PMU_A625_START_CTRL, 0x10);
}

void fh_pmu_set_user(void)
{
#ifdef CONFIG_ARCH_FH885xV200
    unsigned int pmuuser0 = fh_pmu_get_reg(REG_PMU_USER0);
    pmuuser0 &= ~(0x01);
    fh_pmu_set_reg(REG_PMU_USER0,pmuuser0);
#endif
}

unsigned int fh_pmu_get_ddrsize(void)
{
#ifdef REG_PMU_DDR_SIZE
    return fh_pmu_get_reg(REG_PMU_DDR_SIZE);
#else
    return 0;
#endif
}

void fh_pmu_set_stmautostopgpio(int stmid, int gpio)
{
#if defined(CONFIG_ARCH_FH885xV300) || defined(CONFIG_ARCH_FH8862) || defined(CONFIG_ARCH_FH885xV310)
    unsigned int val;

    val = fh_pmu_get_reg(REG_PMU_EFHY_REG0);

    if (stmid)
    {
        val &= (~0xfc00);
        val |= gpio<<10;
    }
    else
    {
        val &= (~0x3f0);
        val |= gpio<<4;
    }
    fh_pmu_set_reg(REG_PMU_EFHY_REG0, val);
#endif
}
void fh_pmu_set_hashen(unsigned long en)
{
#if defined(CONFIG_ARCH_FH885xV300) || defined(CONFIG_ARCH_FH8862) || defined(CONFIG_ARCH_FH885xV310)
    unsigned int val;

    val = fh_pmu_get_reg(REG_PMU_DMA_SEL);
    if (en)
        val |= 1<<16;
    else
        val &= ~(1<<16);

    fh_pmu_set_reg(REG_PMU_DMA_SEL, val);
#endif
}
