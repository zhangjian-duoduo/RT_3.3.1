#ifndef __usb_update_h__
#define __usb_update_h__

#define ISP_PARAM_ADDR 0xb0000
#define UPDATE_CFG_ADDR 0x60000
#define FIRMWARE_VERSION_ADDR 0xAFFF0
#define FIRMWARE_VERSION_ADDR 0xAFFF0
#define FLASH_ADDR_UVC_CONFIG_DATA  0xc0000
#define UVC_INFO_ADDR       (FLASH_ADDR_UVC_CONFIG_DATA + 0x4000)
#define UVC_INFO_BAK_ADDR   (FLASH_ADDR_UVC_CONFIG_DATA + 0x6000)
#define ISP_USER_PARAM_ADDR (FLASH_ADDR_UVC_CONFIG_DATA + 4*1024)

#define FH_ISP_PARA_SIZE    (0x00006000)

enum
{
    UVC_UPDATE_WAIT,
    UVC_UPDATE_UDISK,
    UVC_UPDATE_VCOM,
    UVC_UPDATE_DFUA,
    UVC_UPDATE_DFUP,
    UVC_UPDATE_HID,
};

struct update_cmd
{
    uint8_t header;
    uint8_t resv;
    uint8_t len;
    uint8_t crc;
    uint8_t data[32];
};

#define HID_CMD_HEADER 0x99
#define HID_CMD_UPDATE 0x88

int Flash_Base_Init(void);
unsigned int Flash_Base_Read(int offset, void *buffer, unsigned int size);
unsigned int Flash_Base_Write(int offset, void *buffer, unsigned int size);
void fh_uvc_update_init(void);
void makeCRCTable(unsigned int seed);
void usb_update_check(uint8_t *report, uint32_t size);

extern int update_flash;
extern unsigned int crc32_table_init;

#endif /*__usb_update_h__*/