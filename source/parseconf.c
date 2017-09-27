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
    fp = fopen (configFile, "r");
    if (fp == NULL) {
        syslog(LOG_ERR, "ERROR: Could not open configuration file");
        return 1;
    } else {
        syslog(LOG_DEBUG, "DEBUG: parsing config file: %s", configFile);
        while((fscanf(fp,"%s\n",line)) != EOF) {
            tmp=parseConfLine(&line);
        }
    }
    syslog(LOG_DEBUG, "\tline: |%s|",line);
    fclose(fp);
    return 0;
}

int parseConfLine(char* line) {
  char* remark;// initialisieren und ersten Abschnitt erstellen
  // int oldLine;// initialisieren und ersten Abschnitt erstellen

  // printf(" Text     : %s (%d)\n", line, line[0]);
  // strcpy(oldLine, line);

  if (strchr(line,(int)'#')) {
  // remark-line cut line at "#" char
    remark=strchr(line, 35); //35="#"
    if ( remark != NULL ) {
      return 0;
    }
  }

  if ((strchr(line, (int) '[')!=NULL)&&(strchr(line,(int)']')!=NULL)) {
    char tmpLine[40]="\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
    strncpy(tmpLine, (strchr(line, (int)'[')+1), ((strchr(line,(int)']')-strchr(line, (int) '[')-1)) );
    syslog(LOG_DEBUG, "\tSection: %s\n", tmpLine);
    }

  // get values
  if (strchr(line,(int)'=')) {
    // syslog(LOG_DEBUG, "Value  :");
    ptr = strtok(line, "=");
    ptr2 = strtok(NULL, "=");
    // if (line[0]!=35) {
    //   while(ptr != NULL) {
      // naechsten Abschnitt erstellen
      syslog(LOG_DEBUG, "\tValue  : \"%s\" : \"%s \"", ptr);
    //   ptr2 = strtok(NULL, "=");
    //   }
    // }
  }

  // Get values
return 0;
}
