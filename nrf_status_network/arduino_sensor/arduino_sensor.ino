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
boolean applied = false;
byte packageTot[BOARD_NSENACT][32];
byte package[32];
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
  radio.setChannel( APPLICATION_CH );

  // optionally, reduce the payload size.  seems to
  // improve reliability
  radio.setPayloadSize(PAYLOAD_SIZE);
  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1,pipes[0]);
    

  //
  // Start listening
  //
    packBoard(package);
    for(int i = 0; i < BOARD_NSENACT; i++)
    {
        packSenAct(i, packageTot[i]);
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
                radio.setChannel( APPLICATION_CH );
                Serial.println("\nTimer Conn has gone");
            }
    }
    else
    {//if not connected senact needs to apply and define
        if(applied)
        {//if allready applied continue with defining
            byte k;
            if(readPackage(&k, 1,radio))
            {//read command from server
                if(k == 0xff)
                {//connected...
                    applied = false;
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
                applied = false;
                radio.setChannel( APPLICATION_CH );
                Serial.println("\nTimer has gone");
            }
        }
        else
        {//not applied, here's the application procces
        //SenActs keep sending their Board packages, server can read only one at a time.
        //When the server reads one, it sends the same package back as ack...
        //since all boards names are different, only the one that sent it will get it back
            radio.flush_tx();
            //Send board package
            if(writePackage(package, 32, radio))
            {
                byte testP[32];
                //read the response back and check against board package
                if(readPackageAck(testP, 32,package,32,radio))
                {   
                    radio.setChannel( DEFINITION_CH );  //change channel to definition chanel
                    byte k = 1;
                    if(writePackage(&k, 1,radio))       //send back 1 over def channel for ack
                    {
                        applied = true;
                        Serial.println("\nApplied");
                        timerA = millis();              //start deffinition timer
                    }
                    else
                    {//failed to send ack go back to application
                        radio.setChannel( APPLICATION_CH );
                        Serial.println("\nError 3");
                    }
                }
                else
                {//failed the pack ack, go back to applictaio
                    Serial.println("\nError 2");
                }
            }
            else
            {//failed to write, try again :)
                Serial.println("\nError 1");
            }
        }
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
