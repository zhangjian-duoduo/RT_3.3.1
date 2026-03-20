
#include "dsp/fh_vpu_mpi.h"
#include "isp/isp_common.h"
#include "isp/isp_api.h"
#include "isp/isp_enum.h"
#ifdef CONFIG_ARCH_FH8862
#include "vicap/fh_vicap_mpi.h"
#endif
#include "sample_common.h"
#include "_sensor_.h"
#include "isp_format.h"
#include "isp_sensor.h"
#include "isp_channel.h"
#if defined(FH_APP_USING_HEX_PARA) && defined(APP_USB_DEVICE_UVC)
#include "uvc_extern.h"
#include "usb_update.h"
#endif

#define ISP_PARAM_ADDR_BAK  (ISP_PARAM_ADDR + 8*1024)

FH_SINT32 isp_get_wdr_mode(FH_SINT32 format)
{
    FH_SINT32 wdr_mode;
    wdr_mode = (format & 0x10000) >> 16;
    return wdr_mode;
}
FH_SINT32 sample_common_get_isp_info(FH_UINT32 grpid, struct dev_isp_info *info)
{
    *info = g_isp_info[grpid];
    return 0;
}
FH_SINT32 sample_isp_use_large_frame(FH_UINT32 grpid)
{
    if (g_isp_info[grpid].isp_init_width > 1920 || g_isp_info[grpid].isp_init_height > 1080)
    {
        return 1;
    }

    return 0;
}

FH_UINT32 sample_isp_get_vi_w(FH_UINT32 grpid)
{
    return g_isp_info[grpid].isp_init_width;
}

FH_UINT32 sample_isp_get_vi_h(FH_UINT32 grpid)
{
    return g_isp_info[grpid].isp_init_height;
}

FH_UINT32 sample_isp_get_max_w(FH_UINT32 grpid)
{
    return g_isp_info[grpid].isp_max_width;
}

FH_UINT32 sample_isp_get_max_h(FH_UINT32 grpid)
{
    return g_isp_info[grpid].isp_max_height;
}

FH_VOID isp_vpu_reconfig(FH_UINT32 grpid)
{
    FH_VPU_SIZE vpu_size;
    ISP_VI_ATTR_S isp_vi;

    API_ISP_Pause(grpid);
    API_ISP_Resume(grpid);
	
#if defined RT_USING_HS_CUSTOM_8852V201_GC2083_DZ_20230619
	//API_ISP_SetMirrorAndflip(0, 1, 0);
		
#endif

    FH_VPSS_GetViAttr(grpid, &vpu_size);
    API_ISP_GetViAttr(grpid, &isp_vi);
    if (vpu_size.vi_size.u32Width != isp_vi.u16PicWidth ||
            vpu_size.vi_size.u32Height != isp_vi.u16PicHeight)
    {
        vpu_size.vi_size.u32Width  = isp_vi.u16PicWidth;
        vpu_size.vi_size.u32Height = isp_vi.u16PicHeight;
        FH_VPSS_SetViAttr(grpid, &vpu_size);
    }

    API_ISP_SensorKick(grpid);
}

static FH_SINT32 choose_i2c(FH_UINT32 grpid)
{
#if defined(CONFIG_ARCH_FH8862)
    switch (grpid)
    {
    case 0:
        return 0;
    case 1:
        return 3;
    case 2:
        return 4;
    default:
        break;
    }
#else
    switch (grpid)
    {
    case 0:
        return 0;
    case 1:
        return 0;
    default:
        break;
    }
#endif
    return 0;
}

