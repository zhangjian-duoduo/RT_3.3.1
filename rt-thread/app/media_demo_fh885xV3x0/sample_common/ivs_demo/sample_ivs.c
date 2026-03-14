#include <stdarg.h>
#include "sample_common.h"
#include "fh_trip_area.h"

#define BGMSZ 8 /* bgm ds 8像素采样，框大小为8 */

#define OPEN_FILE(file, filename, flag)         \
    do                                          \
    {                                           \
        file = fopen(filename, flag);           \
        if (file == NULL)                       \
        {                                       \
            printf("open %s fail\n", filename); \
            goto Exit;                          \
        }                                       \
    } while (0)

#define CLOSE_FILE(file)  \
    do                    \
    {                     \
        if (file != NULL) \
            fclose(file); \
    } while (0)

static FH_SINT32 g_ivs_stop = 0;
static FH_SINT32 g_ivs_is_running = 0;
static FH_UINT32 current_max_gbox_id = 0;

extern FH_SINT32 sample_set_gbox(FH_UINT32 chan, FH_UINT32 enable, FH_UINT32 box_id, FH_UINT32 x, FH_UINT32 y, FH_UINT32 w, FH_UINT32 h, FHT_RgbColor_t color);

static FH_SINT32 clear_screen(int reserve_num)
{
    FH_UINT32 i;
    FH_UINT32 ret;
    FHT_RgbColor_t color = {0, 255, 0, 255}; /* 绿 */

    for (i = current_max_gbox_id; i > reserve_num; i--)
    {
        ret = sample_set_gbox(0, 0, i, 0, 0, 0, 0, color);
        if (ret)
        {
            printf("Error: sample_set_gbox %d\n", ret);
            return ret;
        }
    }

    current_max_gbox_id = reserve_num;

    return 0;
}

static FH_SINT32 draw_gbox(FH_UINT32 x, FH_UINT32 y, FH_UINT32 w, FH_UINT32 h, FH_UINT32 colorid)
{
    FHT_RgbColor_t colors[7] = {{255, 0, 0, 255},      /* 红 */
                                {0, 255, 0, 255},      /* 绿 */
                                {0, 0, 255, 255},      /* 蓝 */
                                {255, 255, 0, 255},    /* 黄 */
                                {255, 0, 255, 255},    /* 粉 */
                                {0, 255, 255, 255},    /* 青 */
                                {255, 255, 255, 255}}; /* 白 */

    current_max_gbox_id++;
    return sample_set_gbox(0, 1, current_max_gbox_id, x, y, w, h, colors[colorid]);
}

static int debugCallback(const FH_CHAR *format, ...)
{
    FH_CHAR param[100] = {'\0'};
    va_list valist;

    va_start(valist, format);
    vsprintf(param, format, valist);
    va_end(valist);

    printf("%s", param);

    return 0;
}

static FH_VOID *tad_init(FH_VOID)
{
    FH_SINT32 ret;
    TAD_HANDLE tadHandle = NULL;
    FH_TAD_HANDLE_CFG_t tad_cfg;

    tad_cfg.maxTrackNum = 20;
    tad_cfg.iouThresh = 10;
    tad_cfg.minHits = 3;
    tad_cfg.maxAge = 20;

    ret = FH_TAD_CreatHandle(&tadHandle, &tad_cfg);
    if (ret)
    {
        printf("FH_TAD_CreatHandle failed whit %d\n", ret);
        tadHandle = NULL;
    }

    return tadHandle;
}

static FH_VOID tad_deinit(TAD_HANDLE tadHandle)
{
    if (tadHandle)
    {
        FH_TAD_HandleRelease(tadHandle);
    }
}

FH_TASK_RULE_t trip_task_cfg[2];
FH_TASK_RULE_t peri_task_cfg[2];

