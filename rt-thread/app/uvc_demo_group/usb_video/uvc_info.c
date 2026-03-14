#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "uvc_init.h"
#include "uvc_info.h"
#include "uvc_extern.h"
#include "usb_update.h"

#define INFO_XU_SIZE    (10)

static struct UvcInfo g_UvcInfoData;

/************************************************************************/
/*                                                                      */
/************************************************************************/

static int read_info_data(unsigned int flash_addr)
{
    int dataSize = sizeof(g_UvcInfoData);

    Flash_Base_Read(flash_addr, &g_UvcInfoData, dataSize);
    if (g_UvcInfoData.size ==  sizeof(g_UvcInfoData) - 8)
    {
        if (g_UvcInfoData.crc == calcCRC(((unsigned char *)&g_UvcInfoData)+8, dataSize-8))
        {
            printf("uvc_info_load_data ok, addr=%x\n", flash_addr);
            return 0;
        }
    }
    memset(&g_UvcInfoData, 0, dataSize);

    return -1;
}

struct UvcInfo *uvc_info_option(void)
{
    Flash_Base_Init();
    if (read_info_data(UVC_INFO_ADDR))
    {
        read_info_data(UVC_INFO_BAK_ADDR);
    }
    return &g_UvcInfoData;
}

static int check_flash_mem_data(int flash_addr, int mem_addr, int size)
{
    unsigned char *pData = (unsigned char *)malloc(size);
    unsigned char *pMem = (unsigned char *)mem_addr;

    if (pData == NULL)
    {
        printf("%s allocate info-check buf error\n", __func__);
        return -1;
    }

    Flash_Base_Read(flash_addr, pData, size);
    if (memcmp(pData, pMem, size) != 0)
    {
        printf("check flash and mem diffrent\n");
        free(pData);
        return -1;
    }

    free(pData);
    return 0;
}

unsigned int  uvc_info_save_data(void)
{
    int ret;
    int dataSize = sizeof(g_UvcInfoData);

    g_UvcInfoData.size = dataSize - 8;
    g_UvcInfoData.uvcinfo_change  = 1;
    g_UvcInfoData.crc  = calcCRC(((unsigned char *)&g_UvcInfoData)+8, dataSize-8);

    Flash_Base_Write(UVC_INFO_ADDR, &g_UvcInfoData, dataSize);

    ret = check_flash_mem_data(UVC_INFO_ADDR, (int)&g_UvcInfoData, dataSize);
    if (ret)
    {
        printf("uvc_info_save_data failed!, addr=%x\n", UVC_INFO_ADDR);
        return -1;
    }
    printf("uvc_info_save_data ok!, addr=%x\n", UVC_INFO_ADDR);
    Flash_Base_Write(UVC_INFO_BAK_ADDR, &g_UvcInfoData, dataSize);
    ret = check_flash_mem_data(UVC_INFO_BAK_ADDR, (int)&g_UvcInfoData, dataSize);
    if (ret)
    {
        printf("uvc_info_save_data failed!, addr=%x\n", UVC_INFO_BAK_ADDR);
        return -1;
    }
    printf("uvc_info_save_data ok!, addr=%x\n", UVC_INFO_BAK_ADDR);

    return 0;
}

void deal_xu_get_info(unsigned char xu_id, char *pData)
{
    char *pStrData = NULL;
    unsigned short data;

    switch (xu_id)
    {
    case XU_ID_Facture1:
    case XU_ID_Facture2:
    case XU_ID_Facture3:
        pStrData = g_UvcInfoData.info.infoVal.szFacture;
        memcpy(pData, pStrData+(xu_id-XU_ID_Facture1)*INFO_XU_SIZE, INFO_XU_SIZE);
        break;

    case XU_ID_Product1:
    case XU_ID_Product2:
    case XU_ID_Product3:
        pStrData = g_UvcInfoData.info.infoVal.szProduct;
        memcpy(pData, pStrData+(xu_id-XU_ID_Product1)*INFO_XU_SIZE, INFO_XU_SIZE);
        break;

    case XU_ID_Serial1:
    case XU_ID_Serial2:
    case XU_ID_Serial3:
        pStrData = g_UvcInfoData.info.infoVal.szSerial;
        memcpy(pData, pStrData+(xu_id-XU_ID_Serial1)*INFO_XU_SIZE, INFO_XU_SIZE);
        break;

    case XU_ID_VideoIntf1:
    case XU_ID_VideoIntf2:
    case XU_ID_VideoIntf3:
        pStrData = g_UvcInfoData.info.infoVal.szVideoIntf;
        memcpy(pData, pStrData+(xu_id-XU_ID_VideoIntf1)*INFO_XU_SIZE, INFO_XU_SIZE);
        break;

    case XU_ID_VidPidBcd:
        data = g_UvcInfoData.info.infoVal.vid;
        memcpy(pData, &data, 2);
        data = g_UvcInfoData.info.infoVal.pid;
        memcpy(pData+2, &data, 2);
        data = g_UvcInfoData.info.infoVal.bcd;
        memcpy(pData+4, &data, 2);
        break;

    case XU_ID_AUDIO_INTF:
        pStrData = g_UvcInfoData.info.infoVal.szMicIntf;
        memcpy(pData, pStrData, 3*INFO_XU_SIZE);
        break;
    }
}

void deal_xu_set_info(unsigned char xu_id, char *pData)
{
    char *pStrData = NULL;
    unsigned short data;

    switch (xu_id)
    {
    case XU_ID_Facture1:
    case XU_ID_Facture2:
    case XU_ID_Facture3:
        pStrData = g_UvcInfoData.info.infoVal.szFacture;
        memcpy(pStrData+(xu_id-XU_ID_Facture1)*INFO_XU_SIZE, pData, INFO_XU_SIZE);
        break;

    case XU_ID_Product1:
    case XU_ID_Product2:
    case XU_ID_Product3:
        pStrData = g_UvcInfoData.info.infoVal.szProduct;
        memcpy(pStrData+(xu_id-XU_ID_Product1)*INFO_XU_SIZE, pData, INFO_XU_SIZE);
        break;

    case XU_ID_Serial1:
    case XU_ID_Serial2:
    case XU_ID_Serial3:
        pStrData = g_UvcInfoData.info.infoVal.szSerial;
        memcpy(pStrData+(xu_id-XU_ID_Serial1)*INFO_XU_SIZE, pData, INFO_XU_SIZE);
        break;

    case XU_ID_VideoIntf1:
    case XU_ID_VideoIntf2:
    case XU_ID_VideoIntf3:
        pStrData = g_UvcInfoData.info.infoVal.szVideoIntf;
        memcpy(pStrData+(xu_id-XU_ID_VideoIntf1)*INFO_XU_SIZE, pData, INFO_XU_SIZE);
        break;

    case XU_ID_VidPidBcd:
        memcpy(&data, pData, 2);
        g_UvcInfoData.info.infoVal.vid = data;

        memcpy(&data, pData+2, 2);
        g_UvcInfoData.info.infoVal.pid = data;

        memcpy(&data, pData+4, 2);
        g_UvcInfoData.info.infoVal.bcd = data;
        break;

    case XU_ID_AUDIO_INTF:
        pStrData = g_UvcInfoData.info.infoVal.szMicIntf;
        memcpy(pStrData, pData, 3*INFO_XU_SIZE);
        break;
    }
}

