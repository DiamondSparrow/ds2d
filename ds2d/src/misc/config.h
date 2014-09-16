/*
 * config.h
 *
 *  Created on: Sep 6, 2014
 *      Author: DiamondS
 */

#ifndef CONFIG_H_
#define CONFIG_H_

typedef struct _config_remote
{
    unsigned int tcpPort;
} config_remote_t;

typedef struct _config_wheels
{
    const char* device;
    unsigned int speed;
} config_wheels_t;

typedef struct _config
{
    config_remote_t remote;
    config_wheels_t wheels;

} config_t;

extern config_t configuration;

int CONFIG_Init(char *configFile, int debug);
void CONFIG_Debug(void);

#endif /* CONFIG_H_ */
