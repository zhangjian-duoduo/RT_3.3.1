#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include "mqueue.h"
#include "isp/isp_api.h"
#include "rtconfig_app.h"
#include "errno.h"
#include "sys/reboot.h"
#include "uvc_extern.h"
#include "uvc_info.h"
#include "uvc_init.h"
#include "usb_update.h"
#include <sys/prctl.h>

#ifdef FH_APP_USING_HEX_PARA
static pthread_t g_thread_uvc_extern_intr;
#endif
/* ===================update========================= */
static pthread_t g_thread_uvc_uspara;
/* ============================================ */

/* ============get img_flash and set hex================== */
struct xu_recv_data
{
    unsigned int  type;
    unsigned int  size;
    unsigned int  crc;
    unsigned int  recvCnt;
    unsigned int  blkSize;
    unsigned int  flashAddr;
    unsigned int  writeResult;
    unsigned char *pData;
};

static unsigned int gIoscDataMode;
static unsigned int gFlashAddr;
static unsigned int gFlashSize;
static unsigned char *gUsbTempBuf = NULL;
static struct xu_recv_data gRecvData;

#ifdef FH_APP_USING_HEX_PARA
static unsigned char isp_param_buff_flash[FH_ISP_PARA_SIZE];
#endif

#define CRC_SEED    0xEDB88320
#define Data_Type_UvcCfg    0xc
#define Data_Type_Drv       0xd
#define Data_Type_Flash     0xf

#define USB_TEMP_BUF_SIZE   (68 * 1024)
#define BUF_FLASH_HEAD_SIZE (16)
/* ============================================ */
/* ============version and param================== */
static Reg_Version gUvcVersion;
/* ============================================ */

#define BYTE2INT(n) (*(n+0)|*(n+1)<<8|*(n+2)<<16|*(n+3)<<24)

void int2Byte(uint8_t *buf, uint32_t val)
{
    *buf = (val&0xff);
    *(buf+1) = (val>>8&0xff);
    *(buf+2) = (val>>16&0xff);
    *(buf+3) = (val>>24&0xff);
}

void GetFirmwareVersion(void)
{
    unsigned int buf[2];
    Flash_Base_Read(FIRMWARE_VERSION_ADDR, buf, 8);
    if (buf[0] == (buf[1] ^0xffffffff))
    {
        gUvcVersion.d32 = buf[0];
        printf("uvc version info: year=%d, mon=%d, day=%d, hour=%d, min=%d\n",
                   gUvcVersion.b.year,
                   gUvcVersion.b.mon,
                   gUvcVersion.b.day,
                   gUvcVersion.b.hour,
                   gUvcVersion.b.min);
    }
    else
    {
        printf("get version error, ver=0x%08x, chk=0x%08x\n", buf[0], buf[1]);
        gUvcVersion.d32 = 0;
    }
}

#ifdef FH_APP_USING_HEX_PARA
static int SaveIspPara(int addr)
{
    int ret;
    unsigned int crc = 0;
    ISP_PARAM_CONFIG isp_param_config;

    ret = API_ISP_GetBinAddr(FH_GRP_ID, &isp_param_config);
    if (ret)
    {
        printf("Error: API_ISP_GetBinAddr failed with %d\n", ret);
        return 0;
    }

    if (isp_param_config.u32BinSize <= (sizeof(isp_param_buff_flash) - 16))
    {
        memcpy(isp_param_buff_flash + 16, (void *)isp_param_config.u32BinAddr, isp_param_config.u32BinSize);
    }
    else
    {
        printf("Error: SaveIspPara copy para %d > %d\n", isp_param_config.u32BinSize, sizeof(isp_param_buff_flash));
        return 0;
    }

    crc = calcCRC((void *)isp_param_buff_flash + 16, isp_param_config.u32BinSize);
    int2Byte(isp_param_buff_flash, crc);
    int2Byte(isp_param_buff_flash+4, 0xffffffff - crc);
    int2Byte(isp_param_buff_flash+8, isp_param_config.u32BinSize);
    int2Byte(isp_param_buff_flash+12, 0xffffffff - isp_param_config.u32BinSize);

    ret = Flash_Base_Write(addr, (void *)isp_param_buff_flash, isp_param_config.u32BinSize + 16);
    if (ret == 0)
    {
        printf("Error: SaveIspPara fail!!!\n");
    }
    else
    {
        printf("\nSaveIspPara write =%d paralen=%d, crc=%x\n", ret, isp_param_config.u32BinSize, crc);
    }

    return (ret == isp_param_config.u32BinSize + 16);
}

