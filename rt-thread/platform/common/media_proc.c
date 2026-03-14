#include <rtthread.h>
#include <finsh.h>


#ifdef FH_ENABLE_VIDEO
extern int media_read_proc();
extern int media_mem_proc();
extern int vpu_read_proc();
extern int vpu_write_proc(char *s);
extern int enc_read_proc();
extern int enc_write_proc(char *s);
extern int jpeg_read_proc();
extern int jpeg_write_proc(char *s);
extern int bgm_read_proc(void);
extern int bgm_write_proc(char *s);
extern void cmm_mem_proc(int index);
extern int isp_read_proc(void);
extern int isp_write_proc();
extern int mipi_read_proc(void);
extern int mipi_write_proc(char *s);
extern int nna_read_proc(void);
extern int nna_write_proc(char *s);
extern int media_trace_read_proc(void);
extern int media_trace_write_proc(char *);

#if 0
int cmd_isp_read_proc(int argc, char *argv[])
{
    isp_read_proc();

    return 0;
}

int cmd_isp_write_proc(int argc, char *argv[])
{
    if (argc < 2)
    {
        rt_kprintf("Usage: isp_write_proc proc_string\n");
        return 0;
    }
    isp_write_proc(argv[1]);

    return 0;
}

int cmd_media_read_proc(int argc, char *argv[])
{
    media_read_proc();

    return 0;
}

int cmd_media_mem_proc(int argc, char *argv[])
{
    media_mem_proc();

    return 0;
}

int cmd_vpu_read_proc(int argc, char *argv[])
{
    vpu_read_proc();

    return 0;
}

int cmd_vpu_write_proc(int argc, char *argv[])
{
    if (argc < 2)
    {
        rt_kprintf("Usage: vpu_write_proc proc_string\n");
        return 0;
    }
    vpu_write_proc(argv[1]);

    return 0;
}

int cmd_enc_read_proc(int argc, char *argv[])
{
    enc_read_proc();

    return 0;
}

int cmd_enc_write_proc(int argc, char *argv[])
{
    if (argc < 2)
    {
        rt_kprintf("Usage: enc_write_proc proc_string\n");
        return 0;
    }
    enc_write_proc(argv[1]);

    return 0;
}

int cmd_jpeg_read_proc(int argc, char *argv[])
{
    jpeg_read_proc();

    return 0;
}

int cmd_jpeg_write_proc(int argc, char *argv[])
{
    if (argc < 2)
    {
        rt_kprintf("Usage: jpeg_write_proc proc_string\n");
        return 0;
    }
    jpeg_write_proc(argv[1]);

    return 0;
}

int cmd_bgm_read_proc(int argc, char *argv[])
{
    bgm_read_proc();

    return 0;
}

int cmd_bgm_write_proc(int argc, char *argv[])
{
    if (argc < 2)
    {
        rt_kprintf("Usage: bgm_write_proc proc_string\n");
        return 0;
    }
    bgm_write_proc(argv[1]);

    return 0;
}

int cmd_mipi_read_proc(int argc, char *argv[])
{
    return mipi_read_proc();
}

int cmd_mipi_write_proc(int argc, char *argv[])
{
    if (argc < 2)
    {
        rt_kprintf("Usage: mipi_write_proc proc_string.\n");
        return 0;
    }
    mipi_write_proc(argv[1]);

    return 0;
}
#endif

#ifdef FINSH_USING_MSH
int cmd_cmm_mem_proc(int argc, char *argv[])
{
    int index;
    if (argc < 2)
    {
        rt_kprintf("Usage: cmm_mem_proc index\n");
        return 0;
    }
    index = rt_atoi(argv[1]);
    cmm_mem_proc(index);

    return 0;
}

static void print_usage()
{
    rt_kprintf("Usage:\n");
    rt_kprintf("  media_info [-r] (isp|enc|vpu|nna|jpeg|bgm|media|trace|mem|mipi) read media module status info.\n");
    rt_kprintf("  media_info -w (isp|enc|vpu|jpeg|bgm|trace|mipi) set_string  write media module config string.\n");
    rt_kprintf("  media_info [-h]                                       show this help\n");
}

