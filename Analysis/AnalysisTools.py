#Tools for analyzing monitoring data
#The code here matches that in AnalysisTools.ipynb but is intended for importable use e.g. from AnalysisTools import *
#whereas the notebook is intended for interactive analysis on Google Colab or on another Jupyter notebook server

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt


##-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
## Utility functions for time manipulation 

#Splits times of the format hh:mm:ss into components
#Return the hours, minutes, seconds of the passed time as ints
def splitTimes(timeStr):
    splitTs = timeStr.split(":")
    return int(splitTs[0]), int(splitTs[1]), int(splitTs[2])

#Return time in seconds
def asSeconds(time):
    hr, min, sec = splitTimes(time)
    return (hr*3600) + (min*60) + sec

#Return time in minutes
def asMinutes(time):
    hr, min, sec = splitTimes(time)
    return (hr*60) + min + (sec / 60.0)

#Subtract return time1-time2
#If asSec=False, returns as hh:mm:ss as a str
#If asSec=False, returns as total seconds as an int
def timeDiff(time1, time2, asSec=False):
    hr1, min1, sec1 = splitTimes(time1)
    hr2, min2, sec2 = splitTimes(time2)
    hrDiff = hr1 - hr2
    minDiff = min1 - min2
    secDiff = sec1 - sec2
    if secDiff < 0:
        minDiff -= 1
        secDiff += 60
    if minDiff < 0:
        hrDiff -= 1
        minDiff += 60
    
    timeDiff = str(hrDiff) + ":" + str(minDiff) + ":" + str(secDiff)
    if asSec:
        return asSeconds(timeDiff)
    else:
        return timeDiff

##-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#A class to store monitoring data from a run
#Data is stored in a Pandas Dataframe
class SensorRun:

    name = "" 
    filepath = ""
    data = None #A Pandas Dataframe storing the events
    nEvents = 0


    def __init__(self, name = ""):
        self.name = name


    #Attempt to read the file in filepath into a Pandas DF
    def readFile(self, filepath):
        self.filepath = filepath
        if self.name == "":
            self.name = filepath
        
        self.data = pd.read_csv(filepath, header=0, usecols=[0,1,2,3,4,5,6])
        self.nEvents = self.data.shape[0]
    

    #Append data in filepath to the current dataframe
    def addFile(self, filepath):
        if self.filepath == "":
            self.readFile(filepath)
        else:
            if isinstance(self.filepath, str):
                temp = self.filepath
                self.filepath = []
                self.filepath.append(temp)
            self.append(filepath)
            newDF = pd.read_csv(filepath, header=0, usecols=[0,1,2,3,4,5,6])
            self.data.append(newDF)


    #Drop events which are incomplete from the dataframe
    #if how="all", drops only rows which are entirely NA
    #if how="any" (default) drops all rows which have an NA
    #if resetIndex=True (default), index is reset to be sequential [0, nEvents-1],
    #if resetIndex=False otherwise leaves original row number as index
    def dropIncompleteEvents(self, how="any", resetIndex=True):
        self.data.dropna(inplace=True, how=how)
        self.nEvents = self.data.shape[0]
        if resetIndex:
            self.data.reset_index(inplace=True, drop=True)

    
    #Def drop unwanted rows or columns from the data
    #labels : a single label (e.g row number in index of column name) or list of such to drop
    #axis : 0= drop row(s) (default), 1= drop column(s)
    def drop(self, labels, axis=0, resetIndex = True):
        self.data.drop(labels, axis=axis, inplace=True)
        self.nEvents = self.data.shape[0]
        if resetIndex:
            self.data.reset_index(inplace=True, drop=True)


    #Return a list of column names
    def getCols(self):
        return self.data.columns


    #Retrieve a column from the dataframe
    def get(self, colName, timeUnits=""):
        return self.data[colName]


    #Add a column for time in specificed units, either "s" (seconds) (default) or "m" (minutes)
    def addTimeCol(self, units="s"):
        timeDefault = self.get("Time [hh:mm:ss]")
        if units == "s":
            timeNew = timeDefault.apply(asSeconds)
        elif units == "m":
            timeNew = timeDefault.apply(asMinutes)
        else:
            print("Unrecognized time units, use either 's' for seconds or 'm' for minutes")
            return
        self.data["Time [" + units + "]"] = timeNew


    #Return the total time duration of the run
    def duration(self, asSec=False, startInd=0, endInd=-1):
        if endInd < startInd:
            endInd = self.nEvents - 1
        startTime = self.data.at[startInd, "Time [hh:mm:ss]"]
        endTime = self.data.at[endInd, "Time [hh:mm:ss]"]
        return timeDiff(endTime, startTime, asSec)


    #Plot columns of data
    #columns : a single or list of temp columns to plott. Can be a single column name, a single column index, or a list of either
    #           Default is data.columns[1:6] (all four temps + pressure)
    #timeUnits : if 's' or 'm', plot vs time in seconds or minutes respectively. Otherwise, plot against index
    #subplots: Whether or not to give each column its own subplot.
    #kind: the type of plot to draw. defaults to line
    def plotCols(self, columns=None, timeUnits='s', subplots=True, kind='line'):

        #Set x axis to either be time or event number (index)
        if timeUnits != "s" and timeUnits != 'm':
            x = None
            xLabel = "Event #"
        else:
            timeStr = "Time [" + timeUnits + "]"
            if timeStr not in self.data.columns:
                self.addTimeCol(units=timeUnits)
            x = timeStr
            xLabel = timeStr

        #Choose which columns to plot
        colNames = self.getCols()
        if columns is None:
            cols = colNames[1:6]
            figsize = (12, 10)
        elif isinstance(columns, list):
            figsize = (10, 10)
            if isinstance(columns[0], str):
                cols = columns
            else:
                cols = []
                for colInd in columns:
                    cols.append(colNames[colInd])
        elif isinstance(columns, str):
            figsize = (5, 5)
            cols = columns
        elif isinstance(columns, int):
            figsize = (5, 5)
            cols = colNames[columns]

        self.data.plot(x=x, y=cols, kind=kind, subplots=subplots, figsize=figsize, title='Sensor Data, Run = "' + self.name + '"', xlabel=xLabel)

