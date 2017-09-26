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
char *ptr;
int tmp;

int main() {

 tmp=parseConfFile();
 return(0);
}

    int parseConfFile(void) {
    fp = fopen ("/var/packages/smi-server/target/share/smi-server.conf\n", "r");
    if (fp == NULL) {
        syslog(LOG_ERR, "ERROR: Could not open configuration file");
        return 1;
    } else {
        while((fscanf(fp,"%s\n",line)) != EOF) {
            tmp=parseConfLine(&line);
        }
    }
    printf("\tline: |%s|",line);
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
    printf ("Bereich:\n\t%s\n\n", tmpLine);
    }

  // get values
  if (strchr(line,(int)'=')) {
    printf ("Wert:\n");
    ptr = strtok(line, "=");
    if (line[0]!=35) {
      while(ptr != NULL) {
      // naechsten Abschnitt erstellen
      printf("\t> %s\n", ptr);
      ptr = strtok(NULL, "=");
      }
      printf("\n");
    }
  }

  // Get values
return 0;
}
