/*
 * ds2.h
 *
 *  Created on: Oct 12, 2014
 *      Author: DiamondS
 */

#ifndef DS2_H_
#define DS2_H_

typedef struct _ds2_motor
{
    int speed;
    int current;
    int brake;
} ds2_motor_t;

typedef struct _ds2_data
{
    int speed;
    int angle;
    int brake;
    int pan;
    int tilt;

    ds2_motor_t left;
    ds2_motor_t right;

} ds2_data_t;

volatile ds2_data_t ds2_data;

#endif /* DS2_H_ */
