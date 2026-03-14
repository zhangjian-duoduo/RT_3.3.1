#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include "sample_common_dsp.h"
#include "FHAdv_Isp_mpi_v3.h"
#include "isp/isp_api.h"
#include "usb_video.h"
#include "uvc_init.h"
#include "uvc_extern.h"
#include "uvc_info.h"
#include "uvc_callback.h"
#include "rtconfig_app.h"
#include <sys/prctl.h>

#define UVC_VERSION "20200915_v2.2"
struct uvc_dev_app g_uvc_dev[2];
static pthread_t g_thread_video_stream[2];
static pthread_t g_thread_yuv;
#if defined(CONFIG_ARCH_FH8636_FH8852V20X) || defined(CONFIG_CHIP_FH8626V200)
static pthread_t g_thread_to_yuy2;
#endif
static int g_uvc_th_status = 0;

pthread_mutex_t mutex_sensor;
static pthread_mutex_t mutex_list_free;
static pthread_mutex_t mutex_list_used;
#if defined(CONFIG_ARCH_FH8636_FH8852V20X) || defined(CONFIG_CHIP_FH8626V200)
static pthread_mutex_t mutex_yuy2_free;
static pthread_mutex_t mutex_yuy2_used;
#endif
#define BUF_NUM 1
static struct yuv_frame_buf yuv_buf[BUF_NUM];

static int ir_fream_flag = 0;
extern int ir_stream_on;
extern void *fh_dma_memcpy(void *dst, const void *src, int len);


static struct uvc_stream_ops uvc_ops = {
    .format_change = uvc_stream_change,
#ifdef FH_APP_USING_HEX_PARA
    .uvc_extern_data = uvc_extern_intr_ctrl,
    .uvc_extern_intr = uvc_extern_intr_proc,
    .uvc_vision      = uvc_info_option,
#ifdef FH_USING_USER_PARAM
    .uvc_uspara      = uvc_para_read,
#endif
#endif
    .stream_on_callback = uvc_stream_on,
    .stream_off_callback = uvc_stream_off,

    .SetAEMode = Uvc_SetAEMode,
    .SetExposure = Uvc_SetExposure,
    .SetGain = Uvc_SetGain,
    .SetBrightness = Uvc_SetBrightness,
    .SetContrast = Uvc_SetContrast,
    .SetSaturation = Uvc_SetSaturation,
    .SetSharpeness = Uvc_SetSharpeness,
    .SetAWBGain = Uvc_SetAWBGain,
    .SetAwbMode = Uvc_SetAwbMode,
};

#ifdef H264_P_ADD_S_P_PS
static unsigned char *h264_sps_buf = NULL;
static unsigned char *h264_pps_buf = NULL;
static unsigned int h264_sps_len = 0;
static unsigned int h264_pps_len = 0;
#endif

