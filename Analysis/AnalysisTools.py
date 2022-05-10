#Tools for analyzing monitoring data
#The code here matches that in AnalysisTools.ipynb but is intended for importable use e.g. from AnalysisTools import *
#whereas the notebook is intended for interactive analysis on Google Colab or on another Jupyter notebook server

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt


##---------------------------------------------------------------------------------------##
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

##----------------------------------------------------------------------------------------##

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


#A class to hold data from the Ion Chamber readout
class IonChamberRun:
    name = ""
    filepath = ""
    nEvents = 0
    data = None

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

    #Plot the current vs time
    def plot(self, kind="line", figsize=(10, 5)):
        self.data.plot(x="Time [s]", y="Current [nA]", kind=kind, figsize=figsize, title='Ion Chamber Current, Run = "' + self.name + '"', ylabel="Current [nA]")
