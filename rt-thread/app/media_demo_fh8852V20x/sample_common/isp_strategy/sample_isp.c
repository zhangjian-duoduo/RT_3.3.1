#include "dsp/fh_vpu_mpi.h"
#include "isp/isp_common.h"
#include "isp/isp_api.h"
#include "isp/isp_enum.h"
#include "sample_common.h"
#include "FHAdv_Isp_mpi_v3.h"

static volatile FH_SINT32 g_stop_isp = 0;
static volatile FH_SINT32 g_isp_runnig = 0;

#ifdef FH_APP_ISP_MIRROR_FLIP
static FH_VOID SetMirrorAndFlip(FH_VOID)
{
    int time_wait;

    time_wait = 5;

    printf("[ISP_Strategy] set Mirror.\n");
    FHAdv_Isp_SetMirrorAndflip(FH_APP_GRP_ID, FH_APP_GRP_ID, 1, 0);
    sleep(time_wait);
    printf("[ISP_Strategy] set Flip.\n");
    FHAdv_Isp_SetMirrorAndflip(FH_APP_GRP_ID, FH_APP_GRP_ID, 0, 1);
    sleep(time_wait);
    printf("[ISP_Strategy] set Mirror & Flip.\n");
    FHAdv_Isp_SetMirrorAndflip(FH_APP_GRP_ID, FH_APP_GRP_ID, 1, 1);
    sleep(time_wait);
    printf("[ISP_Strategy] reset.\n");
    FHAdv_Isp_SetMirrorAndflip(FH_APP_GRP_ID, FH_APP_GRP_ID, 0, 0);

}
#endif

#ifdef FH_APP_CHANGE_AE_MODE
static FH_VOID SetAEMode(FH_VOID)
{
    int time_wait;
    int mode;
    int ae_time_level;
    int gain_level;

    time_wait = 5;

    sleep(time_wait);
    mode          = 1;
    ae_time_level = FH_SINT32t_1_100000;
    gain_level    = 0;
    FHAdv_Isp_SetAEMode(FH_APP_GRP_ID, mode, ae_time_level, gain_level);
    printf("[ISP_Strategy] set AEMode mode:%d ae_time_level:%d gain_level:%d.\n", mode, ae_time_level, gain_level);


    sleep(time_wait);
    mode          = 2;
    ae_time_level = 0;
    gain_level    = 50;
    FHAdv_Isp_SetAEMode(FH_APP_GRP_ID, mode, ae_time_level, gain_level);
    printf("[ISP_Strategy] set AEMode mode:%d ae_time_level:%d gain_level:%d.\n", mode, ae_time_level, gain_level);


    sleep(time_wait);
    mode          = 3;
    ae_time_level = FH_SINT32t_1_25;
    gain_level    = 100;
    FHAdv_Isp_SetAEMode(FH_APP_GRP_ID, mode, ae_time_level, gain_level);
    printf("[ISP_Strategy] set AEMode mode:%d ae_time_level:%d gain_level:%d.\n", mode, ae_time_level, gain_level);


    sleep(time_wait);
    mode          = 0;
    ae_time_level = 0;
    gain_level    = 0;
    FHAdv_Isp_SetAEMode(FH_APP_GRP_ID, mode, ae_time_level, gain_level);
    printf("[ISP_Strategy] set AEMode mode:%d ae_time_level:%d gain_level:%d.\n", mode, ae_time_level, gain_level);

}
#endif

#ifdef FH_APP_CHANGE_NPMODE
static FH_VOID SetSharpness(FH_VOID)
{
    int time_wait;
    int mode;
    int value;

    time_wait = 5;

    mode  = 1;
    value = 50;
    FHAdv_Isp_SetSharpeness(FH_APP_GRP_ID, mode, value);
    printf("[ISP_Strategy] set sharpness mode:%d, value:%d.\n", mode, value);


    sleep(time_wait);
    mode  = 1;
    value = -50;
    FHAdv_Isp_SetSharpeness(FH_APP_GRP_ID, mode, value);
    printf("[ISP_Strategy] set sharpness mode:%d, value:%d.\n", mode, value);


    sleep(time_wait);
    mode  = 0;
    value = 0;
    FHAdv_Isp_SetSharpeness(FH_APP_GRP_ID, mode, value);
    printf("[ISP_Strategy] set sharpness mode:%d, value:%d.\n", mode, value);

}
#endif

#ifdef FH_APP_CHANGE_SATURATION
static FH_VOID SetSaturation(FH_VOID)
{
    int i;
    int time_wait;
    int mode;
    int value;

    time_wait = 5;

    for(i = 0; i < 4; i++)
    {
        sleep(time_wait);
        mode  = 1;
        value = 50 * i;
        FHAdv_Isp_SetSaturation(FH_APP_GRP_ID, mode, value);
        printf("[ISP_Strategy] set saturation mode:%d, value:%d.\n", mode, value);
    }

    sleep(time_wait);
    mode  = 0;
    value = 0;
    FHAdv_Isp_SetSaturation(FH_APP_GRP_ID, mode, value);
    printf("[ISP_Strategy] set saturation mode:%d, value:%d.\n", mode, value);

}
#endif

#ifdef FH_APP_CHANGE_CHROMA
static FH_VOID SetChroma(FH_VOID)
{
    return;
}
#endif


FH_VOID* sample_isp_thread(FH_VOID *args)
{
    int cmd = 0;
    int k;

    prctl(PR_SET_NAME, "demo_adv_isp");
    while (!g_stop_isp)
    {
#ifdef FH_APP_ISP_MIRROR_FLIP
        if (cmd == 0)
        {
            SetMirrorAndFlip();
        }
#endif

#ifdef FH_APP_CHANGE_AE_MODE
        if (cmd == 1)
        {
            SetAEMode();
        }
#endif

#ifdef FH_APP_CHANGE_NPMODE
        if (cmd == 2)
        {
            SetSharpness();
        }
#endif

#ifdef FH_APP_CHANGE_SATURATION
        if (cmd == 3)
        {
            SetSaturation();
        }
#endif

#ifdef FH_APP_CHANGE_CHROMA
        if (cmd == 4)
        {
            SetChroma();
        }
#endif

        if (++cmd > 4)
            cmd = 0;

        for (k=0; k<20 && !g_stop_isp; k++)
        {
            usleep(100*1000);
        }
    }

    g_isp_runnig = 0;

    return FH_NULL;
}

FH_SINT32 sample_isp_start(FH_VOID)
{
    FH_SINT32 ret;

    pthread_t isp_thread;
    pthread_attr_t attr;

    if (g_isp_runnig)
    {
        printf("Isp strategy is already running\n");
        return 0;
    }

    ret = FHAdv_Isp_Init(FH_APP_GRP_ID);
    if (ret)
    {
        printf("Error: isp_strategy init failed!\n");
        return ret;
    }

    g_isp_runnig = 1;
    g_stop_isp   = 0;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize(&attr, 10 * 1024);

    ret = pthread_create(&isp_thread, &attr, sample_isp_thread, FH_NULL);
    if (ret)
    {
        printf("Error: Create isp_strategy thread failed!\n");
        g_isp_runnig = 0;
    }

    return ret;
}

FH_VOID sample_isp_stop(FH_VOID)
{
    g_stop_isp   = 1;
    while (g_isp_runnig)
    {
        usleep(40 * 1000);
    }
}
