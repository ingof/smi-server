/* command.h */
#ifndef COMMAND_H
#define COMMAND_H

extern COMMAND command[1];
int fdSMI[MAX_SMI_PORTS];
int fdSWB[MAX_SWB_PORTS];

/* functions */
int handleCommand(void);

#endif /* command.h */
