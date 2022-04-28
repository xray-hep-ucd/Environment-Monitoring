/*
 * Read and communicate temperature information from multiple adafruit MAX31865 RTD sensor amplifiers
 * Four RTDs are read and there outputs sent to serial
 * 
 * Two door sensors are read and the result of an AND of the two sensors output to serial
 * 
 * Serial port COM3 at BAUD 115200
 * 
 *
 */

#include <Adafruit_MAX31865.h>
#include <TimeLib.h>

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
#define DELAY_TIME_MS 5000 //Time between iterations of loop() in ms

int doorSensor1 = 2; //pin # of first sensor
int doorSensor2 = 3; //pin # of second sensor

void setup()
{
  Serial.begin(115200); //NB: Serial monitor must match this value or will display unintelligble symbols
  while(!Serial){ } //Wait until serial monitor has started
  delay(2000);

  Serial.println("Beginning Multi-RTD Readout");
  
  //Initialize the RTDs as the 2-wire variety
  thermo_1.begin(MAX31865_2WIRE); 
  thermo_2.begin(MAX31865_2WIRE);
  thermo_3.begin(MAX31865_2WIRE);
  thermo_4.begin(MAX31865_2WIRE);

  //Setup the door sensor pins
  pinMode(doorSensor1, INPUT_PULLUP);
  pinMode(doorSensor2, INPUT_PULLUP);
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
  Serial.print("RunDuration=");Serial.print(hour(t));Serial.print(":");Serial.print(minute(t));Serial.print(":");Serial.println(second(t));
  
  //Print temps to serial monitor
  Serial.print("Temp_1="); Serial.println(temp_1);
  Serial.print("Temp_2="); Serial.println(temp_2);
  Serial.print("Temp_3="); Serial.println(temp_3);
  Serial.print("Temp_4="); Serial.println(temp_4);

  //Check if any of the RTD boards had faults
  faultCheck();  

  //Check if all door sensors are closed
  if (digitalRead(doorSensor1) == LOW && digitalRead(doorSensor2) == LOW)
  {
    Serial.println("Door=CLOSED");
  }
  else
  {
    Serial.println("Door=OPEN");
  }

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

  
