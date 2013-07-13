#include "functions.h"

Board theBoard;
SenAct theSenActs[100];
unsigned char nSenActs = 0;
void boardPack(FILE * fp)
{
    fprintf(fp,"//Board package\n");
    fprintf(fp,"//%s",theBoard.name);
    int kk;
    for( kk = strlen(theBoard.name);kk<MAX_SIZE_OF_NAME;kk++)fputc('0',fp);
    char temp[10];
    itoa(theBoard.nSenAct,temp,10);
    fprintf(fp,"%s",temp);
    fprintf(fp,"\nbyte boardPack[32] = { 0x%X",theBoard.name[0]);
    int i = 1;
    while((theBoard.name[i] != 0)&&(i<MAX_SIZE_OF_NAME))
    {
        fprintf(fp,", 0x%X",theBoard.name[i++]);
    }
    while((MAX_SIZE_OF_NAME - i)>0)
    {
        fprintf(fp,", 0x%X",0);
        i++;
    }
    fprintf(fp,", 0x%X",theBoard.nSenAct);  
    fprintf(fp," };\n\n");
}
void sensorPack(FILE * fp, FILE * fpArduinoMain, unsigned int nSA )
{
    fprintf(fp,"//The Sensors pack\n");
    fprintf(fp,"byte Sensors[%d][32] = \n{\n",nSA);  
    int i;
    for(i = 0; i < nSA;i++)
    {
        SensorGenerate(i);
        fprintf(fp,"\t{ 0x%X",theSenActs[i].name[0]);
        int j = 1;
        while((theSenActs[i].name[j] != 0)&&(j<MAX_SIZE_OF_NAME))
        {
            fprintf(fp,", 0x%X",theSenActs[i].name[j++]);
        }
        while((MAX_SIZE_OF_NAME - j)>0)
        {
            fprintf(fp,", 0x%X",0);
            j++;
        }
        char type[20];
        switch (theSenActs[i].type)
        {
            case 0:
                sprintf(theSenActs[i].typeS,"boolean");
                theSenActs[i].nData = 1;
                break;
            case 1:
                sprintf(theSenActs[i].typeS,"unsigned char");
                theSenActs[i].nData = 1;
                break;
            case 2:
                sprintf(theSenActs[i].typeS,"signed char");
                theSenActs[i].nData = 1;
                break;
            case 3:
                sprintf(theSenActs[i].typeS,"unsigned int");
                theSenActs[i].nData = 2;
                break;
            case 4:
                sprintf(theSenActs[i].typeS,"signed int");
                theSenActs[i].nData = 2;
                break;
            case 5:
                sprintf(theSenActs[i].typeS,"unsigned long int");
                theSenActs[i].nData = 4;
                break;
            case 6:
                sprintf(theSenActs[i].typeS,"signed long int");
                theSenActs[i].nData = 4;
                break;
            case 7:
                sprintf(theSenActs[i].typeS,"float");
                theSenActs[i].nData = 4;
                break;
            case 8:
                sprintf(theSenActs[i].typeS,"double");
                theSenActs[i].nData = 8;
                break;
            case 9:
                sprintf(theSenActs[i].typeS,"char*");
                theSenActs[i].nData = 32;
                break;
            default:
                printf("error");
        }
        fprintf(fp,", 0x%X",theSenActs[i].type);
        fprintf(fp,", 0x%X",theSenActs[i].nData);    
        fprintf(fp,", 0x%X",SENSOR);
        fprintf(fp,", 0x%X",i);  
        if(i== (theBoard.nSenAct - 1))fprintf(fp," }\n");
        else fprintf(fp," },\n");
        writeArduinoMain(fpArduinoMain,theSenActs[i].typeS, theSenActs[i].nData, i);    
    }
    fprintf(fp,"\n};\n");
    writePackAllS(fpArduinoMain);
}
void typeSwitch(DATATYPE *theType/*char * type*/)
{
    int choice;
    bool error = true;
    while(error)
    {
        printf("\nChose the Sensor data type: ");
        printf("\n0: boolean");
        printf("\n1: unsigned char      (1byte)");
        printf("\n2: signed char        (1byte)");
        printf("\n3: unsigned int       (2bytes)");
        printf("\n4: signed int         (2bytes)");
        printf("\n5: unsigned long int  (4bytes)");
        printf("\n6: signed long int    (4bytes)");
        printf("\n7: float              (4bytes)");
        printf("\n8: double             (8bytes)");
        printf("\n9: char[32]            (32bytes)");
        printf("\nEnder (0-9):");
        scanf("%d", &choice);
        if((choice>=0)&&(choice<=9))error = false;
        else printf("\n\nError: must chose between 0 and 9");
    }
    *theType = (DATATYPE)choice;    
}

