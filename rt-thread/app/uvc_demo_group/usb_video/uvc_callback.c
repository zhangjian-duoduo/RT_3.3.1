#include "sample_common_dsp.h"
#include "sample_common_isp.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include "FHAdv_Isp_mpi_v3.h"
#include "isp/isp_api.h"
#include "uvc_init.h"
#include "uvc_extern.h"
#include "rtconfig_app.h"
#include "usb_video.h"
#include "usb_update.h"
#include <sys/prctl.h>

#define SENSOR_SLEEP_DELAY_S    20

static FH_SINT32 g_AE_mode = 0;
static FH_SINT32 g_AE_time_level = 0;
static FH_SINT32 g_AE_gain_level = 0;

static FH_SINT32 g_uspara_change = 0;
static FH_SINT32 g_uvc_uspara_save[USER_PARAM_SUPPORT_CNT];

static void sensor_sleep_delay(void)
{
    int i;

#ifdef UVC_DOUBLE_STREAM
    if (!fh_uvc_status_get(STREAM_ID1) && !fh_uvc_status_get(STREAM_ID2))
    {
        for (i = 0; i < (SENSOR_SLEEP_DELAY_S * 10); i++)
        {
            usleep(100000);
            if (fh_uvc_status_get(STREAM_ID1) || fh_uvc_status_get(STREAM_ID2))
                break;
        }
        pthread_mutex_lock(&mutex_sensor);
        if (i == (SENSOR_SLEEP_DELAY_S * 10) &&
            !fh_uvc_status_get(STREAM_ID1) &&
            !fh_uvc_status_get(STREAM_ID2))
        {
            ISP_sleep_mode(ISP_SENSOR_SLEEP);
        }
        pthread_mutex_unlock(&mutex_sensor);
    }
#else
    if (!fh_uvc_status_get(STREAM_ID1))
    {
        for (i = 0; i < (SENSOR_SLEEP_DELAY_S * 10); i++)
        {
            usleep(100000);
            if (fh_uvc_status_get(STREAM_ID1))
                break;
        }

        pthread_mutex_lock(&mutex_sensor);
        if (i == (SENSOR_SLEEP_DELAY_S * 10) && !fh_uvc_status_get(STREAM_ID1))
        {
            ISP_sleep_mode(ISP_SENSOR_SLEEP);
        }
        pthread_mutex_unlock(&mutex_sensor);
    }
#endif
}

int ir_stream_on = 0;
void uvc_stream_on(int stream_id)
{
    while (!g_uvc_dev[STREAM_ID1].isp_complete)
        usleep(10000);
    pthread_mutex_lock(&mutex_sensor);
    ISP_sleep_mode(ISP_SENSOR_WAKEUP);
    pthread_mutex_unlock(&mutex_sensor);
    printf("%s:stream_id %d\n", __func__, stream_id);
#ifdef UVC_SUPPORT_WINDOWS_HELLO_FACE
    if (stream_id == STREAM_ID2)
        ir_stream_on = 1;
#endif
}

void uvc_stream_off(int stream_id)
{
    g_uspara_change++;
#ifdef UVC_SUPPORT_WINDOWS_HELLO_FACE
    if (stream_id == STREAM_ID2)
        ir_stream_on = 0;
#endif
    printf("%s:stream_id %d\n", __func__, stream_id);
}

FH_VOID Uvc_SetAEMode(FH_UINT32 mode)
{
    unsigned char setflg = 0;

    if (mode == 4 || mode == 1)
    {
        if (g_AE_mode != 3)
        {
            g_AE_mode = 3;
            g_AE_time_level = 3;
            g_AE_gain_level = GAIN_DEF;/* defaut */
            setflg =  1;
        }
    } else
    {
        if (g_AE_mode != 0)
        {
            g_AE_mode = 0; /* auto Exposure */
            setflg =  1;
        }
    }
    if (setflg)
    {
        FHAdv_Isp_SetAEMode(FH_GRP_ID, g_AE_mode, g_AE_time_level, g_AE_gain_level);
        g_uvc_uspara_save[USER_PARAM_AEMODE] = mode;
        g_uspara_change++;
    }
}

/**
 * level:
 * 0:inttMax*8
 * 1:inttMax*4
 * 2:inttMax*2
 * 3:inttMax --< begin exp=2500
 * 4:inttMax/2 exp=1250
 * 5:inttMax/4 exp=625
 * 6:inttMax/6 exp=312
 * 7:inttMax/8 exp=156
 * 8:inttMax/10 exp=78
 * 9:inttMax/20 exp=39
 * 10:inttMax/30 exp=20
 * 11:inttMax/40  exp=10
 * 12:inttMax/50 --<end exp=5
 */
static FH_SINT32 ct_ae_exp_time2level(FH_UINT32 time)
{
    unsigned int i;
    unsigned int res;

    for (i = 0; i < 31; i++)
    {
        if ((time>>i) == 0)
        {
            break;
        }
    }
    res = 15 - i;
    return res;
}

