/*
    http://maniacbug.github.io/RF24/
    uses the RF24 library, so connect up the board as per the isntruction on the link above..
    
    this is a sketch for a server board which reads sensors automaticalay, 
    senses new ones, and attaches them to the existing network...
    
 */

#include <SPI.h>
//#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"
#include "Board.h"



// Set up nRF24L01 radio on SPI bus plus pins 9 & 10 
RF24 radio(9,10);

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = {0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL};

void setup(void)
{
  Serial.begin(57600);
  printf_begin();
  radio.begin();
  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);
  radio.setPayloadSize(PAYLOAD_SIZE);
  radio.openReadingPipe(1,pipes[1]);
  radio.startListening();
}

void loop()
{    
    byte k =1;
    for(int i = 0; i < nBoards; i++)
    {
    //readAllSonBoard(&(Boards[i]));
    //delay(2500);
    }
    
    
    if(unregBoardAvailable())
    {
        if(newBoardConnect())
        {
            if(newBoardDefine()) Serial.println("\nBoard added");
            else;
        }
        else;
    }
    else;
}

// vim:cin:ai:sts=2 sw=2 ft=cpp

