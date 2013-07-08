#include "Board.h"

char * SenActNames[BOARD_NSENACT] = {SENSOR1_NAME,SENSOR2_NAME};
DATATYPE SenActTypes[BOARD_NSENACT] = {SENSOR1_DATATYPE,SENSOR2_DATATYPE};
unsigned char SenActNdata[BOARD_NSENACT] = {SENSOR1_NDATA,SENSOR2_NDATA};
S_OR_A  SenActSoRa[BOARD_NSENACT] = {SENSOR1_SORA,SENSOR2_SORA};
unsigned char SenActNSA[BOARD_NSENACT] = {SENSOR1_NSA,SENSOR2_NSA};

byte packageTot[BOARD_NSENACT][32];
byte BoardPack[32];

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
                Serial.println("\nACK failed");
                return false;
            }
        }
        return true;
    }
    Serial.println("\nread failed");
    return false;
}

boolean writePackage(void * package, unsigned char len, RF24 radioX)
{
    radioX.stopListening();
    boolean temp = radioX.write(package,32);
    radioX.startListening();
    return temp;    
}

void packBoard()
{
  int i =0;
  char * name = BOARD_NAME;
  unsigned char nSenAct = BOARD_NSENACT;
  while((name[i])&&(i < SIZE_OF_NAME))
  {
      boardPack[i] = (byte) name[i];
      i++;
  }
  while(SIZE_OF_NAME - i)
  {
      boardPack[i++] = 0;
  }
  
  boardPack[SIZE_OF_NAME]=(byte)nSenAct;
  
  while(31-i)
  {
      package[++i] = '0';
  }
}


void packSenAct(unsigned char n)
{
    int i =0;
    /*
    for(i = 0; i < SIZE_OF_NAME; i++)package[i] = (byte) SenActNames[n][i];
    package[i++]=(byte)SenActTypes[n];
    package[i++]=(byte)SenActNdata[n];
    package[i++]=(byte)SenActSoRa[n];
    package[i]=(byte)SenActNSA[n];
    */
    while(SenActNames[n][i]&&((SIZE_OF_NAME-i)>0))
    {
        
      packageTot[n][i] = (byte) SenActNames[n][i];
      i++;
    }
    while((SIZE_OF_NAME - i)>0)
    {
      packageTot[n][i++] = 0;
    }
  
    packageTot[n][SIZE_OF_NAME]=(byte)SenActTypes[n];
    //Serial.print(package[SIZE_OF_NAME]);
    packageTot[n][SIZE_OF_NAME + 1]=(byte)SenActNdata[n];
    //Serial.print(package[SIZE_OF_NAME + 1]);
    packageTot[n][SIZE_OF_NAME + 2]=(byte)SenActSoRa[n];
    //Serial.print(package[SIZE_OF_NAME + 2]);
    packageTot[n][SIZE_OF_NAME + 3]=(byte)SenActNSA[n];
    //Serial.print(package[SIZE_OF_NAME + 3]);
    //Serial.println((byte)SenActNSA[n], DEC);
    
    i+=3;
    while(31-i)
    {
      packageTot[n][++i] = 0;
    }
    
}

//SenActs keep sending their Board packages, server can read only one at a time.
//When the server reads one, it sends the same package back as ack...
//since all boards names are different, only the one that sent it will get it back
boolean registerBoard(RF24 radioX)
{
    radiradioX.flush_tx();
    //Send board package
    if(writePackage(boardPack, 32, radioX))
    {
        byte testP[32];
        //read the response back and check against board package
        if(readPackageAck(testP, 32,boardPack,32,radioX))
        {   
            radioX.setChannel( DEFINITION_CH );  //change channel to definition chanel
            byte k = 1;
            if(writePackage(&k, 1,radioX))       //send back 1 over def channel for ack
            {
                //registered = true;
                Serial.println("\nApplied");
                //timerA = millis();              //start deffinition timer
            }
            else
            {//failed to send ack go back to application
                radio.setChannel( REGISTRATION_CH );
                Serial.println("\nError 3");
                return false;
            }
        }
        else
        {//failed the pack ack, go back to applictaio
            Serial.println("\nError 2");
            return false;
        }
    }
    else
    {//failed to write, try again :)
        Serial.println("\nError 1");
        return false;
    }
    return true;
}

boolean defineBoard(boolean * registered, unsigned long timerA, radio RF24)
{   //if allready registered continue with defining
    byte k;
    if(readPackage(&k, 1,radio))
    {//read command from server
        if(k == 0xff)
        {//connected...
            Serial.println("Connected");
            return true;
        }
        if(k == 0xf0)
        {//new channel setup...
        }                
        if(k < 0xf0)
        {   //send SenAct package #k to server
            if(writePackage(packageTot[k], 32, radio))
                Serial.println("\nSent #k SenACt");
            else
            {//failed to send #k senact package
                Serial.println("\nError in sending k pack");
            }
        }
    }
    else
    {// failed to read command
        Serial.println("\nError in read command");
    }
    return false;
}
