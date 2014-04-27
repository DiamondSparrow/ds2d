/*
 * comport.c
 *
 *  Created on: Nov 27, 2013
 *      Author: DiamondS
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>

#include "comport.h"

void COMPORT_Init(comport_t *com, int debug, char *device, int baudrate)
{
	com->debug = debug;
	com->baudrate = baudrate;
	memset(com->device, 0, sizeof(com->device));
	strncpy(com->device, device, strlen(device));

	return;
}

int COMPORT_Connect(comport_t *com)
{
	struct termios newtio = {0};
	speed_t speed = {0};

	if (com->debug)
	{
		fprintf(stdout, "Opening %s at %d bps (8N1).\n",
				com->device,
				com->baudrate);
	}
	com->stream = open(com->device, O_RDWR | O_NOCTTY);
	if (com->stream < 0)
	{
		fprintf(stderr, "ERROR: Failed to open %s. %s.\n",
				com->device,
				strerror(errno));
		return -1;
	}

	/* Save */
	tcgetattr(com->stream, &(com->oldTermios));

	memset(&newtio, 0, sizeof(struct termios));

	newtio.c_cflag &= ~CRTSCTS; /* disable hardware flow control */
	newtio.c_cflag &= ~PARENB; /* no parity */
	newtio.c_cflag &= ~CSTOPB; /* send one stop bit */
	newtio.c_cflag &= ~CSIZE; /* character size mask */
	newtio.c_cflag &= ~HUPCL; /* disable hang up */
	newtio.c_cflag |= CS8 | CLOCAL | CREAD; /* 8 bits per byte | ignore modem control lines | enable receiver */
	newtio.c_iflag &= ~(IXON | IXOFF); /* disable XON/XOFF flow control on output & input */
	newtio.c_iflag |= IGNBRK | IGNPAR; /* ignore BREAK condition on input & framing errors & parity errors */
	newtio.c_oflag &= ~OPOST; /* disable output processing */
	newtio.c_lflag = 0; /* echo off, echo newline off, canonical mode off */
	newtio.c_cc[VMIN] = 0; /* disable timeout */
	newtio.c_cc[VTIME] = 10; /* wait for data, deciseconds */

	/*
	 * C_ISPEED		Input baud (new interface)
	 * C_OSPEED		Output baud (new interface)
	 */
	switch (com->baudrate)
	{
	case 110:
		speed = B110;
		break;
	case 300:
		speed = B300;
		break;
	case 600:
		speed = B600;
		break;
	case 1200:
		speed = B1200;
		break;
	case 2400:
		speed = B2400;
		break;
	case 4800:
		speed = B4800;
		break;
	case 9600:
		speed = B9600;
		break;
	case 19200:
		speed = B19200;
		break;
	case 38400:
		speed = B38400;
		break;
	case 57600:
		speed = B57600;
		break;
	case 115200:
		speed = B115200;
		break;
	default:
		speed = B115200;
		fprintf(stderr, "WARNING: Unknown baudrate, using default 115200.\n");
		break;
	}
	/* Set the baud rate */
	if ((cfsetispeed(&newtio, speed) < 0) || (cfsetospeed(&newtio, speed) < 0))
	{
		fprintf(stderr,
				"ERROR: Failed to set speed (cfsetispeed/cfsetospeed). %s.\n",
				strerror(errno));
		return -2;
	}

	errno = 0;
	if (tcflush(com->stream, TCIFLUSH))
	{
		fprintf(stderr, "WARNING: Failed to flush serial device. %s.\n",
				strerror(errno));
	}
	if (tcsetattr(com->stream, TCSANOW, &newtio))
	{
		fprintf(stderr,
				"ERROR: Failed to configure serial device, closing. %s.\n",
				strerror(errno));
		if (close(com->stream))
		{
			fprintf(stderr, "ERROR: Failed to close serial device. %s.\n",
					strerror(errno));
		}
		return -3;
	}

	errno = 0;
	if (tcflush(com->stream, TCIOFLUSH))
	{
		fprintf(stderr, "WARNING: Failed to flush serial device. %s.\n",
				strerror(errno));
	}

	return 1;
}

int COMPORT_Send(comport_t *com, unsigned char *data, int dataCount)
{
	int i = 0, ret = 0;

	if (com->debug)
	{
		printf("TX Data: ");
		for (i = 0; i < dataCount; i++)
		{
			if (i > 0)
			{
				printf(",");
			}
			printf("0x%02X", data[i]);
		}
		printf(" (%d B.)\n", dataCount);
	}

	if ((ret = write(com->stream, data, dataCount) != dataCount))
	{
		fprintf(stderr, "ERROR: Failed to write to serial device. %s.\n",
				strerror(errno));
		ret = -1;
	}
	return ret;
}

int COMPORT_Receive(comport_t *com, unsigned char *data, int dataCount)
{
	int i = 0, ret = 0;

	ret = read(com->stream, data, dataCount);

	if (com->debug)
	{
		printf("RX Data: ");
		for (i = 0; i < ret; i++)
		{
			if (i > 0)
			{
				printf(",");
			}
			printf("0x%02X", data[i]);
		}
		printf(" (%d B.)\n", ret);
	}

	return ret;
}

int COMPORT_Disconnect(comport_t *com)
{
	if (com->debug)
	{
		fprintf(stdout, "Closing %s at %d bps (8N1).\n", com->device,
				com->baudrate);
	}

	if (tcsetattr(com->stream, TCSANOW, &com->oldTermios))
	{
		fprintf(stderr, "ERROR: Failed to restore serial device. %s.\n",
				strerror(errno));
		if (close(com->stream))
		{
			fprintf(stderr, "ERROR: Failed to close serial device. %s.\n",
					strerror(errno));
		}
		return -1;
	}

	errno = 0;
	if (tcflush(com->stream, TCIOFLUSH))
	{
		fprintf(stderr, "WARNING: Failed to flush serial device. %s.\n",
				strerror(errno));
		if (close(com->stream))
		{
			fprintf(stderr, "ERROR: Failed to close serial device. %s.\n",
					strerror(errno));
		}
		return -2;
	}

	if (close(com->stream))
	{
		fprintf(stderr, "ERROR: Failed to close serial device. %s.\n",
				strerror(errno));
		return -3;
	}

	return 1;
}

