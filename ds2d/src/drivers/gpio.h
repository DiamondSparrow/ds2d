/*
 * gpio.h
 *
 *  Created on: Apr 27, 2014
 *      Author: DiamondS
 */

#ifndef GPIO_H_
#define GPIO_H_

#define GPIO_SYSFS_DIR		"/sys/class/gpio"
#define GPIO_POLL_TIMEOUT	(3 * 1000)	/* 3 seconds */
#define GPIO_BUFFER_SIZE	64

typedef enum _gpio_direction
{
	gpioInput,
	gpioOutput
} gpio_direction_e;

typedef enum _gpio_value
{
	gpioLow,
	gpioHigh
} gpio_value_e;

int GPIO_Export(unsigned int gpio);
int GPIO_Unexport(unsigned int gpio);
int GPIO_SetDirection(unsigned int gpio, gpio_direction_e direction);
int GPIO_SetValue(unsigned int gpio, gpio_value_e value);
int GPIO_SetEdge(unsigned int gpio, char *edge);
int GPIO_GetValue(unsigned int gpio, unsigned int *value);

#endif /* GPIO_H_ */
