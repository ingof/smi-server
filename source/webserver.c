#include <stdio.h>			/* Standard input/output definitions */
#include <string.h>			/* String function definitions */
#include <unistd.h>			/* UNIX standard function definitions */
#include <fcntl.h>			/* File control definitions */
#include <errno.h>			/* Error number definitions */
#include <termios.h>		/* POSIX terminal control definitions */
#include <stdlib.h>			/* converting functions */
#include <sys/ioctl.h>		/* ioctl() */
#include <sys/types.h>		/* ?? */
#include <sys/stat.h>		/* ?? */
#include <sys/time.h>		/* ?? */
#include <linux/serial.h>	/* custom divisor */

#include "typesdef.h"	/* own funcions */
#include "webserver.h"
#include "smi-server.h"	/* own funcions */

/* web server */

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// int smiCmd=0;
// int smiId=0;
// int smiGrp=0;

// TODO use syslog for logs

int setNonblocking(int fd) {
    int flags;
        /* If they have O_NONBLOCK, use the Posix way to do it */
    #if defined(O_NONBLOCK)
        /* Fixme: O_NONBLOCK is defined but broken on SunOS 4.1.x and AIX 3.2.5. */
        if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
            flags = 0;
        return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    #else
        /* Otherwise, use the old way of doing it */
        flags = 1;
        return ioctl(fd, FIOBIO, &flags);
    #endif
}


/* for webserver */
// int new_socket;
// socklen_t clientaddrlen;
// int bufsize = 1024;
// unsigned char *bufferHTTP = malloc(bufsize);
// struct sockaddr_in clientaddress;
// int tmpListen;


void closeWebserver(int socket) {
    close(socket);
}

int initWebserver(int port) {
    /* webserver */
    int webSocket;
    int tmpBind;
    struct sockaddr_in address;

    if ((webSocket = socket(AF_INET, SOCK_STREAM, 0)) > 0) {
        syslog(LOG_DEBUG, "DEBUG: The socket was created:\n");
    }

    // TODO: allow only one or two ip-addresses
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    tmpBind=bind(webSocket, (struct sockaddr *) &address, sizeof(address)) ;
    if (tmpBind== 0) {
        syslog(LOG_DEBUG, "DEBUG: Binding Socket %d\n",tmpBind);
    } else {
        syslog(LOG_NOTICE, "webserver bind");
    }
    return webSocket;
}

int getPostData(unsigned char *buffer, int size, int count) {
    int smiCmd;
    int smiId;
    int smiGrp;
    char *token;
	char *tokenName;
	char *tokenValue;
	// char *word="\r\n\r\n";
	char *word="GET";
	char *postStart;
	// printBuffer(buffer, size);
	// printBufferAscii(buffer, size);
	//TODO check header
	// printf(".");
	// char string[] = {"Ein Teststring mit Worten"};
	// printf(".");
	// printf("%s\n",strchr(string, (int)'W'));
	// printf(".");
	// printf("%s\n",strchr(string, (int)'T'));
	// printf(".");


	/* find end of header */
	postStart = strstr((char*) buffer,word)+6;

	/* remove "end of header" marker */
	// token=strsep(&postStart,"\n");
	// token=strsep(&postStart,"\n");
	// printf("\nTOKENS:");
	// printf("Size: (%d) PostStart:{%s}\n",size,&postStart[0]);

	/* clear old values */
	smiCmd=0;
	smiId=0;
	smiGrp=0;

	/* extract each posted data pair */
	while ((token=strsep(&postStart,"&")) != NULL) {
			tokenName=strsep(&token,"=");
		tokenValue=strsep(&token,"=");
		// printf("Parameter \"%s\" is \"%s\"\n",tokenName,tokenValue);
		// printf(".");
		if ((tokenName != NULL) && (tokenValue != NULL)) {
		// if ((tokenName != "") && (tokenValue != "")) {
			// printf("\n1");
			if (strcmp(tokenName,"cmd")==0) {
				// printf(".");
				smiCmd=atoi(tokenValue);
				// printf(".");
				if (smiCmd>16) smiCmd=16;
				// printf(".");
				if (smiCmd<0) smiCmd=0;
				// printf(".");
			}
			// printf("\n2");
			if (strcmp(tokenName,"id")==0) {
				smiId=atoi(tokenValue);
				if (smiId>16) smiId=16;
				if (smiId<0) smiId=0;
			}
			// printf("\n3");
			if (strcmp(tokenName,"grp")==0) {
				smiGrp=atoi(tokenValue);
				smiGrp &=0xffff;
				if (smiGrp<0) smiGrp=0;
			}
			// printf("\n4\n\n");
		} else {
			syslog(LOG_NOTICE, "NOTICE: no token found");
		}
	}
	syslog(LOG_DEBUG, "DEBUG:\n\033[36m%6d.%03d WWW: ID:%02X GR:%02X CM:%02X\033[1m\033[0m",count/2000,(count/2)%1000,smiId,smiGrp,smiCmd);
	// printf("\n\033[1m%6d.%03d SMI: ",loop/2000,(loop/2)%1000);
	// fflush(stdout);
	return 0;
}

// next function...
