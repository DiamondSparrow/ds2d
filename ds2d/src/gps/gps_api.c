/*
 * gps.c
 *
 *  Created on: Jun 10, 2014
 *      Author: DiamondS
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <gps.h>

#include "gps_api.h"
#include "options.h"
#include "sleep.h"
#include "types.h"
#include "debug.h"


pthread_t GPS_Thread;
int GPS_Run = FALSE;

void *GPS_Control();

int GPS_Init(void)
{
    GPS_Run = TRUE;
    if (pthread_create(&GPS_Thread, NULL, GPS_Control, NULL))
    {
        fprintf(stderr, "ERROR: Failed to create gps control thread: %s.\n",
                strerror(errno));
        return -1;
    }

    return 0;
}

void GPS_Close(void)
{
    GPS_Run = FALSE;

    pthread_join(GPS_Thread, NULL);


    return;
}

void *GPS_Control(void)
{
    struct gps_data_t gpsData;

    if (gps_open("127.0.0.1", "2947", &gpsData) < 0)
    {
        fprintf(stderr,"ERROR: Could not connect to gpsd.\n");
        return NULL;
    }


    pthread_setname_np(GPS_Thread, "ds2d-gps");

    DEBUG_Print(options.debugGps, debugGps, "* started.");

    SLEEP_Delay(1.0);

    //register for updates
    gps_stream(&gpsData, WATCH_ENABLE | WATCH_JSON, NULL);

    while(GPS_Run)
    {
        //while (gpsData.status == 0)
        {
            //block for up to .5 seconds
            if (gps_waiting(&gpsData, 500))
            {
                //dunno when this would happen but its an error
                if (gps_read(&gpsData) == -1)
                {
                    fprintf(stderr, "ERROR: failed to read gpsd.\n");
                    break;
                }
                else
                {
                    //status>0 means you have data
                    if (gpsData.status > 0)
                    {
                        //sometimes if your GPS doesn't have a fix, it sends you data anyways
                        //the values for the fix are NaN. this is a clever way to check for NaN.
                        if (gpsData.fix.longitude != gpsData.fix.longitude
                                || gpsData.fix.altitude != gpsData.fix.altitude)
                        {
                            fprintf(stderr, "ERROR: Could not get a GPS fix.");
                            break;
                        }
                        //otherwise you have a legitimate fix!
                        else
                        {
                            DEBUG_Print(options.debugGps, debugGps, "%d, %d/%d, %f, lon %f, lat %f, alt %f",
                                    gpsData.status,
                                    gpsData.satellites_used,
                                    gpsData.satellites_visible,
                                    gpsData.online,
                                    gpsData.fix.longitude,
                                    gpsData.fix.latitude,
                                    gpsData.fix.altitude);
                        }
                    }
                    //if you don't have any data yet, keep waiting for it.
                    else
                    {
                        DEBUG_Print(options.debugGps, debugGps, "%d, %d/%d, %f, lon %f,  lat %f, alt %f",
                                gpsData.status,
                                gpsData.satellites_used,
                                gpsData.satellites_visible,
                                gpsData.online,
                                gpsData.fix.longitude,
                                gpsData.fix.longitude,
                                gpsData.fix.altitude);
                    }
                }
            }
            //apparently gps_stream disables itself after a few seconds.. in this case, gps_waiting returns false.
            //we want to re-register for updates and keep looping! we don't have a fix yet.
            else
            {
                gps_stream(&gpsData, WATCH_ENABLE | WATCH_JSON, NULL);
            }
            //just a sleep for good measure.
            SLEEP_Delay(0.5);
            if(!GPS_Run)
            {
                break;
            }
        }
    }

    //cleanup
    gps_stream(&gpsData, WATCH_DISABLE, NULL);
    gps_close(&gpsData);

    return NULL;
}
