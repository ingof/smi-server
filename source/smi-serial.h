/* smi-serial.h */
#ifndef SMI_SERIAL_H
#define SMI_SERIAL_H


/* open port to smi-bus */
int openSmiPort (char *port);
/* check smi checksum */
int checkSmiCrc(unsigned char *buffer, int size);
/* add the cheksum byte to the buffer and returns number of cheksum bytes */
int addSmiCrc(unsigned char *buffer, int size);
/* add the crc to an existing message */


#endif /* smi-serial.h */
