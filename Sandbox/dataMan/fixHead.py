from astropy.io import fits
from astropy.table import Table
from astropy import units as u
from astropy.coordinates import SkyCoord, EarthLocation
from astropy.time import Time

import sys

if len(sys.argv) == 3:
   origFits = sys.argv[1]
   newFits = sys.argv[2]
else:
   print("Usage: fixHead.py origFits newFits")
   sys.exit(1)

try:
   hdu = fits.open(origFits)
except Exception as exp:
   print(f"ERROR: could not open {origFits} - {exp}")
   sys.exit(1)

# get header parameters we need

c0=int(hdu[0].header['ref-pix1']) # reference pixel 
r0=int(hdu[0].header['ref-pix2'])

nc=hdu[1].header['naxis1'] # total size of a quadrant (all same)
nr=hdu[1].header['naxis2']

ncbias = hdu[1].header['ovrscan1'] # bias columns

# compute corrected DETSEC and LVTn parameters

detsec1=f"[{c0-nc+ncbias+1}:{c0},{r0-nr+1}:{r0}]"
detsec2=f"[{c0+nc+ncbias}:{c0+1},{r0-nr+1}:{r0}]" # flip x
detsec3=f"[{c0-nc+ncbias+1}:{c0},{r0+1}:{r0+nr}]"
detsec4=f"[{c0+nc+ncbias}:{c0+1},{r0+1}:{r0+nr}]" # flip x

q24_LTV1 = c0 + nc - ncbias + 1 
q34_LTV2 = -r0

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

# Get the Archon controller status snapshot from HDU 5 (bintable)

t = Table(hdu[5].data)
archonDict = dict(zip(t['Keyword'].tolist(),t['Value'].tolist()))

# fix CCDTEMP and BASETEMP

try:
   ccdTemp = float(archonDict['MOD10/TEMPA'])
except:
   ccdTemp = -999.9 # no-read value
hdu[0].header['CCDTEMP'] = (ccdTemp,'CCD Detector Temperature [degrees C]')

try:
   baseTemp = float(archonDict['MOD10/TEMPB'])
except:
   baseTemp = -999.9 # no-read value
hdu[0].header['BASETEMP'] = (baseTemp,'CCD Mount Base Temperature [degrees C]')

# fix DATE-OBS to make it FITS- and LBT Archive format compliant ISO8601

try:        
   dateObs = hdu[0].header["DATE-OBS"]
   utcObs = hdu[0].header["UTC-OBS"]
   newDateObs = f"{dateObs}T{utcObs}"
   hdu[0].header["DATE-OBS"] = (newDateObs,"UTC Date at start of obs")
            
   # compute modified Julian Date
            
   obsTime = Time(newDateObs,format="isot",scale="utc")
   hdu[0].header["MJD-OBS"] = (obsTime.mjd,"Modified JD at start of obs")
            
except Exception as err:
   print(f"Could not process date/time in {fitsFile} - corrupted header? - {err}")
        
# Compute heliocentric and barycentric JD.  HJD is in UTC time scale,
# but BJD is in Barycentric Dynamical Time (TDB).  We need site and
# target coordinate info for this.
        
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

   hdu[0].header["HJD-OBS"] = (obsHelio.mjd,"Barycentric MJD at start of obs [TDB]")
   hdu[0].header["BJD-OBS"] = (obsBary.mjd,"Heliocentric MJD at start of obs [UTC]")
except:
   # might have header corruption, but this is non-critical so let pass
   pass
      
# write out fixed FITS file

try:
   hdu.writeto(newFits)
except Exception as exp:
   print(f"Could not write {newFits} - {exp}")
   
hdu.close()

'''
IRAF transform between CCD pixel (Cx,Cy) and Image (Ic,Il) coords
source: https://www.cfht.hawaii.edu/~veillet/imagedef.html

    Ic = LTM1_1 * Cx + LTM1_2 * Cy + LTV1
    Il = LTM2_1 * Cx + LTM2_2 * Cy + LTV2

    Cx = ( LTM2_2 * (Ic - LTV1) - LTM1_2 * (Il - LTV2)) /
	 (LTM1_1 * LTM2_2 - LTM1_2 * LTM2_1)
    Cy = (-LTM2_1 * (Ic - LTV1) + LTM1_1 * (Il - LTV2)) /
	 (LTM1_1 * LTM2_2 - LTM1_2 * LTM2_1)
'''
