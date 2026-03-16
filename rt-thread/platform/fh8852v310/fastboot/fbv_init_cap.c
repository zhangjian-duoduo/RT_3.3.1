#include <rtthread.h>
#include "types/type_def.h"
#include "dsp/fh_system_mpi.h"
#include "dsp/fh_vpu_mpi.h"
#include "dsp/fh_venc_mpi.h"
#include "isp/isp_common.h"
#include "isp/isp_api.h"
#include "isp/isp_enum.h"
#include "types/bufCtrl.h"
#include "fh_clock.h"
#include "gpio.h"
#include "delay.h"
#include "timer.h" /* for read_pts */

#include "fh_def.h"
#include "platform_def.h"
#include "all_sensor_array.h"
#include "fbv_sensor_pwm.h"

#ifdef FH_SENSOR_CLK
#undef FH_SENSOR_CLK
#endif

/* MACRO definition */
#ifndef FH_APP_THREAD_PRIORITY
#define FH_APP_THREAD_PRIORITY 70
#endif

#ifdef CONFIG_CHIP_FH8626V100
#error "This SDK doesn't support fastboot(FH8626V200). please contact FAE!"
#endif

/* for all platform, OV2732 uses 24MHz CIS Clock */
#define FH_SENSOR_CLK (24 * 1000 * 1000)

#define ERR_RETURN(name, ret_val)                      \
    do                                                 \
    {                                                  \
        if (ret_val != RETURN_OK)                      \
        {                                              \
            rt_kprintf(name " failed(%d)\n", ret_val); \
            return ret_val;                            \
        }                                              \
    } while (0)

unsigned int _fbv_is_pae_ready = 0;
static struct isp_sensor_if *g_sensor = 0;
static volatile int pic_size_changed = 0;

extern void fh_media_process_module_init(void);
extern void fh_vpu_module_init(void);
extern void fh_isp_module_init(void);
extern FH_SINT32 clk_enable(struct clk *clk);

#ifdef CONFIG_STARTUP_TIMECOST
#define PIC_NUM (6)
static unsigned int g_stc_time[4];
static unsigned int g_stc_dsp[10];
static unsigned int g_stc_isp[10];
static unsigned int g_stc_pic_end_g0[PIC_NUM + 1];
static unsigned int g_stc_pic_start_g0[PIC_NUM + 1];
static unsigned int g_stc_pic_end_g1[PIC_NUM + 1];
static unsigned int g_stc_pic_start_g1[PIC_NUM + 1];
extern void print_dsp_timecost(void);
void print_fastboot_timecost(void)
{

    rt_kprintf("\n");
    rt_kprintf("     fastboot startup cost: \n");
    rt_kprintf("            meidia startup time: %lu\n", g_stc_time[0]);
    rt_kprintf("            model init cost: %d\n", g_stc_time[1] - g_stc_time[0]);
    rt_kprintf("            vpu startup cost: %d\n", g_stc_time[2] - g_stc_time[1]);
    rt_kprintf("            isp startup cost: %d\n", g_stc_time[3] - g_stc_time[2]);
    rt_kprintf("            sys pre startup cost: %d\n", g_stc_dsp[1] - g_stc_dsp[0]);
    rt_kprintf("            vpu create chan startup cost: %d\n", g_stc_dsp[2] - g_stc_dsp[1]);
    rt_kprintf("            isp(part1) startup cost: %d\n", g_stc_isp[1] - g_stc_isp[0]);
    rt_kprintf("            isp(part2) (pwm) startup cost: %d\n", g_stc_isp[2] - g_stc_isp[1]);
    rt_kprintf("            isp proc run startup time: % 10u\n", g_stc_isp[3]);

    int i;
    for (i = 0; i <= PIC_NUM; i++)
    {
        rt_kprintf("       sensor0:pic[%d] time: [%10u %10u]\n", i, g_stc_pic_start_g0[i], g_stc_pic_end_g0[i]);
        rt_kprintf("       sensor1:pic[%d] time: [%10u %10u]\n", i, g_stc_pic_start_g1[i], g_stc_pic_end_g1[i]);
    }
    print_dsp_timecost();
}

static int startof_pict_grop0(void)
{
    static int idx = 0;

    g_stc_pic_start_g0[idx++] = (unsigned int)read_pts();
    if (idx > PIC_NUM)
    {
        API_ISP_RegisterPicStartCallback(0, RT_NULL);
    }

    return 0;
}

