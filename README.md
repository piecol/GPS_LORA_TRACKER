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



By uploading the Arduino sketch [GPS_GNSS_Toolkit_UART.ino](TESTS/GPS_GNSS_Toolkit_UART.ino), you'll be able to communicate with the GPS unit via the GNSS Toolkit3 software.
![GNSS_Toolkit](/Pics/GNSS_Toolkit.png)


The sketch [AM02_GPS_LORA_V01.ino](AM02_GPS_LORA_V01/AM02_GPS_LORA_V01.ino) will allow the prototype to transmit coordinates via LoRa to the gateway.
When compiling, please use the following settings:
![Board_details](Pics/Board_details_LoRa_GPS.png)



