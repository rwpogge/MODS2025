'''
dataMan - MODS data post-processor agent

Usage
-----
    dataMan [configFile]

Parameters
----------
    configFile : string
        optional configuration file to use instead of the default
        
Description
-----------

This is a receive-only UDP socket server.  It accepts and processes
commands sent by a client, but makes no reply.  All activities
are logged.

Author
------
R. Pogge, OSU Astronomy Dept. (pogge.1@osu.edu)

Modification History
--------------------
 * 2025 Aug 29 - start of development, test server/client stubs w/threading
 * 2025 Oct 16 - added logging, config file, paths, and FITS handling
 * 2025 Oct 31 - boo! Started header processing coding
 
'''

import os
import sys
import socket
import threading
import time
import datetime

# astropy.io for FITS file handling

from astropy.io import fits

# astropy.table for FITS binary table handling

from astropy.table import Table

# pathlib for path handling

from pathlib import Path

# logging for runtime logging

import logging

# We use yaml for runtime config

import yaml

# shutil for high-level file operations (copy and move)

import shutil

# Methods

def loadConfig(cfgFile):
    '''
    Load a YAML format runtime configuration file

    Parameters
    ----------
    cfgFile : string
        Name (including full path) of the YAML runtime config file.

    Raises
    ------
    RuntimeError
        returned if file could not be opened or loaded.
    ValueError
        returned if the file does not exist

    Returns
    -------
    config : dict
        Dictionary with the configuration file contents.

    '''

    if os.path.exists(cfgFile):
        try:
            stream = open(cfgFile,'r')
        except Exception as exp:
            raise RuntimeError(f"ERROR: could not open {cfgFile}: {exp}")
        try:
            config = yaml.safe_load(stream) #,Loader=Loader)
        except Exception as exp:
            raise RuntimeError(f"ERROR: could not load {cfgFile}: {exp}")
        
        if len(config)==0:
            raise RuntimeError(f"File file {cfgFile} is empty or incorrectly formatted")
        
        return config
    else:
        raise ValueError(f"Runtime configuration file {cfgFile} does not exist")


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

    if float(datetime.datetime.now().strftime("%H")) < 12.0:  # before noon
        return (datetime.date.today() - datetime.timedelta(days=1)).strftime("%Y%m%d")
    else:
        return datetime.date.today().strftime("%Y%m%d")


