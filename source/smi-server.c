/* smi-server */
//#include <stdio.h>		/* Standard input/output definitions */
//#include <string.h>		/* String function definitions */
//#include <unistd.h>		/* UNIX standard function definitions */
//#include <fcntl.h>		/* File control definitions */
//#include <errno.h>		/* Error number definitions */
//#include <termios.h>		/* POSIX terminal control definitions */
//#include <stdlib.h>		/* converting functions */
//#include <sys/ioctl.h>	/* ioctl() */
//#include <sys/types.h>	/* ?? */
//#include <sys/stat.h>		/* ?? */
//#include <sys/time.h>		/* ?? */
//#include <linux/serial.h>	/* custom divisor */
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>

#include "typesdef.h"			/* type definitions */
#include "swb-serial.h"		/* swb-bus functions */
#include "smi-serial.h"		/* swb-bus functions */

#include "smi-server.h"		/* own funcions */


#include<netinet/in.h>		/* web server */
#include<sys/socket.h>		/* web server */
#include<sys/stat.h>		/* web server */
#include<sys/types.h>		/* web server */
#include<unistd.h>			/* web server */
#include <syslog.h>			/* syslog */
#include <stdio.h>			/* syslog */

int main(int argc, char *argv[]) {

	/* my process ID and Session ID */
	pid_t pid, sid;

	openlog(argv[0], LOG_PID | LOG_NDELAY, LOG_LOCAL0);
	syslog(LOG_INFO, "INFO: SMI server starting");
	syslog(LOG_INFO, argv[0]);
	syslog(LOG_INFO, argv[1]);

	/* fork of the parent process */
	pid = fork();
	if (pid < 0) {
		exit(EXIT_FAILURE);
		syslog(LOG_ERR,  "ERROR: fork failed");
	}
	/* exit parent if got good PID */
	if (pid > 0) {
		exit(EXIT_SUCCESS);
		syslog(LOG_ERR,  "ERROR: getPID failed");
	}

	umask(0);

	/* Open any logs here */

	/* create new SID for the child process */
	sid = setsid();
	if (sid < 0) {
		/* Log the failure */
		syslog(LOG_ERR,  "ERROR: creation of new SID failed");
		exit(EXIT_FAILURE);
	}

	/* change current working directory */
	if ((chdir("/")) < 0) {
		/* Log the failure */
		syslog(LOG_ERR,  "ERROR: changing current working directory failed");
		exit(EXIT_FAILURE);
	}

	/* close the standard file descriptors */
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	syslog(LOG_INFO, "INFO: server damonized and start working");

	syslog(LOG_INFO, argv[0]);
	syslog(LOG_INFO, argv[1]);

	/* Daemon-specific initialization goes here */


	/* Do some task here ... */
	// // logging to 'regular' facility LOCAL0
	// sleep(3);
	// syslog(LOG_INFO, "log at info to local0");
	// sleep(3);
	// syslog(LOG_WARNING, "log at warn to local0");
	// sleep(3);
	// syslog(LOG_ERR,  "log at error to local0");
	//
	// // logging to 'special' facility LOCAL3
	// sleep(3);
	// syslog(LOG_INFO|LOG_LOCAL3, "special log message to local3");

	// close "regular" log
	closelog();
	syslog(LOG_INFO, "INFO: SMI-server closed");
	exit(EXIT_SUCCESS);
}