FH_VOID Uvc_SetExposure(FH_UINT32 time_value)
{
    static FH_UINT32 last_time_v = 0;

    if (g_AE_mode == 3)
    {
        if (last_time_v == time_value)
            return;

        last_time_v = time_value;
        g_AE_time_level = ct_ae_exp_time2level(time_value);
        FHAdv_Isp_SetAEMode(FH_GRP_ID, g_AE_mode, g_AE_time_level, g_AE_gain_level);
        g_uvc_uspara_save[USER_PARAM_EXPOSURE] = time_value;
        g_uspara_change++;
    }
}

FH_VOID Uvc_SetGain(FH_UINT32 gain_level)
{
    static FH_UINT32 last_time_v = 0;

    if (g_AE_mode == 3)
    {
        if (last_time_v == gain_level)
            return;

        last_time_v = gain_level;
        g_AE_gain_level = gain_level;
        FHAdv_Isp_SetAEMode(FH_GRP_ID, g_AE_mode, g_AE_time_level, g_AE_gain_level);
        g_uvc_uspara_save[USER_PARAM_GAIN] = gain_level;
        g_uspara_change++;
    }
}

FH_VOID Uvc_SetBrightness(FH_UINT32 value)
{
    static int cur_Brightness = USER_PARAM_NO_CHANGE_FLAG;
    static FH_UINT32 last_time_v = 0;
    int set_Brightness = 0;

    if (last_time_v == value)
        return;

    last_time_v = value;
    if (cur_Brightness < 0)
        FHAdv_Isp_GetBrightness(FH_GRP_ID, (FH_UINT32 *)&cur_Brightness);

    set_Brightness = cur_Brightness + value - BIGHTNESS_DEF;
    if (set_Brightness <= BIGHTNESS_MIN)
        set_Brightness = BIGHTNESS_MIN;
    if (set_Brightness >= BIGHTNESS_MAX)
        set_Brightness = BIGHTNESS_MAX;
    FHAdv_Isp_SetBrightness(FH_GRP_ID, set_Brightness);
    g_uvc_uspara_save[USER_PARAM_BRIGHTNESS] = value;
    g_uspara_change++;
}

FH_VOID Uvc_SetContrast(FH_UINT32 value)
{
    static int cur_Contrast = USER_PARAM_NO_CHANGE_FLAG;
    static FH_UINT32 cur_mode = 0;
    int set_Contrast = 0;
    static FH_UINT32 last_time_v = 0;

    if (last_time_v == value)
        return;

    last_time_v = value;

    if (cur_Contrast < 0)
        FHAdv_Isp_GetContrast(FH_GRP_ID, &cur_mode, (FH_UINT32 *)&cur_Contrast);

    set_Contrast = cur_Contrast + value - CONTRAST_DEF;
    if (set_Contrast <= CONTRAST_MIN)
        set_Contrast = CONTRAST_MIN;
    if (set_Contrast >= CONTRAST_MAX)
        set_Contrast = CONTRAST_MAX;
    FHAdv_Isp_SetContrast(FH_GRP_ID, cur_mode, set_Contrast);
    g_uvc_uspara_save[USER_PARAM_CONTRAST] = value;
    g_uspara_change++;
}

FH_VOID Uvc_SetSaturation(FH_SINT32 value)
{
    static int cur_Saturation = USER_PARAM_NO_CHANGE_FLAG;
    static FH_UINT32 cur_mode = 0;
    int set_Saturation = 0;
    static FH_SINT32 last_time_v = 0;

    if (last_time_v == value)
        return;

    last_time_v = value;
    if (cur_Saturation < 0)
        FHAdv_Isp_GetSaturation(FH_GRP_ID, &cur_mode, (FH_UINT32 *)&cur_Saturation);

    set_Saturation = cur_Saturation + value - SATURATION_DEF;
    if (set_Saturation <= SATURATION_MIN)
        set_Saturation = SATURATION_MIN;
    if (set_Saturation >= SATURATION_MAX)
        set_Saturation = SATURATION_MAX;
    FHAdv_Isp_SetSaturation(FH_GRP_ID, cur_mode, set_Saturation);
    g_uvc_uspara_save[USER_PARAM_SATURATION] = value;
    g_uspara_change++;
}

FH_VOID Uvc_SetSharpeness(FH_SINT32 value)
{
    static int cur_Sharpeness = USER_PARAM_NO_CHANGE_FLAG;
    static FH_UINT32 cur_mode = 0;
    int set_Sharpeness = 0;
    static FH_SINT32 last_time_v = 0;

    if (last_time_v == value)
        return;

    last_time_v = value;
    if (cur_Sharpeness < (-SHARPENESS_MAX))
        FHAdv_Isp_GetSharpeness(FH_GRP_ID, &cur_mode, (FH_UINT32 *)&cur_Sharpeness);

    set_Sharpeness = cur_Sharpeness + 2 * (value - SHARPENESS_DEF);
    if (set_Sharpeness <= (-SHARPENESS_MAX))
        set_Sharpeness = (-SHARPENESS_MAX);
    if (set_Sharpeness >= SHARPENESS_MAX)
        set_Sharpeness = SHARPENESS_MAX;
    FHAdv_Isp_SetSharpeness(FH_GRP_ID, cur_mode, set_Sharpeness);
    g_uvc_uspara_save[USER_PARAM_SHARPENESS] = value;
    g_uspara_change++;
}

