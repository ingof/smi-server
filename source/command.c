#include <syslog.h>			  /* syslog */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "configuration.h"
#include "parseconf.h"
#include "typesdef.h"          /* type definitions and include of global configuration */
#include "command.h"

#include "swb-serial.h"		   /* swb-bus functions */
#include "smi-serial.h"		   /* swb-bus functions */


int handleCommand(void) {
// int handleCommand(int port) {
    // TODO: transfer TX-Part to smi-serial.c or swb-serial.c
    int loop;
    struct timeval tmpTime;
    // syslog(LOG_DEBUG, "DEBUG: handle command (ID=%02d Group=%04d Command=%02d)", command[0].id, command[0].group, command[0].command);
    //TODO avoid using flush rx buffer
    // flushSwb(fdSWB[0]);
    //TODO: get configuration of swb id
    // int loop;
    // for (loop=0, loop < MAX_SWB_PORTS ,loop++) {
    //  ....
    // }
    //TODO: decide witch bus and handle communication
    // TODO: log message to syslog with send command

    if ((command[0].id < 0) && (command[0].group < 0)) {
      syslog(LOG_DEBUG, "DEBUG: ... keine gültige Adresse übergeben", command[0].id);
      return EXIT_FAILURE;
    }
    if (command[0].id > MAX_DRIVES) {
      syslog(LOG_DEBUG, "DEBUG: ... Motor ID %d ist zu groß", command[0].id);
      return EXIT_FAILURE;
    }
    if (command[0].group > MAX_GROUPS) {
      syslog(LOG_DEBUG, "DEBUG: ... Motor Gruppe %d ist zu groß", command[0].group);
      return EXIT_FAILURE;
    }

    // syslog(LOG_DEBUG, "DEBUG: (1) I:%d G:%d C:%d", command[0].id, command[0].group, command[0].command);

    if (command[0].group >= 0) {
      //syslog(LOG_DEBUG, "DEBUG: ... Gruppen werden noch nicht unterstützt", command[0].id);
      // TODO: add support of Groups
      int tmpReturn;
      char tmpMsg[60];
      int flagResponse = 0;
      int smiPort;
      // syslog(LOG_DEBUG, "DEBUG: (2) I:%d G:%d C:%d", command[0].id, command[0].group, command[0].command);

      if (command[0].command >= 0) {
        // syslog(LOG_DEBUG, "DEBUG: (3) I:%d G:%d C:%d", command[0].id, command[0].group, command[0].command);
        for (smiPort = 0; smiPort < MAX_SMI_PORTS; smiPort++) {
          // syslog(LOG_DEBUG, "DEBUG: 1.S:%d=%d I:%d G:%d C:%d", smiPort, group[command[0].group].smiID[smiPort], command[0].id, command[0].group, command[0].command);
          if (group[command[0].group].smiID[smiPort] > 0) {
            // syslog(LOG_DEBUG, "DEBUG: 2.S:%d=%d I:%d G:%d C:%d", smiPort, group[command[0].group].smiID[smiPort], command[0].id, command[0].group, command[0].command);
            smiTxBuffer[0] = 0x40 ;
            smiTxBuffer[1] = 0xC0 ;
            smiTxBuffer[2] = ((group[command[0].group].smiID[smiPort] / 256) & 0xff);
            smiTxBuffer[3] = (group[command[0].group].smiID[smiPort] & 0xff);
            smiTxBuffer[4] = command[0].command & 0x07;
            addSmiCrc(smiTxBuffer, 5);
            smiTxSize = 6;
            write(fdSMI[smiPort], &smiTxBuffer,smiTxSize);
            usleep(50000);
            // TODO: check ocho of sent bytes
            /* ignore sent bytes */
            readSmiByte(smiPort);
            readSmiByte(smiPort);
            readSmiByte(smiPort);
            readSmiByte(smiPort);
            readSmiByte(smiPort);
            readSmiByte(smiPort);
            // readSmiByte(drive[command[0].id].bus);
            for (loop = 0; loop < 10; loop++) {
                gettimeofday( &tmpTime, (struct timezone *) 0 );
                // write(fdSMI[drive[command[0].id].bus], &smiTxBuffer,smiTxSize);
                sprintf(tmpMsg, "DEBUG: %03d \e[1mSMI:%d -> %02x %02x %02x %02x %02x %02x ", (tmpTime.tv_usec/1000), smiPort, smiTxBuffer[0], smiTxBuffer[1], smiTxBuffer[2], smiTxBuffer[3], smiTxBuffer[4], smiTxBuffer[5] );
                usleep(5000);
                // TODO: check if response is ok
                tmpReturn = readSmiByte(smiPort);
                if (tmpReturn != -1) {
                  switch (tmpReturn) {

                    case 0xC0:  /* NACK (0xC0, 0xE0) */
                    case 0xD0:
                                strcat(tmpMsg, "\e[31m(NACK)\e[0m ");
                                break;
                    case 0xFE:  /* ACK (0xFE, 0xFF) */
                    case 0xFF:
                                strcat(tmpMsg, "\e[32m(ACK)\e[0m ");
                                break;
                    default:    strcat(tmpMsg, "\e[90m(?\?\?)\e[0m ");
                                break;

                  }
                  flagResponse = 1;
                }
                if (flagResponse == 1) {
                  break;
                }
            }
            if (flagResponse == 0) {
              strcat(tmpMsg, "\e[1m\e[90m(timeout)\e[0m ");
            }
            syslog(LOG_DEBUG, tmpMsg);
          }
        }
      }
      // uncaught exception
      command[0].id = -1;
      command[0].group = -1;
      command[0].command = -1;
      return(EXIT_FAILURE);
    }


    if (command[0].id >= 0) {
      if (drive[command[0].id].controlSwb==TRUE) {
          // use SWB-Bus for communication
          flushSwb(fdSWB[0]);
          swbTxBuffer[0]= 0xF0;
          swbTxBuffer[1]= drive[command[0].id].switchAddr.Byte.High;  // swbTxBuffer[1]= 0x24;
          swbTxBuffer[2]= drive[command[0].id].switchAddr.Byte.Low;   // swbTxBuffer[2]= 0x6B;
          swbTxBuffer[3]= 0x01;
          switch (command[0].command) {
              default:
              case 00:                            //STOP
              case 01:    swbTxBuffer[4]= 0x02;   //UP
                          break;
              case 02:    swbTxBuffer[4]= 0x12;   //DOWN
                          break;
          }
          swbTxSize=7;
          calcSwbCrc(swbTxBuffer, swbTxSize);
          for (loop = 0; loop < 5; loop++) {
              gettimeofday( &tmpTime, (struct timezone *) 0 );
              write(fdSWB[0],&swbTxBuffer,swbTxSize);
              syslog(LOG_DEBUG, "DEBUG: %03d CMD:  -> %02x %02x %02x %02x %02x %02x %02x", (tmpTime.tv_usec/1000), swbTxBuffer[0], swbTxBuffer[1], swbTxBuffer[2], swbTxBuffer[3], swbTxBuffer[4], swbTxBuffer[5], swbTxBuffer[6]);
              usleep(20000);          // wait 15ms for response
              // TODO: check if response is right!!!
              if (readSwb(fdSWB[0],0) == EXIT_SUCCESS) {
                  break;
              }
          }

          // syslog(LOG_DEBUG, "DEBUG: readSwb returned" );
          swbTxBuffer[0]= 0xF0;
          swbTxBuffer[1]= drive[command[0].id].switchAddr.Byte.High;  // swbTxBuffer[1]= 0x24;
          swbTxBuffer[2]= drive[command[0].id].switchAddr.Byte.Low;   // swbTxBuffer[2]= 0x6B;
          swbTxBuffer[3]= 0x81;
          switch (command[0].command) {
              default:
              case 00:    swbTxBuffer[4]= 0x03;   //STOP (up)
                          break;
              case 01:    swbTxBuffer[4]= 0x04;   //UP
                          break;
              case 02:    swbTxBuffer[4]= 0x14;   //DOWN
                          break;
              case 03:    swbTxBuffer[4]= 0x15;   //POS1
                          break;
              case 04:    swbTxBuffer[4]= 0x05;   //POS2
                          break;
              case 05:    swbTxBuffer[4]= 0x05;   //POS
                          //break;
          }
          swbTxSize=7;
          calcSwbCrc(swbTxBuffer, swbTxSize);
          for (loop = 0; loop < 5; loop++) {
              gettimeofday( &tmpTime, (struct timezone *) 0 );
              write(fdSWB[0],&swbTxBuffer,swbTxSize);
              syslog(LOG_DEBUG, "DEBUG: %03d CMD:  -> %02x %02x %02x %02x %02x %02x %02x", (tmpTime.tv_usec/1000), swbTxBuffer[0], swbTxBuffer[1], swbTxBuffer[2], swbTxBuffer[3], swbTxBuffer[4], swbTxBuffer[5], swbTxBuffer[6]);
              usleep(20000);          // wait 15ms for response
              // TODO: check if response is right
              if (readSwb(fdSWB[0],0) == EXIT_SUCCESS) {
                  break;
              }
          }
      } else { // controlSwb is False
        int tmpReturn;
        char tmpMsg[60];
        int flagResponse = 0;
        // TODO: use configured SMI-Bus
        // TODO: log message to syslog with send command
        // if ((command[0].id < 0) && (command[0].group < 0)) {
        //   syslog(LOG_DEBUG, "DEBUG: ... keine gültige Adresse übergeben", command[0].id);
        //   return EXIT_FAILURE;
        // }
        // if (command[0].id > MAX_DRIVES) {
        //   syslog(LOG_DEBUG, "DEBUG: ... Motor ID %d ist zu groß", command[0].id);
        //   return EXIT_FAILURE;
        // }
        // if (command[0].group > MAX_GROUPS) {
        //   syslog(LOG_DEBUG, "DEBUG: ... Motor Gruppe %d ist zu groß", command[0].group);
        //   return EXIT_FAILURE;
        // }
        // if (command[0].group >= 0) {
        //   syslog(LOG_DEBUG, "DEBUG: ... Gruppen werden noch nicht unterstützt", command[0].id);
        //   // TODO: add support of Groups
        //   return EXIT_FAILURE;
        // }
        if (command[0].command >= 0) {
          smiTxBuffer[0] = 0x50 + (drive[command[0].id].id & 0x0f);
          smiTxBuffer[1] = command[0].command & 0x07;
          addSmiCrc(smiTxBuffer, 2);
          // smiTxSize = 4;
          smiTxSize = 3;
          write(fdSMI[drive[command[0].id].bus], &smiTxBuffer,smiTxSize);
          usleep(50000);
          // TODO: check ocho of sent bytes
          /* ignore sent bytes */
          readSmiByte(drive[command[0].id].bus);
          readSmiByte(drive[command[0].id].bus);
          readSmiByte(drive[command[0].id].bus);
          // readSmiByte(drive[command[0].id].bus);
          for (loop = 0; loop < 10; loop++) {
              gettimeofday( &tmpTime, (struct timezone *) 0 );
              // write(fdSMI[drive[command[0].id].bus], &smiTxBuffer,smiTxSize);
              sprintf(tmpMsg, "DEBUG: %03d \e[1mSMI:%d -> %02x %02x %02x %02x ", (tmpTime.tv_usec/1000), drive[command[0].id].bus, smiTxBuffer[0], smiTxBuffer[1], smiTxBuffer[2], smiTxBuffer[3] );
              usleep(5000);
              // TODO: check if response is ok
              tmpReturn = readSmiByte(drive[command[0].id].bus);
              if (tmpReturn != -1) {
                switch (tmpReturn) {

                  case 0xC0:  /* NACK (0xC0, 0xE0) */
                  case 0xD0:
                              strcat(tmpMsg, "\e[31m(NACK)\e[0m ");
                              break;
                  case 0xFE:  /* ACK (0xFE, 0xFF) */
                  case 0xFF:
                              strcat(tmpMsg, "\e[32m(ACK)\e[0m ");
                              break;
                  default:    strcat(tmpMsg, "\e[90m(?\?\?)\e[0m ");
                              break;

                }
                flagResponse = 1;
              }
              if (flagResponse == 1) {
                break;
              }
          }
          if (flagResponse == 0) {
            strcat(tmpMsg, "\e[1m\e[90m(timeout)\e[0m ");
          }
          syslog(LOG_DEBUG, tmpMsg);
          if ((command[0].command & 0x07) == 0) {
            smiTxBuffer[0] = 0x70 + (drive[command[0].id].id & 0x0f);
            smiTxBuffer[1] = 0x05;
            addSmiCrc(smiTxBuffer, 2);
            smiTxSize = 3;
            write(fdSMI[drive[command[0].id].bus], &smiTxBuffer,smiTxSize);
          }

        }
        // uncaught exception
        command[0].id = -1;
        command[0].group = -1;
        command[0].command = -1;
        return(EXIT_FAILURE);
      }
    }

    // clear command-buffer
    command[0].id = -1;
    command[0].group = -1;
    command[0].command = -1;
    return(EXIT_SUCCESS);
}

