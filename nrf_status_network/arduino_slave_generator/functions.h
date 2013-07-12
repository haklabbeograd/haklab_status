#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_SIZE_OF_NAME 20 

/**
 * Sensor/Actuator value datatype
 *
 * For use with SenAct structure
 */
typedef enum {BOOL, U_CHAR, S_CHAR, U_INT16, S_INT16, UL_INT32, SL_INT32,FLOAT, DOUBLE } DATATYPE;

/**
 * Sensor/Actuator  definition
 * 
 * For use with SenAct structure
 */
typedef enum {SENSOR, ACTUATOR} S_OR_A;

/**
 * Sensor/Actuator stucture
 */
typedef struct senact
{
    char name[MAX_SIZE_OF_NAME+1];      /**< Name of sensor/actuator */
    DATATYPE type;                      /**< Datatype of sensor/actuator value */
    char typeS[20];
    unsigned char nData;                /**< Number of bytes for value sensor/actuator */
    S_OR_A  sOrA;                       /**< Sensor or Actuator */
    unsigned char nSA;                  /**< Number of sensor on Board */
    
}SenAct;

//typedef struct senact SenAct;

/**
 * Board stucture
 */
typedef struct board
{
    char name[MAX_SIZE_OF_NAME+1];    /**< Name of Board */
    unsigned char nSenAct;      /**< Number of Sensor/Actuator on Board */
}Board;

//typedef struct board Board;

extern Board theBoard;
extern SenAct theSenActs[100];
extern unsigned char nSenActs;
void typeSwitch(DATATYPE *theType);
void writeArduinoMain(FILE *fpArduinoMain, char *type, unsigned int nData, int i);
void boardPack(FILE * fp);
void sensorPack(FILE * fp, FILE * fpArduinoMain, unsigned int nSA );
void BoardGenerate();
void SensorGenerate(int index);
void printReadFunctionsH(FILE *fp);
void printReadFunctionsC(FILE *fp);
void writePackAllS(FILE *fp);
void copyFile(FILE *source, FILE *destination);
#endif
