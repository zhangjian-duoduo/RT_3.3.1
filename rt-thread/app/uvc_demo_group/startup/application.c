#include "sample_common.h"
#include "rtconfig_app.h"

void user_main(void)
{
#if defined (NN_ENABLE_G0)
    int ret = 0;
    extern int fh_nna_module_init(FH_VOID);
    ret = fh_nna_module_init();
    if (ret)
    {
        printf("fh_nna_module_init failed %x !\n", ret);
    }
#endif
    printf("%s-%d\n", __func__, __LINE__);
#ifdef FH_APP_USING_HEX_PARA
    extern void fh_uvc_update_init(void);
    fh_uvc_update_init();
#endif

#ifdef APP_USB_DEVICE_UVC
    extern int uvc_init(void);
    uvc_init();
#endif

#ifdef APP_USB_DEVICE_UAC
    extern int uac_init(void);
    uac_init();
#endif

#if defined(APP_USB_DEVICE_CDC)
    extern int cdc_init(void);
    cdc_init();
#endif

#ifdef APP_USB_DEVICE_HID
    extern int hid_demo(void);
    hid_demo();
#endif

}


