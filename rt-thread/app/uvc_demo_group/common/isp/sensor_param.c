#include "sample_common.h"
#include "_sensor_.h"
#include "isp/isp_common.h"

static char *os_get_isp_sensor_param(int grp_id, char *file_name, int *olen);
static void os_free_isp_sensor_param(char *param);

static void get_hex_file_name(char *sensor_name, int flags, char *file_name)
{
    char chr;
    char *suffix = "";

    if (flags == SAMPLE_SENSOR_FLAG_NORMAL)
        suffix = "_attr.hex";
    else if (flags == SAMPLE_SENSOR_FLAG_WDR)
        suffix = "_wdr_attr.hex";
    else if (flags == SAMPLE_SENSOR_FLAG_WDR_NIGHT)
        suffix = "_wdr_night_attr.hex";
    else if (flags == SAMPLE_SENSOR_FLAG_NIGHT)
        suffix = "_night_attr.hex";

    while (1)
    {
        chr = *(sensor_name++);
        if (!chr)
            break;
        if (chr >= 'A' && chr <= 'Z')
            chr += 32; /*转为小写字母*/
        *(file_name++) = chr;
    }

    do
    {
        chr = *(suffix++);
        *(file_name++) = chr;
    } while (chr);
}

char *get_isp_sensor_param(int grp_id, char *sensor_name, int flags, int *olen)
{
    char file_name[64];

    get_hex_file_name(sensor_name, flags, file_name);
    return os_get_isp_sensor_param(grp_id, file_name, olen);
}

void free_isp_sensor_param(char *param)
{
    os_free_isp_sensor_param(param);
}

#ifdef __LINUX_OS__
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

static void get_program_path(char *prog_path)
{
    char path[256];
    char *dname;
    FILE *proc_file;
    pid_t pid = getpid();

    *prog_path = 0;
    sprintf(path, "/proc/%i/cmdline", pid);
    proc_file = fopen(path, "r");
    if (proc_file)
    {
        fgets(path, 256, proc_file);
        fclose(proc_file);
        dname = dirname(path);
        strcpy(prog_path, dname);
    }
}

static char *os_get_isp_sensor_param(int grp_id, char *file_name, int *olen)
{
    FH_SINT32 ret = 0;
    char prog_path[256];
    int  fd;
    int  size;
    char *buf = NULL;
    ISP_PARAM_CONFIG isp_param_config;
    struct stat _fstat;

    get_program_path(prog_path);
    strcat(prog_path, "/");
    strcat(prog_path, file_name);

    fd = open(prog_path, O_RDONLY);
    if (fd >= 0)
    {
        if (fstat(fd, &_fstat) == 0 && _fstat.st_size > 0 && _fstat.st_size <= 0x10000) /*0x10000应该足够大了,正常就几KB而已*/
        {
            ret = API_ISP_GetBinAddr(grp_id, &isp_param_config);
            if (ret)
                printf("ISP[%d] API_ISP_GetBinAddr failed with %x(%d)!\n", grp_id, ret, ret);
            size = isp_param_config.u32BinSize;
            if (size <= 0)
                {
                size = _fstat.st_size;
                printf("ISP[%d]API_ISP_GetBinAddr Size == 0!\n", grp_id);
                }
            buf = malloc(size);
            if (buf)
            {
                FH_UINT32 readNum = read(fd, buf, size);
                printf("ISP[%d] Param Size: 0x%x, Load %s (0x%x)!\n", grp_id, size, file_name, readNum);
            }
        }

        close(fd);
    }

    return buf;
}

static void os_free_isp_sensor_param(char *param)
{
    if (param)
        free(param);
}
#endif /*__LINUX_OS__*/

#ifdef __RTTHREAD_OS__

#include "all_sensor_array.h"

#define SNS_HEX_ENTRY_MAGIC (0x6ad7bfc5)
struct hex_file_hdr
{
    unsigned int total_len; /*文件总长度*/
    unsigned int magic;
    unsigned int reserved[2];
};

struct hex_file_entry
{
    unsigned int offset;
    unsigned int len;
    unsigned int magic;
    char file_name[36];
};

extern char g_sensor_hex_array[];

static char *os_get_isp_sensor_param(int grp_id, char *file_name, int *olen)
{
    struct hex_file_entry *entry = (struct hex_file_entry *)(g_sensor_hex_array + sizeof(struct hex_file_hdr));

    while (1)
    {
        if (entry->magic != SNS_HEX_ENTRY_MAGIC || !entry->offset)
            break;

        if (!strcmp(entry->file_name, file_name)) /*find it*/
        {
            *olen = entry->len;
            return (g_sensor_hex_array + entry->offset);
        }
        entry++;
    }

    return NULL;
}

static void os_free_isp_sensor_param(char *param)
{
}
#endif /*__RTTHREAD_OS__*/
