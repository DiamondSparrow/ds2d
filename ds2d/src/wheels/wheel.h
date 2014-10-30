/*
 * wheel.h
 *
 *  Created on: Nov 27, 2013
 *      Author: DiamondS
 */

#ifndef WHEEL_H_
#define WHEEL_H_


typedef struct _wheel_motor
{
    double speed;
    int current;
    int brake;

} wheel_motor_t;

typedef struct _wheel
{
    wheel_motor_t left;
    wheel_motor_t right;
    unsigned char error;
} wheel_t;

int WHEEL_Init(void);
int WHEEL_Close(void);

#endif /* WHEEL_H_ */
