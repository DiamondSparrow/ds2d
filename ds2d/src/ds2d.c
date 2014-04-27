/*
 ============================================================================
 Name        : ds2d.c
 Author      : DiamondS
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "daemonize.h"
#include "options.h"
#include "sleep.h"
#include "types.h"
#include "debug.h"

#include "indication.h"
#include "remote.h"

volatile unsigned int run = 1;

void terminate(int signalNumber);

int main(int argc, char *argv[])
{
	OPTIONS_Init(&options, argc, argv);

	if(!options.debug)
	{
		DAEMONIZE_LogOpen();
		DAEMONIZE_Init();
	}
	else
	{
		printf("\n\t%s %d.%d.%d.%d (%s)\n\n", OPTIONS_SOFTWARE_NAME,
				OPTIONS_SOFTWARE_MAJOR,
				OPTIONS_SOFTWARE_MINOR,
				OPTIONS_SOFTWARE_MAINTENANCE,
				OPTIONS_SOFTWARE_BUILD,
				OPTIONS_SOFTWARE_EDITION);
	}

	signal(SIGINT, terminate);
	signal(SIGTERM, terminate);

	if(INDICATION_Init() < 0)
	{
		fprintf(stderr, "ERROR: Failed to initialize indication.\n");
		exit(EXIT_FAILURE);
	}
	if(REMOTE_Init(options.tcpPort) < 0)
	{
		fprintf(stderr, "ERROR: Failed to initialize remote.\n");
		exit(EXIT_FAILURE);
	}

	SLEEP_Delay(0.1);

	DEBUG_Print(options.debug, debugMain, "runnning.");

	while(run)
	{
		SLEEP_Delay(1.0);
	}

	REMOTE_Close();
	INDICATION_Close();

	if(!options.debug)
	{
		DAEMONIZE_LogClose();
	}

	return EXIT_SUCCESS;
}

void terminate(int signalNumber)
{
	if(options.debug)
	{
		printf("\n");
	}
	DEBUG_Print(options.debug, debugMain, "Terminate.");
	run = 0;

	return;
}