static FH_SINT32 sample_isp_init(FH_UINT32 grpid)
{
    FH_SINT32 flag = SAMPLE_SENSOR_FLAG_NORMAL;
    FH_SINT32 ret;
    FH_SINT32 hex_file_len;
    FH_CHAR   *param = NULL;

    ISP_MEM_INIT stMemInit = {0};
    Sensor_Init_t initConf = {0};
    ISP_VI_ATTR_S sensor_vi_attr = {0};

#if defined(CONFIG_ARCH_FH8862)
#ifdef CONFIG_ISP_OFFLINE_MODE
    FH_BIND_INFO src, dst;

    if (isp_get_wdr_mode(g_isp_info[grpid].isp_format))
        stMemInit.enOfflineWorkMode = ISP_OFFLINE_MODE_WDR;
    else
        stMemInit.enOfflineWorkMode = ISP_OFFLINE_MODE_LINEAR;
#else
    stMemInit.enOfflineWorkMode = ISP_OFFLINE_MODE_DISABLE;
#endif
    stMemInit.enIspOutMode = ISP_OUT_TO_VPU;
#else
    stMemInit.enIspOutMode = ISP_OUT_TO_VPU;
#endif

    stMemInit.stPicConf.u32Width  = g_isp_info[grpid].isp_max_width;
    stMemInit.stPicConf.u32Height = g_isp_info[grpid].isp_max_height;
    ret = API_ISP_MemInit(grpid, &stMemInit);
    if (ret)
    {
        printf("Error(%d - %x): API_ISP_MemInit (grpid):(%d)!\n", ret, ret, grpid);
        return ret;
    }

    g_isp_info[grpid].sensor = start_sensor(g_isp_info[grpid].sensor_name, grpid);
    if (!g_isp_info[grpid].sensor)
    {
        printf("Error(%d - %x): start_sensor (grpid):(%d)!\n", ret, ret, grpid);
        return -1;
    }

    ret = API_ISP_SensorRegCb(grpid, 0, g_isp_info[grpid].sensor);
    if (ret)
    {
        printf("Error(%d - %x): API_ISP_SensorRegCb (grpid):(%d)!\n", ret, ret, grpid);
        return ret;
    }

    initConf.u8CsiDeviceId = grpid;
    initConf.u8CciDeviceId = choose_i2c(grpid);
    initConf.bGrpSync = FH_FALSE;
    ret = API_ISP_SensorInit(grpid, &initConf);
    if (ret)
    {
        printf("Error(%d - %x): API_ISP_SensorInit (grpid):(%d)!\n", ret, ret, grpid);
        return ret;
    }

#ifdef EMU
    ret = API_ISP_SetSensorFmt(grpid, g_isp_info[grpid].isp_format);
    if (ret)
    {
        printf("Error(%d - %x): API_ISP_SetSensorFmt (grpid):(%d)!\n", ret, ret, grpid);
        return ret;
    }
    sensor_vi_attr.u16InputHeight = g_isp_info[grpid].isp_max_height;
    sensor_vi_attr.u16InputWidth = g_isp_info[grpid].isp_max_width;
    sensor_vi_attr.u16PicHeight = g_isp_info[grpid].isp_init_height;
    sensor_vi_attr.u16PicWidth = g_isp_info[grpid].isp_init_width;
    sensor_vi_attr.u16OffsetX = 0;
    sensor_vi_attr.u16OffsetY = 0;
    sensor_vi_attr.enBayerType = BAYER_GRBG;
    ret = API_ISP_SetViAttr(grpid, &sensor_vi_attr);
    if (ret)
    {
        printf("Error(%d - %x): API_ISP_SetViAttr (grpid):(%d)!\n", ret, ret, grpid);
        return ret;
    }
#else
    ret = API_ISP_SetSensorFmt(grpid, g_isp_info[grpid].isp_format);
        if (ret)
        {
        printf("Error(%d - %x): API_ISP_SetSensorFmt (grpid):(%d)!\n", ret, ret, grpid);
            return ret;
    }
    #endif


    ret = API_ISP_SensorKick(grpid);
    if (ret)
    {
        printf("Error(%d - %x): API_ISP_SensorKick (grpid):(%d)!\n", ret, ret, grpid);
        return ret;
    }

    /* 初始化ISP硬件寄存器 */
    ret = API_ISP_Init(grpid);
    if (ret)
    {
        printf("Error(%d - %x): API_ISP_Init (grpid):(%d)!\n", ret, ret, grpid);
        return ret;
    }

    /* 3 根据实际使用，配置vicap是离线模式还是在线模式 */
    ret = API_ISP_GetViAttr(grpid, &sensor_vi_attr);
    if (ret)
    {
        printf("Error(%d - %x): API_ISP_GetViAttr (grpid):(%d)!\n", ret, ret, grpid);
        return ret;
    }

#if defined(CONFIG_ARCH_FH8862)
    FH_VICAP_DEV_ATTR_S stViDev;
    FH_VICAP_VI_ATTR_S stViAttr;

    /* 初始化vi设备 */
#ifdef CONFIG_ISP_OFFLINE_MODE
    stViAttr.enWorkMode = VICAP_WORK_MODE_OFFLINE;
    stViDev.enWorkMode = VICAP_WORK_MODE_OFFLINE;
#else
    stViAttr.enWorkMode = VICAP_WORK_MODE_ONLINE;
    stViDev.enWorkMode = VICAP_WORK_MODE_ONLINE;
#endif
    stViDev.stSize.u16Width = g_isp_info[grpid].isp_init_width;
    stViDev.stSize.u16Height = g_isp_info[grpid].isp_init_height;
    stViDev.bUsingVb         = 0;
    FH_VICAP_InitViDev(grpid, &stViDev);
    if (ret)
    {
        printf("Error(%d - %x): FH_VICAP_InitViDev (grpid):(%d)!\n", ret, ret, grpid);
        return ret;
    }

    /* 配置vi attr */
    stViAttr.stInSize.u16Width = sensor_vi_attr.u16InputWidth;
    stViAttr.stInSize.u16Height = sensor_vi_attr.u16InputHeight;
    stViAttr.stCropSize.bCutEnable = 1;
    stViAttr.stCropSize.stRect.u16Width = sensor_vi_attr.u16PicWidth;
    stViAttr.stCropSize.stRect.u16Height = sensor_vi_attr.u16PicHeight;
    stViAttr.stCropSize.stRect.u16X = sensor_vi_attr.u16OffsetX;
    stViAttr.stCropSize.stRect.u16Y = sensor_vi_attr.u16OffsetY;
    stViAttr.stOfflineCfg.u8Priority = 0;
    stViAttr.u8WdrMode = isp_get_wdr_mode(g_isp_info[grpid].isp_format);
    stViAttr.enBayerType = sensor_vi_attr.enBayerType;

    ret = FH_VICAP_SetViAttr(grpid, &stViAttr);
    if (ret)
    {
        printf("Error(%d - %x): FH_VICAP_SetViAttr (grpid):(%d)!\n", ret, ret, grpid);
        return ret;
    }

#ifdef CONFIG_ISP_OFFLINE_MODE
    src.obj_id = FH_OBJ_VICAP;
    src.dev_id = 0;
    src.chn_id = grpid;
    dst.obj_id = FH_OBJ_ISP;
    dst.dev_id = grpid;
    dst.chn_id = 0;

    ret = FH_SYS_Bind(src, dst);
    if (ret)
    {
        printf("Error(%d - %x): FH_SYS_BindVicap2Isp (grpid):(%d)!\n", ret, ret, grpid);
        return -1;
    }
#endif
#endif

    int flash_para_load = 0;
#if defined(FH_APP_USING_HEX_PARA) && defined(APP_USB_DEVICE_UVC)
    char *sensor_param;
    int param_len = FH_ISP_PARA_SIZE;
    sensor_param = malloc(FH_ISP_PARA_SIZE);
    if (!sensor_param)
        printf("%s--%d sensor hex buf alloc failed!\n", __func__, __LINE__);
    else
    {
        ret = fh_uvc_ispara_load(ISP_PARAM_ADDR, sensor_param, &param_len);
        if (ret)
        {
            ret = fh_uvc_ispara_load(ISP_PARAM_ADDR_BAK, sensor_param, &param_len);
            if (ret == 0)
            {
                flash_para_load = 1;
                printf("ISP use backup param!!!\n");
            }
            else
            {
                flash_para_load = 0;
                printf("ISP param Load from Flash fail!!!\n");
            }
        } else
        {
            printf("ISP use flash param!!!\n");
            flash_para_load = 1;
        }
        if (flash_para_load)
        {
            ret = API_ISP_LoadIspParam(grpid, sensor_param);
            if (ret)
            {
                printf("API_ISP_LoadIspParam failed with %d\n", ret);
                return ret;
            }
        }
        free(sensor_param);
    }

#endif

    if (flash_para_load == 0)
    {
        if ((g_isp_info[grpid].isp_format >> 16) & 1)
            flag |= SAMPLE_SENSOR_FLAG_WDR;

        param = get_isp_sensor_param(grpid, (FH_CHAR *)g_isp_info[grpid].sensor->name, flag, &hex_file_len);
        if (param)
        {
            if (1 /* hex_file_len == param_size */)
            {
                ret = API_ISP_LoadIspParam(grpid, param);
                if (ret)
                {
                    printf("Error(%d - %x): API_ISP_LoadIspParam (grpid):(%d)!\n", ret, ret, grpid);
                }
            }
            else
            {
                printf("Error: Invalid sensor hex file length!\n");
            }
            free_isp_sensor_param(param);
        }
        else
        {
            printf("Error: Cann't load sensor hex file!\n");
        }
    }


        /* 启动sensor输出 */

    return 0;
}


