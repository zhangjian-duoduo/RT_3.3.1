#include "dsp/fh_vpu_mpi.h"
#include "isp/isp_common.h"
#include "isp/isp_api.h"
#include "isp/isp_enum.h"
#include "sample_common.h"
#include "_sensor_.h"
#include "isp_format.h"
#include "isp_sensor.h"
#include "sample_smart_ir.h"
#include "isp_channel.h"
#include "sensor_pwm.h"

#ifdef __LINUX_OS__
static int gpio_enable(unsigned int gpio, int value)
{
    char cmd[100];

    sprintf(cmd, "echo \"dev,GPIO%d,0,0\" >/proc/driver/pinctrl", gpio);
    system(cmd);

    memset(cmd, 0, sizeof(cmd));
    sprintf(cmd, "echo %d > /sys/class/gpio/export", gpio);
    system(cmd);

    memset(cmd, 0, sizeof(cmd));
    sprintf(cmd, "echo out > /sys/class/gpio/GPIO%d/direction", gpio);
    system(cmd);

    memset(cmd, 0, sizeof(cmd));
    sprintf(cmd, "echo %d > /sys/class/gpio/GPIO%d/value", value, gpio);
    system(cmd);

    memset(cmd, 0, sizeof(cmd));
    sprintf(cmd, "echo %d > /sys/class/gpio/unexport", gpio);
    system(cmd);

    return 0;
}

static void reset_sensor(void)
{
    gpio_enable(13, 0);
    usleep(10 * 1000);
    gpio_enable(13, 1);
    usleep(10 * 1000);
}
#endif
#ifdef __RTTHREAD_OS__
#include "gpio.h"

static void reset_sensor(void)
{
    gpio_set_value(13, 0);
    usleep(10 * 1000);
    gpio_set_value(13, 1);
    usleep(10 * 1000);
}
#endif

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

/* 切换ISP幅面制式 */
FH_VOID sample_isp_change_resolution(FH_UINT32 grpid)
{
    FH_SINT32 ret;
    FH_SINT32 flag = 0;
    FH_UINT32 target_format;
    FH_UINT32 target_vi_w;
    FH_UINT32 target_vi_h;
    FH_VPU_SIZE vi_pic;
    FH_CHAR *param;
    FH_SINT32 hex_file_len;

    static FH_SINT32 isp_resolution_changed = 0;

    if (g_isp_info[grpid].isp_max_width < 1920 || g_isp_info[grpid].isp_max_height < 1080)
    {
        printf("Error: origin isp frame should be larger than 1080P\n");
        return;
    }

    if (!isp_resolution_changed)
    {
        target_format = g_isp_info[grpid].isp_format;
        target_vi_w = sample_isp_get_vi_w(grpid);
        target_vi_h = sample_isp_get_vi_h(grpid);
    }
    else
    {
        target_format = FORMAT_1080P25;
        target_vi_w = 1920;
        target_vi_h = 1080;
    }

    /* 停止isp */
    ret = API_ISP_Pause(grpid);
    if (ret != 0)
    {
        printf("Error(%d - %x): API_ISP_Pause\n", ret, ret);
        return;
    }

    /* 去使能vpu */
    ret = FH_VPSS_Disable(grpid);
    if (ret != 0)
    {
        printf("Error(%d - %x): FH_VPSS_Disable\n", ret, ret);
        return;
    }

    /* 设置isp_format为目标制式 */
    ret = API_ISP_SetSensorFmt(grpid, target_format);
    if (ret != 0)
    {
        printf("Error(%d - %x): API_ISP_SetSensorFmt\n", ret, ret);
        return;
    }

    // 启动sensor输出
    ret = API_ISP_SensorKick(grpid);
    if (ret)
    {
        printf("Error(%d - %x): API_ISP_SensorKick (grpid):(%d)!\n", ret, ret, grpid);
        return;
    }

    if ((target_format >> 16) & 1)
        flag |= SAMPLE_SENSOR_FLAG_WDR;

    vi_pic.vi_size.u32Width = target_vi_w;
    vi_pic.vi_size.u32Height = target_vi_h;
    vi_pic.crop_area.crop_en = 0;
    ret = FH_VPSS_SetViAttr(grpid, &vi_pic);
    if (ret != 0)
    {
        printf("Error(%d - %x): FH_VPSS_SetViAttr\n", ret, ret);
        return;
    }

    /* 使能vpu */
    ret = FH_VPSS_Enable(grpid, VPU_MODE_ISP); /*ISP直通模式,可以节省内存开销*/
    if (ret != 0)
    {
        printf("Error(%d - %x): FH_VPSS_Enable\n", ret, ret);
        return;
    }

    /* 获得sensor参数文件位置及大小，并且加载参数 */
    param = get_isp_sensor_param(grpid, (FH_CHAR *)g_isp_info[grpid].sensor->name, flag, &hex_file_len);
    printf("hex param addr: %p, size: %d\n", param, hex_file_len);
    if (param)
    {
        ret = API_ISP_LoadIspParam(grpid, param);
        if (ret)
        {
            printf("Error: API_ISP_LoadIspParam with %d(%x)\n", ret, ret);
        }

        free_isp_sensor_param(param);
    }
    else
    {
        printf("Error[%d]: Cann't load sensor hex file!\n", grpid);
        return;
    }

    /* 恢复isp */
    ret = API_ISP_Resume(grpid);
    if (ret != 0)
    {
        printf("Error(%d - %x): API_ISP_Resume!\n", ret, ret);
        return;
    }

    isp_resolution_changed = !isp_resolution_changed;
}

