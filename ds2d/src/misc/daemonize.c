/*
 * daemonize.c
 *
 *  Created on: Nov 27, 2013
 *      Author: DiamondS
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <errno.h>
#include <pwd.h>
#include <signal.h>
#include <assert.h>

#include "daemonize.h"

size_t DAEMONIZE_LogToWriter(void *cookie, char const *data, size_t leng);
int DAEMONIZE_LogToWriterNoop(void);
void DAEMONIZE_LogTo(FILE **pStream);
void DAEMONIZE_ChildHandler(int signalNumber);

static char const *priov[] = { "EMERG:", "ALERT:", "CRIT:", "ERR:", "WARNING:", "NOTICE:", "INFO:","DEBUG:" };
static cookie_io_functions_t DAEMONIZE_LogToWriterFNS =
{
		(void*) DAEMONIZE_LogToWriterNoop,
		(void *) DAEMONIZE_LogToWriter,
		(void*) DAEMONIZE_LogToWriterNoop,
		(void *) DAEMONIZE_LogToWriterNoop
};

void DAEMONIZE_Init(void)
{
	pid_t pid = 0;
	pid_t sid = 0;
	pid_t parent = 0;
	int lfp = -1;
	const char *lockfile = DAEMONIZE_LOCKFILE "" DAEMONIZE_NAME;
	/* Open loging.	*/
	openlog( DAEMONIZE_NAME, LOG_PID, LOG_LOCAL5);
	syslog( LOG_INFO, "starting");
	//DAEMONIZE_LogTo(&stderr);

	/* Already a daemon */
	if (getppid() == 1)
		return;

	/* Create the lock file as the current user */
	if (lockfile && lockfile[0])
	{
		lfp = open(lockfile, O_RDWR | O_CREAT, 0640);
		if (lfp < 0)
		{
			syslog( LOG_ERR, "unable to create lock file %s, code=%d (%s)",
					lockfile, errno, strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	/* Drop user if there is one, and we were run as root */
	if (getuid() == 0 || geteuid() == 0)
	{
		struct passwd *pw = getpwnam(DAEMONIZE_USER);
		if (pw)
		{
			syslog( LOG_NOTICE, "setting user to " DAEMONIZE_USER ".");
			setuid(pw->pw_uid);
		}
	}

	/* Trap signals that we expect to receive */
	signal(SIGCHLD, DAEMONIZE_ChildHandler);
	signal(SIGUSR1, DAEMONIZE_ChildHandler);
	signal(SIGALRM, DAEMONIZE_ChildHandler);

	/* Fork off the parent process */
	if ((pid = fork()) < 0)
	{
		syslog( LOG_ERR, "unable to fork daemon, code=%d (%s)",
				errno, strerror(errno));
		exit(EXIT_FAILURE);
	}
	/* If we got a good PID, then we can exit the parent process. */
	if (pid > 0)
	{
		/*
		 * Wait for confirmation from the child via SIGTERM or SIGCHLD, or
		 * for two seconds to elapse (SIGALRM).  pause() should not return.
		 */
		alarm(2);
		pause();

		exit(EXIT_FAILURE);
	}

	/* At this point we are executing as the child process */
	parent = getppid();

	/* Cancel certain signals */
	signal(SIGCHLD, SIG_DFL); /* A child process dies */
	signal(SIGTSTP, SIG_IGN); /* Various TTY signals */
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGHUP, SIG_IGN); /* Ignore hangup signal */
	signal(SIGTERM, SIG_DFL); /* Die on SIGTERM */

	/* Change the file mode mask */
	umask(0);

	/* Create a new SID for the child process */
	if ((sid = setsid()) < 0)
	{
		syslog( LOG_ERR, "unable to create a new session, code %d (%s)",
		errno, strerror(errno));
		exit(EXIT_FAILURE);
	}

	/*
	 * Change the current working directory.  This prevents the current
	 * directory from being locked; hence not being able to remove it.
	 */
	if ((chdir("/")) < 0)
	{
		syslog( LOG_ERR, "unable to change directory to %s, code %d (%s)", "/",
				errno, strerror(errno));
		exit(EXIT_FAILURE);
	}

	/* Redirect standard files to /dev/null */
	freopen("/dev/null", "r", stdin);
	freopen("/dev/null", "w", stdout);
	freopen("/dev/null", "w", stderr);

	/* Tell the parent process that we are A-okay */
	kill(parent, SIGUSR1);

	return;
}

void DAEMONIZE_Close(void)
{
	syslog( LOG_NOTICE, "closing.");
	closelog();

	return;
}

size_t DAEMONIZE_LogToWriter(void *cookie, char const *data, size_t leng)
{
	(void) cookie;
	int p = LOG_DEBUG, len;
	do
		len = strlen(priov[p]);
	while (memcmp(data, priov[p], len) && --p >= 0);

	if (p < 0)
		p = LOG_INFO;
	else
		data += len, leng -= len;
	while (*data == ' ')
		++data, --leng;

	syslog(p, "%.*s", leng, data);

	return leng;
}

int DAEMONIZE_LogToWriterNoop(void)
{
	return 0;
}

void DAEMONIZE_LogTo(FILE **pStream)
{
	setvbuf(*pStream = fopencookie(NULL, "w", DAEMONIZE_LogToWriterFNS), NULL, _IOLBF, 0);

	return;
}

void DAEMONIZE_ChildHandler(int signalNumber)
{
	switch (signalNumber)
	{
	case SIGALRM:
		exit(EXIT_FAILURE);
		break;
	case SIGUSR1:
		exit(EXIT_SUCCESS);
		break;
	case SIGCHLD:
		exit(EXIT_FAILURE);
		break;
	}

	return;
}