static int single_stream_proc(struct uvc_dev_app *pDev)
{
    FH_VENC_STREAM stream;
    uint32_t ret = -1;
    uint32_t isp_format;
    uint32_t pformat;
    uint32_t chan = 0, len = 0;
    uint8_t *start;
    uint64_t pts = 0;

    pformat = pDev->fcc;
    isp_format = v4l2_type_to_stream_type(pformat);
    if (isp_format == FH_STREAM_NV12 || isp_format == FH_STREAM_YUY2)
    {
#if defined(CONFIG_ARCH_FH8636_FH8852V20X) || defined(CONFIG_CHIP_FH8626V200)
        if (isp_format == FH_STREAM_YUY2)
        {
            struct yuy2_frame_buf *yuy2_buf;
            pthread_mutex_lock(&mutex_yuy2_used);
            if (!list_empty(&pDev->yuy2_used))
            {
                yuy2_buf = list_first_entry(&pDev->yuy2_used, struct yuy2_frame_buf, list);
                if (yuy2_buf)
                {
                    list_del(&yuy2_buf->list);
                    pthread_mutex_unlock(&mutex_yuy2_used);
                    if (ir_stream_on)
                    {
                        if (ir_fream_flag == 1)
                        {
                            ir_fream_flag++;
                            fh_uvc_stream_pts(pDev->stream_id, yuy2_buf->time_stamp);
                            fh_uvc_stream_enqueue(pDev->stream_id, yuy2_buf->data, yuy2_buf->size);
                        }
                    } else
                    {
                        fh_uvc_stream_pts(pDev->stream_id, yuy2_buf->time_stamp);
                        fh_uvc_stream_enqueue(pDev->stream_id, yuy2_buf->data, yuy2_buf->size);
                    }
                    pthread_mutex_lock(&mutex_yuy2_free);
                    list_add_tail(&yuy2_buf->list, &pDev->yuy2_free);
                    pthread_mutex_unlock(&mutex_yuy2_free);
                } else
                    pthread_mutex_unlock(&mutex_yuy2_used);
            } else
            {
                pthread_mutex_unlock(&mutex_yuy2_used);
                usleep(1000);
            }
        }
        else
#endif
        {
            struct yuv_frame_buf *yuv_buf;
            pthread_mutex_lock(&mutex_list_used);
            if (!list_empty(&pDev->list_used))
            {
                yuv_buf = list_first_entry(&pDev->list_used, struct yuv_frame_buf, list);
                if (yuv_buf)
                {
                    list_del(&yuv_buf->list);
                    pthread_mutex_unlock(&mutex_list_used);
                    fh_uvc_stream_pts(pDev->stream_id, yuv_buf->time_stamp);
                    fh_uvc_stream_enqueue(pDev->stream_id, yuv_buf->ydata, yuv_buf->ysize);
                    pthread_mutex_lock(&mutex_list_free);
                    list_add_tail(&yuv_buf->list, &pDev->list_free);
                    pthread_mutex_unlock(&mutex_list_free);
                } else
                    pthread_mutex_unlock(&mutex_list_used);
            } else
            {
                pthread_mutex_unlock(&mutex_list_used);
                usleep(1000);
            }
        }

    }
    else if (pformat == V4L2_PIX_FMT_IR)
    {
        FH_VPU_STREAM_ADV stYuvStream;
        FH_UINT32 handle;
        uint32_t width;
        uint32_t height;
        width = pDev->width;
        height = pDev->height;
        if (fh_uvc_status_get(pDev->stream_id) == UVC_STREAM_ON)
        {
            ret = FH_VPSS_LockChnFrameAdv(FH_GRP_ID, pDev->stream_id, &stYuvStream, 1000, &handle);
            if (ret == RETURN_OK)
            {
                len = width * height;
                pts   = stYuvStream.time_stamp;
                start   = stYuvStream.frm_scan.luma.data.vbase;
                fh_uvc_stream_pts(pDev->stream_id, pts);
                if (ir_stream_on && ir_fream_flag == 0)
                {
                    ir_fream_flag++;
                }
                else if (ir_stream_on && ir_fream_flag == 2)
                {
                    ir_fream_flag = 0;
                }
                else
                {
                    FH_VPSS_UnlockChnFrameAdv(FH_GRP_ID, pDev->stream_id, &stYuvStream, handle);
                    return 0;
                }
                fh_uvc_stream_enqueue(pDev->stream_id, start, len);
                FH_VPSS_UnlockChnFrameAdv(FH_GRP_ID, pDev->stream_id, &stYuvStream, handle);
            }
        }
    }
    else
    {
        ret = FH_VENC_GetStream_Block(isp_format, &stream);
        if (ret == RETURN_OK)
        {
            if (pformat == V4L2_PIX_FMT_MJPEG)
            {
                chan = stream.chan;
                start = stream.mjpeg_stream.start;
                len = stream.mjpeg_stream.length;
                pts = stream.mjpeg_stream.time_stamp;
                if (start[0] != 0xff)
                {
                    rt_kprintf("mjpeg error start=0x%08x len=%d\n", start, len);
                    FH_VENC_ReleaseStream(&stream);
                    return ret;
                }
                if (uvc_get_flash_mode() == IsocDataMode_Flash)
                {
                    uvc_get_flash_data(&start, &len);
                    uvc_set_flash_mode(IsocDataMode_stream);
                }
            }
            else if (pformat == V4L2_PIX_FMT_H264)
            {
                chan = stream.chan;
                start = stream.h264_stream.start;
                len = stream.h264_stream.length;
                pts = stream.h264_stream.time_stamp;
#ifdef H264_P_ADD_S_P_PS
                uint32_t type = stream.h264_stream.frame_type;

                uint32_t len_P_plus = 0;
                uint8_t *p_buf = NULL;
                uint32_t len_I_plus = 0;
                uint8_t *i_buf = NULL;
                int i = 0;

                if (chan == pDev->stream_id)
                {
                    FH_ENC_STREAM_NALU nalu_sps;
                    FH_ENC_STREAM_NALU nalu_pps;

                    if (type == FH_FRAME_I)
                    {
                        nalu_sps = stream.h264_stream.nalu[0];
                        nalu_pps = stream.h264_stream.nalu[1];
                        if (h264_sps_buf != NULL)
                        {
                            free(h264_sps_buf);
                            h264_sps_buf = NULL;
                        }
                        if (h264_pps_buf != NULL)
                        {
                            free(h264_pps_buf);
                            h264_pps_buf = NULL;
                        }
                        if (nalu_sps.type == NALU_SPS)
                        {
                            h264_sps_len = nalu_sps.length;
                            h264_sps_buf = malloc(h264_sps_len);
                            memcpy(h264_sps_buf, nalu_sps.start, h264_sps_len);
                        }
                        else
                            printf("error!!!\n");

                        if (nalu_pps.type == NALU_PPS)
                        {
                            h264_pps_len = nalu_pps.length;
                            h264_pps_buf = malloc(h264_pps_len);
                            memcpy(h264_pps_buf, nalu_pps.start, h264_pps_len);
                        }
                        else
                            printf("error!!!\n");

                        i_buf = yuv_buf[0].ydata;
                        for (i = 0; i < stream.h264_stream.nalu_cnt; i++)
                        {
                            memcpy(i_buf + len_I_plus,
                                    stream.h264_stream.nalu[i].start,
                                    stream.h264_stream.nalu[i].length);
                            len_I_plus += stream.h264_stream.nalu[i].length;
                        }
                        start = i_buf;
                        len = len_I_plus;
                    }
                    if (type == FH_FRAME_P)
                    {
                        p_buf = yuv_buf[0].ydata;
                        memcpy(p_buf, h264_sps_buf, h264_sps_len);
                        len_P_plus += h264_sps_len;
                        memcpy(p_buf + len_P_plus, h264_pps_buf, h264_pps_len);
                        len_P_plus += h264_pps_len;
                        for (i = 0; i < stream.h264_stream.nalu_cnt; i++)
                        {
                            memcpy(p_buf + len_P_plus,
                                    stream.h264_stream.nalu[i].start,
                                    stream.h264_stream.nalu[i].length);
                            len_P_plus += stream.h264_stream.nalu[i].length;
                        }

                        start = p_buf;
                        len = len_P_plus;
                    }
                }
#endif
                if (pDev->g_h264_delay > 1 && chan == pDev->stream_id)
                {
                    if (pDev->g_h264_delay == 5)
                        FH_VENC_RequestIDR(chan);
                    pDev->g_h264_delay--;
                    if (pDev->g_h264_delay == 1)
                    {
                        pDev->g_h264_delay = 0;
                        FH_VENC_RequestIDR(chan);
                    }
                }
                }
#if !defined(CONFIG_ARCH_FH8626V200)
            else if (pformat == V4L2_PIX_FMT_H265)
            {
                chan = stream.chan;
                start = stream.h265_stream.start;
                len = stream.h265_stream.length;
                pts = stream.h265_stream.time_stamp;
                if (pDev->g_h265_delay > 1 && chan == pDev->stream_id)
                {
                    pDev->g_h265_delay--;
                    if (pDev->g_h265_delay == 1)
                    {
                        pDev->g_h265_delay = 0;
                        FH_VENC_RequestIDR(chan);
                    }
                }
            }
#endif
            if (chan == pDev->stream_id || chan == (JPEG_ENC_CHN - pDev->stream_id - (FH_GRP_ID * MAX_VPU_CHN_NUM)))
            {
                if (ir_stream_on)
                {
                    if (ir_fream_flag == 1)
                    {
                        ir_fream_flag++;
                        fh_uvc_stream_pts(pDev->stream_id, pts);
                        fh_uvc_stream_enqueue(pDev->stream_id, start, len);
                    }
                } else
                {
                    fh_uvc_stream_pts(pDev->stream_id, pts);
                    fh_uvc_stream_enqueue(pDev->stream_id, start, len);
                }
            }

            FH_VENC_ReleaseStream(&stream);
        }
    }
    return ret;
}

