/*
 * sensor_ops.c
 *
 *  Created on: Jun 2, 2015
 *      Author: duobao
 */

#include <string.h>
#include "isp_sensor_if.h"
#include "sensor_ops.h"
#include "sensor.h"

#ifdef OS_LINUX
#include <unistd.h>
#include <linux/types.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#else
#include <stdio.h>
#include <rtdevice.h>
#ifdef RPC_RTT
#include "i2c.h"
#endif
#endif
#include <fcntl.h>
#include <stdlib.h>
#include "sensor_ver.h"

#ifdef OS_RTT
#define FH_I2C_DEVICE		"/dev/i2c-0"
#define FH_SPI_DEVICE		"/dev/spi1"

struct rt_i2c_msg msg[2];
static rt_uint16_t data = 0x0;
// static void fh_delay_200us()
// {
// 	int t=6620;
// 	while(t--);
// }

// static void fh_delay_1ms()
// {
// 	int s=5;
// 	while(s--)
// 		fh_delay_200us();
// }
#endif

// #ifdef OS_RTT
// #define fh_printf    rt_kprintf
// #define malloc       rt_malloc
// #define free         rt_free
// #else //OS_LINXU
// #define fh_printf    printf
// #endif //OS_RTT

#define I2C_M_SPERATE_MSG	0x8000
#define SET_SENSOR_VER      _IOWR('i', 31,FH_UINT32)

#ifdef OS_LINUX
#define ISP_DEV_NAME        "/dev/isp"
#else
#define ISP_DEV_NAME        "ISP"
#endif

#ifdef OS_LINUX
#define I2C_MAX_DEVICE_NAME 5
#define I2C_MAX_NAME_STRING 20
static FH_CHAR I2CDeviceList[I2C_MAX_DEVICE_NAME][I2C_MAX_NAME_STRING] =
{
	"/dev/i2c-0",
	"/dev/i2c-1",
	"/dev/i2c-2",
	"/dev/i2c-3",
	"/dev/i2c-4",
};

#define SPI_MAX_DEVICE_NAME 1
#define SPI_MAX_NAME_STRING 20
static FH_CHAR SPIDeviceList[SPI_MAX_DEVICE_NAME][SPI_MAX_NAME_STRING] =
{
	"/dev/spi1"
};
#endif

typedef union
{
    struct
    {
        unsigned int    defXGM         :1;
        unsigned int    defXGM2        :1;
        unsigned int    defXGMPLUS     :1;
        unsigned int    defFPGA_ENV    :1;
    } bf;
    unsigned int dw;
} SENSOR_MACRO_DEF_STATUS;

typedef struct
{
    char                     pVer[32];
    SENSOR_MACRO_DEF_STATUS  stSensorDefStatus;
}SENSOR_VERSION_INFO_S;

static unsigned int sensor_get_macro_def_info(void)
{
    SENSOR_MACRO_DEF_STATUS stDefStatus;
    stDefStatus.dw = 0;

#ifdef XGM
    stDefStatus.bf.defXGM = 1;
#endif
#ifdef XGM2
    stDefStatus.bf.defXGM2 = 1;
#endif
#ifdef XGMPLUS
    stDefStatus.bf.defXGMPLUS = 1;
#endif
#ifdef FPGA_ENV
    stDefStatus.bf.defFPGA_ENV = 1;
#endif

    return stDefStatus.dw;
}

static char *get_sensor_version(char *ver)
{
	SEN_DEV_FD sensor_isp_device;
	SENSOR_VERSION_INFO_S stVerInfo;

	memcpy(stVerInfo.pVer, ver, 32);
	stVerInfo.stSensorDefStatus.dw = sensor_get_macro_def_info();

	sensor_isp_device = sensor_dev_open(ISP_DEV_NAME,0,0);

	if (sensor_isp_device < 0)
		perror("Error: ISP device open failed!\n");
	else
	{
		sensor_dev_ioctl(sensor_isp_device,SET_SENSOR_VER,&stVerInfo);

		sensor_dev_close(sensor_isp_device);
	}

	return ver;
}