/* 切换ISP帧率 */
FH_VOID sample_isp_change_fps(FH_UINT32 grpid)
{
    FH_SINT32 ret;
    FH_UINT32 fps = 0;

    switch (g_isp_info[grpid].isp_format)
    {
    case FORMAT_720P25:
        g_isp_info[grpid].isp_format = FORMAT_720P30;
        fps = 30;
        break;
    case FORMAT_720P30:
        g_isp_info[grpid].isp_format = FORMAT_720P25;
        fps = 25;
        break;
    case FORMAT_960P25:
        g_isp_info[grpid].isp_format = FORMAT_960P30;
        fps = 30;
        break;
    case FORMAT_960P30:
        g_isp_info[grpid].isp_format = FORMAT_960P25;
        fps = 25;
        break;
    case FORMAT_1080P25:
        g_isp_info[grpid].isp_format = FORMAT_1080P30;
        fps = 30;
        break;
    case FORMAT_1080P30:
        g_isp_info[grpid].isp_format = FORMAT_1080P25;
        fps = 25;
        break;
    default:
        break;
    }

    if (!fps)
    {
        printf("Keep current fps\n");
        return;
    }

    /* 停止isp */
    API_ISP_Pause(grpid);

    /* 去使能vpu */
    ret = FH_VPSS_Disable(grpid);
    if (ret != 0)
    {
        printf("Error(%d - %x): FH_VPSS_Disable\n", ret, ret);
        return;
    }

    API_ISP_SetSensorFmt(grpid, g_isp_info[grpid].isp_format);

    /* 使能vpu */
    ret = FH_VPSS_Enable(grpid, VPU_MODE_ISP); /*ISP直通模式,可以节省内存开销*/
    if (ret != 0)
    {
        printf("Error(%d - %x): FH_VPSS_Enable\n", ret, ret);
        return;
    }

    /* 恢复isp */
    API_ISP_Resume(grpid);

    printf("Change current fps to %d\n\n", fps);
}

FH_VOID isp_vpu_reconfig(FH_UINT32 grpid)
{
    FH_VPU_SIZE vpu_size;
    ISP_VI_ATTR_S isp_vi;

    // FH_VPSS_Reset();
    API_ISP_Pause(grpid);
    API_ISP_Resume(grpid);

    FH_VPSS_GetViAttr(grpid, &vpu_size);
    API_ISP_GetViAttr(grpid, &isp_vi);
    if (vpu_size.vi_size.u32Width != isp_vi.u16PicWidth ||
        vpu_size.vi_size.u32Height != isp_vi.u16PicHeight)
    {
        vpu_size.vi_size.u32Width = isp_vi.u16PicWidth;
        vpu_size.vi_size.u32Height = isp_vi.u16PicHeight;
        FH_VPSS_SetViAttr(grpid, &vpu_size);
    }

    API_ISP_SensorKick(grpid);
}

static FH_SINT32 choose_i2c(FH_UINT32 grpid)
{

    switch (grpid)
    {
    case 0:
        return 0;
    case 1:
        return 0;
    case 2:
        return 4;
    default:
        break;
    }
    return 0;
}

static FH_SINT32 sample_isp_init(FH_UINT32 grpid)
{
    FH_SINT32 flag = SAMPLE_SENSOR_FLAG_NORMAL;
    FH_SINT32 ret;
    FH_SINT32 hex_file_len;
    FH_CHAR *param;
    ISP_MEM_INIT stMemInit = {0};
    Sensor_Init_t initConf = {0};
    ISP_VI_ATTR_S sensor_vi_attr = {0};

    stMemInit.stPicConf.u32Width = g_isp_info[grpid].isp_max_width;
    stMemInit.stPicConf.u32Height = g_isp_info[grpid].isp_max_height;
    stMemInit.enIspOutMode = ISP_OUT_TO_VPU;

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

    // 下载配置到sensor
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

    // 启动sensor输出
    ret = API_ISP_SensorKick(grpid);
    if (ret)
    {
        printf("Error(%d - %x): API_ISP_SensorKick (grpid):(%d)!\n", ret, ret, grpid);
        return ret;
    }

    // 初始化ISP硬件寄存器
    ret = API_ISP_Init(grpid);
    if (ret)
    {
        printf("Error(%d - %x): API_ISP_Init (grpid):(%d)!\n", ret, ret, grpid);
        return ret;
    }

    // 3 根据实际使用，配置vicap是离线模式还是在线模式
    ret = API_ISP_GetViAttr(grpid, &sensor_vi_attr);
    if (ret)
    {
        printf("Error(%d - %x): API_ISP_GetViAttr (grpid):(%d)!\n", ret, ret, grpid);
        return ret;
    }

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

#if defined(FH_APP_USING_IRCUT_G0) || defined(FH_APP_USING_IRCUT_G1) || defined(FH_APP_USING_IRCUT_G2)
    sample_SmartIR_init(g_isp_info[grpid].sensor->name, grpid);
#endif

    return 0;
}

