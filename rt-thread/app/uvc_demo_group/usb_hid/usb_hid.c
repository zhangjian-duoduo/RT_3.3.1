#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include <fh_usb_device.h>
#include <usb_update.h>
#include <sys/prctl.h>
#include "usb_hid.h"

static int g_hid_fp = 0;

struct hid_report
{
    uint8_t report_id;
    uint8_t report[63];
    uint8_t size;
};

static void dump_data(uint8_t *data, uint8_t size)
{
    int i;

    for (i = 0; i < size; i++)
    {
        printf("%02x ", *data++);
        if ((i + 1) % 8 == 0)
            printf("\n");
        else if ((i + 1) % 4 == 0)
            printf(" ");
    }
    printf("\n");
}

static void dump_report(struct hid_report *report)
{
    printf("\nHID Recived:");
    printf("\nReport ID %02x\n", report->report_id);
    dump_data(report->report, report->size);
}

unsigned char telephony_status = 0;
static int hid_tele_report(int fp, char id, char val)
{
    int ret = 0;
    char buf[64] = {0};

    /* key press */
    lseek(fp, id, SEEK_SET);
    buf[0] = val;
    ret = write(fp, buf, 1);
    if (ret < 0)
    {
        printf("HID:WriteFail %d(L%d)\n", ret, __LINE__);
        return ret;
    }
    return 0;
}


void hid_send_cmd(char val)
{
    if (telephony_status & HID_TELEPHONY_OUT_OFF_HOOK)
        val |= HID_TELEPHONY_IN_HOOK_SWITCH;
    hid_tele_report(g_hid_fp, HID_REPORT_ID_TELEPHONY, val);
}

static void *hid_thread_entry(void *parameter)
{
    struct hid_report report;
    int size = 0;
    uint8_t buf[64];

    prctl(PR_SET_NAME, "hid_thread_entry");
    while (1)
    {
        size = read(g_hid_fp, buf, sizeof(buf));
        if (size < 0)
        {
            printf("%s--%d: hid read failed %d\n", __func__, __LINE__, size);
            continue;
        }
        if (size > 0)
        {
            memcpy(&report, buf, size);
            report.size = size - 1;
#ifdef APP_USB_DEVICE_UAC
            if (report.report_id == HID_REPORT_ID_TELEPHONY)
            {
                if (report.report[0] & HID_TELEPHONY_OUT_OFF_HOOK)
                {
                    telephony_status |= HID_TELEPHONY_OUT_OFF_HOOK;
                }
                else
                {
                    telephony_status &= ~(HID_TELEPHONY_OUT_OFF_HOOK);
                }
                if (report.report[0] & HID_TELEPHONY_OUT_MUTE)
                {
                    telephony_status |= HID_TELEPHONY_OUT_MUTE;
                    if (hid_mic_mute_status == 0)
                    {
                        _audio_set_mute(1, 0);
                    }
                }
                else
                {
                    telephony_status &= ~(HID_TELEPHONY_OUT_MUTE);
                    if (hid_mic_mute_status == 0)
                    {
                        _audio_set_mute(0, 0);
                    }
                }
            }
#endif
            dump_report(&report);
            usb_update_check(report.report, report.size);
        }
        usleep(100*1000);
    }

    return NULL;
}


int hid_key_press(int fp, char num)
{
    int ret = 0;
    char buf[HID_INPUT_REPORT_LEN] = {0};

    /* key press */
    lseek(fp, HID_REPORT_ID_KEYBOARD1, SEEK_SET);
    buf[2] = num;
    ret = write(fp, buf, HID_INPUT_REPORT_LEN);
    if (ret < 0)
    {
        printf("HID:WriteFail %d(L%d)\n", ret, __LINE__);
        return ret;
    }
    return 0;
}

int hid_key_release(int fp)
{
    int ret = 0;
    char buf[HID_INPUT_REPORT_LEN] = {0};

    /* key press */
    lseek(fp, HID_REPORT_ID_KEYBOARD1, SEEK_SET);
    buf[2] = 0;
    ret = write(fp, buf, HID_INPUT_REPORT_LEN);
    if (ret < 0)
    {
        printf("HID:WriteFail %d(L%d)\n", ret, __LINE__);
        return ret;
    }
    return 0;
}