static int endof_pict_grop0(void)
{
    static int idx = 0;

    g_stc_pic_end_g0[idx++] = (unsigned int)read_pts();
    if (idx > PIC_NUM)
    {
        API_ISP_RegisterPicEndCallback(0, RT_NULL);
    }
    return 0;
}

static int startof_pict_grop1(void)
{
    static int idx = 0;

    g_stc_pic_start_g1[idx++] = (unsigned int)read_pts();
    if (idx > PIC_NUM)
    {
        API_ISP_RegisterPicStartCallback(1, RT_NULL);
    }

    return 0;
}

static int endof_pict_grop1(void)
{
    static int idx = 0;

    g_stc_pic_end_g1[idx++] = (unsigned int)read_pts();
    if (idx > PIC_NUM)
    {
        API_ISP_RegisterPicEndCallback(1, RT_NULL);
    }

    return 0;
}
#endif

#define SNS_HEX_ENTRY_MAGIC (0x6ad7bfc5)
struct hex_file_hdr
{
    unsigned int total_len; /*文件总长度*/
    unsigned int magic;
    unsigned int reserved[2];
};

struct hex_file_entry
{
    unsigned int offset;
    unsigned int len;
    unsigned int magic;
    char file_name[36];
};

#define SAMPLE_SENSOR_FLAG_NORMAL (0x00)
#define SAMPLE_SENSOR_FLAG_NIGHT (0x02)
#define SAMPLE_SENSOR_FLAG_WDR (0x01)
#define SAMPLE_SENSOR_FLAG_WDR_NIGHT (SAMPLE_SENSOR_FLAG_NIGHT | SAMPLE_SENSOR_FLAG_WDR)

static void _get_hex_file_name(char *sensor_name, int flags, char *file_name)
{
    char chr;
    char *suffix = "";

    if (flags == SAMPLE_SENSOR_FLAG_NORMAL)
        suffix = "_attr.hex";
    else if (flags == SAMPLE_SENSOR_FLAG_WDR)
        suffix = "_wdr_attr.hex";
    else if (flags == SAMPLE_SENSOR_FLAG_WDR_NIGHT)
        suffix = "_wdr_night_attr.hex";
    else if (flags == SAMPLE_SENSOR_FLAG_NIGHT)
        suffix = "_night_attr.hex";

    while (1)
    {
        chr = *(sensor_name++);
        if (!chr)
            break;
        if (chr >= 'A' && chr <= 'Z')
            chr += 32; /*转为小写字母*/
        *(file_name++) = chr;
    }

    do
    {
        chr = *(suffix++);
        *(file_name++) = chr;
    } while (chr);
}

static char *_os_get_isp_sensor_param(char *file_name, int *olen)
{
    struct hex_file_entry *entry = (struct hex_file_entry *)(g_sensor_hex_array + sizeof(struct hex_file_hdr));

    while (1)
    {
        if (entry->magic != SNS_HEX_ENTRY_MAGIC || !entry->offset)
            break;

        if (!rt_strcmp(entry->file_name, file_name)) /*find it*/
        {
            *olen = entry->len;
            return (g_sensor_hex_array + entry->offset);
        }
        entry++;
    }

    return NULL;
}

static char *get_sensor_param(char *sensor_name, int flags, int *olen)
{
    char file_name[64];

    _get_hex_file_name(sensor_name, flags, file_name);

    return _os_get_isp_sensor_param(file_name, olen);
}

void myUsleep(long int us)
{
    unsigned long long pts1, pts2;

    pts1 = read_pts();

    while (1)
    {
        pts2 = read_pts();
        if (pts2 - pts1 >= us)
            break;
    }
}

/* static variable definition */
static rt_thread_t g_thread_isp;

extern struct isp_sensor_if *Sensor_Create_gc2083_mipi(void);
extern struct isp_sensor_if *Sensor_Create_ovos02k_mipi(void);
struct isp_sensor_if *fbv_Sensor_Create(void)
{
#ifdef RT_USING_CG2083_MIPI
    return Sensor_Create_gc2083_mipi();
#endif

#ifdef RT_USING_OVOS02K_MIPI
    return Sensor_Create_ovos02k_mipi();
#endif
}

