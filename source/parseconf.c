#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parseconf.h"
#include "typesdef.h"   /* type definitions */

char str1[40], str2[40];
char name[40], value[40];
char line[80];
// int year;
FILE * fp;
char *ptr, *ptr2;
int tmp;

// int main() {
//
//  tmp=parseConfFile();
//  return(0);
// }

int parseConfFile(void) {
    char configFile[80]="/var/packages/smi-server/target/share/smi-server.conf";
    int lineCnt=1;
    fp = fopen (configFile, "r");
    if (fp == NULL) {
        syslog(LOG_ERR, "ERROR: Could not open configuration file");
        return 1;
    } else {
        syslog(LOG_DEBUG, "DEBUG: parsing config file: %s", configFile);
        while((fscanf(fp,"%s\n",line)) != EOF) {
            tmp=parseConfLine(line);
            syslog(LOG_ERR, "\tLine(%d): |%s| ->%d", lineCnt, line, tmp);
            lineCnt++;
        }
    }
    syslog(LOG_DEBUG, "\tline: |%s|",line);
    fclose(fp);
    return 0;
}

int parseConfLine(char* line) {
    char* remark;// initialisieren und ersten Abschnitt erstellen
    // remark-line cut line at "#" char
    remark=strchr(line,(int)'#');
    if ( remark != NULL ) {
        // skip comments
        line[remark]="\0";
        syslog(LOG_DEBUG, "DEBUG: skip comment (%d) |%s|", remark, line);
        // exit(EXIT_FAILURE);
    }
    // } else {

    // get sections
    if ((strchr(line, (int) '[')!=NULL)&&(strchr(line,(int)']')!=NULL)) {
        char tmpLine[40]="\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
        strncpy(tmpLine, (strchr(line, (int)'[')+1), ((strchr(line,(int)']')-strchr(line, (int) '[')-1)) );
        syslog(LOG_DEBUG, "DEBUG: Section: \t|%s|", tmpLine);
        exit(EXIT_SUCCESS);
    }
    // get values
    if (strchr(line,(int)'=')) {
        ptr = strtok(line, "=");
        ptr2 = strtok(NULL, "=");
        syslog(LOG_DEBUG, "DEBUG:  Value: \"%s\" is set to \"%s\" |%s|", ptr, ptr2, line);
        exit(EXIT_SUCCESS);
    }

    //  }
     exit(EXIT_SUCCESS);
}