##-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#A class to hold data from the Ion Chamber readout
class IonChamberRun:
    name = ""
    filepath = ""
    nEvents = 0
    data = None
    rois = [] #List of boundaries [min, max] of regions of interest, portions of data when shutter was open (aka bright)
    darkRegs = [] #List of boundaries [min, max] of portions of data when shutter was closed (dark current aka noise)

    def __init__(self, name = ""):
        self.name = name

    #Attempt to read the file in filepath into a Pandas DF
    def readFile(self, filepath, header=0, skiprows=3):
        self.filepath = filepath
        if self.name == "":
            self.name = filepath.split("/")[-1]
        
        self.data = pd.read_csv(filepath, header=header, skiprows=skiprows )
        self.nEvents = self.data.shape[0]
    
    #Append data in filepath to the current dataframe
    def addFile(self, filepath, header=0, skiprows=3):
        if self.filepath == "":
            self.readFile(filepath)
        else:
            if isinstance(self.filepath, str):
                temp = self.filepath
                self.filepath = []
                self.filepath.append(temp)
            self.append(filepath)
            newDF = pd.read_csv(filepath, header=header, skiprows=skiprows )
            self.data.append(newDF)

    #Smooth data using a rolling average and add a new column to the dataframe
    def smooth(self, windowSize=10):
        newCol = self.data["Current [nA]"].rolling(windowSize).mean()
        self.data["Smoothed Current [nA]"] = newCol
        self.data.dropna()

    #Plot the current vs time
    def plot(self, kind="scatter", smooth=False, incROIs=False, figsize=(10, 5)):
        if smooth:
            if "Smoothed Current [nA]" not in self.data.columns:
                self.smooth()
            self.data.plot(x="Time [s]", y="Smoothed Current [nA]", kind=kind, figsize=figsize, title='Ion Chamber Current, Run = "' + self.name + '"', ylabel="Current [nA]")
        else:
            self.data.plot(x="Time [s]", y="Current [nA]", kind=kind, figsize=figsize, title='Ion Chamber Current, Run = "' + self.name + '"', ylabel="Current [nA]")
        if incROIs and (kind == "scatter" or kind == "line"):
            ax = plt.gca()
            yAxisLims = ax.get_ylim()
            fig = plt.gcf()
            for roi in self.rois:
                plt.plot([roi[0], roi[0]], [yAxisLims[0], yAxisLims[1]], color="darkred", linestyle="dashed")
                fig.show()
                plt.plot([roi[1], roi[1]], [yAxisLims[0], yAxisLims[1]], color="darkred", linestyle="dashed")
                fig.show()
            for roi in self.darkRegs:
                plt.plot([roi[0], roi[0]], [yAxisLims[0], yAxisLims[1]], color="darkmagenta", linestyle="dashed")
                fig.show()
                plt.plot([roi[1], roi[1]], [yAxisLims[0], yAxisLims[1]], color="darkmagenta", linestyle="dashed")
                fig.show()

    #Return the min, max, idx_min, idx_max of the specified column
    #col, the colum name to consider
    def getMinMax(self, col):
        column = self.data[col]
        min = column.min()
        max = column.max()
        idx_min = column.idxmin()
        idx_max = column.idxmax()
        return min, max, idx_min, idx_max

    #Set regions of interest for analysis
    #param roiTuples : a list of [min, max] boundaries or regions of interest
    #param dark : if true, region is marked as a dark current region not an ROI. defaults to False
    def addROIs(self, roiTuples, dark=False, clearPrev=False ):
        if not dark:
            if clearPrev:
                self.rois = []
            for roi in roiTuples:
                self.rois.append(roi)
        else:
            if clearPrev:
                self.darkRegs = []
            for roi in roiTuples:
                self.darkRegs.append(roi)

    #Return the average value and standard deviation for the specified ROI
    #rois = a list of regions of interest time tuples [(t_min1, t_max1), ...] to consider
    #if rois is None, defaults to self.rois. If self.rois is empty, uses all data
    #returns    mean, std dev, Num of points    of the region 
    def getStatsROI(self, col="Current [nA]", rois=None, dark=False):
        theROIs = []
        if rois != None:
            for roi in rois:
                theROIs.append(roi)
        elif not dark and len(self.rois) > 0:
            theROIs = self.rois
        elif dark and len(self.darkRegs) > 0:
            theROIs = self.darkRegs
        else:
            min, max, idx_min, idx_max = self.getMinMax(col="Time [s]")
            theROIs.append((min, max))

        combRegion = None
        for roiNum, roi in enumerate(theROIs):
            roiData = self.data[self.data["Time [s]"] >= roi[0]] #Select the time window of interest
            roiData = roiData[roiData["Time [s]"] <= roi[1]]
            roiData = roiData[col] #Only want one column of data

            if roiNum == 0: #Add data to combined region
                combRegion = roiData
            else:
                combRegion = pd.concat([combRegion, roiData])

        mean = combRegion.mean()
        stdDev = combRegion.std()
        nPts = len(combRegion.index)

        return mean, stdDev, nPts

    #Get the average shutter open (bright) current minus the average dark current and associated uncertainty
    def getCurrentDiff(self):
        if len(self.rois) < 1:
            print("WARNING: Please set at least one ROI first")
            pass
        if len(self.darkRegs) < 1:
            print("WARNING: Please set at least one ROI first")
            pass

        avg_bright, stdDev_bright, nPts_bright = self.getStatsROI(dark=False)
        avg_dark, stdDev_dark, nPts_dark = self.getStatsROI(dark=True)

        currDiff = avg_bright - avg_dark
        currDiffErr = stdDev_bright + stdDev_dark
        return currDiff, currDiffErr

    #Print stats cleanly
    def summarizeStats(self):
        avg_bright, stdDev_bright, nPts_bright = self.getStatsROI(dark=False)
        print("Bright Stats : Mean = " + str(avg_bright)[:5] + "nA, Std Dev = " + str(stdDev_bright)[:5] + "nA, Npts = " + str(nPts_bright))
        avg_dark, stdDev_dark, nPts_dark = self.getStatsROI(dark=True)
        print("Dark Stats : Mean = " + str(avg_dark)[:5] + "nA, Std Dev = " + str(stdDev_dark)[:5] + "nA, Npts = " + str(nPts_dark))
        currDiff, currDiffErr = self.getCurrentDiff()
        print("Dark subtracted current = " + str(currDiff)[:5] + " , error = " + str(currDiffErr)[:5])