FH_VOID *sample_isp_proc(FH_VOID *arg)
{
    struct dev_isp_info *isp_info = (struct dev_isp_info *)arg;
    char name[20];

    sprintf(name, "demo_isp%d", isp_info->channel);
    prctl(PR_SET_NAME, name);
    isp_info->running = 1;

    while (!isp_info->bStop)
    {
        API_ISP_Run(isp_info->channel);
#if defined(FH_APP_USING_IRCUT_G0) || defined(FH_APP_USING_IRCUT_G1) || defined(FH_APP_USING_IRCUT_G2)
        sample_SmartIR_Ctrl(isp_info->channel);
#endif

#ifdef FH_APP_OPEN_AF
        if (isp_info->channel == FH_APP_GRP_ID)
        {
            sample_af_run();
        }
#endif
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

    reset_sensor();
    
    for (grpid = 0; grpid < MAX_GRP_NUM; grpid++)
    {
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
#ifdef MULTI_SENSOR
    FH_PWM_CONF pwm_conf;
    pwm_conf.id = 9;
    pwm_conf.period_ns = 66000000;
    pwm_conf.duty_ns = 33000000;
    pwm_conf.delay_ns = 33000000 - 1.82 * 1000 * 1000 + 1.24 * 1000 * 1000;
    pwm_conf.phase_ns = 0;
    pwm_conf.pulses = 0;

    FH_PWM_Init(9);              // 初始化pwm10
    FH_PWM_setConfig(&pwm_conf); // 设置pwm的参数
    FH_PWM_Start();              // 开启pwm

#ifdef __LINUX_OS__
    signal(SIGUSR1, FH_SensorSequeCreate_Handler);

    kill(getpid(), SIGUSR1);
#endif

#ifdef __RTTHREAD_OS__
    API_ISP_SetSensorReg(0, 0x03fe, 0x0000);
    myUsleep(32840);
    API_ISP_SetSensorReg(1, 0x03fe, 0x0000); // reset sensor 2

    // 同时启动两个sensor
    API_ISP_SetSensorReg(0, 0x023e, 0x0099);
    API_ISP_SetSensorReg(1, 0x023e, 0x0099);

#endif

#endif /*MULTI_SENSOR*/
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
#ifdef __RTTHREAD_OS__
            pthread_attr_setstacksize(&attr[grpid], 30 * 1024);
            param[grpid].sched_priority = 130;
#endif
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

#if defined(FH_APP_USING_IRCUT_G0) || defined(FH_APP_USING_IRCUT_G1) || defined(FH_APP_USING_IRCUT_G2)
                sample_SmartIR_deinit(grpid);
#endif
                API_ISP_Exit(grpid);
            }
        }
    }

#ifdef __LINUX_OS__
#ifdef MULTI_SENSOR
    FH_PWM_Stop();
#endif
#endif
    return 0;
}

static struct dbi_tcp_config g_dbi_tcp_conf;
static volatile FH_SINT32 g_coolview_is_running;
static volatile FH_SINT32 g_stop_coolview;

FH_SINT32 sample_common_start_coolview(FH_VOID *arg)
{
    FH_SINT32 ret;
    pthread_t thread_dbg;
    pthread_attr_t attr;
    struct sched_param param;

    if (g_coolview_is_running)
    {
        printf("Error: coolview is already running!\n");
        return -1;
    }

    g_stop_coolview = 0;
    g_dbi_tcp_conf.cancel = &g_stop_coolview;
    g_dbi_tcp_conf.port = 8888;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize(&attr, 3 * 1024);
#ifdef __RTTHREAD_OS__
    param.sched_priority = 130;
#endif
    pthread_attr_setschedparam(&attr, &param);
    ret = pthread_create(&thread_dbg, &attr, (FH_VOID * (*)(FH_VOID *)) libtcp_dbi_thread, &g_dbi_tcp_conf);
    if (!ret)
    {
        g_coolview_is_running = 1;
    }
    else
    {
        printf("Error: create coolview thread failed!\n");
    }

    return ret;
}

FH_SINT32 sample_common_stop_coolview(FH_UINT32 grpid)
{
    if (g_coolview_is_running)
    {
        /*wait for coolview thread to exit*/
        g_stop_coolview = 1;
        while (g_stop_coolview)
        {
            usleep(20 * 1000);
        }
        g_coolview_is_running = 0;
    }

    return 0;
}
