#ifndef __FH_ALGO_H__
#define __FH_ALGO_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

typedef struct
{
	float x, y, w, h;
} resize_norbox_t;

typedef struct
{
    int x, y, w, h;
} resize_unnorbox_t;

typedef struct
{
    unsigned char r, g, b, a;
} resize_rgb_t;

void resizeBilinear_planar(resize_rgb_t *src_data, int src_w, int src_h, resize_unnorbox_t *crop_box, resize_rgb_t *dst_data, int dst_w, int dst_h, int square_pad);

void resizeBilinear(resize_rgb_t* src_data, int src_w, int src_h, resize_unnorbox_t *crop_box, resize_rgb_t* dst_data, int dst_w, int dst_h, int square_pad);

void unnormalize(resize_norbox_t *src_box, resize_unnorbox_t *dst_box, int src_w, int src_h);

void create_square_box(resize_unnorbox_t *src_box, resize_unnorbox_t *dst_box, int src_w, int src_h);

void create_square_box_enlarge(resize_unnorbox_t *src_box, resize_unnorbox_t *dst_box, int src_w, int src_h, int enlarge);

void FH_ALGO_PrintVersion(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __FH_ALGO_H__ */