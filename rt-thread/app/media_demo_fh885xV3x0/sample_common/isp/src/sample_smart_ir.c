#include "isp/isp_common.h"
#include "isp/isp_api.h"
#include "FHAdv_SmartIR_mpi.h"
#include "sample_common.h"
#include "_sensor_.h"
#include "sample_smart_ir.h"

static FH_CHAR *sensor_param_day[MAX_GRP_NUM];
static FH_CHAR *sensor_param_night[MAX_GRP_NUM];
static FH_SINT32 g_smartir_inited[MAX_GRP_NUM] = {0};

FH_SINT32 sample_SmartIR_init(FH_CHAR *sensor_name, FH_SINT32 grpidx)
{
    FH_SINT32 ret = -1;
    FH_SINT32 hex_file_len;
    FHADV_SMARTIR_CFG_t smtir_cfg;

    if (g_smartir_inited[grpidx])
    {
        printf("SmartIR: already inited!\n");
    	return 0;
    }

    sensor_param_day[grpidx]   = get_isp_sensor_param(grpidx, (char *)sensor_name, SAMPLE_SENSOR_FLAG_NORMAL, &hex_file_len);
    if (!sensor_param_day[grpidx])
    {
        printf("Error: Cann't load sensor hex Day file!\n");
        goto Exit;
    }

    sensor_param_night[grpidx] = get_isp_sensor_param(grpidx, (char *)sensor_name, SAMPLE_SENSOR_FLAG_NIGHT, &hex_file_len);
    if (!sensor_param_night[grpidx])
    {
        printf("Error: Cann't load sensor hex Night file!\n");
        goto Exit;
    }

    ret = FHAdv_SmartIR_Init(grpidx);
    if ( ret != 0 )
    {
        printf("Error: FHAdv_SmartIR_Init failed with %d\n", ret);
        goto Exit;
    }

    ret = FHAdv_SmartIR_GetCfg(grpidx, &smtir_cfg);
    if ( ret != 0 )
    {
        printf("Error: FHAdv_SmartIR_GetCfg failed with %d\n", ret);
        goto Exit;
    }

    smtir_cfg.d2n_thre  = 0x1000;
    smtir_cfg.n2d_value = 0x500;
    smtir_cfg.n2d_gain  = 0x140;
    ret = FHAdv_SmartIR_SetCfg(grpidx, &smtir_cfg);
    if ( ret != 0 )
    {
        printf("Error: FHAdv_SmartIR_SetCfg failed with %d\n", ret);
        goto Exit;
    }

    FHAdv_SmartIR_SetDebugLevel(grpidx, SMARTIR_DEBUG_OFF);

    g_smartir_inited[grpidx] = 1;

    return 0;

Exit:
	if (sensor_param_day[grpidx])
    {
    	free_isp_sensor_param(sensor_param_day[grpidx]);
    	sensor_param_day[grpidx] = NULL;
    }

 	if (sensor_param_night[grpidx])
    {
    	free_isp_sensor_param(sensor_param_night[grpidx]);
    	sensor_param_night[grpidx] = NULL;
    }

    return ret;
}

FH_SINT32 sample_SmartIR_deinit(FH_SINT32 grpidx)
{
    FH_SINT32 ret = -1;

 	if (sensor_param_day[grpidx])
    {
    	free_isp_sensor_param(sensor_param_day[grpidx]);
    	sensor_param_day[grpidx] = NULL;
    }

 	if (sensor_param_night[grpidx])
    {
    	free_isp_sensor_param(sensor_param_night[grpidx]);
    	sensor_param_night[grpidx] = NULL;
    }

    ret = FHAdv_SmartIR_UnInit(grpidx);
    if ( ret != 0 )
    {
        printf("Error: FHAdv_SmartIR_UnInit failed with %d\n", ret);
        return -1;
    }

	g_smartir_inited[grpidx] = 0;

	return 0;
}

FH_VOID sample_SmartIR_Ctrl(FH_SINT32 grpidx)
{
    static FHADV_SMARTIR_STATUS_e sirStaPrev[MAX_GRP_NUM] = {SMARTIR_STATUS_DAY, SMARTIR_STATUS_DAY, SMARTIR_STATUS_DAY}; /*the initial value must be FHADV_IR_DAY*/
    FH_SINT32 irStaCurr;
    FH_SINT32 ret;

    if (!g_smartir_inited[grpidx])
    	return;

    irStaCurr = FHAdv_SmartIR_GetDayNightStatus(grpidx, sirStaPrev[grpidx]);
    if (irStaCurr == sirStaPrev[grpidx])
        return;

    if (irStaCurr == SMARTIR_STATUS_DAY)
    {
        printf("[SmartIR]: Switch to Day\n");
        ret = API_ISP_LoadIspParam(grpidx, sensor_param_day[grpidx]);
        if (ret)
        {
            printf("Error(%d): API_ISP_LoadIspParam!\n", ret);
        }
    }
    else if(irStaCurr == SMARTIR_STATUS_NIGHT) /*night*/
    {
        printf("[SmartIR]: switch to Night\n");
        ret = API_ISP_LoadIspParam(grpidx, sensor_param_night[grpidx]);
        if (ret)
        {
            printf("Error(%d): API_ISP_LoadIspParam!\n", ret);
        }
    }
    else
        printf("[SmartIR]: FHAdv_SmartIR_GetDayNightStatus failed with:%x\n", irStaCurr);

    sirStaPrev[grpidx] = irStaCurr;
}
