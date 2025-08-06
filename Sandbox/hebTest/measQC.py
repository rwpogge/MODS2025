#!/usr/bin/env python
'''
Read and plot quadcell on a WAGO HEB prototype
'''

import sys, os, math, logging
from time import sleep
import datetime

import warnings
warnings.filterwarnings('ignore',category=UserWarning,append=True)

#----------------------------------------------------------------
def getQCs(client,addr):
    numAI = 4
    qcData = []
    try:
        rd = client.read_holding_registers(addr,count=numAI)
    except Exception as ex:
        print(f"[{datetime.datetime.now(datetime.UTC).isoformat()}] *** Warning: Cannot read quad cell board - {ex}")
        for i in range(numAI):
            qcData.append(-999.99)
        return qcData
    
    qcData = rd.registers
    return qcData

def qc2vdc(rawQC):
    posMax = 2**15 - 1
    negMin = 2**16 - 2
    if rawQC > posMax:
        Vout = 10.0*((rawQC-negMin)/posMax)
    else:
        Vout = 10.0*(rawQC/posMax)
    return Vout

#----------------------------------------------------------------
'''
Main program, of sorts.  

We're being very slovenly here and defining the modbus TCP client
instance before we define the getRTDs() function so we don't have to
pass it the client handle.  Just asking for trouble...
'''

# Set up the Modbus I/O client as a synchronous TCP client

from pymodbus.client import ModbusTcpClient as mbc

# This is our WAGO unit on the ISL private subnet when the WAGO is
# being tested standalone outside a DESI IEB in the lab at OSU - check
# IP, may vary from unit to unit, but this is typical.

m1rHost = '192.168.139.141' # MODS1 Red HEB
m1bHost = '192.168.139.142' # MODS1 Blue HEB
m2rHost = '192.168.139.241' # MODS2 Red HEB
m2bHost = '192.168.139.242' # MODS2 Blue HEB

wagoHost = m2bHost
modsID = "MODS2B"

# WAGO register address of the 8-channel AI module

qcAddr = 0

# Instantiate the client - interestingly the initializer does not
# obviously raise exceptions if there are issues, e.g., if we give it
# a bogus IP address.  That comes later when we try to connect

try:
    client = mbc(wagoHost)
except:
    print(f"*** ERROR: Could not initiate WAGO Modbus client on {wagoHost}")
    sys.exit(1)

# connect or fail

if not client.connect():
    print(f"*** ERROR: Could not connect to WAGO {wagoHost}")
    sys.exit(1)

# matplotlib strip_chart_demo.py was used as the starting point,
# but heavily modified.

import numpy as np
import matplotlib
from matplotlib.lines import Line2D
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import matplotlib.dates as mdates

# Font and line weight defaults for axes for readability

lwidth = 1.0
pltFont = {'family':'sans',
           'color':'black',
           'weight':'normal',
           'size': 12,
           }
matplotlib.rcParams.update({'font.size':12})
matplotlib.rc('axes',linewidth=lwidth)

#----------------------------------------------------------------
'''
StripChart class - based on the matplotlib demo

This does the dirty work.

Consists of an initializer and an update() method.

started out simple from strip_chart_demo.py and got all complicated...
'''