def modsFITSProc(fitsFile,repoDir):
    '''
    Process a MODS FITS image

    Parameters
    ----------
    fitsFile : string
        name of the raw MODs FITS file to process
    repoDir : string
        full path of the LBTO new data repository directory
    
    Returns
    -------
    None.


    Description
    -----------
    Processes a raw MODS FITS file and pushes the  processed file into the repository.
    Setup to be called by threading.Thread() to allow concurrency in processing
    data files.
    
    If the image doesn't exist or any processing step fails, a report may be found
    in the runtime logs.  We do not delete or allow overwriting existing data.
    
    Processing Steps
    ----------------
    We limit the processing to those items that require exposure to the time-critical
    or evanescent state of the data-acquisition system.  Processes that can be done
    at relative leisure offline should be done offline, as steps here ultimately
    delay arrival of the data where it is needed, e.g., for target acquisition,
    so time is of the essence.  This limits us to
     * items we must fix before the data are presented to on-site observers
     * items we must fix before the data are presented to the data archive
     * items that can only be fixed using data available on the acquisition machine
     
    Once processing steps are completed, the last step is to copy the processed
    FITS file to the observatory "newdata" repository which is where it may be
    accessed by observers and SciOps personnel on site.  From newdata, the images
    are transferred to the repository and queued for ingestion into the LBTO Data
    Archive.  This latter places a requirement on fixing any FITS header issues
    that might inhibit ingestion into the data archive.
    '''

    if Path(fitsFile).exists():
        logger.info(f"proc: processing {fitsFile}...")

        try:
            hdu = fits.open(fitsFile)
        except:
            logger.error(f"could not open {origFits} - {exp}")
            return

        if procParam["fixROI"]:
            # fix DETSEC and CCDSEC per quadrant

            xc=int(hdu[0].header['ref-pix1'])
            yc=int(hdu[0].header['ref-pix2'])
            nx=hdu[1].header['naxis1']
            ny=hdu[1].header['naxis2']

            # compute corrected DETSEC and LVTn parameters

            detsec1=f"[{xc-nx+1}:{xc},{yc-ny+1}:{yc}]"
            detsec2=f"[{xc+nx}:{xc+1},{yc-ny+1}:{yc}]" # flip x
            detsec3=f"[{xc-nx+1}:{xc},{yc+1}:{yc+ny}]"
            detsec4=f"[{xc+nx}:{xc+1},{yc+1}:{yc+ny}]" # flip x
            q24_LTV1 = xc+nx+1
            q34_LTV2 = -yc

            # we need to fix DETSEC *and* CCDSEC

            hdu[1].header['detsec'] = (detsec1,'Corrected DETSEC')
            hdu[2].header['detsec'] = (detsec2,'Corrected DETSEC')
            hdu[3].header['detsec'] = (detsec3,'Corrected DETSEC')
            hdu[4].header['detsec'] = (detsec4,'Corrected DETSEC')
            hdu[1].header['ccdsec'] = (detsec1,'Corrected CCDSEC')
            hdu[2].header['ccdsec'] = (detsec2,'Corrected CCDSEC')
            hdu[3].header['ccdsec'] = (detsec3,'Corrected CCDSEC')
            hdu[4].header['ccdsec'] = (detsec4,'Corrected CCDSEC')

            # fix LTVn so ds9 cursor returns correct physical pixel coords
            # in Q2,Q3, and Q4 (Q1 is OK)

            hdu[2].header['LTV1'] = (q24_LTV1,'Corrected LTV1')
            hdu[4].header['LTV1'] = (q24_LTV1,'Corrected LTV1')
            hdu[3].header['LTV2'] = (q34_LTV2,'Corrected LTV2')
            hdu[4].header['LTV2'] = (q34_LTV2,'Corrected LTV2')

            hdu[0].header['HISTORY'] = "[dataMan] Updated ROI DETSEC and CCDSEC"
            
        # archon status snapshot to augment/correct header
        
        t = Table(hdu[5].data)
        archonDict = dict(zip(t['Keyword'].tolist(),t['Value'].tolist()))

        # fix CCDTEMP and BASETEMP
   
        try:
            ccdTemp = float(archonDict[procParam['ccdTemp']])
        except:
            ccdTemp = -999.9 # no-read value
        hdu[0].header['CCDTEMP'] = (ccdTemp,'CCD Detector Temperature [deg C]')

        try:
            baseTemp = float(archonDict[procParam['baseTemp']])
        except:
            baseTemp = -999.9 # no-read value
        hdu[0].header['BASETEMP'] = (baseTemp,'CCD Mount Base Temperature [deg C]')

        hdu[0].header['HISTORY'] = "[dataMan] Updated CCDTEMP and BASETEMP"
                             
        # write new data to ...

        # close original and done
        
        hdu.close()
        logger.info(f"proc done: {fitsFile} copied to {repoDir}")
        
    else:
        logger.error(f"proc: no such file {fitsFile}")
        
#---------------------------------------------------------------------------
#
# Basic info and defaults
#

hostname = socket.gethostname()
modsID = hostname[:6]
    
dmHost = 'localhost'
dmPort = 10301

dataDir = "/home/data"
procDir = "/home/data/Proc"
repoDir = "/lbt/data/new"
logDir = "/home/Logs/dataMan"

configPath = "/home/dts/Config"
defaultCfg = "dataman.ini"

#---------------------------------------------------------------------------
#
# Start of the code
#

