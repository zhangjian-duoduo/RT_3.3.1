#ifndef _POST_PROCESS_H_
#define _POST_PROCESS_H_

#include <float.h>
#include <stdint.h>
#include <stdbool.h>

#define PP_MAX_BBOX_NUM 150

#define FH_FAILED -1
#define FH_SUCCESS 0

void LogInfo(const char* str, ...);
void LogError(const char* str, ...);
#define FH_CHECK_PTR(ptr, label)      do {\
    if( NULL == (ptr) ) {\
        LogError("Error: %s: %s at %d\n", __FILE__, __FUNCTION__, __LINE__);\
        goto label;\
    }\
} while(0)

#define FH_CHECK_STATUS(status, label)  do {\
    if( status ) {\
        LogError("Error: %s: %s line %d, errcode[%d]\n", __FILE__, __FUNCTION__, __LINE__, status);\
        goto label;\
    }\
} while(0)

#define FH_RETURN_STATUS(status)  do {\
    if( status ) {\
        LogError("Error: %s: %s line %d, errcode[%d]\n", __FILE__, __FUNCTION__, __LINE__, status);\
    }\
} while(0)

typedef enum {
    PP_NET_PERSONDET = 1,
    PP_NET_FACEDET,
    PP_NET_C2DET
}pp_net_type_e;

typedef enum {
    PP_KERNEL_ROT0 = 0,
    PP_KERNEL_ROT90,
    PP_KERNEL_ROT180,
    PP_KERNEL_ROT270
}kernel_rot_type_e;

typedef struct _box {
    float x, y, w, h;
} box_t;

typedef struct _bbox {
    uint16_t  class_id; //class index
    box_t     bbox;     //bounding box[0, 1] for network inputs
    float     conf;     //confidence
} bbox_t;

typedef struct _det_bbox {
    uint32_t bbox_num;  //number
    bbox_t   bbox[PP_MAX_BBOX_NUM];
} det_bbox_t;

typedef struct _post_processor_param {
    pp_net_type_e     net_type;
    uint16_t          net_width;
    uint16_t          net_height;
    float             conf_thr;
    kernel_rot_type_e kernel_rot;
}post_processor_param_t;

typedef struct _post_processor* post_processor;
typedef struct _post_processor_input {
    //FM out base address
    void *baseaddr;
}post_processor_input_t;

//api
post_processor POST_PROCESSOR_Create(post_processor_param_t *param);
int POST_PROCESSOR_Process(post_processor processor, post_processor_input_t *in, det_bbox_t *dets);
int POST_PROCESSOR_SetConf(post_processor processor, float thresh);
int POST_PROCESSOR_Destroy(post_processor processor);

#endif