static void SPISensor_Write(SensorDevice *sdev, FH_UINT16 addr, FH_UINT32 data)
{

}

static FH_UINT16 SPISensor_Read(SensorDevice *sdev, FH_UINT16 addr)
{
	return 0;
}
#ifdef OS_RTT
static rt_err_t I2CSensor_WriteEx(SensorDevice *sdev, struct rt_i2c_bus_device *fh81_i2c,rt_uint16_t reg, rt_uint16_t data, int no_ack)
{
	struct rt_i2c_msg msg;
	rt_uint8_t send_buf[4];

	msg.addr = sdev->i2c_devid;
	msg.flags = RT_I2C_WR;
	msg.buf = send_buf;
	switch(sdev->mode)
	{
	case 0:
		send_buf[0] = (reg & 0xff);
		send_buf[1] = (data & 0xff);
		msg.len = 2;
		break;
	case 1:
		send_buf[0] = (reg & 0xff);
		send_buf[1] = (data>>8) & 0xff;
		send_buf[2]= data & 0xff;
		msg.len = 3;
		break;
	case 2:
		send_buf[0] = (reg >>8)& 0xff;
		send_buf[1] = reg & 0xff;
		send_buf[2]= data & 0xff;
		msg.len = 3;
		break;
	case 3:
		send_buf[0] = (reg >>8)& 0xff;
		send_buf[1] = reg & 0xff;
		send_buf[2]= (data>>8) & 0xff;
		send_buf[3]=	data & 0xff;
		msg.len = 4;
	}

	rt_i2c_transfer(sdev->fh81_i2c, &msg, 1);
//	if(ret < 0 && !no_ack)
//		{
//			perror("ERROR: Unable to write sensor register!");
//			fh_printf("addr: 0x%x, data: 0x%x\n", reg, data);
//		}
	return RT_EOK;
}

#ifdef FUN_EN_WRITE_MULTI
static rt_err_t I2CSensor_WriteEx_Multi(SensorDevice *sdev, struct rt_i2c_bus_device *fh81_i2c,rt_uint16_t *reg, rt_uint16_t *data, int no_ack,int size)
{
	struct rt_i2c_msg *msg;
	msg = (struct rt_i2c_msg *)malloc(size * sizeof(struct rt_i2c_msg));
	if (!msg)
		return -RT_ERROR;
	int i;
	rt_uint8_t *send_buf;
	send_buf = (rt_uint8_t*)malloc(size*4);
	if (!send_buf)
	{
		free(msg);
		return -RT_ERROR;
	}

	for (i=0;i<size;i++)
	{
		msg[i].addr = sdev->i2c_devid;
		msg[i].flags = RT_I2C_WR | I2C_M_SPERATE_MSG;
		msg[i].buf = &send_buf[4*i];

		switch(sdev->mode)
		{
		case 0:
			send_buf[0+i*4] = (reg[i] & 0xff);
			send_buf[1+i*4] = (data[i] & 0xff);
			msg[i].len = 2;
			break;
		case 1:
			send_buf[0+i*4] = (reg[i] & 0xff);
			send_buf[1+i*4] = (data[i]>>8) & 0xff;
			send_buf[2+i*4] = data[i] & 0xff;
			msg[i].len = 3;
			break;
		case 2:
			send_buf[0+i*4] = (reg[i] >>8)& 0xff;
			send_buf[1+i*4] = reg[i] & 0xff;
			send_buf[2+i*4] = data[i] & 0xff;
			msg[i].len = 3;
			break;
		case 3:
			send_buf[0+i*4] = (reg[i] >>8)& 0xff;
			send_buf[1+i*4] = reg[i] & 0xff;
			send_buf[2+i*4] = (data[i]>>8) & 0xff;
			send_buf[3+i*4] = data[i] & 0xff;
			msg[i].len = 4;
		}

	}

	rt_i2c_transfer(sdev->fh81_i2c, msg, size);

	free(msg);
	free(send_buf);
//	if(ret < 0 && !no_ack)
//		{
//			perror("ERROR: Unable to write sensor register!");
//			fh_printf("addr: 0x%x, data: 0x%x\n", reg, data);
//		}
	return RT_EOK;
}
#endif

