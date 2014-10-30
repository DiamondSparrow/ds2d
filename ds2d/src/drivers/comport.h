/*
 * comport.h
 *
 *  Created on: Nov 27, 2013
 *      Author: DiamondS
 */

#ifndef COMPORT_H_
#define COMPORT_H_

#include <termios.h>

#define COMPORT_DEFAULT_DEBUG		false
#define COMPORT_DEFAULT_BAUDRATE	115200
#define COMPORT_DEFAULT_DEVICE		"/dev/ttyACM0"
#define COMPORT_DEFAULT_TIMEOUT		500000

typedef struct _comport
{
	int debug;
	int stream;
	char device[32];
	int baudrate;
	struct termios oldTermios;
} comport_t;

void COMPORT_Init(comport_t *com, int debug, const char *device, int baudrate);
int COMPORT_Connect(comport_t *com);
int COMPORT_Send(comport_t *com, unsigned char *data, int dataCount);
int COMPORT_Receive(comport_t *com, unsigned char *data, int dataCount);
int COMPORT_Disconnect(comport_t *com);

#endif /* COMPORT_H_ */
