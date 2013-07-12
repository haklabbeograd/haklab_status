
#include "functions.h"

int main()
{
    FILE  *fpArduinoMain,*fpBoardH,*fpBoardC,*fpPrintH,*origArduinoMain,*newBoardH,*newBoardC,*newPrintH, *read_sensors_h;
    
    fpBoardH = fopen("Board.h","r");
    newBoardH = fopen("arduino_slave/Board.h","w");
    copyFile(fpBoardH, newBoardH);
    
    fpBoardH = fopen("arduino_slave/Board.h","a++");
    
    fpBoardC = fopen("Board.cpp","r");
    newBoardC = fopen("arduino_slave/Board.cpp","w");
    copyFile(fpBoardC, newBoardC);
    
    fpPrintH = fopen("printf.h","r");
    newPrintH = fopen("arduino_slave/printf.h","w");
    copyFile(fpPrintH, newPrintH);
    
    origArduinoMain = fopen("arduino_sensor_door.ino","r");
    fpArduinoMain = fopen("arduino_slave/arduino_slave.ino", "w");
    copyFile(origArduinoMain, fpArduinoMain);
    fpArduinoMain = fopen("arduino_slave/arduino_slave.ino", "a+");
    
    read_sensors_h = fopen("arduino_slave/helpers.h", "w");\
            
    BoardGenerate();
    fprintf(fpArduinoMain,"byte Value[%d][8];\n",theBoard.nSenAct);
    fprintf(fpBoardH,"extern byte Value[%d][8];\n",theBoard.nSenAct);
    boardPack(read_sensors_h);
    sensorPack(read_sensors_h, fpArduinoMain, theBoard.nSenAct );
    fprintf(read_sensors_h,"unsigned char nDataS[%d] = { %d",theBoard.nSenAct,theSenActs[0].nData);    
    int i;
    for( i = 1; i<theBoard.nSenAct;i++)fprintf(read_sensors_h,", %d",theSenActs[i].nData);
    fprintf(read_sensors_h,"};"); 
    
    printReadFunctionsH(fpBoardH);
    printReadFunctionsC(fpArduinoMain);
    
    fprintf(fpBoardH,"\n#endif\n");
    fprintf(fpArduinoMain, "// vim:cin:ai:sts=2 sw=2 ft=cpp\n"); 
    
    return 0;
}
//none
