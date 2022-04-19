/*
 * Read and store temperature information from multiple adafruit MAX31865 RTD sensor amplifiers
 * 
 * Based off Adafruit max31865 example code
 * 
 * 
 */

#include <Adafruit_MAX31865.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

// Use software SPI: CS, DI, DO, CLK
//Adafruit_MAX31865 thermo_1 = Adafruit_MAX31865(10, 11, 12, 13);
// use hardware SPI, just pass in the CS pin
//Adafruit_MAX31865 thermo_1 = Adafruit_MAX31865(10);

//Current RTDs
//RTDs share DI, DO, CLK pins but have different CS pins
Adafruit_MAX31865 thermo_1 = Adafruit_MAX31865(10, 11, 12, 13);
Adafruit_MAX31865 thermo_2 = Adafruit_MAX31865(9, 11, 12, 13);
Adafruit_MAX31865 thermo_3 = Adafruit_MAX31865(8, 11, 12, 13);
Adafruit_MAX31865 thermo_4 = Adafruit_MAX31865(7, 11, 12, 13);

#define RREF 430.0 // The value of the Rref resistor. Use 430.0 for PT100 and 4300.0 for PT100
#define RNOMINAL  100.0 // The 'nominal' 0-degrees-C resistance of the sensor, 100.0 for PT100, 1000.0 for PT1000
#define DELAY_TIME_MS 5000 //Time between iterations of loop() in ms

void setup()
{
  Serial.begin(115200); //NB: Serial monitor must match this value or will display unintelligble symbols
  while(!Serial){ } //Wait until serial monitor has started
  
  Serial.println("Beginning Multi-RTD Readout");
  time_t initTime = now();
  Serial.print("Year="); Serial.println(year(t));
  Serial.print("Month="); Serial.println(month(t)); 
  Serial.print("Day="); Serial.println(day(t));

  //Initialize the RTDs as the 2-wire variety
  thermo_1.begin(MAX31865_2WIRE); 
  thermo_2.begin(MAX31865_2WIRE);
  thermo_3.begin(MAX31865_2WIRE);
  thermo_4.begin(MAX31865_2WIRE);
}

void loop()
{
  Serial.println();
  //Read in temps from the sensors
  float temp_1 = thermo_1.temperature(RNOMINAL, RREF);
  float temp_2 = thermo_2.temperature(RNOMINAL, RREF);
  float temp_3 = thermo_3.temperature(RNOMINAL, RREF);
  float temp_4 = thermo_4.temperature(RNOMINAL, RREF);

  //Print the current time 
  time_t t = now();
  Serial.print("Time= ");Serial.print(hour(t));Serial.print(":");Serial.print(min(t));Serial.print(":");Serial.println(second(t));
  
  //Print temps to serial monitor
  Serial.print("Temp_1 = "); Serial.println(temp_1);
  Serial.print("Temp_2 = "); Serial.println(temp_2);
  Serial.print("Temp_3 = "); Serial.println(temp_3);
  Serial.print("Temp_4 = "); Serial.println(temp_4);
  
  //Check if any of the boards had faults
  faultPrint(thermo_1, 1);
  faultPrint(thermo_2, 2);
  faultPrint(thermo_3, 2);
  faultPrint(thermo_4, 4);
  
  delay(DELAY_TIME_MS)
}


//Check if the passed board had any faults and print them
//@param thermo : the board object
//@param thermoNum : a unique identifer for the board for output labelling purposes
void faultPrint(Adafruit_MAX3185 thermo, int thermoNum)
{
 // Check and print any faults
  uint8_t fault = thermo.readFault();
  if (fault) {
    Serial.print("Fault with board # "); Serial.println(thermoNum);
    Serial.print("Fault 0x"); Serial.println(fault, HEX);
    if (fault & MAX31865_FAULT_HIGHTHRESH) {
      Serial.println("RTD High Threshold"); 
    }
    if (fault & MAX31865_FAULT_LOWTHRESH) {
      Serial.println("RTD Low Threshold"); 
    }
    if (fault & MAX31865_FAULT_REFINLOW) {
      Serial.println("REFIN- > 0.85 x Bias"); 
    }
    if (fault & MAX31865_FAULT_REFINHIGH) {
      Serial.println("REFIN- < 0.85 x Bias - FORCE- open"); 
    }
    if (fault & MAX31865_FAULT_RTDINLOW) {
      Serial.println("RTDIN- < 0.85 x Bias - FORCE- open"); 
    }
    if (fault & MAX31865_FAULT_OVUV) {
      Serial.println("Under/Over voltage"); 
    }
    thermo.clearFault();
  }
}

  
