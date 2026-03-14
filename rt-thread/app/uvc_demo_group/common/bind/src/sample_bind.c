#include "sample_common.h"
#include "chn_bind_type.h"
#include "bind_struct_info.h"

FH_VOID sample_common_get_bind_info(FH_SINT32 channel, struct bind_dev_info *bind_info)
{
    *bind_info = g_bind_dev_info[channel];
}

FH_SINT32 sample_common_start_bind(FH_VOID)
{
    int ret;
    int grploop, chnloop;

    struct dev_isp_info isp_info;
    struct grp_vpu_info grp_info;
    struct vpu_channel_info vpu_info;
    struct enc_channel_info enc_info;
    struct bind_dev_info *bind_info;

    FH_BIND_INFO src, dst;

    for (grploop = 0; grploop < MAX_GRP_NUM; grploop++)
    {
        if (grploop != FH_GRP_ID)
            continue;
        sample_common_dsp_get_grp_info(grploop, &grp_info);
        sample_common_get_isp_info(grploop, &isp_info);
#if !defined(CONFIG_ARCH_FH8636_FH8852V20X) && !defined(CONFIG_CHIP_FH8626V200)
        if (isp_info.enable)
        {
            src.obj_id = FH_OBJ_ISP;
            src.dev_id = isp_info.channel;
            src.chn_id = 0;
            dst.obj_id = FH_OBJ_VPU_VI;
            dst.dev_id = grp_info.channel;
            dst.chn_id = 0;
            ret = FH_SYS_Bind(src, dst);
            if (ret != RETURN_OK)
            {
                printf("Error(%d - %x): FH_SYS_Bind ISP to VPU(%d to %d)\n", ret, ret, isp_info.channel, grp_info.channel);
                return -1;
            }
        }
#endif
        if (grp_info.bgm_enable)
        {
            src.obj_id = FH_OBJ_VPU_BGM;
            src.dev_id = 0;
            src.chn_id = grp_info.channel;
            dst.obj_id = FH_OBJ_BGM;
            dst.dev_id = 0;
            dst.chn_id = grp_info.channel;
            ret = FH_SYS_Bind(src, dst);
            if (ret != RETURN_OK)
            {
                printf("Error(%d - %x): FH_SYS_Bind VPU to BGM(%d to %d)\n", ret, ret, grp_info.channel, grp_info.channel);
                return -1;
            }
        }

        for (chnloop = 0; chnloop < MAX_VPU_CHN_NUM; chnloop++)
        {
            sample_common_dsp_get_vpu_chn_info(grploop, chnloop, &vpu_info);
            sample_common_dsp_get_enc_chn_info(grploop, chnloop, &enc_info);
            bind_info = &g_bind_dev_info[grploop * MAX_VPU_CHN_NUM + chnloop];

            if (enc_info.enable)
            {
                if (bind_info->vpu2 == FH_OBJ_ENC)
                {
                    ret = sample_common_vpu_bind_to_enc(grp_info.channel, vpu_info.channel, enc_info.channel);
                    if (ret != RETURN_OK)
                    {
                        printf("Error(%d - %x): FH_SYS_Bind VPU to ENC([%d][%d] to %d)\n", ret, ret, grp_info.channel, vpu_info.channel, enc_info.channel);
                        return -1;
                    }
                }
            }
        }
    }
    return 0;
}
