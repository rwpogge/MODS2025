'''
dataMan - MODS data post-processor agent

UDP socket server.

'''

import os
import sys
import socket
import threading
import time

# pathlib for path handling

from pathlib import Path

# logging for runtime logging

import logging

# We use yaml for runtime config

import yaml
from yaml import load
try:
    from yaml import CLoader as Loader
except ImportError:
    from yaml import Loader

# loadConfig() - load the runtime configuration file

def loadConfig(cfgFile):
    """loadConfig() - load a YAML config file

    Arguments:
       cfgFile - YAML runtime configuration file (local or full path)

    Returns: dictionary with the YAML contents as dictionaries
    """

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
    """
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
    on 2024 Dec 17 and ending at sunrise on 2024 Dec 18 is 20241217

    We use this for filenames for data and logs.
    """

    if float(datetime.datetime.now().strftime("%H")) < 12.0:  # before noon
        return (datetime.date.today() - datetime.timedelta(days=1)).strftime("%Y%m%d")
    else:
        return datetime.date.today().strftime("%Y%m%d")

#---------------------------------------------------------------------------
#
# modsFITSProc for threading FITS file process
#

def modsFITSProc(fitsFile,repoDir):
    """
    Process a MODS FITS image

    Parameters
    ----------
    fitsFile : string
        name of the raw MODs FITS file to process

    repoDir : string
        full path of the LBTO new data repository directory
    
    Setup to be called by threading.Thread()

    Description
    -----------
    Processes a raw MODS FITS file and pushes the  processed file into the repository.
    
    """

    if Path(fitsFile).exists():
        logger.info(f"proc: processing {fitsFile}...")
        time.sleep(10)
        logger.info(f"proc done: {fitsFile} sent to {repoDir}")
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

repoDir = "/lbt/data/new"
logDir = "/home/Logs/dataMan"

configPath = "/home/dts/Config"
defaultCfg = "dataman.ini"

#
# -- sloppy main --
#

if len(sys.argv)-1 == 0:
    cfgFile = str(Path() / configPath / defaultCfg)
elif len(sys.argv)-1 == 1:
    cfgFile = sys.argv[1]
    if not os.path.exists(cfgFile):
        # try adding the default configuration path
        cfgFile = str(Path() / configDir / sys.argv[1])
        if not os.path.exists(cfgFile):
            print(f"ERROR: could not find {cfgFile} in pwd or {str(configDir)}")
            sys.exit(1)
else:
    print(f"Usage: dataMan [cfgFile]")
    sys.exit(0)

# Read in the config file

try:
    cfg = loadConfig(cfgFile)
except Exception as exp:
    print(f"ERROR: (loadConfig): {exp}")
    sys.exit(1)

# Retrieve info we need

dmHost = cfg["server"]["ipAddr"]
dmPort = cfg["server"]["ipPort"]

repoDir = str(Path(cfg["paths"]["repoDir"]))
logDir = Path(cfg["paths"]["logDir"])
dataDir = Path(cfg["paths"]["dataDir"])
              
# start logging

logFile = str(Path(logDir) / f"{modsID}.{obsDate()}.txt")

logging.basicConfig(filename=logFile,
                    format="%(asctime)s %(name)s %(levelname)s: %(message)s",
                    filemode="a",
                    level=logging.INFO)

logger = logging.getLogger("dataMan")

logger.info(f"Started dataMan for {modsID}")

# Datagram (udp) socket

logger.info(f"Starting the {modsID} dataMan server")

try:
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    logger.info("UDP socket created")
except Exception as err:
    logger.error(f"Failed to create UDP socket - {err}")
    sys.exit(-1)

# Bind socket to local host and port

try:
    s.bind((dmHost,dmPort))
except Exception as err:
    logger.error(f"UDP socket binding failed - {err}")
    sys.exit(-1)
        
logger.info(f"{modsID} dataMan UDP/IP server started on {dmHost}:{dmPort}")

# Talk to the client until we are killed off

while 1:
    # received data from client
    
    d = s.recvfrom(1024)
    
    cmdStr = d[0].decode('utf-8')
    addr = d[1]
    
    cmdBits = cmdStr.split()
    cmdWord = cmdBits[0]
    if len(cmdBits) > 1:
        cmdArgs = cmdBits[1]
    else:
        cmdArgs = ''
        
    if len(cmdStr)>0:
        logger.debug(f">> {cmdStr}")

        if cmdWord.lower() == 'quit':
            logger.info(f"Received quit command from remote user")
            break

        elif cmdWord.lower() == 'proc':
            filename = cmdArgs
            logger.info(f"start processing image {filename}")
            t = threading.Thread(target=modsFITSProc,args=[filename,repoDir])
            t.start()
            
        else:
            logger.error(f"Unknown command {cmdWord} received")


# all done

logger.info(f"Done: {modsID} dataMan server shutdown")

s.close()

