--Script to run on Keithley DMM6500 Multimeter to read and store
--ion chamber current


--Reset the instrument to the default settings.
reset()

--Set the measure function to DC current.
dmm.measure.func = dmm.FUNC_DC_CURRENT

--Read the voltage value.
print(dmm.measure.read())