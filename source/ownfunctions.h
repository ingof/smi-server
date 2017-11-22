/* ownfunctions.h */
#ifndef OWNFUNCTIONS_H
#define OWNFUNCTIONS_H


/* functions */
//char itoh(int integer);
unsigned int htoi(char* hex);
int getRemotePort(struct sockaddr *addr)
char[] getRemoteIP(struct sockaddr *addr)
void printBufferAscii(unsigned char *buffer, int size);
void logBufferAscii(unsigned char *buffer, int size);

#endif /* ownfunctions.h */
