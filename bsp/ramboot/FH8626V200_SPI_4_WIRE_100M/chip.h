/*****************************************************************************
*
*                                  chip.h
*
*	Copyright (c) 2015 Shanghai Fullhan Microelectronics Co., Ltd.
*						All Rights Reserved. Confidential.
*
*	File Description:
*		Chip definition. Include the base address of each module, memory
*		address, memory size
*
*	Modification History:
*
******************************************************************************/
#ifndef _CHIP_H_
#define _CHIP_H_

/* Includes -----------------------------------------------------------------*/
/* Public typedef -----------------------------------------------------------*/

/* Public define ------------------------------------------------------------*/
// memory base address: <memName>_BASE
#define		RAM_BASE			(0x10000000)
#define		DDR_BASE			(0xA0000000)

// module register base address: <moduleName>_REG_BASE
#define		DMAC0_REG_BASE		(0xe0300000)

#define		SDC0_REG_BASE		(0xe2000000)
#define		SDC1_REG_BASE		(0xe2200000)

#define		PMU_REG_BASE		(0xf0000000)
#define		WDT_REG_BASE		(0xf0d00000)

#define		GPIO_REG_BASE		(0xf0300000)
#define		GPI1_REG_BASE		(0xf4000000)
#define		UART0_REG_BASE		(0xf0700000)
#define		UART1_REG_BASE		(0xf0800000)
#define		I2S_REG_BASE		(0x99000000)
#define		SPI0_REG_BASE		(0xf0500000)
#define		SPI1_REG_BASE		(0xf0600000)
#define		EFUSE_REG_BASE		(0xf1600000)
#define		AES_REG_BASE		(0xe8200000)
#define		REG_TIMER_BASE		(0xf0c00000)

/* Public function ----------------------------------------------------------*/
#endif