static int read_media_info(char *mod)
{
    if (strcmp(mod, "isp") == 0)
    {
        isp_read_proc();
        return 0;
    }
    if (strcmp(mod, "vpu") == 0)
    {
        vpu_read_proc();
        return 0;
    }
    if (strcmp(mod, "enc") == 0)
    {
        enc_read_proc();
        return 0;
    }
    if (strcmp(mod, "jpeg") == 0)
    {
        jpeg_read_proc();
        return 0;
    }
    if (strcmp(mod, "bgm") == 0)
    {
        bgm_read_proc();
        return 0;
    }
    if (strcmp(mod, "media") == 0)
    {
        media_read_proc();
        return 0;
    }
    if (strcmp(mod, "trace") == 0)
    {
        media_trace_read_proc();
        return 0;
    }
    if (strcmp(mod, "mem") == 0)
    {
        media_mem_proc();
        return 0;
    }
    if (strcmp(mod, "mipi") == 0)
    {
#ifdef CONFIG_ARCH_FH8626V100
        rt_kprintf("not supported on this platform!\n");
#else
        mipi_read_proc();
#endif
        return 0;
    }
    if (strcmp(mod, "nna") == 0)
    {
        nna_read_proc();
        return 0;
    }
    return 1;
}

static int write_media_info(char *mod, char *config)
{
    if (strcmp(mod, "isp") == 0)
    {
        isp_write_proc(config);
        return 0;
    }
    if (strcmp(mod, "enc") == 0)
    {
        enc_write_proc(config);
        return 0;
    }
    if (strcmp(mod, "jpeg") == 0)
    {
        jpeg_write_proc(config);
        return 0;
    }
    if (strcmp(mod, "bgm") == 0)
    {
        bgm_write_proc(config);
        return 0;
    }
    if (strcmp(mod, "vpu") == 0)
    {
        vpu_write_proc(config);
        return 0;
    }
    if (strcmp(mod, "trace") == 0)
    {
        media_trace_write_proc(config);
        return 0;
    }
    if (strcmp(mod, "mipi") == 0)
    {
#ifdef CONFIG_ARCH_FH8626V100
        rt_kprintf("not supported on this platform!\n");
#else
        mipi_write_proc(config);
#endif
        return 0;
    }
    if (strcmp(mod, "nna") == 0)
    {
        nna_write_proc(config);
        return 0;
    }

    return 1;
}

int cmd_media_info(int argc, char *argv[])
{
    if (argc < 2)
    {
        print_usage();
        return 0;
    }
    if (strcmp(argv[1], "-h") == 0)
    {
        print_usage();
        return 0;
    }
    if (strcmp(argv[1], "-r") == 0)
    {
        if (argc != 3)
        {
            print_usage();
            return 0;
        }

        if( read_media_info(argv[2]) == 0)
            return 0;

        print_usage();
        return 0;
    }

    if (argv[1][0] != '-')
    {
        if (read_media_info(argv[1]) == 0)
            return 0;

        print_usage();
        return 0;
    }

    if (strcmp(argv[1], "-w") == 0)
    {
        if (argc != 4)
        {
            print_usage();
            return 0;
        }
        if (write_media_info(argv[2], argv[3]) == 0)
            return 0;
    }

    print_usage();

    return 0;
}
FINSH_FUNCTION_EXPORT_ALIAS(cmd_cmm_mem_proc, __cmd_cmm_mem_proc, get cmm memory status information);
FINSH_FUNCTION_EXPORT_ALIAS(cmd_media_info, __cmd_media_info, read/write media status information);
#else
FINSH_FUNCTION_EXPORT(isp_read_proc, read proc info);
FINSH_FUNCTION_EXPORT(isp_write_proc, write proc info);

FINSH_FUNCTION_EXPORT(media_mem_proc, media mem use info.e.g
                      : media_mem_proc());
FINSH_FUNCTION_EXPORT(jpeg_write_proc, write jpeg proc info);
FINSH_FUNCTION_EXPORT(jpeg_read_proc, read jpeg proc info);
FINSH_FUNCTION_EXPORT(media_read_proc, get media process proc info);
FINSH_FUNCTION_EXPORT(enc_write_proc, write enc proc info);
FINSH_FUNCTION_EXPORT(enc_read_proc, read enc proc info);
FINSH_FUNCTION_EXPORT(vpu_write_proc, write vpu proc info);
FINSH_FUNCTION_EXPORT(vpu_read_proc, read vpu proc info);
FINSH_FUNCTION_EXPORT(cmm_mem_proc, reserved mem use info.e.g
                      : cmm_mem_proc(index));

#endif
#endif