static FH_VOID ivs_rules_setting(FH_UINT32 ch0_width, FH_UINT32 ch0_height)
{
#if (FH_APP_TRIP_NUM > 0)
    FH_SINT32 trip_curminHits = 2;
    FH_SINT32 trip_preminHits = 2;
#endif

#if (FH_APP_PERI_NUM > 0)
    FH_SINT32 peri_curminHits = 2;
    FH_SINT32 peri_preminHits = 2;
#endif

#if (FH_APP_TRIP_NUM > 0)
    trip_task_cfg[0].ruleid = 0;
    trip_task_cfg[0].enable = 1;
    trip_task_cfg[0].curminHits = trip_curminHits;
    trip_task_cfg[0].preminHits = trip_preminHits;
    trip_task_cfg[0].mode = TAD_TRIP_ALL;
    trip_task_cfg[0].trip_cfg.startPoint.x = ch0_width / 2; //左进右出
    trip_task_cfg[0].trip_cfg.startPoint.y = ch0_height;
    trip_task_cfg[0].trip_cfg.endPoint.x = ch0_width / 2;
    trip_task_cfg[0].trip_cfg.endPoint.y = 0;
    draw_gbox(trip_task_cfg[0].trip_cfg.endPoint.x, trip_task_cfg[0].trip_cfg.endPoint.y, 4, ch0_height, 2);
#endif

#if (FH_APP_TRIP_NUM > 1)
    trip_task_cfg[1].ruleid = 1;
    trip_task_cfg[1].enable = 1;
    trip_task_cfg[1].curminHits = trip_curminHits;
    trip_task_cfg[1].preminHits = trip_preminHits;
    trip_task_cfg[1].mode = TAD_TRIP_ALL;
    trip_task_cfg[1].trip_cfg.startPoint.x = ch0_width; //上出下进
    trip_task_cfg[1].trip_cfg.startPoint.y = ch0_height / 2;
    trip_task_cfg[1].trip_cfg.endPoint.x = 0;
    trip_task_cfg[1].trip_cfg.endPoint.y = ch0_height / 2;
    draw_gbox(trip_task_cfg[1].trip_cfg.endPoint.x, trip_task_cfg[1].trip_cfg.endPoint.y, ch0_width, 4, 2);
#endif

#if (FH_APP_PERI_NUM > 0)
    peri_task_cfg[0].ruleid = 2;
    peri_task_cfg[0].enable = 1;
    peri_task_cfg[0].curminHits = peri_curminHits;
    peri_task_cfg[0].preminHits = peri_preminHits;
    peri_task_cfg[0].mode = TAD_AREA_ALL;
    peri_task_cfg[0].area_cfg.iTimeMin = 10;
    peri_task_cfg[0].area_cfg.iPointNum = 4;
    peri_task_cfg[0].area_cfg.rectPoint[0].x = ch0_width / 6;
    peri_task_cfg[0].area_cfg.rectPoint[0].y = ch0_height / 6;
    peri_task_cfg[0].area_cfg.rectPoint[1].x = ch0_width * 2 / 6;
    peri_task_cfg[0].area_cfg.rectPoint[1].y = ch0_height / 6;
    peri_task_cfg[0].area_cfg.rectPoint[2].x = ch0_width * 2 / 6;
    peri_task_cfg[0].area_cfg.rectPoint[2].y = ch0_height * 5 / 6;
    peri_task_cfg[0].area_cfg.rectPoint[3].x = ch0_width / 6;
    peri_task_cfg[0].area_cfg.rectPoint[3].y = ch0_height * 5 / 6;
    draw_gbox(peri_task_cfg[0].area_cfg.rectPoint[0].x, peri_task_cfg[0].area_cfg.rectPoint[0].y, ch0_width * 1 / 6, ch0_height * 4 / 6, 2);
#endif

#if (FH_APP_PERI_NUM > 1)
    peri_task_cfg[0].ruleid = 3;
    peri_task_cfg[0].enable = 1;
    peri_task_cfg[0].curminHits = peri_curminHits;
    peri_task_cfg[0].preminHits = peri_preminHits;
    peri_task_cfg[1].mode = TAD_AREA_ALL;
    peri_task_cfg[1].area_cfg.iTimeMin = 10;
    peri_task_cfg[1].area_cfg.iPointNum = 4;
    peri_task_cfg[1].area_cfg.rectPoint[0].x = ch0_width * 4 / 6;
    peri_task_cfg[1].area_cfg.rectPoint[0].y = ch0_height / 6;
    peri_task_cfg[1].area_cfg.rectPoint[1].x = ch0_width * 5 / 6;
    peri_task_cfg[1].area_cfg.rectPoint[1].y = ch0_height / 6;
    peri_task_cfg[1].area_cfg.rectPoint[2].x = ch0_width * 5 / 6;
    peri_task_cfg[1].area_cfg.rectPoint[2].y = ch0_height * 5 / 6;
    peri_task_cfg[1].area_cfg.rectPoint[3].x = ch0_width * 4 / 6;
    peri_task_cfg[1].area_cfg.rectPoint[3].y = ch0_height * 5 / 6;
    draw_gbox(peri_task_cfg[1].area_cfg.rectPoint[0].x, peri_task_cfg[1].area_cfg.rectPoint[0].y, ch0_width * 1 / 6, ch0_height * 4 / 6, 2);
#endif
}