int fh_uvc_ispara_load(unsigned int addr, char *sensor_param, int *param_len)
{
    uint32_t crc = 0;
    uint32_t crc_comp = 0;
    uint32_t para_len = 0;
    uint32_t para_len_cmp = 0;
    int len = 0;

    if (!crc32_table_init)
    {
        crc32_table_init = 1;
        makeCRCTable(CRC_SEED);
    }
    Flash_Base_Init();
    len = Flash_Base_Read(addr, isp_param_buff_flash, FH_ISP_PARA_SIZE);
    if (len <= 0)
    {
        printf("isp_load_para_flash read error !!! len = %d\n", len);
        return -1;
    }

    crc = BYTE2INT(isp_param_buff_flash);
    crc_comp = BYTE2INT(isp_param_buff_flash + 4);
    para_len = BYTE2INT(isp_param_buff_flash + 8);
    para_len_cmp = BYTE2INT(isp_param_buff_flash + 12);

    if (crc == 0xffffffff && crc_comp == 0xffffffff)
        return -1;
    if ((para_len + para_len_cmp) == 0xffffffff)
    {
        if (para_len <= FH_ISP_PARA_SIZE - 16
            && ((crc + crc_comp) == 0xffffffff)
            && crc == calcCRC(isp_param_buff_flash + 16, para_len))
        {
            memcpy(sensor_param, isp_param_buff_flash + 16, para_len);
            *param_len = para_len;
            return 0;
        }
        else
        {
            printf("isp_load_para_flash error! addr:%x read crc:%x len:%d\n",
                                addr, (unsigned int)crc, (int)para_len);
            return -1;
        }
    } else
    {
        return -1;
    }
}

static int uvc_isp_para_write(int addr, unsigned char *pData, unsigned int size)
{
    unsigned int ret;
    unsigned int saveSize;
    char *sensor_param;
    int param_len;
    ISP_PARAM_CONFIG isp_param_config;

    ret = API_ISP_GetBinAddr(FH_GRP_ID, &isp_param_config);
    if (ret)
    {
        printf("Error: API_ISP_GetBinAddr failed with %d\n", ret);
        return 0;
    }

    saveSize = isp_param_config.u32BinSize > size ? size : isp_param_config.u32BinSize;
    memcpy((void *)isp_param_config.u32BinAddr, pData, saveSize);
    if (!SaveIspPara(addr))
    {
        return 0;
    }

    sensor_param = malloc(FH_ISP_PARA_SIZE);
    if (!sensor_param)
    {
        free(sensor_param);
        return 0;
    }

    param_len = 0;
    fh_uvc_ispara_load(addr, sensor_param, &param_len);
    if (param_len)
    {
        ret = 1;
    }
    ret = API_ISP_LoadIspParam(FH_GRP_ID, sensor_param);
    if (ret)
    {
        printf("API_ISP_LoadIspParam failed with %d\n", ret);
        free(sensor_param);
        return ret;
    }
    free(sensor_param);
    printf("CheckSavePara, ret =%d\n", ret);
    return ret;
}

