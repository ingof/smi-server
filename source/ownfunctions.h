/* ownfunctions.h */
#ifndef OWNFUNCTIONS_H
#define OWNFUNCTIONS_H


/* functions */
//char itoh(int integer);
unsigned int htoi(char* hex);
char itoh(int value, int digits);
void printBufferAscii(unsigned char *buffer, int size);
void logBufferAscii(unsigned char *buffer, int size);
char itoa(int value, int digits);
int myPow(int base, int value );
/* Subtract the ‘struct timeval’ values X and Y,
   storing the result in RESULT.
   Return 1 if the difference is negative, otherwise 0. */

int timeval_subtract (struct timeval *result, struct timeval *x, struct timeval *y);


#endif /* ownfunctions.h */
