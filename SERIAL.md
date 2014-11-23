Serial communication
====================

The serial port can be used to control the master arduino component, and get data from it. The master can relay messages between its serial port and slave components.

## Serial commands
####Available serial commands

    RAS                         - Read all sensors
    LAS                         - List all sensors
    SAS <board number> <value>  - Send a value to a board

#### The format the arduino responds in
RAS command:

    <sensor name> <value>
    <sensor name> <value>
    ...

LAS command:

    Register <board name> <board number> <sensor name> <sensor number>
    Register <board name> <board number> <sensor name> <sensor number>
    ...
