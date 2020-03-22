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

#include "typesdef.h"       /* type definitions and include of global configuration */
#include "webserver.h"
#include "smi-server.h"	    /* own funcions */
#include "ownfunctions.h"

/* web server */
#include <netinet/in.h>

/* web client */
#include <sys/socket.h>
#include <netdb.h>

// socklen_t clientAddrLen;
//char* remoteIp="";
char remoteIP[INET6_ADDRSTRLEN];
int remotePort;

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


int closeWebserver(int socket) {
    return close(socket);
}

int initWebserver(int port) {
    /* webserver */
    //TODO: fix webserver init. add debug-messages via syslog

    int webSocket;
    int tmpBind;
    struct sockaddr_in address;

    if ((webSocket = socket(AF_INET, SOCK_STREAM, 0)) >= 0) {
        syslog(LOG_DEBUG, "D The socket was created:");
    } else {
        syslog(LOG_WARNING, "W socket creation error (%d)", errno);
    }

    // TODO: allow only one or two ip-addresses
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    tmpBind=bind(webSocket, (struct sockaddr *) &address, sizeof(address)) ;
    if (tmpBind== 0) {
        syslog(LOG_DEBUG, "D Binding Socket %d\n",tmpBind);
    } else {
        syslog(LOG_WARNING, "W Can not bind webserver (bind:%d)", tmpBind);
        syslog(LOG_DEBUG, "D Can not bind webserver (bind:%d)", tmpBind);
        //TODO: Return EXIT_FAILURE instead of websocket at binding error????
        // close socket on error?? SO_EXCLUSIVEADDRUSE in bind???
    }
    int tmpListen;

    tmpListen = listen(webSocket, 10);
    if (tmpListen < 0) {
        syslog(LOG_NOTICE, "N webserver listen: %d", tmpListen);
        exit(1);
    }

    // /* nake socket reusable */
    // tmpListen=setsockopt(webSocket, SOL_SOCKET, SO_REUSEADDR,);
    int optval = 1;
    tmpListen = setsockopt(webSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    setNonblocking(webSocket);

    return webSocket;
}

void handleWebserver(int socket) {
    int newSocket;
    socklen_t clientAddrLen;
	  int bufSize = 1024;
	  struct sockaddr_in clientAddress;
    int loop;

    struct sockaddr_storage tmpAddr;
    socklen_t tmpLen;
    tmpLen = sizeof(tmpAddr);


    clientAddrLen = sizeof( (struct sockaddr_in *) &clientAddress);

//    if ((newSocket = accept(socket, (struct sockaddr_in *) &clientAddress, &clientAddrLen)) < 0) {
    if ((newSocket = accept(socket, (struct sockaddr *) &tmpAddr, &tmpLen)) < 0) {
        if (errno == EAGAIN) { // no data available
        } else {
            syslog(LOG_NOTICE, "N webserver accept %d / (%s)", newSocket, inet_ntoa(clientAddress.sin_addr));
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
            syslog(LOG_DEBUG, "D webserver connected (%s:%d)", remoteIP, remotePort);
        }
        /* receive header */
        unsigned char *bufferHTTP = calloc(bufSize,sizeof(char));
        // memset(bufferHTTP, 0, bufsize);
        // fill_n(bufferHTTP, 0, bufsize);
        // for (loop=0;loop<bufSize;loop++) {
        //     bufferHTTP[loop]=0;
        // }

        recv(newSocket, bufferHTTP, bufSize, 0);
        // logBufferAscii(bufferHTTP,bufSize);
        if (getPostData(bufferHTTP,bufSize,loop)!=0) {
            syslog(LOG_DEBUG, "D does not contain control data (%s:%d)", remoteIP, remotePort);
        }
        free(bufferHTTP);

        // TODO: move string generation to smi-server.c or smi-serial.c ???!!!

        char ctrlBuff[MAX_DRIVES][700];
        char ctrlTmpStr[80];
        int ctrlLength = 148;
        int ctrlCount = 0;
        for (loop = 0; loop < MAX_DRIVES; loop ++) {
          if ((drive[loop].id != -1) && (drive[loop].name[0] != 0)) {
            strcpy(ctrlBuff[ctrlCount], "<form action=\"http://192.168.1.211:8088/SmiControl\" method=\"post\">\r\n");
            sprintf(ctrlTmpStr, "<input type=\"hidden\" name=\"id\" value=\"%d\">\r\n", loop);
            strcat(ctrlBuff[ctrlCount], ctrlTmpStr);
            // strcat(ctrlBuff[ctrlCount], "<input type=\"hidden\" name=\"grp\" value=\"-1\">\r\n");
            strcat(ctrlBuff[ctrlCount], "<button type=\"submit\" name=\"cmd\" value=\"1\">&uArr;</button>\r\n");
            strcat(ctrlBuff[ctrlCount], "<button type=\"submit\" name=\"cmd\" value=\"0\">&#9632;</button>\r\n");
            strcat(ctrlBuff[ctrlCount], "<button type=\"submit\" name=\"cmd\" value=\"2\">&dArr;</button>\r\n");
            strcat(ctrlBuff[ctrlCount], "<button type=\"submit\" name=\"cmd\" value=\"3\">&#x1F31E;</button>\r\n");
            strcat(ctrlBuff[ctrlCount], "<button type=\"submit\" name=\"cmd\" value=\"4\">&#x1F453;</button>\r\n");
            sprintf(ctrlTmpStr, "&nbsp;%3d%% &nbsp; %s\r\n", (int)(( drive[loop].actualPos / 655.35)+0.5), drive[loop].name);
            strcat(ctrlBuff[ctrlCount], ctrlTmpStr);
            strcat(ctrlBuff[ctrlCount], "</form>\r\n");
            ctrlLength += strlen(ctrlBuff[ctrlCount]);
            ctrlCount ++;
          }
        }
        for (loop = 0; loop < 5; loop ++) {
          // if ((group[loop].smiID[0] > 0) & (group[loop].smiID[1] > 0)) {
          // if (true) {
            strcpy(ctrlBuff[ctrlCount], "<form action=\"http://192.168.1.211:8088/SmiControl\" method=\"post\">\r\n");
            sprintf(ctrlTmpStr, "<input type=\"hidden\" name=\"grp\" value=\"%d\">\r\n", loop);
            strcat(ctrlBuff[ctrlCount], ctrlTmpStr);
            // strcat(ctrlBuff[ctrlCount], "<input type=\"hidden\" name=\"grp\" value=\"-1\">\r\n");
            strcat(ctrlBuff[ctrlCount], "<button type=\"submit\" name=\"cmd\" value=\"1\">&uArr;</button>\r\n");
            strcat(ctrlBuff[ctrlCount], "<button type=\"submit\" name=\"cmd\" value=\"0\">&#9632;</button>\r\n");
            strcat(ctrlBuff[ctrlCount], "<button type=\"submit\" name=\"cmd\" value=\"2\">&dArr;</button>\r\n");
            strcat(ctrlBuff[ctrlCount], "<button type=\"submit\" name=\"cmd\" value=\"3\">&#x1F31E;</button>\r\n");
            strcat(ctrlBuff[ctrlCount], "<button type=\"submit\" name=\"cmd\" value=\"4\">&#x1F453;</button>\r\n");
            sprintf(ctrlTmpStr, "&nbsp;...%% &nbsp; %s\r\n", group[loop].name);
            strcat(ctrlBuff[ctrlCount], ctrlTmpStr);
            strcat(ctrlBuff[ctrlCount], "</form>\r\n");
            ctrlLength += strlen(ctrlBuff[ctrlCount]);
            ctrlCount ++;
          // }
        }
        int writtenBytes=0;
        /* send response */
        writtenBytes += (write(newSocket, "HTTP/1.1 200 OK\r\n", 17));
        char tmpStr[30];
        sprintf(tmpStr, "Content-length: %d\r\n", ctrlLength); // 22
        writtenBytes += (write(newSocket, tmpStr, (strlen(tmpStr))));
        writtenBytes += (write(newSocket, "Content-Type: text/html\r\n\r\n", 27));
        /* end of Header, start of html data */
        writtenBytes += (write(newSocket, "<!DOCTYPE html>\r\n",17));
        writtenBytes += (write(newSocket, "<html lang=\"de\">\r\n", 18));
        writtenBytes += (write(newSocket, "<head><meta charset=\"utf-8\"/>", 29));
        writtenBytes += (write(newSocket, "<title>SMI-Server</title></head>\r\n", 34));
        writtenBytes += (write(newSocket, " <body>\r\n",9));
        writtenBytes += (write(newSocket, "  <h1>SMI Control</h1>\r\n",22));
        for (loop = 0; loop < ctrlCount; loop ++) {
          writtenBytes += (write(newSocket, ctrlBuff[loop], (strlen(ctrlBuff[loop]))));
        }

        writtenBytes += (write(newSocket, " </body>\r\n",10));
        writtenBytes += (write(newSocket, "</html>\r\n",9));

        // syslog(LOG_DEBUG, "WEBSERVER: Bytes: %d - written: %d = header: %d", ctrlLength, writtenBytes, (ctrlLength - writtenBytes) );



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
  struct timeval tmpTime;
  char *token;
	char *tokenName;
	char *tokenValue;
	// char *word="\r\n\r\n";
  char *word0="SmiControl?";
  char *word1="GET /SmiControl?";
  char *word="GET";
  char *word2="POST";
  char *word4="POST /SmiControl";
  char *word3="\r\n\r\n";
	char *postStart;

  // TODO: check header
  /* clear old values */
  command[0].id = -1;
  command[0].group = -1;
  command[0].command = -1;
  command[0].degree = -1;
  command[0].position = -1;

    // // display header
    // syslog(LOG_DEBUG,"D Header:--------------------------------------------\n%s\n-------------------------------------------------------------------------------------------------------------", buffer);

    // GET request
    if (strstr((char*) buffer,word)) {
      if (strstr((char*) buffer,word1) == NULL) {
        return EXIT_FAILURE;
      }
      /* find start of header */
      postStart = strstr((char*) buffer,word)+6;
      token=strsep(&postStart,"?");
      /* extract first posted data pair */
      tokenName=strsep(&token,"=");
      tokenValue=strsep(&token," =");
      if ((tokenName != NULL) && (tokenValue != NULL)) {
        // syslog(LOG_DEBUG," DEBUG: .GET-Request Name=\"%s\" Value=\"%s\"", tokenName, tokenValue);
        extractData(tokenName, tokenValue);
      }
    }

    // POST request
    if (strstr((char*) buffer,word2)) {
      if (strstr((char*) buffer,word4) == NULL) {
        return EXIT_FAILURE;
      }
      /* find start of Data */
      // syslog(LOG_DEBUG, "Token= %s\r\nName=%s\r\nValue=%s", token, tokenName, tokenValue);
      postStart = strstr((char*) buffer,word3)+2;
      token=strsep(&postStart,"\n");
      // syslog(LOG_DEBUG, "Token= %s\r\nName=%s\r\nValue=%s", token, tokenName, tokenValue);
      /* extract first posted data pair */
      tokenName=strsep(&token,"=");
      tokenValue=strsep(&token," =");
      // syslog(LOG_DEBUG, "Token= %s", &token);
      if ((tokenName != NULL) && (tokenValue != NULL)) {
        // syslog(LOG_DEBUG, " extract P  ID:%02d GR:%02d CM:%02d", command[0].id, command[0].group, command[0].command);
        // syslog(LOG_DEBUG," DEBUG: POST-Request Name=\"%s\" Value=\"%s\"", tokenName, tokenValue);
        extractData(tokenName, tokenValue);
      }
    }

    /* extract each posted data pair */
    while ((token=strsep(&postStart,"&")) != NULL) {
      tokenName=strsep(&token,"=");
      tokenValue=strsep(&token," =");
      if ((tokenName != NULL) && (tokenValue != NULL)) {
        // syslog(LOG_DEBUG," DEBUG: ....-Request Name=\"%s\" Value=\"%s\"", tokenName, tokenValue);
        extractData(tokenName, tokenValue);
      }
    }
  gettimeofday( &tmpTime, (struct timezone *) 0 );
  syslog(LOG_INFO, "I  %03d WEB:  <- \e[36m%s:%d ID:%02d GR:%02d CM:%02d DG:%d PO:%d\e[1m\e[0m", (tmpTime.tv_usec/1000), remoteIP, remotePort, command[0].id, command[0].group, command[0].command, command[0].degree, command[0].position);
	return EXIT_SUCCESS;
}

void extractData(char *name, char *value) {
  if (strncmp(name, "cmd", 3) == 0) {
    command[0].command=atoi(value);
    // stop drive
    if (strncmp(value, "stop", 4) == 0) {
      command[0].command = 0;
    }
    // direction up
    if (strncmp(value, "auf", 3) == 0) {
      command[0].command = 1;
    }
    if (strncmp(value, "hoch", 4) == 0) {
      command[0].command = 1;
    }
    if (strncmp(value, "up", 2) == 0) {
      command[0].command = 1;
    }
    // direction down
    if (strncmp(value, "ab", 2) == 0) {
      command[0].command = 2;
    }
    if (strncmp(value, "runter", 6) == 0) {
      command[0].command = 2;
    }
    if (strncmp(value, "down", 4) == 0) {
      command[0].command = 2;
    }
    // position commands
    if (strncmp(value, "pos1", 4) == 0) {
      command[0].command = 3;
    }
    if (strncmp(value, "sun", 3) == 0) {
      command[0].command = 3;
    }
    if (strncmp(value, "sonne", 5) == 0) {
      command[0].command = 3;
    }
    if (strncmp(value, "pos2", 4) == 0) {
      command[0].command = 4;
    }
    if (strncmp(value, "eye", 3) == 0) {
      command[0].command = 4;
    }
    if (strncmp(value, "auge", 4) == 0) {
      command[0].command = 4;
    }
    if (strncmp(value, "getpos", 6) == 0) {
      command[0].command = 5;
    }

    // check and correct data
    if (command[0].command > 5) command[0].command = 0;
    if (command[0].command < 0) command[0].command = 0;
    // return;
  }
  if (strcmp(name,"id") == 0) {
    command[0].id=atoi(value);
    // check and correct data
    if (command[0].id > 31) command[0].id = -1;
    if (command[0].id < 0) command[0].id = -1;
    // return;
  }
  if (strcmp(name,"grp")==0) {
    command[0].group=atoi(value);
    // check and correct data
    command[0].group &=0x7fff;
    if (command[0].group < 0) command[0].group=-1;
    // return;
  }
  if (strcmp(name,"deg")==0) {
    command[0].degree=atoi(value);
    // check and correct data
    //command[0].degree &=0x01ff;
    if (command[0].degree > 511) command[0].degree=-1;
    if (command[0].degree < 1) command[0].degree=-1;
    // return;
  }
  if (strcmp(name,"pos")==0) {
    command[0].position=atoi(value);
    // check and correct data
    //command[0].position &=0xffff;
    if (command[0].position > 65535) command[0].position=-1;
    if (command[0].position < 0) command[0].position=-1;
    // return;
  }
   //syslog(LOG_DEBUG, " status: ID=%02d GR=%02d CM=%02d DG=%d PS=%d", command[0].id, command[0].group, command[0].command, command[0].degree, command[0].position);
}

int sendGetRequest(char * host, int port, char * url) {
  // TODO freeze after DEBUG: Error Connecting
  int responseCode=0;
  char request[1000];
  struct timeval tmpTime;
  struct hostent *server;
  struct sockaddr_in serveraddr;
  // TODO: check the replacement of 'bzero' and 'bcopy'
  int tcpSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (tcpSocket < 0) {
    gettimeofday( &tmpTime, (struct timezone *) 0 );
    syslog(LOG_ERR, "E %03d Error opening socket\e[0m",(tmpTime.tv_usec/1000));
    return EXIT_FAILURE;
  } else {
    gettimeofday( &tmpTime, (struct timezone *) 0 );
    //syslog(LOG_DEBUG, "D %03d Successfully opened socket\e[0m",(tmpTime.tv_usec/1000));
  }
  server = gethostbyname(host);
  if (server == NULL) {
    gettimeofday( &tmpTime, (struct timezone *) 0 );
    syslog(LOG_ERR, "E %03d gethostbyname() failed\e[0m",(tmpTime.tv_usec/1000));
    return EXIT_FAILURE;
  // } else {
  //     unsigned int j = 0;
  //     while (server -> h_addr_list[j] != NULL){
  //         gettimeofday( &tmpTime, (struct timezone *) 0 );
  //         syslog(LOG_DEBUG, "D %03d %s => %s\e[0m",(tmpTime.tv_usec/1000) ,server->h_name , inet_ntoa(*(struct in_addr*)(server -> h_addr_list[j])));
  //         j++;
  //     }
  }
  bzero((char *) &serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&serveraddr.sin_addr.s_addr, server->h_length);
  serveraddr.sin_port = htons(port);
  if (connect(tcpSocket, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) {
      gettimeofday( &tmpTime, (struct timezone *) 0 );
      syslog(LOG_ERR, "E %03d Error Connecting\e[0m",(tmpTime.tv_usec/1000));
      return EXIT_FAILURE;
  } else {
      gettimeofday( &tmpTime, (struct timezone *) 0 );
      // syslog(LOG_DEBUG, "D %03d Successfully Connected\e[0m",(tmpTime.tv_usec/1000));
  }
  bzero(request, 1000);
  sprintf(request, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", url, host);
  // syslog(LOG_DEBUG, "D\n%s", request);
  if (send(tcpSocket, request, strlen(request), 0) < 0) {
      gettimeofday( &tmpTime, (struct timezone *) 0 );
      syslog(LOG_ERR, "E %03d Error with send()\e[0m",(tmpTime.tv_usec/1000));
      return EXIT_FAILURE;
  } else {
      gettimeofday( &tmpTime, (struct timezone *) 0 );
      // syslog(LOG_DEBUG, "D %03d Successfully sent html fetch request\e[0m",(tmpTime.tv_usec/1000));
  }
  bzero(request, 1000);
  recv(tcpSocket, request, 999, 0);
  // syslog(LOG_DEBUG, "D\n %s", request);
  char *ptr;
  ptr= strtok(request, " ");
  if (strstr(request, "HTTP/1.")) {
    ptr= strtok(NULL, " ");
    responseCode=atoi(ptr);
    gettimeofday( &tmpTime, (struct timezone *) 0 );
    // syslog(LOG_DEBUG, "D %03d HTTP-header gefunden\e[0m",(tmpTime.tv_usec/1000));
  } else {
    gettimeofday( &tmpTime, (struct timezone *) 0 );
    syslog(LOG_ERR, "E %03d kein HTTP-header gefunden\e[0m",(tmpTime.tv_usec/1000));
  }
  gettimeofday( &tmpTime, (struct timezone *) 0 );
  syslog(LOG_DEBUG, "D %03d Status=\'%d\'\e[0m",(tmpTime.tv_usec/1000), responseCode);
  close(tcpSocket);
  return responseCode;
}

// next function...
