#ifndef USER_H_
#define USER_H_

#define FH_GET_REG(addr) 	(*((volatile unsigned int *)(addr)))
#define FH_SET_REG(addr,value)	(*(volatile unsigned int *)(addr)=(value))

/**
 * FH_Gpio_GetValue() - get gpio value
 *
 * @gpio:	gpio number,
 *
 * only support gpio 0 ~ 63
 */
unsigned int FH_Gpio_GetValue(unsigned int gpio);

/**
 * FH_Gpio_SetValue() - set gpio value
 *
 * @gpio:	gpio number,
 * @val:	gpio value,
 *
 * only support gpio 0 ~ 63
 */
void FH_Gpio_SetValue(unsigned int gpio, unsigned int val);

/**
 * FH_Gpio_GetDirection() - get gpio direction
 *
 * @gpio:	gpio number,
 *
 * only support gpio 0 ~ 63
 */
unsigned int FH_Gpio_GetDirection(unsigned int gpio);

/**
 * FH_Gpio_SetDirection() - set gpio direction
 *
 * @gpio:	gpio number,
 * @val:	gpio direction,
 *
 * only support gpio 0 ~ 63
 */
void FH_Gpio_SetDirection(unsigned int gpio, unsigned int val);

/**
 * FH_Sensor_Init() - Sensor and I2C Init
 *
 * @id:		sensor device id, 7-bit
 * 		eg: 	AR0130: 0x10
 * 			OV9732: 0x36
 *
 * @mode:	sensor xfer mode, mode 0 ~ mode 3
 * 		mode 0: 8-bit addr, 8-bit data
 * 		mode 1: 8-bit addr, 16-bit data
 * 		mode 2: 16-bit addr, 8-bit data
 * 		mode 3: 16-bit addr, 16-bit data
 *
 */
void FH_Sensor_Init(unsigned int id, unsigned int mode);

/**
 * FH_Sensor_Write() - sensor register write
 *
 * @reg:	sensor register address,
 * @data:	data to write,
 *
 */
unsigned int FH_Sensor_Write(unsigned short reg, unsigned short data);

/**
 * FH_Sensor_Read() - sensor register read
 *
 * @reg:	sensor register address,
 * @data:	data to read,
 *
 */
unsigned int FH_Sensor_Read(unsigned short reg, unsigned short *data);

/**
 * FH_Flash_Read() - read data from flash to ddr
 *
 * @flash_addr:	flash offset,
 * @len:	read size,
 * @ddr_addr:	ddr address,
 *
 */
void FH_Flash_Read(unsigned int flash_addr, unsigned int len, unsigned char *ddr_addr);

/**
 * FH_UDelay() - delay
 *
 * @us:	usec,
 *
 */
void FH_UDelay(unsigned int us);

/**
 * FH_Set_BootAddr() - Set boot parameters
 *
 * @flash_addr:	image offset in flash,
 * @image_size:	image size,
 * @ddr_addr:	image address in ddr,
 *
 */
void FH_Boot_SetAddr(unsigned int flash_addr, unsigned int image_size, unsigned int ddr_addr);

/**
 * FH_SetBootargs() - Set bootargs for linux
 *
 * @args: bootargs,
 *
 */
void FH_SetBootargs(char *args);


/**
 * FH_SetDDRType() - Set Whether system initpll use in-lib pll setting
 *
 * @auto:	0 for not init in libramboot.a
            !0 for init automaticalyautomaticaly
 *
 */
typedef enum {
    DDR_TYPE_2G,
    DDR_TYPE_4G,
    DDR_TYPE_1G,
}DDR_TYPE_e;
void FH_SetDDRType(DDR_TYPE_e type);






#endif
