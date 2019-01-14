/* parseconf.h */
#ifndef PARSECONF_H
#define PARSECONF_H

extern DRIVE drive[MAX_DRIVES];
extern GROUP group[MAX_GROUPS];
extern BUTTON button[MAX_BUTTONS];
// extern int tcpWEB=8088;
extern char serialSMI[MAX_SMI_PORTS][40];
extern char serialSWB[MAX_SWB_PORTS][40];
extern int serialSwbAck[MAX_SWB_PORTS];
extern long timeoutSMI;
extern long timeoutSWB;
extern int tcpControlPort;
extern int openHABPort;
extern char openHABGet[30];
extern char openHABHost[30];



/* functions */
void initData(void);

int parseConfFile(void);
int parseConfLine(char* line);

int setSection(char* section);
int setValue(char* name, char* value);
int setInterface(char* name, char* value);
int setSwitch(char* name, char* value);
int setDrive(char* name, char* value);
int setGeneral(char* name, char* value);

#endif /* parseconf.h */
