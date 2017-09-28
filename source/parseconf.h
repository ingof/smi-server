/* parseconf.h */
#ifndef PARSECONF_H
#define PARSECONF_H


/* functions */
int parseConfFile(void);
int parseConfLine(char* line);
int setSection(char* section);
int setValue(char* name, char* value);

#endif /* parseconf.h */