static void yuv_buf_init(struct uvc_dev_app *pDev)
{
    int i = 0;

    INIT_LIST_HEAD(&pDev->list_free);
    INIT_LIST_HEAD(&pDev->list_used);
    pthread_mutex_init(&mutex_list_free, NULL);
    pthread_mutex_init(&mutex_list_used, NULL);
    for (i = 0; i < BUF_NUM; i++)
    {
#if defined(CONFIG_ARCH_FH8636_FH8852V20X) || defined(CONFIG_CHIP_FH8626V200)
        yuv_buf[i].ydata = malloc(YUY2_MAX_W * YUY2_MAX_H);
        if (!yuv_buf[i].ydata)
            printf("yuv_buf[%d].data is NULL!\n", i);
        yuv_buf[i].uvdata = malloc(YUY2_MAX_W * YUY2_MAX_H / 2);
        if (!yuv_buf[i].uvdata)
            printf("yuv_buf[%d].data is NULL!\n", i);
#else
        yuv_buf[i].ydata = malloc(YUY2_MAX_W * YUY2_MAX_H * 2);
        if (!yuv_buf[i].ydata)
            printf("yuv_buf[%d].data is NULL!\n", i);
#endif
        list_add(&yuv_buf[i].list, &pDev->list_free);
    }
}

