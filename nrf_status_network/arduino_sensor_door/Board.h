#ifndef __BOARD_H__
#define __BOARD_H__

#include "RF24.h"

typedef enum {BOOL,U_CHAR, S_CHAR, U_INT16, S_INT16, UL_INT32, SL_INT32,FLOAT, DOUBLE } DATATYPE;
typedef enum {SENSOR, ACTUATOR} S_OR_A;

#define REGISTRATION_CH 10
#define DEFINITION_CH  20
#define SIZE_OF_NAME    20
#define MAX_SIZE_OF_DATA 8

#define BOARD_NAME          "Door Lock"
#define BOARD_NSENACT       1

#define SENSOR1_NAME        "Lock"
#define SENSOR1_DATATYPE    BOOL
#define SENSOR1_NDATA       1
#define SENSOR1_NSA         0
#define SENSOR1_SORA        SENSOR

extern bool value[BOARD_NSENACT];
extern byte Value[BOARD_NSENACT][MAX_SIZE_OF_DATA];

boolean readPackage(void * package,unsigned char len, RF24 radioX);
boolean readPackageAck(byte * package, unsigned char len, byte * ack, unsigned char lenAck, RF24 radioX);
boolean writePackage(void * package, unsigned char len, RF24 radioX);
void packBoard();
void packSenAct(unsigned char n);
boolean registerBoard(RF24 radioX);
boolean defineBoard(boolean * registered, RF24 radio);
boolean registerAndDefineBoard(unsigned long *timerA, RF24 radioX);
byte commandReceved(RF24 radioX);
boolean parseCommand(byte command, RF24 radioX);
boolean packValue(void * boardValue, unsigned char nBytes, byte * sendValue);

#endif
