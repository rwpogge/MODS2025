#!/usr/bin/env python
'''
Make long-term measurements with the new quadcell board
in the MODS2025 HEB prototype

see measQC.py for the stripchart version

'''

import sys
from time import sleep
from datetime import datetime
import time

import warnings
warnings.filterwarnings('ignore',category=UserWarning,append=True)

#----------------------------------------------------------------
#
# utility functions
#

def getQCs(client,addr):
    '''
    get raw data from the quad cell
    '''
    numAI = 4
    qcData = []
    try:
        rd = client.read_holding_registers(addr,numAI)
    except Exception as ex:
        print(f"[{datetime.now(datetime.UTC).isoformat()}] *** Warning: Cannot read quad cell board - {ex}")
        for i in range(numAI):
            qcData.append(-999.99)
        return qcData
    
    qcData = rd.registers
    return qcData

def qc2vdc(rawQC):
    '''
    Convert raw quadcell data to a voltage (0..10VDC).
    At 15-bits for the positive side, the digitization scale is ~0.3mV
    '''
    posMax = 2**15 - 1
    negMin = 2**16 - 2
    if rawQC > posMax:
        Vout = 10.0*((rawQC-negMin)/posMax)
    else:
        Vout = 10.0*(rawQC/posMax)
    return Vout

def getRTDs(client,addr,num):
    temp = []
    try:
        rd = client.read_input_registers(addr,num)
    except Exception as ex:
        print(f"[{datetime.utcnow().isoformat()}] WARNING: Cannot read RTD module - {ex}")
        for i in range(num):
            temp.append(-99.99)
        return False,temp

    for i in range(num):
        temp.append(ptRTD2C(float(rd.registers[i])))

    return True,temp

def ptRTD2C(rawRTD):
    tempRes = 0.1
    tempMax = 850.0
    wrapT = tempRes*((2.0**16)-1)

    temp = tempRes*rawRTD
    if temp > tempMax:
        temp -= wrapT

    return temp

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

wagoHost = '192.168.139.141'
qcAddr = 0
rtdAddr = 4

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

# log file

nt = datetime.now()
runDate = nt.strftime("%Y%m%d")

qcFile = f"qcData_{runDate}.csv"

QCLog = open(qcFile,'w')
QCLog.write("time,Tair,qc1,qc2,qc3,qc4\n")

#--------------------------------
#
# Back to the main program
#

cadence = 5.0 # measurement cadence in seconds

# One measurement each to get things started

qcData = getQCs(client,qcAddr)
qc1 = qc2vdc(qcData[0])
qc2 = qc2vdc(qcData[1])
qc3 = qc2vdc(qcData[2])
qc4 = qc2vdc(qcData[3])
status,temp = getRTDs(client,rtdAddr,1)
t0 = time.time() # datetime.now()

tMeas = 0.0
status
logStr = f"{tMeas:.6f},{temp[0]:.1f},{qc1:.5f},{qc2:.5f},{qc3:.5f},{qc4:.5f}"
#logStr = f"{tMeas:.6f},{qcData[0]},{qcData[1]},{qcData[2]},{qcData[3]}"
print(f"{logStr}")
QCLog.write(f"{logStr}\n")

while(True):
    sleep(cadence)
    qcData = getQCs(client,qcAddr)
    qc1 = qc2vdc(qcData[0])
    qc2 = qc2vdc(qcData[1])
    qc3 = qc2vdc(qcData[2])
    qc4 = qc2vdc(qcData[3])
    status,temp = getRTDs(client,rtdAddr,1)
    # dt = datetime.now() - t0
    # tMeas = dt.seconds + 1.e-6*dt.microseconds
    tMeas = time.time() - t0
    
    logStr = f"{tMeas:.6f},{temp[0]:.1f},{qc1:.5f},{qc2:.5f},{qc3:.5f},{qc4:.5f}"
    #logStr = f"{tMeas:.6f},{qcData[0]},{qcData[1]},{qcData[2]},{qcData[3]}"
    print(f"{logStr}")
    QCLog.write(f"{logStr}\n")

sys.exit(0)
