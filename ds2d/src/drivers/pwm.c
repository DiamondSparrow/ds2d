/*
 * pwm.c
 *
 *  Created on: May 28, 2014
 *      Author: DiamondS
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>

#include "pwm.h"

int PWM_Init(pwm_t *pwm, unsigned int port, unsigned int pin)
{
    char file[64] = {0};

    pwm->port = port;
    pwm->pin = pin;

    sprintf(file, "%s/pwm_test_P%d_%d.14/%s", PWM_SYSFS_EHRPWM_PREFIX,
            pwm->port, pwm->pin, PWM_SYSFS_EHRPWM_DUTY);
    if ((pwm->dutyStream = open(file, O_WRONLY)) < 0)
    {
        fprintf(stderr, "ERROR: Failed to open to file '%s'. %s.\n", file,
                strerror(errno));
        return -1;
    }

    sprintf(file, "%s/pwm_test_P%d_%d.14/%s", PWM_SYSFS_EHRPWM_PREFIX,
            pwm->port, pwm->pin, PWM_SYSFS_EHRPWM_PERIOD);
    if ((pwm->periodStream = open(file, O_WRONLY)) < 0)
    {
        fprintf(stderr, "ERROR: Failed to open to file '%s'. %s.\n", file,
                strerror(errno));
        return -2;
    }

    sprintf(file, "%s/pwm_test_P%d_%d.14/%s", PWM_SYSFS_EHRPWM_PREFIX,
            pwm->port, pwm->pin, PWM_SYSFS_EHRPWM_POLARITY);
    if ((pwm->polarityStream = open(file, O_WRONLY)) < 0)
    {
        fprintf(stderr, "ERROR: Failed to open to file '%s'. %s.\n", file,
                strerror(errno));
        return -3;
    }

    sprintf(file, "%s/pwm_test_P%d_%d.14/%s", PWM_SYSFS_EHRPWM_PREFIX,
            pwm->port, pwm->pin, PWM_SYSFS_EHRPWM_RUN);
    if ((pwm->runStream = open(file, O_WRONLY)) < 0)
    {
        fprintf(stderr, "ERROR: Failed to open to file '%s'. %s.\n", file,
                strerror(errno));
        return -4;
    }

    return 0;
}

int PWM_Deinit(pwm_t *pwm)
{
    PWM_Stop(pwm);

    close(pwm->dutyStream);
    close(pwm->periodStream);
    close(pwm->polarityStream);
    close(pwm->runStream);

    return 0;
}

int PWM_SetDuty(pwm_t *pwm, int duty)
{
    char data[9] = {0};

    sprintf(data, "%d", duty);
    if (write(pwm->dutyStream, data, strlen(data) + 1) != (strlen(data) + 1))
    {
        fprintf(stderr, "ERROR: Failed to write to file. %s.\n",
                strerror(errno));
        return -1;
    }

    return 0;
}

int PWM_SetPeriod(pwm_t *pwm, int period)
{
    char data[9] =
    { 0 };

    sprintf(data, "%d", period);
    if (write(pwm->periodStream, data, strlen(data) + 1) != (strlen(data) + 1))
    {
        fprintf(stderr, "ERROR: Failed to write to file. %s.\n",
                strerror(errno));
        return -1;
    }

    return 0;
}

int PWM_SetPolarity(pwm_t *pwm, int polarity)
{
    if (polarity == 1)
    {
        if (write(pwm->polarityStream, "1", 1) != 1)
        {
            fprintf(stderr, "ERROR: Failed to write to file. %s.\n",
                    strerror(errno));
            return -1;
        }
    }
    else
    {
        if (write(pwm->polarityStream, "0", 1) != 1)
        {
            fprintf(stderr, "ERROR: Failed to write to file. %s.\n",
                    strerror(errno));
            return -1;
        }
    }

    return 0;
}

int PWM_Run(pwm_t *pwm)
{
    if (write(pwm->runStream, "1", 1) != 1)
    {
        fprintf(stderr, "ERROR: Failed to write to file. %s.\n",
                strerror(errno));
        return -1;
    }

    return 0;
}

int PWM_Stop(pwm_t *pwm)
{
    if (write(pwm->runStream, "0", 1) != 1)
    {
        fprintf(stderr, "ERROR: Failed to write to file. %s.\n",
                strerror(errno));
        return -1;
    }

    return 0;
}
