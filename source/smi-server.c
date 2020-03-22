/* smi-server */
// #include <string.h>		/* String function definitions */
// #include <unistd.h>		/* UNIX standard function definitions */
// #include <fcntl.h>		/* File control definitions */
// #include <errno.h>		/* Error number definitions */
// #include <termios.h>		/* POSIX terminal control definitions */
// #include <stdlib.h>		/* converting functions */
// #include <sys/ioctl.h>	/* ioctl() */
// #include <sys/types.h>	/* ?? */
// #include <sys/stat.h>		/* ?? */
// #include <sys/time.h>		/* ?? */
// #include <linux/serial.h>	/* custom divisor */

#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
// #include <math.h>
#include <netinet/in.h>		/* web server */
#include <sys/socket.h>		/* web server */
#include <sys/stat.h>		/* web server */
#include <sys/types.h>		/* web server */
#include <unistd.h>			/* web server */
#include <sys/select.h>		/* select fd */
#include <arpa/inet.h>
#include <syslog.h>			/* syslog */
#include <stdio.h>			/* Standard input/output definitions */
#include <termios.h>
#include <sys/ioctl.h>


#include "typesdef.h"		/* type definitions and include of global configuration */
#include "webserver.h"
#include "parseconf.h"		/* config parser */
#include "ownfunctions.h"
#include "command.h"
#include "swb-serial.h"		/* swb-bus functions */
#include "smi-serial.h"		/* swb-bus functions */
#include "smi-server.h"		/* own funcions */

#include "swb-serial.c"
#include "smi-serial.c"
#include "webserver.c"
#include "parseconf.c"
#include "ownfunctions.c"
#include "command.c"

// #include <unistd.h>			/* getpwd() */

DRIVE drive[MAX_DRIVES];
GROUP group[MAX_GROUPS];
BUTTON button[MAX_BUTTONS];
COMMAND command[0];
COMMAND smiCmdBuf[32];
int smiCmdBufCount=0;
int smiCmdBufStart=0;
int smiCmdBufLock=0;
long timeoutSMI=8000;
long timeoutSWB=3000;
int tcpControlPort=8088;
int openHABPort=8088;
char openHABHost[30];
char openHABGet[30];
char serialSMI[MAX_SMI_PORTS][40];
char serialSWB[MAX_SWB_PORTS][40];
int serialSwbAck[MAX_SWB_PORTS];
int fdSMI[MAX_SMI_PORTS];
int fdSWB[MAX_SWB_PORTS];

// int listenFd, maxFd, newFd;
// struct fd_set masterSet, workingSet;

unsigned char swbRxBuffer[MAX_SWB_PORTS][50];
unsigned char swbTxBuffer[50];
unsigned char swbTxSize;
unsigned char smiRxBuffer[MAX_SWB_PORTS][50];
unsigned char smiTxBuffer[50];
unsigned char smiTxSize;
unsigned char buffer[12];

struct timeval tmpTime;



