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
#include "RF24.h"

/**
 * Sensor/Actuator value datatype
 *
 * For use with SenAct structure
 */
typedef enum {BOOL,U_CHAR, S_CHAR, U_INT16, S_INT16, UL_INT32, SL_INT32,FLOAT, DOUBLE, CHAR_ARRAY } DATATYPE;

/**
 * Sensor/Actuator  definition
 * 
 * For use with SenAct structure
 */
typedef enum {SENSOR, ACTUATOR} S_OR_A;

#define REGISTRATION_CH 10      /**< Registration Channel */
#define DEFINITION_CH  20       /**< Definition Channel */
#define SIZE_OF_NAME    20      /**< Maximum size of name array */
#define MAX_SIZE_OF_DATA 8      /**< Maximum number of byte for data */
#define PAYLOAD_SIZE 32         /**< Maximum payload size */

  /**
   * Initialises the Board and all it's library components.
   * 
   * Serial and RF24/radio initialisation. Maps the free RF channels
   */
void initialiseBoard(void);

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
   * Registers the board to the network
   * 
   * Keeps sending it's boardPack on the registration channel
   * and waits for the master to send it back. If the same
   * packet is receved it moves to definition
   * 
   * @return        True if succesfull, fail if not
   * @warning       uses globals boardPack[32], and changes channels
   */
boolean registerBoard(void);

  /**
   * Deffines the board and it's Sensor/Actuators to the master
   * 
   * Sends out the Sensor/Actuator information upon masters
   * request (commads 0x00-0xEF). Receves the new network channel
   * from master (command 0xF0). Connects to the new channel and 
   * network (commmand 0xFF)
   * 
   * @return        True if succesfull, fail if not
   * @warning       uses globals Channel, Sensors[][] and changes channels
   */
boolean defineBoard(void);

/**
   * Connects the board to the sensor network
   * 
   * Simple logic which uses the registerBoard, and defineBoard
   * functions
   * 
   * @return        True if succesfull, fail if not
   * @warning       uses global registered
   */
boolean registerAndDefineBoard(unsigned long *timerA);

/**
   * Checks if a new command was receved and stores it
   *  
   * @param command     where to save the command
   * @return            True if succesfull, fail if not
   */
boolean commandReceved(byte * command);

/**
   * Executes the command
   *  
   * @param command     the command to execute
   * @return            True if succesfull, fail if not
   */
boolean parseCommand(byte command);

/**
   * Packs all the sensor readins into the Value[nSenActs][8] byte array
   * 
   * @return            True if succesfull, fail if not
   * @warning           Changes the byteValue[][] global
   */
boolean packAllSensor();

extern byte Value[2][8];
float readSensor1();
float readSensor2();

#endif
