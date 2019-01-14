/* webserver.h */
#ifndef WEBSERVER_H
#define WEBSERVER_H

// typedef unsigned char uint8_t;
// typedef unsigned int  unint16_t;
extern DRIVE drive[MAX_DRIVES];
extern GROUP group[MAX_GROUPS];
extern COMMAND command[1];

/* nonblocking for network socket */
int setNonblocking(int fd);
int initWebserver(int port);
int closeWebserver(int socket);
void handleWebserver(int socket);
int getPostData(unsigned char *buffer, int size, int count);
void extractData(char *name, char *value);
int sendGetRequest(char * host, int * port, char * url);

#endif /* webserver.h */