static FH_SINT32 get_bgm_info(FH_TAD_GROUP_t *tad_in, int chn0_width, int chn0_height)
{
    FH_SINT32 ret = 0;
    unsigned char *foreground = NULL;
    Motion_BGM_RUNTB_RECT GauRun;
    FH_BGM_SW_STATUS bgm_status; /*bgm检测结果*/
    GauRun.rect = NULL;

    /***** get bgm box value *****/
    memset(&bgm_status, 0, sizeof(FH_BGM_SW_STATUS));
    ret = FH_BGM_GetSWStatus(0, &bgm_status);
    if (ret) /*获取背景建模信息*/
    {
        printf("FH_BGM_GetSWStatus failed with:(%x-%d)!\n", ret, ret);
        goto Exit;
    }

    // FILE *fsw;
    // char file_name[80];
    // char dst_file[80] = "240x136";
    // sprintf(file_name, "%s_sw.hex", dst_file);
    // OPEN_FILE(fsw, file_name, "ab");
    // fwrite(bgm_status.confidence_level.addr, 1, bgm_status.confidence_level.size, fsw);
    // CLOSE_FILE(fsw);

    if (!foreground)
    {
        foreground = malloc(bgm_status.confidence_level.size);
        if (!foreground)
        {
            printf("Error: no memory for BGM data!\n");
            ret = -1;
            goto Exit;
        }
    }

    memcpy(foreground, bgm_status.confidence_level.addr, bgm_status.confidence_level.size); // 置信度 0 - 7

    /***** get frame box value *****/

    GauRun.md_ex_result_addr = foreground;                                         //置信度16像素采样
    GauRun.horizontal_count = ((bgm_status.size.u32Width / BGMSZ + 15) & ~15) / 2; // 8采样后，16对齐，精度问题，每4个合为1个置信度
    GauRun.vertical_count = bgm_status.size.u32Height / BGMSZ / 2;
    GauRun.area_th_w = 10; // FIXME
    GauRun.area_th_h = 10; // FIXME
    GauRun.rect_num = 20;
    GauRun.rect = malloc(sizeof(struct _rect) * 20);
    if (!GauRun.rect)
    {
        printf("Error: no memory for BGM data!\n");
        ret = -1;
        goto Exit;
    }

    getOrdFromGau(&GauRun, 7);
#if TEST
    static int g_count = 0;
    if (g_count < 20)
    {
        printf("g_count:%d\n", g_count);

        int i, j;
        int posline, pos;
        FILE *fp;
        char name[10] = {'\0'};
        sprintf(name, "bgm_%d.txt", g_count);
        fp = fopen(name, "w");

        for (i = 0; i < GauRun.vertical_count; i++)
        {
            posline = i * GauRun.horizontal_count;
            for (j = 0; j < GauRun.horizontal_count; j++)
            {
                pos = posline + j;
                fprintf(fp, "%d", foreground[pos]);
            }
            fprintf(fp, "\n");
        }
        // fwrite(foreground, bgm_status.confidence_level.size, 1, fp);
        fclose(fp);

        FILE *fp1;
        char name1[10] = {'\0'};
        sprintf(name1, "gau_%d.txt", g_count);
        fp1 = fopen(name1, "w");
        printf("GauRun.rect_num:%d\n", GauRun.rect_num);
        fprintf(fp1, "gau数量：%d\n", GauRun.rect_num);
        fclose(fp1);
        g_count++;
    }
#endif

    if (GauRun.rect_num == 0)
    {
        ret = -1;
    }

    /***** get tad_in value *****/
    tad_in->num = GauRun.rect_num;

    for (int i = 0; i < tad_in->num; i++)
    {
        tad_in->object[i].pos.x = GauRun.rect[i].u32X * BGMSZ * 2 * chn0_width / bgm_status.size.u32Width;
        tad_in->object[i].pos.y = GauRun.rect[i].u32Y * BGMSZ * 2 * chn0_height / bgm_status.size.u32Height;
        tad_in->object[i].pos.w = GauRun.rect[i].u32Width * BGMSZ * 2 * chn0_width / bgm_status.size.u32Width;
        tad_in->object[i].pos.h = GauRun.rect[i].u32Height * BGMSZ * 2 * chn0_height / bgm_status.size.u32Height;
        draw_gbox(tad_in->object[i].pos.x, tad_in->object[i].pos.y, tad_in->object[i].pos.w, tad_in->object[i].pos.h, 0);
    }

Exit:
    if (foreground)
    {
        free(foreground);
        foreground = NULL;
    }
    if (GauRun.rect)
    {
        free(GauRun.rect);
        GauRun.rect = NULL;
    }

    return ret;
}

