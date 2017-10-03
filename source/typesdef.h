/* typesdef.h */
#ifndef TYPESDEF_H
#define TYPESDEF_H


typedef unsigned char uint8_t;
typedef unsigned int  unint16_t;


typedef struct {
    char name[30];
    int bus;
    int id;
    unsigned int addr;
    unsigned int switchAddr;
    unsigned int direction;
    int controlSwb;
    unsigned int actualPos;
    unsigned int storedPos[2];
} DRIVE;

typedef struct {
    char name[30];
    unsigned int addr;
    int automatic;
} BUTTON;


#endif /* typesdef.h */
