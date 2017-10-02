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
    int id;
    unsigned int addr;
    unsigned int switchAddr;
    unsigned int direction;
    controlType control;
    unsigned int actualPos;
    unsigned int storedPos[2];
} DRIVE;

typedef struct {
    char name[30];
    unsigned int addr;
    buttonStatus status;
} BUTTON;


#endif /* typesdef.h */
