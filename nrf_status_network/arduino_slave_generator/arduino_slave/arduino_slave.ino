/*
*   This program is free software; you can redistribute it and/or
*   modify it under the terms of the GNU General Public License
*   version 2 as published by the Free Software Foundation.  
*/
/**
 * NRF24L01+ Sensor/Actuator Network basic slave board
 *  
 * This board is designed to serve as a basic slave in a Senso/Actuator Network
 * based on the NRF24L01+ modules. It reads the status of a single pin, 
 * and forwads this status when the Master asks for it
 *  
 * The library used in this project for the NRF modules is RF24 
 * http://maniacbug.github.io/RF24/ you can also find instructions on how
 * connect the board there.
 */
 
#include <SPI.h>
#include "RF24.h"
#include "Board.h"


//
// Hardware configuration
//
boolean connected = false;  
unsigned long timerA;       //Timeout Timer

void setup(void)
{
    //
    // Setup and configure the board
    //
    initialiseBoard();
    //
    //Enter senosr setup here
    //
}

void loop()
{    
    if(connected)
    {   //Connected: respond to Server commands
        byte command;
        
        //value[0]=digitalRead(doorPin);      //read the pin
        //packValue(&value[0], 1, 0);  //pack the value from the original type to byte array
        packAllSensor();
        
        if(commandReceved(&command))//read and parse master command
        {
            parseCommand(command);
            timerA = millis();              //resets timer
        }
        else
        {
            Serial.println("no command receved");
        }
        
        //Timer function disconects after 50s if no command is receved
        if(millis()-timerA > 10000)
            {
                connected = false;
                Serial.println("\nTimer Conn has gone");
            }
    }
    //Not connected: Register, Define, and Connect
    else
    {
        connected = registerAndDefineBoard(&timerA);
    }
}

byte Value[1][8];

boolean packSensor1()
{
	boolean value = readSensor1();
	void * p = &value;
	for(int i = 0; i<1;i++)
		Value[0][i] = *((byte*)p + i);
}

boolean packAllSensor()
{
	packSensor1();
}

boolean readSensor1()
{//enter code here to read the test named sensor
}
// vim:cin:ai:sts=2 sw=2 ft=cpp
