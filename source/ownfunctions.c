#include <syslog.h>			/* syslog */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "parseconf.h"
#include "typesdef.h"           /* type definitions and include of global configuration */


// char itoh(int integer) {
// }



unsigned int htoi(char* hex) {
    int max = strlen(hex);
    int cnt=0;
    unsigned int hexInt = 0;
    while (cnt < max) {
        switch ( hex[cnt] ) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                hexInt = hexInt << 4;
                hexInt |= ( hex[cnt] - 0x30 );
                break;
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
                hexInt = hexInt << 4;
                hexInt |= ( hex[cnt] - (0x60-9) );
                break;
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
                hexInt = hexInt << 4;
                hexInt |= ( hex[cnt] - (0x40-9) );
                break;
            case 'x':   // 0xABCD
                break;
            case ' ':
            case 'h':
            case 'H':
            default:
                return hexInt;
                break;
        }
        cnt++;
    }
    return hexInt;
}

char itoh(int value, int digits) {
  // TODO: check for overflow
  // int digits=3;

  int tmpValue = value;
  char zeichen[digits +1];
  zeichen[digits]=0;
  int i;
  for (i=digits; i>0; i--) {
    printf("i=%2d",i);
    if (tmpValue<myPow(16,i-1)) {
        zeichen[digits-i]='0';
    } else {
        zeichen[digits-i]=0x30 + tmpValue / myPow(16,i-1);
        if (zeichen[digits-i]>0x39) {
            zeichen[digits-i] += 0x27;
        }
        tmpValue = tmpValue % (int) myPow(16,i-1);
    }
    printf(" => %2x %s\n",tmpValue, zeichen );
  }
  return zeichen;
}

char itoa(int value, int digits) {
  // TODO: check for overflow
  // int digits=3;

  int tmpValue = value;
  char zeichen[digits +1];
  zeichen[digits]=0;
  int i;
  for (i=digits; i>0; i--) {
    printf("i=%2d",i);
    if (tmpValue<myPow(10,i-1)) {
        zeichen[digits-i]='0';
    } else {
        zeichen[digits-i]=0x30 + tmpValue / myPow(10,i-1);
        if (zeichen[digits-i]>0x39) {
            zeichen[digits-i] += 0x27;
        }
        tmpValue = tmpValue % (int) myPow(10,i-1);
    }
    printf(" => %2x %s\n",tmpValue, zeichen );
  }
  return zeichen;
}

int myPow(int base, int value ) {
  int i;
  if (value==0) {
    return 1;
  }
  int tmpValue=base;
  for(i = 0;i < value; i++) {
    tmpValue *= base;
  }
}

/* Subtract the ‘struct timeval’ values X and Y,
   storing the result in RESULT.
   Return 1 if the difference is negative, otherwise 0. */

int timeval_subtract (struct timeval *result, struct timeval *x, struct timeval *y) {
  /* Perform the carry for the later subtraction by updating y. */
  if (x->tv_usec < y->tv_usec) {
    int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
    y->tv_usec -= 1000000 * nsec;
    y->tv_sec += nsec;
  }
  if (x->tv_usec - y->tv_usec > 1000000) {
    int nsec = (x->tv_usec - y->tv_usec) / 1000000;
    y->tv_usec += 1000000 * nsec;
    y->tv_sec -= nsec;
  }

  /* Compute the time remaining to wait.
     tv_usec is certainly positive. */
  result->tv_sec = x->tv_sec - y->tv_sec;
  result->tv_usec = x->tv_usec - y->tv_usec;

  /* Return 1 if result is negative. */
  return x->tv_sec < y->tv_sec;
}

/* display buffer in ASCII to stdout */
void printBufferAscii(unsigned char *buffer, int size) {
	printf("  (%d ",size);
	int x;
	char c2;
	for (x = 0; x < (size) ; x++)
	{
		c2 = buffer[x];
		putchar(c2);
	}
	printf(")");
	fflush(stdout); // Will now print everything in the stdout buffer
}

void logBufferAscii(unsigned char *buffer, int size) {
    syslog(LOG_DEBUG, "DEBUG: HTTP Buffer(%d):\n %s", size, buffer);
}
