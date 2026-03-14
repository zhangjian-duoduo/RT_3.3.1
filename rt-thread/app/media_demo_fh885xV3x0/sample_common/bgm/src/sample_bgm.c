#include <sample_common.h>

static pthread_mutex_t g_bgm_lock[MAX_GRP_NUM];
static FH_SINT32     g_bgm_lock_inited[MAX_GRP_NUM] = {0};
static FH_SINT32     g_bgm_inited[MAX_GRP_NUM] = {0};

static FH_UINT8     *g_confidence_buf[MAX_GRP_NUM];      /*存放当前帧bgm前景检测结果*/
static FH_UINT32     g_confidence_bufsz[MAX_GRP_NUM];    /*动态分配缓冲区confidence_level的大小*/
static FH_UINT32     g_confidence_result_w[MAX_GRP_NUM]; /*bgm前景结果宽度*/
static FH_UINT32     g_confidence_result_h[MAX_GRP_NUM]; /*bgm前景结果高度*/

FH_SINT32 sample_common_bgm_init()
{
    FH_SINT32 ret = 0;
    FH_UINT32 init_mem_w;
    FH_UINT32 init_mem_h;
    FH_SIZE picsize;
    FH_SINT32 grploop;
    struct grp_vpu_info grp_info;
    struct vpu_channel_info vpu_info;

    for (grploop = 0; grploop < MAX_GRP_NUM; grploop++)
    {
        sample_common_dsp_get_grp_info(grploop, &grp_info);
        if (grp_info.enable && grp_info.bgm_enable)
        {

            if (g_bgm_inited[grp_info.channel])
                continue;
            if (!g_bgm_lock_inited[grp_info.channel])
            {
                pthread_mutex_init(&g_bgm_lock[grp_info.channel], NULL);
                g_bgm_lock_inited[grp_info.channel] = 1;
            }
            pthread_mutex_lock(&g_bgm_lock[grp_info.channel]);

            sample_common_dsp_get_vpu_chn_info(grploop, 0, &vpu_info);

            init_mem_w = ALIGNTO(get_vpu_chn_w(grploop, 0), 16) / grp_info.bgm_ds;
            init_mem_h = ALIGNTO(get_vpu_chn_h(grploop, 0), 16) / grp_info.bgm_ds;

            ret = FH_BGM_SetCapality(grploop, 0); // 工作模式(0:BGM_PRO,1:BGM,2:BGM_LITE)
            if (ret != 0)
            {
                pthread_mutex_unlock(&g_bgm_lock[grp_info.channel]);
                printf("Error(%d - %x): FH_BGM_SetCapality (grp-chn):(%d-0)!\n", ret, ret, grploop);
                return -1;
            }

            ret = FH_BGM_InitMem(grploop, init_mem_w, init_mem_h);
            if (ret != 0)
            {
                pthread_mutex_unlock(&g_bgm_lock[grp_info.channel]);
                printf("Error(%d - %x): FH_BGM_InitMem (grp-chn):(%d-0)!\n", ret, ret, grploop);
                return -1;
            }

            picsize.u32Width = ALIGNTO(get_vpu_chn_w(grploop, 0), 16) / grp_info.bgm_ds;
            picsize.u32Height = ALIGNTO(get_vpu_chn_h(grploop, 0), 16) / grp_info.bgm_ds;
            ret = FH_BGM_SetConfig(grp_info.channel, &picsize);
            if (ret != 0)
            {
                pthread_mutex_unlock(&g_bgm_lock[grp_info.channel]);
                printf("Error(%d - %x): FH_BGM_SetConfig (grp-chn):(%d-0)!\n", ret, ret, grploop);
                return -1;
            }

            ret = FH_BGM_Enable(grp_info.channel);
            if (ret != 0)
            {
                pthread_mutex_unlock(&g_bgm_lock[grp_info.channel]);
                printf("Error(%d - %x): FH_BGM_Enable (grp-chn):(%d-0)!\n", ret, ret, grploop);
                return -1;
            }
            g_bgm_inited[grp_info.channel] = 1;
            pthread_mutex_unlock(&g_bgm_lock[grp_info.channel]);
        }

    }
    return 0;
}

