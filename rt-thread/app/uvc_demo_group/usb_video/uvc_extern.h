#ifndef __FLASHBASE_h__
#define __FLASHBASE_h__

#include "uvc_init.h"

extern void rt_kprintf(const char *fmt, ...);

/* A.8. Video Class-Specific Request Codes */
#define UVC_RC_UNDEFINED               0x00
#define UVC_SET_CUR                    0x01
#define UVC_GET_CUR                    0x81
#define UVC_GET_MIN                    0x82
#define UVC_GET_MAX                    0x83
#define UVC_GET_RES                    0x84
#define UVC_GET_LEN                    0x85
#define UVC_GET_INFO                   0x86
#define UVC_GET_DEF                    0x87

#define XU_WRITE_REG                    0x10
#define XU_READ_REG                     0x11
#define XU_STREAM_STA                   0x80
#define XU_GPIO_CTRL                    0x81
#define XU_QP_SET                       0x82
#define XU_DFU                          0x83
#define XU_SET_TIME                     0x84
#define XU_SENSOR_I2C                   0x85
#define XU_GPIO_READ                    0x86
#define XU_I2C_RW                       0x87
#define XU_AE_SET                       0x88

#define USER_CMD_DRV_CTRL               0xaa
#define USER_DEV_CTRL_OFF               0x01
#define USER_DEV_CTRL_ON                0x02

enum
{
    IsocDataMode_stream,
    IsocDataMode_Flash,
};

typedef union
{
    struct
    {
        unsigned int year   :8;
        unsigned int mon    :6;
        unsigned int day    :6;
        unsigned int hour   :6;
        unsigned int min    :6;
    } b;
    unsigned int d32;
} Reg_Version;

struct uvc_extern_data
{
    char buf[64];
    unsigned int size;
};

unsigned int calcCRC(unsigned char *buf, unsigned int size);
void int2Byte(uint8_t *buf, uint32_t val);

void uvc_extern_intr_ctrl(uint8_t *data, unsigned int data_size, unsigned int cs);
void uvc_extern_intr_proc(uint8_t req, uint8_t cs, struct uvc_request_data *resp);
void uvc_get_flash_data(uint8_t **data, uint32_t *data_size);
unsigned int uvc_get_flash_mode(void);
void uvc_set_flash_mode(int set);
/*
 *功能:加载flash中的sensor参数
 *addr: sensor参数在flash中的地址
 *sensor_param[OUT]: 用于存储sensor参数的缓存
 *param_len[OUT]: 得到的sensor参数长度
 *return: 0, 得到缓存且crc校验正确; -1, 发生错误
 */
int fh_uvc_ispara_load(unsigned int addr, char *sensor_param, int *param_len);

/*
 *功能:使能uvc flash升级功能
 */
void fh_uvc_flash_init(void);
#endif

