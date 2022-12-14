# GPS_LORA_TRACKER

Install the Heltec board on the Arduino IDE according to the "Board manager" guidelines available here:
https://docs.heltec.org/en/node/cubecell/quick_start.html

Quick link to the .json file:
https://github.com/HelTecAutomation/CubeCell-Arduino/releases/download/V1.4.0/package_CubeCell_index.json

Once installed, you'll be ready to properly compile and upload code for this PSoC.

## Establish serial communication

Use the USB serial bridge (CP2102) and the 6 pin adapter to program the board. Pin order from left to right is:

5V | TX | RX | RTS | DTR | GND

TX and RX are the transmission lines, RTS and DTR are used to automatically set the lora chip AM02 in boot mode, therefore ready to receive new firmware.

![UART](/Pics/UART.png)


## GNSS Toolkit: a tool to test the Air530z module

By uploading the Arduino sketch [GPS_GNSS_Toolkit_UART.ino](TESTS/GPS_GNSS_Toolkit_UART.ino), you'll be able to communicate with the GPS unit via the GNSS Toolkit3 software. The software is provided as a .zip file in this repository. Please be aware that the original language is chinese, but can the GUI can be viewed in english.
![GNSS_Toolkit](/Pics/GNSS_Toolkit.png)
![GNSS_Toolkit](/Pics/GNSS_Toolkit_language.png)

## LoRaWAN communication of geographical coordinates

The sketch [AM02_GPS_LORA_V01.ino](AM02_GPS_LORA_V01/AM02_GPS_LORA_V01.ino) allows the prototype to transmit coordinates via LoRa to the gateway.
Folder structure:

AM02_GPS_LORA_V01
    .
    ├── AM02_GPS_LORA_V01.ino   # this is the main sketch
    ├── readbattery.h           # header file with functions to read battery voltage
    └── TTN.h                   # header file containing all the TTN related objects (keys, duty cycle etc)

  
https://github.com/piecol/GPS_LORA_TRACKER/blob/8f8fe18ed2a30a6c96698fc1f8999d2fb00c988e/AM02_GPS_LORA_V01/AM02_GPS_LORA_V01.ino#L12-L16

When compiling, please use the following settings:
![Board_details](Pics/Board_details_LoRa_GPS.png)



