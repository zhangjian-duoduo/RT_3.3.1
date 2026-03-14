#include "sample_common.h"
#include "media_config_proc.h"
#ifdef __LINUX_OS__

static int write_proc(char *path, char *s)
{
    int fd;

    fd = open(path, O_RDWR);
    if (fd < 0)
    {
        printf("Error: open %s!\n", path);
        return -1;
    }

    write(fd, s, strlen(s));
    close(fd);
    return 0;
}

int vpu_write_proc(char *s)
{
    return write_proc("/proc/driver/vpu", s);
}

int enc_write_proc(char *s)
{
    return write_proc("/proc/driver/enc", s);
}

int jpeg_write_proc(char *s)
{
    return write_proc("/proc/driver/jpeg", s);
}

int bgm_write_proc(char *s)
{
    return write_proc("/proc/driver/bgm", s);
}

int isp_write_proc(char *s)
{
    return write_proc("/proc/driver/isp", s);
}

int mipi_write_proc(char *s)
{
    return write_proc("/proc/driver/mipi", s);
}

int media_trace_write_proc(char *s)
{
    return write_proc("/proc/driver/trace", s);
}
#else
extern int vpu_write_proc(char *s);
extern int enc_write_proc(char *s);
extern int jpeg_write_proc(char *s);
extern int bgm_write_proc(char *s);
extern int isp_write_proc(char *s);
extern int mipi_write_proc(char *s);
extern int media_trace_write_proc(char *);
#endif

void write_media_info(char *mod, char *config)
{
    if (strcmp(mod, "/proc/driver/vpu") == 0)
    {
        vpu_write_proc(config);
    }
    if (strcmp(mod, "/proc/driver/enc") == 0)
    {
        enc_write_proc(config);
    }
    if (strcmp(mod, "/proc/driver/jpeg") == 0)
    {
        jpeg_write_proc(config);
    }
    if (strcmp(mod, "/proc/driver/bgm") == 0)
    {
        bgm_write_proc(config);
    }
    if (strcmp(mod, "/proc/driver/isp") == 0)
    {
        isp_write_proc(config);
    }
    if (strcmp(mod, "/proc/driver/mipi") == 0)
    {
        mipi_write_proc(config);
    }
    if (strcmp(mod, "/proc/driver/trace") == 0)
    {
        media_trace_write_proc(config);
    }
}
