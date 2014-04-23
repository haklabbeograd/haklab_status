all: ArduinoSlaveGenerator

ArduinoSlaveGenerator: arduino_slave ArduinoSlaveGenerator.c functions.c functions.h
	g++ -Wall ArduinoSlaveGenerator.c functions.c -o ArduinoSlaveGenerator

arduino_slave:
	mkdir arduino_slave
