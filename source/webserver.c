#include <stdio.h>			/* Standard input/output definitions */
#include <string.h>			/* String function definitions */
#include <unistd.h>			/* UNIX standard function definitions */
#include <fcntl.h>			/* File control definitions */
#include <errno.h>			/* Error number definitions */
#include <termios.h>		/* POSIX terminal control definitions */
#include <stdlib.h>			/* converting functions */
#include <sys/ioctl.h>		/* ioctl() */
#include "smi-monitor.h"	/* own funcions */
#include <sys/types.h>		/* ?? */
#include <sys/stat.h>		/* ?? */
#include <sys/time.h>		/* ?? */
#include <linux/serial.h>	/* custom divisor */

/* web server */
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>

int smiCmd=0;
int smiId=0;
int smiGrp=0;

int main( int argc, char* argv[] ) {
	/* default for commandline parameter */
	char * serialSwbPort="/dev/ttySWB0";
	char serialSwb0Port[]="/dev/ttySWB0";
	char * serialSmiPort="/dev/ttySMI0";
	char serialSmi0Port[]="/dev/ttySMI0";
	int serialSwbWait=3;					//3ms; 1,5ms; 15ms
	int serialSmiWait=10;					//10ms; 5ms ; 40ms

	int actualSwbTimeout=0;
	int actualSmiTimeout=0;
	int IOReturn;
	int serialSwbDivisor=0;
	int serialSwbAck=0;
	int fdSwb;						/* File descriptor for the SWB-port */
	int fdSmi;						/* File descriptor for the SMI-port */
	int serialBytes;
	int x;
	int loop;
	int loop2;
	int c;

	int bytesSwb=0;
	int bufferSwbCount=0;
	unsigned char bufferSwb[50];

	int bytesSmi=0;
	int bufferSmiCount=0;
	unsigned char bufferSmi[50];

	unsigned char tmpBuffer[40];

	//smi transmit
	char smiTxBuffer[50];
	char smiTxSize=0;

	// temporary test use:
	unsigned char tmp2Buf[50];
	int tmp2Ret;
	int tmp2Siz;
	int tmp2Err;
	// char tmp3Buf[50];

	/* for webserver */
	int mySocket, new_socket;
	int clientaddrlen;
	socklen_t addrlen;
	int bufsize = 1024;
	char *bufferHTTP = malloc(bufsize);
	struct sockaddr_in address, clientaddress;

	int tmpListen;
	int tmpBind;

	/* first parameter is serialSwb0Port*/
	if (argc > 1) {
		serialSwbPort=argv[1];
	} else {
		serialSwbPort=serialSwb0Port;
	}
	/* second parameter is serialSwbWait in ms*/
	if (argc > 2) {
		serialSwbWait=atoi(argv[2]);
	}
	/* third parameter is serialSwbDivisor*/
	if (argc > 3) {
		serialSwbDivisor=atoi(argv[3]);
	}
	/* fourth parameter is serialSwbAck*/
	if (argc > 4) {
		serialSwbAck=atoi(argv[4]);
	}
	/* fifth parameter is serialSmi0Port*/
	if (argc > 5) {
		serialSmiPort=argv[5];
	} else {
		serialSmiPort=serialSmi0Port;
	}
	/* sixth parameter is serialSmiWait in ms*/
	if (argc > 6) {
		serialSmiWait=atoi(argv[6]);
	}

	fdSwb=openSwbPortDiv(serialSwbPort, serialSwbDivisor);
	printf(serialSwbPort);
	if (fdSwb==-1) {
		perror("\tUnable to open serial SWB-port ");
	} else {
		if (serialSwbDivisor>0) {
			printf("\tSWB: Custom 8N2 (%dms /%d",serialSwbWait,serialSwbDivisor);
		} else  {
			printf("\tSWB: 19.200 8N2 (%dms",serialSwbWait);
		}
			if (serialSwbAck==1) printf(" Ack");
			printf(")\n");
	}

	fdSmi=openSmiPort(serialSmiPort);
	printf(serialSmiPort);
	if (fdSmi==-1) {
		perror("\tUnable to open serial SMI-port ");
	} else {
		printf("\tSMI:  2.400 8N1 (%dms)\n",serialSmiWait);
	}

	/* webserver */
	if ((mySocket = socket(AF_INET, SOCK_STREAM, 0)) > 0) {
		printf("The socket was created:\n");
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(8081);

	tmpBind=bind(mySocket, (struct sockaddr *) &address, sizeof(address)) ;
	if (tmpBind== 0) {
		printf("addrlen; %d\n", addrlen);
		printf("address; %d\n", address);
		printf("Binding Socket %d\n",tmpBind);
	} else perror("webserver bind");

	perror(".");
	/* endless-loop */
	for (loop=0; ;loop++) {
		if (loop>=0x80000000) loop=0;


		/* web server */
		tmpListen=listen(mySocket, 10);
		if (tmpListen < 0) {
			perror("webserver listen:");
			printf(" %d\n",tmpListen);
			exit(1);
		}

		setNonblocking(mySocket);
		// addrlen = sizeof(address);
		clientaddrlen = sizeof( (struct sockaddr *) &clientaddress);
		// address.sin_family = AF_INET;
		// address.sin_addr.s_addr = INADDR_ANY;
		// address.sin_port = htons(8081);

		if ((new_socket = accept(mySocket, (struct sockaddr *) &clientaddress, &clientaddrlen)) < 0) {
				if (errno == EAGAIN) { // no data available
				} else {
					perror("webserver accept");
					printf("%d \n",new_socket);
					printf("addrlen: %d", clientaddrlen);
					printf("address: %d", clientaddress);
					exit(1);
				}
			} else { // data available
			if (new_socket <= 0){
				perror("webserver connect:");
			}
			/* receive headder */
			// memset(bufferHTTP, 0, bufsize);
			// fill_n(bufferHTTP, 0, bufsize);
			for (loop2=0;loop2<bufsize;loop2++) {
				bufferHTTP[loop2]=0;
			}

			recv(new_socket, bufferHTTP, bufsize, 0);
			// printf("%s*ENDE*", bufferHTTP);
			getPostData(bufferHTTP,bufsize);
			// printBuffer(bufferHTTP,bufsize);
			/* send response */
			write(new_socket, "HTTP/1.1 200 OK\r\n", 17);
			write(new_socket, "Content-length: 108\r\n", 21);
			write(new_socket, "Content-Type: text/html\r\n\r\n", 27);
			// write(new_socket, "200 OK\r\n",8);
			write(new_socket, "<html>\r\n",8);
			write(new_socket, " <body>\r\n",9);
			write(new_socket, "  <h1>Found</h1>\r\n",18);
			write(new_socket, "  <p>The requested URL was found on this server.</p>\r\n",54);
			write(new_socket, " </body>\r\n",10);
			write(new_socket, "</html>\r\n",9);

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
			close(new_socket);
			smiTxBuffer[0]= 0x50 | (smiId & 0x0f);
			smiTxBuffer[1]= smiCmd & 0x0f;
			smiTxSize=2;
			// printBuffer(smiTxBuffer,smiTxSize);
			smiTxSize+=addSmiCrc(smiTxBuffer,smiTxSize);
			// printBuffer(smiTxBuffer,smiTxSize);
			write(fdSmi,&smiTxBuffer,smiTxSize);
			printf("\n SMI command sent\n");
		}


		/* SWB-Bus */
		IOReturn=ioctl(fdSwb, FIONREAD, &serialBytes);
		// IOReturn=0;
		// serialBytes=0;
		if (IOReturn<0) {
			perror("ioctl(swb)");
			if (actualSwbTimeout>0) actualSwbTimeout--;
		}

		if (IOReturn==0) {
			if ((serialBytes==0)&&(actualSwbTimeout>0)) {
				actualSwbTimeout--;
			}
			if (serialBytes>0) {
				// if ((actualSwbTimeout==0)&&(bufferSwbCount==0)) {
				// 	/* start receiving and reset timeout */
				// 	actualSwbTimeout=serialSwbWait;
				// }
				if ((actualSwbTimeout>=0)&&(bufferSwbCount>=0)) {
					/* start receiving and reset timeout */
					actualSwbTimeout=serialSwbWait;
				}
				/* create temporary buffer for received Bytes */
				bytesSwb = read(fdSwb, &tmpBuffer, serialBytes);
				if (bytesSwb<0) {
					perror("read(Swb)");
				}
				if (bytesSwb<=0) {
					actualSwbTimeout--;
				}
				if (bytesSwb>0) {
					// memcpy(bufferSwb+bufferSwbCount, tmpBuffer, bytesSwb);

					// strncpy(bufferSwb+bufferSwbCount, tmpBuffer, bytesSwb);

					for (loop2=0;loop2<bytesSwb;loop2++) {
						bufferSwb[bufferSwbCount+loop2]=tmpBuffer[loop2];
					}
					bufferSwbCount+=bytesSwb;
				}
			}
			/* stop receiving and print message */
			if ((actualSwbTimeout==0)&&(bufferSwbCount>0)) {
				printf("\n%6d.%03d SWB: ",loop/2000,(loop/2)%1000);
				for (x = 0; x < (bufferSwbCount-2) ; x++)
				{
					c = bufferSwb[x];
					printf("%02X ",c);
				}
				// memmove(tmp2Buf,bufferSwb,bufferSwbCount-2);

				// strncpy(tmp2Buf,bufferSwb,bufferSwbCount-2);

				for (loop2=0;loop2<(bufferSwbCount-2);loop2++) {
					tmp2Buf[loop2]=bufferSwb[loop2];
				}


				tmp2Siz=bufferSwbCount;
				switch (checkSwbCrc(bufferSwb,bufferSwbCount)) {
					case -2:
						/* crc 2 not ok -> yellow */
						printf("\033[1m");
						tmp2Ret=-2;
						break;
					case -1:
						/* crc 1 not ok -> red */
						printf("\033[31m");
						tmp2Ret=-1;
						break;
					default:
						/* crc is ok -> green */
						printf("\033[32m");
						tmp2Ret=0;
						break;
				}
				c = bufferSwb[bufferSwbCount-2];
				printf("%02X ",c);
				c = bufferSwb[bufferSwbCount-1];
				printf("%02X ",c);

				printf("\033[m");
				bufferSwbCount=0;

					if (tmp2Ret==0) {
						if (serialSwbAck==1) {
							createSwbAck(tmp2Buf,tmp2Siz); //
							write(fdSwb,&tmp2Buf,tmp2Siz);
							write(fdSwb,&tmp2Buf,7);
						}
					}
					else {
						tmp2Err++;
						if (tmp2Err<=3) {
						} else {
							tmp2Err=0;
							if (serialSwbAck==1) {
								printf("\007\033[m\033[41m\033[1m   !  S T O P  !\033[40m\033[m"); createSwbAck(tmp2Buf,tmp2Siz); //
								write(fdSwb,&tmp2Buf,tmp2Siz);
								write(fdSwb,&tmp2Buf,7);
						}
					}
				}
				fflush(stdout); // Will now print everything in the stdout buffer
			}
		}

		/* SMI-Bus */
		IOReturn=ioctl(fdSmi, FIONREAD, &serialBytes);
		// IOReturn=0;
		// serialBytes=0;
		if (IOReturn<0) {
			perror("ioctl(smi)");
			if (actualSmiTimeout>0) actualSmiTimeout--;
		}
		if (IOReturn==0) {
			/* no data -< */
			if ((serialBytes==0)&&(actualSmiTimeout>0)) {
				actualSmiTimeout--;
			}
			/* copy received data to buffer */
			if (serialBytes>0) {
				if ((actualSmiTimeout==0)&&(bufferSmiCount==0)) {
					/* start receiving and reset timeout */
					actualSmiTimeout=serialSmiWait;
				}
				if ((actualSmiTimeout>=0)&&(bufferSmiCount>=0)) {
					/* start receiving and reset timeout */
					actualSmiTimeout=serialSmiWait;
				}
				/* create temporary buffer for received Bytes */
				int tmpBuffer[serialBytes];
				bytesSmi = read(fdSmi, &tmpBuffer, sizeof(tmpBuffer));
				if (bytesSmi<0) {
					perror("read(Smi)");
				}
				if (bytesSmi<=0) {
					actualSmiTimeout--;
				}
				if (bytesSmi>0) {
					// strncpy(bufferSmi+bufferSmiCount, tmpBuffer, bytesSmi);
					for (loop2=0;loop2<bytesSmi;loop2++) {
						bufferSmi[bufferSmiCount+loop2]=tmpBuffer[loop2];
					}
					bufferSmiCount+=bytesSmi;
				}
			}
			/* stop receiving and print message */
			if ((actualSmiTimeout==0)&&(bufferSmiCount>0)) {
				printf("\n\033[1m%6d.%03d SMI: ",loop/2000,(loop/2)%1000);
				for (x = 0; x < (bufferSmiCount-1) ; x++)
				{
					c = bufferSmi[x];
					printf("%02X ",c);
				}
				switch (checkSmiCrc(bufferSmi,bufferSmiCount)) {
					case -2:
						/* frame without CRC -> yellow */
						printf("\033[1m");
						break;
					case -1:
						/* crc not ok -> red */
						printf("\033[31m");
						break;
					default:
						/* crc is ok -> green */
						printf("\033[32m");
						break;
				}
				c = bufferSmi[bufferSmiCount-1];
				printf("%02X \033[m",c);
				bufferSmiCount=0;
				fflush(stdout); // Will now print everything in the stdout buffer
			}
		}

		/* wait 0,5ms */
		usleep(500);
	}

	/* Close Ports */
	close(fdSwb);
	printf("<serial Swb port closed!\n");
	close(fdSmi);
	printf("<serial Swb port closed!\n");
	close(mySocket);
	return (0);

}

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

/* display buffer in hex-format to stdout */
void printBuffer(unsigned char *buffer, int size) {
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
	printBuffer(buffer, size);
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

int setNonblocking(int fd)
{
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

int getPostData(unsigned char *buffer, int size) {
	char *token;
	char *tokenName;
	char *tokenValue;
	char *word="\r\n\r\n";
	char *postStart;

	//TODO check header

	/* find end of header */
	postStart = strstr(buffer,word);

	/* remove "end of header" marker */
	token=strsep(&postStart,"\n");
	token=strsep(&postStart,"\n");
	// printf("\nTOKENS:");
	// printf("PostStart:{%s}\n",&postStart[4]);

	/* clear old values */
	smiCmd=0;
	smiId=0;
	smiGrp=0;

	/* extract each posted data pair */
	while ((token=strsep(&postStart,"&")) != NULL) {
		tokenName=strsep(&token,"=");
		tokenValue=strsep(&token,"=");
		if ((tokenName != NULL) && (tokenValue != NULL)) {
			if (strcmp(tokenName,"cmd")==0) {
				smiCmd=atoi(tokenValue);
				if (smiCmd>16) smiCmd=16;
				if (smiCmd<0) smiCmd=0;
			}
			if (strcmp(tokenName,"id")==0) {
				smiId=atoi(tokenValue);
				if (smiId>16) smiId=16;
				if (smiId<0) smiId=0;
			}
			if (strcmp(tokenName,"grp")==0) {
				smiId=atoi(tokenValue);
				smiGrp &=0xffff;
				if (smiGrp<0) smiGrp=0;
			}
		}
	}
	printf("id:%02X grp:%02X cmd:%02X\n",smiId,smiGrp,smiCmd);
	fflush(stdout);
	return 0;
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
	options.c_cflag |= CS8;    		/* Select 8 data bits */
	options.c_cflag &= ~PARENB; 	/* deactivate Parity */
	options.c_cflag |= CSTOPB;		/* two stop bits */
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;
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
