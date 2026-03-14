#include <sample_common.h>

#define HTTP_MJPEG_PORT      (1111)
#define YUV_BUF_NUM 3

enum stream_type {
    FH_PES  = 0x1,
    FH_RTSP = 0x2,
    FH_HTTP = 0x4,
    FH_RAW  = 0x8,
};

struct stream_info {
    FH_UINT32 grpid;
    enum stream_type type;
};

static struct stream_info g_stream_info[MAX_GRP_NUM];

static FH_VOID _get_stream_info_type()
{
    memset(g_stream_info, 0, sizeof(g_stream_info));

    g_stream_info[0].grpid = 0;
#ifdef FH_APP_USING_PES_G0
    g_stream_info[0].type |= FH_PES;
#endif
#ifdef FH_APP_USING_RTSP_G0
    g_stream_info[0].type |= FH_RTSP;
#endif
#ifdef FH_APP_RECORD_RAW_STREAM_G0
    g_stream_info[0].type |= FH_RAW;
#endif

    g_stream_info[1].grpid = 1;
#ifdef FH_APP_USING_PES_G1
    g_stream_info[1].type |= FH_PES;
#endif
#ifdef FH_APP_USING_RTSP_G1
    g_stream_info[1].type |= FH_RTSP;
#endif
#ifdef FH_APP_RECORD_RAW_STREAM_G1
    g_stream_info[1].type |= FH_RAW;
#endif

    g_stream_info[2].grpid = 2;
#ifdef FH_APP_USING_PES_G2
    g_stream_info[2].type |= FH_PES;
#endif
#ifdef FH_APP_USING_RTSP_G2
    g_stream_info[2].type |= FH_RTSP;
#endif
#ifdef FH_APP_RECORD_RAW_STREAM_G2
    g_stream_info[2].type |= FH_RAW;
#endif
}

/******************************************************************************
* function : Get frame from YUV422sp
******************************************************************************/
static FH_VOID sample_y_uv_GetVFrameFromYUVsp(
    FILE *pYFile,
    FILE *pUVFile,
    FH_UINT32 u32Width,
    FH_UINT32 u32Height,
    MEM_DESC *pYMem,
    MEM_DESC *pUVMem)
{
    FH_UINT32 u32SizeY,u32SizeUV;
    FH_UINT32 readLenY,readLenUV;

    u32SizeY =  u32Width * u32Height;
    u32SizeUV =  u32Width * u32Height;

    readLenY = fread((FH_UINT8 *)pYMem->vbase, 1, u32SizeY, pYFile);
    readLenUV = fread((FH_UINT8 *)pUVMem->vbase, 1, u32SizeUV, pUVFile);

    if(!readLenY || !readLenUV)
    { //repeat
        fseek(pYFile, 0, SEEK_SET);
        fseek(pUVFile, 0, SEEK_SET);
        readLenY = fread((FH_UINT8 *)pYMem->vbase, 1, u32SizeY, pYFile);
        readLenUV = fread((FH_UINT8 *)pUVMem->vbase, 1, u32SizeUV, pUVFile);
    }
}

static FH_VOID sample_yuv_GetVFrameFromYUVsp(
    FILE *pYUVFile,
    FH_UINT32 u32Width,
    FH_UINT32 u32Height,
    MEM_DESC *pYUVMem)
{
    FH_UINT32 u32SizeYUV;
    FH_UINT32 readLenYUV;

    u32SizeYUV =  u32Width * u32Height * 2;
    readLenYUV = fread((FH_UINT8 *)pYUVMem->vbase, 1, u32SizeYUV, pYUVFile);

    if(!readLenYUV)
    { //repeat
        fseek(pYUVFile, 0, SEEK_SET);
        readLenYUV = fread((FH_UINT8 *)pYUVMem->vbase, 1, u32SizeYUV, pYUVFile);
    }
}


FH_SINT32 sample_dmc_init(FH_CHAR *dst_ip, FH_UINT32 port)
{
    FH_SINT32 index;
    FH_SINT32 pesgrp;
    FH_SINT32 rtspgrp;
    FH_SINT32 rawgrp;

    _get_stream_info_type();

    dmc_init();

    index   = 0;
    pesgrp  = 0;
    rtspgrp = 0;
    rawgrp  = 0;
    while(index < MAX_GRP_NUM){
        if(g_stream_info[index].type & FH_PES)
            pesgrp |= 1 << g_stream_info[index].grpid;
        if(g_stream_info[index].type & FH_RTSP)
            rtspgrp |= 1 << g_stream_info[index].grpid;
        if(g_stream_info[index].type & FH_RAW)
            rawgrp |= 1 << g_stream_info[index].grpid;

        index++;
    }

    if (dst_ip != NULL && port != 0 && pesgrp)
        dmc_pes_subscribe(pesgrp, dst_ip, port);

    if (port != 0 && rtspgrp)
        dmc_rtsp_subscribe(rtspgrp, port);

    if (rawgrp)
        dmc_record_subscribe(rawgrp);

    dmc_http_mjpeg_subscribe(HTTP_MJPEG_PORT);

    return 0;
}

