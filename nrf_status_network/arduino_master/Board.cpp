/*
*   This program is free software; you can redistribute it and/or
*   modify it under the terms of the GNU General Public License
*   version 2 as published by the Free Software Foundation.
*/

#include "Board.h"

byte newBoardPacked[32];
byte package[32];
Board Boards[MAX_N_BOARDS];
SenAct SenActs[MAX_N_SENACT];
byte freeCH[127];


unsigned char nBoards = 0;
unsigned char nSenActs = 0;
unsigned char nFreeCH=0;
unsigned char nextFreeCH = 0;

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10
RF24 radio(9,10);

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = {0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL};

/****************************************************************************/

void initialiseBoard()
{
    Serial.begin(57600);
    radio.begin();
    mapFreeCH();
    // optionally, increase the delay between retries & # of retries
    radio.setRetries(15,15);
    radio.setPayloadSize(PAYLOAD_SIZE);\
    radio.openWritingPipe(pipes[0]);
    radio.openReadingPipe(1,pipes[1]);
    radio.startListening();

}

/****************************************************************************/

void mapFreeCH()
{
    radio.setAutoAck(false);
    radio.startListening();
    radio.stopListening();

    for( int i = 30; i< 127; i++)
    {
        radio.setChannel(i);
        radio.startListening();
        delay(25);
        radio.stopListening();

        if ( !radio.testCarrier() )
        {
            freeCH[nFreeCH++] = i;
        }
    }
    radio.setAutoAck(true);
    radio.startListening();
    //test printing
    for (int i =0; i<nFreeCH;i++)
    {
        //Serial.println(freeCH[i]);
    }
    //Serial.println("");
    //Serial.print(freeCH[nextFreeCH]);
    //Serial.println("");
}

/****************************************************************************/

boolean readSensorB(SenAct * theSenAct)
{
    //Serial.print((char*)theSenAct->name);
    //Serial.print(": ");
    if(writePackage(&(theSenAct->nSA),1))
    {
        if(readPackage(theSenAct->lastReading, theSenAct->nData))
        {
            Serial.print("\n");
            switch(theSenAct->type)
            {
                case BOOL:
                    //Serial.println(*((boolean*)theSenAct->lastReading));
                    Serial.print(theSenAct->name);
                    Serial.print('\t');
                    Serial.println(*((boolean*)theSenAct->lastReading));
                    break;
                case U_CHAR:
                    //Serial.println(*((unsigned char*)theSenAct->lastReading));
                    break;
                case S_CHAR:
                    //Serial.println(*((signed char*)theSenAct->lastReading));
                    break;
                case U_INT16:
                    //Serial.println(*((unsigned int*)theSenAct->lastReading));
                    break;
                case S_INT16:
                    //Serial.println(*((signed int*)theSenAct->lastReading));
                    break;
                case UL_INT32:
                    //Serial.println(*((unsigned long int*)theSenAct->lastReading));
                    break;
                case SL_INT32:
                    //Serial.println(*((signed long int*)theSenAct->lastReading));
                    break;
                case FLOAT:
                    Serial.print(theSenAct->name);
                    Serial.print('\t');
                    Serial.println(*((float*)theSenAct->lastReading));
                    break;
                case DOUBLE:
                    //Serial.println(*((double*)theSenAct->lastReading));
                    break;
                case CHAR_ARRAY:
                    //Serial.println((char*)theSenAct->lastReading);
                    break;
            }
            return true;
        }
        else
        {
            Serial.println("Fail read sensor");
            return false;
        }
    }
    else
    {
        Serial.println("Fail write to read sensor");
        return false;
    }
}

/****************************************************************************/

boolean readAllSonBoard(Board * theBoard)
{
    radio.stopListening();

    radio.setChannel(theBoard->channel);
    //Serial.println((char*)theBoard->name);
    //Serial.println(theBoard->channel);
    radio.startListening();
    for(int i =0;i < theBoard->nSenAct; i++)
    {
        if((theBoard->arraySenAct+i)->sOrA == SENSOR)
            if( readSensorB(theBoard->arraySenAct+i) == 0 ) return 0;
    }
    return 1;
}

/****************************************************************************/

boolean readAllSwIntonBoard(Board * theBoard)
{
    radio.stopListening();

    radio.setChannel(theBoard->channel);
    //Serial.println((char*)theBoard->name);
    //Serial.println(theBoard->channel);
    radio.startListening();
    for(int i =0;i < theBoard->nSenAct; i++)
    {
        if((theBoard->arraySenAct+i)->reqOrInt == INTERRUPT)
            if((theBoard->arraySenAct+i)->sOrA == SENSOR)
                if( readSensorB(theBoard->arraySenAct+i) == 0 ) return 0;
    }
    return 1;
}

/****************************************************************************/

boolean writePackage(void * package, unsigned char len)
{
    radio.stopListening();
    boolean temp = radio.write(package,32);
    radio.startListening();
    return temp;
}

/****************************************************************************/

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
    //else Serial.println("\nTimeout fail");
    return !timeout;

}

/****************************************************************************/

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

/****************************************************************************/

void packBoard(Board theBoard, byte * package)
{
  int i =0;

  for(i = 0; i < SIZE_OF_NAME; i++)package[i] = (byte) theBoard.name[i];
  package[i]=(byte)theBoard.nSenAct;
}

/****************************************************************************/

void unpackBoard(Board* theBoard, byte * package)
{
    int i =0;

    //Serial.print("Board name: ");
    for(i = 0; i < SIZE_OF_NAME; i++)
    {
        theBoard->name[i] = (char)package[i];
        //Serial.print(theBoard->name[i]);
    }
    //Serial.println("");
    theBoard->nSenAct = (unsigned char)package[SIZE_OF_NAME];
    //Serial.print("Number of Sensors/Actuators: ");
    //Serial.print(theBoard->nSenAct);

}

