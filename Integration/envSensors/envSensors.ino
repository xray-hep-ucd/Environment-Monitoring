/*
 * Read and communicate temperature information from multiple adafruit MAX31865 RTD sensor amplifiers
 * Four RTDs are read and there outputs sent to serial
 * 
 * Barometer and onboard temperature (room) are read using the Adafruit_MPL3115A2 sensor IC and library
 * 
 * Two door sensors are read and the result of an AND of the two sensors output to serial
 * 
 * Serial port COM3 at BAUD 115200
 * 
 *
 */

#include <Adafruit_MAX31865.h>
#include <Adafruit_MPL3115A2.h>
#include <TimeLib.h>


#define DELAY_TIME_MS 5000 //Time between iterations of loop() in ms


// Use software SPI: CS, DI, DO, CLK
//Adafruit_MAX31865 thermo_1 = Adafruit_MAX31865(10, 11, 12, 13);
// use hardware SPI, just pass in the CS pin
//Adafruit_MAX31865 thermo_1 = Adafruit_MAX31865(10);
//Current RTDs
//RTDs have different CS pins but share DI, DO, CLK pins 
Adafruit_MAX31865 thermo_1 = Adafruit_MAX31865(10, 11, 12, 13);
Adafruit_MAX31865 thermo_2 = Adafruit_MAX31865(9, 11, 12, 13);
Adafruit_MAX31865 thermo_3 = Adafruit_MAX31865(8, 11, 12, 13);
Adafruit_MAX31865 thermo_4 = Adafruit_MAX31865(7, 11, 12, 13);
#define RREF 430.0 // The value of the Rref resistor. Use 430.0 for PT100 and 4300.0 for PT100
#define RNOMINAL  100.0 // The 'nominal' 0-degrees-C resistance of the sensor, 100.0 for PT100, 1000.0 for PT1000

Adafruit_MPL3115A2 baro;

//int doorSensor1 = 2; //pin # of first sensor
//int doorSensor2 = 3; //pin # of second sensor

void setup()
{
  Serial.begin(115200); //NB: Serial monitor must match this value or will display unintelligble symbols
  while(!Serial){ } //Wait until serial monitor has started
  delay(2000);

  //Check if the barometer is connected
  if (!baro.begin()) {
    Serial.println("ERROR: Could not find barometer. Check wiring.");
    while(1);
  }
  
  Serial.println("Beginning Integrated Readout");
  
  //Initialize the RTDs as the 2-wire variety
  thermo_1.begin(MAX31865_2WIRE); 
  thermo_2.begin(MAX31865_2WIRE);
  thermo_3.begin(MAX31865_2WIRE);
  thermo_4.begin(MAX31865_2WIRE);

  // use to set sea level pressure for current location
  // this is needed for accurate altitude measurement
  // STD SLP = 1013.26 hPa
  baro.setSeaPressure(1013.26);

  //Setup the door sensor pins
//  pinMode(doorSensor1, INPUT_PULLUP);
//  pinMode(doorSensor2, INPUT_PULLUP);
}

void loop()
{
  
  //Print the duration of the run to this point [dd:hh:mm]
  time_t t = now();
  
  //Read in temps from the sensors
  float temp_1 = thermo_1.temperature(RNOMINAL, RREF);
  float temp_2 = thermo_2.temperature(RNOMINAL, RREF);
  float temp_3 = thermo_3.temperature(RNOMINAL, RREF);
  float temp_4 = thermo_4.temperature(RNOMINAL, RREF);
  //Check if any of the RTD boards had faults
  faultCheck();  


  //Get pressure and room temp from the barometer and print to serial
  float pressure = baro.getPressure(); // [hPa]
  //float altitude = baro.getAltitude(); // [m]
  float temp_baro = baro.getTemperature(); // [C]

  /*
  //Check if all door sensors are closed
  String doorStatus;
  if (digitalRead(doorSensor1) == LOW && digitalRead(doorSensor2) == LOW)
  {
    doorStatus = "CLOSED";
  }
  else
  {
    doorStatus = "OPEN";
  }
  */

  Serial.print("RunDuration=");Serial.print(hour(t));Serial.print(":");Serial.print(minute(t));Serial.print(":");Serial.println(second(t));
  Serial.print("Temp_1="); Serial.println(temp_1);
  Serial.print("Temp_2="); Serial.println(temp_2);
  Serial.print("Temp_3="); Serial.println(temp_3);
  Serial.print("Temp_4="); Serial.println(temp_4);
  Serial.print("Pressure="); Serial.println(pressure);
  Serial.print("Temp_baro="); Serial.println(temp_baro);
  //Serial.print("Door="); Serial.println(doorStatus);

  //Delay before next iteration
  delay(DELAY_TIME_MS);
}


//Check if the passed board had any faults and print them
//Since we don't have an array of boards, repeats code to check for faults with each board
void faultCheck()
{
  
 // Check and print any faults
  uint8_t fault = thermo_1.readFault();
  if (fault) {
    Serial.println("Fault with board 1");
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
      Serial.println("REFIN- < 0.85 xBias - FORCE- open"); 
    }
    if (fault & MAX31865_FAULT_RTDINLOW) {
      Serial.println("RTDIN- < 0.85 x Bias - FORCE- open"); 
    }
    if (fault & MAX31865_FAULT_OVUV) {
      Serial.println("Under/Over voltage"); 
    }
    thermo_1.clearFault();
  }

  fault = thermo_2.readFault();
  if (fault) {
    Serial.println("Fault with board 2");
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
      Serial.println("REFIN- < 0.85 xBias - FORCE- open"); 
    }
    if (fault & MAX31865_FAULT_RTDINLOW) {
      Serial.println("RTDIN- < 0.85 x Bias - FORCE- open"); 
    }
    if (fault & MAX31865_FAULT_OVUV) {
      Serial.println("Under/Over voltage"); 
    }
    thermo_2.clearFault();
  }

  fault = thermo_3.readFault();
  if (fault) {
    Serial.println("Fault with board 3");
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
      Serial.println("REFIN- < 0.85 xBias - FORCE- open"); 
    }
    if (fault & MAX31865_FAULT_RTDINLOW) {
      Serial.println("RTDIN- < 0.85 x Bias - FORCE- open"); 
    }
    if (fault & MAX31865_FAULT_OVUV) {
      Serial.println("Under/Over voltage"); 
    }
    thermo_3.clearFault();
  }

   fault = thermo_4.readFault();
  if (fault) {
    Serial.println("Fault with board 4");
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
      Serial.println("REFIN- < 0.85 xBias - FORCE- open"); 
    }
    if (fault & MAX31865_FAULT_RTDINLOW) {
      Serial.println("RTDIN- < 0.85 x Bias - FORCE- open"); 
    }
    if (fault & MAX31865_FAULT_OVUV) {
      Serial.println("Under/Over voltage"); 
    }
    thermo_4.clearFault();
  }
}

  
