--Script to run on Keithley DMM6500 Multimeter to read ion chamber current and print to console

local DURATION_MIN = 0.5 --Run length in minutes
local INTERVAL = 2.0 --delay between measurements
local TO_FILE = true --If true, write to file, else write to console
local FILENAME = "IonChamberOutputs/testRun.csv" --Output filename/path to write to

reset()--Reset the instrument to the default settings.
dmm.measure.func = dmm.FUNC_DC_CURRENT --Set the measure function to DC current.

local duration_sec = DURATION_MIN * 60.0
local nMeas = (duration_sec / INTERVAL) + 1 -- num measurements to make

--check if USB drive is present and user specified to write to a file
TO_FILE = file.usbdriveexists() == 1 and TO_FILE

--Print header to either output file or console
print("Beginning run...")
if TO_FILE then 
	outfile = file.open(FILENAME, file.MODE_WRITE)
	file.write(outfile, "Time [s],Current [Amps]\n")
	print("Will write to file: " .. FILENAME)
else 
	print("Will NOT write to file, writing to console...")
	print("Time [s],Current [Amps]")
end

--Make the measurements
local measNum = 0
while measNum < nMeas do
	time = measNum * INTERVAL --current time since start in sec
	curr = dmm.measure.read()
	outStr = time .. "," .. curr
	if TO_FILE then
		file.write(outfile, outStr .. "\n")
	else
		print(outStr)
	end
	
	measNum = measNum + 1
	delay(INTERVAL)
end	

--clean up
print("Ending run...")
if TO_FILE then
	file.close(outfile)
	print("Data written to file: " .. FILENAME)
end