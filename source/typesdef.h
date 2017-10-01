/* typesdef.h */
#ifndef TYPESDEF_H
#define TYPESDEF_H


typedef unsigned char uint8_t;
typedef unsigned int  unint16_t;

typedef enum {SWB,SMI} controlType;
typedef enum {AUTO,MANUAL} buttonStatus;

typedef struct {
    char name[30];
    int bus;
    int smiId;
    char smiAddr[9];
    char switchAddr[5];
    unsigned int direction;
    controlType control;
    unsigned int actualPos;
    unsigned int storedPos[2];
} DRIVE;

typedef struct {
    char name[30];
    char switch_addr[4];
    buttonStatus status;
} BUTTON;


#endif /* typesdef.h */
