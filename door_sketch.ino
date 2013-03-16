#include <EEPROM.h>
// #define DEBUG 1

typedef void (*state_t)();

state_t current_state;
const int door_pin = 2;
const int wait_time = 1000 * 60 * 10;
long state_switch_time;
int door_state;

/** 
 * State declarations
 */
void state_locked();
void state_waiting_unlocked();
void state_pre_unlocked();
void state_unlocked();
void state_waiting_locked();
void state_pre_locked();

/**
 * state utils
 */

void store_state(state_t state)
{
	int addr = (int)state;
	uint8_t *ptr = (uint8_t*) &addr;
	for(int i = 0; i < sizeof(state_t); ++i)
	{
		EEPROM.write(i, *(ptr + i));
	}
}

state_t load_state()
{
	int addr;
	uint8_t *ptr = (uint8_t*) &addr;
	for(int i = 0; i < sizeof(state_t); ++i)
	{
		*ptr = EEPROM.read(i);
		++ptr;
	}
	return (state_t)addr;
}

void switch_state(state_t state)
{
	if(state == NULL)
		state = &state_locked;
	// primitive debouncing, lol
	delay(10);
	state_switch_time = millis();
	current_state = state;
	store_state(state);
}

/**
 * State definitions
 */

void state_locked()
{
#ifdef DEBUG
	Serial.println("LOCKED");
#endif
	if(digitalRead(door_pin) == HIGH)
	{
		switch_state(state_waiting_unlocked);
	}

}

void state_waiting_unlocked()
{
#ifdef DEBUG
	Serial.println("WAITING UNLOCKED");
#endif
	// if wait_time has passed
	if ((unsigned long)(millis() - (state_switch_time)) > wait_time)
	{
		switch_state(state_pre_unlocked);
	}
	else if(digitalRead(door_pin) == LOW) //locked
	{
		switch_state(state_locked);
	}
}

void state_pre_unlocked()
{
#ifdef DEBUG
	Serial.println("PRE-UNLOCKED");
#endif
	Serial.println("UNLOCKED");
	while(Serial.read() != 'S')
	{
		if(digitalRead(door_pin) == LOW) //locked
		{
			switch_state(state_locked);
			return;
		}
	}
	switch_state(state_unlocked);
}

void state_unlocked()
{
#ifdef DEBUG
	Serial.println("UNLOCKED");
#endif
	if(digitalRead(door_pin) == LOW) //locked
	{
		switch_state(state_waiting_locked);
	}
}

void state_waiting_locked()
{
#ifdef DEBUG
	Serial.println("WAITING LOCKED");
#endif
	// if wait_time has passed
	if ((unsigned long)(millis() - (state_switch_time)) > wait_time)
	{
		switch_state(state_pre_locked);
	}
	else if(digitalRead(door_pin) == HIGH) //unlocked
	{
		switch_state(state_unlocked);
	}
}

void state_pre_locked()
{
#ifdef DEBUG
	Serial.println("PRE-LOCKED");
#endif
	Serial.println("LOCKED");
	while(Serial.read() != 'S')
	{
		if(digitalRead(door_pin) == HIGH) //unlocked
		{
			switch_state(state_unlocked);
			return;
		}
	}
	switch_state(state_locked);
}

void setup ()
{
#ifdef DEBUG
	// Clear the state stored in the EEPROM
	for(int i = 0; i < sizeof(state_t); ++i)
		EEPROM.write(i, 0);
#endif
	Serial.begin(9600);
	Serial.setTimeout(100);
	//switch_state(load_state());
	switch_state(state_unlocked);
	pinMode(door_pin, INPUT);
}

void loop()
{

	if(current_state)
	{
		current_state();
	}
}
