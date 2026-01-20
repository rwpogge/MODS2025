#!/usr/local/conda/envs/py312/bin/python

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
 * 2025 Dec 30 - many bug fixes resulting from initial live testing at LBTO [rwp/osu]
 
 * 2026 Jan 02 - major overhaul of otmProc() after discoveries from live testing [rwp/osu]
 * 2026 Jan 14 - added fixMisc() method with other header issues from live testing [rwp/osu]
 * 2026 Jan 20 - additions to fixMisc() following ECD time and archive review [rwp/osu]
 
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
from astropy.coordinates import SkyCoord, EarthLocation, Angle
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
        with open(cfgFile,"r",encoding="utf-8") as stream:
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

    if float(datetime.datetime.now().strftime("%H")) < 12.0:  # is it before noon?
        return (datetime.datetime.now().today() - datetime.timedelta(days=1)).strftime("%Y%m%d")
    else:
        return datetime.datetime.now().today().strftime("%Y%m%d")

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

    im24_LTV1 = c0 + nc - ncbias + 1 
    im34_LTV2 = -r0

    # Recompute and correct DETSEC and CCDSEC

    hdu[1].header['detsec'] = (detsec1,'Corrected DETSEC')
    hdu[2].header['detsec'] = (detsec2,'Corrected DETSEC')
    hdu[3].header['detsec'] = (detsec3,'Corrected DETSEC')
    hdu[4].header['detsec'] = (detsec4,'Corrected DETSEC')

    hdu[1].header['ccdsec'] = (detsec1,'Corrected CCDSEC')
    hdu[2].header['ccdsec'] = (detsec2,'Corrected CCDSEC')
    hdu[3].header['ccdsec'] = (detsec3,'Corrected CCDSEC')
    hdu[4].header['ccdsec'] = (detsec4,'Corrected CCDSEC')

    # fix LTVn so the ds9 cursor returns correct physical pixel coords 
    # in IM2,IM3, and IM4 (IM1 is OK)

    hdu[2].header['LTV1'] = (im24_LTV1,'Corrected LTV1')
    hdu[4].header['LTV1'] = (im24_LTV1,'Corrected LTV1')

    hdu[3].header['LTV2'] = (im34_LTV2,'Corrected LTV2')
    hdu[4].header['LTV2'] = (im34_LTV2,'Corrected LTV2')

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