##-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

# A function to make an IV curve for IC
#dir_path : Path to directory containing files.
#Note that files are assumed to have the form ivCurve_###V.csv
#tubeSettings : x-ray tube settings, current and voltage for plot labelling: e.g. "10 mA, 40 kV"
#requireApproval : if true, will show a plot of the rois and darkRegions and ask for approval
def makeIVcurve(dir_path, tubeSettings, defaultBrightRois=[(140, 290)], defaultDarkRegs=[(10, 100)], requireApproval=False):
    voltages = []
    currents = []
    errors = []

    for file in os.listdir(dir_path):
        voltage = file.split(".")[0].split("_")[1][:-1] #Extract voltage from the filename, expected format is ivCurve_###V.csv
        voltages.append(int(voltage))

        icRun = IonChamberRun()
        icRun.readFile(dir_path + file)
        

        rois=defaultBrightRois #Designate a portion of data as bright (shutter open)
        icRun.addROIs(rois, dark=False, clearPrev=True)
        darkRegs=defaultDarkRegs #Designate a portion of data to measure dark current (shutter closed)
        icRun.addROIs(darkRegs, dark=True, clearPrev=True)

        if requireApproval:
            print("\n The next plot...")
            icRun.plot(incROIs=True, figsize=(5, 4))
            plt.pause(1)
            approval = input("Are the default bright and dark regions ok? (y/n): ")
            if approval != "y":

                print("\nEnter the minima and maxima of new bright and dark regions")
                print("Starting with bright regions... Enter nothing when ready to continue to dark regions")
                newRois = []
                while(True):
                    min = input("Enter bright region minimum: ")
                    max = input("Enter bright region maximum: ")
                    if len(min) == 0 or len(max) == 0:
                        break
                    min = int(min)
                    max = int(max)
                    newRois.append((min, max))
                icRun.addROIs(newRois, dark=False, clearPrev=True)
                    
                print("Continuing with dark regions... Enter nothing when done entering dark regions")
                darkRegs = []
                while(True):
                    min = input("Enter dark region minimum: ")
                    max = input("Enter dark region maximum: ")
                    if len(min) == 0 or len(max) == 0:
                        break
                    min = int(min)
                    max = int(max)
                    darkRegs.append((min, max))
                icRun.addROIs(darkRegs, dark=True, clearPrev=True)
            print("The new regions...")
            icRun.plot(incROIs=True, figsize=(5, 4))
            plt.pause(1)
            plt.clf()

        currDiff, error = icRun.getCurrentDiff() #Get bright-dark current and associated error
        currents.append(currDiff)
        errors.append(error)


    fig = plt.figure(figsize=(15,10))
    plt.errorbar(x=voltages, y=currents, fmt='bo', yerr=errors, color="blue", ls='none')
    plt.title("Ion Chamber Response: Tube Settings = " + str(tubeSettings))
    plt.xlabel("Actual Voltage [V]")
    plt.ylabel("Current [nA]")
    
    fig.show()

    print("Voltages: " + str(voltages))
    print("Currents: " + str(currents))
    print("Current Uncertainties: " + str(errors))

##-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------