#else
static void I2CSensor_WriteEx(SensorDevice *sdev, FH_UINT16 addr, FH_UINT16 data, int no_ack)
{
	struct i2c_rdwr_ioctl_data i2c_data;
	struct i2c_msg msgs[1] = {{0}};
	FH_UINT8 tmp[4];
	int ret;

	i2c_data.nmsgs = 1;
	//i2c_data.msgs = (struct i2c_msg *)malloc(i2c_data.nmsgs * sizeof(struct i2c_msg));
	i2c_data.msgs = (struct i2c_msg *)&msgs;

	switch(sdev->mode)
	{
	case 0:
		tmp[0] = addr & 0xff;
		tmp[1] = data & 0xff;
		i2c_data.msgs[0].len = 2;
		break;
	case 1:
		tmp[0] = addr & 0xff;
		tmp[1] = (data>>8) & 0xff;
		tmp[2] = data & 0xff;
		i2c_data.msgs[0].len = 3;
		break;
	case 2:
		tmp[0] = (addr>>8) & 0xff;
		tmp[1] = addr & 0xff;
		tmp[2] = data & 0xff;
		i2c_data.msgs[0].len = 3;
		break;
	default:
	case 3:
		tmp[0] = (addr>>8) & 0xff;
		tmp[1] = addr & 0xff;
		tmp[2] = (data>>8) & 0xff;
		tmp[3] = data & 0xff;
		i2c_data.msgs[0].len = 4;
		break;
	}

	i2c_data.msgs[0].addr = sdev->i2c_devid;				/* */
	i2c_data.msgs[0].flags = 0; 			/* write */
	i2c_data.msgs[0].buf = tmp;

	ret = ioctl(sdev->fd, I2C_RDWR, &i2c_data);
	//free(i2c_data.msgs);
	if(ret < 0 && !no_ack)
	{
		perror("ERROR: Unable to write sensor register!");
		fh_printf("addr: 0x%x, data: 0x%x\n", addr, data);
	}
}
#ifdef FUN_EN_WRITE_MULTI
static void I2CSensor_WriteEx_Multi(SensorDevice *sdev, FH_UINT16 *addr, FH_UINT16 *data, int no_ack,int size)
{
	struct i2c_rdwr_ioctl_data i2c_data;
	FH_UINT8 *tmp;
	int ret;

	i2c_data.nmsgs = size;
	i2c_data.msgs = (struct i2c_msg *)malloc(i2c_data.nmsgs * sizeof(struct i2c_msg));
	if (!i2c_data.msgs)
		return;
	tmp = (FH_UINT8*)malloc(size*4);

	if (!tmp)
	{
		free(i2c_data.msgs);
		return;
	}


	int i;
	for (i=0;i<size;i++)
	{
		switch(sdev->mode)
		{
		case 0:
			tmp[0+i*4] = addr[i] & 0xff;
			tmp[1+i*4] = data[i] & 0xff;
			i2c_data.msgs[i].len = 2;
			break;
		case 1:
			tmp[0+i*4] = addr[i] & 0xff;
			tmp[1+i*4] = (data[i]>>8) & 0xff;
			tmp[2+i*4] = data[i] & 0xff;
			i2c_data.msgs[i].len = 3;
			break;
		case 2:
			tmp[0+i*4] = (addr[i]>>8) & 0xff;
			tmp[1+i*4] = addr[i] & 0xff;
			tmp[2+i*4] = data[i] & 0xff;
			i2c_data.msgs[i].len = 3;
			break;
		default:
		case 3:
			tmp[0+i*4] = (addr[i]>>8) & 0xff;
			tmp[1+i*4] = addr[i] & 0xff;
			tmp[2+i*4] = (data[i]>>8) & 0xff;
			tmp[3+i*4] = data[i] & 0xff;
			i2c_data.msgs[i].len = 4;
			break;
		}

		i2c_data.msgs[i].addr = sdev->i2c_devid;				
		i2c_data.msgs[i].flags = 0 | I2C_M_SPERATE_MSG; 			//write 
		i2c_data.msgs[i].buf = &tmp[4*i];
	}

	ret = ioctl(sdev->fd, I2C_RDWR, &i2c_data);
	//FH_UINT8* sndb  = tmp;
	//int k  =0;
	//for (k = 0; k<size;k++) {
	//	write(sdev->fd,sndb,i2c_data.msgs[k].len);
	//	sndb +=4;
	//	ret =0;
	//}
	free(i2c_data.msgs);
	free(tmp);
	if(ret < 0 && !no_ack)
	{
		perror("ERROR: Unable to write sensor register!");
		fh_printf("addr: 0x%x, data: 0x%x\n", addr[0], data[0]);
	}
}
#endif
#endif
#ifdef FUN_EN_WRITE_MULTI
static void I2CSensor_Write_Multi(SensorDevice *sdev, FH_UINT16 *addr, FH_UINT16 *data,FH_UINT16 size)
{
#ifdef OS_RTT
	I2CSensor_WriteEx_Multi(sdev, sdev->fh81_i2c,addr, data, 0,size);
#else
	I2CSensor_WriteEx_Multi(sdev, addr, data, 0,size);
#endif
}
#endif
static void I2CSensor_Write(SensorDevice *sdev, FH_UINT16 addr, FH_UINT16 data)
{
#ifdef OS_RTT
	I2CSensor_WriteEx(sdev, sdev->fh81_i2c,addr, data, 0);
#else
	I2CSensor_WriteEx(sdev, addr, data, 0);
#endif
}
#ifdef OS_RTT
static rt_err_t I2CSensor_Read(SensorDevice *sdev, struct rt_i2c_bus_device *fh81_i2c,rt_uint16_t reg, rt_uint16_t *data)
{
	struct rt_i2c_msg msg[2];
	rt_uint8_t send_buf[2];
	rt_uint8_t recv_buf[2];
	msg[0].addr = sdev->i2c_devid;
	msg[0].flags = RT_I2C_WR;
	msg[1].addr = sdev->i2c_devid;
	msg[1].flags = RT_I2C_RD;
	switch(sdev->mode)
	{
	case 0:
		send_buf[0] = (reg & 0xff);
		msg[0].len = 1;
		msg[0].buf = send_buf;
		msg[1].len = 1;
		msg[1].buf = recv_buf;
		break;
	case 1:
		send_buf[0] = (reg & 0xff);
		msg[0].len = 1;
		msg[0].buf = send_buf;
		msg[1].len = 2;
		msg[1].buf = recv_buf;
		break;
	case 2:
		send_buf[0] =(reg>>8) & 0xff;
		send_buf[1] =(reg & 0xff);
		msg[0].len = 2;
		msg[0].buf = send_buf;
		msg[1].len = 1;
		msg[1].buf = recv_buf;
		break;
	case 3:
		send_buf[0] =(reg>>8) & 0xff;
		send_buf[1] =(reg & 0xff);
		msg[0].len = 2;
		msg[0].buf = send_buf;
		msg[1].len = 2;
		msg[1].buf = recv_buf;
	}
	rt_i2c_transfer(sdev->fh81_i2c, msg, 2);
	if(sdev->mode & 1)
		*data = (recv_buf[0]<<8)| recv_buf[1];
	else
		*data = recv_buf[0];

	return RT_EOK;
}
#else
static FH_UINT16 I2CSensor_Read(SensorDevice *sdev, FH_UINT16 addr)
{
	struct i2c_rdwr_ioctl_data i2c_data;
	struct i2c_msg msgs[2] = {{0}};
	FH_UINT8 reg[4] = {0};
	FH_UINT8 val[4] = {0};
	int ret;

	i2c_data.nmsgs = 2;
	//i2c_data.msgs = (struct i2c_msg *)malloc(i2c_data.nmsgs * sizeof(struct i2c_msg));
	i2c_data.msgs = msgs;

	switch(sdev->mode)
	{
	case 0:
		reg[0] = addr & 0xff;
		i2c_data.msgs[0].len = 1;
		i2c_data.msgs[1].len = 1;
		break;
	case 1:
		reg[0] = addr & 0xff;
		i2c_data.msgs[0].len = 1;
		i2c_data.msgs[1].len = 2;
		break;
	case 2:
		reg[0] = (addr>>8) & 0xff;
		reg[1] = addr & 0xff;
		i2c_data.msgs[0].len = 2;
		i2c_data.msgs[1].len = 1;
		break;
	default:
	case 3:
		reg[0] = (addr>>8) & 0xff;
		reg[1] = addr & 0xff;
		i2c_data.msgs[0].len = 2;
		i2c_data.msgs[1].len = 2;
		break;
	}

	i2c_data.msgs[0].addr = sdev->i2c_devid;				/* */
	i2c_data.msgs[0].flags = 0; 						/* write */
	i2c_data.msgs[0].buf = reg;

	i2c_data.msgs[1].addr = sdev->i2c_devid;				/* */
	i2c_data.msgs[1].flags = I2C_M_RD; 					/* read */
	i2c_data.msgs[1].buf = val;

	ret = ioctl(sdev->fd, I2C_RDWR, &i2c_data);

	//free(i2c_data.msgs);
	if(ret < 0)
	{
		perror("ERROR: Unable to read sensor register!");
		return -1;
	}
	if(sdev->mode & 1)
		return (val[0] << 8) | (val[1]);
	else
		return val[0];
}
#endif
FH_UINT16 Sensor_Read(SensorDevice *sdev, FH_UINT16 addr)
{
	if(sdev->mode > 3 )
		return SPISensor_Read(sdev, addr);
	else
	{
#ifdef OS_RTT
		I2CSensor_Read(sdev, sdev->fh81_i2c,addr,&data);
		return data;
#else
		return I2CSensor_Read(sdev, addr);
#endif
	}
}
#ifdef OS_RTT
FH_UINT16 SRead(SensorDevice *sdev, FH_UINT16 addr)
{
		I2CSensor_Read(sdev, sdev->fh81_i2c,addr,&data);
		rt_kprintf("data=%x\n",data);
	return data;
}
FINSH_FUNCTION_EXPORT(SRead,Sensor_Read);
FH_UINT16 SWrite(SensorDevice *sdev, FH_UINT16 addr,FH_UINT16 data)
{
		I2CSensor_WriteEx(sdev, sdev->fh81_i2c,addr, data, 0);
		rt_kprintf("write ok\n");
	return 0;
}
FINSH_FUNCTION_EXPORT(SWrite,Sensor_Read);
#endif

