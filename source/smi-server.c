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

#include "typesdef.h"		/* type definitions */
#include "webserver.h"
#include "parseconf.h"		/* config parser */
#include "swb-serial.h"		/* swb-bus functions */
#include "smi-serial.h"		/* swb-bus functions */
#include "smi-server.h"		/* own funcions */
#include "ownfunctions.h"
#include "ownfunctions.c"
#include "parseconf.c"
#include "webserver.c"

#include <netinet/in.h>		/* web server */
#include <sys/socket.h>		/* web server */
#include <sys/stat.h>		/* web server */
#include <sys/types.h>		/* web server */
#include <unistd.h>			/* web server */
#include <syslog.h>			/* syslog */
#include <stdio.h>			/* syslog */
// #include <unistd.h>			/* getpwd() */

DRIVE drive[16];
BUTTON button[32];
int tcpControl=8088;
char serialSmi1[40];
char serialSmi2[40];
char serialSmi3[40];
char serialSwb[40];

int newSocket;
socklen_t clientAddrLen;
int bufSize = 1024;
unsigned char *bufferHTTP = malloc(bufSize);
struct sockaddr_in clientAddress;
int tmpListen;




int main(int argc, char *argv[]) {
	int mySocket;

/* damonize */
{
	/* my process ID and Session ID */
	pid_t pid, sid;

	openlog(argv[0], LOG_PID | LOG_NDELAY, LOG_LOCAL7);
	syslog(LOG_DEBUG, "DEBUG: SMI server starting (%s)", argv[1]);

	/* fork of the parent process */
	pid = fork();
	if (pid < 0) {
		exit(EXIT_FAILURE);
		syslog(LOG_ERR,  "ERROR: fork failed [%m]");
	}
	/* exit parent if got good PID */
	if (pid > 0) {
		exit(EXIT_SUCCESS);
		syslog(LOG_ERR,  "ERROR: getPID failed [%m]");
	}

	umask(0);

	/* Open any logs here */

	/* create new SID for the child process */
	sid = setsid();
	if (sid < 0) {
		/* Log the failure */
		syslog(LOG_ERR,  "ERROR: creation of new SID failed [%m]");
		exit(EXIT_FAILURE);
	}

	/* change current working directory */
	if ((chdir("/")) < 0) {
		/* Log the failure */
		syslog(LOG_ERR,  "ERROR: changing current working directory failed [%m]");
		exit(EXIT_FAILURE);
	}

	/* close the standard file descriptors */
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	syslog(LOG_DEBUG, "DEBUG: server damonized and start working");
}  /* damonize */

	/* Daemon-specific initialization goes here */

	char puffer[200];

	// if(getcwd(puffer,sizeof(puffer)) == NULL) {
	// 	//    fprintf(stderr, "Fehler bei getcwd ...\n");
	// 	syslog(LOG_ERR, "ERROR: can not get PWD! [%m]");
	// 	// return EXIT_FAILURE;
	// } else {
	// 	syslog(LOG_DEBUG, "DEBUG: the actual working directory: %s", puffer);
	// }

	int tmp;
	tmp=parseConfFile();

	// syslog(LOG_EMERG,   "EMERG  : A panic condition. This is normally broadcast to all users.");
	// syslog(LOG_ALERT,   "ALERT  : A condition that should be corrected immediately, such as a corrupted system database.");
	// syslog(LOG_CRIT,    "CRIT   : Critical conditions, e.g., hard device errors.");
	// syslog(LOG_ERR,     "ERR    : Errors.");
	// syslog(LOG_WARNING, "WARNING: Warning messages.");
	// syslog(LOG_NOTICE,  "NOTICE : Conditions that are not error conditions, but should possibly be handled specially.");
	// syslog(LOG_INFO,    "INFO   : Informational messages.");
	// syslog(LOG_DEBUG,   "DEBUG  : Messages that contain information normally of use only when debugging a program.");

	/* Do some task here ... */
	mySocket=initWebserver(tcpControl);
	closeWebserver(mySocket);

	/* endless-loop */
	for (loop=0; ;loop++) {
		if (loop>=0x80000000) {
			loop=0;
		}
/* webserver */
{
	{
		tmpListen=listen(mySocket, 10);
		if (tmpListen < 0) {
			perror("webserver listen:");
			printf(" %d\n",tmpListen);
			exit(1);
		}

		setNonblocking(mySocket);
		clientAddrLen = sizeof( (struct sockaddr *) &clientAddress);

		if ((new_socket = accept(mySocket, (struct sockaddr *) &clientAddress, &clientAddrLen)) < 0) {
				if (errno == EAGAIN) { // no data available
				} else {
					perror("webserver accept");
					printf("%d \n",newSocket);
					exit(1);
				}
			} else { // data available
			if (newSocket <= 0){
				perror("webserver connect:");
			}
			/* receive header */
			// memset(bufferHTTP, 0, bufsize);
			// fill_n(bufferHTTP, 0, bufsize);
			for (loop2=0;loop2<bufSize;loop2++) {
				bufferHTTP[loop2]=0;
			}

			recv(new_socket, bufferHTTP, bufsize, 0);
			// printf("%s*ENDE*", bufferHTTP);
			// getPostData(bufferHTTP,bufsize);
			getPostData(bufferHTTP,bufsize);
			logBufferAscii(bufferHTTP,bufsize);

			/* send response */
			write(new_socket, "HTTP/1.1 200 OK\r\n", 17);
			write(new_socket, "Content-length: 111\r\n", 21);
			write(new_socket, "Content-Type: text/html\r\n\r\n", 27);
			write(new_socket, "<html>\r\n",8);
			write(new_socket, " <body>\r\n",9);
			write(new_socket, "  <h1>Found</h1>\r\n",18);
			write(new_socket, "  <p>The requested URL was found on this server ;-)</p>\r\n",57);
			write(new_socket, " </body>\r\n",10);
			write(new_socket, "</html>\r\n",9);

			/* TODO: in PHP post-data will be send only after
			receiving the 200-OK-Header. Add or use the second buffer only!
			*/
			// /* receive posted data */
			// memset(bufferHTTP, 0, bufsize);
			// recv(new_socket, bufferHTTP, bufsize, 0);
			// printf("%s*ENDE*\n", bufferHTTP);
			// printBuffer(bufferHTTP,bufsize);
			// fflush(stdout);
			/* close this socket */
			close(new_socket);

} /* webserver */

	}

	syslog(LOG_DEBUG, "DEBUG: SMI-Server closing");
	closelog();
	exit(EXIT_SUCCESS);
}
