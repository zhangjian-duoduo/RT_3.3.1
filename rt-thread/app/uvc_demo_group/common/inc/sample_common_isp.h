
#ifndef __SAMPLE_COMMON_ISP_H__
#define __SAMPLE_COMMON_ISP_H__


#define SAMPLE_SENSOR_FLAG_NORMAL    (0x00)
#define SAMPLE_SENSOR_FLAG_NIGHT     (0x02)
#define SAMPLE_SENSOR_FLAG_WDR       (0x01)
#define SAMPLE_SENSOR_FLAG_WDR_NIGHT (SAMPLE_SENSOR_FLAG_NIGHT | SAMPLE_SENSOR_FLAG_WDR)

typedef enum
{
    ISP_SENSOR_SLEEP          = 0,
    ISP_SENSOR_WAKEUP         = 1,
}FH_ISP_SENSOR_PM;

struct dev_isp_info
{
    FH_SINT32 enable;
    FH_SINT32 channel;
    FH_SINT32 isp_format;
    FH_SINT32 isp_init_width;
    FH_SINT32 isp_init_height;
    FH_SINT32 isp_max_width;
    FH_SINT32 isp_max_height;
    FH_CHAR sensor_name[50];
    struct isp_sensor_if *sensor;
    FH_BOOL bStop;
    FH_BOOL running;
};

extern FH_SINT32 sample_common_get_isp_info(FH_UINT32 grpid, struct dev_isp_info *info);
extern FH_SINT32 sample_isp_use_large_frame(FH_UINT32 grpid);
extern FH_UINT32 sample_isp_get_vi_w(FH_UINT32 grpid);
extern FH_UINT32 sample_isp_get_vi_h(FH_UINT32 grpid);
extern FH_UINT32 sample_isp_get_max_w(FH_UINT32 grpid);
extern FH_UINT32 sample_isp_get_max_h(FH_UINT32 grpid);
extern FH_VOID sample_isp_change_resolution(FH_UINT32 grpid);
extern FH_VOID   sample_isp_change_fps(FH_UINT32 grpid);
extern FH_SINT32 sample_common_start_isp(FH_VOID);
extern FH_SINT32 sample_common_isp_run(FH_VOID);
extern FH_SINT32 sample_common_isp_stop(FH_VOID);

extern FH_SINT32 sample_common_start_coolview(FH_VOID *arg);
extern FH_SINT32 sample_common_stop_coolview(FH_VOID);
FH_VOID ISP_sleep_mode(FH_ISP_SENSOR_PM on);
#endif /*__SAMPLE_COMMON_ISP_H__*/