/****************************************************************************/

void packSenAct(SenAct theSenAct, byte * package)
{
    int i =0;

    for(i = 0; i < SIZE_OF_NAME; i++)package[i] = (byte) theSenAct.name[i];
    package[i++]=(byte)theSenAct.type;
    package[i++]=(byte)theSenAct.nData;
    package[i++]=(byte)theSenAct.sOrA;
    package[i]=(byte)theSenAct.nSA;
}

/****************************************************************************/

void unpackSenAct(SenAct* theSenAct, byte * package)
{
    int i =0;
    //Serial.print("\nSensor/Actuator name: ");
    for(i = 0; i < SIZE_OF_NAME; i++)
    {
        (*theSenAct).name[i] = (char) package[i];
        //Serial.print((*theSenAct).name[i]);
    }
    //Serial.println("");
    //Serial.print("Data_type: ");
    theSenAct->type = (DATATYPE) package[SIZE_OF_NAME];
    //Serial.println(theSenAct->type,DEC);
    //Serial.print("Number of data bytes: ");
    theSenAct->nData = (unsigned char) package[SIZE_OF_NAME + 1];
    //Serial.println(theSenAct->nData,DEC);
    //Serial.print("Sensor or Acuator: ");
    theSenAct->sOrA = (S_OR_A) package[SIZE_OF_NAME + 2];
    //Serial.println(theSenAct->sOrA,DEC);
    //Serial.print("Sensor number: ");
    theSenAct->nSA = (unsigned char) package[SIZE_OF_NAME + 3];
    //Serial.println(theSenAct->nSA,DEC);
    theSenAct->reqOrInt = (REQ_OR_INT) package[SIZE_OF_NAME + 4];
}

/****************************************************************************/

boolean newBoardAvailable()
{
    if((nBoards < MAX_N_BOARDS) && (nSenActs < MAX_N_SENACT)&&nFreeCH)
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
            //Serial.println("\nError: no New Board");
            return false;
        }
    }
    else
    {
        Serial.println("Error: no more buffer/channel space");
        return false;
    }
}

/****************************************************************************/

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

/****************************************************************************/

boolean newBoardDefine()
{
    byte k;
    int index;
    if(alreadyRegistared(&Boards[nBoards],&index))
        return returnBoardToNetwork(&Boards[nBoards],index);

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
                //Serial.println((char*)SenActs[nSenActs].name);
                nSenActs++;
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
    //Send change channel command
    k=0xf0;
    if(writePackage(&k, 1))
    {
        k=1;
        if(readPackageAck(package,1,&k,1))
        {
            if(writePackage(&freeCH[nextFreeCH],1))
            {
                if(readPackageAck(package,1,&k,1))
                {
                    Boards[nBoards].channel = freeCH[nextFreeCH];
                    //Serial.println("\nfree Chanell: ");
                    //Serial.print(freeCH[nextFreeCH],DEC);

                    nFreeCH=nFreeCH-4;
                    nextFreeCH=nextFreeCH+4;
                }
                else
                {
                    nSenActs = tempNsenAct;
                    Serial.println("\nError in read ch ACK");
                    return false;
                }
            }
            else
            {
                nSenActs = tempNsenAct;
                Serial.println("\nError in write ch");
                return false;
            }
        }
        else
        {
            nSenActs = tempNsenAct;
            Serial.println("\nError in read ch comm ACK");
            return false;
        }
    }
    else
    {
        nSenActs = tempNsenAct;
        Serial.println("\nError in write ch comm");
        return false;
    }


    //Send connected command
    k=0xff;
    delay(10);
    if(writePackage(&k, 1))
    {
        nBoards++;
        return true;
    }
    else
    {
        nSenActs = tempNsenAct;
        Serial.println("\nError in write conn");
        return false;
    }
}

/****************************************************************************/

boolean alreadyRegistared(Board *theBoard, int *index)
{
    boolean found = true;
    for(int i = 0; i < nBoards; i++)
    {
        found = true;
        *index = i;
        for(int j = 0; j < SIZE_OF_NAME;j++)
        {
            if(theBoard->name[j]!=Boards[i].name[j])found = false;
        }
        if(found) return true;
    }
    return false;
}

/****************************************************************************/

boolean returnBoardToNetwork(Board *theBoard, int index)
{
    byte k=0xf0;
    if(writePackage(&k, 1))
    {
        k=1;
        if(readPackageAck(package,1,&k,1))
        {
            if(writePackage(&(Boards[index].channel),1))
            {
                if(readPackageAck(package,1,&k,1))
                {
                    //Send connected command
                    k=0xff;
                    delay(10);
                    if(writePackage(&k, 1))
                    {
                        //Serial.println("Board reconnected!");
                        return true;
                    }
                    else   return false;
                }
                else return false;
            }
            else
            {return false;
            }
        }
        else
        {return false;
        }
    }
    else
    {return false;
    }
}

/****************************************************************************/

void listBoardsAndSenActsToSerial(void)
{
     if(nBoards)
    {
        for(int i = 0; i < nBoards; i++)
        {
            for(int j =0; j < Boards[i].nSenAct; j++)
            {
                Serial.print("\nRegister\t");
                Serial.print(Boards[i].name);
                Serial.print(" ");
                Serial.print(i);
                Serial.print(" ");
                Serial.print((Boards[i].arraySenAct+j)->name);
                Serial.print(" ");
                Serial.println(j);
            }
        }
    }
}

void changeChannel(byte newC)
{
    radio.stopListening();

    radio.setChannel(newC);
    radio.startListening();
}
