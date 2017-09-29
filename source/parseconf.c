#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parseconf.h"
#include "typesdef.h"   /* type definitions */

char str1[40], str2[40];
char name[40], value[40];
define LINE_MAX_CHAR 80;
define SECT_MAX_CHAR 20;
// const int lineMaxChar=80;
// const int sectionMaxChar=20;
// char line[lineMaxChar];
// char confSection[sectionMaxChar];
char line[LINE_MAX_CHAR];
char confSectionName[SECT_MAX_CHAR];
int confSectionNumber=0;

FILE * fp;
char *ptr, *ptr2;
int tmp;
int lineCnt;

int parseConfFile(void) {
    char configFile[80]="/var/packages/smi-server/target/share/smi-server.conf";
    lineCnt=0;
    fp = fopen (configFile, "r");
    if (fp == NULL) {
        syslog(LOG_ERR, "ERROR: Could not open configuration file");
        return 1;
    } else {
        syslog(LOG_DEBUG, "DEBUG: parsing config file: %s", configFile);
        while((fgets(line,LINE_MAX_CHAR,fp)) != NULL) {
            lineCnt++;
            tmp=parseConfLine(line);
        }
    }
    // syslog(LOG_DEBUG, "DEBUG: line: |%s|",line);
    fclose(fp);
    return 0;
}

int parseConfLine(char* line) {

    // skip comments
    char tmpLine[LINE_MAX_CHAR]="";
    if (strchr(line, (int) '#') == line ) {
        strncpy(tmpLine, "  ", 1);
    }
    strncat(tmpLine, line, (LINE_MAX_CHAR-1));
    strtok(tmpLine, "#");
    strncpy(line,tmpLine, LINE_MAX_CHAR);

    // get sections
    if ((strchr(line, (int) '[')!=NULL)&&(strchr(line,(int)']')!=NULL)) {
        char tmpLine[80]="\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
        strncpy(tmpLine, (strchr(line, (int)'[')+1), ((strchr(line,(int)']')-strchr(line, (int) '[')-1)) );
        tmp=setSection(tmpLine);
        return(EXIT_SUCCESS);
    }

    // get values
    if ( strchr(line, (int) '=' ) ) {
        ptr = strtok(line, "=");
        ptr2 = strtok(NULL, "=");
        tmp=setValue(ptr, ptr2);
        return(EXIT_SUCCESS);
    }
    return(EXIT_SUCCESS);
}

int setSection(char* section) {
    char* sectionName;
    char* sectionNumber;
    sectionName = strtok(section, ":");
    sectionNumber = strtok(NULL, ":");
    // TODO handle section without number
    strncpy(confSectionName, sectionName, SECT_MAX_CHAR);
    confSectionNumber=atoi(sectionNumber);
    return(EXIT_SUCCESS);
}

int setValue(char* name, char* value) {
    if (strncmp(confSectionName,"interface", SECT_MAX_CHAR)==0) {
        setInterface(name, value);
    } else if (strncmp(confSectionName,"switch", SECT_MAX_CHAR)==0) {
        setSwitch(name, value);
    } else if (strncmp(confSectionName,"drive", SECT_MAX_CHAR)==0) {
        setDrive(name, value);
    } else {
        syslog(LOG_NOTICE, "NOTICE: %3d: unknown section: \"%s#%s\" , value:, \"%s#%s\"", lineCnt, confSectionName, confSectionNumber, name, value);
    }
    return(EXIT_SUCCESS);
}

int setInterface(char* name, char* value) {
    if (strncmp(name,"smi:0",SECT_MAX_CHAR)==0) {
        syslog(LOG_DEBUG, "DEBUG: %3d: interface: %s = %s", lineCnt, name, value);
    } else if (strncmp(name,"smi:1",SECT_MAX_CHAR)==0) {
        syslog(LOG_DEBUG, "DEBUG: %3d: interface: %s = %s", lineCnt, name, value);
    } else if (strncmp(name,"swb:0",SECT_MAX_CHAR)==0) {
        syslog(LOG_DEBUG, "DEBUG: %3d: interface: %s = %s", lineCnt, name, value);
    } else {
        syslog(LOG_NOTICE, "NOTICE: %3d: unknown interface type: %s", lineCnt, name);
    }
    return(EXIT_SUCCESS);
};

int setSwitch(char* name, char* value) {
    char* tmpName;
    tmpName = strtok(name, ":");
    syslog(LOG_DEBUG, "DEBUG: %3d:    switch%02d.%s = %s", lineCnt, confSectionNumber, tmpName, value);
    return(EXIT_SUCCESS);
};

int setDrive(char* name, char* value) {
    char* tmpName;
    tmpName = strtok(name, ":");
    syslog(LOG_DEBUG, "DEBUG: %3d:     drive%02d.%s = %s", lineCnt, confSectionNumber, tmpName, value);
    return(EXIT_SUCCESS);
};
