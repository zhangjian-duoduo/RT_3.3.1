/*****************************************************************************
*
*                                  gpio.h
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
#ifndef GPIO_H_
#define GPIO_H_
#include "fh_defs.h"
#include "chip_reg.h"
#include "config.h"



typedef enum{
	GPIO_NOR_MODE,
	GPIO_INT_MODE
}Gpio_Mode;

typedef enum {
	GPIO_LEVEL_LOW,
	GPIO_LEVEL_HIGH ,
}Gpio_Level;

typedef enum {
	GPIO_DIR_IN,
	GPIO_DIR_OUT
}Gpio_Direction;

typedef enum{
	GPIO_INT_YYPE_LEVEL,
	GPIO_INT_YYPE_EDGE
}Gpio_Int_Pol;

typedef enum{

	GPIO_INT_POL_FALL,
	GPIO_INT_POL_RISK
}Gpio_Int_Sens;

typedef enum {
	GPIO_INT_UNTRIGGERED,
	GPIO_INT_TRIGGERED
}Gpio_Int_Status;

enum {
	GPIO_EVENT_DOWN,
	GPIO_EVENT_UP,
	GPIO_EVENT_NONE = 9999
};

enum {
	GPIO_BUTTON_UP = 15,
	GPIO_BUTTON_LEFT = 16,
	GPIO_BUTTON_DOWN = 17,
	GPIO_BUTTON_RIGHT = 18,
	GPIO_BUTTON_MENU = 19,
	GPIO_BUTTON_NONE = 999
};

#define TB_OPEN			0
#define ISP_OPEN 		1
#define CFG_OPEN		0
#define TAE_SIMULATION 	0
#define SENSOR_OPEN	    1
#define OPEN_3DNR		0
#define BANDWIDTHTEST	0

typedef struct tag_GPIO_EVENT {
	unsigned short sbutton;								// left/right/up/down/menu
	unsigned short sEvent __attribute__((packed));		// up/down
	unsigned int   iTime;
}GPIO_EVENT;

UINT32 Gpio_GetPortX(UINT32 gpio_num,UINT32 idx);
UINT32 Gpio_GetPort(UINT32 gpio_num);
void Gpio_SetPortX(UINT32 gpio_num,UINT32 idx, UINT32 val);
void Gpio_SetPort(UINT32 gpio_num,UINT32 val);
void Gpio_SetDirectionX(UINT32 gpio_num,UINT32 idx, UINT32 val);
void Gpio_SetDirection(UINT32 gpio_num,UINT32 val);
UINT32 Gpio_GetDirectionX(UINT32 gpio_num,UINT32 idx);
UINT32 Gpio_GetDirection(UINT32 gpio_num);

#endif /*GPIO_H_*/
