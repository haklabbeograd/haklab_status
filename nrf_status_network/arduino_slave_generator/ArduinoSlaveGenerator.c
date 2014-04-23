
#include "functions.h"

int main()
{
    FILE  *fpArduinoMain,*fpBoardH, *read_sensors_h;
    copyFile("input/Board.h", "arduino_slave/Board.h");
    copyFile("input/Board.cpp", "arduino_slave/Board.cpp");
    copyFile("input/printf.h", "arduino_slave/printf.h");
    copyFile("input/arduino_sensor_door.ino", "arduino_slave/arduino_slave.ino");

    BoardGenerate();
    fpArduinoMain = fopen("arduino_slave/arduino_slave.ino", "a+");
    if (!fpArduinoMain) return 1;
    fprintf(fpArduinoMain,"byte Value[%d][8];\n",theBoard.nSenAct);
    fpBoardH = fopen("arduino_slave/Board.h","a++");
    if (!fpBoardH) return 1;
    fprintf(fpBoardH,"extern byte Value[%d][8];\n",theBoard.nSenAct);
    read_sensors_h = fopen("arduino_slave/helpers.h", "w");
    if (!read_sensors_h) return 1;
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
