#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parseconf.h"
#include "typesdef.h"   /* type definitions */

char str1[40], str2[40];
char name[40], value[40];
const int lineMaxChar=80;
const int sectionMaxChar=20;
// char line[lineMaxChar];
// char confSection[sectionMaxChar];
char line[80];
char confSection[20];

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
        while((fgets(line,lineMaxChar,fp)) != NULL) {
            lineCnt++;
            tmp=parseConfLine(line);
        }
    }
    // syslog(LOG_DEBUG, "DEBUG: line: |%s|",line);
    fclose(fp);
    return 0;
}

int parseConfLine(char* line) {
    int remark;
    // remark-line cut line at "#" char
    remark=strchr(line,(int)'#');
    if ( remark != NULL ) {
        // TODO only ignore comments an not the whole line
        // skip comments
        return(EXIT_SUCCESS);
    }
    // get sections
    if ((strchr(line, (int) '[')!=NULL)&&(strchr(line,(int)']')!=NULL)) {
        char tmpLine[40]="\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
        strncpy(tmpLine, (strchr(line, (int)'[')+1), ((strchr(line,(int)']')-strchr(line, (int) '[')-1)) );
        tmp=setSection(tmpLine);
        // syslog(LOG_DEBUG, "DEBUG: Section: \t|%s|", tmpLine);
        return(EXIT_SUCCESS);
    }
    // get values
    if (strchr(line,(int)'=')) {
        ptr = strtok(line, "=");
        ptr2 = strtok(NULL, "=");
        setValue(ptr, ptr2);
        // syslog(LOG_DEBUG, "DEBUG:  Value: \"%s\" is set to \"%s", ptr, ptr2);
        return(EXIT_SUCCESS);
    }
    return(EXIT_SUCCESS);
}

int setSection(char* section) {
    strncpy(confSection,section,sectionMaxChar);
    return(EXIT_SUCCESS);
}

int setValue(char* name, char* value) {
    syslog(LOG_DEBUG,"DEBUG: %4d: Section: %s,\tName: %s,\tValue: %s",lineCnt, confSection, name, value);
    char* sectionName[20];
    char* sectionNumber[20];
    sectionName = strtok(name, ":");
    sectionNumber = strtok(NULL, ":");

    switch (sectionName) {
        case "interface":
            setInterface(name, value);
            break;

        case "switch:0","switch:1","switch:2","switch:3","switch:4","switch:5","switch:6","switch:7" :
            setSwitch(name, value);
            break;

        case "drive:0","drive:1","drive:2","drive:3","drive:4","drive:5","drive:6","drive:7" :
            setDrive(name, value);
            break;

        default:
            syslog(LOG_NOTICE, "NOTICE: unknown section: \"%s\"#\"%s\"", sectionName, sectionNumber);
            break;
    }
    return(EXIT_SUCCESS);
}

int setInterface(char* name, char* value) {
    switch (name) {
        case "smi0":
            break;
        case "smi1":
            break;
        case "swb0":
            break;
        default:
            syslog(LOG_NOTICE, "NOTICE: unknown interface type: \"%s\"", name);
            break;
    }
    return(EXIT_SUCCESS)
};

int setSwitch(char* name, char* value) {
    char* tmpName[20];
    char* tmpNumber[20];
    tmpName = strtok(name, ":");
    tmpNumber = strtok(NULL, ":");

    syslog(LOG_NOTICE, "DEBUG: s.%s[%s]", tmpName, tmpNumber);

    return(EXIT_SUCCESS)
};

int setDrive(char* name, char* value) {
    char* tmpName[20];
    char* tmpNumber[20];
    tmpName = strtok(name, ":");
    tmpNumber = strtok(NULL, ":");

    syslog(LOG_NOTICE, "DEBUG: d.%s[%s]", tmpName, tmpNumber);

    return(EXIT_SUCCESS)
};