void writeArduinoMain(FILE *fpArduinoMain, char *type, unsigned int nData,int i)
{
    fprintf(fpArduinoMain, "\nboolean packSensor%d()\n{\n",i+1);
    fprintf(fpArduinoMain, "\t%s value = readSensor%d();\n",type,i+1);
    fprintf(fpArduinoMain, "\tvoid * p = ");
    if(type[strlen(type)-1] != '*')fprintf(fpArduinoMain, "&");
    fprintf(fpArduinoMain, "value;\n");
    fprintf(fpArduinoMain, "\tfor(int i = 0; i<%d;i++)\n",nData);
    fprintf(fpArduinoMain, "\t\tValue[%d][i] = *((byte*)p + i);\n}\n",i);
}

void BoardGenerate()
{
    char BoardName[100];
    unsigned int nSA;
    printf("\nEnter the Board Name: ");
    scanf("%s", BoardName);
    if(strlen(BoardName)>MAX_SIZE_OF_NAME)BoardName[MAX_SIZE_OF_NAME] = 0;
    strncpy ( theBoard.name, BoardName, MAX_SIZE_OF_NAME );
    
    printf("\nEnter the number of Sensor/Actuators on Board: ");  
    scanf("%d", &nSA);
    theBoard.nSenAct = nSA;
}
void SensorGenerate(int index)
{
    char SensorName[100];
    printf("\nEnter the Sensor Name: ");
    scanf("%s", SensorName);
    if(strlen(SensorName)>MAX_SIZE_OF_NAME)SensorName[MAX_SIZE_OF_NAME] = 0;
    strncpy ( theSenActs[index].name, SensorName, MAX_SIZE_OF_NAME );
    typeSwitch(&theSenActs[index].type);
}

void printReadFunctionsH(FILE *fp)
{
    int i;
    for( i = 0; i < theBoard.nSenAct; i++ )fprintf(fp, "%s readSensor%d();\n", theSenActs[i].typeS,i+1);
}
void printReadFunctionsC(FILE *fp)
{
    int i;
    for( i = 0; i < theBoard.nSenAct; i++ ) 
    {
        fprintf(fp, "\n%s readSensor%d()\n{//", theSenActs[i].typeS,i+1);
        fprintf(fp,"enter code here to read the %s named sensor",theSenActs[i].name);
        fprintf(fp, "\n}\n");
    }
}

void writePackAllS(FILE *fp)
{
    fprintf(fp, "\nboolean packAllSensor()\n{\n");
    int i;
    for(i = 0; i < theBoard.nSenAct; i++)
        fprintf(fp, "\tpackSensor%d();\n",i+1);
    fprintf(fp, "}\n");
    //printf(fp, "// vim:cin:ai:sts=2 sw=2 ft=cpp\n");    
}

void copyFile(FILE *source, FILE *destination)
{
    char ch;
    while( ( ch = fgetc(source) ) != EOF )
      fputc(ch, destination);
 
   printf("File copied successfully.\n");
 
   fclose(source);
   fclose(destination);
}
