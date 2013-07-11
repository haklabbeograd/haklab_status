haklab_status
=============

## Twitter
## NRF24L01+ status network
This is a plug and play sensor or actuator network based on the Nordic nRF24L01+ 2.4GHz radio modules. The network is controlled with a single master board. 
### Master
The master automatically detects new sensors or actuators and adds them to the network. The idea is to connect the master board to a PC or a USB capable Router(WR703N) running a script that will request sensor data, and display it on a web interface.
### Slave
The slaves are designed to be as much plug and play as possible. Meaning that very little programing know how is needed to get them running. Code examples and wiring diagrams will be provided for the most common sensors. For sensors that don't have examples, a web interface will be provided where you can input your sensor information, and an Arduno sketch will be generated. The only thing the user will need to do is to code or paste the sensor reading function.
### Communication protocol
The protocol is a simple Master controlled request replay
#### Master side
On start up the Master scans the entire RF range for any devices that might use the same frequency range, and maps out the available channels. It then proceeds to listen on the **registration channel**, if a package is read its immediately written back on the same channel. As the package contains the slave's name, only the slave which sent it will understand the message.

Next both the Master and Slave switch to a **definition channel** where the master sends requests for information and the Slave replies. Once the Sensor is defined, the Master sends it the next available free channel, and sends it a command to connect to it. 

If there are boards already in the network the master connects to each boards channel and requests latest sensor data. This is done by sending a 0-100 digit which indicates the number of the sensor on the sensor board, which doubles as the read sensor command.

Once the master is done reading all the connected boards, it switches to the registration channel and listens to any new boards available.

If a sensor board gets disconnected for some reason, the master will pick it up from the registration channel and reconnect it back to it's original channel
#### Slave side
At start up the slave switches to the **registration channel** and continuously sends out its name. Since many slaves are doing this at the same time, the master is able to read only one package, and it will replay it back. If a sensor receives a package with it's own information it will then proceed to the definition channel. Here the Master will request all it's sensor data(commands 0x00-0xEF), following which it will send the sensor the new channel to which it should connect(0xF0). The connection is made when the master send the connect command (0xFF).

Once connected the master will request data from the sensors on the sensor board (commands 0x00-0xEF). If no command is received for a preset time the sensor will disconnect and go back to the registration stage. 
### Things to do
####The control script
####The Web interface for generating the Slave Arduino sketch from sensor data.
####The Web interface where the sensor data is displayed, and where the actuator control is possible.