int main(int argc, char *argv[]) {
	int i;
	int mySocket;
	int listenFd, maxFd, newFd;
	// struct fd_set masterSet, workingSet;
	fd_set masterSet, workingSet;

/* damonize */
{
	/* my process ID and Session ID */
	pid_t pid, sid;

	openlog(argv[0], LOG_PID | LOG_NDELAY, LOG_LOCAL7);
	syslog(LOG_DEBUG, "D SMI server starting (%s)", argv[1]);
	syslog(LOG_DEBUG, "D Version %s Build %s Date %s", BUILD_VERSION, BUILD_COUNT, BUILD_DATE);
	// syslog(LOG_DEBUG, "D Version %s Build %s", VersionStr, BuildCnt);

	/* fork of the parent process */
	pid = fork();
	if (pid < 0) {
		exit(EXIT_FAILURE);
		syslog(LOG_ERR,  "E fork failed [%m]");
	}
	/* exit parent if got good PID */
	if (pid > 0) {
		exit(EXIT_SUCCESS);
		syslog(LOG_ERR,  "E getPID failed [%m]");
	}

	umask(0);

	/* Open any logs here */

	/* create new SID for the child process */
	sid = setsid();
	if (sid < 0) {
		/* Log the failure */
		syslog(LOG_ERR,  "E creation of new SID failed [%m]");
		exit(EXIT_FAILURE);
	}

	/* change current working directory */
	if ((chdir("/")) < 0) {
		/* Log the failure */
		syslog(LOG_ERR,  "E changing current working directory failed [%m]");
		exit(EXIT_FAILURE);
	}

	/* close the standard file descriptors */
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	syslog(LOG_DEBUG, "D server damonized and start working");
}  /* damonize */

	/* Daemon-specific initialization goes here */

	char puffer[200];

	int tmp;
	initData();
	tmp=parseConfFile();

	// syslog(LOG_EMERG,   "P: A panic condition. This is normally broadcast to all users.");
	// syslog(LOG_ALERT,   "A: A condition that should be corrected immediately, such as a corrupted system database.");
	// syslog(LOG_CRIT,    "C: Critical conditions, e.g., hard device errors.");
	// syslog(LOG_ERR,     "E: Errors.");
	// syslog(LOG_WARNING, "W Warning messages.");
	// syslog(LOG_NOTICE,  "N: Conditions that are not error conditions, but should possibly be handled specially.");
	// syslog(LOG_INFO,    "I: Informational messages.");
	// syslog(LOG_DEBUG,   "D: Messages that contain information normally of use only when debugging a program.");

	/* Do some task here ... */
	mySocket=initWebserver(tcpControlPort);
	command[0].id = -1;
	command[0].group = -1;
	command[0].command = -1;
	command[0].degree = -1;
	command[0].position = -1;

	// fdSWB[0]=openSwbPortDIV(serialSWB[0],serialSwbDivisor);
	// TODO: open all configured ports instead of the first port only
	// fdSWB[0]=openSwbPort(serialSWB[0]);
	// fdSMI[0]=openSmiPort(serialSMI[0]);
	// fdSMI[1]=openSmiPort(serialSMI[1]);

  //TODO: remove unused "else" part
	for (i = 0; i<MAX_SWB_PORTS; i++) {
		// syslog(LOG_DEBUG, "D Loop ->  swb:%d = \'%s\'", i, serialSWB[i]);
		// if (serialSWB[i]=="") {
		if (strcmp(serialSWB[i], "")==0) {
//			syslog(LOG_DEBUG, "D leer =>  SWB-Port#%d \'%s\' ", i, serialSWB[i]);
			//fdSWB[i]=-1;
		} else {
			fdSWB[i]=openSwbPort(serialSWB[i]);
//			syslog(LOG_DEBUG, "D voll =>  SWB-Port#%d \'%s\' returns %d", i, serialSWB[i], fdSWB[i]);
		}
	}

	for (i = 0; i<MAX_SMI_PORTS; i++) {
		// syslog(LOG_DEBUG, "D Loop --> smi:%d = \'%s\'", i, serialSMI[i]);
		if (strcmp(serialSMI[i], "")==0) {
	//		syslog(LOG_DEBUG, "D leer ==> SMI-Port#%d \'%s\'", i, serialSMI[i]);
	    //fdSMI[i]=-1;
		} else {
			fdSMI[i]=openSmiPort(serialSMI[i]);
//			syslog(LOG_DEBUG, "D voll ==> SMI-Port#%d \'%s\' returns %d", i, serialSMI[i], fdSMI[i]);
		}
	}

	FD_ZERO(&masterSet);
	maxFd = mySocket;
	for (i = 0; i < MAX_SWB_PORTS; i ++) {
		if (fdSWB[i]>maxFd) {
			maxFd = fdSWB[i];
		}
	}
	for (i = 0; i < MAX_SMI_PORTS; i ++) {
		if (fdSMI[i]>maxFd) {
			maxFd = fdSMI[i];
		}
	}

	// add open filedescriptors
	FD_SET(mySocket, &masterSet);
	for (i = 0; i < MAX_SWB_PORTS; i ++) {
		if (fdSWB[i] >= 0) {
			FD_SET(fdSWB[i], &masterSet);
		}
	}
	for (i = 0; i < MAX_SMI_PORTS; i ++) {
		if (fdSMI[i] >= 0) {
			FD_SET(fdSMI[i], &masterSet);
		}
	}

	/* endless-loop */
	struct timeval selectTimeout;
	selectTimeout.tv_sec=0;
	selectTimeout.tv_usec=1000;

	int loop;
	// int i;
	for (loop=0; ;loop++) {
		if (loop>=0x80000000) {
			loop=0;
		}

		memcpy(&workingSet, &masterSet, sizeof(masterSet));

		int tmpSelect;

		tmpSelect = select(maxFd + 1, &workingSet, NULL, NULL, &selectTimeout);
		if (tmpSelect < 0) {
			syslog(LOG_WARNING, "W select failed");
		}

		if(FD_ISSET(mySocket, &workingSet)) {
			// syslog(LOG_DEBUG,"enter handleWEB()");
			// syslog(LOG_DEBUG, "D receiving from Webserver");
			handleWebserver(mySocket);
			if ((command[0].id != -1) || (command[0].group != -1) || (command[0].command != -1)) {
				usleep(3000); // wait 3ms for whole message
				handleCommand();
			}
			// syslog(LOG_DEBUG,"exit  handleWEB()");
		}

		for (i = 0; i < MAX_SWB_PORTS; i ++) {
			if(FD_ISSET(fdSWB[i], &workingSet)) {
				// syslog(LOG_DEBUG,"enter handleSWB()");
				if (fdSWB[i] >= 0) {
					usleep(500);
					handleSWB(fdSWB[i], i);
				}
				// syslog(LOG_DEBUG,"exit  handleSWB()");
			}
		}

		gettimeofday( &tmpTime, (struct timezone *) 0 );
		for (i = 0; i < MAX_SMI_PORTS; i ++) {
			if(FD_ISSET(fdSMI[i], &workingSet)) {
				// syslog(LOG_DEBUG,"enter handleSMI()");
				if (fdSMI[i] >= 0) {
					usleep(500);
					// syslog(LOG_DEBUG, "D %03d handleSMI i:%d h:%d",	(tmpTime.tv_usec/1000), i, fdSMI[i]);
					handleSMI(fdSMI[i], i);
				}
				// syslog(LOG_DEBUG,"exit  handleSMI()");
			}
		}

		for (i = 0; i < MAX_SMI_PORTS; i ++) {
			checkSMI(i);
		}
		/* wait 0,1ms */
		usleep(10);
		// usleep(500);
	}
	closeWebserver(mySocket);
	closeSerialSwb(serialSWB[0]);

	syslog(LOG_DEBUG, "D SMI-Server closing");
	closelog();
	exit(EXIT_SUCCESS);
}
