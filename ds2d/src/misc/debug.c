/*
 * debug.c
 *
 *  Created on: Apr 27, 2014
 *      Author: DiamondS
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#include "debug.h"
#include "types.h"

pthread_mutex_t DEBUG_Mutex = PTHREAD_MUTEX_INITIALIZER;
#if DEBUG_USE_HEADER
const char DEBUG_TypeNames[debugNone+1][10] = { "main", "config", "gps", "indication", "remote", "wheel", "unknown" };
#endif

void DEBUG_Print(int debugFlag, debug_types_e debugType, void *str, ...)
{
	va_list args;
#if DEBUG_USE_HEADER
	struct timeval localTime;
	gettimeofday(&localTime, NULL);
#endif

	if(debugFlag == TRUE)
	{
		pthread_mutex_lock(&DEBUG_Mutex);
#if DEBUG_USE_HEADER
		printf("[%lld] ds2d-%-10.10s: ",
				(localTime.tv_sec*1000LL + localTime.tv_usec/1000),
				DEBUG_TypeNames[debugType]);
#endif
		va_start(args, str);
		vprintf(str, args);
		va_end(args);
		printf("\n");
		pthread_mutex_unlock(&DEBUG_Mutex);
	}

	return;
}

