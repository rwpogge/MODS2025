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
 * 2025 Aug 29 - start of development, test server/client stubs w/threading [rwp/osu]
 * 2025 Oct 16 - added logging, config file, paths, and FITS handling [rwp/osu]
 * 2025 Oct 31 - boo! Started header processing coding [rwp/osu]
 * 2025 Nov 06 - FITS header time/date fixes [rwp/osu]
 * 2025 Dec 25 - Restructuring header pre-proc ops into separate functions, simplifying
                 modsFITSProc() method. Enable with config file directives [rwp/osu]
                 
'''

import os
import sys
import socket
import threading
import datetime
import pytz

# numpy for arrays

import numpy as np

# astropy.io for FITS file handling

from astropy.io import fits

# astropy.table for FITS binary table handling

from astropy.table import Table

# astropy units, coordinates, and time

from astropy import units as u
from astropy.coordinates import SkyCoord, EarthLocation
from astropy.time import Time

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

    myTZ = pytz.timezone("US/Arizona")
    if float(datetime.datetime.now(myTZ).strftime("%H")) < 12.0:  # is it before noon?
        return (datetime.datetime.now(myTZ).today() - datetime.timedelta(days=1)).strftime("%Y%m%d")
    else:
        return datetime.datetime.now(myTZ).today().strftime("%Y%m%d")

    if float(datetime.datetime.now().strftime("%H")) < 12.0:  # before noon
        return (datetime.date.today() - datetime.timedelta(days=1)).strftime("%Y%m%d")
    else:
        return datetime.date.today().strftime("%Y%m%d")

#------------------------------------------------------------------------------
#
# FITS image pre-processing functions
#

def fixDataSec(hdu):
    '''
    fix DATASEC header info in quadrant HDUs

    Parameters
    ----------
    hdu : HDUList
        open header data unit list returned by astropy.io.fits.open()

    Returns
    -------
    None.

    Description
    -----------
    The coordinates in the `DETSEC` and `CCDSEC` keywords for each quadrant
    and the `LVTn` coordinate transform coefficients are incorrect.  If you 
    read the raw MODS FITS image using ds9 and open as "Mosaic IRAF", the
    quadrants will not be stitched together correctly on the display (huge 
    gaps) and the cursor will read the wrong coordinates back.  This fixes 
    the headers in post-processing.  Eventually we should dig deep
    into the `azcam` code and fix it there, but not now.
    
    Transforms
    ----------
    IRAF transform between CCD pixel (Cx,Cy) and Image (Ic,Il) coords
    source: https://www.cfht.hawaii.edu/~veillet/imagedef.html

        Ic = LTM1_1 * Cx + LTM1_2 * Cy + LTV1
        Il = LTM2_1 * Cx + LTM2_2 * Cy + LTV2

        Cx = ( LTM2_2 * (Ic - LTV1) - LTM1_2 * (Il - LTV2)) /
    	 (LTM1_1 * LTM2_2 - LTM1_2 * LTM2_1)
        Cy = (-LTM2_1 * (Ic - LTV1) + LTM1_1 * (Il - LTV2)) /
    	 (LTM1_1 * LTM2_2 - LTM1_2 * LTM2_1)

    '''
    
    # get header parameters we need

    c0=int(hdu[0].header['ref-pix1']) # reference pixel 
    r0=int(hdu[0].header['ref-pix2'])

    nc=hdu[1].header['naxis1'] # total size of a quadrant (all same)
    nr=hdu[1].header['naxis2']

    ncbias = hdu[1].header['ovrscan1'] # bias columns

    # compute corrected DETSEC/CCDSEC and LVTn parameters

    detsec1=f"[{c0-nc+ncbias+1}:{c0},{r0-nr+1}:{r0}]"
    detsec2=f"[{c0+nc+ncbias}:{c0+1},{r0-nr+1}:{r0}]" # flip x
    detsec3=f"[{c0-nc+ncbias+1}:{c0},{r0+1}:{r0+nr}]"
    detsec4=f"[{c0+nc+ncbias}:{c0+1},{r0+1}:{r0+nr}]" # flip x

    q24_LTV1 = c0 + nc - ncbias + 1 
    q34_LTV2 = -r0

    # Rrecompute and correct DETSEC and CCDSEC

    hdu[1].header['detsec'] = (detsec1,'Corrected DETSEC')
    hdu[2].header['detsec'] = (detsec2,'Corrected DETSEC')
    hdu[3].header['detsec'] = (detsec3,'Corrected DETSEC')
    hdu[4].header['detsec'] = (detsec4,'Corrected DETSEC')

    hdu[1].header['ccdsec'] = (detsec1,'Corrected CCDSEC')
    hdu[2].header['ccdsec'] = (detsec2,'Corrected CCDSEC')
    hdu[3].header['ccdsec'] = (detsec3,'Corrected CCDSEC')
    hdu[4].header['ccdsec'] = (detsec4,'Corrected CCDSEC')

    # fix LTVn so the ds9 cursor returns correct physical pixel coords 
    # in Q2,Q3, and Q4 (Q1 is OK)

    hdu[2].header['LTV1'] = (q24_LTV1,'Corrected LTV1')
    hdu[4].header['LTV1'] = (q24_LTV1,'Corrected LTV1')

    hdu[3].header['LTV2'] = (q34_LTV2,'Corrected LTV2')
    hdu[4].header['LTV2'] = (q34_LTV2,'Corrected LTV2')

    # all done

    return


def fixDateTime(hdu):
    '''
    fix date/time info in the Primary HDU

    Parameters
    ----------
    hdu : HDUList
        open header data unit list returned by astropy.io.fits.open()

    Returns
    -------
    None.
    
    Description
    -----------
    Converts the `azcam` style `DATE-OBS` keyword in to LBT Archive compliant 
    ISO8601 format (`azcam` is old school).

    Also compute MJD and heliocentric and barycentric JD and add it to the 
    headers as `HJD-OBS` and `BJD-OBS` at start of exposure useful for MODS 
    observations involving precise timing.

    '''

    # fix DATE-OBS to make it FITS- and LBT Archive format compliant ISO8601

    try:        
        dateObs = hdu[0].header["DATE-OBS"]
        utcObs = hdu[0].header["UTC-OBS"]
        newDateObs = f"{dateObs}T{utcObs}"
        hdu[0].header["DATE-OBS"] = (newDateObs,"UTC Date at start of obs")
            
        # compute modified Julian Date
        
        obsTime = Time(newDateObs,format="isot",scale="utc")
        hdu[0].header["MJD-OBS"] = (obsTime.mjd,"Modified JD at start of obs [UTC]")
            
    except:
        # unlikely...
        pass
        
    # Compute heliocentric and barycentric JD.  HJD is in UTC time scale, but BJD is 
    # in Barycentric Dynamical Time (TDB).  We need site and target coordinate info.
        
    try:
        telRA = hdu[0].header["TELRA"]
        telDec = hdu[0].header["TELDEC"]
   
        obsSite = EarthLocation.of_site("LBT")
        obsTime = Time(newDateObs,format="isot",scale="utc",location=obsSite)
        obsCoord = SkyCoord(telRA,telDec,unit=(u.hourangle,u.deg),frame="icrs")
            
        lttBary = obsTime.light_travel_time(obsCoord,"barycentric")
        lttHelio = obsTime.light_travel_time(obsCoord,"heliocentric")
        obsBary = obsTime.tdb + lttBary
        obsHelio = obsTime.utc + lttHelio            

        hdu[0].header["HJD-OBS"] = (obsHelio.mjd,"Heliocentric MJD at start of obs [UTC]")
        hdu[0].header["BJD-OBS"] = (obsBary.mjd,"Barycentric MJD at start of obs [TDB]")
    except:
        # might be TCS offline so no TELRA/TELDEC, or header corruption
        pass
      
    return


def fixArchonTemps(hdu):
    '''
    fix CCD and Archon temperatures in the Primary HDU

    Parameters
    ----------
    hdu : HDUList
        open header data unit list returned by astropy.io.fits.open()

    Returns
    -------
    None.
    
    Description
    -----------
    Reads the CONPARS extension (5) and extracts the CCD, Base, and Archon
    backplane temperatures at start of exposure and puts these into primary 
    HDU.  This corrects wrong `CCDTEMP` and `BASETEMP` that comes from `azcam`,
    and sets by instrument-specific wiring (MODS1 different than MODS2).

    '''
    
    # Get the Archon controller status snapshot from HDU 5 (bintable)

    t = Table(hdu[5].data)
    archonDict = dict(zip(t['Keyword'].tolist(),t['Value'].tolist()))

    # Which sensor is which depends on the MODS channel

    modsID = hdu[0].header['INSTRUME']
    if modsID in ['MODS1B','MODS1R']:
        ccdSensor = 'MOD10/TEMPB'
        baseSensor = 'MOD10/TEMPA'
    else:
        ccdSensor = 'MOD10/TEMPA'
        baseSensor = 'MOD10/TEMPB'
        
    # fix CCDTEMP and BASETEMP

    try:
        ccdTemp = float(archonDict[ccdSensor])
    except:
        ccdTemp = -999.9 # no-read value
    hdu[0].header['CCDTEMP'] = (ccdTemp,'CCD Detector Temperature [deg C]')

    try:
        baseTemp = float(archonDict[baseSensor])
    except:
        baseTemp = -999.9 # no-read value
    hdu[0].header['BASETEMP'] = (baseTemp,'CCD Mount Base Temperature [deg C]')

    # Extract and store the Archon backplane temperature

    try:
        archonTemp = float(archonDict['BACKPLANE_TEMP'])
    except:
        archonTemp = -999.9 # no-read value
    hdu[0].header['ARCHTEMP'] = (archonTemp,'Archon Controller Backplane Temperature [deg C]')

    return


def otmProc(hdu,skipBiasCols=2,biasRowMargin=2,sigClip=3):
    '''
    subtract overscan bias, trim, and merge quadrants into a single image

    Parameters
    ----------
    hdu : HDUList
        open header data unit list returned by astropy.io.fits.open()
    skipBiasCols : int, optional
        number of starting bias columns to skip. The default is 2 columns
    biasRowMargin : int, optional
        bias rows to skip at start and end. The default is 2 rows
    sigClip : float, optional
        Compute bias with sigma clipping. The default is 3.  Set to 0 for no 
        sigma clipping.  

    Returns
    -------
    otmData : numpy 32-bit floating array
        merged image array
    quadBias : float array
        median overscan bias subtracted from each quadrant.
    quadStd : float array
        standard deviation of overscan bias in each quadrant.

    Description
    -----------
    Extracts each raw quadrant images from FITS extension 1..4, 
    computes the median overscan bias, subtracts it from the quadrant data
    pixels, then trims off the overscan columns.  If quadrants 2 and 4
    it flips the pixels along rows because of the different readout
    direction, then merges all 4 quadrants into a single image
    and returns it as a numpy array (32-bit float) plus lists of
    the median bias levels and their standard deviations.
    
    If sigClip > 0, it performs 1 sigma clipping iteration to compute
    the bias levels so that outlier pixels don't cause problems.
    
    The numpy array is ready to be saved as a standalone FITS file,
    or appended as an extension to an existing MEF FITS file.
    
    '''
    
    # Q2 and Q4 are flipped along rows

    flipRows = [False,True,False,True]

    # Size of data section w/o column bias

    ncQuad = hdu[1].header['naxis1'] - hdu[1].header['ovrscan1']
    nrQuad = hdu[1].header['naxis2']

    ncImg = 2*ncQuad
    nrImg = 2*nrQuad

    # create an empty array to contian the merged image
    
    otmData = np.empty((nrImg,ncImg),dtype=np.float32)

    # starting numpy pixels of each quadrant

    startPix = [(0,0),(0,ncQuad),(nrQuad,0),(nrQuad,ncQuad)]

    # process by quadrant

    quadBias = []
    quadStd = []
    for quad in [1,2,3,4]:
        quadData = hdu[quad].data.astype(np.float32) # convert to 32-bit float for processing
        nc = hdu[quad].header['naxis1'] # number of columns
        nr = hdu[quad].header['naxis2'] # number of rows
        ncbias = hdu[quad].header['ovrscan1'] # number of overscan columns

        # extract bias overscan columns
    
        biasCols = quadData[biasRowMargin:-biasRowMargin,nc-ncbias+skipBiasCols:]
        bias1d = np.median(biasCols,axis=1)
        medBias = np.median(bias1d)
        stdBias = np.std(bias1d)
        if (sigClip > 0):
            loCut = medBias - sigClip*stdBias
            hiCut = medBias + sigClip*stdBias
            iClip = np.where((bias1d >= loCut) & (bias1d <= hiCut))
            medBias= np.median(bias1d[iClip])
            stdBias = np.std(bias1d[iClip])                                    
        quadBias.append(medBias)
        quadStd.append(stdBias)

        # subtract the median overscan bias from the quadrant

        imgData = quadData[0:,0:nc-ncbias] - medBias

        # is this quadrant flipped? Unflip it

        if flipRows[quad-1]:
            imgData = np.flip(imgData,axis=1)
        
        # insert the debiased quadrant into the full merged image array
    
        sr = startPix[quad-1][0]
        er = sr + nr
        sc = startPix[quad-1][1]
        ec = sc + nc - ncbias
    
        otmData[sr:er,sc:ec] = imgData

    # all done, return the image and per-quadrant bias values

    return otmData, quadBias, quadStd

#------------------------------------------------------------------------------
# 
# FITS processor function
#

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
     
    One exception to time-critical updates is getting the observation date
    and time info in the correct standard ISO8601 format required by FITS and
    the LBT Archive. Absent this, the LBT Archive might not ingest the file.
    
    Once processing steps are completed, the last step is to copy the processed
    FITS file to the observatory "newdata" repository which is where it may be
    accessed by observers and SciOps personnel on site.  From newdata, the images
    are transferred to the repository and queued for ingestion into the LBTO Data
    Archive.  This latter places a requirement on fixing any FITS header issues
    that might inhibit ingestion into the data archive.

    Author
    ------
    R. Pogge, OSU Astronomy Dept. (pogge.1@osu.edu)

    Modification
    ------------
    2025 Dec 25 - invoking operations as separate methods, simplifies this method
    
    '''

    if Path(fitsFile).exists():
        logger.info(f"proc: processing {fitsFile}...")

        try:
            hdu = fits.open(fitsFile)
        except Exception as exp:
            logger.error(f"could not open {fitsFile} - {exp}")
            return

        # fix header DETSEC and related records?
        
        if procParam["fixDETSEC"]:
            fixDataSec(hdu)

        # fix header Date/Time records?
            
        if procParam["fixDateTime"]:
            fixDateTime(hdu)

        # fix header CCD and Archon backplane temperature records?
        
        if procParam["fixTemps"]:
            fixArchonTemps(hdu)

        # create an overscan bias subtracted, trimmmed, and merged image
        # and append it to the file?
            
        if procParam["makeOTM"]:
            otmImg,quadBias,quadStd = otmProc(hdu)
            otmHDU = fits.ImageHDU(data=otmImg,header=hdu[0].header,name="Merged")

            for quad in [1,2,3,4]:
                otmHDU.header[f"Q{quad}Bias"] = (quadBias[quad-1],f"Q{quad} median overscan bias [DN]")
                otmHDU.header[f"Q{quad}Std"] = (quadStd[quad-1],f"Q{quad} overscan bias stdev [DN]")

            hdu.append(otmHDU)
             
        # Generate a uniqName coded as
        #    <instID>.CCYYMMDD.hhmmss.fits
        # that we use in the event writing out the processed image would
        # overwrite an existing image.  The FILENAME preserves the intended
        # filename, we add UNIQNAME to the primary header, and use that
        # name to avoid overwrite.
        
        try:
            instID = hdu[0].header["INSTRUME"].lower()
        except:
            logger.warning(f"No INSTRUME keyword found in {fitsFile} - corrupt header, uniqname not created")
            instID = "modsNx"

        dtNow = datetime.datetime.now().strftime("%Y%m%d.%H%M%S")
        uniqName = f"{instID}.{dtNow}.fits"
        hdu[0].header["UNIQNAME"] = (uniqName,"Unique filename")
        
        # write new data to procDir, then copy to repoDir

        baseFile = os.path.basename(fitsFile)
        
        procPath = Path() / procDir / baseFile
        if procPath.exists():
            logger.error(f"{str(procPath)} exists, using uniqname={uniqName}")
            procPath = Path() / procDir / uniqName
        
        repoPath = Path() / repoDir / uniqName
        if repoPath.exists():
            logger.error(f"{str(repoPath)} exists, using uniqname={uniqName}")
            repoPath = Path() / repoDir / uniqName

        # Write out the FITS file with the modified header to the procPath and close it

        try:
            hdu.writeto(str(procPath))
            hdu.close()
            procPath.chmod(0o666) # change to RW for all before copying
            logger.info(f"processed file {fitsFile} copied to {procDir}")
        except Exception as err:
            logger.error(f"could not write {fitsFile} to {procDir} - {err}")
            hdu.close()
            return
            
        # Copy the processed file to the LBTO new data repository

        try:
            shutil.copyfile(str(procPath),str(repoPath))
            logger.info(f"processed {fitsFile} copied to {repoDir}")
        except Exception as err:
            logger.error(f"could not copy {fitsFile} to {repoDir} - {err}")
            return
        
        # we"re done
        
        logger.info(f"proc done: {fitsFile} copied to {repoDir}")
        
    else:
        logger.error(f"proc: no such file {fitsFile}, nothing to process")

    return
        
#---------------------------------------------------------------------------
#
# Basic info and defaults
#

hostname = socket.gethostname()
modsID = hostname[:6]
    
dmHost = "localhost"
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

# image processing directives

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
    
    cmdStr = d[0].decode("utf-8")
    addr = d[1]
    cmdBits = cmdStr.split()
    cmdWord = cmdBits[0]
    if len(cmdBits) > 1:
        cmdArgs = cmdBits[1]
    else:
        cmdArgs = ""
        
    # we got a command 
    
    if len(cmdStr)>0:
        logger.debug(f">> {cmdStr}")

        # quit - stop the dataMan session
        
        if cmdWord.lower() == "quit":
            logger.info("Received QUIT command from remote user")
            break

        # proc - process a file
        
        elif cmdWord.lower() == "proc":
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
