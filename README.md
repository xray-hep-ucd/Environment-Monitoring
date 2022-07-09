# Environment-Monitoring

Environment monitoring for x-ray system using [Arduino](arduino.cc), [Processing](processing.org), and Keithley TSP. Arduino code is written for an [Arduino MEGA 2560 REV3](https://store-usa.arduino.cc/products/arduino-mega-2560-rev3?selectedStore=us). Processing code uses the Java (8) mode. TSP code is written for a [Keithley DMM6500](https://www.tek.com/en/products/keithley/digital-multimeter/dmm6500).

## Integrated Monitoring
The "Integration" subdirectory contains the primary Arduino and Processing scripts which read out temperature, pressure, and door closure sensors, display the status of each to a visual monitor, and write data to a file. To run: Upload the Arduino sketch, envSensors.ino to the board. Set the desired filename and path at the top of environmentMonitor.pde and run the script. **IMPORTANT: To write the data to the file, click on the Processing canvas displaying the sensor status and hit the UP-arrow key. Without this step, data will not be written to the file.**

### Temperature
Temperature monitoring is done with pt100 RTDs interfaced with [Adafruit MAX31865 RTD PT100 Amplifiers](https://learn.adafruit.com/adafruit-max31865-rtd-pt100-amplifier). Boards should be soldered and wired according to the tutorial available from Adafruit. Arduino code to readout four RTDs is under the "rtds" subdirectory. Temperatures are written to serial output.

### Door Closure
The door to the x-ray system has several attached [GikFun magnetic sensors](https://www.amazon.com/Gikfun-Sensor-Magnetic-Switch-Arduino/dp/B0154PTDFI) to check for proper closure. These sensors are read out by the Arduino. One end of readout sensor should be connected to the Arduino's ground and the other, a digital pin. A simple Arduino sketch reading out door sensor states is under the "doorSensors" subdirectory.

### Barometric Pressure
Barometric pressure monitoring is done with with an [Adafruit MPL3115A2 barometer](https://www.adafruit.com/product/1893) and the associated Arduino library. The board was wired following the instructions from Adafruit.

## Ion Chamber Current
Current produced in an ion chamber placed along the x-ray beam is read out using a a [Keithley DMM6500](https://www.tek.com/en/products/keithley/digital-multimeter/dmm6500). Repeated, automated current measurements can be performed and the resulting data stored in three ways. The two .tsp scripts in the IonChamber subdirectory are meant to be run as TSP projects in the the Keithley Test Script Builder (TSB). The simplest script, "currMeasToConsole.tsp" makes repeated current measurements and writes them to the TSB console. The second .tsp script, "currMeasToUSB.tsp" writes data to a USB drive plugged into the front of the multimeter (or console). There is also a python version, "currMeas.py" that does not require the TSB and can be run as a standard python module.

## Analysis
Post-run analysis of sensor data from the Arduino-attached sensors and the ion chamber can be performed by using the tools in the Analysis subdirectory. The directory contains both a Jupyter notebook, configured by default to run on [Google Colab](https://colab.research.google.com/) and a .py python file containing the same utility classes and functions. To use the notebook, the main_path variable in the Setup code section should be modified to the user's path. The code in the .py file is meant to be an importable version of the code in the notebook for use in scripts.
