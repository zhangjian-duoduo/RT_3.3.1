#include <rtthread.h>
#include "types/type_def.h"
#include "dsp/fh_system_mpi.h"
#include "dsp/fh_vpu_mpi.h"
#include "dsp/fh_venc_mpi.h"
#include "isp/isp_common.h"
#include "isp/isp_api.h"
#include "isp/isp_enum.h"
#include "types/bufCtrl.h"

#include "timer.h" /* for read_pts() */
#include "fh_def.h"
/* through platform_def.h includes fastvideo_config.h */
#include "platform_def.h"

#define SWITCH_SENSOR_FRAME_RATE

#ifdef CONFIG_CHIP_FH8626V100
#error "This SDK doesn't support fastboot(FH8626V100). please contact FAE!"
#else
extern unsigned short g_awb_speed_save;
extern unsigned short g_ae_speed_save;
#endif
/* extern definition */
extern unsigned int _fbv_is_pae_ready;
#ifdef CONFIG_CHIP_FH8626V100
extern void fh_pae_module_init(void);
#else
extern void fh_enc_module_init(void);
#endif
#ifdef CONFIG_FAST_VIDEO_RESIZE
extern void _fbv_wait_pic_resized(void);
#endif
extern int enc_write_proc(char *s);
extern int media_write_proc(char *s);
extern char isp_param_buff[];

#ifdef CONFIG_STARTUP_TIMECOST
static unsigned int g_stc_enc[8];
void print_dsp_timecost(void)
{
    rt_kprintf("\n");
    rt_kprintf("          enc startup time: % 10u\n", g_stc_enc[0]);
    rt_kprintf("          post model init cost: % 10d\n", g_stc_enc[1] - g_stc_enc[0]);
    rt_kprintf("          enc init cost: % 10d\n", g_stc_enc[2] - g_stc_enc[1]);
}
#endif