void Sensor_Write(SensorDevice *sdev, FH_UINT16 addr, FH_UINT16 data)
{
	if(sdev->mode > 3 )
		SPISensor_Write(sdev, addr, data);
	else
		I2CSensor_Write(sdev, addr, data);
}
#ifdef FUN_EN_WRITE_MULTI
static void Sensor_Write_Multi(SensorDevice *sdev, FH_UINT16* addr, FH_UINT16* data,FH_UINT16 size)
{
	if(sdev->mode > 3 )
		SPISensor_Write(addr[0], data[0]);
	else
		I2CSensor_Write_Multi(addr, data,size);
}

static void Sensor_WriteEx(SensorDevice *sdev, FH_UINT16 addr, FH_UINT16 data, int no_ack)
{
	if(sdev->mode > 3 )
		SPISensor_Write(addr, data);
	else
#ifdef OS_RTT
		I2CSensor_WriteEx(sdev->fh81_i2c,addr, data, no_ack);
#else
		I2CSensor_WriteEx(addr, data, no_ack);
#endif
}
#endif
int SensorDevice_Init(SensorDevice *sdev, FH_UINT16 i2c_devid, FH_UINT8 mode, FH_UINT8 deviceId)
{
	get_sensor_version(sdev->ver);
#ifdef OS_RTT

	sdev->fh81_i2c=rt_i2c_bus_device_find("i2c0");

	if(mode > 3 )
		sdev->dev = FH_SPI_DEVICE;
	else
		sdev->dev = FH_I2C_DEVICE;

	sdev->i2c_devid = i2c_devid;
	sdev->mode = mode;
#else
	int ret;

	if(sdev->fd > 0)
	{
		perror("Error: Sensor device opened !\n");
		return -1;
	}

	if(mode > 3 )
	{
		if (deviceId < SPI_MAX_DEVICE_NAME)
			sdev->dev = SPIDeviceList[deviceId];
		else
		{
			perror("Error: Sensor device Outof Range!\n");
			return -1;
		}
	}
	else
	{
		if (deviceId < I2C_MAX_DEVICE_NAME)
			sdev->dev = I2CDeviceList[deviceId];
		else
		{
			perror("Error: Sensor device OutofRange!\n");
			return -1;
		}
	}

	sdev->i2c_devid = i2c_devid;
	sdev->mode = mode;

	sdev->fd = open(sdev->dev, O_RDWR | O_NONBLOCK);

	if(sdev->fd < 0)
	{
		perror("ERROR: Unable to open sensor!");
		return -1;
	}

	ret = ioctl(sdev->fd, I2C_TENBIT, 0);

	if(ret < 0)
	{
		perror("Error: Unable to set address mode!\n");
		return -1;
	}

	ret = ioctl(sdev->fd, I2C_SLAVE_FORCE, i2c_devid);
	if (ret < 0)
	{
		perror("Error: Unable to set slave address!\n");
		return -1;
	}
#endif


	return 0;
}


