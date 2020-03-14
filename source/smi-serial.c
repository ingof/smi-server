
#include <fcntl.h>			/* File control definitions */
#include <termios.h>		/* POSIX terminal control definitions */
#include <linux/serial.h>	/* custom divisor */
#include <sys/time.h>

#include "smi-serial.h"		/* own functions */
#include "ownfunctions.h"		/* own functions */

#include "configuration.h" /* configuration */


// TODO: handle only in config-file defined Ports
unsigned char smiRxBuff[MAX_SMI_PORTS][64];
unsigned char smiRxCount[MAX_SMI_PORTS];
struct timeval smiRxStart[MAX_SMI_PORTS];
int posFlag[MAX_SMI_PORTS];
int diagFlag[MAX_SMI_PORTS];




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

void checkSMI(int port) {
struct timeval actualTime;
struct timeval lapsedTime;
if (smiRxCount[port] > 0) {
	gettimeofday(&actualTime, (struct timezone *) 0 );
	timeval_subtract(&lapsedTime, &actualTime, &smiRxStart[port]);
	//syslog(LOG_DEBUG, "DEBUG: S:%u,%06u a:%u,%06u l:%u,%06u SMI <- (%2d)", (smiRxStart[port].tv_sec), (smiRxStart[port].tv_usec), (actualTime.tv_sec), (actualTime.tv_usec), (lapsedTime.tv_sec), (lapsedTime.tv_usec),   smiRxCount[port]  );
	// 5ms lapsed since last received data ???
	if (lapsedTime.tv_usec > timeoutSMI) {
		parseSMI(smiRxBuff[port], smiRxCount[port], port);
		int i;
		char bufMsg[300]="";
		char tmpBufMsg[4]="";
		for (i=0; i<smiRxCount[port]; i++) {
	   sprintf(tmpBufMsg, "%02X ", smiRxBuff[port][i]);
		 strcat(bufMsg,tmpBufMsg);
		}
		syslog(LOG_DEBUG, "DEBUG: %03u SMI:%d <- (%02d) %s", (actualTime.tv_usec)/1000,port , smiRxCount[port] , bufMsg);
		// parseSMI();
		smiRxCount[port] = 0;
	}
}
	// return EXIT_SUCCESS;
}


