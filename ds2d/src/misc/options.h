/*
 * options.h
 *
 *  Created on: Nov 27, 2013
 *      Author: DiamondS
 */

#ifndef OPTIONS_H_
#define OPTIONS_H_

#define OPTIONS_SOFTWARE_MAJOR			(0)
#define OPTIONS_SOFTWARE_MINOR			(1)
#define OPTIONS_SOFTWARE_MAINTENANCE	(1)
#define OPTIONS_SOFTWARE_BUILD			(1)

#define OPTIONS_SOFTWARE_EDITION		"Alpha Edition"
#define OPTIONS_SOFTWARE_NAME			"ds2d"

#define OPTIONS_DEFAULT_DEBUG			0
#define OPTIONS_DEFAULT_TCP_PORT		30003

typedef struct _options
{
	int debug;
    int debugGps;
	int debugIndication;
	int debugRemote;
	int debugTcpServer;
	int debugWheel;

	int tcpPort;
} options_t;

options_t options;

void OPTIONS_Init(options_t *options, int argc, char *argv[]);
void OPTIONS_PrintUsage(FILE *stream, int exitCode, char *argv[]);
void OPTIONS_PrintVersion(FILE *stream, int exitCode);

#endif /* OPTIONS_H_ */
