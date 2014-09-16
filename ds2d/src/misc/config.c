/*
 * config.c
 *
 *  Created on: Sep 6, 2014
 *      Author: DiamondS
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "ini.h"
#include "debug.h"
#include "types.h"

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

static int CONFIG_Handler(void *config, const char *section, const char *name, const char *value);

config_t configuration;

int CONFIG_Init(char *configFile, int debug)
{
    if (ini_parse(configFile, CONFIG_Handler, &configuration) < 0)
    {
        fprintf(stderr, "ERROR: Can't load '%s'\n", configFile);
        return -1;
    }

    DEBUG_Print(TRUE, debugConfig,"Config loaded from '%s'.\n",
            configFile);
    if(debug)
    {
        CONFIG_Debug();
    }

    return 0;
}

void CONFIG_Debug(void)
{
    DEBUG_Print(TRUE, debugConfig, "Remote - port = %d;", configuration.remote.tcpPort);
    DEBUG_Print(TRUE, debugConfig, "Wheels - device = %s, speed = %d;", configuration.wheels.device, configuration.wheels.speed);

    return;
}

static int CONFIG_Handler(void *config, const char *section, const char *name, const char *value)
{
    config_t *pConfig = (config_t *)config;

    if (MATCH("remote", "port"))
    {
        pConfig->remote.tcpPort = atoi(value);
    }
    else if (MATCH("wheels", "device"))
    {
        pConfig->wheels.device = strdup(value);
    }
    else if (MATCH("wheels", "speed"))
    {
        pConfig->wheels.speed = atoi(value);
    }
    else
    {
        return 0;  /* unknown section/name, error */
    }

    return 1;
}

