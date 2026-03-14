#ifndef __SAMPLE_SMART_IR_H_
#define __SAMPLE_SMART_IR_H_

extern FH_SINT32 sample_SmartIR_init(FH_CHAR *sensor_name, FH_SINT32 grpidx);
extern FH_SINT32 sample_SmartIR_deinit(FH_SINT32 grpidx);
extern FH_VOID sample_SmartIR_Ctrl(FH_SINT32 grpidx);

#endif /*__SAMPLE_SMART_IR_H_*/

