#include "isp/isp_common.h"
#include "uvc_init.h"
#include "list.h"

/* #define H264_P_ADD_S_P_PS */

#define YUY2_MAX_W            1920
#define YUY2_MAX_H            1088

#define FH_STREAM_NV12        (1 << 6)
#define FH_STREAM_YUY2        (1 << 7)
#define FH_STREAM_IR          (1 << 8)

#define ARRAY_SIZE(a)   ((sizeof(a) / sizeof(a[0])))

struct yuv_frame_buf
{
    unsigned char *ydata;
    unsigned char *uvdata;
    unsigned int ysize;
    unsigned int uvsize;
    unsigned long long time_stamp;
    struct list_head list;
};

struct yuy2_frame_buf
{
    unsigned char *data;
    unsigned int size;
    unsigned long long time_stamp;
    struct list_head list;
};

struct uvc_dev_app
{
    unsigned int fcc;
    unsigned int fps;
    unsigned int width;
    unsigned int height;
    unsigned int iframe;
    unsigned int finterval;

    unsigned int first_change;
    unsigned int isp_complete;
    unsigned int stream_id;
    unsigned int g_h264_delay;
    unsigned int g_h265_delay;
    unsigned char *yuv_buf;
    unsigned int yuv_size;
    struct list_head list_free;
    struct list_head list_used;
    struct list_head yuy2_free;
    struct list_head yuy2_used;
};

extern pthread_mutex_t mutex_sensor;

extern struct uvc_dev_app g_uvc_dev[2];

void uvc_stream_init(struct uvc_dev_app *pDev);
void uvc_stream_change(int stream_id, UVC_FORMAT fmt, int w, int h, int fps);
void fh_stream_exit(void);
unsigned int v4l2_type_to_stream_type(unsigned int vtype);

struct uvc_format_info *get_uvc_format(int stream_id);
int get_uvc_format_num(int stream_id);

unsigned int uvc_get_yuy2_data_len(void);
void convertNV12ToYUY2(unsigned char *pNV12_Y, unsigned char *pNV12_UV, unsigned char *pYUY2, int width, int height);

