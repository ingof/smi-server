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

int main(void) {

		/* Our process ID and Session ID */
		pid_t pid, sid;
		syslog(LOG_SYSLOG, "\nSMI server daemon:\n\n");
		/* Fork off the parent process */
		pid = fork();
		if (pid < 0) {
				exit(EXIT_FAILURE);
		}
		syslog(LOG_SYSLOG, "forked.\n");
		/* If we got a good PID, then
		   we can exit the parent process. */
		if (pid > 0) {
				exit(EXIT_SUCCESS);
		}
		syslog(LOG_SYSLOG, "PID checked.\n");

		/* Change the file mode mask */
		umask(0);
		syslog(LOG_SYSLOG, "umasked.\n");

		/* Open any logs here */

		/* Create a new SID for the child process */
		sid = setsid();
		if (sid < 0) {
				/* Log the failure */
				exit(EXIT_FAILURE);
		}
		syslog(LOG_SYSLOG, "SID created.\n");



		/* Change the current working directory */
		if ((chdir("/")) < 0) {
				/* Log the failure */
				exit(EXIT_FAILURE);
		}
		syslog(LOG_SYSLOG, "Dir changed.\n");

		/* Close out the standard file descriptors */
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);

		/* Daemon-specific initialization goes here */

		/* The Big Loop */
		syslog(LOG_SYSLOG, "loop.\n");
		while (1) {
		   /* Do some task here ... */
		   sleep(5);
		   syslog(LOG_SYSLOG, "5 sconds.\n");
		   sleep(5);
		   syslog(LOG_SYSLOG, "10 sconds.\n");
		   sleep(5);
		   syslog(LOG_SYSLOG, "15 sconds.\n");
		   sleep(5);
		   syslog(LOG_SYSLOG, "20 sconds.\n");
		   sleep(5);
		   syslog(LOG_SYSLOG, "25 sconds.\n");
		   sleep(5);
		   syslog(LOG_SYSLOG, "30 sconds.\n");
		   sleep(1);
		   syslog(LOG_SYSLOG, "exiting.\n");
		}
   exit(EXIT_SUCCESS);
}