static FH_BOOL g_sleep_isp = FH_TRUE;
FH_VOID *sample_isp_proc(FH_VOID *arg)
{
    struct dev_isp_info *isp_info = (struct dev_isp_info *)arg;
    char name[20];

    sprintf(name, "demo_isp%d", isp_info->channel);
    prctl(PR_SET_NAME, name);
    isp_info->running = 1;

    while (!isp_info->bStop)
    {
        if (g_sleep_isp == FH_TRUE)
        {
            API_ISP_Run(isp_info->channel);
        }


        usleep(10000);
    }

    isp_info->bStop = 0;
    isp_info->running = 0;
    return NULL;
}

FH_SINT32 sample_common_start_isp(FH_VOID)
{
    FH_UINT32 ret;
    FH_UINT32 grpid;

#if defined(CONFIG_ARCH_FH8862)
    FH_VICAP_STITCH_GRP_ATTR_S stStitchConf = {0};


    FH_VICAP_SetStitchGrpAttr(&stStitchConf);
#endif

    for (grpid = 0; grpid < MAX_GRP_NUM; grpid++)
    {
        if (grpid != FH_GRP_ID)
            continue;
        if (g_isp_info[grpid].enable)
        {
            ret = sample_isp_init(grpid);
            if (ret)
            {
                printf("Error(%d - %x): sample_isp_init (grp):(%d)!\n", ret, ret, grpid);
                return -1;
            }
        }
    }

    return 0;
}