int parseSMI(unsigned char * buffer, int length, int port) {
		int tmpID = 0;
		int tmpMask = 0;
		unsigned int tmpPosition = 0;
		struct timeval tmpTime;

		//////int LOG_DEBUG = 0;
		// char strTst[30] = "Test: ";
    // char strTmp[14] = " -angehängt.";
    // strcat(strTst,strTmp);
    // printf("%s\n",strTst);
    // strcat(strTst," #gghgh#");
    // printf("%s\n",strTst);

    unsigned char tmpCRC = 0;
    int i, j;
    char message[150]="";
    char tmpStr[80]="";

    for (i = 0; i < length; i++) {
				// Befehl:
        if ((buffer[i] & 0x80) == 0x00) {
            if (((buffer[i] & 0xe0) == 0x20) && (buffer[i+1] == 0x00)) {
                // Diagnose
								strcpy(message, "Diagnose: ");
                tmpCRC+=buffer[i]+buffer[i+1];
								i++;
								diagFlag[port] = 1;
								posFlag[port] = 0;
            }
            if ((buffer[i] & 0xe0) == 0x40) {
                // Fahrbefehl
                strcpy(message, "Fahren  : ");
                tmpCRC+=buffer[i];
                if ((buffer[i] & 0x10) == 0x10) {
                    //strcat(message, itoa((int*)buffer[i] & 0x0f));
                    sprintf(tmpStr, "ID  :   %02u ",(buffer[i] & 0x0f));
                    strcat(message, tmpStr);
                }
                i++;

                if (buffer[i] == 0xC0) {
                    tmpCRC+=buffer[i]+buffer[i+1]+buffer[i+2];
                    i++;
                    //ID-Mask
                    tmpMask = (buffer[i++]*0x100+buffer[i++]);
                    sprintf(tmpStr, "Mask: %04X ", tmpMask);
                    strcat(message, tmpStr);
                }

                tmpCRC+=buffer[i];
                if ((buffer[i]&0x0f) == 0x00) {
                    strcat(message, "STOP   ");
                    // STOP
                }
                if ((buffer[i]&0x0f) == 0x01) {
                    strcat(message, "HOCH   ");
                    // UP
                }
								if ((buffer[i]&0x0f) == 0x02) {
                    strcat(message, "RUNTER ");
                    // DOWN
                }
								if ((buffer[i]&0x0f) == 0x03) {
                    strcat(message, "POS.1  ");
                    // POSITION 1
                }
								if ((buffer[i]&0x0f) == 0x04) {
                    strcat(message, "POS.2  ");
                    // POSITION 2
                }
								if ((buffer[i]&0x0f) == 0x05) {
                    // sprintf(tmpStr, "->POS. ");
                    strcat(message, "->POS. ");
                    // TO POSITION
                }

								if ((buffer[i] & 0x80) == 0x80) {
                    tmpCRC+=buffer[i+1]+buffer[i+2];
                    sprintf(tmpStr, "Befehlsoption '%02X': %02X ",buffer[i+1],buffer[i+2]);
                    strcat(message, tmpStr);
                    i+=2;
                    // Weitere Befehl
                }
								if ((buffer[i] & 0x40) == 0x40) {
									tmpCRC+=buffer[i+1]+buffer[i+2];
									sprintf(tmpStr, "Daten: %04X = %3d%% ", (buffer[i+1] << 8)+buffer[i+2], (int)(( ((buffer[i+1] << 8)+buffer[i+2]) / 655.35)+0.5));
									strcat(message, tmpStr);
									i+=2;
									// DATA [POS.]
								}
								if ((buffer[i] & 0x20) == 0x20) {
									tmpCRC+=buffer[i+1];
									sprintf(tmpStr, "Daten: %3d° ",buffer[i+1]*2);
									strcat(message, tmpStr);
									i++;
									// DATA [°]
								}
                i++;
                tmpCRC+=buffer[i];
                if (tmpCRC) {
                    sprintf(tmpStr, "\e[31m(%02X %02x)\e[0m", buffer[i], tmpCRC);
                } else {
                    sprintf(tmpStr, "\e[32m(OK)\e[0m");
                }
                strcat(message, tmpStr);
                gettimeofday( &tmpTime, (struct timezone *) 0 );
								syslog(LOG_INFO,"INFO:  %03d \e[1mSMI:%d <- %s\e[0m",(tmpTime.tv_usec/1000) ,port , message);
                strcpy(message,"");
                i+=1; // CRC ignorieren
            }
            if ((buffer[i] & 0xe0) == 0x60) {
                i++;
                if ((buffer[i] == 0x05)) {
									sprintf(message, "Position: ");
                    //Positionsabfrage
                    tmpCRC=((buffer[i-1] + buffer[i] + buffer[i+1]) & 0xff);
                    if (tmpCRC==0x00) {
                        tmpID = buffer[i-1] & 0x0f;
                        sprintf(tmpStr, "ID=%u", tmpID);
                        posFlag[port] = 1;
												diagFlag[port] = 0;
                        i += 1;
                    } else {
                        sprintf(tmpStr, "Frame Fehler! \e[31m(%02X)\e[0m",tmpCRC);
                        posFlag[port] = 0;
												diagFlag[port] = 0;
                    }
                    strcat(message, tmpStr);
                    // TODO: check:  syslog(LOG_DEBUG,"<--SMI %s\n", message);
                }
            }
        /* Antwort */
				} else {

            /* NACK (0xC0, 0xE0) */
						if ((buffer[i]  & 0xDF) == 0xC0) {
							// sprintf(message, "\e[31m(NACK)\e[0m ");
							i++;
            }

            /* ACK (0xFE, 0xFF) */
						if ((buffer[i] & 0xFE) == 0xFE) {
							// sprintf(message, "\e[1m\e[32m(ACK) \e[0m ");
							i++;
							if (diagFlag[port] == 1) {
								diagFlag[port] = 0;
								posFlag[port] = 0;
								strcat(message, "\e[1mStatus:   ");
								if ((buffer[i++]  & 0xDF) == 0xC0) {
									strcat(message, "( HOCH ) ");
								} else {
									strcat(message, "(      ) ");
								}
								if ((buffer[i++]  & 0xDF) == 0xC0) {
									strcat(message, "(RUNTER) ");
								} else {
									strcat(message, "(      ) ");
								}
								if ((buffer[i++]  & 0xDF) == 0xC0) {
									strcat(message, "( STOP ) ");
								} else {
									strcat(message, "(      ) ");
								}
								if ((buffer[i++]  & 0xDF) == 0xC0) {
									strcat(message, "\e[31m(NACK)\e[0m ");
									// strcat(message, "(  ??  ) ");
								} else {
									strcat(message, "\e[32m(ACK) \e[0m ");
									// strcat(message, "(      ) ");
								}
							}
            }

						/* Position */
            if (buffer[i]==0xEF) {
                // mit Position (~ACK)
                i++;
                if ((buffer[i] == 0x45)) {
                    sprintf(message, "Antwort :  ");
                    // Position
                    tmpCRC=((buffer[i-1]+buffer[i]+buffer[i+1]+buffer[i+2]+buffer[i+3])&0xff);
                    if ((tmpCRC == 0x00)) {
                        if (posFlag[port] == 1) {
                            tmpPosition = buffer[i+1]*0x100+buffer[i+2];
                            sprintf(tmpStr, "ID=%u Position=%u ", tmpID, tmpPosition);
														for (j = 0; j < MAX_DRIVES; j++) {
															if ((drive[j].bus == port) && (drive[j].id == tmpID)) {
																sprintf(tmpStr, "ID:%2d Position:%4X %3d%% '%s'", tmpID, tmpPosition, (int)(( tmpPosition / 655.35)+0.5), drive[j].name);
															} else {
															}
														}

														// TODO: use the smi-bus number for setting Positions
														setDrivePos(port, tmpID, tmpPosition);
                        } else {
                            sprintf(tmpStr, "Position ignoriert ");
                        }
                        strcat(message, tmpStr);
                        i += 3;
                    } else {
                        sprintf(message, "Frame Fehler! \e[31m(%02X)\e[0m",tmpCRC);
                    }
                    // gettimeofday( &tmpTime, (struct timezone *) 0 );
										// syslog(LOG_DEBUG,"DEBUG: %03d \e[1mSMI:%d <- %s\e[0m", (tmpTime.tv_usec/1000), port, message);
                    posFlag[port] = 0;
										diagFlag[port] = 0;
                    tmpID = 0;
                }
            }
						gettimeofday( &tmpTime, (struct timezone *) 0 );
						syslog(LOG_INFO, "INFO:  %03d \e[1mSMI:%d <- %s\e[0m", (tmpTime.tv_usec/1000), port, message);
        }
    }
    return EXIT_SUCCESS;
}

