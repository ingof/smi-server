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

//   fp = fopen ("smi-server.conf\n", "w+");
//   fputs("global=1\n", fp);
// //  fputs("# comment\n", fp);
//   fputs("MaxMuster@mail.de\n", fp);
//   fputs("[Person]\n", fp);
//   fputs("mail=Max.Mustermann@mail.de\n", fp);
//   //fputs("mail =Max.Mustermann@mail.de\n", fp);
//   //fputs("mail= Max.Mustermann@mail.de\n", fp);
//   //fputs("mail = Max.Mustermann@mail.de\n", fp);
//   fputs("name=Max\n", fp);
//   fputs(" surname=Mustermann\n", fp);
//   fputs("[car]\n", fp);
//   fputs(" name=Ferrari\n", fp);
//   fputs("model=F40\n", fp);
  // rewind(fp);
//   system("clear");
//   puts("\n\n");
 tmp=parseConfFile();
 return(0);
}

int parseConfFile(void) {
  fp = fopen ("smi-server.conf\n", "r");
  while((fscanf(fp,"%s\n",line)) != EOF) {
   tmp=parseConfLine(&line);
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
    // printf("\n\t |%s| ([#] at %s)\n",line,remark);
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