static void uvc_flash_write(struct xu_recv_data *para)
{
    struct xu_recv_data *pRecvData = para;

    if (pRecvData->type == Data_Type_Flash)/* TODO */
    {
        Flash_Base_Write(pRecvData->flashAddr, pRecvData->pData, pRecvData->size);
        printf("write flash over, addr = %x, size = %d\n", pRecvData->flashAddr, pRecvData->size);
        pRecvData->writeResult = Data_Type_Flash;
    }
    else if (pRecvData->type == Data_Type_Drv)
    {
        if (uvc_isp_para_write(ISP_PARAM_ADDR, pRecvData->pData, pRecvData->size))
        {
            printf("write Drv over, addr = %x, size = %d\n", pRecvData->flashAddr, pRecvData->size);
            pRecvData->writeResult = Data_Type_Drv;
        }
        free(pRecvData->pData);
        pRecvData->pData = NULL;
        gRecvData.pData = NULL;
    }
}
#endif

unsigned int uvc_get_flash_mode(void)
{
    return gIoscDataMode;
}

void uvc_set_flash_mode(int set)
{
    gIoscDataMode = set;
}

void uvc_get_flash_data(uint8_t **data, uint32_t *data_size)
{
    uint32_t chkData = 0;
    int idx = 0;

    memcpy(gUsbTempBuf, "FLSH", 4);
    memcpy(gUsbTempBuf + 4, &gFlashAddr, 4);
    memcpy(gUsbTempBuf + 8, &gFlashSize, 4);

    if (gFlashAddr != 0xffffffff)
    {
        Flash_Base_Read(gFlashAddr, gUsbTempBuf + BUF_FLASH_HEAD_SIZE, gFlashSize);
    }
    for (idx = 0; idx < gFlashSize; idx++)
    {
        chkData += gUsbTempBuf[BUF_FLASH_HEAD_SIZE + idx];
    }

    memcpy(gUsbTempBuf + 12, &chkData, 4);
    *data = gUsbTempBuf;
    *data_size = gFlashSize + BUF_FLASH_HEAD_SIZE;
}


