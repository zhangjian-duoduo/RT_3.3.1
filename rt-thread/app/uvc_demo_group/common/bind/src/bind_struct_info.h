/*************************************************************************
    > File Name: bind_struct_info.h
    > Author: dongky341
    > Mail: dongky341@fullhan.com
    > Created Time: 2021年09月09日 星期四 16时44分57秒
 ************************************************************************/
#ifndef __BIND_STRUCT_INFO_H__
#define __BIND_STRUCT_INFO_H__

struct bind_dev_info g_bind_dev_info[MAX_GRP_NUM * MAX_VPU_CHN_NUM] = {
    {
        .vpu2  = CH0_BIND_TYPE,
        .enc2  = CH0_ENC_BIND_TYPE,
        .vgs2  = CH0_VGS_BIND_TYPE,
    },

    {
        .vpu2  = CH1_BIND_TYPE,
        .enc2  = CH1_ENC_BIND_TYPE,
        .vgs2  = CH1_VGS_BIND_TYPE,
    },

    {
        .vpu2  = CH2_BIND_TYPE,
        .enc2  = CH2_ENC_BIND_TYPE,
        .vgs2  = CH2_VGS_BIND_TYPE,
    },

    {
        .vpu2  = CH3_BIND_TYPE,
        .enc2  = CH3_ENC_BIND_TYPE,
        .vgs2  = CH3_VGS_BIND_TYPE,
    },
/*********************G0************************/

    {
        .vpu2  = CH0_BIND_TYPE,
        .enc2  = CH0_ENC_BIND_TYPE,
        .vgs2  = CH0_VGS_BIND_TYPE,
    },

    {
        .vpu2  = CH1_BIND_TYPE,
        .enc2  = CH1_ENC_BIND_TYPE,
        .vgs2  = CH1_VGS_BIND_TYPE,
    },

    {
        .vpu2  = CH2_BIND_TYPE,
        .enc2  = CH2_ENC_BIND_TYPE,
        .vgs2  = CH2_VGS_BIND_TYPE,
    },

    {
        .vpu2  = CH3_BIND_TYPE,
        .enc2  = CH3_ENC_BIND_TYPE,
        .vgs2  = CH3_VGS_BIND_TYPE,
    },

/*********************G1************************/

    {
        .vpu2  = CH0_BIND_TYPE,
        .enc2  = CH0_ENC_BIND_TYPE,
        .vgs2  = CH0_VGS_BIND_TYPE,
    },

    {
        .vpu2  = CH1_BIND_TYPE,
        .enc2  = CH1_ENC_BIND_TYPE,
        .vgs2  = CH1_VGS_BIND_TYPE,
    },

    {
        .vpu2  = CH2_BIND_TYPE,
        .enc2  = CH2_ENC_BIND_TYPE,
        .vgs2  = CH2_VGS_BIND_TYPE,
    },

    {
        .vpu2  = CH3_BIND_TYPE,
        .enc2  = CH3_ENC_BIND_TYPE,
        .vgs2  = CH3_VGS_BIND_TYPE,
    },
/*********************G2************************/




/*********************G3************************/

};


#endif /* __BIND_STRUCT_INFO_H__ */
