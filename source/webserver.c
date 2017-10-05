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
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>

// int smiCmd=0;
// int smiId=0;
// int smiGrp=0;

/* set nonblocking */
{
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
}


/* for webserver */
// int new_socket;
// socklen_t clientaddrlen;
// int bufsize = 1024;
// unsigned char *bufferHTTP = malloc(bufsize);
// struct sockaddr_in clientaddress;

int tmpListen;
int tmpBind;

void closeWebserver(int socket) {
    close(socket);
}

int initWebserver(int port) {
    /* webserver */
    int socket;
    struct sockaddr_in address;

    if ((socket = socket(AF_INET, SOCK_STREAM, 0)) > 0) {
        printf("The socket was created:\n");
    }

    // TODO: allow only one or two ip-addresses
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    tmpBind=bind(socket, (struct sockaddr *) &address, sizeof(address)) ;
    if (tmpBind== 0) {
        printf("Binding Socket %d\n",tmpBind);
    } else {
        perror("webserver bind");
    }
    return mySocket
}