void setDrivePos (int bus, int id, unsigned int pos) {
	 //TODO: reactivate update to openhab and make it work even if the server is not reachable
	 int i;
	 // bus = 0;
	 struct timeval tmpTime;
	 for (i = 0; i < MAX_DRIVES; i++) {
		 if (drive[i].bus == bus) {
			 // syslog(LOG_DEBUG, " \e[32m[True.]\e[0m \e[1mDrive:%2d Bus:%2d ID:%3d, Pos:%04x\e[0m", i, drive[i].bus, drive[i].id ,drive[i].actualPos);
			 if (drive[i].id == id) {
				 drive[i].actualPos = pos;
				 gettimeofday( &tmpTime, (struct timezone *) 0 );
				 syslog(LOG_DEBUG, "DEBUG: %03d \e[1mDRIVE:%2d Bus:%2d ID:%3d, Pos=%04X\e[0m", (tmpTime.tv_usec/1000), i, drive[i].bus, drive[i].id ,drive[i].actualPos);
				 // TODO: enable update if server present
				 char tmpHost[30]="192.168.1.218";
				 int tmpPort=8080;
				 char tmpUrl[50];
				 sprintf(tmpUrl, "%s%s=%03d",openHABGet,drive[i].openHABItem, (int)(( drive[i].actualPos / 655.35)+0.5));
				 gettimeofday( &tmpTime, (struct timezone *) 0 );
				 syslog(LOG_INFO,"INFO:  %03d \e[1;35mupdate \'%s\': \t\t%s:%d%s [<-%d]\e[0m", (tmpTime.tv_usec/1000), drive[i].name, tmpHost, tmpPort, tmpUrl, sendGetRequest(sendSmi, openHABPort, tmpUrl) );
				 break;
			 }
		 } else {
			 // osyslog(LOG_DEBUG, " \e[31m[false]\e[0m \e[1mDrive:%2d Bus:%2d ID:%3d, Pos:%04x\e[0m", i, drive[i].bus, drive[i].id ,drive[i].actualPos);
		 }
	 }
}

