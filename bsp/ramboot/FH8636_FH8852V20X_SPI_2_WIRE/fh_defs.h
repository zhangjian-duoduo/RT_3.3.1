/**************************************************************
  Copyright (C), 2004-2010, Fullhan Microelectronics Co., Ltd.
  File name:	fh_defs.h
  Author:		Eason
  Date:			2010-9-6
  Description:  ���峣�ú�
***************************************************************/

#ifndef FH_DEFS_H_
#define FH_DEFS_H_

//#include "fh_uart.h"

//extern void write_reg(uint32 addr, uint32 value);
#define DELAY(times)  do{\
	int i=0;\
	for(i=0;i<times;i++){__asm__("NOP");}\
}while(0)


#define FAILED				(0)
#define OK					(!FAILED)

typedef char				SINT8;
typedef short				SINT16;
typedef int					SINT32;
typedef long long			SINT64;
typedef unsigned char		UINT8;
typedef unsigned short		UINT16;
typedef unsigned int		UINT32;
typedef unsigned long long	UINT64;
typedef	unsigned char		uchar;
typedef signed   char		int8;
typedef unsigned char		uint8;
typedef signed   short		int16;
typedef unsigned short		uint16;
typedef signed   int		int32;
typedef unsigned int		uint32;
typedef signed   long  long	int64;
typedef unsigned long long	uint64;

typedef float          ieee_single;
typedef double         ieee_double;
typedef unsigned int rt_uint32_t;

typedef unsigned long  boolean;

#define lift_shift_bit_num(bit_num)			(1<<bit_num)
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))




#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))



#define reg_read(addr) (*((volatile UINT32 *)(addr)))
#define reg_write(addr,value) (*(volatile UINT32 *)(addr)=(value))

#define GET_REG(addr) reg_read(addr)
#define SET_REG(addr,value) reg_write(addr,value)
#define SET_REG_M(addr,value,mask) reg_write(addr,(reg_read(addr)&(~(mask)))|((value)&(mask)))
#define SET_REG_B(addr,element,highbit,lowbit) SET_REG_M((addr),((element)<<(lowbit)),(((1<<((highbit)-(lowbit)+1))-1)<<(lowbit)))

#define GET_REG8(addr) (*((volatile UINT8 *)(addr)))
#define SET_REG8(addr,value) (*(volatile UINT8 *)(addr)=(value))

#define read_reg(addr)  (*((volatile uint32 *)(addr)))
#define write_reg(addr, reg)  (*((volatile uint32 *)(addr))) = (uint32)(reg)
#define inw(addr)  (*((volatile uint32 *)(addr)))
#define outw(addr, reg)  (*((volatile uint32 *)(addr))) = (uint32)(reg)
#define writel(reg, addr)	write_reg(addr, reg)
#define readl(addr)			read_reg(addr)
#define NULL	((void *)0)

#define ___swab32(x) \
    ((UINT32)( \
        (((UINT32)(x) & (UINT32)0x000000ffUL) << 24) | \
        (((UINT32)(x) & (UINT32)0x0000ff00UL) <<  8) | \
        (((UINT32)(x) & (UINT32)0x00ff0000UL) >>  8) | \
        (((UINT32)(x) & (UINT32)0xff000000UL) >> 24) ))

#endif /*FH_DEFS_H_*/
