/*
*   This program is free software; you can redistribute it and/or
*   modify it under the terms of the GNU General Public License
*   version 2 as published by the Free Software Foundation.  
*/

/**
 *  @file Board.h
 * 
 *  Function, defines, enums, and global declarations for network
 */

#ifndef __BOARD_H__
#define __BOARD_H__

//required headers from the RF24 library
#include "nRF24L01.h"
#include "RF24.h"

/**
 * Sensor/Actuator value datatype
 *
 * For use with SenAct structure
 */
typedef enum {BOOL, U_CHAR, S_CHAR, U_INT16, S_INT16, UL_INT32, SL_INT32,FLOAT, DOUBLE, CHAR_ARRAY } DATATYPE;

/**
 * Sensor/Actuator  definition
 * 
 * For use with SenAct structure
 */
typedef enum {SENSOR, ACTUATOR} S_OR_A;

//Board specific defines
#define MAX_N_BOARDS 8
#define MAX_N_SENACT 10
#define APPLICATION_CH 10
#define DEFINITION_CH  20
#define SIZE_OF_NAME 20
#define MAX_DATA_SIZE   32
#define PAYLOAD_SIZE 32



/**
 * Sensor/Actuator stucture
 */
struct senact
{
    char name[SIZE_OF_NAME];        /**< Name of sensor/actuator */
    DATATYPE type;                  /**< Datatype of sensor/actuator value */
    unsigned char nData;            /**< Number of bytes for value sensor/actuator */
    S_OR_A  sOrA;                   /**< Sensor or Actuator */
    unsigned char nSA;              /**< Number of sensor on Board */
    byte lastReading[MAX_DATA_SIZE];/**< Last read value of sensor/actuator */
};

typedef struct senact SenAct;

/**
 * Board stucture
 */
struct board
{
    char name[SIZE_OF_NAME];    /**< Name of Board */
    unsigned long long address; /**< Address of Board */
    unsigned char channel;      /**< RF channel of Board */
    unsigned char nSenAct;      /**< Number of Sensor/Actuator on Board */
    SenAct * arraySenAct;       /**< array of Sensors/Actuators connected to the Board */
};

typedef struct board Board;
//Board specific globals ***will be converted to board.cpp globals only
extern  Board Boards[MAX_N_BOARDS];     /**< Array/Buffer of all connected boards to the Network */
extern  SenAct SenActs[MAX_N_SENACT];   /**< Array/Buffer of all connected Sensor/Actuators to the Network */
extern  unsigned char nBoards;          /**< Number of boards currently connected to the Network */
extern  unsigned char nSenActs;         /**< Number of Sensor/Actuators currently connected to the Network */
extern  unsigned char freeCH[127];      /**< Array of free RF channels */
extern  unsigned char nFreeCH;          /**< number of free RF channels */
extern  unsigned char nextFreeCH;       /**< next free RF channel, index for freeCH[] */

  /**
   * Initialises the Board and all it's library components.
   * 
   * Serial and RF24/radio initialisation. Maps the free RF channels
   */
void initialiseBoard(void);

  /**
   * Maps the free RF channels
   * 
   * Goes through the RF channels one by one and checks if any transmiters
   * are present. Lasts ~3 seconds, each channel is scaned for 25ms
   * 
   * @warning   Uses/changes globals freeCH[], nFreeCH, nextFreeCH
   */
void mapFreeCH(void);

  /**
   * Packs a Board structure into a 32 byte array
   * 
   * @param theBoard    Which Board structure to pack
   * @param package     Array in which to pack it
   */
void packBoard(Board theBoard, byte * package);

  /**
   * Unpacks a 32 byte array into a Board structure
   * 
   * @param theBoard    Which Board structure to unpack to
   * @param package     Array of bytes from which to unpack
   */
void unpackBoard(Board* theBoard, byte * package);

  /**
   * Packs a SenAct structure into a 32 byte array
   * 
   * @param theSenAct   Which SenAct structure to pack
   * @param package     Array in which to pack it
   */
void packSenAct(SenAct theSenAct, byte * package);

  /**
   * Unpacks a 32 byte array into a SenAct structure
   * 
   * @param theSenAct   Which SenAct structure to unpack to
   * @param package     Array of bytes from which to unpack
   */
void unpackSenAct(SenAct* theSenAct, byte * package);

  /**
   * Reads an array of len bytes from the radio
   * 
   * @param package Where to store the package
   * @param len     how many bytes to read
   * @return        True if read succesfull, fail if not
   */
boolean readPackage(void * package,unsigned char len);

  /**
   * Reads an array of len bytes from the radio,
   * and check it against an ACK package
   * 
   * @param package Where to store the package
   * @param len     how many bytes to read
   * @param ack     ACK package
   * @param lenAck  how many of ACK bytes to read
   * @return        True if read succesfull and ACK passed, fail if not
   */
boolean readPackageAck(byte * package, unsigned char len, byte * ack, unsigned char lenAck);

  /**
   * Writes an array of len bytes to the radio
   * 
   * @param package Which array of bytes to write
   * @param len     how many bytes to write
   * @return        True if write succesfull, fail if not
   */
boolean writePackage(void * package, unsigned char len);

  /**
   * Reads a Sensor value from a secific Sensor/Actuator on Board
   * 
   * @param theSenAct   Which Sensor/Actuator to read
   * @return            True if reading is succesfull, fail if not
   */
boolean readSensorB(SenAct * theSenAct);

  /**
   * Reads all Sensor values from from a Board
   * 
   * @param theBoard    Which Board to read from
   * @return            True if all readings are succesfull, fail if not
   */
boolean readAllSonBoard(Board * theBoard);

/**
 * Checks the Registration channel if any boards are available on it.
 * 
 * If there is one avalable reads and writes back their name to them.
 * 
 * @return True if both read and subsequent write are succesfull, fail if not
 * @warning uses/changes global newBoardPacked[]
 */ 
boolean newBoardAvailable();

/**
 * Connects to the board on the Deffinition channel
 * 
 * @return True if the connection is succesfull, fail if not
 * @warning uses/changes global Boards[]
 */ 
boolean newBoardConnect();

/**
 * Defines all the Sensor/Actuators on board and gives the board the 
 * next free channel to which it connects.
 * 
 * Also checks if the last registered board was allready connected to the network
 * and if it was, it gives it back it's channel and reconnects it
 * 
 * @return True if the connection is succesfull, fail if not
 * @warning uses/changes globals Boards[], SenActs[], nBoards, nSenActs, freeCH[], nFreeCH, nextFreeCH
 */
boolean newBoardDefine();

/**
 * Checks if the board was allready connected to the network
 * 
 * @param theBoard  Board to check
 * @param index     where the Boards[] index is stored if Board was allready connected
 * @return          True if board was allready connected to network
 */ 
boolean alreadyRegistared(Board *theBoard,int *index);

/**
 * Reconnects the board back to it's slot in the network
 * 
 * @param theBoard  Board to reconnect
 * @param index     where in the Boards[] index should it reconnect
 * @return          True if board reconnects
 */ 
boolean returnBoardToNetwork(Board *theBoard, int index);

#endif
