/* typesdef.h */
#ifndef TYPESDEF_H
#define TYPESDEF_H


typedef unsigned char uint8_t;
typedef unsigned int  unint16_t;

typedef enum {SWB,SMI} controlType;

typedef struct {
    char name[30];
    int bus;
    int smi_id;
    char smi_addr[8];
    char switch_addr[4];
    int direction;
    controlType control;
} DRIVE;

typedef struct {
    char name[30];
    char switch_addr[4];
} BUTTON;


#endif /* typesdef.h */