FH_VOID Uvc_SetAwbMode(FH_UINT32 value)
{
    static FH_UINT32 last_time_v = 0;

    if (last_time_v == value)
        return;

    last_time_v = value;
    /* TODO */
    g_uvc_uspara_save[USER_PARAM_AWBMODE] = value;
    g_uspara_change++;
}

FH_VOID Uvc_SetAWBGain(FH_UINT32 value)
{
    static int cur_awbgain = USER_PARAM_NO_CHANGE_FLAG;
    int set_awbgain;
    static FH_UINT32 cur_mode = 0;
    static FH_UINT32 last_time_v = 0;

    if (last_time_v == value)
        return;

    last_time_v = value;
    /* TODO : current just as example*/
    if (cur_awbgain < 0)
        FHAdv_Isp_GetContrast(FH_GRP_ID, &cur_mode, (FH_UINT32 *)&cur_awbgain);

    set_awbgain = cur_awbgain + value - AWBGAIN_DEF;
    if (set_awbgain <= AWBGAIN_MIN)
        set_awbgain = AWBGAIN_MIN;
    if (set_awbgain >= AWBGAIN_MAX)
        set_awbgain = AWBGAIN_MAX;
    /* FHAdv_Isp_SetContrast(FH_GRP_ID, cur_mode, set_awbgain); */
    g_uvc_uspara_save[USER_PARAM_AWBGAIN] = value;
    g_uspara_change++;
}

FH_VOID Uvc_SetGammaCfg(FH_UINT32 value)
{
    /* TODO : current just as example*/
    static int cur_gammacfg = USER_PARAM_NO_CHANGE_FLAG;
    static FH_UINT32 cur_mode = 0;
    int set_gammacfg = 0;
    static FH_UINT32 last_time_v = 0;

    if (last_time_v == value)
        return;

    last_time_v = value;
    if (cur_gammacfg < (-GAMMACFG_MAX))
        FHAdv_Isp_GetSharpeness(FH_GRP_ID, &cur_mode, (FH_UINT32 *)&cur_gammacfg); /* example */

    set_gammacfg = cur_gammacfg + 2 * (value - GAMMACFG_DEF);
    if (set_gammacfg <= (-GAMMACFG_MAX))
        set_gammacfg = (-GAMMACFG_MAX);
    if (set_gammacfg >= GAMMACFG_MAX)
        set_gammacfg = GAMMACFG_MAX;
    /* FHAdv_Isp_SetSharpeness(FH_GRP_ID, cur_mode, set_gammacfg); */ /* example */
    g_uvc_uspara_save[USER_PARAM_GAMMACFG] = value;
    g_uspara_change++;
}

#ifdef FH_USING_USER_PARAM
FH_SINT32 *uvc_para_read(FH_VOID)
{
    FH_UINT32 ret, i;
    FH_SINT32 para_size = sizeof(g_uvc_uspara_save);

    for (i = 0; i < USER_PARAM_SUPPORT_CNT; i++)
        g_uvc_uspara_save[i] = USER_PARAM_NO_CHANGE_FLAG;

    ret = fh_uvc_ispara_load(ISP_USER_PARAM_ADDR, (char *)g_uvc_uspara_save, &para_size);
    if (ret == 0)
        printf("uvc_para_read read 0x%x ok!\n", ISP_USER_PARAM_ADDR);

    return g_uvc_uspara_save;
}

FH_SINT32 uvc_para_write(FH_SINT32 *para)
{
    FH_UINT32 crc = 0;
    FH_UINT32 para_size = sizeof(g_uvc_uspara_save);
    FH_UINT8 user_param_buff[para_size + 16];
    FH_UINT32 ret;

    memcpy(user_param_buff + 16, para, para_size);
    crc = calcCRC((void *)user_param_buff + 16, para_size);
    int2Byte(user_param_buff, crc);
    int2Byte(user_param_buff + 4, 0xffffffff - crc);
    int2Byte(user_param_buff + 8, para_size);
    int2Byte(user_param_buff + 12, 0xffffffff - para_size);

    ret = Flash_Base_Write(ISP_USER_PARAM_ADDR, (void *)user_param_buff, para_size + 16);
    if (ret == 0)
    {
        printf("Error: uvc_para_write fail!!!\n");
        return -1;
    }
    return (ret == para_size + 16);
}

void *uvc_uspara_proc(void *arg)
{
    prctl(PR_SET_NAME, "uvc_uspara_proc");
    while (1)
    {
        if (g_uspara_change)
        {
            g_uspara_change = 0;
#ifdef FH_APP_USING_HEX_PARA
            uvc_para_write(g_uvc_uspara_save);
#endif
            sensor_sleep_delay();
            usleep(1000 * 100);
        } else
            usleep(1000 * 1000);
    }
    return NULL;
}

#endif