static char Uvc_ExtData[64] = {0};
static char Uvc_ExtBuf[64] = {0};
static mqd_t uvc_extern_mq = NULL;
#ifdef FH_APP_USING_HEX_PARA
static void *uvc_extern_set(void *arg)
{
    struct uvc_extern_data ext_data;
    struct mq_attr attr;
    char data[64] = {0};

    prctl(PR_SET_NAME, "uvc_extern_set");
    attr.mq_msgsize = sizeof(struct uvc_extern_data);
    attr.mq_maxmsg = 10;
    uvc_extern_mq = NULL;
    uvc_extern_mq = mq_open("uvc_extern_mq", O_RDWR | O_CREAT | O_EXCL, 0x644, &attr);
    if (uvc_extern_mq == NULL)
        printf("Error: Create uvc_extern_mq failed!\n");
    while (1)
    {
        if (mq_receive(uvc_extern_mq, (char *)&ext_data, sizeof(ext_data), NULL) > 0)
        {
            memcpy(data, ext_data.buf, ext_data.size);
            switch (data[0])
            {
            /* ================================save uvc info=============================*/
            case 0xa0:
                uvc_info_save_data();
                break;
            /* ================================save uvc info=============================*/
            /* ====================================reset===================================== */
            case 0xaa:/* 0xaa 0x02 reset */
                if (data[1] == 0x02)
                    reboot(0x01234567);
                break;
            /* ====================================reset===================================== */
            /* ================================udisk uptate================================== */
            case 'R':
                if (memcmp(data, "RESET", 5) == 0)
                {
                    update_flash = UVC_UPDATE_UDISK;
                }
                break;
            /* ================================udisk uptate================================== */
            /* ================================VCOM uptate================================== */
            case 'C':
                if (memcmp(data, "COMUPDATE", 9) == 0)
                {
                    update_flash = UVC_UPDATE_VCOM;
                }
                break;
            /* ================================VCOM uptate================================== */
            /* ================================HID uptate================================== */
            case 'H':
                if (data[1] == 'U')
                {
                    update_flash = UVC_UPDATE_HID;
                }
                break;
            /* ================================HID uptate================================== */
            /* ================================DFU uptate================================== */
            case XU_DFU:
                if (data[1] == 0x11)
                {
                    update_flash = UVC_UPDATE_DFUA;
                }
                else
                {
                    update_flash = UVC_UPDATE_DFUP;
                }
                break;
            /* ================================DFU uptate================================== */
            /* ================================out image==================================== */
            case 'F':
                if (data[1] == 'R')
                {
                    memcpy(&gFlashAddr, data + 2, 4);
                    memcpy(&gFlashSize, data + 6, 4);
                    if (gFlashAddr < 0xffffff)
                    {
                        if (!gUsbTempBuf)
                        {
                            gUsbTempBuf = malloc(USB_TEMP_BUF_SIZE);
                            if (!gUsbTempBuf)
                                printf("flash_img buf alloc failed!\n");
                        }
                        if (gFlashSize > USB_TEMP_BUF_SIZE - BUF_FLASH_HEAD_SIZE)
                            gFlashSize = USB_TEMP_BUF_SIZE - BUF_FLASH_HEAD_SIZE;
                        gIoscDataMode = IsocDataMode_Flash;
                    } else
                    {
                        gIoscDataMode = IsocDataMode_stream;
                    }
                    printf("Read flash, addr=%x, size=%x, mode = %d\n", gFlashAddr, gFlashSize, gIoscDataMode);
                } else if (data[1] == 'E')
                {
                    free(gUsbTempBuf);
                    gUsbTempBuf = NULL;
                    gIoscDataMode = IsocDataMode_stream;
                }
                break;
            /* ================================out image==================================== */
            /* ================================update hex==================================== */
            case 0xda:
                /* header */
                memcpy(&gRecvData.flashAddr, data + 1, 4);
                printf("update hex flashaddr = %x!\n", gRecvData.flashAddr);
                break;
            case 0xdb:
                /* data begin */
                gRecvData.type = data[1];
                memcpy(&gRecvData.size, data + 2, 4);
                memcpy(&gRecvData.crc, data + 6, 4);
                if (gRecvData.pData)
                {
                    free(gRecvData.pData);
                    gRecvData.pData = NULL;
                }
                gRecvData.pData = malloc(gRecvData.size);
                if (!gRecvData.pData)
                    printf("param data buf alloc failed!\n");
                printf("update hex size = %x, crc = %x!\n", gRecvData.size, gRecvData.crc);
                gRecvData.recvCnt = 0;
                break;
            case 0xdd:
                /* data sending */
                gRecvData.blkSize = ext_data.size - 1;
                if (gRecvData.recvCnt + gRecvData.blkSize < gRecvData.size)
                {
                    memcpy(gRecvData.pData + gRecvData.recvCnt, data + 1, gRecvData.blkSize);
                    gRecvData.recvCnt += gRecvData.blkSize;
                } else
                {
                    memcpy(gRecvData.pData + gRecvData.recvCnt, data+1, gRecvData.size - gRecvData.recvCnt);
                    gRecvData.recvCnt = gRecvData.size;
                }
                break;
            case 0xde:
                /* data over */
                if (gRecvData.recvCnt == gRecvData.size)
                {
                    if (gRecvData.crc == calcCRC(gRecvData.pData, gRecvData.size))
                    {
                        printf("recv over, check crc ok\n");
                        uvc_flash_write(&gRecvData);
                    } else
                    {
                        printf("update hex oricrc = %x, datacrc = %x!\n", gRecvData.crc, calcCRC(gRecvData.pData, gRecvData.size));
                        printf("recv over, check crc error!\n");
                        free(gRecvData.pData);
                        gRecvData.pData = NULL;
                    }
                } else
                {
                    printf("recv over, size =%d, error!\n", gRecvData.recvCnt);
                    free(gRecvData.pData);
                    gRecvData.pData = NULL;
                }
            /* ================================update hex==================================== */
            }
        } else
        {
            printf("uvc mq receive failed!\n");
        }

    }

    return NULL;
}
#endif