extern unsigned long long read_pts(void);
static void *yuv_get_proc(void *arg)
{
    FH_VPU_STREAM_ADV stYuvStream;
    uint32_t width;
    uint32_t height;
    uint32_t isp_format;
    uint32_t pformat;
    FH_UINT32 handle;
    uint32_t ret = 0;
    unsigned int copy_cost = 0;
    unsigned int fps_sleep = 0;
    unsigned long long  copy_time = 0;
    struct uvc_dev_app *pDev = (struct uvc_dev_app *)arg;

    prctl(PR_SET_NAME, "yuv_get_proc");
    while (g_uvc_th_status)
    {
        pformat = pDev->fcc;
        width = pDev->width;
        height = pDev->height;
        isp_format = v4l2_type_to_stream_type(pformat);
        if ((isp_format == FH_STREAM_NV12 || isp_format == FH_STREAM_YUY2) && fh_uvc_status_get(pDev->stream_id) == UVC_STREAM_ON)
        {
            struct yuv_frame_buf *yuv_buf;

            fps_sleep = (1000 / pDev->fps) - copy_cost;
            usleep(fps_sleep * 1000);
            copy_time = read_pts();
            ret = FH_VPSS_LockChnFrameAdv(FH_GRP_ID, pDev->stream_id, &stYuvStream, 1000, &handle);
            if (ret == RETURN_OK)
            {
                pthread_mutex_lock(&mutex_list_free);
                if (!list_empty(&pDev->list_free))
                {
                    yuv_buf = list_first_entry(&pDev->list_free, struct yuv_frame_buf, list);
                    if (yuv_buf)
                    {
                        list_del(&yuv_buf->list);
                        pthread_mutex_unlock(&mutex_list_free);
                    }
                    else
                    {
                        pthread_mutex_unlock(&mutex_list_free);
                        printf("%s--%d yuv_buf is NULL\n", __func__, __LINE__);
                        FH_VPSS_UnlockChnFrameAdv(FH_GRP_ID, pDev->stream_id, &stYuvStream, handle);
                        continue;
                    }
                } else
                {
                    pthread_mutex_unlock(&mutex_list_free);
                    pthread_mutex_lock(&mutex_list_used);
                    if (!list_empty(&pDev->list_used))
                    {
                        yuv_buf = list_first_entry(&pDev->list_used, struct yuv_frame_buf, list);
                        if (yuv_buf)
                        {
                            list_del(&yuv_buf->list);
                            pthread_mutex_unlock(&mutex_list_used);
                        }
                        else
                        {
                            pthread_mutex_unlock(&mutex_list_used);
                            printf("%s--%d yuv_buf is NULL\n", __func__, __LINE__);
                            FH_VPSS_UnlockChnFrameAdv(FH_GRP_ID, pDev->stream_id, &stYuvStream, handle);
                            continue;
                        }
                    } else
                    {
                        pthread_mutex_unlock(&mutex_list_used);
                        FH_VPSS_UnlockChnFrameAdv(FH_GRP_ID, pDev->stream_id, &stYuvStream, handle);
                        usleep(1000);
                        continue;
                    }
                }
                yuv_buf->ysize = width * height;
                yuv_buf->uvsize = yuv_buf->ysize >> 1;

                if (isp_format == FH_STREAM_YUY2)
                {
                    yuv_buf->ysize = width * height * 2;
                    yuv_buf->uvsize = 0;
                    fh_dma_memcpy(yuv_buf->ydata, stYuvStream.frm_yuyv.data.vbase, yuv_buf->ysize);
                } else
                {
                    fh_dma_memcpy(yuv_buf->ydata, stYuvStream.frm_scan.luma.data.vbase, yuv_buf->ysize);
                    fh_dma_memcpy(yuv_buf->ydata + yuv_buf->ysize, stYuvStream.frm_scan.chroma.data.vbase, yuv_buf->uvsize);
                }
                yuv_buf->ysize = yuv_buf->ysize + yuv_buf->uvsize;

                yuv_buf->time_stamp = stYuvStream.time_stamp;
                FH_VPSS_UnlockChnFrameAdv(FH_GRP_ID, pDev->stream_id, &stYuvStream, handle);
                pthread_mutex_lock(&mutex_list_used);
                list_add_tail(&yuv_buf->list, &pDev->list_used);
                pthread_mutex_unlock(&mutex_list_used);
                copy_cost = (uint32_t)((read_pts() - copy_time) / 1000);
            }
        }
        else
            usleep(1000);
    }
    return NULL;
}

