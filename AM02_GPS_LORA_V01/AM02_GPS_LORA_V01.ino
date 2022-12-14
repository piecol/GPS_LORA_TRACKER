/* The example is for CubeCell_GPS,
   GPS works only before lorawan uplink, the board current is about 45uA when in lowpower mode.

  EDIT:
  - works with 1.3.0 CubeCell dev framework

  - locate C:\Users\__YOUR_USER_NAME___\AppData\Local\Arduino15\packages\CubeCell\hardware\CubeCell\1.3.0\libraries\OnBoardGPS\src\GPS_Air530Z.h
  - edit line 10 by replacing the powerCtl pin GPIO14 with GPIO11
  e.g. -> Air530ZClass(uint8_t powerCtl=GPIO11,uint8_t modePin=GPIO11); \\ this is because on my custom board the Vext is mapped to GPIO11
*/

#include "LoRaWan_APP.h"
#include "Arduino.h"
#include "TTN.h"
#include "GPS_Air530Z.h" // WARNING!, YOU EDITED THIS FILE (pwrctl on GPIO11, was IO14)
#include "readbattery.h"

Air530ZClass GPS;

#define LED     GPIO6  // on-board led
#define ON_OFF  GPIO8  // controls the power to the GPS unit
#define Wake    GPIO9  // signals to GPS unit to enter/exit ultra-low power mode
#define Vext    GPIO11 // controls power to the I2C line 


//when gps waked, if in GPS_UPDATE_TIMEOUT, gps not fixed then into low power mode
#define GPS_UPDATE_TIMEOUT 120000

//once fixed, GPS_CONTINUE_TIME later into low power mode
#define GPS_CONTINUE_TIME 300

int32_t fracPart(double val, int n)
{
  return (int32_t)((val - (int32_t)(val)) * pow(10, n));
}

void VextON(void)
{
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, LOW);
}

void VextOFF(void) 
{
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, HIGH);
}

void GPS_ON(void)
{
  pinMode(ON_OFF, OUTPUT);
  digitalWrite(ON_OFF, LOW);
}

void GPS_OFF(void) 
{
  pinMode(ON_OFF, OUTPUT);
  digitalWrite(ON_OFF, HIGH );
}

void GPS_WAKE(void)
{
  pinMode(Wake, OUTPUT);
  digitalWrite(Wake, HIGH);
}

void GPS_SLEEP(void)
{
  pinMode(Wake, OUTPUT);
  digitalWrite(Wake, LOW);
}

void printGPSInfo()
{
  Serial.print("Date/Time: ");
  if (GPS.date.isValid())
  {
    Serial.printf("%d/%02d/%02d", GPS.date.year(), GPS.date.day(), GPS.date.month());
  }
  else
  {
    Serial.print("INVALID");
  }

  if (GPS.time.isValid())
  {
    Serial.printf(" %02d:%02d:%02d.%02d", GPS.time.hour(), GPS.time.minute(), GPS.time.second(), GPS.time.centisecond());
  }
  else
  {
    Serial.print(" INVALID");
  }
  Serial.println();

  Serial.print("LAT: ");
  Serial.print(GPS.location.lat(), 6);
  Serial.print(", LON: ");
  Serial.print(GPS.location.lng(), 6);
  Serial.print(", ALT: ");
  Serial.print(GPS.altitude.meters());

  Serial.println();

  Serial.print("SATS: ");
  Serial.print(GPS.satellites.value());
  Serial.print(", HDOP: ");
  Serial.print(GPS.hdop.hdop());
  Serial.print(", AGE: ");
  Serial.print(GPS.location.age());
  Serial.print(", COURSE: ");
  Serial.print(GPS.course.deg());
  Serial.print(", SPEED: ");
  Serial.println(GPS.speed.kmph());
  Serial.println();
}

