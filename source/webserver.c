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
#include "ownfunctions.h"

/* web server */

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// socklen_t clientAddrLen;
//char* remoteIp="";
char remoteIP[INET6_ADDRSTRLEN];
int remotePort;

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
        syslog(LOG_NOTICE, "NOTICE: webserver bind");
    }
    return webSocket;
}

void handleWebserver(int socket) {
    int newSocket;
    socklen_t clientAddrLen;
	int bufSize = 1024;
	struct sockaddr_in clientAddress;
	int tmpListen;
    // char* remoteIp;
    int loop;


    //char remoteIP[INET6_ADDRSTRLEN];
    //int clientPort;
    struct sockaddr_storage tmpAddr;
    socklen_t tmpLen;
    tmpLen = sizeof(tmpAddr);


    tmpListen=listen(socket, 10);
    if (tmpListen < 0) {
        syslog(LOG_NOTICE, "NOTICE: webserver listen: %d", tmpListen);
        exit(1);
    }

    setNonblocking(socket);
    clientAddrLen = sizeof( (struct sockaddr_in *) &clientAddress);

//    if ((newSocket = accept(socket, (struct sockaddr_in *) &clientAddress, &clientAddrLen)) < 0) {
    if ((newSocket = accept(socket, (struct sockaddr *) &tmpAddr, &tmpLen)) < 0) {
        if (errno == EAGAIN) { // no data available
        } else {
            syslog(LOG_NOTICE, "NOTICE: webserver accept %d / (%s)", newSocket, inet_ntoa(clientAddress.sin_addr));
            exit(1);
        }
    } else { // data available

        // get remote IP and Port
        if (tmpAddr.ss_family == AF_INET) {
            struct sockaddr_in *s = (struct sockaddr_in *)&tmpAddr;
            remotePort = ntohs(s->sin_port);
            inet_ntop(AF_INET, &s->sin_addr, remoteIP, sizeof remoteIP);
        } else { // AF_INET6
            struct sockaddr_in6 *s = (struct sockaddr_in6 *)&tmpAddr;
            remotePort = ntohs(s->sin6_port);
            inet_ntop(AF_INET6, &s->sin6_addr, remoteIP, sizeof remoteIP);
        }


        if (newSocket <= 0){
            syslog(LOG_DEBUG, "DEBUG: webserver connected (%s:%d)", remoteIP, remotePort);
        }
        /* receive header */
        unsigned char *bufferHTTP = calloc(bufSize,sizeof(char));
        // memset(bufferHTTP, 0, bufsize);
        // fill_n(bufferHTTP, 0, bufsize);
        // for (loop=0;loop<bufSize;loop++) {
        //     bufferHTTP[loop]=0;
        // }

        recv(newSocket, bufferHTTP, bufSize, 0);
        // printf("%s*ENDE*", bufferHTTP);
        // getPostData(bufferHTTP,bufsize);
        logBufferAscii(bufferHTTP,bufSize);
        if (getPostData(bufferHTTP,bufSize,loop)==0) {
            syslog(LOG_DEBUG, "DEBUG Steuerbefehl empfangen ! (%s:%d)", remoteIP, remotePort);
        }
        free(bufferHTTP);

        /* send response */
        write(newSocket, "HTTP/1.1 200 OK\r\n", 17);
        write(newSocket, "Content-length: 111\r\n", 21);
        write(newSocket, "Content-Type: text/html\r\n\r\n", 27);
        write(newSocket, "<html>\r\n",8);
        write(newSocket, " <body>\r\n",9);
        write(newSocket, "  <h1>Found</h1>\r\n",18);
        write(newSocket, "  <p>The requested URL was found on this server ;-)</p>\r\n",57);
        write(newSocket, " </body>\r\n",10);
        write(newSocket, "</html>\r\n",9);

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
        close(newSocket);
    }
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
    char *word0="SmiControl";
	char *postStart;

	//TODO check header
    // exit if get does not contain 'SmiControl'
    if (strstr((char*) buffer,word0) ==NULL) {
        return EXIT_FAILURE;
    }
	/* find start of header */
	postStart = strstr((char*) buffer,word)+6;

	/* remove "end of header" marker */
	// token=strsep(&postStart,"\n");
	// token=strsep(&postStart,"\n");
	// printf("\nTOKENS:");
	// printf("Size: (%d) PostStart:{%s}\n",size,&postStart[0]);

	/* clear old values */
	smiCmd=NULL;
	smiId=NULL;
	smiGrp=NULL;

	/* extract each posted data pair */
	while ((token=strsep(&postStart,"&?")) != NULL) {
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
		// } else {
		// 	syslog(LOG_NOTICE, "NOTICE: no token found");
		}
	}
	syslog(LOG_DEBUG, "DEBUG: \033[36m %s:%d ID:%02d GR:%02d CM:%02d\033[1m\033[0m",remoteIP, remotePort ,smiId,smiGrp,smiCmd);
	// printf("\n\033[1m%6d.%03d SMI: ",loop/2000,(loop/2)%1000);
	// fflush(stdout);
	return EXIT_SUCCESS;
}

// next function...