FH_SINT32 sample_common_bgm_triger_reload(FH_UINT32 grpid)
{
    int ret;
    FH_BGM_SW_STATUS bgm_status;/*bgm检测结果*/
    struct grp_vpu_info grp_info;

    if (!g_bgm_inited[grpid])
        return -1;

    pthread_mutex_lock(&g_bgm_lock[grpid]);

    sample_common_dsp_get_grp_info(grpid, &grp_info);


    /*invalid previous bgm data...*/
    g_confidence_result_w[grpid] = 0;
    g_confidence_result_h[grpid] = 0;

    ret = FH_BGM_GetSWStatus(grpid, &bgm_status);
    if (ret != 0)/*获取背景建模信息*/
    {
        printf("Error: FH_BGM_GetSWStatus(%d-%d)!\n", ret, ret);
        goto Exit;
    }

    if (bgm_status.confidence_level.size > g_confidence_bufsz[grpid])
    {
        if (g_confidence_buf[grpid])
        {
            free(g_confidence_buf[grpid]);
            g_confidence_buf[grpid] = NULL;
        }
        g_confidence_bufsz[grpid] = 0;

        g_confidence_buf[grpid] = malloc(bgm_status.confidence_level.size);
        if (!g_confidence_buf[grpid])
        {
            printf("Error: no memory for BGM data!\n");
            ret = -1;
            goto Exit;
        }

        g_confidence_bufsz[grpid] = bgm_status.confidence_level.size;
    }

    memcpy(g_confidence_buf[grpid], bgm_status.confidence_level.addr, bgm_status.confidence_level.size);
    g_confidence_result_w[grpid]  = bgm_status.size.u32Width  / grp_info.bgm_ds;
    g_confidence_result_h[grpid]  = bgm_status.size.u32Height / grp_info.bgm_ds;

Exit:
    pthread_mutex_unlock(&g_bgm_lock[grpid]);

    return ret;
}

FH_SINT32 sample_common_bgm_get_outer_motion_area(
        FH_UINT32 grpid,
        FH_UINT32 *winX,
        FH_UINT32 *winY,
        FH_UINT32 *winW,
        FH_UINT32 *winH,
        FH_UINT32 *picW,
        FH_UINT32 *picH,
        FH_UINT8   thresh)
{
    struct grp_vpu_info grp_info;

    if (!g_bgm_inited[grpid])
        return -1;

    pthread_mutex_lock(&g_bgm_lock[grpid]);

    sample_common_dsp_get_grp_info(grpid, &grp_info);

    if (g_confidence_result_w[grpid] <= 0) /*not valid*/
    {
        pthread_mutex_unlock(&g_bgm_lock[grpid]);
        return -1;
    }

    libmisc_get_outer_box(g_confidence_buf[grpid],
            g_confidence_result_w[grpid],
            g_confidence_result_h[grpid],
            winX, winY, winW, winH, thresh); /* get max outer box */
    *winX *= grp_info.bgm_ds;
    *winY *= grp_info.bgm_ds;
    *winW *= grp_info.bgm_ds;
    *winH *= grp_info.bgm_ds;
    *picW = g_confidence_result_w[grpid] * grp_info.bgm_ds;
    *picH = g_confidence_result_h[grpid] * grp_info.bgm_ds;

    pthread_mutex_unlock(&g_bgm_lock[grpid]);

    return 0;
}

FH_SINT32 sample_common_bgm_get_multi_motion_area(
        FH_UINT32 grpid,
        BGM_Rect *motion_rects,
        FH_UINT32 *rect_num,
        FH_UINT32 max_rect_num,
        FH_UINT8   thresh)
{

    if (!g_bgm_inited[grpid])
    {
        *rect_num = 0;
        printf("bgm is not init\n");
        return -1;
    }

    pthread_mutex_lock(&g_bgm_lock[grpid]);

    if (g_confidence_result_w[grpid] <= 0) /*not valid*/
    {
        pthread_mutex_unlock(&g_bgm_lock[grpid]);
        return -1;
    }

    Motion_BGM_RUNTB_RECT md_box;
    FH_SINT32 i;

    struct _rect *rect = malloc(max_rect_num * sizeof(struct _rect));

    md_box.md_ex_result_addr = g_confidence_buf[grpid];
    md_box.horizontal_count  = g_confidence_result_w[grpid];
    md_box.vertical_count    = g_confidence_result_h[grpid];
    md_box.area_th_w         = 2;
    md_box.area_th_h         = 2;
    md_box.rect_num          = max_rect_num;
    md_box.rect              = rect;

    getOrdFromGau(&md_box, thresh);

    pthread_mutex_unlock(&g_bgm_lock[grpid]);

    for (i = 0; i < md_box.rect_num; i++)
    {
        motion_rects[i].x = rect[i].u32X;
        motion_rects[i].y = rect[i].u32Y;
        motion_rects[i].w = rect[i].u32Width;
        motion_rects[i].h = rect[i].u32Height;
    }

    *rect_num = md_box.rect_num;
    free(rect);

    return 0;
}