void uvc_extern_intr_ctrl(uint8_t *data, unsigned int data_size, unsigned int cs)
{
    struct uvc_extern_data ext_data;

    memcpy(Uvc_ExtData, data, data_size);

    if ((cs > XU_ID_INFO_BEGIN && cs < XU_ID_INFO_END) || cs == XU_ID_AUDIO_INTF)
    {
        deal_xu_set_info((unsigned char)cs, Uvc_ExtData);
        return;
    }
    if (uvc_extern_mq)
    {
        memcpy(ext_data.buf, data, data_size);
        ext_data.size = data_size;
        if (mq_send(uvc_extern_mq, (const char *)&ext_data, sizeof(ext_data), 0))
            rt_kprintf("uvc uvc_extern_mq send failed\n");
    }
    return;
}

void uvc_extern_intr_proc(uint8_t req, uint8_t cs, struct uvc_request_data *resp)
{
    unsigned int len = EXTERN_DATA_LEN;

    if (cs == XU_ID_AUDIO_INTF)
        len = 0x20;

    resp->length = len;
    switch (req)
    {
    case UVC_GET_LEN:
        resp->length = 2;
        resp->data[0] = len & 0xff;
        resp->data[1] = len >> 8;
        break;

    case UVC_GET_CUR:
        /**
         * if the value could be changed only in data phase,
         * give the cur value directly
         */
        memset(Uvc_ExtBuf, 0, len);
        if ((cs > XU_ID_INFO_BEGIN && cs < XU_ID_INFO_END) || cs == XU_ID_AUDIO_INTF)
        {
            deal_xu_get_info(cs, Uvc_ExtBuf);
        } else
        {
            switch (Uvc_ExtData[0])
            {
            case 0xdf:
                Uvc_ExtBuf[0] = 0x0d;
                break;
            case 'V':
                memcpy(Uvc_ExtBuf, &gUvcVersion, sizeof(gUvcVersion));
                break;
            case 'U':
                strcpy(Uvc_ExtBuf, "Uvc-Cam");
                break;
            default:
                strcpy(Uvc_ExtBuf, "Fullhan");
            }
        }
        memset(Uvc_ExtData, 0, len);
        memcpy(resp->data, Uvc_ExtBuf, len);
        break;
    case UVC_GET_MIN:
        /* TODO diff type */
        memset(resp->data, 0x0, len);
        break;
    case UVC_GET_MAX:
        memset(resp->data, 0xff, len);
        break;
    case UVC_GET_DEF:
        memset(resp->data, 0x00, len);
        break;
    case UVC_GET_RES:
        memset(resp->data, 0x1, len);
        break;
    case UVC_GET_INFO:
        resp->length = 0x01;
        resp->data[0] = 0x03;
        break;
    default:
        break;
    }
}

void fh_uvc_flash_init(void)
{
    struct sched_param param;
    pthread_attr_t attr;

    Flash_Base_Init();
    gRecvData.pData = NULL;

#ifdef FH_APP_USING_HEX_PARA
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize(&attr, 4*1024);
    param.sched_priority = 30;
    pthread_attr_setschedparam(&attr, &param);
    if (pthread_create(&g_thread_uvc_extern_intr, &attr, uvc_extern_set, NULL) != 0)
    {
        printf("Error: Create uvc_extern_ctrl thread failed!\n");
    }
#endif
#ifdef FH_USING_USER_PARAM
    extern void *uvc_uspara_proc(void *arg);

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize(&attr, 4*1024);
    param.sched_priority = 130; /* should sched_priority >= 29 */
    pthread_attr_setschedparam(&attr, &param);
    if (pthread_create(&g_thread_uvc_uspara, &attr, uvc_uspara_proc, NULL) != 0)
    {
        printf("Error: Create uvc_uspara_proc thread failed!\n");
    }
#endif
    GetFirmwareVersion();
}
