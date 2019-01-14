/* swb-serial.h */
#ifndef SWB_SERIAL_H
#define SWB_SERIAL_H

#include "typesdef.h"           /* type definitions and include of global configuration */

extern unsigned char swbRxBuffer[MAX_SWB_PORTS][50];
extern unsigned char swbTxBuffer[50];
extern unsigned char swbTxSize;
extern BUTTON button[MAX_BUTTONS];


/* print buffer to stdio */
void printSwbBuffer(unsigned char *buffer, int size);
/* open ports to Switch-bus (SWB) */
int openSwbPortDiv (char *port, int divisor);
/* display buffer in hex-format to stdout */
int openSwbPort (char *port);
/* handle SWB input; */
int handleSWB(int handle, int port);
/* close Ports */
int closeSwbPort(int handle);
/* check swb crc */
int checkSwbCrc(unsigned char *buffer, int size);
/* adds swbcrc at telegram end */
void calcSwbCrc(unsigned char *buffer, int size);
/* creates SWB crc16 */
unint16_t createSwbCRC(unsigned char *buffer, int size);
/* create response of received message */
void createSwbAck(unsigned char *buffer, int size);
/* simple read of swb-port */
int readSwb(int fd, int port);
/* simple flush of swb-port */
int flushSwb(int fd);

#endif /* swb-serial.h */
