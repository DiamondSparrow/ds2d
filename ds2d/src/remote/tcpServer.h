/*
 * tcpServer.h
 *
 *  Created on: Nov 27, 2013
 *      Author: root
 */

#ifndef TCPSERVER_H_
#define TCPSERVER_H_


#define TCPSERVER_ERROR_SOCKET			-1
#define TCPSERVER_ERROR_SETSOCKOPT		-2
#define TCPSERVER_ERROR_BIND			-3
#define TCPSERVER_ERROR_LISTEN			-4
#define TCPSERVER_ERROR_CLIENT_NEW		-5

#define TCPSERVER_SEND_SEC			5
#define TCPSERVER_SEND_USEC			0
#define TCPSERVER_RECV_SEC			1
#define TCPSERVER_RECV_USEC			0

#define TCPSERVER_CLIENT_COUNT		1

#define TCPSERVER_BUFFER_SIZE		128

#include <arpa/inet.h>

typedef struct _tcpserver
{
	int	status;
	int port;
	int sock;
	int sockNew;
	int debug;
	struct 	sockaddr_in	serverAddr,
						clientAddr;
	unsigned char dataBuffer[TCPSERVER_BUFFER_SIZE];
	int dataBufferLength;
} tcpserver_t;

int TCPSERVER_Init ( tcpserver_t *tcpserver, int port, int debug );
int TCPSERVER_Connect ( tcpserver_t *tcpserver );
int TCPSERVER_NewConnection ( tcpserver_t *tcpserver );

int TCPSERVER_Disconnect ( tcpserver_t *tcpserver );
int TCPSERVER_Close ( int socket );

int TCPSERVER_Receive ( tcpserver_t *tcpserver );
int TCPSERVER_Send ( tcpserver_t *tcpserver, unsigned char *buffer, unsigned int bufferLength );


#endif /* TCPSERVER_H_ */
