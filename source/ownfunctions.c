#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parseconf.h"
#include "typesdef.h"   /* type definitions */

unsigned int htoi(char *hex) {
    int cnt = sizeof(hex) / sizeof(char) -2;
    unsigned int hexInt = 0;
    while ( cnt >= 0 ) {
        switch ( hex[cnt] ) {
            case (int) '0'..(int) '9':
                hexInt << 4;
                hexint &= ( hex[cnt] - 0x30 );
                break;
            case (int) 'a'..(int) 'f':
            case (int) 'A'..(int) 'F':
                hexInt << 4;
                hexint &= ( hex[cnt] - 0x40 );
                break;
            case (int) ' ':
            default:
                break;
        }
        cnt--;
    }
    return hexInt;
}
