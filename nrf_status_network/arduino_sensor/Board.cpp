#include "Board.h"

char * SenActNames[BOARD_NSENACT] = {SENSOR1_NAME,SENSOR2_NAME};
DATATYPE SenActTypes[BOARD_NSENACT] = {SENSOR1_DATATYPE,SENSOR2_DATATYPE};
unsigned char SenActNdata[BOARD_NSENACT] = {SENSOR1_NDATA,SENSOR2_NDATA};
S_OR_A  SenActSoRa[BOARD_NSENACT] = {SENSOR1_SORA,SENSOR2_SORA};
unsigned char SenActNSA[BOARD_NSENACT] = {SENSOR1_NSA,SENSOR2_NSA};

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

void packBoard(byte * package)
{
  int i =0;
  char * name = BOARD_NAME;
  unsigned char nSenAct = BOARD_NSENACT;
  //for(i = 0; i < SIZE_OF_NAME; i++)package[i] = (byte) name[i];
  while(name[i])
  {
      package[i] = (byte) name[i];
      i++;
  }
  while(SIZE_OF_NAME - i)
  {
      package[i++] = 0;
  }
  
  package[SIZE_OF_NAME]=(byte)nSenAct;
  
  while(31-i)
  {
      package[++i] = '0';
  }
}


void packSenAct(unsigned char n, byte * package)
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
      package[i] = (byte) SenActNames[n][i];
      i++;
    }
    while((SIZE_OF_NAME - i)>0)
    {
      package[i++] = 0;
    }
  
    package[SIZE_OF_NAME]=(byte)SenActTypes[n];
    //Serial.print(package[SIZE_OF_NAME]);
    package[SIZE_OF_NAME + 1]=(byte)SenActNdata[n];
    //Serial.print(package[SIZE_OF_NAME + 1]);
    package[SIZE_OF_NAME + 2]=(byte)SenActSoRa[n];
    //Serial.print(package[SIZE_OF_NAME + 2]);
    package[SIZE_OF_NAME + 3]=(byte)SenActNSA[n];
    //Serial.print(package[SIZE_OF_NAME + 3]);
    //Serial.println((byte)SenActNSA[n], DEC);
    
    i+=3;
    while(31-i)
    {
      package[++i] = 0;
    }
    
}

boolean applyBoard(RF24 radioX)
{
    byte packageT[3][32];
    packBoard(packageT[0]);
    for(int i = 0; i < BOARD_NSENACT; i++)
    {
        packSenAct(i, packageT[i+1]);
    }
    byte package[32];
    byte k = 1;
    boolean connected = false;
    boolean error = false;
    while(!connected)
    {
        radioX.setChannel( APPLICATION_CH );
        if(writePackage(packageT[0],32,radioX))
        {
            if(readPackageAck(package,32,packageT[0],32,radioX))
            {
                radioX.setChannel( DEFINITION_CH );
                if(writePackage(&k, 1,radioX))
                {
                    for(int i = 0; i < BOARD_NSENACT; i++)
                    {
                        delay(10);
                        if(readPackageAck(package,1,&k,1,radioX))
                        {
                            if(writePackage(packageT[i + 1],32,radioX));
                            else
                            {
                                error = true;
                                Serial.println("Error 4");
                            }
                        }
                        else
                        {
                            error = true;
                            Serial.println("Error 3");
                        }
                    }
                    if(!error)connected = true;
                }
                else
                {
                    Serial.println("Error 2");
                }
            }
            else
            {
                Serial.println("Error 1");
            }
        }
        else
        {
            Serial.println("Error 0");
        }
    }
    return true;    
}


/*
boolean applyBoard(RF24 radioX)
{
    byte packageT[3][32];
    packBoard(packageT[0]);
    Serial.println((char*)packageT[0]);
    for(int i = 0; i < BOARD_NSENACT; i++)
    {
        packSenAct(i, packageT[i+1]);
    }
    byte package[32];
    Serial.println("apply board");
    Serial.println((char*)packageT[0]);
    radioX.setChannel( APPLICATION_CH );
    if(writePackage(packageT[0], 32, radioX))
    {
        if(readPackage(package, 32,radioX))
        {
            boolean temp = true;
            for(int i = 0; i < 32; i++)
            {
                if(packageT[0][i] != package[i])boolean temp = false;
            }
            if(temp)
            {
                radioX.setChannel( DEFINITION_CH );
                for(int j = 0; j < BOARD_NSENACT; j++)
                {   
                    delay(100);                 
                    if(readPackage(package, 1,radioX))
                    {
                        if((unsigned char)package[0] == 1)
                        {
                            //packSenAct(j, package);
                            //writePackage(package, 32, radioX);
                            if(writePackage(packageT[j+1], 32, radioX))
                            {
                            }
                            else
                            {
                                Serial.println("Error 5");
                                Serial.println(j);
                                return 0;
                            }
                        }
                        else
                        {
                            Serial.println("Error 4");
                            return 0;
                        }
                    }
                    else
                    {
                        Serial.println("Error 3");
                        return 0;
                    }
                }
            }
            else
            {
                Serial.println("Error 1");
                return 0;
            }
        }
        else
        {
            Serial.println("Error 0");
            return 0;
        }
        return 1;
    }
    return 0;
}
*/