static void prepareTxFrame( uint8_t port )
{
  /*appData size is LORAWAN_APP_DATA_MAX_SIZE which is defined in "commissioning.h".
    appDataSize max value is LORAWAN_APP_DATA_MAX_SIZE.
    if enabled AT, don't modify LORAWAN_APP_DATA_MAX_SIZE, it may cause system hanging or failure.
    if disabled AT, LORAWAN_APP_DATA_MAX_SIZE can be modified, the max value is reference to lorawan region and SF.
    for example, if use REGION_CN470,
    the max value for different DR can be found in MaxPayloadOfDatarateCN470 refer to DataratesCN470 and BandwidthsCN470 in "RegionCN470.h".
  */

  float lat, lon, alt, course, speed, hdop, sats;

  Serial.println("Waiting for GPS FIX ...");

  VextON();     // power up sensors, can be skipped if no additional sensor is attached to the I2C line
  delay(100);
  GPS_WAKE();   // wake GPS unit from ultra-low power mode
  delay(100);
  GPS_ON();     // power up GPS unit
  delay(100);


  GPS.begin(9600);

  uint32_t start = millis();
  while ( (millis() - start) < GPS_UPDATE_TIMEOUT )
  {
    while (GPS.available() > 0)
    {
      GPS.encode(GPS.read());
    }
    // gps fixed in a second
    if ( GPS.location.age() < 1000 )
    {
      break;
    }
  }

  //if gps fixed,  GPS_CONTINUE_TIME later stop GPS into low power mode, and every 1 second update gps, print and display gps info
  if (GPS.location.age() < 1000)
  {
    start = millis();
    uint32_t printinfo = 0;
    while ( (millis() - start) < GPS_CONTINUE_TIME )
    {
      while (GPS.available() > 0)
      {
        GPS.encode(GPS.read());
      }

      if ( (millis() - start) > printinfo )
      {
        printinfo += 1000;
        printGPSInfo();
      }
    }
  }
  else
  {
    Serial.println("No GPS signal");
    delay(2000);
  }
  GPS.end();
  
  VextOFF(); // power off
  //GPS_SLEEP(); // TO BE TESTED !!!
  GPS_OFF();

  lat = GPS.location.lat();
  lon = GPS.location.lng();
  alt = GPS.altitude.meters();
  //course = GPS.course.deg();
  //speed = GPS.speed.kmph();
  sats = GPS.satellites.value();
  hdop = GPS.hdop.hdop();

  uint16_t batteryVoltage = readBattery();

  unsigned char *puc;

  appDataSize = 0;
  puc = (unsigned char *)(&lat);
  appData[appDataSize++] = puc[0];
  appData[appDataSize++] = puc[1];
  appData[appDataSize++] = puc[2];
  appData[appDataSize++] = puc[3];
  puc = (unsigned char *)(&lon);
  appData[appDataSize++] = puc[0];
  appData[appDataSize++] = puc[1];
  appData[appDataSize++] = puc[2];
  appData[appDataSize++] = puc[3];
  /*
    puc = (unsigned char *)(&alt);
    appData[appDataSize++] = puc[0];
    appData[appDataSize++] = puc[1];
    appData[appDataSize++] = puc[2];
    appData[appDataSize++] = puc[3];

    puc = (unsigned char *)(&course);
    appData[appDataSize++] = puc[0];
    appData[appDataSize++] = puc[1];
    appData[appDataSize++] = puc[2];
    appData[appDataSize++] = puc[3];
  */
  puc = (unsigned char *)(&sats);
  appData[appDataSize++] = puc[0];
  appData[appDataSize++] = puc[1];
  appData[appDataSize++] = puc[2];
  appData[appDataSize++] = puc[3];

  puc = (unsigned char *)(&hdop);
  appData[appDataSize++] = puc[0];
  appData[appDataSize++] = puc[1];
  appData[appDataSize++] = puc[2];
  appData[appDataSize++] = puc[3];
  appData[appDataSize++] = (uint8_t)(batteryVoltage >> 8);
  appData[appDataSize++] = (uint8_t)batteryVoltage;

  Serial.print("SATS: ");
  Serial.print(GPS.satellites.value());
  Serial.print(", HDOP: ");
  Serial.print(GPS.hdop.hdop());
  Serial.print(", LAT: ");
  Serial.print(GPS.location.lat());
  Serial.print(", LON: ");
  Serial.print(GPS.location.lng());
  Serial.print(", AGE: ");
  Serial.print(GPS.location.age());
  Serial.print(", ALT: ");
  Serial.print(GPS.altitude.meters());
  Serial.print(" BatteryVoltage:");
  Serial.println(batteryVoltage);

  digitalWrite(LED, HIGH);
  delay(10);
  digitalWrite(LED, LOW);
  delay(50);
}


void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);

#if(AT_SUPPORT)
  enableAt();
#endif
  deviceState = DEVICE_STATE_INIT;
  LoRaWAN.ifskipjoin();
}

void loop()
{
  switch ( deviceState )
  {
    case DEVICE_STATE_INIT:
      {
#if(LORAWAN_DEVEUI_AUTO)
        LoRaWAN.generateDeveuiByChipID();
#endif
#if(AT_SUPPORT)
        getDevParam();
#endif
        printDevParam();
        LoRaWAN.init(loraWanClass, loraWanRegion);
        deviceState = DEVICE_STATE_JOIN;
        break;
      }
    case DEVICE_STATE_JOIN:
      {
        LoRaWAN.join();
        break;
      }
    case DEVICE_STATE_SEND:
      {
        prepareTxFrame( appPort );
        LoRaWAN.send();
        deviceState = DEVICE_STATE_CYCLE;
        break;
      }
    case DEVICE_STATE_CYCLE:
      {
        // Schedule next packet transmission
        txDutyCycleTime = appTxDutyCycle + randr( 0, APP_TX_DUTYCYCLE_RND );
        LoRaWAN.cycle(txDutyCycleTime);
        deviceState = DEVICE_STATE_SLEEP;
        break;
      }
    case DEVICE_STATE_SLEEP:
      {
        LoRaWAN.sleep();
        break;
      }
    default:
      {
        deviceState = DEVICE_STATE_INIT;
        break;
      }
  }
}
