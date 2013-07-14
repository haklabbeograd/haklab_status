/*
*   This program is free software; you can redistribute it and/or
*   modify it under the terms of the GNU General Public License
*   version 2 as published by the Free Software Foundation.  
*/
/**
 *  NRF24L01+ Sensor/Actuator Network master board
 *  
 *  This board is designed to serve as a master in a Senso/Actuator Network
 *  based on the NRF24L01+ modules. The master hadles the sensor registration,
 *  sets up the network ny telling each sensor to connect to a specific channel.
 * 
 *  It's intended to be conected/controlled via a PC or router over the USB 
 *  virtual COM port.
 *  
 *  The library used in this project for the NRF modules is RF24 
 *  http://maniacbug.github.io/RF24/ you can also find instructions on how
 *  connect the board there.
 */
 
//libraries used
#include <SPI.h>    //SPI
#include "RF24.h"   //RF24
#include "printf.h"
//header with network functions
#include "Board.h"


void setup(void)
{

    initialiseBoard();
    //Serial.println("\nFinished init");
}

void loop()
{    
    byte k =1;
    //Serial.println("\nNumber of Boards connected: ");
    //Serial.println(nBoards);
    
    //Reads through all the boards and their sensors    
    if(nBoards)
    {    
        //Serial.println("\nentered readout");
        for(int i = 0; i < nBoards; i++)
        {
            readAllSonBoard(&(Boards[i]));
            delay(250);
        }
        delay(2500);
    }
    
    //Detects new boards on the registration channel
    //If the board is new, adds it to the network
    //If the board is one that has lost connection attaches back
    if(newBoardAvailable())
    {
        Serial.println("entered registration");
        if(newBoardConnect())
        {
            if(newBoardDefine()) 
            {
                //Serial.println("\nBoard added: ");
            }
            else;
        }
        else;
    }
    else;
}

// vim:cin:ai:sts=2 sw=2 ft=cpp

