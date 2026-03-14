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
#include "rtconfig_app.h"
#include <sys/prctl.h>
#include "types/type_def.h"
#include "types/bufCtrl.h"

#include <sample_common_isp.h>
#include <sample_common_dsp.h>
#include <bind/include/sample_bind.h>

#define MAX_VPU_CHN_NUM 4
#define MAX_GRP_NUM 3

#ifdef FH_APP_OPEN_OVERLAY
#include "FHAdv_OSD_mpi.h"
#endif
#define ALIGN_UP(addr, edge)   ((addr + edge - 1) & ~(edge - 1)) /* 数据结构对齐定义 */
#define ALIGN_BACK(addr, edge) ((edge) * (((addr) / (edge))))

#define VPU_GROUP_ID 0

#endif /*__common_h__*/

