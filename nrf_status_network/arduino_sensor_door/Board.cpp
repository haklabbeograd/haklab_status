#include "Board.h"

char * SenActNames[BOARD_NSENACT] = {SENSOR1_NAME};
DATATYPE SenActTypes[BOARD_NSENACT] = {SENSOR1_DATATYPE};
unsigned char SenActNdata[BOARD_NSENACT] = {SENSOR1_NDATA};
S_OR_A  SenActSoRa[BOARD_NSENACT] = {SENSOR1_SORA};
unsigned char SenActNSA[BOARD_NSENACT] = {SENSOR1_NSA};

byte packageTot[BOARD_NSENACT][32];
byte BoardPack[32];
boolean registered = false;
unsigned char Channal;
bool value[BOARD_NSENACT];
byte Value[BOARD_NSENACT][MAX_SIZE_OF_DATA];


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
      BoardPack[i] = (byte) name[i];
      i++;
  }
  while(SIZE_OF_NAME - i)
  {
      BoardPack[i++] = 0;
  }
  
  BoardPack[SIZE_OF_NAME]=(byte)nSenAct;
  
  while(31-i)
  {
      BoardPack[++i] = '0';
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
    Serial.println("\nPacking Sensor");
    while(SenActNames[n][i]&&((SIZE_OF_NAME-i)>0))
    {
        
      packageTot[n][i] = (byte) SenActNames[n][i];
      Serial.print((char)packageTot[n][i]);
      i++;
    }
    while((SIZE_OF_NAME - i)>0)
    {
      packageTot[n][i++] = 0;
    }
  
    packageTot[n][SIZE_OF_NAME]=(byte)SenActTypes[n];
    Serial.print(packageTot[n][SIZE_OF_NAME]);
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
    //radioX.flush_tx();
    //Send board package
    if(writePackage(BoardPack, 32, radioX))
    {
        byte testP[32];
        //read the response back and check against board package
        if(readPackageAck(testP, 32,BoardPack,32,radioX))
        {   
            radioX.setChannel( DEFINITION_CH );  //change channel to definition chanel
            byte k = 1;
            if(writePackage(&k, 1,radioX))       //send back 1 over def channel for ack
            {
                Serial.println("\nApplied");
                return true;
            }
            else
            {//failed to send ack go back to application
                radioX.setChannel( REGISTRATION_CH );
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
}

boolean defineBoard(boolean * registered, RF24 radioX)
{   //if allready registered continue with defining
    byte c,k,ch;
    if(readPackage(&c, 1,radioX))
    {//read command from server
        Serial.print("\ncommand receved:");
        Serial.print(c,HEX);
        Serial.println("");
        if(c == 0xff)
        {//connected...
            Serial.println("Connected");
            radioX.stopListening();
            radioX.setChannel(Channal);
            radioX.startListening();
            Serial.println(Channal);
            return true;
        }
        if(c == 0xf0)
        {//new channel setup...
            k = 1;
            if(writePackage(&k, 1, radioX))
            {
                if(readPackage(&ch, 1,radioX))
                {
                    if(writePackage(&k, 1, radioX))
                    {
                        Channal = (unsigned char)ch;
                        Serial.println(Channal);
                    }
                    else
                    {
                        Serial.println("\nError write ch ack");
                    }
                }
                else
                {
                    Serial.println("\nError read ch");
                }
            }
            else
            {
                Serial.println("\nError write ch comm ack");
            }
        }                
        if(c < 0xf0)
        {   //send SenAct package #k to server
            if(writePackage(packageTot[c], 32, radioX))
            {
                Serial.println("\nSent #k SenACt");
                Serial.println( (char*)packageTot[c]);
            }
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

boolean registerAndDefineBoard(unsigned long *timerA, RF24 radioX)
{   //if not connected senact needs to apply and define
    if(registered)
    {
        //Definition timer
        //goes back to register mode, if not connected within 5s
        if(millis()-*timerA > 5000)
        {
            registered = false;
            radioX.stopListening();
            radioX.setChannel( REGISTRATION_CH );
            radioX.startListening();
            Serial.println("\nTimer has gone");
        }
        
        if(defineBoard(&registered, radioX))
        {
            registered = false;
            *timerA = millis();//TEST
            return true;
        }
    }
    else
    {
        if(registerBoard(radioX))
        {
            registered = true;
            *timerA = millis();
        }
    }
    return false;
}

boolean commandReceved(byte command, RF24 radioX)
{
    
}
boolean parseCommand(byte command, RF24 radioX)
{
    Serial.print(command,HEX);
    if(command < 0xf0)
    {
        writePackage(&(Value[command]),SENSOR1_NDATA,radioX);
    }
    else if (command == 0xf0)
    {
    }
    else if (command == 0xff)
    {
    }
}

boolean packValue(void * boardValue, unsigned char nBytes, byte * sendValue)
{
    for (byte i = 0; i < nBytes; i++)
    {
        *(sendValue+i)= *((byte*)boardValue + i);
    }
}