def fixMisc(hdu):
    '''
    Fix miscellanous FITS info in the Primary HDU

    Parameters
    ----------
    hdu : HDUList
        open header data unit list returned by astropy.io.fits.open()

    Returns
    -------
    None.
    
    Description
    ----------- 
    Fixes header issues that defy simple classification so "miscellaneous".
    Among the fixes:
     * convert mount alt/az from sexagesimal to decimal for LBT archive
     * compute zenith distance, zd=90-alt (legacy keyword)
     * convert LBTWLINK 1/0 boolean to string "Up"/"Down"
     * compute CCDROI header keyword from image dimensions
     * convert Archon CCDSUM=(nx ny) into CCDXBIN= and CCDYBIN= keywords
     * copy OBJECT -> OBJNAME for back compatibility (may be temporary?)
     * add OBJ/GUI keywords EQUIN, RADEC, and EPOCH not in DD
     * add GUINAME='None' as placeholder for datum not in DD
     * fix blank non-sidereal header keywords for Archive format compliance
     
    the list is getting bigger, we may split off subsets later.
     
    '''

    # DD returns mount alt/az in sexigesimal degrees, LBT data archive is orthodox about
    # using decimal degrees, but DD only returns decimal angles in radians.  So, we use 
    # astropy.coordinates Angle() to make the conversion. Why they can't handle this natively...
    
    try:         
        dAlt = Angle(hdu[0].header["TELALT"],unit=u.degree).degree
        dAz = Angle(hdu[0].header["TELAZ"],unit=u.degree).degree
        hdu[0].header["TELALT"] = (dAlt,"Telescope Altitude at start of obs [deg]")
        hdu[0].header["TELAZ"] = (dAz,"Telescope Azimuth at start of obs [deg]")
    except:
        # unlikely...
        pass

    # Zenith distance is just 90-altitude, but OK
    
    try:         
        zd = 90.0 - Angle(hdu[0].header["TELALT"],unit=u.degree).degree
        hdu[0].header["ZD"] = (zd,"Zenith distance at start of obs [deg]")
    except:
        hdu[0].header["ZD"] = (-99.99,"Zenith distance at start of obs [deg]")

    # LBTWLINK is returned by the DD as a 1/0 boolean, old MODS translated to Up/Down
    
    try:
        lbtWLink = int(hdu[0].header["LBTWLINK"])
        if (lbtWLink == 1):
            hdu[0].header["LBTWLINK"] = ("Up","LBT Weather Station Link State")
        else:
            hdu[0].header["LBTWLINK"] = ("Down","LBT Weather Station Link State")
    except:
        hdu[0].header["LBTWLINK"] = ("Unknown","LBT Weather Station Link State")

    # azcam does not record a merged image CCDROI keyword, compute it. Header 1 
    # (IM1) has the relevant dimensions plus overscan, Header 0 has ref pix
    
    try:
        nc = hdu[1].header['NAXIS1']
        nr = hdu[1].header['NAXIS2']
        overx = hdu[1].header['OVRSCAN1']
        xref = hdu[0].header['REF-PIX1']
        yref = hdu[0].header['REF-PIX2']
        
        sc = int(xref - nc + overx +1)
        ec = int(xref + nc - overx)
        sr = int(yref - nr + 1)
        er = int(yref + nr)
        hdu[0].header["CCDROI"] = (f"[{sc}:{ec},{sr}:{er}]","CCD readout ROI coords")
    except:
        # unlikely...
        pass

    # azcam stores binning factors in the CCDSUM keyword as "(nx ny)", convert to old-style
    # MODS CCDXBIN and CCDYBIN. Get from header 1 (IM1 extension), all are the same.
    
    try:
        ccdsum = hdu[1].header["CCDSUM"]
        bits = ccdsum.split(" ")
        hdu[0].header["CCDXBIN"] = (int(bits[0]),"CCD X-axis Binning Factor")
        hdu[0].header["CCDYBIN"] = (int(bits[1]),"CCD Y-axis Binning Factor")
    except:
        pass
    
    # Missing target and guide star keywords from the SetStars() preset info for
    # the Archive data model.  We make placeholders for data not (yet?) exposed
    # in the IIF data dictionary
    
    try:
        hdu[0].header["OBJNAME"] = (hdu[0].header['OBJECT'],"Target Name")
    except:
        hdu[0].header["OBJNAME"] = ("None","Target name")
        
    try:
        hdu[0].header["OBJRADEC"] = ("FK5","Target coordinate system")
        hdu[0].header["OBJEQUIN"] = ("J2000","Target coordinate system equinox")
        hdu[0].header["OBJEPOCH"] = (2000.00,"Target epoch")
    except:
        pass

    try:
        hdu[0].header["GUINAME"]  = ("None","Guide star name")
        hdu[0].header["GUIRADEC"] = ("FK5","Guide star coordinate system")
        hdu[0].header["GUIEQUIN"] = ("J2000","Guide star coordinate system equinox")
        hdu[0].header["GUIPMRA"]  = (0.0,"Guide star RA proper motion [mas/yr]")
        hdu[0].header["GUIPMDEC"] = (0.0,"Guide star DEC proper motion [mas/yr]")
        hdu[0].header["GUIEPOCH"] = (2000.00,"Guide star epoch")
    except:
        pass    

    # Check the non-sidereal keywords as some can be blank, make "None" because
    # the archive will raise warnings if blank keywords
    
    try:
        if (len(hdu[0].header["NSTYPE"])==0):
            hdu[0].header["NSTYPE"] = ("None","Non-sidereal target type")
        if (len(hdu[0].header["NSEPHFIL"])==0):
            hdu[0].header["NSEPHFIL"] = ("None","Non-sidereal ephemeris file name")    
    except:
        hdu[0].header["NSTYPE"] = ("None","Non-sidereal target type")
        hdu[0].header["NSEPHFIL"] = ("None","Non-sidereal ephemeris file name")
        
    # Other stuff goes here
    
    return


