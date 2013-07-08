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

//
// Hardware configuration
//

DHT dht(DHTPIN, DHTTYPE);
RF24 radio(9,10);
boolean connected = false;
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
    {   //Connected: respond to Server commands
        
        //Testing function, disconects after 5s
        if(millis()-timerA > 5000)
            {
                connected = false;
                radio.setChannel( REGISTRATION_CH );
                Serial.println("\nTimer Conn has gone");
            }
    }
    else
    {
        connected = registerAndDefineBoard(&timerA, radio);
    }
}

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
