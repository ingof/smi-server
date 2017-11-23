/* webserver.h */
#ifndef WEBSERVER_H
#define WEBSERVER_H

// typedef unsigned char uint8_t;
// typedef unsigned int  unint16_t;
extern COMMAND command[1];

/* nonblocking for network socket */
int setNonblocking(int fd);
int initWebserver(int port);
void closeWebserver(int socket);
void handleWebserver(int socket);
int getPostData(unsigned char *buffer, int size, int count);

#endif /* webserver.h */
