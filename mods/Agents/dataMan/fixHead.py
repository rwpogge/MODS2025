from astropy.io import fits
from astropy.table import Table
from astropy import units as u
from astropy.coordinates import SkyCoord, EarthLocation
from astropy.time import Time

import numpy as np

import sys

# functions

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
    mergedImg : numpy 32-bit floating array
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
    
    mergedImg = np.empty((nrImg,ncImg),dtype=np.float32)

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
    
        mergedImg[sr:er,sc:ec] = imgData

    # all done return the image

    return mergedImg, quadBias, quadStd

#---------------------------------------------------
#
# sloppy main...
#

if len(sys.argv) == 3:
   origFITS = sys.argv[1]
   newFITS = sys.argv[2]
else:
   print("Usage: fixHead.py origFITS newFITS")
   sys.exit(1)

try:
   hdu = fits.open(origFITS)
except Exception as exp:
   print(f"ERROR: could not open {origFITS} - {exp}")
   sys.exit(1)

# fix the DETSEC, CCDSEC, and LVTn transforms in the
# individual quadrant headers (extension 1..4)

fixDataSec(hdu)

# Fix CCD and archon temperatures in the Primary HDU

fixArchonTemps(hdu)

# fix DATE-OBS to make it FITS- and LBT Archive format compliant ISO8601

fixDateTime(hdu)
      
# subtract overscan bias, trim, and merge quadrants into a single "otm" image

otmImg,quadBias,quadStd = otmProc(hdu)

# make an image header data unit for the OTM image and append it into the main HDU
# with a copy of the full header from the primary HDU plus header cards for
# the bias overscan values subtracted.

otmHDU = fits.ImageHDU(data=otmImg,header=hdu[0].header,name="Merged")

for quad in [1,2,3,4]:
    otmHDU.header[f"Q{quad}Bias"] = (quadBias[quad-1],f"Q{quad} median overscan bias [DN]")
    otmHDU.header[f"Q{quad}Std"] = (quadStd[quad-1],f"Q{quad} overscan bias stdev [DN]")

hdu.append(otmHDU)

# write to a new file

try:
   hdu.writeto(newFITS)
except Exception as exp:
   print(f"Could not write {newFITS} - {exp}")
   
hdu.close()

sys.exit(0)