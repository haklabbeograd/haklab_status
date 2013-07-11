#include "Board.h"

char * SenActNames[BOARD_NSENACT] = {SENSOR1_NAME};
DATATYPE SenActTypes[BOARD_NSENACT] = {SENSOR1_DATATYPE};
unsigned char SenActNdata[BOARD_NSENACT] = {SENSOR1_NDATA};
S_OR_A  SenActSoRa[BOARD_NSENACT] = {SENSOR1_SORA};
unsigned char SenActNSA[BOARD_NSENACT] = {SENSOR1_NSA};

RF24 radio(9,10);
byte packageTot[BOARD_NSENACT][32];
byte BoardPack[32];
boolean registered = false;
unsigned char Channal;
byte Value[BOARD_NSENACT][MAX_SIZE_OF_DATA];
// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

/****************************************************************************/

void initialiseBoard()
{
    Serial.begin(57600);        //Starts serial for debug
    radio.begin();              //starts nRF24
    // optionally, increase the delay between retries & # of retries
    radio.setRetries(15,15);
    radio.setChannel( REGISTRATION_CH );

    // optionally, reduce the payload size.  seems to
    // improve reliability
    radio.setPayloadSize(PAYLOAD_SIZE);
    radio.openWritingPipe(pipes[1]);
    radio.openReadingPipe(1,pipes[0]);
    radio.startListening();
    radio.printDetails();   //prints out all the nRF registars for debuging
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
                Serial.println("\nACK failed");
                return false;
            }
        }
        return true;
    }
    Serial.println("\nread failed");
    return false;
}

boolean writePackage(void * package, unsigned char len)
{
    radio.stopListening();
    boolean temp = radio.write(package,32);
    radio.startListening();
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
boolean registerBoard(void)
{
    //radioX.flush_tx();
    //Send board package
    if(writePackage(BoardPack, 32))
    {
        byte testP[32];
        //read the response back and check against board package
        if(readPackageAck(testP, 32,BoardPack,32))
        {   
            radio.stopListening();
            radio.setChannel( DEFINITION_CH );  //change channel to definition chanel
            radio.startListening();
            byte k = 1;
            if(writePackage(&k, 1))       //send back 1 over def channel for ack
            {
                Serial.println("\nApplied");
                return true;
            }
            else
            {//failed to send ack go back to application
                radio.stopListening();
                radio.setChannel( REGISTRATION_CH );
                radio.startListening();
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

boolean defineBoard(void)
{   //if allready registered continue with defining
    byte c,k,ch;
    if(readPackage(&c, 1))
    {//read command from server
        Serial.print("\ncommand receved:");
        Serial.print(c,HEX);
        Serial.println("");
        if(c == 0xff)
        {//connected...
            Serial.println("Connected");
            radio.stopListening();
            radio.setChannel(Channal);
            radio.startListening();
            Serial.println(Channal);
            return true;
        }
        if(c == 0xf0)
        {//new channel setup...
            k = 1;
            if(writePackage(&k, 1))
            {
                if(readPackage(&ch, 1))
                {
                    if(writePackage(&k, 1))
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
            if(writePackage(packageTot[c], 32))
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

boolean registerAndDefineBoard(unsigned long *timerA)
{   //if not connected senact needs to apply and define
    if(registered)
    {
        //Definition timer
        //goes back to register mode, if not connected within 5s
        if(millis()-*timerA > 5000)
        {
            registered = false;
            Serial.println("\nTimer has gone");
        }
        
        if(defineBoard())
        {
            registered = false;
            *timerA = millis();//TEST
            return true;
        }
    }
    else
    {
        radio.stopListening();
        radio.setChannel( REGISTRATION_CH );
        radio.startListening();
        
        if(registerBoard())
        {
            registered = true;
            *timerA = millis();
        }
    }
    return false;
}

boolean commandReceved(byte * command)
{
    return readPackage(command, 1);
}
            
boolean parseCommand(byte command)
{    
    Serial.print(command,HEX);
    if(command < 0xf0)
    {
        writePackage(&(Value[command]),SENSOR1_NDATA);
    }
    else if (command == 0xf0)
    {
    }
    else if (command == 0xff)
    {
    }
}

boolean packValue(void * boardValue, unsigned char nBytes, unsigned char index)
{
    for (byte i = 0; i < nBytes; i++)
    {
        Value[index][i]= *((byte*)boardValue + i);
    }
}