void fbv_isp_proc(void *arg)
{
    int grpid = (int)arg;
#ifdef RT_USING_CG2083_MIPI
    ISP_AWB_GAIN awb;
    if (grpid == 0)
    {
        awb.u16Rgain = 0x286;
        awb.u16Ggain = 0x200;
        awb.u16Bgain = 0x400;
        API_ISP_SetAwbGain(0, &awb);
    }
    if (grpid == 1)
    {
        awb.u16Rgain = 0x2b0;
        awb.u16Ggain = 0x200;
        awb.u16Bgain = 0x3b0;
        API_ISP_SetAwbGain(1, &awb);
    }
#endif
    g_stc_isp[3] = read_pts();
    /* API_ISP_LoadIspParam(isp_param_buff);*/
    while (1)
    {
        API_ISP_Run(grpid);
        /* rt_thread_delay(1); */
    }
}
extern int isp_write_proc(char *s);
static int fbv_isp_init(void)
{
    FH_SINT32 ret;
    FH_UINT32 grpid = 0;
    FH_BIND_INFO src, dst;
    ISP_MEM_INIT stMemInit = {0};
    Sensor_Init_t initConf = {0};
    ISP_VI_ATTR_S sensor_vi_attr = {0};

    stMemInit.stPicConf.u32Width = ISP_INIT_WIDTH;
    stMemInit.stPicConf.u32Height = ISP_INIT_HEIGHT;
    stMemInit.enIspOutMode = ISP_OUT_TO_VPU;
#ifdef CONFIG_STARTUP_TIMECOST
    g_stc_isp[0] = read_pts();
#endif

    for (grpid = 0; grpid < FBV_GRP_NUM; grpid++)
    {
        ret = API_ISP_MemInit(grpid, &stMemInit);
        if (ret)
        {
            rt_kprintf("Error: [GROP=%d]API_ISP_MemInit failed with %d\n", grpid, ret);
            return -1;
        }

        g_sensor = fbv_Sensor_Create();
        if (g_sensor == RT_NULL)
        {
            rt_kprintf("Error: sensor create error\n");
            return -1;
        }

        ret = API_ISP_SensorRegCb(grpid, 0, g_sensor);
        if (ret < 0)
        {
            rt_kprintf("Error: [GROP=%d]API_ISP_SensorRegCb failed with %d\n", grpid, ret);
            return -1;
        }

        initConf.u8CsiDeviceId = grpid;
        initConf.u8CciDeviceId = 0;
        initConf.bGrpSync = FH_FALSE;

        ret = API_ISP_SensorInit(grpid, &initConf);
        if (ret)
        {
            rt_kprintf("Error: [GROP=%d]API_ISP_SensorInit failed with %d\n", grpid, ret);
            return -1;
        }
        ret = API_ISP_SetSensorFmt(grpid, ISP_FORMAT);
        if (ret)
        {
            rt_kprintf("Error: [GROP=%d]API_ISP_SetSensorFmt failed with %d\n", grpid, ret);
            return -1;
        }
#if defined(RT_USING_CG2083_MIPI)
        ret = API_ISP_GetViAttr(grpid, &sensor_vi_attr);
        if (grpid == 0)
        {
            /* 修改sensor 输出幅面的高为1084 */
            API_ISP_SetSensorReg(grpid, 0x0196, 0x3c);
            /* 修改isp 输入幅面大小为1084 */
            sensor_vi_attr.u16InputHeight += 4;
        }
        ret = API_ISP_SetViAttr(grpid, &sensor_vi_attr);
        if (ret)
        {
            return ret;
        }
#endif
        ret = API_ISP_SensorKick(grpid);
        if (ret)
        {
            rt_kprintf("Error: [GROP=%d]API_ISP_SensorKick failed with %d\n", grpid, ret);
            return -1;
        }

        ret = API_ISP_Init(grpid);
        if (ret)
        {
            rt_kprintf("Error: API_ISP_Init failed with %d\n", ret);
            return ret;
        }

        /* FIXME: This Callback just for check launch time: API_ISP_RegisterPicStartCallback(&scb); */
#if defined(CONFIG_STARTUP_TIMECOST)
        if (grpid == 0)
        {
            API_ISP_RegisterPicStartCallback(0, startof_pict_grop0);
            API_ISP_RegisterPicEndCallback(0, endof_pict_grop0);
        }
        if (grpid == 1)
        {
            API_ISP_RegisterPicStartCallback(1, startof_pict_grop1);
            API_ISP_RegisterPicEndCallback(1, endof_pict_grop1);
        }
#endif
        {
            char *isp_param_buff;
            int olen;

            isp_param_buff = get_sensor_param(g_sensor->name, 0, &olen);
            API_ISP_LoadIspParam(grpid, isp_param_buff);
        }

        src.obj_id = FH_OBJ_ISP;
        src.dev_id = grpid;
        src.chn_id = 0;
        dst.obj_id = FH_OBJ_VPU_VI;
        dst.dev_id = grpid;
        dst.chn_id = 0;
        ret = FH_SYS_Bind(src, dst);
        if (ret != 0)
        {
            rt_kprintf("[ERROR]: FH_SYS_Bind[%x]\n", ret);
            return -1;
        }

        g_thread_isp = rt_thread_create("fast_isp_thrd", (void *)fbv_isp_proc, (void *)grpid,
                                        10 * 1024, FH_APP_THREAD_PRIORITY, 10);
        rt_thread_startup(g_thread_isp);
    }
#ifdef CONFIG_STARTUP_TIMECOST
    g_stc_isp[1] = read_pts();
#endif

#ifdef MULTI_SENSOR
    {
        struct fh_pwm_chip_data pwm_conf;
        rt_memset(&pwm_conf, 0, sizeof(pwm_conf));
        pwm_conf.id = 9;
        pwm_conf.config.period_ns = 66000000;
        pwm_conf.config.duty_ns = 33000000;
        pwm_conf.config.delay_ns = 33000000;
        pwm_conf.config.phase_ns = 0;
        pwm_conf.config.pulses = 0;
        pwm_conf.config.pulse_num = 0;
        extern unsigned int fh_pwm_output_disable(unsigned int n);
        extern long fh_pwm_set_config(struct fh_pwm_chip_data *chip_data);
        extern unsigned int fh_pwm_output_enable(unsigned int n);
        fh_pwm_output_disable(pwm_conf.id);
        fh_pwm_set_config(&pwm_conf);
        fh_pwm_output_enable(pwm_conf.id);

        extern long rt_hw_interrupt_disable(void);
        extern void rt_hw_interrupt_enable(int);
        long flag = rt_hw_interrupt_disable();

        API_ISP_SetSensorReg(0, 0x03fe, 0x0000);
        myUsleep(32640);
        API_ISP_SetSensorReg(1, 0x03fe, 0x0000); /* reset sensor 2 */

        /*同时启动两个sensor*/
        API_ISP_SetSensorReg(0, 0x023e, 0x0099);
        API_ISP_SetSensorReg(1, 0x023e, 0x0099);

        rt_hw_interrupt_enable(flag);
    }
#endif
#ifdef CONFIG_STARTUP_TIMECOST
    g_stc_isp[2] = read_pts();
#endif
    return 0;
}

