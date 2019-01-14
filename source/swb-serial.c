#include <syslog.h>			/* syslog */
#include <stdio.h>			/* Standard input/output definitions */
#include <fcntl.h>			/* File control definitions */
#include <termios.h>		/* POSIX terminal control definitions */
#include <sys/ioctl.h>		/* ioctl() */
#include <linux/serial.h>	/* custom divisor */

#include <time.h>
#include <sys/time.h>

#include "typesdef.h"			/* type definitions and include of global configuration */
#include "swb-serial.h"		/* own functions */
#include "command.h"			/* command handler */


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
	//printSwbBuffer(buffer, size);
}

/* add the crc to an existing message */
void calcSwbCrc(unsigned char *buffer, int size) {
	buffer[size-2]=0;
	buffer[size-1]=0;
	// printBuffer(buffer, size);
	int crc=createSwbCrc(buffer, size);
	buffer[size-2]=(uint8_t) crc;
	buffer[size-1]=(uint8_t) (crc>>8);
	// printBuffer(buffer, size+2);
	//size += 2;
	// syslog(LOG_DEBUG, "DEBUG: CRC %02X %02X %02X %02X %02X %02X %02X", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6]);
}

/* check SwitchBus crc-16 */
int  checkSwbCrc(unsigned char *buffer, int size) {
	int crc=createSwbCrc(buffer, size);
	// syslog(LOG_DEBUG, "DEBUG: CRC 0x%04X (<- %d %02x %02x %02x %02x %02x %02X%02X)", crc, size, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[size-2], buffer[size-1]);
	if (buffer[size-2]!=(uint8_t) crc) {
		return EXIT_FAILURE;
	}
	if (buffer[size-1]!=(uint8_t) (crc>>8)) {
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int handleSWB(int handle, int port) {
	// unsigned char swbRxBuffer[50];
	int IOReturn=-1;
	int serialBytes=-1;
	int bytesSwb=-1;
	int crcStat;
	int i;
	int loop;
	struct timeval tmpTime;
	IOReturn=ioctl(handle, FIONREAD, &serialBytes);
	if (IOReturn<0) {
		syslog(LOG_DEBUG, "DEBUG: IOReturn:2=%d", IOReturn);
	}
	// if (serialBytes>0) {
	if (serialBytes>=7) {
		//// TODO: read all complete messages starting with 0xF0
		//// bytesSwb = read(handle, &rxBuff, serialBytes);
		// bytesSwb = read(handle, &swbRxBuffer[port], 7);
		// serialBytes=-1;
		for (loop = 0; loop < serialBytes; loop++) {
			bytesSwb = read(handle, &swbRxBuffer[port], 1);
			serialBytes--;
			// syslog(LOG_DEBUG,   "   DEBUG: loop:%2d bytesSwb:%2d serialBytes:%2d <- %02X %02X %02X %02X %02X %02X %02X \e[33m####\e[0m", loop, bytesSwb, serialBytes, swbRxBuffer[port][0], swbRxBuffer[port][1], swbRxBuffer[port][2], swbRxBuffer[port][3], swbRxBuffer[port][4], swbRxBuffer[port][5], swbRxBuffer[port][6] );
			if (swbRxBuffer[port][0] == 0xF0) {
				// syslog(LOG_DEBUG, "   BREAK: loop:%2d bytesSwb:%2d serialBytes:%2d <- %02X %02X %02X %02X %02X %02X %02X \e[32m####\e[0m", loop, bytesSwb, serialBytes, swbRxBuffer[port][0], swbRxBuffer[port][1], swbRxBuffer[port][2], swbRxBuffer[port][3], swbRxBuffer[port][4], swbRxBuffer[port][5], swbRxBuffer[port][6] );
				break;
			};
		}
		// syslog(LOG_DEBUG,     "   REST : loop:%2d bytesSwb:%2d serialBytes:%2d <- %02X %02X %02X %02X %02X %02X %02X \e[39m####\e[0m", loop, bytesSwb, serialBytes, swbRxBuffer[port][0], swbRxBuffer[port][1], swbRxBuffer[port][2], swbRxBuffer[port][3], swbRxBuffer[port][4], swbRxBuffer[port][5], swbRxBuffer[port][6] );
		if (serialBytes < 6) {
			serialBytes=-1;
			// syslog(LOG_DEBUG,     "   FAIL : loop:%2d bytesSwb:%2d serialBytes:%2d <- %02X %02X %02X %02X %02X %02X %02X \e[31m####\e[0m", loop, bytesSwb, serialBytes, swbRxBuffer[port][0], swbRxBuffer[port][1], swbRxBuffer[port][2], swbRxBuffer[port][3], swbRxBuffer[port][4], swbRxBuffer[port][5], swbRxBuffer[port][6] );
			return EXIT_FAILURE;
		}
		bytesSwb = read(handle, &(swbRxBuffer[port][1]), 6) + 1;
		// syslog(LOG_DEBUG,     "   ALL  : loop:%2d bytesSwb:%2d serialBytes:%2d <- %02X %02X %02X %02X %02X %02X %02X \e[31m####\e[0m", loop, bytesSwb, serialBytes, swbRxBuffer[port][0], swbRxBuffer[port][1], swbRxBuffer[port][2], swbRxBuffer[port][3], swbRxBuffer[port][4], swbRxBuffer[port][5], swbRxBuffer[port][6] );
		// serialBytes=-1;


		if (bytesSwb == 0) {
			syslog(LOG_DEBUG, "DEBUG: data available but no data received (%d)", bytesSwb);
		}
		if (bytesSwb > 0) {
			// crcStat = checkSwbCrc(rxBuff, bytesSwb);
			gettimeofday( &tmpTime, (struct timezone *) 0 );
			crcStat = checkSwbCrc(swbRxBuffer[port], 7);
			if (crcStat==EXIT_SUCCESS) {
				// TODO: only on first message (swb telegram counter 0x80)
				syslog(LOG_DEBUG, "DEBUG: %03d SWB:%d <- %02X %02X %02X %02X %02X %02X %02X \e[32m(OK)\e[0m", (tmpTime.tv_usec/1000), port, swbRxBuffer[port][0], swbRxBuffer[port][1], swbRxBuffer[port][2], swbRxBuffer[port][3], swbRxBuffer[port][4], swbRxBuffer[port][5], swbRxBuffer[port][6] );
				if ((swbRxBuffer[port][4] & 0x0D) !=0) {
					char tmpName[30];
					char tmpAction[25];
					int smiCmd=-1;
					unsigned int tmpAddr;
					tmpAddr = (swbRxBuffer[port][1] << 8) + swbRxBuffer[port][2];
					for (i = 0; i < MAX_BUTTONS; i++) {
						if (tmpAddr == button[i].addr) {
							strncpy(tmpName, button[i].name, 29);
							switch (swbRxBuffer[port][4] & 0x30) {
								case 0x00:	strcpy(tmpAction, "Hoch ");
														break;
								case 0x10:	strcpy(tmpAction, "Runter ");
														break;
								case 0x20:	strcpy(tmpAction, "Stop ");
														break;
								case 0x30:	strcpy(tmpAction, "Automatik ");
														break;
							}
							switch (swbRxBuffer[port][4] & 0x0f) {
								case 0x03:	strcat(tmpAction, "Klick");
														break;
								case 0x04:	strcat(tmpAction, "Druck");
														break;
								case 0x05:	strcat(tmpAction, "Doppelklick");
														break;
								case 0x06:	strcat(tmpAction, "AUS");
														break;
								case 0x07:	strcat(tmpAction, "EIN");
														break;
								default:		strcat(tmpAction, "'unbekannt'");
														break;
							}
							switch (swbRxBuffer[port][4] & 0x3f) {
								case 0x03:
								case 0x13:
														smiCmd=0; //"Stop"
														break;
								case 0x04:	smiCmd=1; //"Hoch"
														break;
								case 0x14:	smiCmd=2; //"Runter"
														break;
								case 0x15:	smiCmd=3; //"POS1"
														break;
								case 0x05:	smiCmd=4; //"POS2"
														break;
								default:		smiCmd=-1; //"unbekannt");
														break;
							}

							break;
						}
					}
					syslog(LOG_INFO, "INFO:  %03d \e[1mSWB:%d <- Switch %04X '%s -> %s'\e[0m ", (tmpTime.tv_usec/1000), port, tmpAddr, tmpName, tmpAction);
					if (smiCmd >= 0) {
						//TODO: handle more than one drive
						//TODO: serch for drive and handle SMI-Command
					}
					sendSmiCmd(tmpAddr, smiCmd);
				}
				// TODO: only reply one time (swb telegramm counter 0x80)
				if ((serialSwbAck[port] == TRUE) && (swbRxBuffer[port][4] != 0x00)) {
					createSwbAck(swbRxBuffer[port], 7);
					write(handle,swbRxBuffer[port],7);
					syslog(LOG_DEBUG, "DEBUG: %03d SWB:%d -> %02X %02X %02X %02X %02X %02X %02X \e[1m\e[32m(ACK)\e[0m", (tmpTime.tv_usec/1000), port, swbRxBuffer[port][0], swbRxBuffer[port][1], swbRxBuffer[port][2], swbRxBuffer[port][3], swbRxBuffer[port][4], swbRxBuffer[port][5], swbRxBuffer[port][6] );
				}
			} else {
				syslog(LOG_DEBUG, "DEBUG: %03d SWB:%d <- %02X %02X %02X %02X %02X %02X %02X \e[31m(**)\e[0m", (tmpTime.tv_usec/1000), port, swbRxBuffer[port][0], swbRxBuffer[port][1], swbRxBuffer[port][2], swbRxBuffer[port][3], swbRxBuffer[port][4], swbRxBuffer[port][5], swbRxBuffer[port][6] );
			}
		}
		if (bytesSwb < 0) {
			syslog(LOG_DEBUG, "DEBUG: error reading swb-port");
		}

	} else {
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

/* close port */
int closeSwbPort(int handle) {
	return close(handle);
}


int flushSwb(int fd) {
	// syslog(LOG_DEBUG, "DEBUG: flushing swb rx buffer");
	// return tcflush(fd, TCIFLUSH);
	int IOReturn=-1;
	int serialBytes=-1;
	IOReturn=ioctl(fd, FIONREAD, &serialBytes);
	if (IOReturn<0) {
		syslog(LOG_DEBUG, "DEBUG: IOReturn:3=%d", IOReturn);
	}
	if (serialBytes>0) {
		syslog(LOG_DEBUG, "DEBUG: %d swb bytes flushed", serialBytes);
	}
	return tcflush(fd, TCIFLUSH);
}

int readSwb(int fd, int port) {
	// TODO: check for start byte 0xF0;
	unsigned char swbRxBuffer[port][50];
	int IOReturn=-1;
	int serialBytes=-1;
	int bytesSwb=-1;
	int crcStat;
	struct timeval tmpTime;
// int loop;
	//TODO readSwb() broken.
	//usleep(700000);
	IOReturn=ioctl(fd, FIONREAD, &serialBytes);
	if (IOReturn<0) {
		syslog(LOG_DEBUG, "DEBUG: IOReturn:4=%d", IOReturn);
		// if (actualSwbTimeout>0) actualSwbTimeout--;
	}
    //
	// if (IOReturn==0) {
	// 	if ((serialBytes==0)&&(actualSwbTimeout>0)) {
	// 		actualSwbTimeout--;
	// 	}
	// usleep(100000);
	if (serialBytes>0) {
		// if ((actualSwbTimeout>=0)&&(bufferSwbCount>=0)) {
		// 	/* start receiving and reset timeout */
		// 	actualSwbTimeout=serialSwbWait;
		// }
		/* create temporary buffer for received Bytes */
		// syslog(LOG_DEBUG, "DEBUG: serialBytes=%d bytesSwb=%d", serialBytes, bytesSwb);
		bytesSwb = read(fd, &swbRxBuffer[port], serialBytes);
		serialBytes=-1;
		// syslog(LOG_DEBUG, "DEBUG: serialBytes=%d bytesSwb=%d", serialBytes, bytesSwb);
		if (bytesSwb == 0) {
			syslog(LOG_DEBUG, "DEBUG: data available but no data received (%d)", bytesSwb);
		}
		if (bytesSwb > 0) {
			// // memcpy(bufferSwb+bufferSwbCount, tmpBuffer, bytesSwb);
			// // strncpy(bufferSwb+bufferSwbCount, tmpBuffer, bytesSwb);
			// for (loop=0;loop<bytesSwb;loop++) {
			// 	rxBuffer[bytesSwb+loop]=tmpBuffer[loop];
			// syslog(LOG_DEBUG, "DEBUG: <- %02X %02X %02X %02X %02X %02X %02X", swbRxBuffer[port][0], swbRxBuffer[port][1], swbRxBuffer[port][2], swbRxBuffer[port][3], swbRxBuffer[port][4], swbRxBuffer[port][5], swbRxBuffer[port][6] );

			gettimeofday( &tmpTime, (struct timezone *) 0 );
			crcStat = checkSwbCrc(swbRxBuffer[port], bytesSwb);
			if (crcStat==EXIT_SUCCESS) {
				syslog(LOG_DEBUG, "DEBUG: %03d CMD:  <- %02X %02X %02X %02X %02X %02X %02X \e[32m(OK)\e[0m", (tmpTime.tv_usec/1000), swbRxBuffer[port][0], swbRxBuffer[port][1], swbRxBuffer[port][2], swbRxBuffer[port][3], swbRxBuffer[port][4], swbRxBuffer[port][5], swbRxBuffer[port][6] );
			} else {
				syslog(LOG_DEBUG, "DEBUG: %03d CMD:  <- %02X %02X %02X %02X %02X %02X %02X \e[31m(**)\e[0m", (tmpTime.tv_usec/1000), swbRxBuffer[port][0], swbRxBuffer[port][1], swbRxBuffer[port][2], swbRxBuffer[port][3], swbRxBuffer[port][4], swbRxBuffer[port][5], swbRxBuffer[port][6] );
			}

		}

		if (bytesSwb < 0) {
			syslog(LOG_DEBUG, "DEBUG: error reading swb-port");
		}
		// if (bytesSwb<=0) {
		// 	actualSwbTimeout--;
		// }


		// // TODO: check received frame
		// crcStat = checkSwbCrc(swbRxBuffer, bytesSwb);
		// if (crcStat == EXIT_SUCCESS)  {
		// 	syslog(LOG_DEBUG, "DEBUG: SWB CRC is OK");
		// } else {
		// 	syslog(LOG_NOTICE, "NOTICE: SWB CRC check has failed");
		// }
		// return EXIT_SUCCESS;
	} else {
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

/* open port to Switch-bus (SWB) */
int openSwbPort (char *port) {
	return openSwbPortDiv(port, 0);
}

/* open port to Switch-bus (SWB) */
int openSwbPortDiv (char *port, int divisor) {
	int fd; /* File descriptor for the port */
	fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
	syslog(LOG_DEBUG, "DEBUG: SWB-Port-FD:%d", fd);
	if (fd == -1) {
		return(-1);
		syslog(LOG_WARNING, "WARNING: Unable to open serial SWB-port (fd:%d)", fd);
	} else {
		fcntl(fd, F_SETFL, 0);
		syslog(LOG_DEBUG, "DEBUG: SWB: 19.200 8N2 (fd:%d)", fd);
	}
	struct termios options;
	/* Get the current options for the SWB-port... */
	if (tcgetattr(fd, &options)<0) {
		syslog(LOG_WARNING, "WARNING: tcGetattr");
	}
	if (divisor==0) {
		/* Set the baud rates to 19200... */
		cfsetispeed(&options, B19200);
		cfsetospeed(&options, B19200);
	} else if (divisor>=0) {
		struct serial_struct ser;
		if (divisor>65536) divisor=65536;
		cfsetispeed(&options, B38400);
		cfsetospeed(&options, B38400);
		/* Set the custom_divisor for special baudrates */
		if (ioctl(fd, TIOCGSERIAL, &ser)<0) {
			syslog(LOG_WARNING, "WARNING: tiocgSerial");
		}
		ser.flags |= ASYNC_SPD_CUST;
		ser.custom_divisor=divisor;
		if (ioctl(fd, TIOCSSERIAL, &ser)<0) {
			syslog(LOG_WARNING, "WARNING: tiocsSerial");
		}
	}
	/* Enable the receiver and set local mode... */
	options.c_cflag |= (CLOCAL | CREAD);
	/* communication flags */
	options.c_cflag &= ~CSIZE;		/* mask for data bits */
	options.c_cflag |= CS8;    		/* Select 8 data bits */
	options.c_cflag &= ~PARENB; 	/* deactivate Parity */
	options.c_cflag |= CSTOPB;		/* two stop bits */
	/* input flags */
	options.c_iflag &= ~IXON;		  /* deactivate XON */
	options.c_iflag &= ~IXOFF;		/* deactivate XOFF */
	options.c_iflag &= ~IGNCR;		/* do NOT ignore CR */
	options.c_iflag &= ~ICRNL;		/* do NOT replace CR with NL */
	options.c_iflag &= ~INLCR;		/* do NOT replace NL with CL */
	options.c_iflag |= IGNBRK;		/* ignore break condition (SWB) */
	/* output FLags */
	options.c_oflag &= ~OPOST;    /* choosing RAW Output */
	/* choosing RAW-Input */
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	/* Set the new options for the port... */
	if (tcsetattr(fd, TCSANOW, &options)<0) perror("tcsetattr");
	return fd;
}
