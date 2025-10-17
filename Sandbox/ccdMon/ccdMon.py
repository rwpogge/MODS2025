#
# ccdMon - monitor CCD cooldown and temperature control for Archon
#
# A simple TCP client to talk to the azcam server on port 2402
#
# Use: python ccdMon.py
#
# R. Pogge, OSU Astronomy Dept.
# pogge.1@osu.edu
#
# 2025 Oct 17
#

import time
from datetime import datetime, timezone, date, timedelta

import socket
import select

from pathlib import Path

#
# AzClient class for azcam tcp client 
#

class AzClient(object):

    def __init__(self):
        self.sock = None
        self.azHost = 'localhost'
        self.azPort = 2402
        self.timeout = 5.0
        self.msgLen = 1024
        self.encoding = 'UTF-8'
        self.repStr = ''

    def startClient(self):
        if self.sock is not None:
            self.sock.close()

        try:
            self.sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        except Exception as exp:
            print(f"Cannot create socket - {exp}")
            self.sock = None

        try:
            self.sock.connect((self.azHost,self.azPort))
        except Exception as exp:
            print(f"Cannot connect to azcam server - {exp}")
            self.sock.close()
            self.sock = None

    def stopClient(self):
        if self.sock is not None:
            self.sock.close()
            self.sock = None
            print("AzCam client stopped")
        else:
            print("AzCam client not running, nothing to stop...")

    def cmd(self,cmd,timeout=None):
        if timeout is None:
            timeout = self.timeout
        if self.sock is not None:
            cmdStr = cmd
            if not cmd.endswith("\n"):
                cmdStr += "\n"
            #print(f"IC>AZ {cmd}")
            self.sock.send(bytes(cmdStr,self.encoding))
            ready = select.select([self.sock],[],[],timeout)
            if ready[0]:
                rep = self.sock.recv(self.msgLen)
                self.repStr = rep.decode(self.encoding).strip()
                #print(f"AZ>IC {self.repStr}")
                return self.repStr
            else:
                raise TimeoutError(f"socket read timed out after {timeout:.1f} seconds")

    def procRep(self,repStr=None):
        if repStr is None:
            repStr = self.repStr
        if repStr.startswith("OK"):
            sep,code,msg = repStr.partition("OK")
            return msg.strip()
        elif repStr.startswith("ERROR:"):
            sep,code,msg = repStr.partition("ERROR:")
            return msg.strip()
        else:
            print(f'Unknown status tag: {repStr}')
            return

# obsDate

def obsDate():
    '''
    Return the observing date string

    Returns
    -------
    string
        observing date in CCYYMMD format, see description.

    Description
    -----------        
    Returns the observing date in CCYYMMDD format.  We define the
    an "observing date" as running from noon to noon local time.
    
    For example, the observing date for the night starting at sunset
    on 2025 Dec 15 and ending at sunrise on 2025 Oct 16 is `20251015`.

    We use obsDate for logs and data files.
    '''

    if float(datetime.now().strftime("%H")) < 12.0:  # before noon
        return (date.today() - timedelta(days=1)).strftime("%Y%m%d")
    else:
        return date.today().strftime("%Y%m%d")

# main

hostname = socket.gethostname()
modsID = hostname[:6]
cadence = 10 # seconds

# open logfile and start header

logFile = str(Path() / f"{modsID}_ccd.{obsDate()}.log")

LF = open(logFile,"a")
LF.write("#\n")
LF.write(f"# {modsID.upper()} CCD Cooldown Data\n")
LF.write("#\n")
LF.write(f"# Run started: {datetime.now(timezone.utc).strftime('%Y-%m-%dT%H:%M:%S')}\n")
LF.write("#\n")
hdrStr = "utcTime                    azcamT    ccdT   baseT  pidV  P       I D"
print(hdrStr)
LF.write(f"{hdrStr}\n")
LF.flush()

az = AzClient()
az.startClient()

try:
    while True:
        statStr = az.cmd("mods.archonStatus")
        bits = statStr.split()
        timeTag = datetime.now(timezone.utc).strftime('%Y-%m-%dT%H:%M:%S.%f')
        dataStr = f"{timeTag} {float(bits[2]):6.3f} {float(bits[3]):7.2f} {float(bits[4]):7.2f} {float(bits[5]):5.2f} {bits[6]} {bits[7]} {bits[8]}"
        print(dataStr)
        LF.write(f"{dataStr}\n")
        LF.flush()
        time.sleep(cadence)
except KeyboardInterrupt:
    LF.close()
    az.sock.close()
    exit(0)            

# done