def otmProc(hdu,biasColSkip=2,biasRowSkip=2):
    '''
    subtract overscan bias, trim, and merge quadrants into a single image

    Parameters
    ----------
    hdu : HDUList
        open header data unit list returned by astropy.io.fits.open()
    biasColSkip : int, optional
        number of starting bias columns to skip. The default is 2 columns
    biasRowSkip : int, optional
        bias rows to skip at top and bottom. The default is 2 rows

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
    
    Readout Mapping
    ---------------
    The Archon controllers readout the CCDs using 4 amplifiers into
    a 4-channel ADC. The data from each channel is stored in the 
    multi-extension FITS files as extension IM1 through IM4, where
    the number (1..4) refers to the ADC channel.  The mapping from
    ADC channels (IM1..4) to MODS CCD quadrants (Q1..4) including
    transforms that map pixels from readout order to physical order
    on the CCD proper are:
        Q1 = IM3 flipped along columns
        Q2 = IM4 flipped along rows and columns
        Q3 = IM1 flipped along columns
        Q4 = IM2 flipped along rows and columns
    
    Put in a MODSQUAD keyword in each raw IMx header identifying
    which MODS quadrant that IMx data maps into.
    
    Notes
    -----
    The method below is adapted from one written by Olga Kuhn (LBTO)
    
    '''
    
    # Size of a single quadrant 

    nx = hdu[1].header["naxis1"]
    ny = hdu[1].header["naxis2"]

    # columns (x) trimmed of overscan
    
    xtrim = nx - hdu[1].header["ovrscan1"]

    # images read by Archon ADC channel 1..4 are IM1..4

    im1 = hdu[1].data
    im2 = hdu[2].data
    im3 = hdu[3].data
    im4 = hdu[4].data

    # Compute the median of the overscan sector because it is essentially
    # constant with rows even on bright flats.  Ignore skipped columns and
    # rows (biasColSkip and biasRowSkip).  Also compute standard deviation.

    im1mb = np.median(im1[biasRowSkip:-biasRowSkip,xtrim+biasColSkip:nx])
    im1sb = np.std(im1[biasRowSkip:-biasRowSkip,xtrim+biasColSkip:nx])
    
    im2mb = np.median(im2[biasRowSkip:-biasRowSkip,xtrim+biasColSkip:nx])
    im2sb = np.std(im2[biasRowSkip:-biasRowSkip,xtrim+biasColSkip:nx])
    
    im3mb = np.median(im3[biasRowSkip:-biasRowSkip,xtrim+biasColSkip:nx])
    im3sb = np.std(im3[biasRowSkip:-biasRowSkip,xtrim+biasColSkip:nx])
    
    im4mb = np.median(im4[biasRowSkip:-biasRowSkip,xtrim+biasColSkip:nx])
    im4sb = np.std(im4[biasRowSkip:-biasRowSkip,xtrim+biasColSkip:nx])   

    # Subtract the scalar median bias from the data sections and trim here
    
    im1t = im1[:,:xtrim] - im1mb
    im2t = im2[:,:xtrim] - im2mb
    im3t = im3[:,:xtrim] - im3mb
    im4t = im4[:,:xtrim] - im4mb

    # Map ADC channels IM1..4 to original MODS CCD quadrants Q1..4:
    #   Q1 = IM3 flipped along columns
    #   Q2 = IM4 flipped along rows and columns
    #   Q3 = IM1 flipped along columns
    #   Q4 = IM2 flipped along rows and columns

    q1 = np.flipud(im3t)
    q2 = np.flipud(np.fliplr(im4t))
    q3 = np.flipud(im1t)
    q4 = np.flipud(np.fliplr(im2t))

    # Quadrant mapping (MODSQUAD) mapping info
    
    hdu[1].header['MODSQUAD'] = ("Q3 flip cols","Mapping into MODS CCD")
    hdu[2].header['MODSQUAD'] = ("Q4 flip rows and cols","Mapping into MODS CCD")
    hdu[3].header['MODSQUAD'] = ("Q1 flip cols","Mapping into MODS CCD")
    hdu[4].header['MODSQUAD'] = ("Q2 flip rows and cols","Mapping into MODS CCD")
    
    # Quadrant bias subtracted for the FITS headers

    quadBias = [im3mb,im4mb,im1mb,im2mb]
    quadStd = [im3sb,im4sb,im1sb,im2sb]
    
    # Merge the quadrants into a single-image mosaic retaining the 
    # original orientation of the pre-Archon MODS CCD controllers

    mosaic = np.zeros((ny*2,xtrim*2),dtype=np.float32)
    mosaic[:ny,:xtrim] = q1
    mosaic[:ny,xtrim:] = q2
    mosaic[ny:,:xtrim] = q3
    mosaic[ny:,xtrim:] = q4

    # all done, return the mosaic image and per-quadrant bias medians/sigmas

    return mosaic, quadBias, quadStd

