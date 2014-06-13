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
#include "wheel.h"
#include "pwm.h"
#include "gps_api.h"

volatile unsigned int run = 1;

void terminate(int signalNumber);

int main(int argc, char *argv[])
{
    OPTIONS_Init(&options, argc, argv);

    if (!options.debug)
    {
        DAEMONIZE_Init();
    }
    else
    {
        printf("\033[2J\033[1;1H\033[37m");
        printf("\033[1;15H .-'/ ,_  \'-.   ");
        printf("\033[2;15H/  (  ( >  )  \\ ");
        printf("\033[3;15H\\   '-' '-'   / ");
        printf("\033[4;15H'-..__ __..-'    ");
        printf("\033[5;15H      /_\\       ");
        printf("\033[0m");
        printf("\n\t%s %d.%d.%d.%d (%s)\n\n",
                OPTIONS_SOFTWARE_NAME,
                OPTIONS_SOFTWARE_MAJOR,
                OPTIONS_SOFTWARE_MINOR,
                OPTIONS_SOFTWARE_MAINTENANCE,
                OPTIONS_SOFTWARE_BUILD,
                OPTIONS_SOFTWARE_EDITION);
    }

    signal(SIGINT, terminate);
    signal(SIGTERM, terminate);

    GPS_Init();

/*
    if (INDICATION_Init() < 0)
    {
        fprintf(stderr, "ERROR: Failed to initialize indication.\n");
        exit(EXIT_FAILURE);
    }
    if (REMOTE_Init(options.tcpPort) < 0)
    {
        fprintf(stderr, "ERROR: Failed to initialize remote.\n");
        exit(EXIT_FAILURE);
    }
    if (WHEEL_Init() < 0)
    {
        fprintf(stderr, "ERROR: Failed to initialize wheel.\n");
        exit(EXIT_FAILURE);
    }
*/
    SLEEP_Delay(0.1);

    DEBUG_Print(options.debug, debugMain, "# runnning.");

    while (run)
    {
        SLEEP_Delay(1.0);
    }

    //WHEEL_Close();
    //REMOTE_Close();
    //INDICATION_Close();
    GPS_Close();

    if (!options.debug)
    {
        DAEMONIZE_Close();
    }

    return EXIT_SUCCESS;
}

void terminate(int signalNumber)
{
    if (options.debug)
    {
        printf("\n");
    }
    DEBUG_Print(options.debug, debugMain, "Terminate.");
    run = 0;

    return;
}

