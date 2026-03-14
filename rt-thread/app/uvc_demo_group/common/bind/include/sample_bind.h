#ifndef __SAMPLE_BIND_H__
#define __SAMPLE_BIND_H__

struct bind_dev_info
{
    FH_SINT32 vpu2;
    FH_SINT32 enc2;
    FH_SINT32 vgs2;
};

extern FH_VOID sample_common_get_bind_info(FH_SINT32 channel, struct bind_dev_info *bind_info);
extern FH_SINT32 sample_common_start_bind(FH_VOID);

#endif /* __SAMPLE_BIND_H__ */