static int start_vpu(void)
{
    FH_SINT32 ret = 0;
    FH_UINT32 grpid = 0;
    FH_VPU_SET_GRP_INFO stChnconfig;
    FH_VPU_CHN_INFO chn_info;
    FH_VPU_CHN_CONFIG chn_attr;
    FH_VPU_SIZE vi_pic;

/*
 * step  1: init media platform
 */
#ifdef CONFIG_STARTUP_TIMECOST
    g_stc_dsp[0] = read_pts();
#endif

    /*
     * media_trace_write_proc("create_0_0x400000");
     * media_trace_write_proc("zonelog_0x7ffdf");
     * media_trace_write_proc("modall_2_2");
     */
    ret = FH_SYS_Init_Pre();
    if (ret != RETURN_OK)
    {
        rt_kprintf("Error: FH_SYS_Init failed with %d\n", ret);
        return -1;
    }
#ifdef CONFIG_STARTUP_TIMECOST
    g_stc_dsp[1] = read_pts();
#endif
    stChnconfig.vi_max_size.u32Width = VIDEO_INPUT_WIDTH;
    stChnconfig.vi_max_size.u32Height = VIDEO_INPUT_HEIGHT;
    stChnconfig.ycmean_ds = 16;
    stChnconfig.ycmean_en = 1;

    for (grpid = 0; grpid < FBV_GRP_NUM; grpid++)
    {
        /*
         * step  2: set vpss resolution
         */
        ret = FH_VPSS_CreateGrp(grpid, &stChnconfig);
        if (ret)
        {
            rt_kprintf("Error: [GROP=%d]FH_VPSS_CreateGrp failed with %d\n", grpid, ret);
            return -1;
        }

        vi_pic.vi_size.u32Width = VIDEO_INPUT_WIDTH;
        vi_pic.vi_size.u32Height = VIDEO_INPUT_HEIGHT;
        vi_pic.crop_area.crop_en = 0;

        ret = FH_VPSS_SetViAttr(grpid, &vi_pic);
        if (ret != RETURN_OK)
        {
            rt_kprintf("Error: [GROP=%d]FH_VPSS_SetViAttr failed with %d\n", grpid, ret);
            return -1;
        }

        /*
         * step  3: start vpss
         */
        ret = FH_VPSS_Enable(grpid, 0);
        if (ret != RETURN_OK)
        {
            rt_kprintf("Error: [GROP=%d]FH_VPSS_Enable failed with %d\n", grpid, ret);
            return -1;
        }

        /*
         * step  4: configure vpss channel 0
         */

        chn_info.bgm_enable = 1;
        chn_info.cpy_enable = 1;
        chn_info.sad_enable = 1;
        chn_info.bgm_ds = 8;
        chn_info.chn_max_size.u32Width = CH0_WIDTH;
        chn_info.chn_max_size.u32Height = CH0_HEIGHT;
        chn_info.out_mode = VPU_VOMODE_SCAN;
        chn_info.support_mode = 1 << VPU_VOMODE_SCAN;
        chn_info.bufnum = 3;
        chn_info.max_stride = 0;

        ret = FH_VPSS_CreateChn(grpid, 0, &chn_info);
        if (ret != 0)
        {
            rt_kprintf("Error: [GROP=%d]FH_VPSS_CreateChn failed with %d\n", grpid, ret);
            return ret;
        }

        chn_attr.vpu_chn_size.u32Width = CH0_WIDTH;
        chn_attr.vpu_chn_size.u32Height = CH0_HEIGHT;
        chn_attr.crop_area.crop_en = 0;
        chn_attr.stride = 0;
        chn_attr.offset = 0;
        chn_attr.depth = 1;

        ret = FH_VPSS_SetChnAttr(grpid, 0, &chn_attr);
        if (ret != RETURN_OK)
        {
            rt_kprintf("Error: [GROP=%d]FH_VPSS_SetChnAttr failed with %d\n", grpid, ret);
            return -1;
        }

        ret = FH_VPSS_SetVOMode(grpid, 0, VPU_VOMODE_SCAN);
        if (ret != 0)
        {
            rt_kprintf("Error: [GROP=%d]FH_VPSS_SetVOMode failed with %d\n", grpid, ret);
            return ret;
        }

        /*
         * step  5: open vpss channel 0
         */
        ret = FH_VPSS_OpenChn(grpid, 0);
        if (ret != RETURN_OK)
        {
            rt_kprintf("Error: [GROP=%d]FH_VPSS_OpenChn failed with %d\n", grpid, ret);
            return -1;
        }
    }
    /*grop0 ch0支持nn检测*/
    extern int create_nn_chan(void);
    create_nn_chan();
#ifdef CONFIG_STARTUP_TIMECOST
    g_stc_dsp[2] = read_pts();
#endif
    return 0;
}
extern void init_nn_module(void);
int __fastvideo_init(void)
{
    int ret;
#ifdef CONFIG_STARTUP_TIMECOST
    g_stc_time[0] = read_pts();
#endif

    bufferInit((unsigned char *)FH_SDK_MEM_START, FH_SDK_MEM_SIZE);
    fh_media_process_module_init();

#if !defined(CONFIG_ARCH_FH8636_FH8852V20X) && !defined(CONFIG_ARCH_FH8626V100)
    fh_vpu_module_init();
#endif

    init_nn_module(); /* 初始化NN */

    fh_isp_module_init();
#ifdef CONFIG_STARTUP_TIMECOST
    g_stc_time[1] = read_pts();
#endif
    start_vpu();
#ifdef CONFIG_STARTUP_TIMECOST
    g_stc_time[2] = read_pts();
#endif
    ret = fbv_isp_init();
    if (ret != 0)
    {
        rt_kprintf("fbv_isp_init returns %d\n", ret);
        return -1;
    }
#ifdef CONFIG_STARTUP_TIMECOST
    g_stc_time[3] = read_pts();
#endif
    return 0;
}
