/* parseconf.h */
#ifndef PARSECONF_H
#define PARSECONF_H

extern DRIVE drive[16];
extern BUTTON button[32];
// extern int tcpWEB=8088;
extern char* serialSmi1[40];
extern char* serialSmi2[40];
extern char* serialSmi3[40];
extern char* serialSwb[40];
extern int tcpWEB;



/* functions */
int parseConfFile(void);
int parseConfLine(char* line);

int setSection(char* section);

int setValue(char* name, char* value);

int setInterface(char* name, char* value);
int setSwitch(char* name, char* value);
int setDrive(char* name, char* value);

#endif /* parseconf.h */
