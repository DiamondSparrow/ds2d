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

void INDICATION_LedControl(indication_led_t *indicationLed);
void *INDICATION_Handler();

pthread_t INDICATION_Thread = {0};
unsigned int INDICATION_Run = FALSE;

int INDICATION_Init(void)
{
	int ret = 0;

	memset(&indication, 0, sizeof indication);

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

	return 0;
}

void INDICATION_LedControl(indication_led_t *indicationLed)
{
	switch(indicationLed->type)
	{
	case ledON:
		// Turn on led
		break;
	case ledOFF:
		// Turn off led.
		break;
	case ledBlinky:
		if(indicationLed->blinkyPeriodCounter == 0)
		{
			// Turn on led.
		}
		else if(indicationLed->blinkyPeriodCounter == indicationLed->blinkyOnTime)
		{
			// Turn off led.
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