int sendSmiCmd(int swbAddr, int smiCmd) {
  struct timeval tmpTime;
  int loop;
  // search assigned drive(s) for given swb-address
  // search for smi bus id for assigned drive(s)
  for (loop = 0; loop <= MAX_DRIVES; loop++) {
    if ((drive[loop].switchAddr.Addr == swbAddr) || (drive[loop].switchAddr2.Addr == swbAddr)) {
      sendSmi(drive[loop].bus, drive[loop].id , smiCmd);
      // if (smiCmd == 0) {
      //   usleep(40000);
      //   gettimeofday( &tmpTime, (struct timezone *) 0 );
      //   syslog(LOG_INFO, "INFO : %03d SMI:  getPos %d,%d ", (tmpTime.tv_usec/1000), drive[loop].bus, drive[loop].id );
      //   sendSmiGetPos(drive[loop].bus, drive[loop].id);
      // }
      break;
    }

  // search assigned group(s) for given swb-address
  }
  // for (loop = 0; loop <= MAX_GROUPS; loop++) {
  for (loop = 0; loop <= 5; loop++) {
    if ((group[loop].switchAddr.Addr == swbAddr) || (group[loop].switchAddr2.Addr == swbAddr)) {
      sendSmiGrp(loop, smiCmd);
      // TODO: Loop for every send command to get Positions
      break;
    }
  }
  return EXIT_SUCCESS;
}

int getGrpPositons(int grpID) {
  //SMI bus 0
  // Loop for all ids on smi bus 0
    // send "Get Position"
    // wait for response
    // set PositionsArray
  return EXIT_SUCCESS;
}

int getDrvPosition(int drvId) {
  return EXIT_SUCCESS;
}
