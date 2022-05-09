#Script to make multiple current measurements from Keithley DMM6500
#Uses the pyvisa-py implementation to interact with the TSP controlled instrument

DURATION_MIN = 0.5 #How long to collect measurements for in minutes
INTERVAL = 1.0 #Interval between measurements in seconds
FILENAME = "c:/Users/xrayh/Desktop/IonChamberOutputs/testRun.csv" # File to write data to


import pyvisa
import time

resMan = pyvisa.ResourceManager()
instAddress = resMan.list_resources()[0] # The address if the inst (e.g USB0::0x05E6::0x6500::04403629::INSTR)
inst = resMan.open_resource(instAddress)
inst.timeout = 10000

inst.write("reset()") # reset the inst to defaults
inst.write("dmm.measure.func = dmm.FUNC_DC_CURRENT") # Set measure function to current

with open(FILENAME, "a") as file:
    print("Beginning run...")
    print("Data will be written to " + FILENAME)
    
    #Write a file header
    file.write("Beginning run...\n")
    startTime = time.strftime("%d%b%y %H:%M:%S") #
    file.write("Start time: " + startTime + "\n\n")
    file.write("Time [s],Current [Amps]\n")

    #Make the measurements
    nMeas = int(((DURATION_MIN * 60.0) / INTERVAL) + 1)
    for measNum in range(nMeas):
        t = measNum * INTERVAL #Time since start of run
        inst.write("curr = dmm.measure.read()")
        curr = inst.query("print(curr)")
        try: #Not every query returns a valid measurement (time delay in communication/measurement)
            float(curr[:-1])
            file.write(str(t) + "," + curr)
        except:
            pass
        
        time.sleep(INTERVAL)
       



