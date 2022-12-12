#include "Arduino.h"

/* 
  Establish a serial communication pass-through line (MCU-GPS unit) to monitor GPS signal
  with GNSS TOOLKIT (to link in this REPO), the tool used by the Air530Z module for testing and programming.
  
  Compile the code for the HTCC-AB02s board (Arduino IDE, Menu > Board: Cubecell > HTCC-AB02s).

  Upload the firmware, be sure the serial monitor is closed. Open the GNSS Toolkit and establish the connection
  by selecting the right port (eg on Windows COM3) with the leftmost button below the menu line. Be sure that the
  baudrate of the GPS unit and the GNSS Toolkit is the same. According to this sketch is 9600, but it can be faster.

  by Pierluigi Colangeli 12.12.2022
  
*/


#define TIMEOUT 10//time in ms
#define Vext    GPIO11
#define ON_OFF  GPIO8
#define Wake    GPIO9

#define LED     GPIO6
uint8_t buffer[64];
int count=0;

void VextON(void)
{
  pinMode(Vext,OUTPUT);
  digitalWrite(Vext, LOW);
}

void VextOFF(void) //Vext default OFF
{
  pinMode(Vext,OUTPUT);
  digitalWrite(Vext, HIGH);
}

void GPS_ON(void)
{
  pinMode(ON_OFF,OUTPUT);
  digitalWrite(ON_OFF, LOW);
}

void GPS_OFF(void) //Vext default OFF
{
  pinMode(ON_OFF,OUTPUT);
  digitalWrite(ON_OFF, HIGH);
}
void WAKE(void)
{
  pinMode(Wake,OUTPUT);
  digitalWrite(Wake, HIGH);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  VextON();// 
  delay(100);
  WAKE();
  delay(100);
  GPS_ON();
  delay(100);

  Serial1.begin(9600);


}

void loop()
{
    if (Serial1.available())                     // if date is coming from software serial port ==> data is coming from SoftSerial shield
    {
        while(Serial1.available())               // reading data into char array
        {
            buffer[count++]=Serial1.read();      // writing data into array
            if(count == 64)break;
        }
        Serial.write(buffer,count);                 // if no data transmission ends, write buffer to hardware serial port
        clearBufferArray();                         // call clearBufferArray function to clear the stored data from the array
        count = 0;                                  // set counter of while loop to zero 
    }
    if (Serial.available())                 // if data is available on hardware serial port ==> data is coming from PC or notebook
    Serial1.write(Serial.read());        // write it to the SoftSerial shield
}


void clearBufferArray()                     // function to clear buffer array
{
    for (int i=0; i<count;i++)
    {
        buffer[i]=NULL;
    }                      // clear all index of array with command NULL
}
