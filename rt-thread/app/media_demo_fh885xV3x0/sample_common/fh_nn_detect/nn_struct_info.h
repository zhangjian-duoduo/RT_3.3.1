/*************************************************************************
    > File Name: nn_struct_info.h
    > Author: dongky341
    > Mail: dongky341@fullhan.com
    > Created Time: 2021年09月09日 星期四 16时44分57秒
 ************************************************************************/
#ifndef __NN_STRUCT_INFO_H__
#define __NN_STRUCT_INFO_H__

struct nn_channel_info g_nn_channel_info[MAX_GRP_NUM] = {
    {
        .enable = 0,
#if defined(NN_ENABLE_G0)
        .enable = 1,
        .channel = 0,
        .type = FH_OBJ_VPU_VO,
#endif
#endif
    },

    {
        .enable = 0,
#if defined(NN_ENABLE_G1)
        .enable = 1,
        .channel = 1,
        .type = FH_OBJ_VPU_VO,
#endif
#endif
    },

    {
        .enable = 0,
#if defined(NN_ENABLE_G2)
        .enable = 1,
        .channel = 2,
        .type = FH_OBJ_VPU_VO,
#endif
#endif
    },
};


#endif /* __NN_STRUCT_INFO_H__ */
