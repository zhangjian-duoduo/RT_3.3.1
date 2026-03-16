#include <stdlib.h>
#include <rtthread.h>
#include "dsp/fh_system_mpi.h"
#include "dsp/fh_common.h"
#include "dsp/fh_vpu_mpi.h"
#include "dsp/fh_nna_mpi.h"
#include "dsp/fh_nna_mpipara.h"
#include "platform_def.h" /* for NN frame size & channel num */

/* #define TIME_COSTS_STAT  */

#define MAX_NN_MODEL_SIZE (0x300000)

#if (defined(CONFIG_ARCH_FH865x) || defined(CONFIG_ARCH_FH8636_FH8852V20X)) || defined(CONFIG_ARCH_FH885xV310) && defined(FH_FAST_BOOT)
static FH_VOID *g_nn_handle = RT_NULL;
static unsigned int g_detected_frames = 0;

int process_nn_model(void);
extern unsigned long long read_pts(void);
extern FH_SINT32 fh_nna_module_init(void);
extern int load_nbg_file(unsigned char *nn_buf, unsigned int nn_size);

void init_nn_module(void)
{
    int ret;

    ret = fh_nna_module_init();
    if (ret)
    {
        rt_kprintf("fh_nna_module_init failed %x !\n", ret);
    }
}

static void nn_dect_thrd(void *args)
{
    int ret;
    FH_VPU_STREAM_ADV frmData;
    FH_IMAGE_T src;
    FH_DETECTION_T nn_out;

    g_detected_frames = 0;
    static unsigned int idx = 0;
    static unsigned int frame = 0;
    while (1)
    {
        idx++;
        ret = FH_VPSS_GetChnFrameAdv_NoRpt(0, NN_DETECT_CHAN, &frmData, 1000);
        if (ret)
        {
            continue;
        }
        src.frame_id = frmData.frame_id;
        src.width = NN_DETECT_WIDTH;
        src.height = NN_DETECT_HEIGHT;
        src.stride = NN_DETECT_WIDTH;
        src.imageType = FH_IMAGE_FORMAT_RGB888;
        src.src_data = frmData.frm_rgb888.data;
        src.timestamp = frmData.time_stamp;

        ret = FH_NNA_DET_Process(g_nn_handle, &src, &nn_out);
        if (ret)
        {
            rt_kprintf("FH_NNA_DET_Process failed %x !\n", ret);
        }
        else
        {
            if (idx == 1)
            {
                rt_kprintf("first NN frame PTS: 0x%x%08x, current time(us): %10u\n", (unsigned int)(frmData.time_stamp >> 32), (unsigned int)(frmData.time_stamp & 0xffffffff), (unsigned int)read_pts());
            }
        }
        /* process with nn_out */
        if (ret == 0 && nn_out.boxNum > 0)
        {
            frame++;
            if (frame == 1)
            {
                rt_kprintf("\n$$$$$$$$$$$$$$$   first nn detected pts= %u  $$$$$$$$$$$$$$$$$$$$$\n", (unsigned int)read_pts());
            }
        }
    }
}

static void start_do_detect(void)
{
    rt_thread_t tid;

    tid = rt_thread_create("nn_detect", nn_dect_thrd, RT_NULL, 0x4000, 30, 10);
    if (tid != RT_NULL)
        rt_thread_startup(tid);
}

void load_nn_modules(void)
{
    int ret;
    unsigned int nn_size = MAX_NN_MODEL_SIZE;
    unsigned char *nn_buf;
    FH_NN_INIT_PARAM_T nna_param;

#ifdef TIME_COSTS_STAT
    unsigned long long t1, t2;
#endif

    nn_buf = rt_malloc(nn_size);
    if (nn_buf == RT_NULL)
    {
        rt_kprintf("malloc nn buffer(0x%x) failed.\n", nn_size);
        return;
    }
    rt_memset(nn_buf, 0, nn_size);
#ifdef TIME_COSTS_STAT
    t1 = read_pts();
#endif
    if (load_nbg_file(nn_buf, nn_size) < 0)
    {
        rt_kprintf("No NBG file found.\n");
        return;
    }
#ifdef TIME_COSTS_STAT
    t2 = read_pts();
    rt_kprintf("load nbg costs: %d\n", (unsigned int)(t2 - t1));
#endif

    nna_param.src_w_in = NN_DETECT_WIDTH;
    nna_param.src_h_in = NN_DETECT_HEIGHT;
    nna_param.src_c_in = 4;
    nna_param.type = FN_DET_TYPE_PERSON;
    nna_param.conf_thr = 0.8;
    nna_param.rotate = FN_ROT_0;
    nna_param.nbg_data = nn_buf;

    ret = FH_NNA_DET_Init(&g_nn_handle, 0, &nna_param);
    if (ret != 0)
    {
        rt_kprintf("FH_NNA_DET_Init failed, get: 0x%x\n", ret);
        return;
    }

    rt_free(nn_buf); /* not used now, free it */

    /* shall we start a new thread to start detection??? */
    start_do_detect();
}

int create_nn_chan(void)
{
    FH_SINT32 ret;
    FH_VPU_CHN_INFO chn_info;
    FH_VPU_CHN_CONFIG rgb_chan_attr;

    chn_info.bgm_enable = 0;
    chn_info.cpy_enable = 0;
    chn_info.sad_enable = 0;
    chn_info.bgm_ds = 0;
    chn_info.chn_max_size.u32Width = NN_DETECT_WIDTH;
    chn_info.chn_max_size.u32Height = NN_DETECT_HEIGHT;
    chn_info.out_mode = VPU_VOMODE_RGB888;
    chn_info.support_mode = 1 << VPU_VOMODE_RGB888;
    chn_info.bufnum = 3;
    chn_info.max_stride = 0;

    ret = FH_VPSS_CreateChn(0, NN_DETECT_CHAN, &chn_info);
    if (ret != 0)
    {
        rt_kprintf("Error[FH_NN]: FH_VPSS_CreateChn fail, ret = %x\n", ret);
        return ret;
    }

    rgb_chan_attr.vpu_chn_size.u32Width = NN_DETECT_WIDTH;
    rgb_chan_attr.vpu_chn_size.u32Height = NN_DETECT_HEIGHT;
    rgb_chan_attr.crop_area.crop_en = 0;
    rgb_chan_attr.crop_area.vpu_crop_area.u32X = 0;
    rgb_chan_attr.crop_area.vpu_crop_area.u32Y = 0;
    rgb_chan_attr.crop_area.vpu_crop_area.u32Width = 0;
    rgb_chan_attr.crop_area.vpu_crop_area.u32Height = 0;
    rgb_chan_attr.stride = 0;
    rgb_chan_attr.offset = 0;
    rgb_chan_attr.depth = 1;
    ret = FH_VPSS_SetChnAttr(0, NN_DETECT_CHAN, &rgb_chan_attr);
    if (ret != 0)
    {
        rt_kprintf("Error[FH_NN]: FH_VPSS_SetChnAttr fail, ret = %x\n", ret);
        return ret;
    }

    ret = FH_VPSS_SetVOMode(0, NN_DETECT_CHAN, VPU_VOMODE_RGB888);
    if (ret != 0)
    {
        rt_kprintf("Error[FH_NN]: FH_VPSS_SetVOMode fail, ret = %x\n", ret);
        return ret;
    }

    ret = FH_VPSS_OpenChn(0, NN_DETECT_CHAN);
    if (ret != 0)
    {
        rt_kprintf("Error[FH_NN]: FH_VPSS_OpenChn fail, ret = %x\n", ret);
        return ret;
    }

    return ret;
}
#endif
