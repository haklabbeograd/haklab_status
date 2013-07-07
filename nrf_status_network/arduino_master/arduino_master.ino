/*
    http://maniacbug.github.io/RF24/
    uses the RF24 library, so connect up the board as per the isntruction on the link above..
    
    this is a sketch for a server board which reads sensors automaticalay, 
    senses new ones, and attaches them to the existing network...
    
 */

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"
#include "Board.h"
#define PAYLOAD_SIZE 32


// Set up nRF24L01 radio on SPI bus plus pins 9 & 10 
RF24 radio(9,10);

Board Boards[MAX_N_BOARDS];
SenAct SenActs[MAX_N_SENACT];

unsigned char nBoards = 0;
unsigned char nSenActs = 0;

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 
  0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

void setup(void)
{
  Serial.begin(57600);
  printf_begin();

  radio.begin();

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);

  // optionally, reduce the payload size.  seems to
  // improve reliability
  radio.setPayloadSize(PAYLOAD_SIZE);
  radio.openReadingPipe(1,pipes[1]);

  //
  // Start listening
  //

  radio.startListening();

  //radio.printDetails();
}

void loop()
{
    byte k =1;
    for(int i = 0; i < nBoards; i++)
    {
    //readAllSonBoard(&(Boards[i]), radio);
    //delay(2500);
    }
    
    if((nBoards < MAX_N_BOARDS) && (nSenActs < MAX_N_SENACT))
    {
    }
    
    
}

boolean addNewBoard()
    {
        byte newBoard[32], package[32];
        radio.setChannel( APPLICATION_CH );
        
        if(readPackage(newBoard, 32, radio))
        {
            Serial.println((char *)newBoard);
            if(writePackage(newBoard, 32, radio))
            {
                radio.setChannel( DEFINITION_CH );                
                if(readPackageAck(package,1,&k,1,radio))
                {
                    Serial.println("\nUspeh!!!!");
                    unpackBoard(&Boards[nBoards],newBoard);
                    Boards[nBoards].arraySenAct =  &SenActs[nSenActs];
                    delay(100);
                    for(int ss =0; ss< 2;ss++)
                    {    
                        k=ss;
                        if(writePackage(&k, 1, radio))
                        {
                            unsigned char tempNsenAct = nSenActs;
                            if(readPackage(package,32,radio))
                            {
                                unpackSenAct( &SenActs[nSenActs],package);
                                nSenActs=nSenActs++;
                            }
                            else
                            {
                                nSenActs = tempNsenAct;
                                Serial.println("\n erro in reading Senact pack");
                            }                            
                        }
                        else
                        {
                            Serial.println("\nError in write k");
                        }
                    }
                    nBoards++;
                    k=0xff;
                    if(writePackage(&k, 1, radio))Serial.println("\nNew Board Connected");
                    else Serial.println("\nError in write conn");
                    radio.setChannel( APPLICATION_CH );
                }
                else
                {
                    radio.setChannel( APPLICATION_CH );
                }
            }
            else
            {
                Serial.println("Error 1");
            }
        }
        else
        {
            //Serial.println("Error 0");
        }        
}
// vim:cin:ai:sts=2 sw=2 ft=cpp

