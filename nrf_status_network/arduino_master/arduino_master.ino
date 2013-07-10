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
    //mapFreeCH();
    nFreeCH = 2;
    freeCH[0] = 70;
    freeCH[1] = 72;
    nextFreeCH = 0;
}

void loop()
{    
    //Serial.print(nFreeCH);
    Serial.println("\npassed setup");
    byte k =1;
        
    if(nBoards)
    {    
        Serial.println("\nentered readout");
        for(int i = 0; i < nBoards; i++)
        {
        readAllSonBoard(&(Boards[i]));
        delay(2500);
        }
    }
    
    
    if(nFreeCH && unregBoardAvailable())
    {
        Serial.println("entered registration");
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

