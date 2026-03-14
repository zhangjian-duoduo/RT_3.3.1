#ifndef __SAMPLE_NNA_DETECT_H__
#define __SAMPLE_NNA_DETECT_H__

struct nn_channel_info {
    FH_UINT32 enable;
    FH_UINT32 channel;
    FH_UINT32 type;
};

extern FH_SINT32 sample_fh_nn_obj_detect_stop(FH_VOID);
extern FH_SINT32 sample_fh_nn_obj_detect_start(FH_VOID);

#endif // __SAMPLE_NNA_DETECT_H__
