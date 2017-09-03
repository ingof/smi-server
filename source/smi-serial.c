
#include <fcntl.h>			/* File control definitions */
#include <termios.h>		/* POSIX terminal control definitions */
#include <linux/serial.h>	/* custom divisor */

#include "smi-serial.h"		/* own functions */


/* check SMI-Bus checksum */
int checkSmiCrc(unsigned char *buffer, int size) {
	if (size<=2) {
		return -2;
	}
	/* create checksum */
	unsigned char tmpChkSum=0;
	int i;
	for (i = 0; i < size-1; i++) {
		tmpChkSum+=  buffer[i];
	}
	tmpChkSum=(~tmpChkSum)+1;
	if (buffer[size-1]!=tmpChkSum) {
		return -1;
	}
	return 0;
}

/* add the checksum byte to the buffer and returns number of checksum bytes */
int addSmiCrc(unsigned char *buffer, int size) {
	/* create checksum */
	unsigned char tmpChkSum=0;
	int i;
	for (i = 0; i < size; i++) {
		tmpChkSum+=buffer[i];
	}
	tmpChkSum=(~tmpChkSum)+1;
	buffer[size]=tmpChkSum;
	//TODO "return 0" in error case
	return 1;
}

/* open port to smi-bus */
int openSmiPort (char *port) {
	int fd2;
	fd2 = open(port, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
	if (fd2 == -1) {
		return(-1);
	} else {
		fcntl(fd2, F_SETFL, 0);
	}
	struct termios options2;
	/* Get the current options for the SMI-port... */
	tcgetattr(fd2, &options2);
	/* Set the baud rates to 2400... */
	cfsetispeed(&options2, B2400);
	cfsetospeed(&options2, B2400);
	/* Enable the receiver and set local mode... */
	options2.c_cflag |= (CLOCAL | CREAD);
	/* Setting 8N1 */
	options2.c_cflag &= ~CSIZE; 	/* Mask the character size bits */
	options2.c_cflag |= CS8;    	/* Select 8 data bits */
	options2.c_cflag &= ~PARENB; 	/* deactivate Parity */
	options2.c_cflag &= ~CSTOPB;	/* one stop bit */
	options2.c_iflag &= ~IXON;		/* deactivate XON */
	options2.c_iflag &= ~IXOFF;		/* deactivate XOFF */
	options2.c_iflag &= ~IGNCR;		/* do NOT ignore CR */
	options2.c_iflag &= ~ICRNL;		/* do NOT replace CR with NL */
	options2.c_iflag &= ~INLCR;		/* do NOT replace NL with CL */
	options2.c_iflag |= IGNBRK;		/* ignore break condition (SMI) */
	/* choosing RAW-Input */
	options2.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	/* Set the new options for the port... */
	tcsetattr(fd2, TCSANOW, &options2);
	return fd2;
}
