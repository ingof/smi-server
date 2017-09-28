/* parseconf.h */
#ifndef PARSECONF_H
#define PARSECONF_H


/* functions */
int parseConfFile(void);
int parseConfLine(char* line);

int setSection(char* section);

int setValue(char* name, char* value);

int setInterface(char* name, char* value);
int setSwitch(char* name, char* value);
int setDrive(char* name, char* value);

#endif /* parseconf.h */
