// this constant won't change:
const int doorPin = 12;  // the pin that the pushbutton is attached to


// Variables will change:
int doorState=0;         // current state of the button
int lastDoorState=0;     // previous state of the button


void setup() 
{
  // initialize the button pin as a input
  pinMode(doorPin, INPUT);
  pinMode(13, OUTPUT);

  // initialize serial communication
  Serial.begin(9600);
  doorState = digitalRead(doorPin);  
  lastDoorState = doorState;
}


void loop() 
{
  delay(2000);  

  // read the pushbutton input pin
  doorState = digitalRead(doorPin);
  if(doorState != lastDoorState)
  {
    lastDoorState = doorState;
    digitalWrite(13,doorState);
    Serial.println(doorState);
  }
}
