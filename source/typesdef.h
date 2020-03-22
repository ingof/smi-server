/* typesdef.h */
#ifndef TYPESDEF_H
#define TYPESDEF_H

#define TRUE 1
#define YES 1
#define FALSE 0
#define NO 0

#define MAX_ITEMS 29

#include "configuration.h"	/* global configuration headder */

typedef unsigned char uint8_t;
typedef unsigned int  unint16_t;

// struct checkedUnion {
//   int type;  // Variable zum Speichern des in der Union verwendeten Datentyps
//   union intFloat {
//    int i;
//    float f;
//   } intFloat1;
//  };
//
// typedef struct {
//   int type;  // Variable zum Speichern des in der Union verwendeten Datentyps
//   union intFloat {
//    int i;
//    float f;
//   } intFloat1;
// } CHECKDUNION;

typedef struct {
    char name[MAX_ITEMS];
    char openHABItem[MAX_ITEMS];        // openHAB2 items
    int bus;
    int id;
    unsigned int addr;
    //unsigned int switchAddr; //drive[x].switchAddr
    union {
        unsigned int Addr;      //drive[x].switchAddr.Addr
        // struct {
        //     unsigned char low;  //drive[x].switchAddr.Byte[0] ?????
        //     unsigned char high; //drive[x].switchAddr.Byte[1] ?????
        // } Byte[2];
        struct {
            unsigned char Low;    // drive[x].switchAddr.Byte.Low
            unsigned char High;   // drive[x].switchAddr.Byte.High
        } Byte;
    } switchAddr;
    union {
        unsigned int Addr;      //drive[x].switchAddr2.Addr
        // struct {
        //     unsigned char low;  //drive[x].switchAddr2.Byte[0] ?????
        //     unsigned char high; //drive[x].switchAddr2.Byte[1] ?????
        // } Byte[2];
        struct {
            unsigned char Low;    // drive[x].switchAddr2.Byte.Low
            unsigned char High;   // drive[x].switchAddr2.Byte.High
        } Byte;
    } switchAddr2;
    unsigned int direction;  // cardinal direction of window
    int controlSwb;     // TRUE if communikation via SWB-Bus
    unsigned int actualPos;
    unsigned int storedPos[2];
} DRIVE;

typedef struct {
    char name[MAX_ITEMS];
    char openHABItem[MAX_ITEMS];  // openHAB2 items
    union {
        unsigned int Addr;        // group[x].switchAddr.Addr
        struct {
            unsigned char Low;    // group[x].switchAddr.Byte.Low
            unsigned char High;   // group[x].switchAddr.Byte.High
        } Byte;
    } switchAddr;
    union {
        unsigned int Addr;        // group[x].switchAddr2.Addr
        struct {
            unsigned char Low;    // group[x].switchAddr2.Byte.Low
            unsigned char High;   // group[x].switchAddr2.Byte.High
        } Byte;
    } switchAddr2;
    unsigned int smiID[MAX_SMI_PORTS];
    unsigned int driveID[MAX_DRIVES / 16];
} GROUP;


// typedef struct {
//     char name[30];
//     int bus;
//     int id;
//     unsigned int addr;
//     unsigned int switchAddr;
//     unsigned int direction;
//     int controlSwb;     // TRUE if communikation via SWB-Bus
//     unsigned int actualPos;
//     unsigned int storedPos[2];
// } DRIVE;

typedef struct {
    char name[MAX_ITEMS];
    unsigned int addr;
    int automatic;
} BUTTON;

typedef struct {
    int id;
    int group;
    int command;
    int degree;
    int position;
} COMMAND;


#endif /* typesdef.h */
