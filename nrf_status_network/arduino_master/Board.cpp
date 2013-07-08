#include "Board.h"


boolean readSensorB(SenAct * theSenAct, RF24 radioX)
{
  byte data[theSenAct->nData];
  while(!radioX.write( &(theSenAct->nSA), 1));
  
  boolean temp = readPackage(theSenAct->lastReading, theSenAct->nData, radioX);
  
  return temp;
}

boolean readAllSonBoard(Board * theBoard, RF24 radioX)
{
  radioX.setChannel(theBoard->channel);
  
  for(int i =0;i < theBoard->nSenAct; i++)
  {
    if( readSensorB(&((theBoard->arraySenAct)[i]), radioX) == 0 ) return 0;
  }
  return 1;
}

boolean writePackage(void * package, unsigned char len, RF24 radioX)
{
    radioX.stopListening();
    boolean temp = radioX.write(package,32);
    radioX.startListening();
    return temp;
}

boolean readPackage(void * package,unsigned char len, RF24 radioX)
{
    unsigned long started_waiting_at = millis();
    bool timeout = false;
    while ( ! radioX.available() && ! timeout )
      if (millis() - started_waiting_at > 1000 )
        timeout = true;
        
    if(!timeout)
    {
        timeout = !radioX.read(package, len);
    }
    else Serial.println("\nTimeout fail");
    return !timeout;
           
} 

boolean readPackageAck(byte * package, unsigned char len, byte * ack, unsigned char lenAck, RF24 radioX)
{
    if(readPackage(package,len,radioX))
    {
        for(int i = 0; i < lenAck; i++)
        {
            if(package[i] != ack[i])
            {
                Serial.println("\nACK failed: ");
                //Serial.print(package[0]);
                return false;
            }
        }
        return true;
    }
    Serial.println("\nread failed");
    return false;
}



void packBoard(Board theBoard, byte * package)
{
  int i =0;
                  
  for(i = 0; i < SIZE_OF_NAME; i++)package[i] = (byte) theBoard.name[i];
  package[i]=(byte)theBoard.nSenAct;
}
void unpackBoard(Board* theBoard, byte * package)
{
    int i =0;

    Serial.print("Board name: ");
    for(i = 0; i < SIZE_OF_NAME; i++)
    {
        theBoard->name[i] = (char)package[i];
        Serial.print(theBoard->name[i]);
    }
    Serial.println("");
    theBoard->nSenAct = (unsigned char)package[SIZE_OF_NAME];
    Serial.print("Number of Sensors/Actuators: ");
    Serial.print(theBoard->nSenAct);
}
void packSenAct(SenAct theSenAct, byte * package)
{
    int i =0;

    for(i = 0; i < SIZE_OF_NAME; i++)package[i] = (byte) theSenAct.name[i];
    package[i++]=(byte)theSenAct.type;
    package[i++]=(byte)theSenAct.nData;
    package[i++]=(byte)theSenAct.sOrA;
    package[i]=(byte)theSenAct.nSA;
}
void unpackSenAct(SenAct* theSenAct, byte * package)
{
    int i =0;
    Serial.print("\nSensor/Actuator name: ");
    for(i = 0; i < SIZE_OF_NAME; i++)
    {
        (*theSenAct).name[i] = (char) package[i];
        Serial.print((*theSenAct).name[i]);
    }
    Serial.println("");
    Serial.print("Data_type: ");
    theSenAct->type = (DATATYPE) package[SIZE_OF_NAME];
    Serial.println(theSenAct->type,DEC);
    Serial.print("Number of data bytes: ");
    theSenAct->nData = (unsigned char) package[SIZE_OF_NAME + 1];
    Serial.println(theSenAct->nData,DEC);
    Serial.print("Sensor or Acuator: ");
    theSenAct->sOrA = (S_OR_A) package[SIZE_OF_NAME + 2];
    Serial.println(theSenAct->sOrA,DEC);
    Serial.print("Sensor number: ");
    theSenAct->nSA = (unsigned char) package[SIZE_OF_NAME + 3];
    Serial.println(theSenAct->nSA,DEC);
}

