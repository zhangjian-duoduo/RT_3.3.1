#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "config.h"
#include "libdmc/include/libdmc.h"
#include "libdmc/include/libdmc_record_raw.h"

#define MAX_FILE_NAME  50

struct _record_file_info
{
    unsigned int printed;
    FILE *fp;
    char file_name[MAX_FILE_NAME];
};

static struct _record_file_info *g_record_files[MAX_GRP_NUM] = {NULL};

static int _record_input_fn(int media_chn,
        int media_type,
        int media_subtype,
        unsigned long long frame_pts,
        unsigned char *frame_data,
        int frame_len,
        int frame_end_flag)
{
    unsigned int ret;
    int grpid;
    int channel;

    if (media_chn > MAX_GRP_NUM * MAX_VPU_CHN_NUM)
    {
        printf("Error: libpes, max channel num is %d!\n", media_chn);
        return 0;
    }

    grpid = media_chn / MAX_VPU_CHN_NUM;
    channel = media_chn % MAX_VPU_CHN_NUM;

    if (g_record_files[grpid][channel].fp == NULL)
    {
        if (media_type == DMC_MEDIA_TYPE_H264)
        {
            snprintf(g_record_files[grpid][channel].file_name, MAX_FILE_NAME, "grp_%d_chan_%d.h264", grpid, channel);
        }
        else if (media_type == DMC_MEDIA_TYPE_H265)
        {
            snprintf(g_record_files[grpid][channel].file_name, MAX_FILE_NAME, "grp_%d_chan_%d.h265", grpid, channel);
        }
        else if (media_type == DMC_MEDIA_TYPE_AUDIO)
        {
            snprintf(g_record_files[grpid][channel].file_name, MAX_FILE_NAME, "grp_%d_chan_%d.data", grpid, channel);
        }
        else if (media_type == DMC_MEDIA_TYPE_MJPEG)
        {
            snprintf(g_record_files[grpid][channel].file_name, MAX_FILE_NAME, "grp_%d_chan_%d.mjpeg", grpid, channel);
        }

        g_record_files[grpid][channel].fp = fopen(g_record_files[grpid][channel].file_name, "wb+");
        if (!g_record_files[grpid][channel].fp)
        {
            printf("Error: open file %s failed\n", g_record_files[grpid][channel].file_name);
            return -1;
        }
    }

    ret = fwrite(frame_data, 1, frame_len, g_record_files[grpid][channel].fp);
    if (ret < frame_len)
    {
        printf("Error: write file %s fail, <%d> <%d>\n", g_record_files[grpid][channel].file_name, ret, frame_len);
        return -1;
    }
    fflush(g_record_files[grpid][channel].fp);
    sync();

    if (!g_record_files[grpid][channel].printed)
    {
        printf("Save stream in %s\n", g_record_files[grpid][channel].file_name);
        g_record_files[grpid][channel].printed = 1;
    }

    return 0;
}

int dmc_record_subscribe(int rawgrp)
{
    int code = 0x1;
    int grpid = 0;

    for(grpid = 0; grpid <= MAX_GRP_NUM; grpid++) {
        if((rawgrp & code) && !g_record_files[grpid]) {
            g_record_files[grpid] = malloc(sizeof(struct _record_file_info) * MAX_VPU_CHN_NUM);
            if (!g_record_files[grpid]) {
                printf("Error: insufficient space for stream record\n");
                return -1;
            }
            memset(g_record_files[grpid], 0, sizeof(struct _record_file_info) * MAX_VPU_CHN_NUM);
        }
        code <<= 1;
    }

    dmc_subscribe("RECORD", rawgrp, DMC_MEDIA_TYPE_H264 | DMC_MEDIA_TYPE_H265 | DMC_MEDIA_TYPE_AUDIO | DMC_MEDIA_TYPE_MJPEG, _record_input_fn);

    return 0;
}

int dmc_record_unsubscribe(void)
{
    int i, grpid;

    dmc_unsubscribe("RECORD",  DMC_MEDIA_TYPE_H264 | DMC_MEDIA_TYPE_H265 | DMC_MEDIA_TYPE_AUDIO | DMC_MEDIA_TYPE_MJPEG);

    for(grpid = 0; grpid < MAX_GRP_NUM; grpid++) {
        if (g_record_files[grpid]) {
            for (i = 0; i < MAX_VPU_CHN_NUM; i++) {
                if (g_record_files[grpid][i].fp)
                {
                    fclose(g_record_files[grpid][i].fp);
                    g_record_files[grpid][i].fp = NULL;
                }
            }

            free(g_record_files[grpid]);
            g_record_files[grpid] = NULL;
        }
    }

    return 0;
}
