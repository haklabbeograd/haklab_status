/*
http://maniacbug.github.io/RF24/
uses the RF24 library so connect up the nRF24 modules as per the link above...
this skect is of a sensor board....
*/

#include <stdio.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"
#include "Board.h"
#include "DHT.h"

#define DHTPIN 8     // what pin we're connected to
#define PAYLOAD_SIZE 32

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11 

DHT dht(DHTPIN, DHTTYPE);

//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10

RF24 radio(9,10);
boolean connected = false;
boolean registered = false;
//byte packageTot[BOARD_NSENACT][32];
//byte package[32];

unsigned long timerA;
// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

void setup(void)
{
  //
  // Setup and configure rf radio
  //
  Serial.begin(57600);
  printf_begin();
  radio.begin();
  dht.begin();
  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);
  radio.setChannel( REGISTRATION_CH );

  // optionally, reduce the payload size.  seems to
  // improve reliability
  radio.setPayloadSize(PAYLOAD_SIZE);
  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1,pipes[0]);
    

  //
  // Start listening
  //
    packBoard();
    for(int i = 0; i < BOARD_NSENACT; i++)
    {
        packSenAct(i);
    }
    radio.startListening();
    radio.printDetails();
}


void loop()
{
    if(connected)
    {//Connected respond to Server commands
        //Testing function, disconects after 5s
        if(millis()-timerA > 5000)
            {
                connected = false;
                radio.setChannel( REGISTRATION_CH );
                Serial.println("\nTimer Conn has gone");
            }
    }
    else
    {//if not connected senact needs to apply and define
        if(registered)
        {
            if(connected = defineBoard(&registered, &TimerA, radio))
            {
                registered = false;
                timerA = millis();//TEST
            }
            //Definition timer
            //goes back to register mode, if not connected within 5s
            if(millis()-timerA > 5000)
            {
                registered = false;
                radio.setChannel( REGISTRATION_CH );
                Serial.println("\nTimer has gone");
            }
        }
        else
        {
            if(registerBoard(RF24 radioX))
            {
                registered = true;
                timerA = millis();
            }
        }
    }
}

/*{   //if allready registered continue with defining
            byte k;
            if(readPackage(&k, 1,radio))
            {//read command from server
                if(k == 0xff)
                {//connected...
                    registered = false;
                    connected = true;
                    Serial.println("Connected");
                    timerA = millis(); //TEST
                }
                if(k == 0xf0)
                {//new channel setup...
                }                
                if(k < 0xf0)
                {   //send SenAct package #k to server
                    if(writePackage(packageTot[k], 32, radio))
                        Serial.println("\nSent #k SenACt");
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
            
            //Definition timer
            //goes back to apply mode, if not connected within 5s
            if(millis()-timerA > 5000)
            {
                registered = false;
                radio.setChannel( REGISTRATION_CH );
                Serial.println("\nTimer has gone");
            }
        }
        */



/*
void loop()
{
    if(connected)
    {
    }
    else
    {
        if(applyBoard(radio))connected = 1;
    }
}
*/

/*
void loop(void)
{
  
  // if there is data ready
  while(!radio.available() );
    
  float value;
  printf("usao");
  // Dump the payloads until we've gotten everything
  unsigned char nOfSensor;
  radio.read( &nOfSensor, 1);
  printf("Got payload %d...",nOfSensor);
    
  // First, stop listening so we can talk
  radio.stopListening();
  if(nOfSensor == 0)
  {
    value = dht.readHumidity();
    Serial.print("Hum:");
    Serial.println(value);
  }
  else if (nOfSensor == 1)
  {
    value = dht.readTemperature();
    Serial.print("Temp:");
    Serial.println(value);
  }
  else value = 0;
    
  while(!radio.write( &value, 4));
    // Now, resume listening so we catch the next packets.
  radio.startListening();
}
*/
// vim:cin:ai:sts=2 sw=2 ft=cpp
