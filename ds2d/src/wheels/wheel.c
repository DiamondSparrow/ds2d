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

#include "wheel.h"
#include "pololu.h"
#include "options.h"
#include "sleep.h"
#include "types.h"
#include "debug.h"

pthread_t WHEEL_Thread;
int WHEEL_Run = 0;

void *WHEEL_Control();
static int WHEEL_SpeedControl(wheel_t *wheel);
static int WHEEL_VariableRead(wheel_t *wheel);

int WHEEL_Init(void)
{
	int ret = 0;

	WHEEL_Run = TRUE;

	if (POLOLU_Init(WHEEL_DEV, WHEEL_BAUDRATE) < 0)
	{
		return -1;
	}

	WHEEL_Left.id = WHEEL_LEFT_ID;
	WHEEL_Right.id = WHEEL_RIGHT_ID;

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
	int ret = 0;

	pthread_setname_np(WHEEL_Thread, "ds2d-wheel");

	DEBUG_Print(options.debugWheel, debugWheel, "* started.");
	WHEEL_Left.start = TRUE;
	WHEEL_Right.start = TRUE;

	SLEEP_Delay(1.0);

	while (WHEEL_Run)
	{
		ret = WHEEL_SpeedControl(&WHEEL_Left);
		ret |= WHEEL_SpeedControl(&WHEEL_Right);

		if (ret != 0)
		{
			if (WHEEL_Left.stage == wheelVarWait)
			{
				WHEEL_Left.stage = wheelVarTemperature;
			}
			if (WHEEL_Right.stage == wheelVarWait)
			{
				WHEEL_Right.stage = wheelVarTemperature;
			}
			ret = 0;
		}
		WHEEL_VariableRead(&WHEEL_Left);
		WHEEL_VariableRead(&WHEEL_Right);
		SLEEP_Delay(0.01);
	}

	DEBUG_Print(options.debugWheel, debugWheel, "x stopped.");

	return NULL;
}

static int WHEEL_SpeedControl(wheel_t *wheel)
{
	int ret = 0;

	if (wheel->start)
	{
		POLOLU_ExitSafeStart(wheel->id);
		wheel->start = 0;
		ret = 1;
	}

	if (wheel->speedPrev != wheel->speed)
	{
		wheel->speedPrev = wheel->speed;
		if (wheel->speed > 0)
		{
			POLOLU_MotorForward(wheel->id, wheel->speed);
		}
		else if (wheel->speed < 0)
		{
			POLOLU_MotorReverse(wheel->id, -1 * wheel->speed);
		}
		else
		{
			POLOLU_MotorBreake(wheel->id, wheel->breake);
		}
		ret = 1;
	}

	return ret;
}

static int WHEEL_VariableRead(wheel_t *wheel)
{
	int i = 0;
	switch (wheel->stage)
	{
	case wheelVarTemperature:
		wheel->temperature = (float)POLOLU_GetVariable(wheel->id, POLOLU_VARIABLE_TEMPERATURE) / 10;
		wheel->varDebug = TRUE;
		wheel->stage++;
		break;
	case wheelVarVoltage:
		wheel->voltage = (float)POLOLU_GetVariable(wheel->id, POLOLU_VARIABLE_INPUT_VOLTAGE) / 1000;
		wheel->stage++;
		break;
	case wheelVarErrorStatus:
		wheel->errorStatus = POLOLU_GetVariable(wheel->id, POLOLU_VARIABLE_ERROR_STATUS);
		wheel->errorOccured = POLOLU_GetVariable(wheel->id, POLOLU_VARIABLE_ERRORS_OCCURED);
		wheel->stage++;
		break;
	case wheelVarErrorSerial:
		wheel->errorSerial = POLOLU_GetVariable(wheel->id, POLOLU_VARIABLE_SERIAL_ERRORS_OCCURED);
		wheel->stage++;
		break;
	case wheelVarLimitStatus:
		wheel->limitStatus = POLOLU_GetVariable(wheel->id, POLOLU_VARIABLE_LIMIT_STATUS);
		wheel->stage++;
		break;
	case wheelVarResetFlags:
		wheel->resetFlags = POLOLU_GetVariable(wheel->id, POLOLU_VARIABLE_RESET_FLAGS);
		wheel->stage++;
		break;
	case wheelVarDebug:
		if(wheel->varDebug == TRUE)
		{
			wheel->varDebug = FALSE;
			DEBUG_Print(options.debugWheel, debugWheel, "? %s data - "
					"temperature %05.02f C., voltage %03.02f V., "
					"Errors 0x%04X/0x%04X/%04X, Limits 0x%04X, Reset 0x%02X.",
					wheel->id == WHEEL_LEFT_ID ? "left " : "right",
					wheel->temperature,
					wheel->voltage,
					wheel->errorStatus,
					wheel->errorOccured,
					wheel->errorSerial,
					wheel->limitStatus,
					wheel->resetFlags);
			for(i=0; i<10; i++)
			{
				if(BIT_IS_SET(wheel->errorOccured, i))
				{
					fprintf(stderr, "ERROR: %s wheel error - %s.\n",
							wheel->id == WHEEL_LEFT_ID ? "Left" : "Right",
							POLOLU_Errors[i]);
				}
			}
		}
		wheel->stage++;
		break;
	case wheelVarWait:
		break;
	default:
		wheel->stage = 0;
		break;
	}

	return 0;
}