int handleSMI(int handle, int port) {
	// TODO: remove port if IOReturn =-1
	// TODO: reopen removed ports if available, again.
	// int port =0;
	struct timeval actualTime;
	struct timeval lapsedTime;


	if (handle <0) {
		return EXIT_FAILURE;
	}
	unsigned char rxBuff[100];
	int IOReturn=-1;
	int serialBytes=-1;
	int bytesSmi=-1;
	// struct timeval tmpTime;
	IOReturn=ioctl(handle, FIONREAD, &serialBytes);
	if (IOReturn<0) {
		syslog(LOG_DEBUG, "DEBUG: IOReturn=%d (h:%d)", IOReturn, handle);
	}
	if (serialBytes>=1) {
		bytesSmi = read(handle, &rxBuff, serialBytes);
		serialBytes=-1;

		if (bytesSmi == 0) {
			syslog(LOG_DEBUG, "DEBUG: data available but no data received (%d)", bytesSmi);
		}
		if (bytesSmi > 0) {
			//syslog(LOG_DEBUG, "DEBUG: (%2d) started: %d",smiRxCount[port] ,smiRxStart[port].tv_usec/1000);
			gettimeofday(&actualTime, (struct timezone *) 0 );
			smiRxStart[port] = actualTime;
			timeval_subtract(&lapsedTime, &actualTime, &smiRxStart[port]);
			// syslog(LOG_DEBUG, "DEBUG: SMI:%d (%2d) start:%3d actual:%3d lapsed:%3d\n", port, smiRxCount[port] ,smiRxStart[port].tv_usec/1000, actualTime.tv_usec/1000, lapsedTime.tv_usec/1000);
			int i;
			for (i = 0; i < bytesSmi; i++) {
				smiRxBuff[port][smiRxCount[port]] = rxBuff[i];
				smiRxCount[port]++;
			}


			// gettimeofday( &tmpTime, (struct timezone *) 0 );
			// char byteStr[40] = "";
			// int i;
			// for (i = 0; i < bytesSmi; i++) {
			// 	char tmpByte = itoh(rxBuff[i], 2);
			// 	strcat(byteStr,  tmpByte);
			// 	strcat(byteStr, " ");
			// }
			//syslog(LOG_DEBUG, "DEBUG: %03d SMI <- %s", (tmpTime.tv_usec/1000),  byteStr );

			//syslog(LOG_DEBUG, "DEBUG: %03d ... <- %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X", (tmpTime.tv_usec/1000),  rxBuff[0], rxBuff[1], rxBuff[2], rxBuff[3], rxBuff[4], rxBuff[5], rxBuff[6], rxBuff[7], rxBuff[8], rxBuff[9], rxBuff[10] );
		}
		if (bytesSmi < 0) {
			syslog(LOG_DEBUG, "DEBUG: error reading smi-port");
		}

	} else {
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}


/* try to read one byte from SMI. returns byte or negative number*/
int readSmiByte(int port) {
	// syslog(LOG_DEBUG," enter readSmiByte()");
	int serialBytes = -1;
	int IOReturn;
	unsigned char rxBuff[10];
	IOReturn = ioctl(fdSMI[port], FIONREAD, &serialBytes);
	if (IOReturn < 0) {
		// syslog(LOG_DEBUG," readSmiByte() returns -1 (IOReturn)");
		return -1;
	}
	if (serialBytes >= 1) {
		// syslog(LOG_DEBUG," readSmiByte() returns 'Data'");
		read(fdSMI[port], &rxBuff, 1);
		return rxBuff[0];
	}
	// syslog(LOG_DEBUG," readSmiByte() returns -1 (no Data)");
	return -2;
}

// int parseSMI() {
// 	return EXIT_FAILURE;
// 	return EXIT_SUCCESS;
// }


/* open port to smi-bus */
int openSmiPort(char *port) {
	int fd;
	fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
	syslog(LOG_DEBUG, "DEBUG: SMI-Port-FD:%d", fd);
	if (fd == -1) {
		return(-1);
		syslog(LOG_WARNING, "WARNING: Unable to open serial SMI-port (fd:%d)", fd);
	} else {
		fcntl(fd, F_SETFL, 0);
		syslog(LOG_DEBUG, "DEBUG: SMI:  2.400 8N1 (fd:%d)", fd);
	}
	struct termios options;
	/* Get the current options for the SMI-port... */
	if (tcgetattr(fd, &options)<0) perror("tcGetattr");
	/* Set the baud rates to 2400... */
	cfsetispeed(&options, B2400);
	cfsetospeed(&options, B2400);
	/* Enable the receiver and set local mode... */
	options.c_cflag |= (CLOCAL | CREAD);
	/* control flags */
	options.c_cflag &= ~CSIZE; 	  /* mask for data bits */
	options.c_cflag |= CS8;     	/* Select 8 data bits */
	options.c_cflag &= ~PARENB; 	/* deactivate Parity */
	options.c_cflag &= ~CSTOPB;	  /* one stop bit */
	/* input flags */
	options.c_iflag &= ~IXON;		  /* deactivate XON */
	options.c_iflag &= ~IXOFF;		/* deactivate XOFF */
	options.c_iflag &= ~IGNCR;		/* do NOT ignore CR */
	options.c_iflag &= ~ICRNL;		/* do NOT replace CR with NL */
	options.c_iflag &= ~INLCR;		/* do NOT replace NL with CL */
	options.c_iflag |= IGNBRK;		/* ignore break condition (SMI) */
	/* output FLags */
	options.c_oflag &= ~OPOST;    /* choosing RAW Output */
	/* choosing RAW-Input */
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	/* Set the new options for the port... */
	if (tcsetattr(fd, TCSANOW, &options)<0) perror("tcsetattr");
	return fd;
}

int sendSmi(int port, int id, int cmd) {
// 	// if (port==0) {
// 	// 	return EXIT_FAILURE;
// 	// }
	// syslog(LOG_INFO, "INFO : port=%d id=%d cmd=%d",port ,id ,cmd );
	// TODO: check if bus is controlled by "smi-server"
	// build and send smi telegramm
	smiTxBuffer[0] = 0x50 + (id & 0x0f);
	smiTxBuffer[1] = cmd & 0x07;
	smiTxSize = 3;
	addSmiCrc(smiTxBuffer, smiTxSize - 1);
	write(fdSMI[port], &smiTxBuffer, smiTxSize);
	// // wait for sending telegram (50ms)
	// usleep(50000);
	// // TODO: check ocho of sent bytes
	// /* ignore sent bytes */
	// readSmiByte(port);
	// readSmiByte(port);
	// readSmiByte(port);
	// // check smi response
	// /* ignore response */
	// readSmiByte(port);
	// // TODO: repeat if response or timeout
	return EXIT_SUCCESS;
}
int sendSmiGetPos(int port, int id) {
	struct timeval tmpTime;
	smiTxBuffer[0] = 0x70 + (id & 0x0f);
	smiTxBuffer[1] = 0x05;
	smiTxSize = 3;
	addSmiCrc(smiTxBuffer, smiTxSize - 1);
	write(fdSMI[port], &smiTxBuffer, smiTxSize);
	gettimeofday( &tmpTime, (struct timezone *) 0 );
	syslog(LOG_DEBUG, "DEBUG: %03d SMI:  -> %02x %02x %02x ", (tmpTime.tv_usec/1000), smiTxBuffer[0], smiTxBuffer[1], smiTxBuffer[2]);
return EXIT_SUCCESS;
}

int sendSmiGrp(int groupID, int cmd) {
	int port;
	// TODO: check what happens if one smi-bus is out of order;
	// TODO: check positions after STOP-command
	for (port = 0; port < MAX_SMI_PORTS; port++) {
		if (group[groupID].smiID[port] != 00) {
			// prepare smi telegram
			smiTxBuffer[0] = 0x40;
			smiTxBuffer[1] = 0xC0;
			smiTxBuffer[2] = group[groupID].smiID[port] >> 8;
			smiTxBuffer[3] = group[groupID].smiID[port] & 0xff;
			smiTxBuffer[4] = cmd & 0x07;
			smiTxSize = 6;
			// complete smi telegramm
			switch (cmd) {
				case 1:		// up
									smiTxBuffer[4] |= 0x80;
									smiTxBuffer[5] = 0x22;
									smiTxBuffer[6] = 0x00;
									smiTxSize += 2;
									break;
				case 0: 	// stop
				case 2:		// down
									break;
				case 3: 	// pos1: sun protection
				case 4:		// pos2: privacy shield
									smiTxBuffer[4] |= 0x80;
									smiTxBuffer[5] = 0x21;
									smiTxBuffer[6] = 0x00;
									smiTxSize += 2;
									break;
				default:	// stop, down
									return EXIT_FAILURE;
									break; // normally not used
			}
			addSmiCrc(smiTxBuffer, smiTxSize - 1);
			write(fdSMI[port], &smiTxBuffer, smiTxSize);
			usleep(500);
		}
	}
	return EXIT_SUCCESS;
}
