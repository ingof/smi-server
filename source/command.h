/* command.h */
#ifndef COMMAND_H
#define COMMAND_H

// #include "configuration.h"

extern COMMAND command[1];
extern DRIVE drive[MAX_DRIVES];
extern GROUP group[MAX_GROUPS];
extern BUTTON button[MAX_BUTTONS];
extern int fdSMI[MAX_SMI_PORTS];
extern int fdSWB[MAX_SWB_PORTS];
extern unsigned char swbTxBuffer[50];
extern unsigned char swbTxSize;
extern unsigned char smiRxBuffer[MAX_SWB_PORTS][50];
extern unsigned char smiTxBuffer[50];
extern unsigned char smiTxSize;
extern unsigned char buffer[12];

/* functions */
int handleCommand(void);
int sendSmiCmd(int swbAddr, int smiCmd);
int getGrpPositons(int grpID);
int getDrvPosition(int drvId);

#endif /* command.h */
