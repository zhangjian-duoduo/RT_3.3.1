

extern void uvc_stream_on(int stream_id);
extern void uvc_stream_off(int stream_id);
extern FH_VOID Uvc_SetAEMode(FH_UINT32 mode);
extern FH_VOID Uvc_SetExposure(FH_UINT32 time_value);
extern FH_VOID Uvc_SetGain(FH_UINT32 gain_level);
extern FH_VOID Uvc_SetBrightness(FH_UINT32 value);
extern FH_VOID Uvc_SetContrast(FH_UINT32 value);
extern FH_VOID Uvc_SetSaturation(FH_SINT32 value);
extern FH_VOID Uvc_SetSharpeness(FH_SINT32 value);
extern FH_VOID Uvc_SetAWBGain(FH_UINT32 value);
extern FH_VOID Uvc_SetAwbMode(FH_UINT32 value);
extern FH_VOID Uvc_SetGammaCfg(FH_UINT32 value);
extern FH_SINT32 *uvc_para_read(FH_VOID);
