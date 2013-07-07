#ifndef __BOARD_H__
#define __BOARD_H__

#include "RF24.h"

typedef enum {BOOL,U_CHAR, S_CHAR, U_INT16, S_INT16, UL_INT32, SL_INT32,FLOAT, DOUBLE } DATATYPE;
typedef enum {SENSOR, ACTUATOR} S_OR_A;

#define APPLICATION_CH 10
#define DEFINITION_CH  20
#define SIZE_OF_NAME    20

#define BOARD_NAME          "DHT11"
#define BOARD_NSENACT       2

#define SENSOR1_NAME        "Humidity"
#define SENSOR1_DATATYPE    FLOAT
#define SENSOR1_NDATA       4
#define SENSOR1_NSA         0
#define SENSOR1_SORA        SENSOR

#define SENSOR2_NAME        "Temperature"
#define SENSOR2_DATATYPE    FLOAT
#define SENSOR2_NDATA       4
#define SENSOR2_NSA         1
#define SENSOR2_SORA        SENSOR






boolean readPackage(void * package,unsigned char len, RF24 radioX);
boolean readPackageAck(byte * package, unsigned char len, byte * ack, unsigned char lenAck, RF24 radioX);
boolean writePackage(void * package, unsigned char len, RF24 radioX);
void packBoard(byte * package);
void packSenAct(unsigned char n, byte * package);
boolean applyBoard(RF24 radioX);

#endif