FH_SINT32 sample_dmc_deinit(FH_VOID)
{
    dmc_rtsp_unsubscribe();

    dmc_pes_unsubscribe();

    dmc_record_unsubscribe();

    dmc_http_mjpeg_unsubscribe();

    dmc_deinit();

    return 0;
}

FH_VOID *sample_common_get_stream_proc(FH_VOID *arg)
{
    FH_SINT32 ret, i;
    FH_SINT32 end_flag;
    FH_SINT32 subtype;
    FH_VENC_STREAM stream;
    FH_SINT32 *stop = (FH_SINT32 *)arg;

    prctl(PR_SET_NAME, "demo_get_stream");
    while (*stop == 0)
    {
        WR_PROC_DEV(TRACE_PROC, "timing_GetStream_START");

    	/*阻塞模式下,获取一帧H264或者H265数据*/
        ret = FH_VENC_GetStream_Block(FH_STREAM_ALL & (~(FH_STREAM_JPEG)), &stream);
        WR_PROC_DEV(TRACE_PROC, "timing_EncBlkFinish_xxx");

        if (ret != 0)
        {
            printf("Error(%d - %x): FH_VENC_GetStream_Block(FH_STREAM_ALL & (~(FH_STREAM_JPEG))) failed!\n", ret, ret);
            continue;
        }

		/*获取到一帧H264数据,按照下面的方式处理*/
        if (stream.stmtype == FH_STREAM_H264)
        {
            subtype = stream.h264_stream.frame_type == FH_FRAME_I ? DMC_MEDIA_SUBTYPE_IFRAME : DMC_MEDIA_SUBTYPE_PFRAME;
            for (i = 0; i < stream.h264_stream.nalu_cnt; i++)
            {
            	end_flag = (i == (stream.h264_stream.nalu_cnt - 1)) ? 1 : 0;
                dmc_input(stream.chan,
			    		  DMC_MEDIA_TYPE_H264,
			    		  subtype,
			    		  stream.h264_stream.time_stamp,
		    		      stream.h264_stream.nalu[i].start,
		    		      stream.h264_stream.nalu[i].length,
		    		      end_flag);
            }
        }

		/*获取到一帧H265数据,按照下面的方式处理*/
        else if (stream.stmtype == FH_STREAM_H265)
        {
            subtype = stream.h265_stream.frame_type == FH_FRAME_I ? DMC_MEDIA_SUBTYPE_IFRAME : DMC_MEDIA_SUBTYPE_PFRAME;
            for (i = 0; i < stream.h265_stream.nalu_cnt; i++)
			{
            	end_flag = (i == (stream.h265_stream.nalu_cnt - 1)) ? 1 : 0;
                dmc_input(stream.chan,
			    		  DMC_MEDIA_TYPE_H265,
			    		  subtype,
			    		  stream.h265_stream.time_stamp,
		    		      stream.h265_stream.nalu[i].start,
		    		      stream.h265_stream.nalu[i].length,
		    		      end_flag);
			}
        }

        /*获取到一帧MJPEG数据,按照下面的方式处理*/
        else if (stream.stmtype == FH_STREAM_MJPEG)
        {
            dmc_input(stream.chan,
                      DMC_MEDIA_TYPE_MJPEG,
                      0,
                      0,
                      stream.mjpeg_stream.start,
                      stream.mjpeg_stream.length,
                      1);
        }

		/*必须和FH_VENC_GetStream配套调用,以释放码流资源*/
        ret = FH_VENC_ReleaseStream(&stream);
        if(ret)
        {
            printf("Error(%d - %x): FH_VENC_ReleaseStream failed for chan(%d)!\n", ret, ret, stream.chan);
        }
        WR_PROC_DEV(TRACE_PROC, "timing_GetStream_END");
    }

    *stop = 0;
    return NULL;
}