FH_SINT32 sample_common_isp_run(FH_VOID)
{
    FH_UINT32 ret;
    pthread_t isp_thread[MAX_GRP_NUM];
    pthread_attr_t attr[MAX_GRP_NUM];
    struct sched_param param[MAX_GRP_NUM];
    FH_UINT32 grpid;

    for (grpid = 0; grpid < MAX_GRP_NUM; grpid++)
    {
        if (g_isp_info[grpid].enable)
        {
            g_isp_info[grpid].bStop = 0;

            pthread_attr_init(&attr[grpid]);
            pthread_attr_setdetachstate(&attr[grpid], PTHREAD_CREATE_DETACHED);
            pthread_attr_setstacksize(&attr[grpid], 30 * 1024);

            param[grpid].sched_priority = 130;

            pthread_attr_setschedparam(&attr[grpid], &param[grpid]);
            ret = pthread_create(&isp_thread[grpid], &attr[grpid], sample_isp_proc, &g_isp_info[grpid]);
            if (ret != 0)
            {
                printf("Error: create ISP%d thread failed!\n", grpid);
                return -1;
            }
        }
    }

    return ret;
}

FH_SINT32 sample_common_isp_stop(FH_VOID)
{
    FH_UINT32 grpid;

    for (grpid = 0; grpid < MAX_GRP_NUM; grpid++)
    {
        if (g_isp_info[grpid].enable)
        {
            if (g_isp_info[grpid].bStop == 0)
            {
                g_isp_info[grpid].bStop = 1;
                while (g_isp_info[grpid].running)
                {
                    usleep(20 * 1000);
                }


#if defined(CONFIG_ARCH_FH8862)
                FH_VICAP_Exit(grpid);
#endif
                API_ISP_Exit(grpid);
            }
        }
    }

    return 0;
}

#include "dbi_over_tcp.h"
#include "dbi_over_udp.h"
#include "dbi_over_uart.h"

