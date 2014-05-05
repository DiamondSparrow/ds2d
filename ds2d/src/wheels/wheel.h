/*
 * wheel.h
 *
 *  Created on: Nov 27, 2013
 *      Author: DiamondS
 */

#ifndef WHEEL_H_
#define WHEEL_H_

#define WHEEL_DEV		"/dev/ttyO4"
#define WHEEL_BAUDRATE	115200
#define WHEEL_LEFT_ID	2
#define WHEEL_RIGHT_ID	1

typedef enum _wheel_var
{
	wheelVarTemperature,
	wheelVarVoltage,
	wheelVarErrorStatus,
	wheelVarErrorSerial,
	wheelVarLimitStatus,
	wheelVarResetFlags,
	wheelVarDebug,
	wheelVarWait
} wheel_var_e;

typedef struct _wheel
{
	int id;
	float voltage;
	float temperature;
	int speed;
	int speedPrev;
	int speedStep;
	int speedStart;
	int breake;

	wheel_var_e stage;
	int varDebug;

	int start;

	int errorStatus;
	int errorOccured;
	int errorSerial;
	int limitStatus;
	int resetFlags;
} wheel_t;

wheel_t WHEEL_Left;
wheel_t WHEEL_Right;

int WHEEL_Init ( void );
int WHEEL_Close ( void );

#endif /* WHEEL_H_ */