int SensorDevice_Close(SensorDevice *sdev)
{
	int ret = 0;
#ifdef OS_RTT
#else

	if(sdev->fd <= 0)
	{
		perror("Error: Sensor device not open !\n");
		ret = -1;
	}
	else
	{
		close(sdev->fd);

		sdev->fd = -1;
	}
#endif

	return ret;
}

struct isp_sensor_if *Sensor_Create_Common(SensorCreateInfo *info)
{
    struct isp_sensor_if *s_if;
    SNS_PARA_S* pstPara;

    s_if = (struct isp_sensor_if *)malloc(sizeof(struct isp_sensor_if));
    if(info->pstSnsIf)
        memcpy(s_if, info->pstSnsIf, sizeof(struct isp_sensor_if));

    pstPara = (SNS_PARA_S *)malloc(sizeof(SNS_PARA_S));
    if(info->pstSnsCtrlPara)
        memcpy(&pstPara->stCtrlPara, info->pstSnsCtrlPara, sizeof(SNS_CTRL_PARA_S));
    memset(&pstPara->stCci, 0, sizeof(SensorDevice));
    sprintf(pstPara->stCci.ver, info->pVer);

    s_if->para = pstPara;

    return s_if;
}

void Sensor_Destroy_Common(struct isp_sensor_if *s_if)
{
    free(s_if->para);
    s_if->para = 0;
    free(s_if);
    s_if = 0;
}