class StripChart:
    def __init__(self, fig, ax,
                 mint=10.,
                 maxt=60.,
                 dt=0.02,
                 y0=(0.0,0.0,0.0,0.0),
                 dy0=5.0,
                 title=f'{modsID} QuadCell Signal',
                 geom=(1600,512)):
        self.fig = fig
        self.ax = ax
        self.dt = dt
        self.dy0 = dy0
        qcMn = np.min(y0)
        qcMx = np.max(y0)
        dya = qcMx - qcMn
        self.qcMin = qcMn - 0.1*dya
        self.qcMax = qcMx + 0.1*dya
        self.mint = mint
        self.maxt = maxt
        self.tdata = [datetime.datetime.now(datetime.UTC)] # [0]
        self.ydata1 = [y0[0]]
        self.ydata2 = [y0[1]]
        self.ydata3 = [y0[2]]
        self.ydata4 = [y0[3]]
        self.qcID = ['QC1','QC2','QC3','QC4']
		
        self.line1 = Line2D(self.tdata,self.ydata1,color='black',label=self.qcID[0],lw=1,ds='steps') 
        self.ax.add_line(self.line1)

        self.line2 = Line2D(self.tdata,self.ydata2,color='brown',label=self.qcID[1],lw=1,ds='steps')
        self.ax.add_line(self.line2)

        self.line3 = Line2D(self.tdata,self.ydata3,color='red',label=self.qcID[2],lw=1,ds='steps')
        self.ax.add_line(self.line3)

        self.line4 = Line2D(self.tdata,self.ydata4,color='orange',label=self.qcID[3],lw=1,ds='steps')
        self.ax.add_line(self.line4)
		
        self.ax.set_ylim(self.qcMin,self.qcMax)
        self.ax.set_xlim(self.tdata[0],(self.tdata[0]+datetime.timedelta(seconds=1.025*self.mint)))
        self.legend = self.ax.legend(loc='upper center',ncols=4,fontsize=10)
        self.ax.set_title(title,fontsize=12)
        self.ax.set_xlabel('UTC Time',fontsize=12)
        self.ax.set_ylabel('QC Out [VDC]',fontsize=12)
        self.fig.autofmt_xdate()
        self.ax.xaxis.set_major_formatter(mdates.DateFormatter('%H:%M:%S'))
        dpi = self.fig.get_dpi()
        self.fig.set_size_inches(float(geom[0])/float(dpi),
                                 float(geom[1])/float(dpi),forward=True)

        self.fig.tight_layout()
		
    def update(self, y):
        lastt = self.tdata[-1]
        dt = self.tdata[-1]-self.tdata[0]
        dtTest = (dt.seconds+(dt.microseconds/1.0e6))  # seconds elapsed
        if dtTest > self.maxt: # slide the arrays in time
            self.tdata.pop(0)
            self.ydata1.pop(0)
            self.ydata2.pop(0)
            self.ydata3.pop(0)
            self.ydata4.pop(0)
            self.ax.set_xlim(self.tdata[0],(self.tdata[0]+datetime.timedelta(seconds=1.025*self.maxt)))
        elif dtTest < self.maxt and dtTest > self.mint:
            self.ax.set_xlim(self.tdata[0],(self.tdata[0]+datetime.timedelta(seconds=1.025*dtTest)))
        else:
            self.ax.set_xlim(self.tdata[0],(self.tdata[0]+datetime.timedelta(seconds=1.025*self.mint)))

        tnow = datetime.datetime.now(datetime.UTC)
        if (y[0]>-999 and y[1]>-999 and y[2]>-999 and y[3]>-999):  # trap bad reads
            self.tdata.append(tnow)
            self.ydata1.append(y[0])
            self.ydata2.append(y[1])
            self.ydata3.append(y[2])
            self.ydata4.append(y[3])
			
        self.line1.set_data(self.tdata, self.ydata1)
        self.line2.set_data(self.tdata, self.ydata2)
        self.line3.set_data(self.tdata, self.ydata3)
        self.line4.set_data(self.tdata, self.ydata4)
        minY = min(np.min(self.ydata1),np.min(self.ydata2),np.min(self.ydata3),np.min(self.ydata4))
        maxY = max(np.max(self.ydata1),np.max(self.ydata2),np.max(self.ydata3),np.max(self.ydata4))

        dY = math.fabs(maxY-minY)
        self.qcMin = minY-0.1*dY
        self.qcMax = maxY+0.1*dY
        self.ax.set_ylim(self.qcMin,self.qcMax)
        self.ax.figure.canvas.draw()

        # annotation

        med = np.median(self.ydata1)
        std = np.std(self.ydata1)
        self.legend.get_texts()[0].set_text(f'{self.qcID[0]}={y[0]:.5f} [{med:.5f}+/-{std:.5f}]')
        med = np.median(self.ydata2)
        std = np.std(self.ydata2)
        self.legend.get_texts()[1].set_text(f'{self.qcID[1]}={y[1]:.5f} [{med:.5f}+/-{std:.5f}]')
        med = np.median(self.ydata3)
        std = np.std(self.ydata3)
        self.legend.get_texts()[2].set_text(f'{self.qcID[2]}={y[2]:.5f} [{med:.5f}+/-{std:.5f}]')
        med = np.median(self.ydata4)
        std = np.std(self.ydata4)
        self.legend.get_texts()[3].set_text(f'{self.qcID[3]}={y[3]:.5f} [{med:.5f}+/-{std:.5f}]')
        
        return self.line1,self.line2,self.line3,self.line4

#----------------------------------------------------------------
#
# emitter() - how the animator gets updated data
#
# This reads the WAGO RTDs, convert to deg C, and emits
# this as a duple to the animator
#

def emitter():

    numRead = 1
    readSleep = 1.0
    #WAGO temps

    qc1 = 0
    qc2 = 0
    qc3 = 0
    qc4 = 0
    for i in range(numRead):
        qcData = getQCs(client,qcAddr)
        qc1 += qc2vdc(qcData[0])
        qc2 += qc2vdc(qcData[1])
        qc3 += qc2vdc(qcData[2])
        qc4 += qc2vdc(qcData[3])
        if i < numRead-1:
            sleep(readSleep)

    if numRead > 1:
        qc1 /= numRead
        qc2 /= numRead
        qc3 /= numRead
        qc4 /= numRead
        
    yield (qc1,qc2,qc3,qc4)
    
#--------------------------------
#
# Back to the main program
#

timeStep  = 500.0 # time step in milliseconds
minWidth  = 60.0  # show at least this many seconds at the start
timeWidth = 300. # max time to show in the moving window in seconds (e.g., 3600, 1800, 600)
#timeWidth = 10.0 # max time to show in the moving window in seconds (e.g., 3600, 1800, 600)
minHeight = 5.0   # minimum height of the window, +/-0.5*minHeight 

# One measurement each to get things started

qcData = getQCs(client,qcAddr)
qc1 = qc2vdc(qcData[0])
qc2 = qc2vdc(qcData[1])
qc3 = qc2vdc(qcData[2])
qc4 = qc2vdc(qcData[3])

t = datetime.datetime.now(datetime.UTC)

# Get us a plot window

fig, ax = plt.subplots()

# Instantiate a StripChart instance

chart = StripChart(fig,ax,
                   dt=timeStep,
                   mint=minWidth,
                   maxt=timeWidth,
                   y0=(qc1,qc2,qc3,qc4),
                   dy0=minHeight)

# Start up the function animator, passing it the chart.update() method
# and the emitter function that reads the WAGO

ani = animation.FuncAnimation(fig, chart.update, emitter, blit=True, interval=timeStep)

plt.show()
