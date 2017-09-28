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
char confSectionName[20];
char confSectionNumber[20];

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
        tmp=setValue(ptr, ptr2);
        // syslog(LOG_DEBUG, "DEBUG:  Value: \"%s\" is set to \"%s", ptr, ptr2);
        return(EXIT_SUCCESS);
    }
    return(EXIT_SUCCESS);
}

int setSection(char* section) {
    char* sectionName;
    char* sectionNumber;
    sectionName = strtok(section, ":");
    sectionNumber = strtok(NULL, ":");
    strncpy(confSectionName,sectionName,sectionMaxChar);
    strncpy(confSectionNumber,sectionNumber,sectionMaxChar);
    syslog(LOG_INFO,"INFO: setSection: Name=\"%s\", value=\"%s\"", confSectionName, confSectionNumber)
    return(EXIT_SUCCESS);
}

int setValue(char* name, char* value) {
    syslog(LOG_DEBUG,"DEBUG: %4d: Section: %s:%s,\tName: %s,\tValue: %s",lineCnt, confSectionName, confSectionNumber, name, value);

    if (strncmp(confSectionName,"interface",20)==0) {
        setInterface(name, value);
    } else if (strncmp(confSectionName,"switch",20)==0) {
        setSwitch(name, value);
    } else if (strncmp(confSectionName,"drive",20)==0) {
        setDrive(name, value);
    } else {
        syslog(LOG_NOTICE, "NOTICE: unknown section: \"%s#%s\" , value:, \"%s#%s\"",confSectionName, confSectionNumber, name, value);
    }
    return(EXIT_SUCCESS);
}

int setInterface(char* name, char* value) {
    if (strncmp(name,"smi:0",20)==0) {
        syslog(LOG_DEBUG, "DEBUG: interface: \"%s\"", name);
    } else if (strncmp(name,"smi:1",20)==0) {
        syslog(LOG_DEBUG, "DEBUG: interface: \"%s\"", name);
    } else if (strncmp(name,"swb:0",20)==0) {
        syslog(LOG_DEBUG, "DEBUG: interface: \"%s\"", name);
    } else {
        syslog(LOG_NOTICE, "NOTICE: unknown interface type: \"%s\"", name);
    }
    return(EXIT_SUCCESS);
};

int setSwitch(char* name, char* value) {
    char* tmpName;
    char* tmpNumber;
    tmpName = strtok(name, ":");
    tmpNumber = strtok(NULL, ":");

    syslog(LOG_DEBUG, "DEBUG: s.%s[%s] = \"%s\"", tmpName, tmpNumber, value);

    return(EXIT_SUCCESS);
};

int setDrive(char* name, char* value) {
    char* tmpName;
    char* tmpNumber;
    tmpName = strtok(name, ":");
    tmpNumber = strtok(NULL, ":");

    syslog(LOG_DEBUG, "DEBUG: d.%s[%s] = \"%s\"", tmpName, tmpNumber, value);

    return(EXIT_SUCCESS);
};
