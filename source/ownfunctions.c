#include <syslog.h>			/* syslog */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
