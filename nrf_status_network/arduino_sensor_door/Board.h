#ifndef __BOARD_H__
#define __BOARD_H__

#include "RF24.h"

typedef enum {BOOL,U_CHAR, S_CHAR, U_INT16, S_INT16, UL_INT32, SL_INT32,FLOAT, DOUBLE } DATATYPE;
typedef enum {SENSOR, ACTUATOR} S_OR_A;

#define REGISTRATION_CH 10
#define DEFINITION_CH  20
#define SIZE_OF_NAME    20
#define MAX_SIZE_OF_DATA 8
#define PAYLOAD_SIZE 32

#define BOARD_NAME          "Door Lock"
#define BOARD_NSENACT       1

#define SENSOR1_NAME        "Lock"
#define SENSOR1_DATATYPE    BOOL
#define SENSOR1_NDATA       1
#define SENSOR1_NSA         0
#define SENSOR1_SORA        SENSOR


void initialiseBoard(void);
boolean readPackage(void * package,unsigned char len);
boolean readPackageAck(byte * package, unsigned char len, byte * ack, unsigned char lenAck);
boolean writePackage(void * package, unsigned char len);
void packBoard(void);
void packSenAct(unsigned char n);
boolean registerBoard(void);
boolean defineBoard(void);
boolean registerAndDefineBoard(unsigned long *timerA);
boolean commandReceved(byte * command);
boolean parseCommand(byte command);
boolean packValue(void * boardValue, unsigned char nBytes, unsigned char index);

#endif