static FH_VOID res_deal(FH_TAD_GROUP_t *tad_data)
{
    FH_SINT32 i;
    for (i = 0; i < tad_data->num; i++)
    {
        if (tad_data->object[i].objId != -1)
        {
            switch (tad_data->object[i].mode)
            {
            case TAD_NO:
                printf("TAD_NO\n");
                break;
            case TAD_TRIP_LTOR:
                printf("TAD_TRIP_LTOR\n");
                break;
            case TAD_TRIP_RTOL:
                printf("TAD_TRIP_RTOL\n");
                break;
            case TAD_TRIP_ALL:
                printf("TAD_TRIP_ALL\n");
                break;
            case TAD_AREA_ENTER:
                printf("TAD_AREA_ENTER\n");
                break;
            case TAD_AREA_EXIT:
                printf("TAD_AREA_EXIT\n");
                break;
            case TAD_AREA_INTRUSION:
                printf("TAD_AREA_INTRUSION\n");
                break;
            case TAD_AREA_ALL:
                printf("TAD_AREA_ALL\n");
                break;
            }
            // printf("[BGM TAD] id=%d,mode=%d,pos x=%d,y=%d,w=%d,h=%d\n", tad_data->object[i].objId, tad_data->object[i].mode, tad_data->object[i].pos.x, tad_data->object[i].pos.y, tad_data->object[i].pos.w, tad_data->object[i].pos.h);
        }
    }
}

static FH_VOID *ivs_task(FH_VOID *arg)
{
    FH_SINT32 i;
    FH_SINT32 ret = -1;

    TAD_HANDLE tadHandle = NULL;
    FH_TAD_GROUP_t tad_in;
    FH_TAD_GROUP_t tad_out;
    FH_BGM_SW_STATUS bgm_status; /*bgm检测结果*/
    struct vpu_channel_info chn_info;

    sample_common_dsp_get_vpu_chn_info(FH_APP_GRP_ID, 0, &chn_info);
    FH_UINT32 ch0_width = chn_info.width;
    FH_UINT32 ch0_height = chn_info.height;

    ret = sample_common_bgm_get_init_status(FH_APP_GRP_ID);

    if (ret != 0)
    {
        ret = sample_common_bgm_init();
        if (ret)
        {
            printf("[bgm_Init] failed with:%d\n", ret);
            goto Exit;
        }
    }

    int (*callback)(const char *format, ...) = debugCallback;
    FH_TAD_PrintRegister(callback);

    tadHandle = tad_init(); // tad 初始化
    if (tadHandle == NULL)
    {
        printf("Error[FH_NN_PF]: tad init failed\n");
        goto Exit;
    }

    ivs_rules_setting(ch0_width, ch0_height);

    for (i = 0; i < 20; i++) // wait for bgm ready
    {
        ret = FH_BGM_GetSWStatus(0, &bgm_status);
        if (ret)
        {
            usleep(10 * 1000);
        }
        else
        {
            break;
        }
    }

    prctl(PR_SET_NAME, "demo_ivs");

    while (!g_ivs_stop)
    {
        clear_screen(FH_APP_TRIP_NUM + FH_APP_PERI_NUM);

        if (get_bgm_info(&tad_in, ch0_width, ch0_height) == 0)
        {
            for (i = 0; i < FH_APP_TRIP_NUM; i++)
            {
                FH_TAD_Process(tadHandle, &trip_task_cfg[i], &tad_in, &tad_out);
                res_deal(&tad_out);
            }

            for (i = 0; i < FH_APP_PERI_NUM; i++)
            {
                FH_TAD_Process(tadHandle, &peri_task_cfg[i], &tad_in, &tad_out);
                res_deal(&tad_out);
            }
        }

        usleep(10 * 1000);
    }

Exit:
    g_ivs_is_running = 0;
    tad_deinit(tadHandle); // tad 反初始化
    return NULL;
}

FH_SINT32 sample_ivs_start(FH_VOID)
{
    FH_SINT32 ret;
    pthread_t thread;
    pthread_attr_t attr;

    if (g_ivs_is_running)
    {
        printf("Error: ivs already running!\n");
        return -1;
    }

    g_ivs_stop = 0;
    g_ivs_is_running = 1;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize(&attr, 16 * 1024);
    ret = pthread_create(&thread, &attr, ivs_task, NULL);
    if (ret)
    {
        printf("[ERROR]: create md/cd thread failed, ret=%d\n", ret);
        g_ivs_is_running = 0;
        return ret;
    }

    return 0;
}

FH_SINT32 sample_ivs_stop(FH_VOID)
{
    while (g_ivs_is_running)
    {
        g_ivs_stop = 1;
        usleep(20 * 1000);
    }
    clear_screen(0);

    return 0;
}