#------------------------------------------------------------------------------
# 
# FITS processor function
#

def modsFITSProc(fitsFile):
    '''
    Process a MODS FITS image

    Parameters
    ----------
    fitsFile : string
        name of the raw MODs FITS file to process
    
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
    2025 Dec 25 - invoking processing ops as separate methods, simplifies this method
    2025 Dec 26 - now building NFS-mounted paths for one dataMan per MODS
    
    '''

    # Raw data files are in directories like /data/mods1b/ that are NFS-mounted
    # on the dataMan server machine. Get what we need from fitsFile passed
    # to this method.
    
    baseName = os.path.basename(fitsFile)
    modsChan = baseName.split(".")[0]
    obsDate = baseName.split(".")[1]
    
    rawFile = str(Path() / dataDir / modsChan / baseName)
    
    # If rawFile does not exist, nothing to do.

    if not Path(rawFile).exists():
        logger.error(f"proc: {rawFile} not found")
        return
    
    # We have a file to process    
    
    logger.info(f"Processing {baseName}...")

    try:
        hdu = fits.open(rawFile)
    except Exception as exp:
        logger.error(f"Cannot open {rawFile} - {exp}")
        return
                     
    # procPath is the directory where the processed data will
    # be written.  It will have a name like /home/data/20251226/,
    # the date derived from obsDate.
    
    procPath = Path() / procDir / obsDate    
    
    # does procPath exist?  If not, create it and set permissions
    
    try:
        procPath.mkdir(parents=True, exist_ok=False)
        procPath.chmod(0o777)
    except FileExistsError:
        pass
    else:
        logger.info(f"Created processed image folder {str(procPath)}")
    
    # Generate a uniqName coded as
    #    <instID>.CCYYMMDD.hhmmss.fits
    # that we use in the event writing out the processed image would
    # overwrite an existing image.  The FILENAME preserves the intended
    # filename, we add UNIQNAME to the primary header, and use that
    # name to avoid overwrite.
        
    try:
        instID = hdu[0].header["INSTRUME"].lower()
    except:
        logger.warning(f"No INSTRUME keyword found in {fitsFile} - corrupt header?")
        instID = "modsNx"

    dtNow = datetime.datetime.now().strftime("%Y%m%d.%H%M%S")
    uniqName = f"{instID}.{dtNow}.fits"
    hdu[0].header["UNIQNAME"] = (uniqName,"Unique filename")

    #--------------------------------------------------------------------------
    # Start processing
    
    # fix DETSEC and related header records?
        
    if procParam["fixDETSEC"]:
        fixDataSec(hdu)

    # fix Date/Time header records?
            
    if procParam["fixDateTime"]:
        fixDateTime(hdu)

    # fix CCD and Archon backplane temperature header records?
        
    if procParam["fixTemps"]:
        fixArchonTemps(hdu)

    if procParam["fixMisc"]:
        fixMisc(hdu)

    # Create an overscan bias subtracted, trimmmed, and merged image
    # and append it to the file?
            
    if procParam["makeOTM"]:
        otmImg,quadBias,quadStd = otmProc(hdu)
        otmHDU = fits.ImageHDU(data=otmImg,header=hdu[0].header,name="Merged")

        for quad in [1,2,3,4]:
            otmHDU.header[f"Q{quad}Bias"] = (quadBias[quad-1],f"Q{quad} median overscan bias [DN]")
            otmHDU.header[f"Q{quad}Std"] = (quadStd[quad-1],f"Q{quad} overscan bias stdev [DN]")

        hdu.append(otmHDU)

    #--------------------------------------------------------------------------
    # Done: write out the processed image and stage to the LBTO archive
             
    # We need two paths to write:
    #   processed FITS saved as procFile = procPath/baseName
    #   copy of processed FITS archived to repoFile = repoDir/baseName
    # if the target filename exists, write/copy using uniqName
    
    numErrors = 0
    
    procFile = Path() / procPath / baseName
    if procFile.exists():
        logger.warning(f"{str(procFile)} exists, using unique name {uniqName}")
        procFile = Path() / procPath / uniqName

    repoFile = Path() / repoDir / baseName
    if repoFile.exists():
        logger.warning(f"{str(repoFile)} exists, using unique name {uniqName}")
        repoFile = Path() / repoDir / uniqName

    # Write out the processed FITS file to the procPath and close it

    try:
        hdu.writeto(str(procFile))
        hdu.close()
        logger.info(f"Processed image {baseName} written to {str(procPath)}")
    except Exception as err:
        logger.error(f"Cannot write {baseName} to {str(procPath)} - {err}")
        numErrors += 1
        hdu.close()
            
    # Copy the processed FITS file to the LBTO new data repository which 
    # stages it for ingestion in the LBTO data archive

    try:
        shutil.copyfile(str(procFile),str(repoFile))
        logger.info(f"Processed image {baseName} copied to {repoDir}")
        
        # update the modsChan.new file on repoDir

        newFile = str(Path() / repoDir / f"{modsChan.upper()}.new")
        with open(newFile, "w") as nf:
            try:
                nf.write(f"{baseName}\n")
                logger.info(f"Updated {newFile}")
            except Exception as err:
                logger.error(f"Cannot update {newFile} - {err}")
                numErrors += 1
    except Exception as err:
        logger.error(f"Cannot copy {baseName} to {repoDir} - {err}")
        numErrors += 1
        
    # we're done
        
    if numErrors > 0:
        logger.warning(f"{baseName} processing had {numErrors} error(s)")
    else:
        logger.info(f"Done: {baseName} processed and archived")
        
    return

        
