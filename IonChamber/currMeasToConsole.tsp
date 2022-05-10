--Script to run on Keithley DMM6500 Multimeter to read ion chamber current and print to console
--Variable defining the run length, set each run
local DURATION_MIN = 0.5 
local INTERVAL = 2.0 --delay between measurements

reset()--Reset the instrument to the default settings.
dmm.measure.func = dmm.FUNC_DC_CURRENT --Set the measure function to DC current.

local duration_sec = DURATION_MIN * 60.0

local nMeas = (duration_sec / INTERVAL) + 1 -- num measurements to make

print("Current [Amps]")

--Make nMeas measurements of the current
local measNum = 0
while measNum < nMeas do
	print(dmm.measure.read())
	measNum = measNum + 1
	delay(INTERVAL)
end


print("Ending run")