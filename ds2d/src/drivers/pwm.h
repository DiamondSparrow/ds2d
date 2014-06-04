/*
 * pwm.h
 *
 *  Created on: May 14, 2014
 *      Author: DiamondS
 */

#ifndef PWM_H_
#define PWM_H_

#define PWM_SYSFS_EHRPWM_PREFIX         "/sys/devices/ocp.3"
#define PWM_SYSFS_EHRPWM_DUTY           "duty"
#define PWM_SYSFS_EHRPWM_PERIOD         "period"
#define PWM_SYSFS_EHRPWM_POLARITY       "polarity"
#define PWM_SYSFS_EHRPWM_RUN            "run"

typedef struct _pwm
{
    // P{port}_{pin}:
    unsigned int port;
    unsigned int pin;
    // Values:
    int duty;
    int period;
    int polarity;
    int run;
    // File Stream's:
    int dutyStream;
    int periodStream;
    int polarityStream;
    int runStream;
} pwm_t;

int PWM_Init(pwm_t *pwm, unsigned int port, unsigned int pin);
int PWM_Deinit(pwm_t *pwm);

int PWM_SetDuty(pwm_t *pwm, int duty);
int PWM_SetPeriod(pwm_t *pwm, int period);
int PWM_SetPolarity(pwm_t *pwm, int polarity);
int PWM_Run(pwm_t *pwm);
int PWM_Stop(pwm_t *pwm);

#endif /* PWM_H_ */