#if defined(CONFIG_ARCH_FH8636_FH8852V20X) || defined(CONFIG_CHIP_FH8626V200)
static struct yuy2_frame_buf yuy2_buf[BUF_NUM];
static void yuy2_init(struct uvc_dev_app *pDev)
{
    int i = 0;
    INIT_LIST_HEAD(&pDev->yuy2_free);
    INIT_LIST_HEAD(&pDev->yuy2_used);
    pthread_mutex_init(&mutex_yuy2_free, NULL);
    pthread_mutex_init(&mutex_yuy2_used, NULL);
    for (i = 0; i < BUF_NUM; i++)
    {
        yuy2_buf[i].data = malloc(YUY2_MAX_W * YUY2_MAX_H * 2);
        if (!yuy2_buf[i].data)
            printf("yuy2_buf[%d].data is NULL!\n", i);
        list_add(&yuy2_buf[i].list, &pDev->yuy2_free);
    }
}
static void *yuy2_proc(void *arg)
{
    uint32_t isp_format;
    uint32_t pformat;
    uint32_t width;
    uint32_t height;
    struct uvc_dev_app *pDev = (struct uvc_dev_app *)arg;
    prctl(PR_SET_NAME, "yuy2_proc");
    while (g_uvc_th_status)
    {
        pformat = pDev->fcc;
        width = pDev->width;
        height = pDev->height;
        isp_format = v4l2_type_to_stream_type(pformat);
        if (isp_format == FH_STREAM_YUY2 && fh_uvc_status_get(pDev->stream_id) == UVC_STREAM_ON)
        {
            struct yuv_frame_buf *yuv_buf;
            struct yuy2_frame_buf *yuy2_buf;
            pthread_mutex_lock(&mutex_yuy2_free);
            if (!list_empty(&pDev->yuy2_free))
            {
                yuy2_buf = list_first_entry(&pDev->yuy2_free, struct yuy2_frame_buf, list);
                if (yuy2_buf)
                {
                    list_del(&yuy2_buf->list);
                    pthread_mutex_unlock(&mutex_yuy2_free);
                }
                else
                {
                    pthread_mutex_unlock(&mutex_yuy2_free);
                    printf("%s--%d yuv_buf is NULL\n", __func__, __LINE__);
                    continue;
                }
            }
            else
            {
                pthread_mutex_unlock(&mutex_yuy2_free);
                usleep(1000);
                continue;
            }
            pthread_mutex_lock(&mutex_list_used);
            if (!list_empty(&pDev->list_used))
            {
                yuv_buf = list_first_entry(&pDev->list_used, struct yuv_frame_buf, list);
                if (yuv_buf)
                {
                    list_del(&yuv_buf->list);
                    pthread_mutex_unlock(&mutex_list_used);
                }
                else
                {
                    pthread_mutex_unlock(&mutex_list_used);
                    printf("%s--%d yuv_buf is NULL\n", __func__, __LINE__);
                    pthread_mutex_lock(&mutex_yuy2_free);
                    list_add_tail(&yuy2_buf->list, &pDev->yuy2_free);
                    pthread_mutex_unlock(&mutex_yuy2_free);
                    continue;
                }
            } else
            {
                pthread_mutex_unlock(&mutex_list_used);
                usleep(1000);
                pthread_mutex_lock(&mutex_yuy2_free);
                list_add_tail(&yuy2_buf->list, &pDev->yuy2_free);
                pthread_mutex_unlock(&mutex_yuy2_free);
                continue;
            }
            convertNV12ToYUY2(yuv_buf->ydata, yuv_buf->uvdata, yuy2_buf->data, width, height);
            yuy2_buf->size = uvc_get_yuy2_data_len();
            yuy2_buf->time_stamp = yuv_buf->time_stamp;
            pthread_mutex_lock(&mutex_yuy2_used);
            list_add_tail(&yuy2_buf->list, &pDev->yuy2_used);
            pthread_mutex_unlock(&mutex_yuy2_used);
            pthread_mutex_lock(&mutex_list_free);
            list_add_tail(&yuv_buf->list, &pDev->list_free);
            pthread_mutex_unlock(&mutex_list_free);
        }
        else
            usleep(1000);
    }
    return NULL;
}
#endif
static void *video_stream_proc(void *arg)
{
    struct uvc_dev_app *pDev = (struct uvc_dev_app *)arg;

    prctl(PR_SET_NAME, "video_stream_proc");
    while (g_uvc_th_status)
    {
        single_stream_proc(pDev);
        if (fh_uvc_status_get(pDev->stream_id) != UVC_STREAM_ON)
        {
            usleep(10000);
        }
    }

    fh_stream_exit();
    printf("uvc exit %s\n", UVC_VERSION);
    return NULL;
}

