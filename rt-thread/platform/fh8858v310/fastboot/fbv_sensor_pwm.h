#ifndef __SENSOR_PWM_H__
#define __SENSOR_PWM_H__

#define MAX_PWM_CHANNUM (8)

typedef struct
{
    int groupid;
    int pwm_cnt;
    unsigned int freq;
    unsigned char chn_param[MAX_PWM_CHANNUM];
} pwm_motor_config_t;

typedef struct
{
    int groupid;
    int pwm_cnt;
    int period_ns[MAX_PWM_CHANNUM];
    int duty_ns[MAX_PWM_CHANNUM];
    int phase_ns[MAX_PWM_CHANNUM];
} pwm_motor_config2_t;

typedef unsigned int __u32;

#ifndef NSEC_PER_SEC
#define NSEC_PER_SEC (1000000000ULL)
#endif
#define DEVICE_NAME "/dev/pwm"
#define ENABLE_PWM                      0
#define DISABLE_PWM                     1
#define SET_PWM_CONFIG                  2
#define GET_PWM_CONFIG                  3
#define SET_PWM_DUTY_CYCLE_PERCENT      4
#define SET_PWM_ENABLE                  5
#define SET_PWM_STOP_CTRL               6
#define ENABLE_MUL_PWM                  7
#define ENABLE_FINSHALL_INTR            8
#define ENABLE_FINSHONCE_INTR           9
#define DISABLE_FINSHALL_INTR           10
#define DISABLE_FINSHONCE_INTR          11

struct fh_pwm_config
{
    unsigned int period_ns;
    unsigned int duty_ns;
#define  FH_PWM_PULSE_LIMIT         (0x0)
#define  FH_PWM_PULSE_NOLIMIT       (0x1)
    unsigned int pulses;
    unsigned int pulse_num;
#define FH_PWM_STOPLVL_LOW          (0x0)
#define FH_PWM_STOPLVL_HIGH         (0x3)
#define FH_PWM_STOPLVL_KEEP         (0x1)
    unsigned int stop;
#define FH_PWM_STOPCTRL_ATONCE (0x10)
#define FH_PWM_STOPCTRL_AFTERFINISH (0x0)
    unsigned int stop_ctrl;
    unsigned int delay_ns;
    unsigned int phase_ns;
    unsigned int percent;
    unsigned int shadow_enable;
    unsigned int finish_once;
    unsigned int finish_all;
};

struct fh_pwm_status
{
    unsigned int done_cnt;
    unsigned int total_cnt;
    unsigned int busy;
    unsigned int error;
};

struct fh_pwm_chip_data
{
    int id;
    struct fh_pwm_config config;
    struct fh_pwm_status status;
    void (*finishall_callback)(struct fh_pwm_chip_data *data);
    void (*finishonce_callback)(struct fh_pwm_chip_data *data);
};
#endif /* __SENSOR_PWM_H__ */
