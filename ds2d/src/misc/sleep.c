/*
 * sleep.c
 *
 *  Created on: Nov 28, 2013
 *      Author: DiamondS
 */

#include <errno.h>
#include <time.h>

#include "sleep.h"

int SLEEP_Delay(double sleepTime)
{
	int rval;
	struct timespec tv;
	/* Construct the timespec from the number of whole seconds...  */
	tv.tv_sec = (time_t) sleepTime;
	/* ... and the remainder in nanoseconds.  */
	tv.tv_nsec = (long) ((sleepTime - tv.tv_sec) * 1e+9);

	while (1)
	{
		/*
		 * Sleep for the time specified in tv.  If interrupted by a
		 * signal, place the remaining time left to sleep back into tv.
		 */
		rval = nanosleep(&tv, &tv);
		if (rval == 0)
		{
			/* Completed the entire sleep time; all done.  */
			return 0;
		}
		else if (errno == EINTR)
		{
			/* Interruped by a signal.  Try again.  */
			continue;
		}
		else
		{
			/* Some other error; bail out.  */
			return rval;
		}
	}
	return 0;
}
