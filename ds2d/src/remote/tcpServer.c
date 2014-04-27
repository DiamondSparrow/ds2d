/*
 * tcpServer.c
 *
 *  Created on: Nov 27, 2013
 *      Author: DiamondS
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "tcpServer.h"

int TCPSERVER_Init ( tcpserver_t *tcpserver, int port, int debug )
{
	int				trues = 1;
	struct timeval	tvSend,
					tvRecv;

	tcpserver->port		= port;
	tcpserver->debug	= debug;
	tcpserver->status	= 1;

	tvSend.tv_sec	= TCPSERVER_SEND_SEC;
	tvSend.tv_usec	= TCPSERVER_SEND_USEC;
	tvRecv.tv_sec	= TCPSERVER_RECV_SEC;
	tvRecv.tv_usec	= TCPSERVER_RECV_USEC;

	// PF_INET - Internet namespaces, PF_LOCAL- Local namespaces
	if ((tcpserver->sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		fprintf(stderr, "ERROR: Failed to create socket: %s.\n", strerror(errno));
		tcpserver->status = TCPSERVER_ERROR_SOCKET;
	}
	else
	{
		if (setsockopt(tcpserver->sock, SOL_SOCKET, SO_REUSEADDR, &trues, sizeof(int)) == -1)
		{
			fprintf(stderr, "ERROR: Failed to set socket option SOL_SOCKET: %s.\n", strerror(errno));
			tcpserver->status = TCPSERVER_ERROR_SETSOCKOPT;
		}
		if (setsockopt(tcpserver->sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&tvRecv, sizeof(struct timeval)) == -1)
		{
			fprintf(stderr, "ERROR: Failed to set socket option SO_RCVTIMEO: %s.\n", strerror(errno));
			tcpserver->status = TCPSERVER_ERROR_SETSOCKOPT;
		}
		if (setsockopt(tcpserver->sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&tvSend, sizeof(struct timeval)) == -1)
		{
			fprintf(stderr, "ERROR: Failed to set socket option SO_SNDTIMEO: %s.\n", strerror(errno));
			tcpserver->status = TCPSERVER_ERROR_SETSOCKOPT;
		}
	}

	return tcpserver->status;
}


int TCPSERVER_Connect ( tcpserver_t *tcpserver )
{
	if(tcpserver->status == 1)
	{
		tcpserver->serverAddr.sin_family 		= AF_INET;	// AF_INET - Internet namespaces, AF_LOCAL - Local namespaces
		tcpserver->serverAddr.sin_port 			= htons(tcpserver->port);
		tcpserver->serverAddr.sin_addr.s_addr	= htonl(INADDR_ANY);

		if (bind(tcpserver->sock, (struct sockaddr *)&tcpserver->serverAddr, sizeof(struct sockaddr)) == -1)
		{
			fprintf(stderr, "ERROR: Failed to bind: %s.\n", strerror(errno));
			tcpserver->status = TCPSERVER_ERROR_BIND;
		}
		else
		{
			if (listen(tcpserver->sock, TCPSERVER_CLIENT_COUNT) == -1)
			{
				fprintf(stderr, "ERROR: Failed to listen: %s.\n", strerror(errno));
				tcpserver->status = TCPSERVER_ERROR_LISTEN;
			}
		}

		fflush(stdout);
	}

	return tcpserver->status;
}


int TCPSERVER_NewConnection ( tcpserver_t *tcpserver )
{
	socklen_t	sin_size;
	int sockNew = 0;

	sin_size = sizeof(struct sockaddr_in);
	sockNew = accept(tcpserver->sock, (struct sockaddr *)&tcpserver->clientAddr, &sin_size);

	return sockNew;
}

int TCPSERVER_Disconnect ( tcpserver_t *tcpserver )
{
	close(tcpserver->sock);
	tcpserver->status = 0;

	return tcpserver->status;
}

int TCPSERVER_Close ( int socket )
{
	close(socket);

	return 1;
}

int TCPSERVER_Receive ( tcpserver_t *tcpserver )
{
	socklen_t addrLength;

	memset(tcpserver->dataBuffer, 0, TCPSERVER_BUFFER_SIZE);
	addrLength = sizeof tcpserver->clientAddr;
	if ((tcpserver->dataBufferLength = recvfrom(tcpserver->sockNew, tcpserver->dataBuffer, TCPSERVER_BUFFER_SIZE-1 , 0, (struct sockaddr *)&tcpserver->clientAddr, &addrLength)) == -1)
	{
		if(errno != EAGAIN)
		{
			fprintf(stderr, "ERROR: Failed to recvfrom: %s.\n", strerror(errno));
		}
		return 0;
	}

	tcpserver->dataBuffer[tcpserver->dataBufferLength] = '\0';

	return tcpserver->dataBufferLength;
}

int TCPSERVER_Send ( tcpserver_t *tcpserver, unsigned char *buffer, unsigned int bufferLength )
{
	if( sendto(tcpserver->sockNew, buffer, bufferLength, 0, (struct sockaddr *)&tcpserver->clientAddr, sizeof(tcpserver->clientAddr)) == -1)
	{
		fprintf(stderr, "ERROR: Failed to sendto: %s.\n", strerror(errno));
		return 0;
	}

	return 1;
}
