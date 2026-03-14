

#define UAC_STREAM_ON   1
#define UAC_STREAM_OFF   0


void ERR_RETURN(char *name, int ret_val)
{
    if (ret_val != 0)
        printf("%s failed(%d)\n", name, ret_val);
}