FH_VOID *sample_common_send_stream_proc(FH_VOID *arg)
{
	FH_UINT32 u32Size;
    FH_UINT32 u32Width;
    FH_UINT32 u32Height;
    FH_SINT32 ret =0;
    FH_SINT32 loop = 0;
    FILE * yfd=NULL;
    FILE * uvfd=NULL;
    FILE * yuvfd=NULL;
    FH_UINT32 u32BufIdx=0;
	MEM_DESC YMem[YUV_BUF_NUM];
    MEM_DESC UVMem[YUV_BUF_NUM];
    MEM_DESC YUVMem[YUV_BUF_NUM];
    char Mem_name[20];
	FH_VPU_USER_PIC user_pic;
    struct vpu_send_param  * sendParam = NULL;

    sendParam = (struct vpu_send_param  *)arg;

    if(sendParam->SendFrameInfo.yfile && sendParam->SendFrameInfo.uvfile){
        printf("open file:%s \n",(char *)sendParam->SendFrameInfo.yfile);
        printf("open file:%s \n",(char *)sendParam->SendFrameInfo.uvfile);
        yfd = fopen(sendParam->SendFrameInfo.yfile, "rb");
        if (!yfd)
        {
            printf("open file -> %s fail \n", (char *)sendParam->SendFrameInfo.yfile);
            goto Exit;
        }
        uvfd = fopen(sendParam->SendFrameInfo.uvfile, "rb");
        if (!uvfd)
        {
            printf("open file -> %s fail \n", (char *)sendParam->SendFrameInfo.uvfile);
            goto Exit;
        }
    }
    else if(sendParam->SendFrameInfo.yuvfile){
        printf("open file:%s \n",(char *)sendParam->SendFrameInfo.yuvfile);
        yuvfd = fopen(sendParam->SendFrameInfo.yuvfile, "rb");
        if (!yuvfd)
        {
            printf("open file -> %s fail \n", (char *)sendParam->SendFrameInfo.yuvfile);
            goto Exit;
        }
    }
    else {
        printf("file:%s doesn't exit\n",(char *)sendParam->SendFrameInfo.yfile);
        printf("file:%s doesn't exit\n",(char *)sendParam->SendFrameInfo.uvfile);
        printf("file:%s doesn't exit\n",(char *)sendParam->SendFrameInfo.yuvfile);
        goto Exit;
    }

	// printf("%s %d: Run Here! \n",__FUNCTION__,__LINE__);

	for(u32BufIdx = 0; u32BufIdx < YUV_BUF_NUM; u32BufIdx++)
	{
        u32Width = sendParam->SendFrameInfo.u32Width;//sendParam->u32Width;
        u32Height = sendParam->SendFrameInfo.u32Height;
        u32Size =  u32Width*u32Height;

        if(yuvfd) {
            memset(Mem_name, 0, sizeof(Mem_name));
            sprintf(Mem_name, "UsrpicYUV mem%d", u32BufIdx);
            ret = buffer_malloc_withname(&YUVMem[u32BufIdx], u32Size * 2, 16, Mem_name);
            if(ret)
            {
                printf("[Error]: YUV buffer_malloc_withname failed with ret:%d \n", ret);
                goto Exit;
            }
        }
        else {
            memset(Mem_name, 0, sizeof(Mem_name));
            sprintf(Mem_name, "UsrpicY mem%d", u32BufIdx);
            ret = buffer_malloc_withname(&YMem[u32BufIdx], u32Size, 16, Mem_name);
            if(ret)
            {
                printf("[Error]: Y buffer_malloc_withname failed with ret:%d \n", ret);
                goto Exit;
            }

            memset(Mem_name, 0, sizeof(Mem_name));
            sprintf(Mem_name, "UsrpicUV mem%d", u32BufIdx);
            ret = buffer_malloc_withname(&UVMem[u32BufIdx], u32Size, 16, Mem_name);
            if(ret)
            {
                printf("[Error]: UV buffer_malloc_withname failed with ret:%d \n", ret);
                goto Exit;
            }
        }
        // printf("%s %d: Run Here!u32Width:%d,u32Height:%d,size:0x%x,name:(%s,%s)\n",__FUNCTION__,__LINE__,u32Width,u32Height,u32Size,Mem_name[u32BufIdx],Mem_name[u32BufIdx + 1]);
    }

	u32BufIdx = 0;

    prctl(PR_SET_NAME, "demo_vpu_send");
    while(!sendParam->bStop)
    {
        u32Width = sendParam->SendFrameInfo.u32Width;//sendParam->u32Width;
        u32Height = sendParam->SendFrameInfo.u32Height;

        if(yuvfd) {
            sample_yuv_GetVFrameFromYUVsp(yuvfd, u32Width, u32Height, &YUVMem[u32BufIdx]);
            user_pic.yluma  = YUVMem[u32BufIdx].base;
            user_pic.chroma = YUVMem[u32BufIdx].base + u32Width * u32Height;
        }
        else {
            sample_y_uv_GetVFrameFromYUVsp(yfd, uvfd, u32Width, u32Height, &YMem[u32BufIdx], &UVMem[u32BufIdx]);
            user_pic.yluma  = YMem[u32BufIdx].base;
            user_pic.chroma = UVMem[u32BufIdx].base;
        }

        user_pic.pic_size.u32Width = u32Width;
        user_pic.pic_size.u32Height= u32Height;
        user_pic.ystride = u32Width;
        user_pic.cstride = u32Width;
        user_pic.data_format = sendParam->data_format;
        user_pic.work_mode = VPU_MODE_OFFLINE_VPU;
        user_pic.time_stamp = sendParam->time_stamp * loop;
        FH_VPSS_SendUserPic(sendParam->u32Grp, &user_pic);

        u32BufIdx ++;
        u32BufIdx = (u32BufIdx == YUV_BUF_NUM) ? 0 : (u32BufIdx);
        loop++;
    }

    if(yfd)
        fclose(yfd);
    if(uvfd)
        fclose(uvfd);
    if(yuvfd)
        fclose(yuvfd);

Exit:
    sendParam->bStart = 0;
    sendParam->bStop = 0;

    return NULL;
}