static struct dbi_tcp_config g_tcp_conf;
static struct dbi_uart_config g_uart_conf;
static volatile int g_coolview_is_running;
static volatile int g_stop_coolview;

FH_SINT32 sample_common_start_coolview(FH_VOID *arg)
{
    pthread_attr_t attr;
    struct sched_param param;

    if (g_coolview_is_running)
    {
        printf("Error: coolview is already running!\n");
        return -1;
    }

    g_stop_coolview = 0;
    g_tcp_conf.cancel = (int *)&g_stop_coolview;
    g_tcp_conf.port = 8888;

    g_uart_conf.index = 4;
    g_uart_conf.cancel = (int *)&g_stop_coolview;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize(&attr, 4*1024);
    param.sched_priority = 30;
    pthread_attr_setschedparam(&attr, &param);

#ifdef FH_APP_TCP_COOLVIEW
    pthread_t thread_dbg;
    if (pthread_create(&thread_dbg, &attr, (void *)tcp_dbi_thread, (void *)&g_tcp_conf) != 0)
    {
        printf("Error: Create tcp_dbi_thread thread failed!\n");
        return -1;
    }
    g_coolview_is_running = 1;
#endif
#ifdef FH_APP_CDC_COOLVIEW
    pthread_t thread_dbg;
    if (pthread_create(&thread_dbg, &attr, (void *)uart_dbi_thread, (void *)&g_uart_conf) != 0)
    {
        printf("Error: Create uart_dbi_thread thread failed!\n");
        return -1;
    }
    g_coolview_is_running = 1;
#endif
    return 0;
}

FH_SINT32 sample_common_stop_coolview(FH_VOID)
{
    if (g_coolview_is_running)
    {
        /*wait for coolview thread to exit*/
        g_stop_coolview = 1;
        g_coolview_is_running = 0;
    }

    return 0;
}