int uvc_init(void)
{
    struct sched_param param;
    pthread_attr_t attr;

    if (!g_uvc_th_status)
        g_uvc_th_status = 1;
    else
    {
        g_uvc_th_status = 0;
        return 0;
    }

    g_uvc_dev[STREAM_ID1].stream_id = STREAM_ID1;
#ifdef UVC_DOUBLE_STREAM
    g_uvc_dev[STREAM_ID2].stream_id = STREAM_ID2;
#endif
    /* printf("uvc run %s\n", UVC_VERSION); */
    fh_uvc_flash_init();
    fh_uvc_ops_register(&uvc_ops);
    fh_uvc_init(STREAM_ID1, get_uvc_format(STREAM_ID1), get_uvc_format_num(STREAM_ID1));
#ifdef UVC_DOUBLE_STREAM
    fh_uvc_init(STREAM_ID2, get_uvc_format(STREAM_ID2), get_uvc_format_num(STREAM_ID2));
#endif
    pthread_mutex_init(&mutex_sensor, NULL);
    uvc_stream_init(g_uvc_dev);

    yuv_buf_init(&g_uvc_dev[STREAM_ID1]);
#if defined(CONFIG_ARCH_FH8636_FH8852V20X) || defined(CONFIG_CHIP_FH8626V200)
    yuy2_init(&g_uvc_dev[STREAM_ID1]);
#endif
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize(&attr, 8*1024);

#if defined(CONFIG_ARCH_FH8636_FH8852V20X) || defined(CONFIG_CHIP_FH8626V200)
    param.sched_priority = 35;
    pthread_attr_setschedparam(&attr, &param);
    if (pthread_create(&g_thread_to_yuy2, &attr, yuy2_proc, (void *)&g_uvc_dev[STREAM_ID1]) != 0)
    {
        printf("Error: Create yuy2_proc thread failed!\n");
    }
#endif
    param.sched_priority = 31;
    pthread_attr_setschedparam(&attr, &param);
    if (pthread_create(&g_thread_yuv, &attr, yuv_get_proc, (void *)&g_uvc_dev[STREAM_ID1]) != 0)
    {
        printf("Error: Create yuv_get_proc thread failed!\n");
    }

    param.sched_priority = 30;
    pthread_attr_setschedparam(&attr, &param);

    if (pthread_create(&g_thread_video_stream[STREAM_ID1], &attr, video_stream_proc, (void *)&g_uvc_dev[STREAM_ID1]) != 0)
    {
        printf("Error: Create video_stream_proc thread failed!\n");
    }

#ifdef UVC_DOUBLE_STREAM
    param.sched_priority = 30;
    pthread_attr_setschedparam(&attr, &param);
    if (pthread_create(&g_thread_video_stream[STREAM_ID2], &attr, video_stream_proc, (void *)&g_uvc_dev[STREAM_ID2]) != 0)
    {
        printf("Error: Create video_stream_proc2 thread failed!\n");
    }
#endif
    printf("%s uvc_init finish!\n", __func__);
    return 0;
}
