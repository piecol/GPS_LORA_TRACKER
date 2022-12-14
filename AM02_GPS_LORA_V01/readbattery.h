#ifndef READBATTERY_H
#define READBATTERY_H

#define BATT_READ     GPIO15  // battery voltage reading mosfet

uint16_t batteryVoltage;

uint16_t readBattery() {

  pinMode(BATT_READ, OUTPUT);
  digitalWrite(BATT_READ, LOW);

    uint16_t sum = 0;
    for (int i=0; i<3; i++){    
        sum += analogRead(ADC)*4.33;
    }
    uint16_t batteryVoltage = sum /3;


    //uint16_t batteryVoltage = analogReadmV(ADC)*4.33;//*3.26; //*4.33

    return batteryVoltage;
    
  digitalWrite(BATT_READ, HIGH);
}

#endif
