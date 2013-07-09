#include "Board.h"

byte newBoardPacked[32];
byte package[32];
Board Boards[MAX_N_BOARDS];
SenAct SenActs[MAX_N_SENACT];
unsigned char nBoards = 0;
unsigned char nSenActs = 0;

boolean readSensorB(SenAct * theSenAct)
{
  byte data[theSenAct->nData];
  while(!radio.write( &(theSenAct->nSA), 1));
  
  boolean temp = readPackage(theSenAct->lastReading, theSenAct->nData);
  
  return temp;
}

boolean readAllSonBoard(Board * theBoard)
{
  radio.setChannel(theBoard->channel);
  
  for(int i =0;i < theBoard->nSenAct; i++)
  {
    if( readSensorB(&((theBoard->arraySenAct)[i])) == 0 ) return 0;
  }
  return 1;
}

boolean writePackage(void * package, unsigned char len)
{
    radio.stopListening();
    boolean temp = radio.write(package,32);
    radio.startListening();
    return temp;
}

boolean readPackage(void * package,unsigned char len)
{
    unsigned long started_waiting_at = millis();
    bool timeout = false;
    while ( ! radio.available() && ! timeout )
      if (millis() - started_waiting_at > 1000 )
        timeout = true;
        
    if(!timeout)
    {
        timeout = !radio.read(package, len);
    }
    else Serial.println("\nTimeout fail");
    return !timeout;
           
} 

boolean readPackageAck(byte * package, unsigned char len, byte * ack, unsigned char lenAck)
{
    if(readPackage(package,len))
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

boolean unregBoardAvailable()
{
    if((nBoards < MAX_N_BOARDS) && (nSenActs < MAX_N_SENACT))
    {
        radio.stopListening();
        radio.setChannel( APPLICATION_CH );
        radio.startListening();
        
        if(readPackage(newBoardPacked, 32))
        {
            if(writePackage(newBoardPacked, 32))return true;
            else
            {
                Serial.println("\nError: writng back board");
                return false;
            }
        }
        else
        {
            Serial.println("\nError: reading newBoard pack");
            return false;
        }       
    }
    else
    {
        Serial.println("Error: no more buffer space");
        return false;
    }
}

boolean newBoardConnect()
{
    byte k =1;
    radio.stopListening();
    radio.setChannel( DEFINITION_CH );
    radio.startListening();
    
    if(readPackageAck(package,1,&k,1))
    {
        unpackBoard(&Boards[nBoards],newBoardPacked);
        Boards[nBoards].arraySenAct =  &SenActs[nSenActs];
        return true;
    }
    else
    {
        Serial.println("\nError: Board not connected");
        return false;
    }
}

boolean newBoardDefine()
{
    byte k;
    unsigned char tempNsenAct = nSenActs;
    for(int i =0; i< Boards[nBoards].nSenAct; i++)
    {    
        k=i;
        if(writePackage(&k, 1))
        {
            unsigned char tempNsenAct = nSenActs;
            if(readPackage(package,32))
            {
                unpackSenAct( &SenActs[nSenActs],package);
                nSenActs=nSenActs++;
            }
            else
            {
                nSenActs = tempNsenAct;
                Serial.println("\nError in reading Senact pack");
                return false;
            }                            
        }
        else
        {
        nSenActs = tempNsenAct;
        Serial.println("\nError in write k");
        return false;
        }
    }
    nBoards++;
    k=0xff;
    if(writePackage(&k, 1))return true;
    else
    {
        nBoards-=1;
        nSenActs = tempNsenAct;
        Serial.println("\nError in write conn");
        return false;
    }
}
