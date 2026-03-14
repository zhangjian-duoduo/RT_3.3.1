#ifdef __RTTHREAD_OS__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <optparse.h>
#include <config.h>
#include <sys/prctl.h>
#include "fh_af_ctrl.h"

static int g_af_running = 0;
static int g_af_stop = 0;
static FH_AF_CTRL_e g_cmd = AF_HOLD;
static FH_AF_CFG_t g_af_cfg;

static void af_usage(void)
{
    printf("\nusage:\n");
    printf("    autofocus -c [cmd] -i [id] -d [dir] -s [step]\n\n");
    printf("info:\n");
    printf("    [cmd] :\n");
    printf("        0:AF_HOLD\n");
    printf("        1:AF_LENS_RESET\n");
    printf("        2:AF_LENS_MOVE_ONLY\n");
    printf("        3:AF_MOVE_ZOOM\n");
    printf("        4:AF_AUTOFOCUS\n");
    printf("        5:AF_UNDO\n");
    printf("        6:AF_GENERATE_MAPFILE\n");
    printf("        7:AF_TEST\n");
    printf("    [id] :\n");
    printf("        0:LENS_FOCUS\n");
    printf("        1:LENS_ZOOM\n");
    printf("    [dir] :\n");
    printf("        0:BACKWARD\n");
    printf("        1:FORWARD\n");
    printf("example: \n");
    printf("        autofocus -c 3 -i 1 -d 1 -s 100\n");
}

static void autofocus(int argc, char *argv[])
{
    struct optparse options;
    int param;

    optparse_init(&options, argv);
    while ((param = optparse(&options, "c:i:d:s:")) != -1)
    {
        switch(param)
        {
            case 'c':
                g_cmd = strtol(options.optarg, NULL, 0);
                break;
            case 'i':
                g_af_cfg.id = strtol(options.optarg, NULL, 0);
                break;
            case 'd':
                g_af_cfg.dir = strtol(options.optarg, NULL, 0);
                break;
            case 's':
                g_af_cfg.step = strtol(options.optarg, NULL, 0);
                break;
            default:
                af_usage();
                break;
        }
    }
}

static void *af_run(void *args)
{
    FH_AF_CFG_t cfg;

    prctl(PR_SET_NAME, "af_run");
    g_af_stop = 0;
    while(!g_af_stop)
    {
        usleep(50);
        if(g_cmd != AF_HOLD)
        {
            memcpy(&cfg, &g_af_cfg, sizeof(FH_AF_CFG_t));
            FH_AF_Ctrl(FH_APP_GRP_ID, g_cmd, &cfg);
            g_cmd = AF_HOLD;
        }
    }

    FH_AF_Exit(FH_APP_GRP_ID);
    g_af_running = 0;

    return NULL;
}

int sample_af_start(void)
{
    int i;
    int ret = -1;
    pthread_t af_thread;
    pthread_attr_t af_attr;
    FH_AF_MOTOR_CFG_t motor_cfg;
    FH_AF_STG_CFG_t af_cfg;

    if(g_af_running)
    {
        printf("af already running!\n");
        ret = 0;
        goto Exit;
    }

    motor_cfg.zoom_range = 2342;
    motor_cfg.focus_range = 3011;
    motor_cfg.focus_reserve_range = 0;
    motor_cfg.zoom_reserve_range = 0;
    motor_cfg.inv_error = 6;
    motor_cfg.freq_low = 100;
    motor_cfg.freq_mid = 100;
    motor_cfg.freq_high = 150;

    for (i = 0; i < 4; i++)
        motor_cfg.zoom_pwm[i] = i + 6;
    for (i = 0; i < 4; i++)
        motor_cfg.focus_pwm[i] = i + 2;

    motor_cfg.type = AF_MOTOR_STM;

    af_cfg.mode = 0;
    af_cfg.interCfg.gainthre = 0x400;
    af_cfg.interCfg.frmW = 2560;
    af_cfg.interCfg.frmH = 1440;
    strcpy(af_cfg.mapPth, "zfmap");

    ret = FH_AF_Init(FH_APP_GRP_ID, &motor_cfg, &af_cfg);
    if (ret)
    {
        printf("[FH_AF_Init] failed with:%d\n", ret);
        goto Exit;
    }
    FH_AF_SetDebugLevel(FH_APP_GRP_ID, AF_DEBUG_OFF);

    pthread_attr_init(&af_attr);
    pthread_attr_setdetachstate(&af_attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize(&af_attr, 40 * 1024);
    ret = pthread_create(&af_thread, &af_attr, af_run, NULL);
    if(ret)
    {
        printf("[pthread_create]: failed with %d\n", ret);
        goto Exit;
    }

    g_af_running = 1;

Exit:
    return ret;
}

void sample_af_stop(void)
{
    g_af_stop = 1;
    while(g_af_running)
    {
        usleep(50);
    }
}

void sample_af_run(void)
{
    if(!g_af_stop)
    {
        FH_AF_FrameRun(FH_APP_GRP_ID);
    }
}

#include <rttshell.h>
SHELL_CMD_EXPORT(autofocus, autofocus(cmd, *argv[]));

#endif