FH_SINT32 sample_common_bgm_get_statistics_sum(
        FH_UINT32 grpid,
        FH_UINT32 detect_frame_w,
        FH_UINT32 detect_frame_h,
        FH_UINT32 x,
        FH_UINT32 y,
        FH_UINT32 w,
        FH_UINT32 h,
        FH_UINT32 *sum,
        FH_UINT32 *blk_num)
{
    FH_SINT32 i;
    FH_SINT32 j;
    FH_UINT32 mbx;
    FH_UINT32 mby;
    FH_UINT32 mbw;
    FH_UINT32 mbh;
    FH_UINT32 confidence_sum = 0;
    FH_UINT8* confidence;
    struct grp_vpu_info grp_info;

    if (!g_bgm_inited[grpid])
        return -1;

    pthread_mutex_lock(&g_bgm_lock[grpid]);

    sample_common_dsp_get_grp_info(grpid, &grp_info);

    if (g_confidence_result_w[grpid] <= 0)
    {
        pthread_mutex_unlock(&g_bgm_lock[grpid]);
        return -1;
    }

    mbx = (x * (g_confidence_result_w[grpid] * grp_info.bgm_ds) / detect_frame_w) / grp_info.bgm_ds;
    mby = (y * (g_confidence_result_h[grpid] * grp_info.bgm_ds) / detect_frame_h) / grp_info.bgm_ds;
    mbw = (w * (g_confidence_result_w[grpid] * grp_info.bgm_ds) / detect_frame_w + 15) / grp_info.bgm_ds;
    mbh = (h * (g_confidence_result_h[grpid] * grp_info.bgm_ds) / detect_frame_h + 15) / grp_info.bgm_ds;

    confidence = g_confidence_buf[grpid] + mby * g_confidence_result_w[grpid] + mbx;

    for (i = 0; i < mbh; i++)
    {
        for (j = 0; j < mbw; j++)
        {
            confidence_sum += confidence[j];
        }
        confidence += g_confidence_result_w[grpid];
    }

    pthread_mutex_unlock(&g_bgm_lock[grpid]);

    *sum = confidence_sum;
    *blk_num = mbw * mbh;

    return 0;
}

FH_SINT32 sample_common_bgm_exit()
{
    FH_SINT32 ret;
    FH_SINT32 grploop;
    struct grp_vpu_info grp_info;

    for (grploop = 0; grploop < MAX_GRP_NUM; grploop++)
    {
        sample_common_dsp_get_grp_info(grploop, &grp_info);
        if (grp_info.enable && grp_info.bgm_enable)
        {
            if (g_bgm_lock_inited[grp_info.channel])
            {
                pthread_mutex_lock(&g_bgm_lock[grp_info.channel]);
            }

            ret = FH_BGM_Disable(grp_info.channel);
            if (ret)
            {
                printf("ret = %d\n", ret);
            }

            if (g_confidence_buf[grp_info.channel])
            {
                free(g_confidence_buf[grp_info.channel]);
                g_confidence_buf[grp_info.channel] = NULL;
            }
            g_confidence_bufsz[grp_info.channel] = 0;
            g_confidence_result_w[grp_info.channel] = 0;
            g_confidence_result_h[grp_info.channel] = 0;
            g_bgm_inited[grp_info.channel] = 0;

            if (g_bgm_lock_inited[grp_info.channel])
            {
                pthread_mutex_unlock(&g_bgm_lock[grp_info.channel]);
            }
        }
    }
    return 0;
}

FH_SINT32 sample_common_bgm_get_init_status(FH_UINT32 grpid)
{
    return g_bgm_inited[grpid];
}