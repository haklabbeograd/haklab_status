#ifndef __BOARD_H__
#define __BOARD_H__

#include "nRF24L01.h"
#include "RF24.h"

//typedef enum {BOOL, U_CHAR, S_CHAR, U_INT16, S_INT16, UL_INT32, SL_INT32,FLOAT, DOUBLE } Format;
typedef enum {BOOL, U_CHAR, S_CHAR, U_INT16, S_INT16, UL_INT32, SL_INT32,FLOAT, DOUBLE } DATATYPE;
typedef enum {SENSOR, ACTUATOR} S_OR_A;

extern RF24 radio;

#define MAX_N_BOARDS 8
#define MAX_N_SENACT 10
#define APPLICATION_CH 10
#define DEFINITION_CH  20
#define SIZE_OF_NAME 20
#define MAX_DATA_SIZE
#define PAYLOAD_SIZE 32

struct senact
{
    char name[SIZE_OF_NAME];
    DATATYPE type;
    unsigned char nData;
    S_OR_A  sOrA;
    unsigned char nSA;
    byte lastReading[MAX_DATA_SIZE];
};

typedef struct senact SenAct;

struct board
{
    char name[SIZE_OF_NAME];
    unsigned long long address;
    unsigned char channel;
    unsigned char nSenAct;
    SenAct * arraySenAct;
};

typedef struct board Board;

extern  Board Boards[MAX_N_BOARDS];
extern  SenAct SenActs[MAX_N_SENACT];
extern  unsigned char nBoards;
extern  unsigned char nSenActs;

boolean readSensorB(SenAct * theSenAct);
boolean readAllSonBoard(Board * theBoard);
boolean readPackage(void * package,unsigned char len);
boolean readPackageAck(byte * package, unsigned char len, byte * ack, unsigned char lenAck);
boolean writePackage(void * package, unsigned char len);
void packBoard(Board theBoard, byte * package);
void unpackBoard(Board* theBoard, byte * package);
void packSenAct(SenAct theSenAct, byte * package);
void unpackSenAct(SenAct* theSenAct, byte * package);
boolean unregBoardAvailable();
boolean newBoardConnect();
boolean newBoardDefine();


#endif
