/*
 * options.c
 *
 *  Created on: Nov 27, 2013
 *      Author: DiamondS
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "options.h"
#include "types.h"

void OPTIONS_Init(options_t *options, int argc, char *argv[])
{
	int nextOption;
	const char *shortOptions = "hVdirtwp:";
	const struct option longOptions[] =
	{
			{ "port", required_argument, NULL, 'p' },
			{ "debug", no_argument, NULL, 'd' },
			{ "indication", no_argument, NULL, 'i' },
			{ "remote", no_argument, NULL, 'r' },
			{ "tcp", no_argument, NULL, 't' },
			{ "wheel", no_argument, NULL, 'w' },
			{ "Version", no_argument, NULL, 'V' },
			{ "help", no_argument, NULL, 'h' },
			{ NULL, no_argument, NULL, 0 }
	};

	options->debug = FALSE;
	options->debugIndication = FALSE;
	options->debugRemote = FALSE;
	options->debugTcpServer = FALSE;
	options->debugWheel = FALSE;

	options->tcpPort = OPTIONS_DEFAULT_TCP_PORT;

	do
	{
		nextOption = getopt_long(argc, argv, shortOptions, longOptions, NULL);
		switch (nextOption)
		{
		case 'p':
			if (optarg > 0)
			{
				options->tcpPort = atoi(optarg);
			}
			break;

		case 'd':
			options->debug = TRUE;
			break;
		case 'i':
			options->debugIndication = TRUE;
			options->debug = TRUE;
			break;
		case 'r':
			options->debugRemote= TRUE;
			options->debug = TRUE;
			break;
		case 't':
			options->debugTcpServer = TRUE;
			options->debug = TRUE;
			break;
		case 'w':
			options->debugWheel = TRUE;
			options->debug = TRUE;
			break;

		case 'V':
			OPTIONS_PrintVersion(stdout, 0);
			break;

		case 'h':
			OPTIONS_PrintUsage(stdout, 0, argv);
			break;

		case '?':
			OPTIONS_PrintUsage(stderr, 1, argv);
			break;

		case -1:
			break;

		default:
			abort();
			break;
		}
	} while (nextOption != -1);

	return;
}

void OPTIONS_PrintUsage(FILE *stream, int exitCode, char *argv[])
{
	fprintf(stream, "\nUsage:\n %s options\n", argv[0]);

	fprintf(stream, "Options:\n"
			"  -p, --port	<port>	Socket Server Port (default: %d).\n", OPTIONS_DEFAULT_TCP_PORT);
	fprintf(stream, "\n"
			"Debug:\n"
			"  -d, --debug			Debug mode;\n"
			"  -i, --indication		Debug Indication.\n"
			"  -r, --remote			Debug Remote.\n"
			"  -t, --tcp			Debug TCP Server.\n");
	fprintf(stream, "\n"
			"  -V, --version		Version;\n"
			"  -h, --help			Display this usage information.\n"
			"\n");

	exit(exitCode);
}

void OPTIONS_PrintVersion(FILE *stream, int exitCode)
{
	fprintf(stream, "Version: %s v%d.%d.%d.%d %s\n",
			OPTIONS_SOFTWARE_NAME,
			OPTIONS_SOFTWARE_MAJOR,
			OPTIONS_SOFTWARE_MINOR,
			OPTIONS_SOFTWARE_MAINTENANCE,
			OPTIONS_SOFTWARE_BUILD,
			OPTIONS_SOFTWARE_EDITION);

	exit(exitCode);
}

