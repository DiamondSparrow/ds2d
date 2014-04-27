/*
 * indication.h
 *
 *  Created on: Apr 26, 2014
 *      Author: DiamondS
 */

#ifndef INDICATION_H_
#define INDICATION_H_

#define INDICATION_GPIO_LED_BLUE	66
#define INDICATION_GPIO_LED_GREEN	67
#define INDICATION_GPIO_LED_YELLOW	69
#define INDICATION_GPIO_LED_RED		68

typedef enum _type
{
	ledOFF,
	ledON,
	ledBlinky
} indication_type_e;

typedef struct _indication_led
{
	unsigned int ledNumber;
	indication_type_e type;
	indication_type_e typeCurrent;
	unsigned int blinkyOnTime;	// in deciseconds.
	unsigned int blinkyOffTime;	// in deciseconds.
	unsigned int blinkyPeriodCounter;
} indication_led_t;

typedef struct _indication
{
	indication_led_t blue;
	indication_led_t green;
	indication_led_t yellow;
	indication_led_t red;
} indication_t;

indication_t indication;

int INDICATION_Init(void);
int INDICATION_Close(void);

#endif /* INDICATION_H_ */
