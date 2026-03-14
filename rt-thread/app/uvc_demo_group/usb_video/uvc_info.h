#ifndef __UVC_INFO_H__
#define __UVC_INFO_H__



enum
{
    XU_ID_INFO_BEGIN = 2,
    XU_ID_Facture1 = 3,
    XU_ID_Facture2,
    XU_ID_Facture3,
    XU_ID_Product1,
    XU_ID_Product2,
    XU_ID_Product3,
    XU_ID_Serial1,
    XU_ID_Serial2,
    XU_ID_Serial3,
    XU_ID_VideoIntf1,
    XU_ID_VideoIntf2,
    XU_ID_VideoIntf3,
    XU_ID_VidPidBcd,
    XU_ID_INFO_END,
    XU_ID_AUDIO_INTF    = 0x16,
};

struct UvcInfo *uvc_info_option(void);
unsigned int uvc_info_save_data(void);
void deal_xu_set_info(unsigned char xu_id, char *pData);
void deal_xu_get_info(unsigned char xu_id, char *pData);

#endif

