/*
    http://maniacbug.github.io/RF24/
    uses the RF24 library, so connect up the board as per the isntruction on the link above..
    
    this is a sketch for a server board which reads sensors automaticalay, 
    senses new ones, and attaches them to the existing network...
    
 */

#include <SPI.h>
#include "RF24.h"
#include "Board.h"


void setup(void)
{
    initialiseBoard();
}

void loop()
{    
    byte k =1;
    Serial.print("\nNumber of Boards connected: ");
    Serial.println(nBoards);    
    if(nBoards)
    {    
        Serial.println("\nentered readout");
        for(int i = 0; i < nBoards; i++)
        {
        readAllSonBoard(&(Boards[i]));
        delay(250);
        }
        delay(2500);
    }
    
    
    if(unregBoardAvailable())
    {
        Serial.println("entered registration");
        if(newBoardConnect())
        {
            if(newBoardDefine()) 
            {
                Serial.println("\nBoard added: ");
            }
            else;
        }
        else;
    }
    else;
}

// vim:cin:ai:sts=2 sw=2 ft=cpp

