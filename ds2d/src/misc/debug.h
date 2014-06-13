/*
 * debug.h
 *
 *  Created on: Apr 27, 2014
 *      Author: DiamondS
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#define DEBUG_USE_HEADER	1

typedef enum _debug_types
{
	debugMain,
	debugGps,
	debugIndication,
	debugRemote,
	debugWheel,
	debugNone
} debug_types_e;

void DEBUG_Print(int debugFlag, debug_types_e debugType, void *str, ...);

#endif /* DEBUG_H_ */
