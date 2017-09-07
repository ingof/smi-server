#include <stdio.h>			/* Standard input/output definitions */
#include <fcntl.h>			/* File control definitions */
#include <termios.h>		/* POSIX terminal control definitions */
#include <sys/ioctl.h>		/* ioctl() */
#include <linux/serial.h>	/* custom divisor */

#include "typesdef.h"		/* own type definitions */
#include "swb-serial.h"		/* own functions */

/* creates SWB crc16 */
unint16_t  createSwbCrc(unsigned char *buffer, int size) {
	unint16_t crc = 0xffff;    // preset CRC
	unint16_t CRC = 0x8408;    // for reverse calculation of CRC-16-CCITT
	int i,j;

	for (i=0; i < size-2; i++){
		crc = crc ^ buffer[i];
		for (j=0; j<8; j++){
			if((crc & 0x01) == 0){
				crc = crc >> 1;
			} else {
				crc = crc >> 1;
				crc = crc ^ CRC;
			}
		}
	}
 return ~crc;
}

/* display swbBuffer in hex-format to stdout */
void printSwbBuffer(unsigned char *buffer, int size) {
	printf("  (%d ",size);
	int x;
	char c2;
	for (x = 0; x < (size) ; x++)
	{
		c2 = buffer[x];
		printf("%02X.",c2);
	}
	printf(")");
	fflush(stdout); // Will now print everything in the stdout buffer
}

/* create response of received message */
void createSwbAck(unsigned char *buffer, int size) {
	// fix for short messages:
	if (size==6) size++;
	// clear switch-byte
	buffer[size-3]=0;
	// clear old crc
	buffer[size-2]=0;
	buffer[size-1]=0;
	// display old message
	int crc=createSwbCrc(buffer, size);
	buffer[size-2]=(uint8_t) crc;
	buffer[size-1]=(uint8_t) (crc>>8);
	// display generated acknolegde
	printSwbBuffer(buffer, size);
}

/* add the crc to an existing message */
void addSwbCrc(unsigned char *buffer, int size) {
	buffer[size]=0;
	buffer[size+1]=0;
	// printBuffer(buffer, size);
	int crc=createSwbCrc(buffer, size);
	buffer[size]=(uint8_t) crc;
	buffer[size+1]=(uint8_t) (crc>>8);
	// printBuffer(buffer, size+2);
}

/* check SwitchBus crc-16 */
int  checkSwbCrc(unsigned char *buffer, int size) {
	int crc=createSwbCrc(buffer, size);
	if (buffer[size-2]!=(uint8_t) crc) {
		return -1;
	}
	if (buffer[size-1]!=(uint8_t) (crc>>8)) {
		return -2;
	}
	return 0;
}

/* open port to Switch-bus (SWB) */
int openSwbPort (char *port) {
	int fd;
	fd=openSwbPortDiv(port, 0);
	return fd;
}

/* open port to Switch-bus (SWB) */
int openSwbPortDiv (char *port, int divisor) {
	int fd; /* File descriptor for the port */
	fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
	struct termios options;
	struct serial_struct ser;
	/* Get the current options for the SWB-port... */
	if (tcgetattr(fd, &options)<0) perror("tcGetattr");
	if (divisor==0) {
		cfsetispeed(&options, B19200);
		cfsetospeed(&options, B19200);
	} else if (divisor>=0) {
		if (divisor>65536) divisor=65536;
		cfsetispeed(&options, B38400);
		cfsetospeed(&options, B38400);
		/* Set the custom_divisor for special baudrates */
		if (ioctl(fd, TIOCGSERIAL, &ser)<0) perror("tioGserial");
		ser.flags |= ASYNC_SPD_CUST;
		ser.custom_divisor=divisor;
		if (ioctl(fd, TIOCSSERIAL, &ser)<0) perror("tioSserial");
	}
	/* Enable the receiver and set local mode... */
	options.c_cflag |= (CLOCAL | CREAD);
	/* Setting Character Size */
	options.c_cflag &= ~CSIZE; 		/* Mask the character size bits */
	/* Setting 8N2 */
	options.c_cflag &= ~PARENB; 	/* deactivate Parity */
	options.c_cflag |= CSTOPB;		/* two stop bits */
	options.c_cflag &= ~CSIZE;		/* mask for data bits */
	options.c_cflag |= CS8;    		/* Select 8 data bits */
	options.c_iflag &= ~IXON;		/* deactivate XON */
	options.c_iflag &= ~IXOFF;		/* deactivate XOFF */
	options.c_iflag &= ~IGNCR;		/* do NOT ignore CR */
	options.c_iflag &= ~ICRNL;		/* do NOT replace CR with NL */
	options.c_iflag &= ~INLCR;		/* do NOT replace NL with CL */
	options.c_iflag |= IGNBRK;		/* ignore break condition (SWB) */
	/* choosing RAW-Input */
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	/* Set the new options for the port... */
	if (tcsetattr(fd, TCSANOW, &options)<0) perror("tcsetattr");
	return fd;
}