int hid_consumer_press(int fp, char num)
{
    int ret = 0;
    char buf[HID_INPUT_REPORT_LEN] = {0};

    /* key press */
    lseek(fp, HID_REPORT_ID_KEYBOARD4, SEEK_SET);
    buf[0] = num;
    ret = write(fp, buf, HID_INPUT_REPORT_LEN);
    if (ret < 0)
    {
        printf("HID:WriteFail %d(L%d)\n", ret, __LINE__);
        return ret;
    }
    return 0;
}

int hid_consumer_release(int fp)
{
    int ret = 0;
    char buf[HID_INPUT_REPORT_LEN] = {0};

    /* key press */
    lseek(fp, HID_REPORT_ID_KEYBOARD4, SEEK_SET);
    buf[0] = 0;
    ret = write(fp, buf, HID_INPUT_REPORT_LEN);
    if (ret < 0)
    {
        printf("HID:WriteFail %d(L%d)\n", ret, __LINE__);
        return ret;
    }
    return 0;
}

int hid_general_send(int fp)
{
    int ret = 0;
    char buf[63] = {0};
    int i = 0;

    lseek(fp, HID_REPORT_ID_GENERAL, SEEK_SET);
    for (i = 0; i < sizeof(buf); i++)
        buf[i] = i;

    ret = write(fp, buf, sizeof(buf));
    if (ret < 0)
    {
        printf("HID:WriteFail %d(L%d)\n", ret, __LINE__);
        return ret;
    }
    return 0;
}

int hid_demo(void)
{
    struct sched_param param;
    pthread_attr_t attr;
    static pthread_t thread_hid;
    int fp = 0;
    int i = 0;

#if !defined(FH_APP_USING_COOLVIEW) && !defined(APP_USB_DEVICE_UAC) && !defined(APP_USB_DEVICE_UVC)
    fh_hid_init();
    sleep(2);   /* wait for usb enumeration to complete */
#endif

    fp = open("/dev/hidd", O_RDWR);
    if (fp == -1)
    {
        printf("[/dev/hidd] open hidd failed\n");
        return 0;
    }
    g_hid_fp = fp;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize(&attr, 2*1024);

    param.sched_priority = 130;
    pthread_attr_setschedparam(&attr, &param);
    if (pthread_create(&thread_hid, &attr, hid_thread_entry, NULL) != 0)
    {
        printf("Error: Create thread_hid thread failed!\n");
    }

    for (i = 0; i < 5; i++)
    {
        /* example */
#if 0
        printf("[/dev/hidd] open hidd %d\n", i);
        /* key press */
        hid_key_press(fp, HID_KEY_CODE_a);
        /* key release */
        usleep(10000);
        hid_key_release(fp);

        /* key press */
        usleep(500000);
        hid_key_press(fp, HID_KEY_CODE_1);
        /* key release */
        usleep(10000);
        hid_key_release(fp);

        /* key press */
        usleep(500000);
        hid_key_press(fp, HID_KEY_CODE_UpArrow);
        /* key release */
        usleep(10000);
        hid_key_release(fp);

        /* key press */
        usleep(500000);
        hid_key_press(fp, HID_KEY_CODE_DownArrow);
        /* key release */
        usleep(10000);
        hid_key_release(fp);

        /* key press */
        usleep(500000);
        hid_consumer_press(fp, HID_KEY_CODE_Volume_Up);
        /* key release */
        usleep(10000);
        hid_consumer_release(fp);

        /* key press */
        usleep(500000);
        hid_consumer_press(fp, HID_KEY_CODE_Volume_Down);
        /* key release */
        usleep(10000);
        hid_consumer_release(fp);

        /* key press */
        usleep(500000);
        hid_consumer_press(fp, HID_KEY_CODE_Volume_MUTE);
        /* key release */
        usleep(10000);
        hid_consumer_release(fp);

        hid_general_send(fp);
#endif
        sleep(5);
    }
    return 0;
}
