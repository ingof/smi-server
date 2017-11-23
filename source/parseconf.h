/* parseconf.h */
#ifndef PARSECONF_H
#define PARSECONF_H

extern DRIVE drive[16];
extern BUTTON button[32];
// extern int tcpWEB=8088;
extern char serialSmi[3][40];
extern char serialSwb[1][40];
extern int tcpControl;



/* functions */
int parseConfFile(void);
int parseConfLine(char* line);

int setSection(char* section);

int setValue(char* name, char* value);

int setInterface(char* name, char* value);
int setSwitch(char* name, char* value);
int setDrive(char* name, char* value);

#endif /* parseconf.h */
