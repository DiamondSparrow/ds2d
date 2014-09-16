/*
 * remote.c
 *
 *  Created on: Nov 27, 2013
 *      Author: DiamondS
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#include "remote.h"
#include "tcpServer.h"
#include "options.h"
#include "sleep.h"
#include "types.h"
#include "debug.h"

pthread_t REMOTE_Thread;
tcpserver_t REMOTE_ServerTCP;
int REMOTE_ServerRun = FALSE;
int REMOTE_ClientRun = FALSE;

void *REMOTE_Server();
void *REMOTE_Client();

int REMOTE_Init(int port)
{

    if (TCPSERVER_Init(&REMOTE_ServerTCP, port, options.debugTcpServer) != 1)
    {
        fprintf(stderr, "ERROR: Failed to initialize TCP Server.\n");
        return -1;
    }

    if (TCPSERVER_Connect(&REMOTE_ServerTCP) != 1)
    {
        fprintf(stderr, "ERROR: Failed to connect in TCP Server (%d).",
                REMOTE_ServerTCP.port);
        return -1;
    }

    REMOTE_ServerRun = TRUE;
    if (pthread_create(&REMOTE_Thread, NULL, REMOTE_Server, NULL))
    {
        fprintf(stderr, "ERROR: Failed to create remote control thread: %s.\n",
                strerror(errno));
        return -1;
    }

    return 1;
}

int REMOTE_Close(void)
{
    REMOTE_ServerRun = FALSE;

    pthread_join(REMOTE_Thread, NULL);
    TCPSERVER_Disconnect(&REMOTE_ServerTCP);

    return 1;
}

void *REMOTE_Server()
{
    int newSock = 0;
    pthread_t threadClient;

    pthread_setname_np(REMOTE_Thread, "ds2d-remote");

    DEBUG_Print(options.debugRemote, debugRemote, "* started.");

    while (REMOTE_ServerRun)
    {
        if ((newSock = TCPSERVER_NewConnection(&REMOTE_ServerTCP)) != -1)
        {
            if (REMOTE_ClientRun == TRUE)
            {
                REMOTE_ClientRun = FALSE;
                SLEEP_Delay(0.01);
                pthread_tryjoin_np(threadClient, NULL);
            }
            DEBUG_Print(options.debugRemote, debugRemote,
                    "- New Connection.\n");
            pthread_cancel(threadClient);
            SLEEP_Delay(0.001);

            REMOTE_ServerTCP.sockNew = newSock;
            REMOTE_ClientRun = TRUE;
            if (pthread_create(&threadClient, NULL, REMOTE_Client, NULL))
            {
                fprintf(stderr,
                        "ERROR: Failed to create remote control thread: %s.\n",
                        strerror(errno));
                TCPSERVER_Close(newSock);
                REMOTE_ServerRun = FALSE;
                break;
            }
            pthread_setname_np(threadClient, "x ds2d-client");
        }
        SLEEP_Delay(0.1);
    }

    if (REMOTE_ClientRun == TRUE)
    {
        REMOTE_ClientRun = FALSE;
        pthread_join(threadClient, NULL);
    }

    DEBUG_Print(options.debugRemote, debugRemote, "stopped.");

    return (NULL);
}

void *REMOTE_Client()
{
    int ret = 0;

    while (REMOTE_ClientRun)
    {
        if ((ret = TCPSERVER_Receive(&REMOTE_ServerTCP)) > 0)
        {
            DEBUG_Print(options.debugRemote, debugRemote, "- RX: %s (%d B.).\n",
                    REMOTE_ServerTCP.dataBuffer,
                    REMOTE_ServerTCP.dataBufferLength);
            if ((ret = TCPSERVER_Send(&REMOTE_ServerTCP,
                    REMOTE_ServerTCP.dataBuffer,
                    REMOTE_ServerTCP.dataBufferLength)) < 0)
            {
                break;
            }
        }

        SLEEP_Delay(0.001);
    }

    DEBUG_Print(options.debugRemote, debugRemote, "- New Connection closed.\n");
    TCPSERVER_Close((int) REMOTE_ServerTCP.sockNew);

    return (NULL);
}

