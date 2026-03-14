#ifndef __SAMPLE_BGM_H__
#define __SAMPLE_BGM_H__

typedef struct _bgm_rect
{
    unsigned int x;
    unsigned int y;
    unsigned int w;
    unsigned int h;
}BGM_Rect;

FH_SINT32 sample_common_bgm_init();
FH_SINT32 sample_common_bgm_exit();
FH_SINT32 sample_common_bgm_get_init_status(FH_UINT32 grpid);
FH_SINT32 sample_common_bgm_triger_reload(FH_UINT32 grpid);
FH_SINT32 sample_common_bgm_get_outer_motion_area(
        FH_UINT32 grpid,
        FH_UINT32 *winX,
        FH_UINT32 *winY,
        FH_UINT32 *winW,
        FH_UINT32 *winH,
        FH_UINT32 *picW,
        FH_UINT32 *picH,
        FH_UINT8   thresh);

FH_SINT32 sample_common_bgm_get_multi_motion_area(
        FH_UINT32 grpid,
        BGM_Rect *motion_rects,
        FH_UINT32 *rect_num,
        FH_UINT32 max_rect_num,
        FH_UINT8 thresh);

FH_SINT32 sample_common_bgm_get_statistics_sum(
        FH_UINT32 grpid,
        FH_UINT32 detect_frame_w,
        FH_UINT32 detect_frame_h,
        FH_UINT32 x,
        FH_UINT32 y,
        FH_UINT32 w,
        FH_UINT32 h,
        FH_UINT32 *sum,
        FH_UINT32 *blk_num);

#endif /*__SAMPLE_BGM_H__*/
