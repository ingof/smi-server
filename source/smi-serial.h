/* smi-serial.h */
#ifndef SMI_SERIAL_H
#define SMI_SERIAL_H

extern DRIVE drive[MAX_DRIVES];
extern GROUP group[MAX_GROUPS];
extern char openHABHost[30];
extern int openHABPort;


/* open port to smi-bus */
int openSmiPort(char *port);
/* check smi checksum */
int checkSmiCrc(unsigned char *buffer, int size);
/* add the cheksum byte to the buffer and returns number of cheksum bytes */
/* add the crc to an existing message */
int addSmiCrc(unsigned char *buffer, int size);

/* try to read one byte from SMI. returns byte or negative number*/
int readSmiByte(int port);

/* handle received SMI-bus data */
int handleSMI(int handle, int port);

/* check time lapsed since last receive of data */
void checkSMI(int port);

/* parse smi buffer */
int parseSMI(unsigned char * buffer, int length, int port);
/* */
void setDrivePos (int bus, int id, unsigned int pos);

/* send smi command to one drive */
int sendSmi(int port, int id, int cmd);
int sendSmiGetPos(int port, int id);

/* send smi command to a group of drives */
int sendSmiGrp(int groupID, int smiCmd);

#endif /* smi-serial.h */