FH_UINT32 get_fmt_by_fps(int fps)
{
    FH_SINT32 g_isp_format = g_isp_info[FH_GRP_ID].isp_format;
    FH_UINT32 fmt = g_isp_format;

    if (fps == 25)
    {
        switch (g_isp_format)
        {
        case FORMAT_720P25:
        case FORMAT_720P30:
            fmt = FORMAT_720P25;
            break;
        case FORMAT_960P25:
        case FORMAT_960P30:
            fmt = FORMAT_960P25;
            break;
        case FORMAT_1080P60:
        case FORMAT_1080P25:
        case FORMAT_1080P30:
        case FORMAT_1080P20:
        case FORMAT_1080P15:
            fmt = FORMAT_1080P25;
            break;
        case FORMAT_1536P40:
        case FORMAT_1536P30:
        case FORMAT_1536P25:
        case FORMAT_1536P15:
            fmt = FORMAT_1536P25;
            break;
        case FORMAT_400WP30:
        case FORMAT_400WP25:
        case FORMAT_400WP20:
        case FORMAT_400WP15:
            fmt = FORMAT_400WP25;
            break;
        case FORMAT_WDR_1080P25:
        case FORMAT_WDR_1080P30:
            fmt = FORMAT_WDR_1080P25;
            break;
        case FORMAT_WDR_1536P40:
        case FORMAT_WDR_1536P30:
        case FORMAT_WDR_1536P25:
        case FORMAT_WDR_1536P15:
            fmt = FORMAT_WDR_1536P25;
            break;
        case FORMAT_2304X1296P25:
            fmt = FORMAT_2304X1296P25;
            break;
        case FORMAT_WDR_2304X1296P25:
            fmt = FORMAT_WDR_2304X1296P25;
            break;
        case FORMAT_WDR_400WP30:
        case FORMAT_WDR_400WP25:
        case FORMAT_WDR_400WP20:
        case FORMAT_WDR_400WP15:
            fmt = FORMAT_WDR_400WP25;
            break;
#if defined(CONFIG_ARCH_FH885xV310)
        case FORMAT_500WP25:
        case FORMAT_500WP20:
        case FORMAT_500WP15:
        case FORMAT_500W12P5:
            fmt = FORMAT_500WP25;
            break;
        case FORMAT_WDR_500WP25:
        case FORMAT_WDR_500WP20:
        case FORMAT_WDR_500WP15:
        case FORMAT_WDR_500W12P5:
            fmt = FORMAT_WDR_500WP25;
            break;
#endif
#if defined(CONFIG_ARCH_FH8862)
        case FORMAT_WDR_800WP15:
        case FORMAT_WDR_800WP25:
            fmt = FORMAT_WDR_800WP25;
            break;
        case FORMAT_800WP30:
        case FORMAT_800WP25:
        case FORMAT_800WP15:
            fmt = FORMAT_800WP25;
            break;
#endif
        case FORMAT_2688X1520P30:
        case FORMAT_2688X1520P25:
            fmt = FORMAT_2688X1520P25;
            break;
        case FORMAT_WDR_2688X1520P30:
        case FORMAT_WDR_2688X1520P25:
        case FORMAT_WDR_2688X1520P20:
            fmt = FORMAT_WDR_2688X1520P25;
            break;
        default:
            printf("sensor fps format %x is not suppott!\n", g_isp_format);
            break;
        }
    }
    else if (fps == 30)
    {
        switch (g_isp_format)
        {
        case FORMAT_720P25:
        case FORMAT_720P30:
            fmt = FORMAT_720P30;
            break;
        case FORMAT_960P25:
        case FORMAT_960P30:
            fmt = FORMAT_960P30;
            break;
#if defined(CONFIG_ARCH_FH885xV200) || defined(CONFIG_ARCH_FH865x) || defined(CONFIG_ARCH_FH885xV210)
        case FORMAT_1080P60:
#endif
        case FORMAT_1080P25:
        case FORMAT_1080P30:
        case FORMAT_1080P20:
        case FORMAT_1080P15:
            fmt = FORMAT_1080P30;
            break;
        case FORMAT_1536P40:
        case FORMAT_1536P30:
        case FORMAT_1536P25:
        case FORMAT_1536P15:
            fmt = FORMAT_1536P30;
            break;
        case FORMAT_400WP30:
        case FORMAT_400WP25:
        case FORMAT_400WP20:
        case FORMAT_400WP15:
            fmt = FORMAT_400WP30;
            break;
        case FORMAT_WDR_1080P25:
        case FORMAT_WDR_1080P30:
            fmt = FORMAT_WDR_1080P30;
            break;
        case FORMAT_WDR_1536P40:
        case FORMAT_WDR_1536P30:
        case FORMAT_WDR_1536P25:
        case FORMAT_WDR_1536P15:
            fmt = FORMAT_WDR_1536P30;
            break;
        case FORMAT_WDR_400WP30:
        case FORMAT_WDR_400WP25:
        case FORMAT_WDR_400WP20:
        case FORMAT_WDR_400WP15:
            fmt = FORMAT_WDR_400WP30;
            break;
        case FORMAT_WDR_800WP15:
        case FORMAT_WDR_800WP30:
        case FORMAT_WDR_800WP25:
        case FORMAT_WDR_800WP20:
            fmt = FORMAT_WDR_800WP30;
            break;
        case FORMAT_800WP30:
        case FORMAT_800WP25:
        case FORMAT_800WP20:
        case FORMAT_800WP15:
            fmt = FORMAT_800WP30;
            break;
        case FORMAT_2688X1520P30:
        case FORMAT_2688X1520P25:
            fmt = FORMAT_2688X1520P30;
            break;
        case FORMAT_WDR_2688X1520P30:
        case FORMAT_WDR_2688X1520P25:
        case FORMAT_WDR_2688X1520P20:
            fmt = FORMAT_WDR_2688X1520P30;
            break;
        default:
            printf("sensor fps %d format %x is not suppott!\n", fps, g_isp_format);
            break;
        }
    }
    else if (fps == 20)
    {
        switch (g_isp_format)
        {
#if defined(CONFIG_ARCH_FH885xV200) || defined(CONFIG_ARCH_FH865x) || defined(CONFIG_ARCH_FH885xV210)
        case FORMAT_1080P60:
#endif
        case FORMAT_1080P25:
        case FORMAT_1080P30:
        case FORMAT_1080P20:
        case FORMAT_1080P15:
            fmt = FORMAT_1080P20;
            break;
        case FORMAT_400WP30:
        case FORMAT_400WP25:
        case FORMAT_400WP20:
        case FORMAT_400WP15:
            fmt = FORMAT_400WP20;
            break;
        case FORMAT_500WP25:
        case FORMAT_500WP20:
        case FORMAT_500WP15:
        case FORMAT_500W12P5:
            fmt = FORMAT_500WP20;
            break;
        case FORMAT_WDR_400WP30:
        case FORMAT_WDR_400WP25:
        case FORMAT_WDR_400WP20:
        case FORMAT_WDR_400WP15:
            fmt = FORMAT_WDR_400WP20;
            break;
        case FORMAT_WDR_500WP25:
        case FORMAT_WDR_500WP20:
        case FORMAT_WDR_500WP15:
        case FORMAT_WDR_500W12P5:
            fmt = FORMAT_WDR_500WP20;
            break;
        case FORMAT_WDR_800WP30:
        case FORMAT_WDR_800WP20:
        case FORMAT_WDR_800WP15:
            fmt = FORMAT_WDR_800WP20;
            break;
        case FORMAT_800WP30:
        case FORMAT_800WP20:
        case FORMAT_800WP15:
            fmt = FORMAT_800WP20;
            break;
        case FORMAT_WDR_2688X1520P30:
        case FORMAT_WDR_2688X1520P25:
        case FORMAT_WDR_2688X1520P20:
            fmt = FORMAT_WDR_2688X1520P20;
            break;
        default:
            printf("sensor fps %d format %x is not suppott!\n", fps, g_isp_format);
            break;
        }
    }
    else if (fps == 15)
    {
        switch (g_isp_format)
        {
#if defined(CONFIG_ARCH_FH885xV200) || defined(CONFIG_ARCH_FH865x) || defined(CONFIG_ARCH_FH885xV210)
        case FORMAT_1080P60:
#endif
        case FORMAT_1080P25:
        case FORMAT_1080P30:
        case FORMAT_1080P20:
        case FORMAT_1080P15:
            fmt = FORMAT_1080P15;
            break;
        case FORMAT_1536P40:
        case FORMAT_1536P30:
        case FORMAT_1536P25:
        case FORMAT_1536P15:
            fmt = FORMAT_1536P15;
            break;
        case FORMAT_400WP30:
        case FORMAT_400WP25:
        case FORMAT_400WP20:
        case FORMAT_400WP15:
            fmt = FORMAT_400WP15;
            break;
        case FORMAT_500WP25:
        case FORMAT_500WP20:
        case FORMAT_500WP15:
        case FORMAT_500W12P5:
            fmt = FORMAT_500WP15;
            break;
        case FORMAT_WDR_1536P40:
        case FORMAT_WDR_1536P30:
        case FORMAT_WDR_1536P25:
        case FORMAT_WDR_1536P15:
            fmt = FORMAT_WDR_1536P15;
            break;
#if defined(CONFIG_ARCH_FH8636_FH8852V20X)
        case FORMAT_2304X1296P15:
        case FORMAT_2304X1296P25:
            fmt = FORMAT_2304X1296P15;
            break;
#endif
        case FORMAT_WDR_400WP30:
        case FORMAT_WDR_400WP25:
        case FORMAT_WDR_400WP20:
        case FORMAT_WDR_400WP15:
            fmt = FORMAT_WDR_400WP15;
            break;
        case FORMAT_WDR_500WP25:
        case FORMAT_WDR_500WP20:
        case FORMAT_WDR_500WP15:
        case FORMAT_WDR_500W12P5:
            fmt = FORMAT_WDR_500WP15;
            break;
        case FORMAT_WDR_800WP30:
        case FORMAT_WDR_800WP25:
        case FORMAT_WDR_800WP20:
        case FORMAT_WDR_800WP15:
            fmt = FORMAT_WDR_800WP15;
            break;
        case FORMAT_800WP30:
        case FORMAT_800WP25:
        case FORMAT_800WP20:
        case FORMAT_800WP15:
            fmt = FORMAT_800WP15;
            break;
        default:
            printf("sensor fps %d format %x is not suppott!\n", fps, g_isp_format);
            break;
        }
    }
    else if (fps == 40)
    {
        switch (g_isp_format)
        {
        case FORMAT_1536P40:
        case FORMAT_1536P30:
        case FORMAT_1536P25:
        case FORMAT_1536P15:
            fmt = FORMAT_1536P40;
            break;
        case FORMAT_WDR_1536P40:
        case FORMAT_WDR_1536P30:
        case FORMAT_WDR_1536P25:
        case FORMAT_WDR_1536P15:
            fmt = FORMAT_WDR_1536P40;
            break;
        default:
            printf("sensor fps %d format %x is not suppott!\n", fps, g_isp_format);
            break;
        }
    }
    else if (fps == 12)
    {
        switch (g_isp_format)
        {
#if defined(CONFIG_ARCH_FH885xV310)
        case FORMAT_500WP25:
        case FORMAT_500WP20:
        case FORMAT_500WP15:
        case FORMAT_500W12P5:
            fmt = FORMAT_500W12P5;
            break;
        case FORMAT_WDR_500WP25:
        case FORMAT_WDR_500WP20:
        case FORMAT_WDR_500WP15:
        case FORMAT_WDR_500W12P5:
            fmt = FORMAT_500W12P5;
            break;
#endif
        default:
            printf("sensor fps %d format %x is not suppott!\n", fps, g_isp_format);
            break;
        }
    }
    else if (fps == 60)
    {
        switch (g_isp_format)
        {
        case FORMAT_1080P60:
        case FORMAT_1080P25:
        case FORMAT_1080P30:
        case FORMAT_1080P20:
        case FORMAT_1080P15:
            fmt = FORMAT_1080P60;
            break;
        default:
            printf("sensor fps %d format %x is not suppott!\n", fps, g_isp_format);
            break;
        }
    }
    else
    {
        printf("sensor fps %d is not suppott!\n", fps);
    }

    return fmt;
}



