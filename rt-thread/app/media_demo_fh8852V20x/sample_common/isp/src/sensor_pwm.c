#ifdef __LINUX_OS__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <poll.h>
#include <signal.h>
#include <getopt.h>
#include <time.h>
#include "sensor_pwm.h"
#include "isp/isp_api.h"

#ifdef MULTI_SENSOR
#define DEV_OPEN(a, flag) open(a, flag)
#define DEV_CLOSE(a) close(a)
static PWM_CONFIG_DATA g_pwm_data;

// 初始化PWM
void FH_PWM_Init(int pwmId)
{
    char cmd[100];

    system("echo 'mux,PWM10,PWM10,1' > /proc/driver/pinctrl");

    // 设置PWM的控制时钟
    system("echo 'pwm_clk,enable,10000000' > /proc/driver/clock");

    sprintf(cmd, "echo dev,PWM%d,PWM%d,%d > /proc/driver/pinctrl", pwmId, pwmId, pwmId);

    system(cmd);
}

// 设置PWM参数
void FH_PWM_setConfig(FH_PWM_CONF *pwm_conf)
{
    if (pwm_conf == NULL)
    {
        perror("illegal parameters!!!");
    }
    g_pwm_data.id = pwm_conf->id;
    g_pwm_data.config.period_ns = pwm_conf->period_ns; 
    g_pwm_data.config.duty_ns = pwm_conf->duty_ns;     
    g_pwm_data.config.delay_ns = pwm_conf->delay_ns;   
    g_pwm_data.config.phase_ns = pwm_conf->phase_ns;
    g_pwm_data.config.pulses = pwm_conf->pulses;
}

void FH_PWM_Start(void)
{
    int pwm_fd; 
    unsigned int pwm_mask;
    pwm_mask = 1 << g_pwm_data.id;

    pwm_fd = DEV_OPEN(DEVICE_NAME, O_RDWR | O_SYNC);
    ioctl(pwm_fd, SET_PWM_DUTY_CYCLE, &g_pwm_data);
    ioctl(pwm_fd, ENABLE_MUL_PWM, &pwm_mask);
    DEV_CLOSE(pwm_fd);
}

void FH_PWM_Stop(void)
{
    int pwm_fd;
    unsigned int pwm_mask;
    pwm_mask = 1 << g_pwm_data.id;

    pwm_fd = DEV_OPEN(DEVICE_NAME, O_RDWR | O_SYNC);
    ioctl(pwm_fd, DISABLE_MUL_PWM, &pwm_mask);
}

void FH_PWM_RateChange(char *rate)
{
    char cmd[100];
    sprintf(cmd, "echo 'pwm_clk,enable,%s' > /proc/driver/clock", rate);
    system(cmd);
}

timer_t timerid;
void timer_handler(int signum, siginfo_t *siginfo, void *secret)
{
    API_ISP_SetSensorReg(1, 0x03fe, 0x0000); // reset sensor 2

    // 同时启动两个sensor
    API_ISP_SetSensorReg(0, 0x023e, 0x0099);
    API_ISP_SetSensorReg(1, 0x023e, 0x0099);
    
    timer_delete(timerid);
}
void FH_TimerInit(unsigned int microseconds)
{
    struct sigevent sev;
    struct itimerspec its;
    struct sigaction sa;

    // 注册定时器中断处理函数
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = timer_handler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGRTMIN, &sa, NULL);

    // 创建实时定时器
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGRTMIN;
    sev.sigev_value.sival_ptr = &timerid;
    timer_create(CLOCK_MONOTONIC, &sev, &timerid);

    // 设置定时器间隔
    its.it_value.tv_sec = microseconds / 1000000;
    its.it_value.tv_nsec = (microseconds % 1000000) * 1000;
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;

    // 启动定时器
    timer_settime(timerid, 0, &its, NULL);
}


void FH_SensorSequeCreate_Handler(int signum)
{
    API_ISP_SetSensorReg(0, 0x03fe, 0x0000);

    FH_TimerInit(32840);
}
#endif /* MULTI_SENSOR */
#endif /* __LINUX_OS__ */
