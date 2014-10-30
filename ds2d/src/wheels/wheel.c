/*
 * wheel.c
 *
 *  Created on: Nov 27, 2013
 *      Author: DiamondS
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>

#include "wheel.h"
#include "qik.h"
#include "options.h"
#include "sleep.h"
#include "types.h"
#include "debug.h"
#include "config.h"
#include "ds2.h"

pthread_t WHEEL_Thread;
int WHEEL_Run = 0;
wheel_t WHEEL_Data = {{0}, {0}};

void *WHEEL_Control();
static void WHEEL_GetSpeed(double velocity, double angle, double *leftSpeed, double *rightSpeed);
static void WHEEL_ControlSpeed(void);
static void WHEEL_FeedbackControl(void);

int WHEEL_Init(void)
{
    int ret = 0;

    WHEEL_Run = TRUE;

    if (QIK_Init(configuration.wheels.device, configuration.wheels.speed, (unsigned char)QIK_DEFAULT_DEVICE_ID) < 0)
    {
        return -1;
    }

    if ((ret = pthread_create(&WHEEL_Thread, NULL, WHEEL_Control, NULL)))
    {
        fprintf(stderr, "ERROR: Failed to create wheel control thread: %s.\n",
                strerror(errno));
        return -2;
    }

    return 0;
}

int WHEEL_Close(void)
{
    WHEEL_Run = FALSE;
    pthread_join(WHEEL_Thread, NULL);

    return 0;
}

void *WHEEL_Control(void)
{
    pthread_setname_np(WHEEL_Thread, "ds2d-wheel");

    DEBUG_Print(options.debugWheel, debugWheel, "* started.");

    SLEEP_Delay(1.0);

    while (WHEEL_Run)
    {
        WHEEL_ControlSpeed();
        WHEEL_FeedbackControl();
        DEBUG_Print(options.debugWheel, debugWheel,
                "left - speed = %+04.0f, current = %04d mA.; right - speed = %+04.0f, current = %04d mA; error = %02X",
                WHEEL_Data.left.speed,
                WHEEL_Data.left.current,
                WHEEL_Data.right.speed,
                WHEEL_Data.right.current,
                WHEEL_Data.error);
        ds2_data.left.speed     = WHEEL_Data.left.speed;
        ds2_data.left.current   = WHEEL_Data.left.current;
        ds2_data.left.brake      = WHEEL_Data.left.brake;
        ds2_data.right.speed    = WHEEL_Data.right.speed;
        ds2_data.right.current  = WHEEL_Data.right.current;
        ds2_data.right.brake    = WHEEL_Data.right.brake;

        SLEEP_Delay(options.debugWheel ? 0.01 : 0.001);
    }

    DEBUG_Print(options.debugWheel, debugWheel, "x stopped.");

    return NULL;
}

static void WHEEL_GetSpeed(double velocity, double angle, double *leftSpeed, double *rightSpeed)
{
    if (angle < 0.0)
    {
        angle = 360.0 + angle;
    }

    if (angle == 90.0)      // Left and Right Forward
    {
        *leftSpeed = velocity;
        *rightSpeed = velocity;
    }
    if (angle < 90.0 && angle > 0.0)
    {
        *leftSpeed = velocity;
        *rightSpeed = ceil((velocity * ((angle - 45) * 2.222)) / 100);
    }

    if (angle == 0.0 || angle == 360)
    {
        *leftSpeed = velocity;
        *rightSpeed = -velocity;
    }
    if (angle < 180.0 && angle > 90.0)
    {
        *leftSpeed = ceil((velocity * ((angle - 135) * -2.222)) / 100);
        *rightSpeed = velocity;
    }
    if (angle == 180.0)
    {
        *leftSpeed = -velocity;
        *rightSpeed = velocity;
    }
    if (angle > 180.0 && angle < 270.0)
    {
        *leftSpeed = -velocity;
        *rightSpeed = ceil((velocity * ((angle - 225) * -2.222)) / 100);
    }
    if (angle == 270.0)
    {
        *leftSpeed = -velocity;
        *rightSpeed = -velocity;
    }
    if (angle > 270.0 && angle < 360.0)
    {
        *leftSpeed = ceil((velocity * ((angle - 315) * 2.222)) / 100);
        *rightSpeed = -velocity;
    }

    return;
}

static void WHEEL_ControlSpeed(void)
{
    int    brake = 0;
    double speed = 0;
    double angle = 0;
    double leftSpeed = 0;
    double rightSpeed = 0;

    brake = ds2_data.brake;
    speed = ds2_data.speed;
    angle = ds2_data.angle;

    if(brake > 0)
    {
        QIK_M0_Brake(brake);
        QIK_M1_Brake(brake);

        WHEEL_Data.left.speed  = 0;
        WHEEL_Data.left.brake  = brake;
        WHEEL_Data.right.speed = 0;
        WHEEL_Data.right.brake = brake;
    }
    else
    {
        WHEEL_GetSpeed(speed, angle, &leftSpeed, &rightSpeed);

        leftSpeed  = (leftSpeed  * (127.0 / 100.0));
        rightSpeed = (rightSpeed * (127.0 / 100.0));

        if(leftSpeed > 0)
        {
            QIK_M0_Forward((unsigned char)leftSpeed);
        }
        else if(leftSpeed < 0)
        {
            QIK_M0_Reverse((unsigned char)((-1) * leftSpeed));
        }
        else
        {
            QIK_M0_Brake(0);
        }

        if(rightSpeed > 0)
        {
            QIK_M1_Forward((unsigned char)rightSpeed);
        }
        else if(rightSpeed < 0)
        {
            QIK_M1_Reverse((unsigned char)((-1) * rightSpeed));
        }
        else
        {
            QIK_M1_Brake(0);
        }

        WHEEL_Data.left.speed  = leftSpeed;
        WHEEL_Data.left.brake  = 0;
        WHEEL_Data.right.speed = rightSpeed;
        WHEEL_Data.right.brake = 0;
    }

    return;
}

static void WHEEL_FeedbackControl(void)
{
    WHEEL_Data.left.current  = QIK_M0_GetCurrent();
    WHEEL_Data.right.current = QIK_M1_GetCurrent();
    WHEEL_Data.error = QIK_GetErrorByte();

    return;
}