void change_sensor_fps(int fps)
{
    FH_SINT32 ret;
    FH_UINT32 fmt;
    static FH_SINT32 g_isp_format = ISP_FORMAT;

    fmt = get_fmt_by_fps(fps);
    if (g_isp_format == fmt)
        return;

    API_ISP_Pause(FH_GRP_ID);

    ret = FH_VPSS_Disable(FH_GRP_ID);
    if (ret != RETURN_OK)
    {
        printf("Error: FH_VPSS_Disable failed with %d\n", ret);
        return;
    }

    ret = API_ISP_SetSensorFmt(FH_GRP_ID, fmt); /* can not run in isr */
    if (ret != RETURN_OK)
    {
        printf("Error: API_ISP_SetSensorFmt failed with %d\n", ret);
        return;
    }

    ret = FH_VPSS_Enable(FH_GRP_ID, VPU_MODE_ISP);
    if (ret != RETURN_OK)
    {
        printf("Error: FH_VPSS_Enable failed with %d\n", ret);
        return;
    }
    API_ISP_Resume(FH_GRP_ID);
    g_isp_format = fmt;
	
#if defined RT_USING_HS_CUSTOM_8852V201_GC2083_DZ_20230619
	//API_ISP_SetMirrorAndflip(0, 1, 0);
		
#endif
}



void Sensor_Write(FH_UINT16 addr, FH_UINT16 data);

static void Sensor_Sleep(void)
{
    printf("Sensor_Sleep\n");
}

static void Sensor_Wakeup(void)
{
    printf("Sensor_Wakeup\n");
}

FH_VOID ISP_sleep_mode(FH_ISP_SENSOR_PM on)
{
    static int sleep_cur = -1;

    if (on == ISP_SENSOR_SLEEP)
    {
        g_sleep_isp = FH_FALSE;
        sleep_cur = 1;
        API_ISP_Pause(FH_GRP_ID);
        FH_VPSS_Disable(FH_GRP_ID);
        Sensor_Sleep();
    }
    else if (sleep_cur == 1)
    {
        g_sleep_isp = FH_TRUE;
        sleep_cur = 0;
        FH_VPSS_Enable(FH_GRP_ID, 0);
        API_ISP_Resume(FH_GRP_ID);
		
#if defined RT_USING_HS_CUSTOM_8852V201_GC2083_DZ_20230619
	//API_ISP_SetMirrorAndflip(0, 1, 0);
		
#endif
        Sensor_Wakeup();
    }
}

