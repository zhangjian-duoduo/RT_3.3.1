#include <stdio.h>
#include <pthread.h>
#include "fh_aod_mpi.h"
#include "sample_common.h"

/* 效果一般，误检和错检概率较高 */

static int g_stop_aod;
static int g_aod_running;

extern FH_SINT32 sample_set_gbox(FH_UINT32 chan, FH_UINT32 enable,  FH_UINT32 box_id, FH_UINT32 x, FH_UINT32 y, FH_UINT32 w, FH_UINT32 h, FHT_RgbColor_t color);

static void *aod_proc(void *args)
{
    int i, j;
    FH_AOD_INIT_PARAM_t aod_param;
    struct grp_vpu_info grp_info;

    BGM_Rect motion[48];
    BGM_Rect motion1[48];
    BGM_Rect motion2[48];

    unsigned int motion_num = 0;
    unsigned int motion_num1;
    unsigned int motion_num2;
    int flag = 0;

    struct vpu_channel_info vpu_info_chn0;
    struct vpu_channel_info vpu_info_chn1;

    sample_common_dsp_get_vpu_chn_info(FH_APP_GRP_ID, 0, &vpu_info_chn0);
    sample_common_dsp_get_vpu_chn_info(FH_APP_GRP_ID, 1, &vpu_info_chn1);
    if(!vpu_info_chn0.enable || !vpu_info_chn1.enable)
    {
        printf("[DEMO_ABANDON] vpu grp[%d] chn[0] and chn[1] needs to be enabled!\n", FH_APP_GRP_ID);
        goto Exit;
    }

    aod_param.alarm_cnt = 45;
    aod_param.max_miss_cnt = 15;
    FH_AOD_Init(&aod_param);

    FH_AOD_OBJ_INFO_t aodIn, aodOut;

    sample_common_bgm_init();

    FHT_RgbColor_t color = {255, 0, 255, 255};

    unsigned int lastCnt = 0;
    unsigned int lastMorionCnt = 0;
    sample_common_dsp_get_grp_info(FH_APP_GRP_ID, &grp_info);

    usleep(400 * 1000); /* wait for bgm ready */
    while(!g_stop_aod)
    {
        sample_common_bgm_triger_reload(FH_APP_GRP_ID);
        sample_common_bgm_get_multi_motion_area(FH_APP_GRP_ID, motion1, &motion_num1, 48, 5);
        sample_common_bgm_get_multi_motion_area(FH_APP_GRP_ID, motion2, &motion_num2, 48, 6);

        motion_num = 0;
        for (i = 0; i < motion_num1; i++)
        {
            flag = 0;
            for (j = 0; j < motion_num2; j++)
            {
                if((motion2[i].x >= motion1[i].x)&&(motion2[i].x+motion2[i].w <= motion1[i].x+motion1[i].w)&&(motion2[i].y >= motion1[i].y)&&(motion2[i].y+motion2[i].h <= motion1[i].y+motion1[i].h))
                    flag = 1;
            }
            if(flag)
            {
                motion[motion_num].x = motion1[i].x;
                motion[motion_num].y = motion1[i].y;
                motion[motion_num].w = motion1[i].w;
                motion[motion_num].h = motion1[i].h;
                motion_num+=1;
            }
        }

        for (i = 0; i < motion_num; i++)
        {
            sample_set_gbox(1, 1, i, motion[i].x * grp_info.bgm_ds * vpu_info_chn1.width / vpu_info_chn0.width, motion[i].y * grp_info.bgm_ds * vpu_info_chn1.height / vpu_info_chn0.height,
                motion[i].w * grp_info.bgm_ds * vpu_info_chn1.width / vpu_info_chn0.width, motion[i].h * grp_info.bgm_ds * vpu_info_chn1.height / vpu_info_chn0.height, color);
        }

        for (; i < lastMorionCnt; i++)
        {
            sample_set_gbox(1, 0, i, grp_info.bgm_ds, grp_info.bgm_ds, grp_info.bgm_ds, grp_info.bgm_ds, color);
        }

        lastMorionCnt = motion_num;

        memcpy(aodIn.aodRects, motion, motion_num * sizeof(BGM_Rect));

        aodIn.aodCnt = motion_num;
        FH_AOD_Process(&aodIn,&aodOut);
        for (i = 0; i < aodOut.aodCnt; i++)
        {
            if (aodOut.aodRects[i].w * grp_info.bgm_ds * aodOut.aodRects[i].h * grp_info.bgm_ds * 2 > vpu_info_chn0.width * vpu_info_chn0.height)
                continue;
            // printf("%d %d %d %dcompare %d %d\n",aodOut.aodRects[i].w, aodOut.aodRects[i].h, vpu_info_chn0.width , vpu_info_chn0.height, aodOut.aodRects[i].w * grp_info.bgm_ds * aodOut.aodRects[i].h * grp_info.bgm_ds * 10, 2 * vpu_info_chn0.width * vpu_info_chn0.height);
            sample_set_gbox(0, 1, i, aodOut.aodRects[i].x * grp_info.bgm_ds, aodOut.aodRects[i].y * grp_info.bgm_ds,
                aodOut.aodRects[i].w * grp_info.bgm_ds, aodOut.aodRects[i].h * grp_info.bgm_ds, color);
        }

        for (; i < lastCnt; i++)
        {
            sample_set_gbox(0, 0, i, grp_info.bgm_ds, grp_info.bgm_ds, grp_info.bgm_ds, grp_info.bgm_ds, color);
        }

        lastCnt = aodOut.aodCnt;

        usleep(40 * 1000);
    }

Exit:
    FH_AOD_Exit();
    g_aod_running = 0;

    return NULL;
}


void sample_aod_start(void)
{
    pthread_t aodTh;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize(&attr, 10 * 1024);

    g_stop_aod = 0;
    g_aod_running = 1;
    pthread_create(&aodTh, &attr, aod_proc, NULL);

    pthread_attr_destroy(&attr);
}

void sample_aod_stop(void)
{
     g_stop_aod = 1;

     while(g_aod_running)
     {
     	usleep(10 * 1000);
     }
}