# process command-line arguments.

if len(sys.argv)-1 == 0:
    cfgFile = str(Path() / configPath / defaultCfg)
elif len(sys.argv)-1 == 1:
    cfgFile = sys.argv[1]
    if not os.path.exists(cfgFile):
        # try adding the default configuration path
        cfgFile = str(Path() / configPath / sys.argv[1])
        if not os.path.exists(cfgFile):
            print(f"ERROR: could not find {cfgFile} in pwd or {str(configPath)}")
            sys.exit(1)
else:
    print("Usage: dataMan [cfgFile]")
    sys.exit(0)

# Load and parse the runtime configuration file

try:
    cfg = loadConfig(cfgFile)
except Exception as exp:
    print(f"ERROR: (loadConfig): {exp}")
    sys.exit(1)

# Process runtime configuration parameters

dmHost = cfg["server"]["ipAddr"]
dmPort = cfg["server"]["ipPort"]

# data and log directory paths

dataDir = Path(cfg["paths"]["dataDir"])
procDir = Path(cfg["paths"]["procDir"])
repoDir = str(Path(cfg["paths"]["repoDir"]))
logDir = Path(cfg["paths"]["logDir"])

# image processing parameters

procParam = cfg["processing"]

# other runtime flags

if cfg["debug"]:
    logLevel = logging.DEBUG
else:
    logLevel = logging.INFO
    
# Start runtime logging

logFile = str(Path(logDir) / f"{modsID}.{obsDate()}.txt")

logging.basicConfig(filename=logFile,
                    format="%(asctime)s %(name)s %(levelname)s: %(message)s",
                    filemode="a",
                    level=logLevel)

logger = logging.getLogger("dataMan")

logger.info(f"Started dataMan for {modsID}")

# Initialze the datagram (udp) socket 

logger.info(f"Initializing the {modsID} dataMan server UDP socket")

try:
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    logger.info("UDP socket created")
except Exception as err:
    logger.exception(f"Failed to create UDP socket - {err}")
    sys.exit(-1)

# Bind the socket to the localhost IP address and port

try:
    s.bind((dmHost,dmPort))
except Exception as err:
    logger.exception(f"UDP socket binding failed - {err}")
    s.close()
    sys.exit(-1)
        
logger.info(f"{modsID} dataMan server started on {dmHost}:{dmPort}")

# Listen for remote client directives, including quit

while 1:
    
    # receive data from client
    
    d = s.recvfrom(1024)
    
    # decode it
    
    cmdStr = d[0].decode('utf-8')
    addr = d[1]
    cmdBits = cmdStr.split()
    cmdWord = cmdBits[0]
    if len(cmdBits) > 1:
        cmdArgs = cmdBits[1]
    else:
        cmdArgs = ''
        
    # we got a command 
    
    if len(cmdStr)>0:
        logger.debug(f">> {cmdStr}")

        # quit - stop the dataMan session
        
        if cmdWord.lower() == 'quit':
            logger.info("Received QUIT command from remote user")
            break

        # proc - process a file
        
        elif cmdWord.lower() == 'proc':
            filename = cmdArgs
            if Path(filename).exists():
                logger.info(f"proc: start processing image {filename}")
                t = threading.Thread(target=modsFITSProc,args=[filename,repoDir])
                t.start()                
            else:
                testFile = str(Path() / dataDir / filename) # is it in dataDir?
                if not os.path.exists(testFile):
                    logger.error(f"proc: file {testFile} not found, no processing")
                else:
                    logger.info(f"proc: start processing image {testFile}")
                    t = threading.Thread(target=modsFITSProc,args=[testFile,repoDir])
                    t.start()

        # unknown command received, log it
        
        else:
            logger.error(f"Unknown command {cmdWord} received")


# all done

s.close()

logger.info(f"Done: {modsID} dataMan server shutdown complete")

exit(0)
