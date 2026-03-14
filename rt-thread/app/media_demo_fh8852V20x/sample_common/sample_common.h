#ifndef __common_h__
#define __common_h__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "pthread.h"
#include "time.h"
#include <signal.h>
#include <fcntl.h>
#include <stddef.h>
#include <config.h>
#include <sys/prctl.h>
#include "types/type_def.h"
#include "types/bufCtrl.h"

#include <libdbi_over_tcp/include/dbi_over_tcp.h>
#include <librtsp/include/librtsp.h>
#include <libdmc/include/libdmc.h>
#include <libdmc/include/libdmc_pes.h>
#include <libdmc/include/libdmc_rtsp.h>
#include <libdmc/include/libdmc_record_raw.h>
#include <libdmc/include/libdmc_http_mjpeg.h>

#include <libpes/include/libpes.h>
#include <librtsp/include/librtsp.h>
#include <libscaler/include/fh_scaler.h>
#include <librect_merge_by_gaus/include/rect_merge_by_gaus.h>
#include <libmisc/include/libmisc.h>

#include <isp/include/sample_common_isp.h>
#include <dsp/include/sample_common_dsp.h>
#include <overlay/sample_overlay.h>
#include <motion_and_cover/sample_md_cd.h>
#include <bgm/include/sample_bgm.h>
#include <venc/sample_venc.h>
#include <fh_nn_detect/sample_nna_detect.h>
#include <isp_strategy/sample_isp.h>
#include <ivs_demo/sample_ivs.h>
#include <abandon_detect/sample_abandon_detect.h>
#include <bind/include/sample_bind.h>
#include "FHAdv_OSD_mpi.h"

#define ALIGN_UP(addr, edge)   ((addr + edge - 1) & ~(edge - 1)) /* 数据结构对齐定义 */
#define ALIGN_BACK(addr, edge) ((edge) * (((addr) / (edge))))
#define ALIGNTO(addr, edge) ((addr + edge - 1) & ~(edge - 1))
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define CLIP(x, min, max) MAX((min), MIN((x), (max)))

#define ISP_PROC "/proc/driver/isp"
#define VPU_PROC "/proc/driver/vpu"
#define BGM_PROC "/proc/driver/bgm"
#define ENC_PROC "/proc/driver/enc"
#define JPEG_PROC "/proc/driver/jpeg"
#define TRACE_PROC "/proc/driver/trace"

extern void write_media_info(char *mod, char *config);
#define WR_PROC_DEV(device, cmd) write_media_info(device, cmd)

#endif /*__common_h__*/