int __fastvideo_startup(void)
{
    FH_VENC_CHN_CAP cfg_vencmem;
    FH_VENC_CHN_CONFIG cfg_param;
    FH_BIND_INFO src, dst;

    FH_SINT32 ret;
    FH_UINT32 grpid;
#ifdef CONFIG_STARTUP_TIMECOST
    g_stc_enc[0] = read_pts();
#endif
    fh_enc_module_init();

#ifdef FH_APP_USING_SAMPLE_MJPEG
    extern void fh_jpeg_module_init(void);
    fh_jpeg_module_init();
#endif
    FH_SYS_Init_Post();
    enc_write_proc("stm_4000000_128");
    enc_write_proc("allchnstm_0_4000000_128");
#ifdef CONFIG_STARTUP_TIMECOST
    g_stc_enc[1] = read_pts();
#endif
    /******************************************
     * step  1: create venc channel 0
     ******************************************/

#ifdef FH_APP_USING_SAMPLE_H265
    cfg_vencmem.support_type = FH_NORMAL_H265;
    cfg_vencmem.max_size.u32Width = CH0_WIDTH;
    cfg_vencmem.max_size.u32Height = CH0_HEIGHT;

    for (grpid = 0; grpid < FBV_GRP_NUM; grpid++)
    {
        ret = FH_VENC_CreateChn(grpid * FBV_GRP_NUM, &cfg_vencmem);
        if (ret != RETURN_OK)
        {
            rt_kprintf("Error: [GROP%d]FH_VENC_CreateChn(0) failed with %d\n", grpid, ret);
            return -1;
        }
    }

    cfg_param.chn_attr.enc_type = FH_NORMAL_H265;
    cfg_param.chn_attr.h265_attr.profile = H265_PROFILE_MAIN;
    cfg_param.chn_attr.h265_attr.i_frame_intterval = 50;
    cfg_param.chn_attr.h265_attr.size.u32Width = CH0_WIDTH;
    cfg_param.chn_attr.h265_attr.size.u32Height = CH0_HEIGHT;

    cfg_param.rc_attr.rc_type = FH_RC_H265_VBR;
    cfg_param.rc_attr.h265_vbr.init_qp = 35;
    cfg_param.rc_attr.h265_vbr.bitrate = CH0_BIT_RATE;
    cfg_param.rc_attr.h265_vbr.ImaxQP = 42;
    cfg_param.rc_attr.h265_vbr.IminQP = 28;
    cfg_param.rc_attr.h265_vbr.PmaxQP = 42;
    cfg_param.rc_attr.h265_vbr.PminQP = 28;
    cfg_param.rc_attr.h265_vbr.maxrate_percent = 200;
    cfg_param.rc_attr.h265_vbr.IFrmMaxBits = 0;
    cfg_param.rc_attr.h265_vbr.IP_QPDelta = 3;
    cfg_param.rc_attr.h265_vbr.I_BitProp = 10;
    cfg_param.rc_attr.h265_vbr.P_BitProp = 1;
    cfg_param.rc_attr.h265_vbr.fluctuate_level = 0;
    cfg_param.rc_attr.h265_vbr.FrameRate.frame_count = CH0_FRAME_COUNT;
    cfg_param.rc_attr.h265_vbr.FrameRate.frame_time = CH0_FRAME_TIME;
#else

    cfg_vencmem.support_type = FH_NORMAL_H264;
    cfg_vencmem.max_size.u32Width = CH0_WIDTH;
    cfg_vencmem.max_size.u32Height = CH0_HEIGHT;

    for (grpid = 0; grpid < FBV_GRP_NUM; grpid++)
    {
        ret = FH_VENC_CreateChn(grpid * FBV_GRP_NUM, &cfg_vencmem);
        if (ret != RETURN_OK)
        {
            rt_kprintf("Error: [GROP%d]FH_VENC_CreateChn failed with %d\n", grpid, ret);
            return -1;
        }
    }

    cfg_param.chn_attr.enc_type = FH_NORMAL_H264;
    cfg_param.chn_attr.h264_attr.profile = H264_PROFILE_MAIN;
    cfg_param.chn_attr.h264_attr.i_frame_intterval = 50;
    cfg_param.chn_attr.h264_attr.size.u32Width = CH0_WIDTH;
    cfg_param.chn_attr.h264_attr.size.u32Height = CH0_HEIGHT;

    cfg_param.rc_attr.rc_type = FH_RC_H264_VBR;
    cfg_param.rc_attr.h264_vbr.bitrate = CH0_BIT_RATE;
    cfg_param.rc_attr.h264_vbr.init_qp = 35;
    cfg_param.rc_attr.h264_vbr.ImaxQP = 50;
    cfg_param.rc_attr.h264_vbr.IminQP = 28;
    cfg_param.rc_attr.h264_vbr.PmaxQP = 50;
    cfg_param.rc_attr.h264_vbr.PminQP = 28;
    cfg_param.rc_attr.h264_vbr.FrameRate.frame_count = CH0_FRAME_COUNT;
    cfg_param.rc_attr.h264_vbr.FrameRate.frame_time = CH0_FRAME_TIME;
    cfg_param.rc_attr.h264_vbr.maxrate_percent = 200;
    cfg_param.rc_attr.h264_vbr.IFrmMaxBits = 0;
    cfg_param.rc_attr.h264_vbr.IP_QPDelta = 3;
    cfg_param.rc_attr.h264_vbr.I_BitProp = 5;
    cfg_param.rc_attr.h264_vbr.P_BitProp = 1;
    cfg_param.rc_attr.h264_vbr.fluctuate_level = 0;
#endif

    for (grpid = 0; grpid < FBV_GRP_NUM; grpid++)
    {
        ret = FH_VENC_SetChnAttr(grpid * FBV_GRP_NUM, &cfg_param);
        if (ret != RETURN_OK)
        {
            rt_kprintf("Error: FH_VENC_SetChnAttr failed with %d\n", ret);
            return -1;
        }

        /******************************************
         * step 2: start venc channel 0
         ******************************************/
        ret = FH_VENC_StartRecvPic(grpid * FBV_GRP_NUM);
        if (RETURN_OK != ret)
        {
            rt_kprintf("Error: FH_VENC_StartRecvPic failed with %d\n", ret);
            return -1;
        }
    }

    /******************************************
     * step  2.1: init jpeg system
     ******************************************/
#ifdef FH_APP_USING_SAMPLE_MJPEG
    cfg_vencmem.support_type = FH_JPEG;
    cfg_vencmem.max_size.u32Width = CH0_WIDTH;
    cfg_vencmem.max_size.u32Height = CH0_HEIGHT;

    ret = FH_VENC_CreateChn(JPEG_ENC_CHNUM, &cfg_vencmem);
    if (ret != RETURN_OK)
    {
        rt_kprintf("Error: FH_VENC_CreateChn(MJPEG) failed with %x\n", ret);
        return -1;
    }

    cfg_param.chn_attr.enc_type = FH_JPEG;
    cfg_param.chn_attr.jpeg_attr.qp = 20;          /* 0-99 */
    cfg_param.chn_attr.jpeg_attr.rotate = 0;       /* 0-3  */
    cfg_param.chn_attr.jpeg_attr.encode_speed = 0; /* 0-9 */
    ret = FH_VENC_SetChnAttr(JPEG_ENC_CHNUM, &cfg_param);
    if (ret != RETURN_OK)
    {
        rt_kprintf("Error: FH_VENC_SetChnAttr(JPEG) failed with %x\n", ret);
        return -1;
    }
#endif
#ifdef CONFIG_STARTUP_TIMECOST
    g_stc_enc[2] = read_pts();
#endif
    /****************************************************
     * step 3: init ISP, and then start ISP process thread
     ****************************************************/
    for (grpid = 0; grpid < FBV_GRP_NUM; grpid++)
    {
        src.obj_id = FH_OBJ_VPU_VO;
        src.dev_id = grpid;
        src.chn_id = 0;

        dst.obj_id = FH_OBJ_ENC;
        dst.dev_id = 0;
        dst.chn_id = grpid * FBV_GRP_NUM;

        ret = FH_SYS_Bind(src, dst);
        if (ret != 0)
        {
            rt_kprintf("[GROP%d ENC]FH_SYS_Bind %d!\n", grpid, ret);
        }
    }

    /*
     * usleep(500000);
     * media_trace_write_proc("zonelog_0x7ffff");
     */

    return 0;
}
