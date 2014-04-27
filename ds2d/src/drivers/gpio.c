/*
 * gpio.c
 *
 *  Created on: Apr 27, 2014
 *      Author: DiamondS
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "gpio.h"

int GPIO_Export(unsigned int gpio)
{
	int fd = 0;
	int length = 0;
	char buffer[GPIO_BUFFER_SIZE] = {0};
	struct stat s = {0};

	snprintf(buffer, sizeof(buffer), GPIO_SYSFS_DIR  "/gpio%d", gpio);

	if (stat(buffer, &s) < 0)
	{
		if ((fd = open(GPIO_SYSFS_DIR "/export", O_WRONLY)) < 0)
		{
			fprintf(stderr, "ERROR: Failed to open %s/export. %s.",
					GPIO_SYSFS_DIR,
					strerror(errno));
			return -1;
		}

		length = snprintf(buffer, sizeof(buffer), "%d", gpio);
		write(fd, buffer, length);
		close(fd);
	}

	return 0;
}

int GPIO_Unexport(unsigned int gpio)
{
	int fd = 0;
	int length = 0;
	char buffer[GPIO_BUFFER_SIZE] = {0};
	struct stat s = {0};

	snprintf(buffer, sizeof(buffer), GPIO_SYSFS_DIR  "/gpio%d", gpio);

	if (stat(buffer, &s) == 0)
	{
		if ((fd = open(GPIO_SYSFS_DIR "/unexport", O_WRONLY)) < 0)
		{
			fprintf(stderr, "ERROR: Failed to open %s/unexport. %s.",
					GPIO_SYSFS_DIR,
					strerror(errno));
			return -1;
		}

		length = snprintf(buffer, sizeof(buffer), "%d", gpio);
		write(fd, buffer, length);
		close(fd);
	}

	return 0;
}

int GPIO_SetDirection(unsigned int gpio, gpio_direction_e direction)
{
	int fd = 0;
	char buffer[GPIO_BUFFER_SIZE] = {0};

	snprintf(buffer, sizeof(buffer), GPIO_SYSFS_DIR  "/gpio%d/direction", gpio);

	if ((fd = open(buffer, O_WRONLY)) < 0)
	{
		fprintf(stderr, "ERROR: Failed to open %s. %s.",
				buffer,
				strerror(errno));
		return -1;
	}

	switch(direction)
	{
	case gpioOutput:
		write(fd, "out", 4);
		break;
	case gpioInput:
		write(fd, "in", 3);
		break;
	default:
		break;
	}

	close(fd);

	return 0;
}

int GPIO_SetValue(unsigned int gpio, gpio_value_e value)
{
	int fd = 0;
	char buffer[GPIO_BUFFER_SIZE] = {0};

	snprintf(buffer, sizeof(buffer), GPIO_SYSFS_DIR "/gpio%d/value", gpio);

	if ((fd = open(buffer, O_WRONLY)) < 0)
	{
		fprintf(stderr, "ERROR: Failed to open %s. %s.",
				buffer,
				strerror(errno));
		return -1;
	}

	switch(value)
	{
	case gpioLow:
		write(fd, "0", 2);
		break;
	case gpioHigh:
		write(fd, "1", 2);
		break;
	default:
		break;
	}

	close(fd);

	return 0;
}

int GPIO_SetEdge(unsigned int gpio, char *edge)
{
	int fd = 0;
	char buffer[GPIO_BUFFER_SIZE] = {0};

	snprintf(buffer, sizeof(buffer), GPIO_SYSFS_DIR "/gpio%d/edge", gpio);

	if ((fd = open(buffer, O_WRONLY)) < 0)
	{
		fprintf(stderr, "ERROR: Failed to open %s. %s.",
				buffer,
				strerror(errno));
		return -1;
	}

	write(fd, edge, strlen(edge) + 1);
	close(fd);

	return 0;
}

int GPIO_GetValue(unsigned int gpio, unsigned int *value)
{
	int fd = 0;
	char buffer[GPIO_BUFFER_SIZE] = {0};
	char val = 0;

	snprintf(buffer, sizeof(buffer), GPIO_SYSFS_DIR "/gpio%d/value", gpio);

	if ((fd = open(buffer, O_RDONLY)) < 0)
	{
		fprintf(stderr, "ERROR: Failed to open %s. %s.",
				buffer,
				strerror(errno));
		return -1;
	}

	read(fd, &val, 1);

	if (val != '0')
	{
		*value = 1;
	}
	else
	{
		*value = 0;
	}

	close(fd);

	return 0;
}


