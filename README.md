# Environment-Monitoring

Environment monitoring for x-ray system using [Arduino](arduino.cc) and [Processing](processing.org). Arduino code is written for an [Arduino MEGA 2560 REV3](https://store-usa.arduino.cc/products/arduino-mega-2560-rev3?selectedStore=us). Processing code uses the Java (8) mode.

## Integrated Monitoring
The "Integration" subdirectory contains the primary Arduino and Processing scripts which read out temperature, pressure, and door closure sensors, display the status of each to a visual monitor, and write data to a file. To run: Upload the Arduino sketch, envSensors.ino to the board. Set the desired filename and path at the top of environmentMonitor.pde and run the script. **IMPORTANT: To write the data to the file, click on the Processing canvas displaying the sensor status and hit the UP-arrow key. Without this step, data will not be written to the file.**

## Temperature
Temperature monitoring is done with pt100 RTDs interfaced with [Adafruit MAX31865 RTD PT100 Amplifiers](https://learn.adafruit.com/adafruit-max31865-rtd-pt100-amplifier). Boards should be soldered and wired according to the tutorial available from Adafruit. Arduino code to readout four RTDs is under the "rtds" subdirectory. Temperatures are written to serial output.

## Door Closure
The door to the x-ray system has several attached [GikFun magnetic sensors](https://www.amazon.com/Gikfun-Sensor-Magnetic-Switch-Arduino/dp/B0154PTDFI) to check for proper closure. These sensors are read out by the Arduino. One end of readout sensor should be connected to the Arduino's ground and the other, a digital pin. A simple Arduino sketch reading out door sensor states is under the "doorSensors" subdirectory.

## Barometric Pressure
Barometric pressure monitoring is done with with an [Adafruit MPL3115A2 barometer](https://www.adafruit.com/product/1893) and the associated Arduino library. The board was wired following the instructions from Adafruit.