#---------------------------------------------------------------------------
#
# Basic info and defaults
#

hostname = socket.gethostname()
modsID = hostname[:5]
    
dmHost = "localhost" # actual from config needs to be full ipAddr
dmPort = 10301

dataDir = "/data"
procDir = "/home/data"
repoDir = "/lbt/data/new"
logDir = "/home/Logs/dataMan"

configPath = "/home/dts/Config"
defaultCfg = "dataman.ini"

#---------------------------------------------------------------------------
#
# start of main program
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
    
# Start the runtime logger: {modsID}dm.ccyymm.txt

logFile = str(Path(logDir) / f"{modsID}dm.{obsDate()}.txt")

logging.basicConfig(filename=logFile,
                    format="%(asctime)s | %(levelname)s | %(message)s",
                    filemode="a",
                    level=logLevel)

logger = logging.getLogger("dataMan")

logger.info(f"Started {modsID} dataMan server")

# Initialze the datagram (udp) socket 

try:
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
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

#
# Listen for commands from remote socekt clients
#
# Supported commands:
#   proc rawFile - processed the named raw FITS file
#   quit - exit the server loop and stop the session
#

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
        logger.debug(f"received command: {cmdStr}")

        # quit - stop the dataMan session
        
        if cmdWord.lower() == "quit":
            logger.info("Received QUIT command from remote user")
            break

        # proc - process a file
        
        elif cmdWord.lower() == "proc":
            filename = cmdArgs
            if len(filename) > 0:
                logger.debug(f"started processing image {filename}")
                t = threading.Thread(target=modsFITSProc,args=[filename])
                t.start()                
        # unknown command received, log it
        
        else:
            logger.error(f"Unknown command {cmdWord} received")


# all done

s.close()

logger.info(f"Done: {modsID} dataMan server shutdown complete")

exit(0)
