/*
 * indication.c
 *
 *  Created on: Apr 26, 2014
 *      Author: DiamondS
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>

#include "indication.h"
#include "options.h"
#include "sleep.h"
#include "types.h"
#include "debug.h"
#include "gpio.h"

void INDICATION_LedControl(indication_led_t *indicationLed);
void *INDICATION_Handler();

pthread_t INDICATION_Thread = {0};
unsigned int INDICATION_Run = FALSE;

int INDICATION_Init(void)
{
	int ret = 0;

	memset(&indication, 0, sizeof indication);

	indication.blue.ledNumber = INDICATION_GPIO_LED_BLUE;
	indication.green.ledNumber = INDICATION_GPIO_LED_GREEN;
	indication.yellow.ledNumber = INDICATION_GPIO_LED_YELLOW;
	indication.red.ledNumber = INDICATION_GPIO_LED_RED;

	GPIO_Export(INDICATION_GPIO_LED_BLUE);
	GPIO_Export(INDICATION_GPIO_LED_GREEN);
	GPIO_Export(INDICATION_GPIO_LED_YELLOW);
	GPIO_Export(INDICATION_GPIO_LED_RED);

	GPIO_SetDirection(INDICATION_GPIO_LED_BLUE, gpioOutput);
	GPIO_SetDirection(INDICATION_GPIO_LED_GREEN, gpioOutput);
	GPIO_SetDirection(INDICATION_GPIO_LED_YELLOW, gpioOutput);
	GPIO_SetDirection(INDICATION_GPIO_LED_RED, gpioOutput);

	GPIO_SetValue(INDICATION_GPIO_LED_BLUE, gpioLow);
	GPIO_SetValue(INDICATION_GPIO_LED_GREEN, gpioLow);
	GPIO_SetValue(INDICATION_GPIO_LED_YELLOW, gpioLow);
	GPIO_SetValue(INDICATION_GPIO_LED_RED, gpioLow);

	INDICATION_Run = TRUE;
	if( (ret = pthread_create( &INDICATION_Thread, NULL, &INDICATION_Handler, NULL)))
	{
		fprintf(stderr,"ERROR: Failed to create indication thread (%d). %s.\n", ret, strerror(errno));
		return -1;
	}

	return 0;
}

int INDICATION_Close(void)
{
	INDICATION_Run = FALSE;
	pthread_join(INDICATION_Thread, NULL);

	GPIO_SetValue(INDICATION_GPIO_LED_BLUE, gpioLow);
	GPIO_SetValue(INDICATION_GPIO_LED_GREEN, gpioLow);
	GPIO_SetValue(INDICATION_GPIO_LED_YELLOW, gpioLow);
	GPIO_SetValue(INDICATION_GPIO_LED_RED, gpioLow);
/*
	GPIO_Unexport(INDICATION_GPIO_LED_BLUE);
	GPIO_Unexport(INDICATION_GPIO_LED_GREEN);
	GPIO_Unexport(INDICATION_GPIO_LED_YELLOW);
	GPIO_Unexport(INDICATION_GPIO_LED_RED);
*/

	return 0;
}

void INDICATION_LedControl(indication_led_t *indicationLed)
{
	switch(indicationLed->type)
	{
	case ledON:
		// Turn on led
		if(indicationLed->typeCurrent != ledON)
		{
			indicationLed->typeCurrent = ledON;
			GPIO_SetValue(indicationLed->ledNumber, gpioHigh);
		}
		break;
	case ledOFF:
		// Turn off led.
		if(indicationLed->typeCurrent != ledOFF)
		{
			indicationLed->typeCurrent = ledOFF;
			GPIO_SetValue(indicationLed->ledNumber, gpioLow);
		}
		break;
	case ledBlinky:
		if(indicationLed->blinkyPeriodCounter == 0)
		{
			// Turn on led.
			GPIO_SetValue(indicationLed->ledNumber, gpioHigh);
			indicationLed->blinkyPeriodCounter++;
		}
		else if(indicationLed->blinkyPeriodCounter == indicationLed->blinkyOnTime)
		{
			// Turn off led.
			GPIO_SetValue(indicationLed->ledNumber, gpioLow);
			indicationLed->blinkyPeriodCounter++;
		}
		else if(indicationLed->blinkyPeriodCounter == indicationLed->blinkyOffTime)
		{
			indicationLed->blinkyPeriodCounter = 0;
		}
		else
		{
			indicationLed->blinkyPeriodCounter++;
		}
		break;
	default:
		break;
	}

	return;
}

void *INDICATION_Handler()
{
	pthread_setname_np(INDICATION_Thread, "ds2d-indication");

	DEBUG_Print(options.debugIndication, debugIndication, "* started.");

	indication.blue.type = ledBlinky;
	indication.blue.blinkyOnTime = 10;
	indication.blue.blinkyOffTime = 90;

	while(INDICATION_Run)
	{
		INDICATION_LedControl(&indication.blue);
		INDICATION_LedControl(&indication.green);
		INDICATION_LedControl(&indication.red);
		INDICATION_LedControl(&indication.yellow);

		if(indication.blue.type != ledBlinky &&
				indication.green.type != ledBlinky &&
				indication.red.type  != ledBlinky &&
				indication.yellow.type  != ledBlinky)
		{
			SLEEP_Delay(0.1);
		}
		else
		{
			SLEEP_Delay(0.01);
		}
	}

	DEBUG_Print(options.debugIndication, debugIndication, "x stopped.");

	return (NULL);
}
