#!/usr/bin/env python
#
# python options:
#   LBT: /usr/bin/env python (or python3)
#   OSU: /usr/bin/env python
#
# modsAlign - align MODS long-slit and multi-slit masks
#
# Version 2 of modsAlign is a ground-up rewrite of the original
# modsAlign program.  The final release version of old modsAlign is
# version 1.6.3.
#
# This version uses separate named ds9 windows for MODS1 and MODS2 to
# permit the execution of simultaneous instances on a single machine
# for performing binocular mask alignment.
#
# The biggest change from version 1 is the removal of all of pyraf
# except for irafdisplay() for CDL-like image interaction (i.e., 99.9%
# pyraf-free).  This allows use of ds9's tools for changing the
# display's intensity stretch and scaling (yay!), and eliminates the
# requirement of marking alignment stars in an exact order (double
# yay!).  The result is a simplified work flow that makes modsAlign v2
# very fast and less susceptable to time-wasting mistakes.  This is
# crucial for efficient binocular operation where the observers need
# to juggle two mask alignments at the same time.
#
# Other features new to version 2:
#
#   Flexible command-line parsing so that precise order of file names
#   on the command line is no longer fixed. It uses a combination of
#   the file extension and the contents of the FITS headers (as
#   needed) to distinguish between the mask, field, and MMS files.
#
#   A simple yet robust automatic star finding algorithm for MOS
#   alignment that tries to find the nearest bright object to each
#   alignment box.  Users can expand the search radius around each box
#   with the --sw (search window multipler) option, but with the
#   expected problem that a wider search area reduces the robustness,
#   especially in low-latitude fields.  An interactive editing mode
#   lets you correct or delete bad alignment star selections by the
#   autofinder.
#
#   The MOS mask alignment parameters (dX, dY, dRot) are computed by
#   using an affine transform implemented in numpy.linalg, completely
#   bypassing IRAF geomap and its discontents.  By getting rid of both
#   imexam and geomap, this version completely eliminates the need for
#   the various auxiliary ASCII text files created by IRAF that
#   cluttered up the place when running the original modsAlign.
#
#   Implements a simple iterative fit facility (e.g., --maxiter 5
#   --reject 2) to allow the system to reject outliers/bad stars, it
#   is generally less useful than you might think as the number of
#   alignment boxes gets smaller.  Default is no iterations, as
#   generally this isn't needed, and with the usual small number of
#   alignment boxes, iteration is less robust (as you'd expect).
#
#   Do you feel lucky?  Try "--turbo" mode.  It'll change your life,
#   but like life it offers no guarantees.  Surprisingly robust in
#   most cases...  But, it is only implemented for MOS mask alignment
#   as there is no sensible way to implement turbo mode for long-slit
#   or calibration slit alignment.
#
#   ds9path is defined to allow use of a "safe" verified version of
#   ds9.  There are some known bugs in past versions of ds9 that can
#   cause problems with the pyraf irafdisplay interaction.  If ds9path
#   is set to "", then whatever the default ds9 in the standard
#   executable path is used.
#
#   MODS instrument/channel parameters and conversion methods (e.g.,
#   pixels to arcsec) are defined in a new modsInst class to contain
#   the various data and convenience methods.
#
# Environment:
#   MODS_IMGDIR - default directory for raw images (e.g., /newdata)
#   MODS_MMSDIR - default directory for MMS files (e.g., ./)
#
#   If these are undefined, the default raw image and MMS file paths
#   are the current working directory (./).
#
# Author:
#   Rick Pogge (OSU Astronomy Dept).
#   pogge.1@osu.edu
#
# Modification History:
#   2016 Dec 03 - First alpha release version 2 for LBTO, based on the
#                 mosAlign concept testbed [rwp/osu]
#   2016 Dec 05 - Iterations based on LBTO comments, addressing
#                 various bugs and missing features. [rwp/osu]
#   2016 Dec 06 - Advanced to beta, more bug squashing, refinement of target
#                 identification and response to comments from Olga
#                 Kuhn doing beta testing at LBTO [rwp/osu]
#   2016 Dec 07 - Introduced iterative centroid fitting, encapsulating
#                 all Gaussian Marginal-Sum centroid calculation in a
#                 function named findCentroid(), removing all
#                 gmsCentroid() calls from main. Iterative centroid
#                 makes for more robust star and target centroids
#                 [rwp/osu]
#   2016 Dec 08 - Auto-centering alignment boxes immediately for
#                 initial and interactive box selection. Moving some
#                 verbose output to debug mode and taking down
#                 beta-test scaffolding preparatory to release to
#                 observers [rwp/osu]
#   2016 Dec 12 - fixed bug in manual box selection introduced by
#                 re-organization from the previous version [rwp/osu]
#   2016 Dec 15 - added try/except for ds9 instance connection, as if
#                 some other user has a ds9 instance with the same name
#                 running it raises an exception [rwp/osu]
#   2016 Dec 20 - First LBTO release ready for night-time use [rwp/osu]
#   2016 Dec 21 - Added function to flag if a star or box marked in
#                 the interactive selection routines is already
#                 selected to avoid duplicates (e.g., accidentally
#                 marking an alignment box more than once) [rwp/osu]
#   2016 Dec 22 - Added drawing of the nominal wide slit position on
#                 top of the field image in -r mode at request of
#                 O. Kuhn, restoring a feature from V1 inadvertently
#                 omitted during V2 development [rwp/osu]
#   2016 Dec 29 - Refined the automatic long-slit centering algorithm
#                 in response to problems in field testing (tolerances
#                 for the finder were too tight, causing it to just
#                 miss the slit in some cases).  [rwp/osu]
#   2018 May 23 - Experimental support for an offset snap-n-shuffle (sns)
#                 slit mask [rwp/osu]
#   2018 Sep 13 - Added --slit option to use a named slit in a MOS
#                 mask as the acquisition slit [rwp/osu]
#   2024 Nov 30 - Fixed a small bug with the --slit option that was 
#                 discovered when it was run on a mask with no boxes [opk/lbto]
#   2024 Dec 24 - Updated to astropy-samp-ds9 [mdcb/lbto]
#
# ---- version 3.1.x ----
#
#   2026 Jan 18 - Updates for the MODS Archon controllers - see notes [rwp/osu]
#   2026 Jan 20 - Updates from live testing [rwp/osu]
#   2026 Feb 17 - Updated xRef/yRef for any acq image size, if we have
#                 a field image override maskScale for image scaling [rwp/osu]
#   2026 Feb 20 - Fixes from testing with live images [rwp/osu]
#
#---------------------------------------------------------------------------

import math
import time
import scipy
import getopt
import signal
import os, sys
import termios
import subprocess
import numpy as np

from pathlib import Path

from scipy import ndimage

from astropy.io import fits

from scipy.optimize import curve_fit

from lbto.sciops.misc import logger, slack, beep, DS9IgnoreTimeoutWithLogger

# Version info

verName = "modsAlign v3.2.1"
verDate = "2026-02-20"

log = logger(f"modsAlign-{os.environ.get('USER','anon')}")

# Silence nuisance FITS warnings

import warnings
warnings.filterwarnings('ignore',category=UserWarning, append=True)
warnings.filterwarnings('ignore',category=RuntimeWarning, append=True)
warnings.filterwarnings('ignore',category=FutureWarning, append=True)
warnings.filterwarnings('ignore',category=DeprecationWarning, append=True)

from astropy.utils.exceptions import AstropyWarning
warnings.filterwarnings('ignore',category=AstropyWarning, append=True)

# LBTO facility paths

modsDir = Path(__file__).resolve().parents[0]

# Where are we running modsAlign?  This matters to ds9.

myDir = os.getcwd()

# If MODS_IMGDIR is defined, use this as the default raw image
# directory, otherwise set it to ./

imgDir = os.getenv("MODS_IMGDIR","./")

# If MODS_MMSDIR is defined, use it as the MMS file directory,
# otherwise set it to ./

mmsDir = os.getenv("MODS_MMSDIR","./")

# Global defaults

maxIter = 1      # disable outlier iteration by default
rejSigma  = 4.0  # default rejection threshold in sigma units

# default star search box: margin around alignment box as wide as the box

defSearch = 1.5
searchFac = defSearch

# default box/star matching radius multiplier

defMatch = 2.0        # multiplier for star/box XY matching
matchFac = defMatch

# define interactive key press mappings

keySaveQuit = 'q'
keyAbort = 'c'
keyMarkAuto = 'a'
keyMarkExact = 'x'
keyMarkDelete = 'd'
keyHelp = 'h'
keyRadius = 'r'

# This input function first clears any buffered input that might still
# be around and then calls the real input function. we don't want any
# previous buffered input characters to interfere with our current input.

def unbuffered_input(prompt=''):
    # notice we only want to flush the input queue (and therefore discard
    # its contents), we do not want affect the output queue, so we'll
    # be using the termios.TCIFLUSH selector!
    termios.tcflush(sys.stdin, termios.TCIFLUSH)
    return input(prompt)

#---------------------------------------------------------------------------
#
# printUsage() - print the modsAlign usage message
#
# Set short=True to print only a 1-line synopsis
#

def printUsage(short=False):
    print("Usage: modsAlign [options] maskImage fieldImage [mmsFile]")
    if short:
        return
    print("Where:")
    print("   maskImage = thru-mask FITS image")
    print("  fieldImage = acquisition field FITS image")
    print("    mmsFile  = MODS Mask Specification (.mms) file")
    print("Options:")
    print("  -y dY long-slit acquisition to dY offset from center in arcsec")
    print("  -r    long-slit acquisition to preset LS60x5 center")
    print("  -l    fully manual long-slit acquisition")
    print("  -v    verbose debugging output (very noisy)")
    print("  -B    suppress quick bias subtraction for bright targets")
    print("Advanced Options:")
    print("  --info  print version info and exit")
    print("  --slit=ID use named slit ID in mmsFile as the acquistion slit")
    print("  --sw f  change the star search width factor [default: %.1f]" % (searchFac))
    print("  --mw f  change the star-to-box matching width factor [default: %.1f]" % (matchFac))
    print("  --turbo turbo mode, don't ask questions, except at the end")
    print("  --maxiter N iterate MOS fit up to N times [default: %d]" % (maxIter))
    print("  --reject S  reject S*sigma outliers [default: %.1f]" % (rejSigma))
    print("              requires --maxiter > 1")

#--------------------------------------------------------------------------
#
# sigHandler() - trap SIGINT (Ctrl+C)
#
# Ctrl+C can do wicked-bad voodoo things to ds9 interaction, so we try
# to trap it here using the python signal module.
#

def sigHandler(signal,frame):
    print("You pressed Ctrl+C, stop that!")
    log.warning('You pressed Ctrl+C, stop that!')

#---------------------------------------------------------------------------
#
# modsInst: class for MODS instrument-specific utilities
#
# Initializes the coefficients for the given instrument instance
# (e.g., MODS1R), then provides methods for computing the transform
# given mask x,y coordinates, the image and pixel scales, etc.
#
# Other methods provide for convenient calculation of detector and
# mask unit conversions, etc.
#
# Updates to coordinate transforms, rotator center, and photometric
# calibration target center are made in this class definition.
#

class modsInst:

    # __init__ defines the transform coefficients by instID
    # or raises an exception if instID is unknown

    def __init__(self,myInstID=None,myNX=None,myNY=None):
        self.instID = myInstID
        self.lbtScale = 0.600    # mm/arcsec in f/15 LBT Direct Gregorian Focal Plane

        # These parameters are
        if self.instID == None:
            raise Exception('Must specify the MODS instance, e.g., MODS1')

        else:
            if self.instID.lower() == 'mods1r':
                # MODS1 Red Channel
                self.modsID = "MODS1"
                self.modsCmd = "modsCmd --mods1"
                self.rotSign = -1.0
                self.pixScale = 0.1228  # arcsec/pixels
                self.xRotCen = 1505
                self.yRotCen = 1537
                # LS60x5 slit reference x-position updated 2022 Sep 26 [opk/lbto]
                self.xPhotCal = 499.9
                self.yPhotCal = 620.0
                # Mask to CCD coordinate transform
                # Measured 2012 Apr 28 from image mods1r.20120426.0001.fits [rwp/osu]
                self.nx0 = 3088
                self.ny0 = 3088
                self.x0 = 1537.828270817946
                self.y0 = 1560.490136167825
                self.CL1_1 = 1536.82827081795
                self.CL2_1 = 1559.49013616783
                self.CL1_2 = -13.58955501388256
                self.CL2_2 = 0.09838863450110919
                self.CL1_3 = 0.1125122577171493
                self.CL2_3 = 13.58896520871774
                self.XA_11 = -1.194623590087266
                self.YA_11 = -0.1246226672745723
                self.XA_21 = 0.004174701818556745
                self.YA_21 = -0.003451244444165062
                self.XA_31 = 2.135464120540344E-4
                self.YA_31 = 4.209709382273207E-5
                self.XA_12 = -1.185992357645963E-4
                self.YA_12 = -0.003123867298523521
                self.XA_22 = -3.141290924905715E-5
                self.YA_22 = -9.268193978392627E-6
                self.XA_32 = 2.878365436440259E-8
                self.YA_32 = 9.133817930178020E-7
                self.XA_13 = 1.826012614347623E-4
                self.YA_13 = 2.177783555961285E-5
                self.XA_23 = -1.223861713554312E-6
                self.YA_23 = 1.009853715409536E-6
                self.XA_33 = 2.701157426581779E-9
                self.YA_33 = -6.340303334968345E-9

            elif self.instID.lower() == 'mods1b':
                # MODS1 Blue Channel
                self.modsID = "MODS1"
                self.modsCmd = "modsCmd --mods1"
                self.rotSign = -1.0
                self.pixScale = 0.120  # arcsec/pixels
                self.xRotCen = 1505
                self.yRotCen = 1537
                # LS60x5 slit reference x-position updated 2022 Sep 26 [opk/lbto]
                self.xPhotCal = 504.5
                self.yPhotCal = 620.0
                # Sieve Mask Transform:
                # Measured 2012 Apr 28 from image mods1b.20120426.0001.fits [rwp/osu]
                self.nx0 = 3088
                self.ny0 = 3088
                self.x0 = 1537.857128532157
                self.y0 = 1554.052660988475
                self.CL1_1 = 1536.85712853216
                self.CL2_1 = 1553.05266098848
                self.CL1_2 = -13.92879686899504
                self.CL2_2 = -0.02617145325242218
                self.CL1_3 = -0.02305283693008685
                self.CL2_3 = 13.92149778584121
                self.XA_11 = 1.154135364434469
                self.YA_11 = -0.2128584990629176
                self.XA_12 = -2.569155942564166E-4
                self.YA_12 = 1.076380187359115E-4
                self.XA_13 = -2.087501301870838E-4
                self.YA_13 = 3.321369567728565E-6
                self.XA_21 = 0.002541329101387684
                self.YA_21 = -6.114711649289867E-4
                self.XA_22 = -8.811659931320620E-5
                self.YA_22 = -3.969341829711140E-6
                self.XA_23 = 0.0
                self.YA_23 = 0.0
                self.XA_31 = -2.360311288150591E-4
                self.YA_31 = 7.886296288944751E-5
                self.XA_32 = 0.0
                self.YA_32 = 0.0
                self.XA_33 = 0.0
                self.YA_33 = 0.0

            elif self.instID.lower() == 'mods2r':
                # - MODS2R derived from the sudoku mask 2016-02-14 [rwp]
                self.modsID = "MODS2"
                self.modsCmd = "modsCmd --mods2"
                self.rotSign = -1.0
                self.pixScale = 0.1228
                self.xRotCen = 1518
                self.yRotCen = 1522
                # LS60x5 slit reference x-position updated 2022 Sep 26 [opk/lbto]
                self.xPhotCal = 502.0
                self.yPhotCal = 590.0
                # Mask Transform:
                # Measured 2016 Mar 7 from mods2r.20160307.0002.fits [rwp/osu]
                # new MODS2 Sieve Mask
                self.nx0 = 3088
                self.ny0 = 3088
                self.x0 = 1539.633228228111
                self.y0 = 1549.739338525228
                self.CL1_1 = 1538.63322822811
                self.CL2_1 = 1548.73933852523
                self.CL1_2 = -13.57728533075886
                self.CL2_2 = -0.02524708297618738
                self.CL1_3 = -0.03371689444459192
                self.CL2_3 = 13.58884684678874
                self.XA_11 = -1.787522506097103
                self.YA_11 = -0.1145034551723603
                self.XA_21 = 0.002079747724643815
                self.YA_21 = 3.458398664221449E-4
                self.XA_31 = 3.025846903756650E-4
                self.YA_31 = -1.696158249071796E-6
                self.XA_12 = -0.001452397724579583
                self.YA_12 = -0.00147802243659268
                self.XA_22 = -2.770754627374278E-5
                self.YA_22 = -2.322940994581582E-5
                self.XA_32 = 5.950214737267883E-7
                self.YA_32 = 4.251274044272997E-7
                self.XA_13 = 2.908655908299184E-4
                self.YA_13 = 4.969767069006697E-5
                self.XA_23 = -4.164178805798189E-7
                self.YA_23 = -1.121266649633963E-7
                self.XA_33 = 6.055794545245245E-9
                self.YA_33 = -2.853686827931451E-9

            elif self.instID.lower() == 'mods2b':
                self.modsID = "MODS2"
                self.modsCmd = "modsCmd --mods2"
                self.rotSign = -1.0
                self.pixScale = 0.120  # arcsec/pixels
                self.xRotCen = 1518
                self.yRotCen = 1522
                # LS60x5 slit reference x-position updated 2022 Sep 26 [opk/lbto]
                self.xPhotCal = 510.0
                self.yPhotCal = 590.0
                # Measured 2016 Mar 7 from mods2b.20160307.0002.fits [rwp/osu]
                # new MODS2 Sieve Mask
                self.ny0 = 3088
                self.nx0 = 3088
                self.x0 = 1548.46179290629
                self.y0 = 1521.816799967785
                self.CL1_1 = 1547.46179290629
                self.CL2_1 = 1520.81679996779
                self.CL1_2 = -13.87880930733258
                self.CL2_2 = -0.01625107433583811
                self.CL1_3 = -0.01542362656334407
                self.CL2_3 = 13.88973247533051
                self.XA_11 = 0.8443954448958198
                self.YA_11 = -0.1263138962461641
                self.XA_21 = 0.001628164381288277
                self.YA_21 = 5.740974835260505E-4
                self.XA_31 = -1.298110999807756E-4
                self.YA_31 = -2.149866088685041E-5
                self.XA_12 = -0.002661378754853013
                self.YA_12 = -0.002247638071490493
                self.XA_22 = -3.458353608604697E-5
                self.YA_22 = -3.800626578457055E-5
                self.XA_32 = 6.835017219769168E-7
                self.YA_32 = 6.505167337534667E-7
                self.XA_13 = -1.609916691750494E-4
                self.YA_13 = 6.408779992055081E-5
                self.XA_23 = -5.815576562780869E-7
                self.YA_23 = -1.783256510682649E-7
                self.XA_33 = -1.661875598354154E-11
                self.YA_33 = -1.042905685380622E-10

            else:
                raise Exception('Unknown/unimplemented MODS instrument/channel - %s' % (self.instID))

            # Transform coordinate offset

            if myNX is None:
                self.dx0 = 0
            else:
                self.dx0 = int(0.5*(self.nx0 - myNX))

            if myNY is None:
                self.dy0 = 0
            else:
                self.dy0 = int(0.5*(self.ny0 - myNY))

    #----------------------------------------------------------------
    #
    # MODS instrument/channel specific transformation methods:
    #

    # linTran() - Linear part of the mask-to-CCD transform

    def linTran(self,xr,yr):
        xf = self.CL1_1 + self.CL1_2*xr + self.CL1_3*yr
        yf = self.CL2_1 + self.CL2_2*xr + self.CL2_3*yr
        return xf,yf

    # polyTran() - Polynomial ("distortion") part of the mask-to-CCD transform

    def polyTran(self,xr,yr):
        xf = self.XA_11 +self.XA_21*xr + self.XA_31*xr*xr + self.XA_12*yr + self.XA_22*xr*yr + self.XA_32*xr*xr*yr + self.XA_13*yr*yr + self.XA_23*xr*yr*yr + self.XA_33*xr*xr*yr*yr
        yf = self.YA_11 + self.YA_21*xr + self.YA_31*xr*xr + self.YA_12*yr + self.YA_22*xr*yr + self.YA_32*xr*xr*yr + self.YA_13*yr*yr + self.YA_23*xr*yr*yr + self.YA_33*xr*xr*yr*yr
        return xf, yf

    # mask2ccd() - Transform (x,y) mm in the LBT focal plane to CCD pixel coordinates

    def mask2ccd(self,x,y):
        xm = np.asanyarray(x)
        ym = np.asanyarray(y)
        xlin,ylin = self.linTran(xm,ym)
        xpol,ypol = self.polyTran(xm,ym)
        return xlin+xpol-self.dx0,ylin+ypol-self.dy0

    # arcsec2pix() - Convert arcsec to CCD pixels

    def arcsec2pix(self,x):
        return x/self.pixScale

    # pix2arcsec() - Convert CCD pixels to arcseconds

    def pix2arcsec(self,x):
        return x*self.pixScale

    # mm2arcsec() - Convert LBT focal plane millimeters to arcseconds

    def mm2arcsec(self,x):
        return x/self.lbtScale

    # arcsec2mm() - Convert arcseconds to mm in the LBT focal plane

    def arcsec2mm(self,x):
        return x*self.lbtScale

    # mm2pix() - Convert mm in focal plane to pixels on the CCD (nominal)

    def mm2pix(self,x):
        return (x/self.lbtScale)/self.pixScale

    # pix2mm() - Convert CCD pixels to mm in the focal plane

    def pix2mm(self,x):
        return x*self.lbtScale*self.pixScale

    # id() - Who am I? (MODS Instrument + channel, e.g., mods1r)

    def id(self):
        return self.instID

    # imgCen() - return the nominal image center in pixels (convenience)

    def imgCen(self):
        return self.x0,self.y0

    # rotCen() - return the nominal rotator center in pixels on the full-frame image (convenience)

    def rotCen(self):
        return self.xRotCen,self.yRotCen

    # modsID() - return the MODS instrument ID (MODS1 or MODS2)

    def modsID(self):
        return self.modsID

    # modsCmd() - return the MODS remote command base syntax (e.g., "modsCmd --mods1")

    def modsCmd(self):
        return self.modsCmd

    # rotSign() - return the MODS rotator sign ("helicity")

    def rotSign(self):
        return self.rotSign

    # photRef() - return the MODS instance's nominal LS60x5 slit "sweet spot"

    def photRef(self):
        return self.xPhotCal,self.yPhotCal


#---------------------------------------------------------------------------
#
# xyRotTran() - 2D rigid-body rotation/translation transform
#
# For mask alignment the transform between mask and star coordinates
# on an image is an affine transform with no skewing or scaling, but
# translation (dx,dy) and rotation (dRot).
#
# This implementation uses the numpy linalg module to do the least
# squares calculation, and sets up the matrix algebra in the classic
# way.  I received guidance from a brief article by Jarno Elonen on 2D
# rigid body/RST transformations between points, so this is not a
# generic affine transform fit but a more specific case.
#
# Our matricies are small enough and the expected transforms are
# sufficiently well-behaved (only small translations/rotations) that
# we can use standard least squares without resort to QR decomposition
# or singular-value decomposition methods for performance improvement.
# The default implemention in numpy.linalg.lstsq() is sufficient.
#
# Method:
#
# Inputs: mask positions: (xm,ym)[i]
#         star positions: (xs,ys)[i]
#
# RST Transform:
#    xs(i) = xm(i)*costh - ym(i)*sinth + dx
#    ys(i) = xm(i)*sinth + ym(i)*costh + dy
#
#    where: sinth = sin(theta), theta = field rotation angle
#           costh = cos(theta)
#
# Formulation of the least squares problem in matrix form:
#
#   A * c = B
#
#   | -ym0  xm0  1   0  |   | sinth |   | xs0 |
#   |  xm0  ym0  0   1  |   | costh |   | ys0 |
#   | -ym1  xm1  1   0  | * |  dx   | = | xs1 |
#   |  xm1  ym1  0   1  |   |  dy   |   | ys1 |
#   |  ...  ... ... ... |               | ... |
#
# Solution is via standard least squares:
#
#      c = (A^t*A)^-1 A^t*B
#
# numpy.linalg.lstsq() does the job.  We also use numpy matrix objects
# for the data.  The functionaly logic is taken directly from Jarno
# Elonen's article (elonen.iki.fi/code/misc-notes/2d-rigid-fit).
#
# Usage:
#   xyRotTran(mask_xy,star_xy)
#
# Where:
#   mask_xy = array of mask locations in pixels ([xm1,ym1],[xm2,ym2],...,[xmN,ymN])
#   star_xy = array of star centroids in pixels ([xs1,ys1],[xs2,ys2],...,[xsN,ysN])
#
# both mask_xy and star_xy must be the same length *and* aligned
#
# Returns: dx, dy in pixels, theta in degrees, and coeffs
#
# It is the responsibility of the calling program to convert dx,dy
# into appropriate units for translating/rotating the instrument.
#
# Author:
#   R. Pogge, OSU Astronomy
#   pogge.1@osu.edu
#   2013 Dec 10
#

def xyRotTran(mask_xy,star_xy):

    # Build the "coefficient" matrix (A) with the mask position data

    dataA = []
    for dp in mask_xy:
        dataA.append([-dp[1],dp[0],1,0])
        dataA.append([ dp[0],dp[1],0,1])
    A = np.matrix(dataA)

    # Build the B matrix with the star position data

    dataB = []
    for dp in star_xy:
        dataB.append(dp[0])
        dataB.append(dp[1])
    B = np.matrix(dataB).T   # transpose

    # Do the least squares solution for coefficients c

    (coeffs,resid,rank,s) = np.linalg.lstsq(A,B)

    c = coeffs.ravel().tolist()[0] # ugh...

    # Convert best fit coefficients to dx,dy in arcsec and rotation in degrees

    dx = c[2]
    dy = c[3]
    theta = math.degrees(math.atan2(c[0],c[1]))

    return dx, dy, theta, c

#----------------------------------------------------------------
#
# findNearXY() - find object nearest a given X,Y location
#
# Finds the item in an XY coordinate list nearest a given X,Y
# position, and returns the index.  Must be within searchRad to be a
# valid match.
#
# Uses a simple gravity algorithm.
#

def findNearXY(xPts,yPts,x,y,searchRad=10):
    nearList = []
    for i in range(len(xPts)):
        rad = math.sqrt((xPts[i]-x)**2+(yPts[i]-y)**2)
        if rad <= searchRad:
            nearList.append(i)

    numNear = len(nearList)

    if numNear == 0:
        return -1   # no matches found
    elif numNear == 1:
        return nearList[0]  # only one match found

    # More than one match found, determine which is closest

    for i in nearList:
        if i==nearList[0]:
            drmin = math.sqrt((xPts[i]-x)**2+(yPts[i]-y)**2)
            imin = 0
        else:
            dr = math.sqrt((xPts[i]-x)**2+(yPts[i]-y)**2)
            if dr < drmin:
                drmin = dr
                imin = i

    return i

#---------------------------------------------------------------------------
#
# isDuplicate() - test if (x,y) point already in a list of XY points
#
# input:
#   x,y   = vector of x,y points
#   xPts,yPts = test point (scalars)
#   tol   = radial tolerance in xy units (default: 1)
#
# return:
#   True if x,y is closer than tol to any member of xPts,yPts
#   False if not a duplicate by this criterion.
#
# Uses a simple r-squared radial test, declares a "duplicate" if the
# test (x,y) position is within tol of any (x,y) point in the input
# arrays.
#
# This is a variant on findNearXY() that just tests for proximity,
# rather than finding the closest match.
#

def isDuplicate(xPts,yPts,x,y,tol=1):
    isDup = False
    minR2 = tol*tol
    for i in range(len(xPts)):
        dx = x-xPts[i]
        dy = y-yPts[i]
        r2 = dx*dx+dy*dy
        if r2 <= minR2:
            isDup = True
    return isDup

#----------------------------------------------------------------
#
# findPeak() - find the peak pixel nearest an XY pixel location
#
# Finds the peak pixel within radius of the given X,Y coordinates
#

def findPeak(image,x,y,radius=2):
    ny,nx = image.shape
    # search rectangle, make sure within the image boundaries
    xmin = int(max(0,x-radius))
    xmax = int(min(nx,x+radius))
    ymin = int(max(0,y-radius))
    ymax = int(min(ny,y+radius))
    maxPix = np.amax(image[ymin:ymax,xmin:xmax])
    maxIdx = np.argwhere(image[ymin:ymax,xmin:xmax]==maxPix)
    # even if many found, choose the first
    return maxIdx[0][1]+xmin,maxIdx[0][0]+ymin,maxPix

#----------------------------------------------------------------
#
# matchLists() - match star and mask lists within a search radius
#

def matchLists(xStar,yStar,xMask,yMask,searchRad,verbose=False):
    starXY = []
    maskXY = []
    for i in range(len(xStar)):
        iNear = findNearXY(xMask,yMask,xStar[i],yStar[i],searchRad)

        if iNear < 0:
            print("No alignment box within %d pixels of star at (%.1f,%.1f)" % (searchRad,xStar[i],yStar[i]))
        else:
            starXY.append([xStar[i],yStar[i]])
            maskXY.append([xMask[iNear],yMask[iNear]])
            if verbose:
                print("Found match: star %d matches with alignment box %d" % (i+1,iNear+1))

    return starXY,maskXY

#----------------------------------------------------------------
# Centroid Methods
#----------------------------------------------------------------
#
# gmsCentroid() - Gaussian Marginal Sum Centroid Method
#

def gmsCentroid(image,x,y,xWid,yWid,axis='both',verbose=False):
    ny,nx = image.shape
    if axis.lower() == 'both':
        doXaxis = True
        doYaxis = True
    elif axis.lower() == 'x':
        doXaxis = True
        doYaxis = False
        yCen = None
        yErr = None
    elif axis.lower() == 'y':
        doXaxis = False
        doYaxis = True
        xCen = None
        xErr = None
    else:
        raise Exception("Invalid axis '%s' - must be one of {x,y,both}" % (axis))

    # Subimage region, make sure it is within the image boundaries

    xmin = int(max( 0,x-xWid))
    xmax = int(min(nx,x+xWid))
    ymin = int(max( 0,y-yWid))
    ymax = int(min(ny,y+yWid))
    boxImg = np.array(image[ymin:ymax,xmin:xmax]) # cut out the sub image

    # Form the marginal sums along the working axis

    if doXaxis:
        xMS  = boxImg.mean(axis=0)  # X marginal sum
        xPix = xmin+np.arange(len(xMS))

    if doYaxis:
        yMS  = boxImg.mean(axis=1)  # Y marginal sum
        yPix = ymin+np.arange(len(yMS))

    # Guesses of the Gaussian fit parameters common to both axes

    bkg0 = np.median(boxImg)
    s0 = 1.0

    # Marginal sum Gaussian fits

    if doXaxis:
        maxX = np.amax(xMS)
        x0 = xPix[np.argwhere(xMS==maxX)][0][0]
        a0 = maxX-bkg0
        px=[bkg0,a0,x0,s0]
        xCoeff,varX = curve_fit(skyGauss,xPix,xMS,p0=px)
        xFit = skyGauss(xPix,*xCoeff)
        xCen = xCoeff[2]
        xErr = math.sqrt(varX[2,2])
        xBkg = xCoeff[0]

    if doYaxis:
        maxY = np.amax(yMS)
        y0 = yPix[np.argwhere(yMS==maxY)][0][0]
        a0 = maxY-bkg0
        py=[bkg0,a0,y0,s0]
        yCoeff,varY = curve_fit(skyGauss,yPix,yMS,p0=py)
        yFit = skyGauss(yPix,*yCoeff)
        yCen = yCoeff[2]
        yErr = math.sqrt(varY[2,2])
        yBkg = yCoeff[0]

    # Some quality checks

    # If either fit sigma is <0, probable failure even if a formal
    # fit was found (curve_fit raised no exceptions)

    if doXaxis and xCoeff[3] < s0:
        raise Exception('Cannot compute X centroid - sigX=%.3f < %.3f' % (xCoeff[3],s0))

    if doYaxis and yCoeff[3] < s0:
        raise Exception('Cannot compute Y centroid - sigY=%.3f < %.3f' % (yCoeff[3],s0))

    # Cleanup

    del boxImg
    if doXaxis:
        del xPix
        del xMS
    if doYaxis:
        del yPix
        del yMS

    # Return data

    if doXaxis and doYaxis:
        return xCen,yCen,xErr,yErr
    elif doXaxis and not doYaxis:
        return xCen,xErr
    elif doYaxis and not doXaxis:
        return yCen,yErr

#---------------------------------------------------------------------------
#
# smsBisector() - Sobel Marginal Sum Bisector Method
#
# Returns xCen, yCen, and median in the box
#

def smsBisector(image,x,y,xWid,yWid,axis='both',clipStars=True,wfac=1,verbose=False):
    ny,nx = image.shape
    if axis.lower() == 'both':
        doXaxis = True
        doYaxis = True
        do2D = True
    elif axis.lower() == 'x':
        doXaxis = True
        doYaxis = False
        do2D = False
        yCen = None
    elif axis.lower() == 'y':
        doXaxis = False
        doYaxis = True
        do2D = False
        xCen = None
    else:
        raise Exception("Invalid axis '%s' - must be one of {x,y,both}" % (axis))

    # Two rectangles:
    #   1) Main Box: 2*wfac*(xWid x yWid) for subimage
    #   2) Inner Box: xWid x wWid for star clipping

    # Make sure the main box is within the image boundaries

    xmin = int(max( 0,x-wfac*xWid))
    xmax = int(min(nx,x+wfac*xWid))
    ymin = int(max( 0,y-wfac*yWid))
    ymax = int(min(ny,y+wfac*yWid))
    subImg = np.array(image[ymin:ymax,xmin:xmax])

    # Star clipping needed in the inner box?

    bkgMed = 0.0
    if clipStars:
        xhw = xWid/2  # half-width
        yhw = yWid/2
        y1=int(y-yhw)
        y2=int(y+yhw)
        x1=int(x-xhw)
        x2=int(x+xhw)
        bkgMed = np.median(image[y1:y2,x1:x2])
        bkgSig = math.sqrt(bkgMed)
        thresh = bkgMed + 2*bkgSig
        if np.any(subImg>thresh):
            subImg[subImg>thresh] = bkgMed

    # Form the marginal sums along the working axis and compute a 1D
    # squared Sobel filtered marginal sum

    if doXaxis:
        xMS  = subImg.mean(axis=0)  # X marginal sum
        xPix = xmin+np.arange(len(xMS))
        xSobel = ndimage.sobel(xMS,0)
        xSobel *= xSobel
        xSobel[0] = 0.0
        xSobel[len(xSobel)-1]=0.0

    if doYaxis:
        yMS  = subImg.mean(axis=1)  # Y marginal sum
        yPix = ymin+np.arange(len(yMS))
        ySobel = ndimage.sobel(yMS,0)
        ySobel *= ySobel
        ySobel[0] = 0.0
        ySobel[len(ySobel)-1]=0.0

    # Find the peaks of the squared-sobel filter marginal sums.  The
    # guess is that the peaks are in each half of the profile.  These
    # give us the initial guesses for the two-Gaussian fits.

    xLeft = 0.0
    yLeft = 0.0
    xRight = 0.0
    yRight = 0.0
    if doXaxis:
        xm1 = xmin + np.argmax(xSobel[:(len(xSobel)//2)])
        xa1 = np.amax(xSobel[:(len(xSobel)//2)])
        xs1 = 1.0

        xm2 = xmin + 0.5*len(xSobel) + np.argmax(xSobel[(len(xSobel)//2):])
        xa2 = np.amax(xSobel[(len(xSobel)//2):])
        xs2 = 1.0

        px = [xa1,xm1,xs1,xa2,xm2,xs2]

        try:
            xCoeff,xCov = curve_fit(twoGauss,xPix,xSobel,p0=px)
        except:
            raise Exception('smsBisector() - fit failed on X marginal sum')

        xFit = twoGauss(xPix,*xCoeff)
        xCen = 0.5*(xCoeff[1] + xCoeff[4])
        xLeft = xCoeff[1]
        xRight = xCoeff[4]

    if doYaxis:
        ym1 = ymin + np.argmax(ySobel[:(len(ySobel)//2)])
        ya1 = np.amax(ySobel[:(len(ySobel)//2)])
        ys1 = 1.0

        ym2 = ymin + 0.5*len(ySobel) + np.argmax(ySobel[(len(ySobel)//2):])
        ya2 = np.amax(ySobel[(len(ySobel)//2):])
        ys2 = 1.0

        py = [ya1,ym1,ys1,ya2,ym2,ys2]

        try:
            yCoeff,yCov = curve_fit(twoGauss,yPix,ySobel,p0=py)
        except:
            raise Exception('smsBisector() - fit failed on Y marginal sum')

        yFit = twoGauss(yPix,*yCoeff)
        yCen = 0.5*(yCoeff[1] + yCoeff[4])
        yLeft = yCoeff[1]
        yRight = yCoeff[4]

    # Quality checks here someday...

    # Diagnostic output if running in verbose mode

    if doXaxis and verbose:
        print("  X: Peaks %.2f %.2f, Bisector %.2f s1=%.2f s2=%.2f" % (xCoeff[1],xCoeff[4],xCen,xCoeff[2],xCoeff[5]))
        print("  Slit X Width: %.2f pixels" % (xCoeff[4]-xCoeff[1]))
    if doYaxis and verbose:
        print("  Y: Peaks %.2f %.2f, Bisector %.2f s1=%.2f s2=%.2f" % (yCoeff[1],yCoeff[4],yCen,yCoeff[2],yCoeff[5]))
        print("  Slit Y Width: %.2f pixels" % (yCoeff[4]-yCoeff[1]))

    # Clean up

    del subImg
    if doXaxis:
        del xPix
        del xSobel
        del xMS
    if doYaxis:
        del yPix
        del ySobel
        del yMS

    # Return data

    return xCen,yCen,bkgMed

#----------------------------------------------------------------
#
# 1D Gaussian functions
#   oneGauss = 1D Gaussian, no background
#              f(x) = a*exp(-(x-m)^2/2*s^2)
#
#   skyGauss = 1D Gaussian with constant background
#              f(x) = b + a*exp(-(x-m)^2/2*s^2)
#
#   twoGauss = Two 1D Gaussians, no background
#              f(x) = a1*exp(-(x-m1)^2/2*s1^2) + a2*exp(-(x-m2)^2/2*s2^2)
#
# Written in the form required by scipy.optimize.curve_fit()
#

def oneGauss(x,*p):
    a,m,s=p
    return a*np.exp(-(x-m)**2/(2*s**2))

def twoGauss(x,*p):
    a1,m1,s1,a2,m2,s2=p
    return a1*np.exp(-(x-m1)**2/(2*s1**2)) + a2*np.exp(-(x-m2)**2/(2*s2**2))

def skyGauss(x,*p):
    b,a,m,s=p
    return b + a*np.exp(-(x-m)**2/(2*s**2))

#----------------------------------------------------------------
#
# findCentroid() - iterative centroid fitting
#
# Uses gmsCentroid() and sets up an iterative centroid loop
# to try to get the best centroid.  Convergence is change in
# centroid less than tol (computed as a radial offset between
# the pre- and post-iteration measurements).  Wil iterate up
# to maxiter times.
#

def findCentroid(img,x0,y0,cenRad,maxiter=5,tol=0.01,verbose=False):
    # first iteration: use the guess provided
    try:
        xCen0,yCen0,xErr,yErr = gmsCentroid(img,x0,y0,cenRad,cenRad,axis='both')
        gotStar = True
    except Exception as err:
        errStr = err
        gotStar = False

    if not gotStar:
        raise Exception('initial centroid failed - %s' % (errStr))

    if verbose:
        print("  iteration 1: X=%.3f+/-%.3f Y=%.3f+/-%.3f"  % (xCen0+1,yCen0+1,xErr,yErr))

    # Iterate until convergence or maxiter

    for i in range(maxiter-1):
        try:
            xCen,yCen,xErr,yErr = gmsCentroid(img,xCen0,yCen0,cenRad,cenRad,axis='both')
            gotStar = True
        except:
            gotStar = False

        if not gotStar:
            raise Exception('Could not compute refined centroid')

        dX = xCen - xCen0
        dY = yCen - yCen0
        if verbose:
            print("  iteration %d: X=%.3f+/-%.3f Y=%.3f+/-%.3f dX=%.3f dY=%.3f"  % (i+2,xCen+1,xErr,yCen+1,yErr,dX,dY))
        dR = math.sqrt(dX*dX+dY*dY)
        if dR <= tol:
            return xCen,yCen,xErr,yErr
        else:
            xCen0 = xCen
            yCen0 = yCen

    # no convergence, raise exception

    raise Exception('Did not converge within %.2f pix in %d iterations' % (tol,maxiter))

#----------------------------------------------------------------
#
# medImgScale() - median auto scaling within an image
#
# quick and dirty, does the job
#

def medImgScale(image,gain=1.0,fac=1.0):
    med = np.median(image)
    sig = math.sqrt(med/gain)   # Poisson "sigma" modulo e-/ADU gain
    zMin = med - 5.0*sig
    zMax = med + (fac*10.0*sig)

    return zMin, zMax, med, sig

#----------------------------------------------------------------
#
# readMMS()- read and parse a MODS Mask Specification (MMS) file
#

def readMMS(file, verbose=False):
    slitXmm = []
    slitYmm = []
    slitWmm = []
    slitHmm = []
    slitRot = []
    slitID = []

    F=open(file, "r")
    M=F.readlines()[::]
    F.close()
    nrefslits = 0
    if verbose:
        print("Found %d lines in %s" % (len(M), file))

    for i in range(len(M)):
        inStr = M[i].strip()
        if not inStr.startswith('#') and len(inStr)>0: # ignore comments and blank lines
            param,datum = inStr.split()
            slitType,targID,field = param.split('.')
            if slitType == "INS":
                if targID=="RSLIT" and field=="NUMBER":
                    numRef = int(datum)
                    firstTarget = 100+numRef+1  # first non-reference slit ID number
                elif targID[0:4] == "TARG":
                    targNum = int(targID[4:])
                    if targNum >= firstTarget:
                        if field == "XMM":  # X center in mm in focal plane
                            slitXmm.append(-float(datum))
                        elif field == "YMM":
                            slitYmm.append(-float(datum))
                        elif field == "WIDMM":
                            slitWmm.append(float(datum))
                        elif field == "LENMM":
                            slitHmm.append(float(datum))
                        elif field == "ROT":
                            slitRot.append(float(datum))
                        elif field == "NAME":
                            slitID.append(datum)

    # Return the slit data

    return slitXmm, slitYmm, slitWmm, slitHmm, slitRot, slitID

#---------------------------------------------------------------------------
#
# slitRegion() - given a slit position in mm, return a ds9 regions
#                command with the slit transformed the CCD pixels
#
# Inputs:
#   instID - MODS instrument instance
#   sWid = slit width in mm
#   sLen = slit length in mm
#   x0,y0 = slit center in mm [default: 0,0]
#   xBin,yBin = CCD binning factor [default: 1x1]
#   dx,dy = pixel offset to apply [default: 0,0]
#   color = color to use [default: cyan]
#
# Returns:
#   regions command as a string ready to be passed to the DS9.set() method.
#

def slitRegion(instID,sWid,sLen,x0=0.0,y0=0.0,xBin=1,yBin=1,dx=0.0,dy=0.0,color='cyan'):

    # Slit corners in mm: [ll, lr, ur, ul]

    xSmm = [x0-0.5*sWid,x0+0.5*sWid,x0+0.5*sWid,x0-0.5*sWid]
    ySmm = [y0-0.5*sLen,y0-0.5*sLen,y0+0.5*sLen,y0+0.5*sLen]

    # Transform from mm to unbinned CCD pixels

    xCCD,yCCD = instID.mask2ccd(xSmm,ySmm)

    # Account for binning

    if xBin != 1:
        xCCD /= xBin
    if yBin != 1:
        yCCD /= yBin

    if dx != 0.0:
        xCCD += dx
    if dy != 0.0:
        yCCD += dy

    # Build the DS9 regions command that will draw the slit outline
    # as a polygon transformed to binned CCD pixel coordinates.

    regSlit = "regions command {polygon("
    for i in range(len(xCCD)):
        if i == 0:
            regSlit += "%6.2f,%6.2f" % (xCCD[i],yCCD[i])
        else:
            regSlit += ",%6.2f,%6.2f" % (xCCD[i],yCCD[i])
    regSlit += ") # color=%s}" % (color)

    return regSlit

#---------------------------------------------------------------------------
#
# selectBoxes() - interactively select alignment boxes on an ds9 image
#

def selectBoxes(image,d,boxWidth,verbose=False):
    print("\nSelect mask alignment boxes and press 'q' when done.")
    print("Commands:")
    print(f"  '{keyMarkAuto}' select the alignment box nearest the cursor")
    print(f"  '{keyMarkExact}' select the alignment box at the current cursor XY position")
    print(f"  '{keyMarkDelete}' delete the previously selected box nearest the cursor")
    print(f"  '{keySaveQuit}' quit alignment box selection and save choices")
    print(f"  '{keyAbort}' abort alignment box selection and exit modsAlign now")
    print(f"  '{keyHelp}' list interactive cursor commands")
    print("You must select at least two (2) alignment boxes, and the")
    print("cursor must be inside the box being marked.\n")

    xPick = []
    yPick = []
    keepGoing = True
    d.set("regions delete all")
    while keepGoing:
        retStr = d.get('iexam key coordinate')
        bits = [s.strip() for s in retStr.split()]
        if len(bits)<3:
            print(bits)
            cursKey = ''
        else:
            cursKey = bits[0].lower()
            xCurs = float(bits[1])
            yCurs = float(bits[2])

        if cursKey == keySaveQuit:
            print("\nAlignment box selection done.")
            keepGoing = False

        elif cursKey == keyAbort:
            print("\nAborting alignment box selection and exiting modsAlign")
            log.info('Aborting alignment box selection and exiting modsAlign')
            sys.exit(1)

        elif cursKey == keyMarkAuto: # mark using auto-peak assist
            xPeak,yPeak,maxPix = findPeak(image,xCurs,yCurs,boxWidth)
            if verbose:
                d.set("regions command {point %.3f %.3f # point=cross color=green}" % (xPeak+1,yPeak+1))

            try:
                xCen,yCen,boxMed = smsBisector(image,xPeak,yPeak,boxWidth,boxWidth,wfac=1.5,clipStars=True)
                hasBox = True
            except Exception as err:
                print("  * Could not compute box center - %s" % (err))
                hasBox = False

            if hasBox:
                if len(xPick)>0 and isDuplicate(xPick,yPick,xCen+1,yCen+1,tol=boxWidth/2.):
                    print("  * You've already marked that box...")
                else:
                    xPick.append(xCen+1)
                    yPick.append(yCen+1)
                    print("  Alignment box center: X=%.2f Y=%.2f" % (xPick[-1],yPick[-1]))
                    d.set("regions command {box(%.3f,%.3f,%.3f,%.3f) # color=green}" % (xPick[-1],yPick[-1],boxWidth+2,boxWidth+2))

        elif cursKey == keyMarkExact:  # mark using exact XY cursor
            try:
                xCen,yCen,boxMed = smsBisector(image,xCurs,yCurs,boxWidth,boxWidth,wfac=1.5,clipStars=True)
                hasBox = True

            except Exception as err:
                print("  * Could not compute box center - %s" % (err))
                hasBox = False

            if hasBox:
                if len(xPick)>0 and isDuplicate(xPick,yPick,xCen+1,yCen+1,tol=boxWidth/2.):
                    print("  * You've already marked that box...")
                else:
                    xPick.append(xCen+1)
                    yPick.append(yCen+1)
                    print("  Alignment box center: X=%.2f Y=%.2f" % (xPick[-1],yPick[-1]))
                    d.set("regions command {box(%.3f,%.3f,%.3f,%.3f) # color=green}" % (xPick[-1],yPick[-1],boxWidth+2,boxWidth+2))

        elif cursKey == keyMarkDelete:
            if len(xPick) < 2:
                if len(xPick) == 1:
                    print("  Alignment box at X=%.1f Y=%.1f deleted" % (xPick[0],yPick[0]))
                    d.set("regions command {point %.3f %.3f # point=x color=red width=2}" % (xPick[0],yPick[0]))
                    xPick = []
                    yPick = []
                else:
                    print("  ** No boxes to delete")
            else:
                xTest = xCurs
                yTest = yCurs
                iNear = findNearXY(xPick,yPick,xTest,yTest,32)
                if iNear < 0:
                    print("  ** No alignment box near the cursor")
                else:
                    print("  Alignment box at X=%.1f Y=%.1f deleted" % (xPick[iNear],yPick[iNear]))
                    d.set("regions command {point %.3f %.3f # point=x color=red width=2}" % (xPick[iNear],yPick[iNear]))
                    xPick.pop(iNear)
                    yPick.pop(iNear)

        elif cursKey == keyHelp:
            print("\nAlignment Box Selection Commands:")
            print(f"  '{keyMarkAuto}' select the alignment box nearest the cursor")
            print(f"  '{keyMarkExact}' select the alignment box at the current cursor XY position")
            print(f"  '{keyMarkExact}' select the alignment box nearest the cursor XY position")
            print(f"  '{keyMarkDelete}' delete the previously selected box nearest the cursor")
            print(f"  '{keySaveQuit}' quit alignment box selection and save choices")
            print(f"  '{keyAbort}' abort alignment box selection and exit modsAlign now")

    # Bottom of the cursor loop

    return xPick,yPick

#---------------------------------------------------------------------------
#
# selectStars() - interactively select alignment stars on a field image in ds9
#

def selectStars(image,d,verbose=False,cenRad=15):
    print("\nSelect alignment stars and press 'q' when done.")
    print("Commands:")
    print(f"  '{keyMarkAuto}' select the star nearest the cursor with centroid computation")
    print(f"  '{keyMarkExact}' select the star at the current cursor XY position")
    print(f"  '{keyMarkDelete}' delete the alignment star nearest the cursor")
    print(f"  '{keyRadius}' change the star search radius [currently %.1f pixels]" % (cenRad))
    print(f"  '{keySaveQuit}' quit alignment star selection and save choices")
    print(f"  '{keyAbort}' abort alignment star selection and exit modsAlign now")
    print(f"  '{keyHelp}' list interactive cursor commands")
    print("Stars may be selected in any order, at leat two (2) stars needed\n")

    xStar = []
    yStar = []
    keepGoing = True
    while keepGoing:
        retStr = d.get('iexam key coordinate')
        bits = [s.strip() for s in retStr.split()]
        if len(bits)<3:
            print(bits)
            cursKey = ''
        else:
            cursKey = bits[0].lower()
            xCurs = float(bits[1])
            yCurs = float(bits[2])

        if cursKey == keySaveQuit:
            print("\nAlignment star selection done.")
            keepGoing = False

        elif cursKey == keyAbort:
            print("\nAborting alignment star selection and exiting modsAlign")
            sys.exit(1)

        elif cursKey == keyMarkExact:
            if len(xStar)>0 and isDuplicate(xStar,yStar,xCurs,yCurs,tol=cenRad/3.):
                print("  * You've already marked that star...")
            else:
                xStar.append(xCurs)
                yStar.append(yCurs)
                if verbose:
                    print("   [%d]: cursor position: X=%.2f Y=%.2f" % (len(xStar),xStar[-1],yStar[-1]))
                d.set("regions command {point %.3f %.3f # point=cross color=green}" % (xStar[-1],yStar[-1]))

        elif cursKey == keyMarkAuto:
            # find the peak pixel within cenRad of the cursor, use this as the first guess
            xPeak,yPeak,maxPix = findPeak(image,xCurs,yCurs,cenRad)
            try:
                xCen,yCen,xErr,yErr = findCentroid(image,xPeak,yPeak,cenRad,verbose=verbose)
                gotStar = True
            except Exception as err:
                print("* Star not found near %.1f,%.1f" % (xCurs,yCurs))
                print("* Reason: %s" % (err))
                gotStar = False

            if gotStar:
                xCen += 1
                yCen += 1
                if len(xStar)>0 and isDuplicate(xStar,yStar,xCen,yCen,tol=cenRad/3.):
                    print("  * You've already marked that star...")
                else:
                    d.set("regions command {point %.3f %.3f # point=cross color=green}" % (xCen,yCen))
                    d.set("regions command {circle %.3f %.3f %.3f # color=green}" % (xCen,yCen,cenRad))
                    print("  Object centroid: X=%.2f+/-%.2f Y=%.2f+/-%.2f" % (xCen,xErr,yCen,yErr))
                    xStar.append(xCen)
                    yStar.append(yCen)

        elif cursKey == keyMarkDelete:
            if len(xStar) < 2:
                if len(xStar) == 1:
                    print("  Star at X=%.2f Y=%.2f deleted" % (xStar[0],yStar[0]))
                    d.set("regions command {point %.3f %.3f # point=x color=red width=2}" % (xStar[0],yStar[0]))
                    xStar = []
                    yStar = []
                print("  ** No stars left to delete!")
            else:
                xTest = xCurs
                yTest = yCurs
                iNear = findNearXY(xStar,yStar,xTest,yTest,20)
                if iNear < 0:
                    print("  ** No star near the cursor to delete")
                else:
                    print("  Star at X=%.2f Y=%.2f deleted" % (xStar[iNear],yStar[iNear]))
                    d.set("regions command {point %.3f %.3f # point=x color=red width=2}" % (xStar[iNear],yStar[iNear]))
                    xStar.pop(iNear)
                    yStar.pop(iNear)

        elif cursKey == keyRadius: # change the star search radius
            promptStr = "  Change the star search radius [%.1f]: " % (cenRad)
            foo = unbuffered_input(promptStr)
            if len(foo)==0:
                print("  Keeping search radius %.1f pixels" % (cenRad))
            else:
                try:
                    newRad = float(foo)
                    if newRad > 0.0:
                        cenRad = newRad
                        print("  New search radius is %.1f pixels" % (cenRad))
                    else:
                        print("  Search radius must be positive and non-zero - no change")
                except:
                    print("  You must enter a number - no change")

        elif cursKey == keyHelp: # help
            print("\nAlignment Star Selection Commands:")
            print(f"  '{keyMarkAuto}' select the star nearest the cursor with centroid computation")
            print(f"  '{keyMarkExact}' select the star at the current cursor XY position")
            print(f"  '{keyMarkDelete}' delete the alignment star nearest the cursor")
            print(f"  '{keyRadius}' change the star search radius [currently %.1f pixels]" % (cenRad))
            print(f"  '{keySaveQuit}' quit alignment star selection and save choices")
            print(f"  '{keyAbort}' abort alignment star selection and exit modsAlign now")

    # Bottom of the cursor loop

    return xStar,yStar

#---------------------------------------------------------------------------
#
# editStars() - interactively edit alignment star selections on a ds9 image
#

def editStars(image,d,xStar,yStar,verbose=False,cenRad=15):
    print("\nEdit alignment stars and press 'q' when done.")
    print(f"  '{keyMarkAuto}' add the star nearest the cursor with centroid calculation")
    print(f"  '{keyMarkExact}' add a star at the current cursor XY position")
    print(f"  '{keyMarkDelete}' delete the star nearest the cursor")
    print(f"  '{keyRadius}' change the star search radius [currently %.1f pix]" % (cenRad))
    print(f"  '{keySaveQuit}' quit editing and save changes")
    print(f"  '{keyAbort}' abort editing, discard all changes, and continue")
    print(f"  '{keyHelp}' shows a list of all cursor commands\n")

    xNew = xStar
    yNew = yStar
    keepGoing = True
    while keepGoing:
        retStr = d.get('iexam key coordinate')
        bits = [s.strip() for s in retStr.split()]
        if len(bits)<3:
            print(bits)
            cursKey = ''
        else:
            cursKey = bits[0].lower()
            xCurs = float(bits[1])
            yCurs = float(bits[2])

        if cursKey == keySaveQuit:
            print("\nAlignment star editing done.")
            keepGoing = False

        elif cursKey == keyAbort:
            print("\nAborting alignment star editing and discarding all changes.")
            xNew = xStar
            yNew = yStar
            keepGoing = False

        elif cursKey == keyMarkExact:
            if len(xNew)>0 and isDuplicate(xNew,yNew,xCurs,yCurs,tol=cenRad/3.):
                print("  * This star is already selected...")
            else:
                xNew.append(xCurs)
                yNew.append(yCurs)
                if verbose:
                    print("  [%d]: cursor position: X=%.2f Y=%.2f" % (len(xNew),xNew[-1],yNew[-1]))
                d.set("regions command {point %.3f %.3f # point=cross color=green}" % (xNew[-1],yNew[-1]))

        elif cursKey == keyMarkAuto:
            # find the peak pixel within cenRad of the cursor, use this as the first guess
            xPeak,yPeak,maxPix = findPeak(image,xCurs,yCurs,cenRad)
            try:
                xCen,yCen,xErr,yErr = findCentroid(image,xPeak,yPeak,cenRad,verbose=verbose)
                gotStar = True
            except Exception as err:
                print("* Cannot compute centroid of star near (%.1f,%.1f)" % (xCurs,yCurs))
                print("* Reason: %s" % (err))
                gotStar = False

            if gotStar:
                xCen += 1
                yCen += 1
                if len(xNew)>0 and isDuplicate(xNew,yNew,xCen,yCen,tol=cenRad/3.):
                    print("  * This star is already selected...")
                else:
                    d.set("regions command {point %.3f %.3f # point=cross color=green}" % (xCen,yCen))
                    d.set("regions command {circle %.3f %.3f %.3f # color=green}" % (xCen,yCen,cenRad))
                    print("  Object centroid: X=%.2f+/-%.2f Y=%.2f+/-%.2f" % (xCen,xErr,yCen,yErr))
                    xNew.append(xCen)
                    yNew.append(yCen)

        elif cursKey == keyMarkDelete:
            if len(xNew) < 2:
                if len(xNew) == 1:
                    print("  Star at X=%.2f Y=%.2f deleted" % (xNew[0],yNew[0]))
                    d.set("regions command {point %.3f %.3f # point=x color=red width=2}" % (xNew[0],yNew[0]))
                    xNew = []
                    yNew = []
                print("  ** No stars left to delete!")
            else:
                xTest = xCurs
                yTest = yCurs
                iNear = findNearXY(xNew,yNew,xTest,yTest,20)
                if iNear < 0:
                    print("  ** No star near the cursor to delete.")
                else:
                    print("  Star at X=%.2f Y=%.2f deleted" % (xNew[iNear],yNew[iNear]))
                    d.set("regions command {point %.3f %.3f # point=x color=red width=2}" % (xNew[iNear],yNew[iNear]))
                    xNew.pop(iNear)
                    yNew.pop(iNear)

        elif cursKey == keyRadius: # change the star search radius
            promptStr = "  Change the star search radius [%.1f]: " % (cenRad)
            foo = unbuffered_input(promptStr)
            if len(foo)==0:
                print("  Keeping search radius %.1f pixels" % (cenRad))
            else:
                try:
                    newRad = float(foo)
                    if newRad > 0.0:
                        cenRad = newRad
                        print("  New search radius is %.1f pixels" % (cenRad))
                    else:
                        print("  Search radius must be positive and non-zero - no change")
                except:
                    print("  You must enter a number - no change")

        elif cursKey == keyHelp:
            print("\nAlignment Star Editing Commands:")
            print(f"  '{keyMarkAuto}' = add the star nearest the cursor w/centroid computation")
            print(f"  '{keyMarkExact}' = add a star at the current cursor XY position (no centroid)")
            print(f"  '{keyMarkDelete}' = delete the star nearest the cursor")
            print(f"  '{keyRadius}' = change the star search radius [currently %.1f pixels]" % (cenRad))
            print(f"  '{keySaveQuit}' = quit editing and save changes")
            print(f"  '{keyAbort}' = abort editing, discard all changes, and continue")
            print(f"  '{keyHelp}' = shows a list of all cursor commands\n")

    # Bottom of the cursor loop

    return xNew,yNew

#---------------------------------------------------------------------------
#
# markSlit() - interactively mark a slit destination position on a ds9 image
#

def markSlit(d,verbose=False):
    print("\nMark the desired target position along the slit and press 'q' when done.")
    print("Commands:")
    print(f"  '{keyMarkExact}' mark the desired target position along the slit")
    print(f"  '{keySaveQuit}' quit slit marking and continue")
    print(f"  '{keyAbort}' abort slit marking and exit modsAlign now")
    print(f"  '{keyHelp}' list interactive cursor commands\n")

    xPick = []  # says "nothing picked"
    yPick = []
    keepGoing = True
    while keepGoing:
        retStr = d.get('iexam key coordinate')
        bits = [s.strip() for s in retStr.split()]
        if len(bits)<3:
            print(bits)
            cursKey = ''
        else:
            cursKey = bits[0].lower()
            xCurs = float(bits[1])
            yCurs = float(bits[2])

        if cursKey == keySaveQuit:
            print("\nSlit position marking done.\n")
            keepGoing = False

        elif cursKey == keyAbort:
            print("\nAborting slit position marking and exiting modsAlign")
            sys.exit(1)

        elif cursKey == keyMarkExact:
            xPick.append(xCurs)
            yPick.append(yCurs)
            print("  Marked position X=%.2f Y=%.2f" % (xPick[-1],yPick[-1]))
            d.set("regions command {point %.3f %.3f # point=cross color=green}" % (xPick[-1],yPick[-1]))

        elif cursKey == keyHelp:
            print("\nSlit Marking Cursor Commands:")
            print(f"  '{keyMarkExact}' mark the desired target position along the slit")
            print(f"  '{keySaveQuit}' quit slit marking and continue")
            print(f"  '{keyAbort}' abort slit marking and exit modsAlign now")
            print(f"  '{keyHelp}' list interactive cursor commands")

    # Bottom of the cursor loop

    return xPick,yPick

#---------------------------------------------------------------------------
#
# selectTarget() - interactively select the long-slit target on a ds9 image
#
# The default centroid radius of 15 pixels, about what imexam allows.
#

def selectTarget(image,d,verbose=False,cenRad=15):
    print("\nSelect the target to move onto the slit and press 'q' when done.")
    print("Commands:")
    print(f"  '{keyMarkAuto}' measure the centroid of the object nearest the cursor")
    print(f"  '{keyMarkExact}' use the current cursor XY position as the target center")
    print(f"      (best for targets too faint to centroid)")
    print(f"  '{keyRadius}' change the target search radius [currently: %.1f pixels]" % (cenRad))
    print(f"  '{keySaveQuit}' quit selection and save choices.")
    print(f"  '{keyAbort}' abort selection and exit modsAlign")
    print(f"  '{keyHelp}' list interactive cursor commands")
    print("The *last* position marked is used as the target position\n")

    xTarget = []
    yTarget = []
    keepGoing = True
    while keepGoing:
        retStr = d.get('iexam key coordinate')
        bits = [s.strip() for s in retStr.split()]
        if len(bits)<3:
            print(bits)
            cursKey = ' '
        else:
            cursKey = bits[0].lower()
            xCurs = float(bits[1])
            yCurs = float(bits[2])

        if cursKey == keySaveQuit:
            print("\nTarget selection done.")
            keepGoing = False

        elif cursKey == keyAbort:
            print("\nAborting target selection and exiting modsAlign")
            sys.exit(1)

        elif cursKey == keyMarkExact:
            xTarget.append(xCurs)
            yTarget.append(yCurs)
            print("  Cursor position: X=%.2f Y=%.2f" % (xTarget[-1],yTarget[-1]))
            d.set("regions command {point %.3f %.3f # point=cross color=green}" % (xTarget[-1],yTarget[-1]))

        elif cursKey == keyMarkAuto:
            # find the peak pixel within cenRad of the cursor, use this as the first guess
            xPeak,yPeak,maxPix = findPeak(image,xCurs,yCurs,cenRad)
            try:
                xCen,yCen,xErr,yErr = findCentroid(image,xPeak,yPeak,cenRad,verbose=verbose)
                gotStar = True
            except Exception as err:
                print("  * Object not found near %.1f,%.1f" % (xCurs,yCurs))
                print("  * Reason: %s" % (err))
                gotStar = False

            if gotStar:
                xCen += 1
                yCen += 1
                d.set("regions command {point %.3f %.3f # point=cross color=green}" % (xCen,yCen))
                d.set("regions command {circle %.3f %.3f %.3f # color=green}" % (xCen,yCen,cenRad/2.0))
                print("  Object centroid: X=%.2f+/-%.2f Y=%.2f+/-%.2f" % (xCen,xErr,yCen,yErr))
                xTarget.append(xCen)
                yTarget.append(yCen)

        elif cursKey == keyRadius: # change the target search radius
            promptStr = "  Change the target search radius [%.1f]: " % (cenRad)
            foo = unbuffered_input(promptStr)
            if len(foo)==0:
                print("  Keeping search radius %.1f pixels" % (cenRad))
            else:
                try:
                    newRad = float(foo)
                    if newRad > 0.0:
                        cenRad = newRad
                        print("  New search radius is %.1f pixels" % (cenRad))
                    else:
                        print("  Search radius must be positive and non-zero - no change")
                except:
                    print("  You must enter a number - no change")

        elif cursKey == keyHelp: # help
            print("\nTarget Selection Commands:")
            print(f"  '{keyMarkAuto}' measure the centroid of the object nearest the cursor")
            print(f"  '{keyMarkExact}' use the current cursor XY position as the target center")
            print(f"      (best for targets too faint to centroid)")
            print(f"  '{keyRadius}' change the target search radius [currently %.1f pixels]" % (cenRad))
            print(f"  '{keySaveQuit}' quit selection and save choices")
            print(f"  '{keyAbort}' abort selection and exit modsAlign now")
            print(f"  '{keyHelp}' list interactive cursor commands")
            print("The *last* position marked is used as the target position.")

    # Bottom of the cursor loop

    return xTarget,yTarget

#---------------------------------------------------------------------------
#
# centerBoxes() - return alignment box centers using the SMS bisector method
#

def centerBoxes(image,xBox,yBox,boxWidth,wfac=1.5,clip=True,verbose=False):
    xAlign = []
    yAlign = []
    for i in range(len(xBox)):
        xPix = xBox[i]
        yPix = yBox[i]
        try:
            xCen,yCen,boxMed = smsBisector(image,xPix,yPix,boxWidth,boxWidth,wfac=1.5,clipStars=True)
            xAlign.append(xCen+1)
            yAlign.append(yCen+1)
            if verbose:
                print("  Alignment box %2d SMS bisector center: X=%.2f Y=%.2f" % (i+1,xAlign[-1],yAlign[-1]))

        except Exception as err:
            print("  * Cannot compute SMS bisector for box %d" % (i+1))
            print("  * Reason: %s" % (err))

    return xAlign,yAlign

#---------------------------------------------------------------------------
#
# getSlitPos() - interactively measure the slit position on a ds9 image
#
# Measure slit offset target positions for long-slit acquisitions.  If
# useRefPos is True, we use the reference center if defined, otherwise
# require marking.
#
# We parse the long-slit maskID if it follows the standard
# convention:
#    1 slit:  LS<length>X<width>
#    N slits: LS<num>X<length>X<width>
# If it does not follow the naming convention, we use placeholder
# values.
#
# Added transform = True/False option to allow mapping the expected
# central slit position using the mm-to-pix transforms loaded by
# modsDistCoeffs()
#
# The initial guess of the slit position can be:
#   1) a cursor XY position from markSlit (-l option)
#   2) an estimated XY position from a y offset in arcsec (-y option)
#   3) a preset wide-slit XY position (-r option)
#
# For options 1 and 2, the initial guess is refined using a 1D
# Gaussian Marginal Sum (GMS) centroid in a relatively wide window
# (3*slit_width) to make the initial guesses forgiving.  The 1D GMS
# estimate breaks down with the very wide slit for the -r option.
#
# If the slit is 0.6" or narrower, the 1D GMS centroid is taken as the
# slit center, as the edge-detection algorithm breaks down for very
# narrow slits (5 pixels or less wide).
#
# For wider slits, we use a 1D Sobel Marginal Sum (SMS) bisector
# algorith to refine the slit center at the given Y location, using
# the 1D GMS centroid as input.  The 1D SMS bisector is computed in a
# narrower window than used by the 1D GMS centroid which gives better
# precision.
#
# Note there is a +1 offset in the xCen coordinate computed internally
# and the xy coordinates used by ds9 to display the image.  Because
# people will be visually confirming the positions with the ds9
# display, we pass this +1 offset along.  This is because python uses
# 0-relative array indexing, but ds9 uses 1-relative indexing.
#

def getSlitPos(d, inst, mask, useXYRef, useOffset, yOffset, swFac=3, transform=True):
    naxis1 = mask[imExt].header['NAXIS1'] # dimensions of the mask image
    naxis2 = mask[imExt].header['NAXIS2']
    xBin = mask[imExt].header['CCDXBIN']  # binning factor of the mask image
    yBin = mask[imExt].header['CCDYBIN']
    if xBin <= 0:
        xBin = 1
    if yBin <= 0:
        yBin = 1
    maskName = mask[imExt].header['MASKNAME'] # name of the slit mask
    hasRef = False

    # If MASKNAME starts with LS, this is a long-slit mask which
    # follows the MODS standard naming convention for single or
    # segmented long-slit masks.

    x0Nom = 0.0  # nominal acq slit center
    y0Nom = 0.0
    if maskName.upper().startswith("LS"):
        slitID =  maskName.upper()
        if slitID == 'LS10X0.8SNS':
            numSlits = 1
            asLen = 10.0
            asWid = 0.8
            slitWid = inst.arcsec2pix(asWid)/xBin
            slitLen = inst.arcsec2pix(asLen)/yBin
            mmWid = inst.arcsec2mm(asWid)
            mmLen = inst.arcsec2mm(asLen)
            yOffset = -120.0 # arcsec offset
            useOffset = True
            x0Nom = 0.0
            y0Nom = inst.arcsec2mm(yOffset)

        else:
            (dummy,sep,slitDims) = slitID.partition('LS')
            comps = slitDims.split('X')
            if len(comps) == 3:
                numSlits = int(comps[0]) # number of slit segments
                asLen = float(comps[1])  # slit segment length in arcsec
                asWid = float(comps[2])  # slit width in arcsec
                slitWid = inst.arcsec2pix(asWid)/xBin
                slitLen = inst.arcsec2pix(asLen)/yBin
                mmWid = inst.arcsec2mm(asWid)
                mmLen = inst.arcsec2mm(asLen)
            else:
                numSlits = 1
                asLen = float(comps[0]) # slit length in arcsec
                asWid = float(comps[1]) # slit width in arcsec
                slitWid = inst.arcsec2pix(asWid)/xBin
                slitLen = inst.arcsec2pix(asLen)/yBin
                mmWid = inst.arcsec2mm(asWid)
                mmLen = inst.arcsec2mm(asLen)

        # The facility wide slit for flux calibration stars uses a
        # preset x,y center if -r is used.  Note that unlike earlier
        # versions, this reference center is a guess rather than taken
        # as given, and refined with the SMS bisector

        if slitID == "LS60X5":
            hasRef = True
            xRef = float(inst.xPhotCal)
            yRef = float(inst.yPhotCal)

    # If the mask name begins with CS, this is a "Custom Slit"
    # selected in various ways (e.g., from a MOS slit set)

    elif maskName.upper().startswith('CS'):
        numSlits = 1
        comps = maskName.split('_')
        x0Nom = float(comps[1])
        y0Nom = float(comps[2])
        mmWid = float(comps[3])
        asWid = inst.mm2arcsec(mmWid)
        mmLen = float(comps[4])
        asLen = inst.mm2arcsec(mmLen)
        slitWid = inst.arcsec2pix(asWid)/xBin
        slitLen = inst.arcsec2pix(asLen)/yBin
        regSlit = slitRegion(inst,mmWid,mmLen,x0=x0Nom,y0=y0Nom,dx=0.0,xBin=xBin,yBin=yBin)
        d.set(regSlit)

    # If the mask name begins with ID, it's a multi-object mask!
    # Complain and exit

    elif maskName.upper().startswith("ID"):
        print("** ERROR: Slit mask %s is a multi-object mask!" % (maskName))
        print("          You must perform a MOS mask alignment using an MMS file")
        print("   modsAlign aborting with errors\n")
        log.error(f"Slit mask {maskName} is a multi-object mask, exiting")
        sys.exit(1)

    else:
        # the "default" proxy long-slit is 60x1-arcsec (LxW)
        numSlits = 1
        asWid = 1.0
        asLen = 60.0
        slitWid = inst.arcsec2pix(asWid)/xBin
        slitLen = inst.arcsec2pix(asLen)/yBin
        mmWid = inst.arcsec2mm(asWid)
        mmLen = inst.arcsec2mm(asLen)

    # Get the pixel data

    sciData = mask[imExt].data
    slitXPos = []
    slitYPos = []
    slitFlux = []

    # If we are using a preset reference position, use it, otherwise
    # go into interactive mode and ask to mark the desired position

    if useXYRef and hasRef:
        # correct for image size relative to 1024x1024 [rwp/osu - 2026 Feb 17]
        xPix = (xRef-512.0) + 0.5*naxis1
        yPix = (yRef-512.0) + 0.5*naxis2
        if verbose:
            print("\nMeasuring the slit at preset position X=%.1f Y=%.1f pix..." % (xPix,yPix))
        try:
            xCen,yCen,boxMed = smsBisector(sciData,xPix,yPix,slitWid,swFac*slitWid,axis='x',clipStars=True,
                                           wfac=1.5,verbose=verbose)
            if verbose:
                print("  1D SMS bisector X slit center: %.2f" % (xCen+1))
            slitXPos = float(xCen+1)
            slitYPos = float(yPix)
        except Exception as err:
            print("** ERROR: Cannot compute 1D SMS bisector X slit center")
            print("          Reason: %s" % (err))
            print("   modsAlign aborting")
            log.error(f"Cannot compute 1D SMS bisector X slit center: {err!r}, exiting")
            sys.exit(1)

    elif useOffset:
        if slitID=='LS10X0.8SNS':
            print('\nMeasuring the center of the SNS slit')
        else:
            print("\nMeasuring the slit at dY=%.1f arcsec from the slit center..." % (yOffset))
        xsmm = 0.0
        ysmm = inst.arcsec2mm(yOffset)

        # transform from mm to unbinned pixels

        xPix,yPix = inst.mask2ccd(xsmm,ysmm)

        # transform if binned?

        xPix /= xBin
        yPix /= yBin

        # First guess: 1D Gaussian Marginal Sum in a relatively wide box

        yg = yPix
        try:
            xg,xerr = gmsCentroid(sciData,xPix,yPix,swFac*slitWid,swFac*slitWid,axis='x')
            if verbose:
                print("  1D GMS centroid X slit center: %.3f+/-%.3f pix" % (xg+1,xerr))

        except:
            print("  * 1D GMS centroid failed, trying the initial XY guess...")
            xg = xPix

        if verbose:
            d.set("regions command {point(%6.2f,%6.2f) # color=green point=cross}" % (xg+1,yg))
            print("  Slit offset position xy estimate: %.2f %.2f" % (xg+1,yg))

        # Pass this guess to the SMS bisector algorithm for refinement
        # if the slit width is greater than 0.6" (~5 pixels).  For
        # very narrow slits, the SMS bisector method works poorly.

        if asWid > 0.6:
            if verbose:
                print("  Slit width >0.6\", refining using the 1D SMS bisector...")
            try:
                xCen,yCen,boxMed = smsBisector(sciData,xg,yPix,slitWid,swFac*slitWid,axis='x',wfac=1.5,clipStars=True)
                if verbose:
                    print("  1D SMS bisector X slit center: %.2f" % (xCen+1))
                slitXPos = float(xCen+1)
                slitYPos= float(yPix)
            except Exception as err:
                print("** ERROR: Cannot compute 1D SMS bisector X slit center")
                print("          Reason: %s" % (err))
                print("   modsAlign aborting")
                log.error(f"Cannot compute 1D SMS bisector X slit center (2): {err!r}, exiting")
                sys.exit(1)
        else:
            if verbose:
                print("  Slit width <= 0.6\", using the 1D GMS centroid...")
            slitXPos = float(xg+1)
            slitYPos = float(yPix)

    else:
        xSlit,ySlit = markSlit(disp)
        if len(xSlit)==0:
            print("** ERROR: No slit position marked, modsAlign aborting")
            log.error(f"No slit position marked, modsAlign aborting")
            sys.exit(1)

        xPix = xSlit[-1] # use the *last* position marked
        yPix = ySlit[-1]
        nbox = 1

        # First pass: 1D Gaussian Marginal Sum in a relatively wide box

        yg = yPix
        try:
            xg,xerr = gmsCentroid(sciData,xPix,yPix,swFac*slitWid,swFac*slitWid,axis='x')
            if verbose:
                print("  1D GMS centroid X slit center: %.3f+/-%.3f pix" % (xg+1,xerr))
                d.set("regions command {point(%6.2f,%6.2f) # color=green point=cross}" % (xg+1,yg))

        except:
            print("  * 1D GMS centroid failed, using the unrefined cursor XY...")
            xg = xPix

        # Pass this guess to the SMS bisector algorithm for refinement
        # if the slit width is greater than 0.6" (~5 pixels).  For
        # very narrow slits, the SMS bisector method works poorly.

        if asWid > 0.6:
            if verbose:
                print("  Slit width > 0.6\", refining using the 1D SMS bisector...")

            try:
                xCen,yCen,boxMed = smsBisector(sciData,xg,yPix,slitWid,swFac*slitWid,axis='x',wfac=1.5,clipStars=True)
                if verbose:
                    print("  1D SMS bisector X slit center: %.2f" % (xCen+1))
                slitXPos = float(xCen)+1
                slitYPos = float(yPix)
            except Exception as err:
                print("** ERROR: Cannot compute 1D SMS bisector X slit center")
                print("          Reason: %s" % (err))
                print("   modsAlign aborting with errors")
                log.error(f"Cannot compute 1D SMS bisector X slit center (3): {err!r}")
                sys.exit(1)
        else:
            if verbose:
                print("  Slit width <= 0.6\", using the 1D GMS centroid...")
            slitXPos = float(xg+1)
            slitYPos = float(yPix)

    if transform:
        # nominal slit (0,0) center in mm to pixels

        xs0,ys0 = inst.mask2ccd(0.0,0.0)
        dySlit = slitYPos-ys0      # Y position from slit center in pixels
        dymm = inst.pix2mm(dySlit) # and in millimeters
        xsY,ysY = inst.mask2ccd([-0.5*mmWid,0.5*mmWid],[dymm,dymm])

        # apply nominal slit x offset from marked center

        if useMOSSlit:
            dx = 0.0
            dy = 0.0
        else:
            dx = slitXPos - np.mean(xsY)
            dy = 0.0

        regSlit = slitRegion(inst,mmWid,mmLen,x0=x0Nom,y0=y0Nom,dx=dx,xBin=xBin,yBin=yBin)

    else: # crude, no coord transform so will be rotated relative to true slit outline
        regSlit = "regions command {box(%6.2f,%6.2f,%.2f,%.2f) # color=cyan}" % (slitXPos, slitYPos, slitWid, slitLen)

    regCross = "regions command {point(%6.2f,%6.2f) # point=cross color=red width=2}" % (slitXPos, slitYPos)

    # overlay on the image in displayed in ds9

    d.set(regSlit)
    d.set(regCross)

    # flux at the slit position

    slitFlux = np.median(sciData[int(slitYPos)-2:int(slitYPos)+2, int(slitXPos)-2:int(slitXPos)+2], axis=None)
    if verbose:
        print("  slitXPos=%.2f slitYPos=%.2f slitFlux=%.2f" % (slitXPos,slitYPos,slitFlux))

    # all done, return data

    return slitXPos, slitYPos, slitFlux, regSlit, regCross

#---------------------------------------------------------------------------
#
# startDS9 - launch a named ds9 window
#

def startDS9(ds9ID,timeout=30):
    try:
        return DS9IgnoreTimeoutWithLogger(
            log,
            title=ds9ID,
            timeout=timeout,
            debug=verbose,
            kill_ds9_on_exit=False,
            kill_on_exit=True,
            singleton=False,
            samp_conf_use_internet=False,
            samp_conf_n_retries=100, # 100 * 0.01 = 1sec
        )
    except:
        log.error(f"failed to detect any ds9 window with title '{ds9ID}'")
        raise RuntimeError(f"failed to detect any ds9 window with title '{ds9ID}'") from None


#===========================================================================
#
# Main Program
#
#===========================================================================

# Trap Ctrl+C=SIGINT.  ds9 really loses its mind if Ctrl+C is pressed
# during cursor interaction, so trap SIGINT. No guarantees, but we try.

signal.signal(signal.SIGINT,sigHandler)

# Some defaults we need

brightAcq = False
verbose = False
debug = False
cenRad = 15      # Centroid radius search in pixels
imExt = 6        # The merged, bias-subtracted MODS image is MEF extension 6 (Archon update rwp/osu)
# doBias = False   # No longer relevant for the MODS Archon controllers [rwp/osu]
turboMode = False
reject = False
modsTO = 60      # TCS offsetPointing command timeout in seconds
longSlit = False # long-slit mode
useRefPos = False  # do not use the long-slit reference pos'n coords
useSlitPos = False # do not use ySlitPos for the long-slit position
ySlitPos = 0.0     # slit center
useMOSSlit = False # use a specific MOS slit for acquisition (default: not)

try:
    opts, files = getopt.gnu_getopt(sys.argv[1:],'iVvmqy:Btlry:d',
                                    ['interact','verbose','version',
                                     'bright','turbo','reject=','slit=',
                                     'maxiter=','sw=','mw=','info',
                                     'refpos','y=','ypos=','debug',
                                     'mos','longslit','long'])

except getopt.GetoptError as err:
    print("\n** ERROR: %s" % (err))
    printUsage()
    sys.exit(2)

if len(opts)==0 and len(files)==0:
    printUsage()
    sys.exit(1)

for opt, arg in opts:
    if opt in ('-V','--version','--info'):
        print("%s (%s)" % (verName,verDate))
        sys.exit(0)

    elif opt in ('-v','--verbose'):
        verbose = True

    elif opt in ('-d','--debug'):
        debug = True

    elif opt in ('-y','--y','--ypos'):
        try:
            ySlitPos = float(arg)
        except:
            print("** ERROR: %s requires an offset in arcseconds " % (opt))
            printUsage()
            sys.exit(1)
        useSlitPos = True
        longSlit = True
        useRefPos = False

    elif opt in ('-r','--refpos'):
        longSlit = True
        useRefPos = True

    elif opt in ('-l','--long','--longslit'):
        longSlit = True
        useRefPos = False

    elif opt in ('-m','--mos'): # convenience options
        longSlit = False

    elif opt in ('-B','--bright'):
        brightAcq = True
        # doBias = False

    elif opt in ('-t','--turbo'):
        turboMode = True

    elif opt in ('--maxiter'):
        try:
            maxIter = int(arg)
        except:
            print("** ERROR: The %s option requires a number" % (opt))
            printUsage()
            sys.exit(1)
        if maxIter <= 1:
            reject = False
            maxIter = 1
        else:
            reject = True

    elif opt in ('--reject'):
        try:
            rejSigma = float(arg)
        except:
            print("** ERROR: The %s option requires a number" % (opt))
            printUsage()
            sys.exit(1)

    elif opt in ('--sw'):
        searchFac = float(arg)
        if verbose:
            print("Star search width multiplier set to %.1f" % (searchFac))

    elif opt in ('--mw'):
        matchFac = float(arg)
        if verbose:
            print("Star/box XY matching width multiplier set to %.1f" % (matchFac))

    elif opt in ('--slit'):
        mosSlitID = arg
        useMOSSlit = True
        if verbose:
            print('Using MOS slitID %s for the acquisition slit' % (mosSlitID))

numOpts = len(opts)
numFiles = len(files)

# If no files given, print usage info and exit

if numFiles == 0:
    printUsage()
    sys.exit(0)

mods1Disp = "mods1Align"
mods2Disp = "mods2Align"

# Check the files given on the command line.  Identify first by
# extension (.fits or .mms), then if FITS sniff the header to
# distinguish mask and field images.  Also record the instrument ID
# info and make sure they are from the same MODS instrument and
# channel (can happen now that we have both MODS running).
#
# This makes it so that the order in which the files appear on the
# command line is irrelevant so long as the correct types of files are
# given.  This mitigates issues of forgetting the alignment command
# syntax late at night at high elevation...

haveMask = False
haveField = False
haveMMS = False
maskInst = 'None'
fieldInst = 'None'

for inFile in files:
    chkExt = os.path.splitext(inFile)[1]
    if chkExt == '.fits':
        pathBits = os.path.split(inFile)
        if (len(pathBits[0]) == 0):
            testFile = os.path.join(imgDir,inFile)
        else:
            testFile = inFile
    elif chkExt == '.mms':
        pathBits = os.path.split(inFile)
        if (len(pathBits[0]) == 0):
            testFile = os.path.join(mmsDir,inFile)
        else:
            testFile = inFile
        mmsRoot = inFile

    else:
        print("** ERROR: File %s has unrecognized extension %s" % (inFile,chkExt))
        print("   modsAlign aborting")
        sys.exit(1)

    # Quick validation: does testFile exist?

    if not os.path.exists(testFile):
        print("** ERROR: File %s not found" % (testFile))
        print("   modsAlign aborting")
        printUsage(short=True)
        sys.exit(1)

    # Test by file extension: FITS or MMS file, then sniff contents if FITS

    if chkExt == '.fits':
        try:
           testFITS = fits.open(testFile)
        except:
            print("** ERROR: File %s caused errors on FITS open" % (testFile))
            print("   modsAlign aborting")
            sys.exit(1)
        maskPos = testFITS[0].header['MASKPOS']
        if maskPos.upper() == 'STOW':
            if haveField and fieldFile==testFile:
                print("** ERROR: Filename %s appears twice on the command line!" % (testFile))
                print("          Check command syntax and try again.")
                print("modsAlign aborting.")
                sys.exit(1)
            haveField = True
            fieldFile = testFile
            fieldInst = testFITS[0].header['INSTRUME']
            fieldRoot = inFile
        elif maskPos.upper() == 'IN':
            if haveMask and maskFile==testFile:
                print("** ERROR: Filename %s appears twice on the command line!" % (testFile))
                print("          Check command syntax and try again.")
                print("modsAlign aborting.")
                sys.exit(1)
            haveMask = True
            maskFile = testFile
            maskInst = testFITS[0].header['INSTRUME']
            maskRoot = inFile
        else:
            print("** ERROR: Cannot determine mask position in/out")
            print("          Does %s have a corrupted FITS header?" % (testFile))
            print("   modsAlign aborting")
            sys.exit(1)
        testFITS.close()

    elif chkExt == '.mms':
        haveMMS = True
        mmsFile = testFile
        longSlit = False

# In principle we could figure out the mode (MOS, LS, or STD) by
# looking at what files we were given:
#
#   Mode      Mask?  Field?  MMS?
#   MOS         Y      Y      Y
#   Long-Slit   Y      Y      N
#   Std Acq     N      Y      N
#   Std Refine  Y      N      N
#
# For now this just a note, there are risks in having it assume too
# much, and acting incorrectly on bad inputs.

# Make sure we have what we need, abort if anything is missing

numErr = 0
missList = []

# If we were given -r (useRefPos), we need either a mask or a field
# image, but not both.  This is an immediate abort condition.  Also
# abort if neither were given.

if useRefPos:
    if haveMask and haveField:
        print("** ERROR: the -r option given with both field and a mask images")
        print("          You must provide only one type of image with -r")
        print("   modsAlign aborting")
        sys.exit(1)
    if not haveMask and not haveField:
        print("** ERROR: The -r option requires a field or a mask image.")
        print("   modsAlign aborting")
        sys.exit(1)

# In long slit, we need a mask image and a field image.
# For a calibration star with useRefPos=True, we only need a mask image.
# In MOS mode we need a mask image, a field image, and an MMS file

if longSlit:
    if not useRefPos:
        if not haveMask:
            missList.append('the mask image')
            numErr += 1

        if not haveField:
            missList.append('a field image')
            numErr += 1
else:
    if not haveField:
        missList.append('a field image')
        numErr += 1
    if not haveMMS:
        missList.append('an MMS File')
        numErr += 1

if numErr > 0:
    print("** ERROR: One or more files are missing from the command line:")
    for item in missList:
        print("            %s" % (item))
    print("   modsAlign aborting")
    printUsage(short=True)
    sys.exit(1)

# Sanity check: mask and field image better be from the same MODS channel!

if not useRefPos and (maskInst != fieldInst):
    print("** ERROR: Mask and Field images are NOT from the same MODS channel!")
    print("          Mask: %s is from %s" % (maskFile,maskInst))
    print("         Field: %s is from %s" % (fieldFile,fieldInst))
    print("   modsAlign aborting\n")
    sys.exit(1)

if verbose:
    if haveField:
        print("Field image: %s" % (fieldFile))
    if haveMask:
        print("Thru-Mask image: %s" % (maskFile))
    if longSlit:
        print("Long-slit acquisition")
    else:
        print("MOS mask acquisition using MMS file: %s" % (mmsFile))

#
# Checks complete, load the images and the mms file.
#

# 2026 Jan 18 [rwp/osu]:
#
#  The Archon controllers and azcam server create MEF FITS files
#  with the extension 6 being the reconstructed and bias-subtracted
#  single-frame image.  Extentions 1-4 are the raw quadrants with
#  overscan bias (5 is a BinTable with Archon data).
#
# The full Primary header is copied to extension 6, so we don't need
# to switch between extension 0 and 6 to get header info we need.
#
#    mask[0] -> mask[imExt]
#   field[0] -> field[imExt]
#
# See older versions for the code that used doBias and quick-bias for
# the old MODS controllers.  I've deleted it from this version as it would
# be positively harmful to run with Archon data.
#

# Read in the mask image data and sniff its header for bits we need.

if haveMask:
    try:
        mask = fits.open(maskFile)
    except:
        print("** ERROR: Cannot open the mask image FITS file %s" % (maskFile))
        print("   modsAlign aborting")
        sys.exit(1)

    maskImage = mask[imExt].data
    maskScale = medImgScale(maskImage)
    maskPos = mask[imExt].header['MASKPOS']
    maskObj = mask[imExt].header['OBJECT']
    maskName = mask[imExt].header['MASKNAME']
    naxis1 = mask[imExt].header['NAXIS1']
    naxis2 = mask[imExt].header['NAXIS2']
    instID = mask[imExt].header['INSTRUME']
    xBin = mask[imExt].header['CCDXBIN']
    yBin = mask[imExt].header['CCDYBIN']

    
# If used, read in the field image data and sniff its header for bits
# we need later.  If no mask images is given, get the common
# instrument and detector info from the field image header.

if haveField:
    try:
        field = fits.open(fieldFile)
    except:
        print("** ERROR: Cannot open the field image FITS file %s" % (fieldFile))
        print("   modsAlign aborting")
        sys.exit(1)

    fieldImage = field[imExt].data
    fieldScale = medImgScale(fieldImage)
    maskScale = fieldScale  # override maskScale with fieldScale
    fieldPos = field[imExt].header['MASKPOS']
    fieldObj = field[imExt].header['OBJECT']
    if not haveMask:
        naxis1 = field[imExt].header['NAXIS1']
        naxis2 = field[imExt].header['NAXIS2']
        instID = field[imExt].header['INSTRUME']
        xBin = field[imExt].header['CCDXBIN']
        yBin = field[imExt].header['CCDYBIN']
        maskName = field[imExt].header['MASKNAME']

    field.close() # we don't need this elsewhere
    
# Get a MODS instrument object for the images. This gives us the
# various instance- and channel-dependent scales, transforms, etc.

try:
    mods = modsInst(instID,naxis1,naxis2)
except Exception as err:
    print("** ERROR: Cannot load MODS instrument/channel parameters - %s" % (err))
    print("   modsAlign aborting")
    sys.exit(1)

# If a MOS mask, read the MMS file and load the alignment box parameters.

if not longSlit:
    if verbose:
        print("Loading MODS Mask Specification (.mms) file %s" % (mmsFile))

    xBox = []
    yBox = []
    wBox = []
    findRadius = 2*32  # 2x4" alignment box
    xmm,ymm,wmm,hmm,srot,sID = readMMS(mmsFile)
    numSlits = len(xmm)
    for i in range(numSlits):
        if wmm[i] == hmm[i]:
            xccd,yccd = mods.mask2ccd(xmm[i],ymm[i])
            xBox.append(xccd)
            yBox.append(yccd)
            wBox.append(mods.mm2pix(wmm[i]))

    numBoxes = len(xBox)
    if numBoxes != 0: 
        boxWidth = wBox[0]

    if useMOSSlit:
        foundSlit = False
        for i in range(numSlits):
            if mosSlitID.lower() == sID[i].lower():
                newSlitID = 'CS_%.3f_%.3f_%.1f_%.1f' % (xmm[i],ymm[i],wmm[i],hmm[i])
                mask[imExt].header['MASKNAME']=newSlitID
                longSlit = True
                foundSlit = True
                break;
        if not foundSlit:
            print('**ERROR: No slit named %s found in MMS file %s' % (mosSlitID,mmsFile))
            print('  modsAlign aborting with errors')
            sys.exit(1)

        if verbose:
            print('Using MOS Slit ID %s = %s' % (mosSlitID,newSlitID))


# Setup the ds9 display to use for this instrument

if mods.modsID == "MODS1":
    try:
        disp = startDS9(mods1Disp)
    except ValueError as errStr:
        print("** ERROR: Cannot connect to the %s modsAlign ds9 display" % (mods.modsID))
        print("  Reason: %s" % (errStr))
        print("modsAlign aborting with errors on startup.")
        sys.exit(1)

else:
    try:
        disp = startDS9(mods2Disp)
    except ValueError as errStr:
        print("** ERROR: Cannot connect to the %s modsAlign ds9 display" % (mods.modsID))
        print("  Reason: %s" % (errStr))
        print("modsAlign aborting with errors on startup.")
        log.error(f"Cannot connect to the {mods.modsID} modsAlign ds9 display")
        sys.exit(1)

# Clear out any previous stuff in the ds9 window from the last time

disp.set("raise")
disp.set("frame clear all")
disp.set("frame delete all")
disp.set("regions delete all")
disp.set("view image no")        # redundant pixel readout
disp.set("view colorbar no")     # don't need color bar
disp.set("mode none")            # disable rude modes ds9 now has on startup
disp.set("width 800")
disp.set("height 800")

# Display the mask image in the ds9 display, frame 1

# 2017-03-27: is the XPA$ERROR invalid command name "" fault because
# of a timing race condition on LBTO machines after all the ds9
# configuration set() calls above?  Let's find out...

time.sleep(0.5)
try:
    disp.set("frame 1")
except:
    time.sleep(0.5)
    disp.set("frame 1")

# we now resume our normal program...

if haveMask:
    print("\n%s thru-mask image %s" % (instID,maskRoot))
    disp.set(f"file {maskFile}[{imExt}]")
    # if we have a field image, use its min/max scale [rwp/osu 2026Feb17]
    # use min scale of 0.0 because we need to see sky in boxes, and we are now bias subtracting
    if haveField:
        disp.set("scale limits %.2f %.2f" % (0.0,fieldScale[1]))
    else:
        disp.set("scale limits %.2f %.2f" % (0.0,2.0*maskScale[1]))
else:
    print("\n%s field image %s" % (instID,fieldRoot))
    disp.set(f"file {fieldFile}[{imExt}]")
    disp.set("scale limits %.2f %.2f" % (fieldScale[0],fieldScale[1]))

disp.set("zoom to fit")

#----------------------------------------------------------------
#
# Long-Slit Mask target alignment workflow:
#
#   1) Display the mask image and identify the long-slit target
#      location (or execute one of the auto-slit options)
#   2) Display the field image with the long-slit mask overlay
#   3) Ask the user to identify the target object with the cursor and
#      compute its centroid (or take the cursor position if faint).
#   4) Compute offset (dx, dy) to move the object to the specified
#      slit position
#   5) Send offset to the telescope
#
# For standard star acquisition in the wide slit (useRefPos=T), there
# are two variations:
#   Initial Acquisition:
#     1) Define xSlit,ySlit using the preset  reference position.
#     2) Display the field image, ask user to identify the standard
#        star with the cursor and measure the star's centroid.
#     3) Compute the offset to move the standard star to the nominal
#        refernce postion.
#     4) Send the offset to the telescope
#
#   Refine Position:
#     1) Display the mask image and autofit the slit center at the
#        nominal reference Y location, plotting the refined position
#        and the nominal slit outline over the image.
#     2) Ask the user to identify the standard star with the cursor
#        and measure the star's centroid. -- future option: auto-ID star?
#     3) Compute the offset to refine the standard star position in the slit
#     4) Send the offset to the telescope
#

if longSlit:

    # Step 1: Define the slit target position

    if useRefPos and haveField:
        # use the preset reference XY position for initial acquisition
        # xSlit,ySlit = mods.photRef() - fixed for any acqimage size, not just 1024x1024 [rwp/osu]
        
        xRef,yRef = mods.photRef()
        xSlit = (xRef-512.0) + 0.5*naxis1
        ySlit = (yRef-512.0) + 0.5*naxis2
        print("\nComputing the offset to put the standard star at preset slit position")
        print("X=%.1f Y=%.1f pix." % (xSlit,ySlit))
        # display the nominal wide slit position
        slitWid = mods.arcsec2mm(5.0)
        slitLen = mods.arcsec2mm(60.0)
        regSlit = slitRegion(mods,slitWid,slitLen,xBin=xBin,yBin=yBin)
        disp.set(regSlit)

    else:
        if useRefPos:
            # measure the actual slit position for confirmation
            print("\nVerifying target centering in the confirmatory thru-slit image")
        xSlit,ySlit,slitMed,regSlit,regCross = getSlitPos(disp,mods,mask,useRefPos,useSlitPos,ySlitPos)

    print("  Desired slit center at X=%.2f and Y=%.2f pix (red cross)" % (xSlit,ySlit))

    # Step 2: Identify and measure the center of the target to move into the slit

    if useRefPos:
        # This is a confirmatory thru-slit image to refine the previous offset.
        if haveMask:
            print("\nAuto-centroiding the star nearest the slit preset position...")
            searchRad = mods.arcsec2pix(5.0)  # search around the refined reference position
            try:
                xCen,yCen,xErr,yErr = findCentroid(maskImage,xSlit,ySlit,searchRad,verbose=debug)
                gotStar = True
            except Exception as err:
                print("* ERROR: No star found near the nominal preset slit position")
                print("* Reason: %s" % (err))
                gotStar = False
            if gotStar:
                xCen += 1
                yCen += 1
                print("  Star found at X=%.2f+/-%.2f Y=%.2f+/-%.2f (green cross/circle)" % (xCen,xErr,yCen,yErr))
                disp.set("regions command {point %.3f %.3f # point=cross color=green}" % (xCen,yCen))
                disp.set("regions command {circle %.3f %.3f %.2f # color=green}" % (xCen,yCen,searchRad/4.0))
                xTarg = [xCen]
                yTarg = [yCen]

                # Give the option to accept/measure/abort. If turboMode, auto-accept.

                if turboMode:
                    ans = "a"
                else:
                    foo = unbuffered_input(
                            "\nReview the slit and star centroids shown on ds9:"
                            "\n  'a' = accept the auto-centroid measurement and continue"
                            "\n  'm' = reject the auto-centroid and measure the star manually"
                            "\n  '!' = abort modsAlign and exit the program now"
                            "\nOption: (a|m|!)?: ")
                    ans = foo.lower()
                    if ans == 'm':
                        xTarg,yTarg = selectTarget(maskImage,disp,verbose=verbose)
                    elif ans == '!':
                        print("** modsAlign aborted by user.")
                        sys.exit(1)

            else:
                # Oops! No star found, let them try by hand, can also
                # opt to abort inside selectTarget()
                print("* Try selecting the star by hand...")
                xTarg,yTarg = selectTarget(maskImage,disp,verbose=verbose)

        else:
            # We need to identify the target in the field image
            xTarg,yTarg = selectTarget(fieldImage,disp,verbose=verbose)

        if len(xTarg)==0:
            print("** ERROR: no target selected - modsAlign aborting")
            sys.exit(1)

    # Regular long-slit acquisition

    else:
        disp.set("frame 2")
        print("\n%s field image %s" % (instID,fieldRoot))
        disp.set(f"file {fieldFile}[{imExt}]")
        disp.set("zoom to fit")
        disp.set("scale limits %.2f %.2f" % (fieldScale[0],fieldScale[1]))
        disp.set(regSlit)
        disp.set(regCross)
        xTarg,yTarg = selectTarget(fieldImage,disp,verbose=verbose)
        if len(xTarg)==0:
            print("** ERROR: no target selected - modsAlign aborting")
            sys.exit(1)

    # Compute the offset in pixels and arcseconds

    xStar = xTarg[-1] # use the last object marked as the target
    yStar = yTarg[-1]
    dx = xSlit-xStar
    dy = ySlit-yStar
    dXoff = mods.pix2arcsec(dx*xBin)  # includes pixel binning
    dYoff = mods.pix2arcsec(dy*yBin)

#----------------------------------------------------------------
#
# MOS Mask alignment workflow:
#
#   1) Display the mask image and identify the alignment boxes
#      using the mms file.  Centroid the boxes automatically
#      and allow the user to make changes as needed.
#   2) Display the field image with the centered alignment boxes
#      superimposed.
#   3) Auto-find alignment stars, show the results, and allow the
#      user to edit or do by hand.
#   4) Compute (dx, dy, dPA) tranform to align the mask with the
#      target field.
#   5) Send offset to the telescope
#

else:
    # Compute refined box centers using the 2D SMS bisector algorithm

    xAlign,yAlign = centerBoxes(maskImage,xBox,yBox,boxWidth,verbose=debug)

    # Draw the alignment boxes on the mask image

    for i in range(len(xAlign)):
        disp.set("regions command {box(%.3f,%.3f,%.3f,%.3f) # text=\"%d\" color=cyan}" % (xAlign[i],yAlign[i],wBox[i],wBox[i],i+1))

    # If the coordinate transform predictions are good enough, we can
    # skip manual box marking in turbo mode.

    if turboMode:
        ans = "a"
    else:
        foo = unbuffered_input(
                "\nReview the alignment boxes shown on ds9:"
                "\n  'a' = accept all alignment boxes shown and continue"
                "\n  'm' = reject all alignment boxes and select boxes manually"
                "\n  '!' = abort modsAlign and exit the program now"
                "\nOption: (a|m|!)?: ")
        ans = foo.lower()

    if ans == '!':
        print("\nmodsAlign aborted by user at alignment box selection.\n")
        sys.exit(1)

    elif ans == 'm':
        xPick,yPick = selectBoxes(maskImage,disp,boxWidth,verbose=debug)
        numAlign = len(xPick)
        if numAlign < 2:
            print("** ERROR: Insufficient alignment boxes marked - minimum 2 are required.")
            print("   modsAlign aborting")
            sys.exit(1)
        xBox = []
        yBox = []
        for i in range(len(xPick)):
            xBox.append(xPick[i])
            yBox.append(yPick[i])

        # Refine box centers

        xAlign,yAlign = centerBoxes(maskImage,xBox,yBox,boxWidth,verbose=debug)

        # Display the boxes with labels

        disp.set("regions delete all")
        for i in range(len(xAlign)):
            disp.set("regions command {box(%.3f,%.3f,%.3f,%.3f) # text=\"%d\" color=cyan}" % (xAlign[i],yAlign[i],boxWidth,boxWidth,i+1))

#
# Removed y/n prompt @ LBTO request - 2016-12-29 [rwp/osu]
#
#        foo = input("\nOK to proceed (Y|N)?: ")
#        ans = foo.lower()
#        if ans != 'y':
#            print("modsAlign aborting\n")
#            sys.exit(1)
#

    # We need at least 2 boxes for alignment

    if len(xAlign) < 2:
        print("** ERROR: Insufficient alignment boxes (minimum 2 required).")
        print("   modsAlign aborting\n")
        sys.exit(1)

    # Step 2: Display the field image with the refined alignment boxes overlaid

    disp.set("frame 2")
    print("\n%s field image %s" % (instID,fieldRoot))
    disp.set(f"file {fieldFile}[{imExt}]")
    disp.set("zoom to fit")
    disp.set("scale limits %.2f %.2f" % (fieldScale[0],fieldScale[1]))

    # Draw the alignment boxes on the image in the frame for reference

    for i in range(len(xAlign)):
        disp.set("regions command {box(%.3f,%.3f,%.3f,%.3f) # text=\"%d\" color=yellow}" % (xAlign[i],yAlign[i],boxWidth,boxWidth,i+1))

    # First cut - try to find stars automatically.

    x0rot,y0rot = mods.rotCen() # image rotator center

    maskXY = []
    starXY = []
    for i in range(len(xAlign)):
        xPix = xAlign[i]
        yPix = yAlign[i]
        searchRad = searchFac*boxWidth

        # First Pass: use peak pixel, then compute centroid

        if verbose:
            disp.set("regions command {box(%.3f,%.3f,%.3f,%.3f) # color=green}" % (xPix,yPix,2*searchRad,2*searchRad))
        xPeak,yPeak,maxPix = findPeak(fieldImage,xPix,yPix,searchRad)
        if debug:
            print("  star %d peak pixel: %.0f ADU at (%d,%d)" % (i+1,maxPix,xPeak+1,yPeak+1))
            disp.set("regions command {point %.3f %.3f # point=cross color=cyan}" % (xPeak+1,yPeak+1))

        try:
            xCen,yCen,xErr,yErr = findCentroid(fieldImage,xPeak,yPeak,cenRad,verbose=debug)
            gotStar = True
        except Exception as err:
            print("* No candidate alignment star found near box %i" % (i+1))
            print("* Reason: %s" % (err))
            gotStar = False

        # We found a star...

        if gotStar:
            xCen += 1
            yCen += 1
            disp.set("regions command {point %.3f %.3f # point=cross color=green}" % (xCen,yCen))
            disp.set("regions command {circle %.3f %.3f %.2f # color=green}" % (xCen,yCen,cenRad))
            starXY.append([xCen-x0rot,yCen-y0rot])
            maskXY.append([xAlign[i]-x0rot,yAlign[i]-y0rot])

    # Give the option to accept the autofind results, edit those results,
    # or throw them out and select alignment stars by hand

    if not turboMode:
        foo = unbuffered_input(
                "\nReview the alignment stars shown on ds9:"
                "\n  'a' = accept all alignment stars and continue"
                "\n  'e' = edit the alignment stars then continue"
                "\n  'm' = reject all alignment stars and select stars manually"
                "\n  '!' = abort modsAlign and exit the program now"
                "\nOption: (a|e|m|!)?: ")
        ans = foo.lower()

        if ans == '!':
            print("modsAlign aborted by user at alignment star selection.")
            sys.exit(1)

        elif ans == 'm':
            disp.set("regions delete all")
            for i in range(len(xAlign)):
                disp.set("regions command {box(%.3f,%.3f,%.3f,%.3f) # text=\"%d\" color=yellow}" % (xAlign[i],yAlign[i],boxWidth,boxWidth,i+1))

            xStar,yStar = selectStars(fieldImage,disp,verbose=verbose)
            if len(xStar) < 2:
                print("** ERROR: Fewer than 2 alignment stars marked")
                print("   modsAlign aborting")
                sys.exit(1)

            # match marked stars with the nearest alignment box

            starXY = []
            maskXY = []
            starXY,maskXY = matchLists(xStar,yStar,xAlign,yAlign,matchFac*boxWidth,verbose=verbose)
            if len(starXY) == 0:
                print("** ERROR: No matches found between stars and alignment boxes")
                print("   modsAlign aborting")
                sys.exit(1)
            # transform to rotator-center coordinates
            for i in range(len(starXY)):
                starXY[i][0] -= x0rot
                starXY[i][1] -= y0rot
                maskXY[i][0] -= x0rot
                maskXY[i][1] -= y0rot

        elif ans == 'e':
            disp.set("regions delete all")
            for i in range(len(xAlign)):
                disp.set("regions command {box(%.3f,%.3f,%.3f,%.3f) # text=\"%d\" color=yellow}" % (xAlign[i],yAlign[i],boxWidth,boxWidth,i+1))
            xStar = []
            yStar = []
            for i in range(len(starXY)):
                xStar.append(starXY[i][0]+x0rot)
                yStar.append(starXY[i][1]+y0rot)
                disp.set("regions command {point %.3f %.3f # point=cross color=green}" % (xStar[i],yStar[i]))
                disp.set("regions command {circle %.3f %.3f %.2f # color=green}" % (xStar[i],yStar[i],15))

            xNew,yNew = editStars(fieldImage,disp,xStar,yStar,verbose=verbose)
            starXY = []
            maskXY = []
            starXY,maskXY = matchLists(xNew,yNew,xAlign,yAlign,matchFac*boxWidth,verbose=verbose)
            if len(starXY) == 0:
                print("** ERROR: No matches found between stars and alignment boxes")
                print("   modsAlign aborting")
                sys.exit(1)
            # move to image center coordinates
            for i in range(len(starXY)):
                starXY[i][0] -= x0rot
                starXY[i][1] -= y0rot
                maskXY[i][0] -= x0rot
                maskXY[i][1] -= y0rot


    if verbose:
        print("Found %d stars" % (len(starXY)))

    if len(starXY) < 2:
        print("** ERROR: Found too few stars (n=%d), cannot fit" % (len(starXY)))
        print("   modsAlign aborting")
        sys.exit(1)

    if not reject:
        maxIter = 1

    wts = np.ones(len(starXY))
    for i in range(maxIter):
        objXY = []
        refXY = []
        for j in range(len(wts)):
            if wts[j] > 0:
                objXY.append(starXY[j])
                refXY.append(maskXY[j])
        dXfit,dYfit,dPAfit,c = xyRotTran(objXY,refXY)
        dXoff = mods.pix2arcsec(dXfit)
        dYoff = mods.pix2arcsec(dYfit)
        dPAoff = mods.rotSign*dPAfit
        sinth = c[0]
        costh = c[1]
        dr = []
        for i in range(len(starXY)):
            xs = starXY[i][0]
            ys = starXY[i][1]
            xStar = x0rot + (xs*costh - ys*sinth + dXfit)
            yStar = y0rot + (xs*sinth + ys*costh + dYfit)
            # calculate residuals
            dx = xStar-maskXY[i][0]-x0rot
            dy = yStar-maskXY[i][1]-y0rot
            dr.append(math.sqrt(dx*dx + dy*dy))

        # evaluate residuals - any outliers?

        numRej = 0
        if reject:
            sigdr = np.std(dr)
            mndr = np.average(dr)
            for i in range(len(dr)):
                if wts[i] > 0 and dr[i] > rejSigma*sigdr:
                    print("** Star %d appears to be a %.2f sigma outlier, rejecting" % (i+1,dr[i]/sigdr))
                    wts[i] = 0.0
                    numRej += 1

        # sanity check

        if np.sum(wts) < 2:
            print("** ERROR: Rejected too many outliers, no fit possible.")
            print("   modsAlign aborting")
            sys.exit(1)

        if numRej == 0:
            break

    # Summary of results

    sinth = c[0]
    costh = c[1]
    for i in range(len(starXY)):
        if wts[i] > 0:
            xs = starXY[i][0]
            ys = starXY[i][1]
            xStar = x0rot + (xs*costh - ys*sinth + dXfit)
            yStar = y0rot + (xs*sinth + ys*costh + dYfit)
            disp.set("regions command {point %.3f %.3f # point=cross color=red width=2}" % (xStar,yStar))

# Report the answer and prepare to send it to the telescope

if longSlit:
    print("\nComputed Slit Alignment Offset:")
    print("   dX = %.3f arcsec\n   dY = %.3f arcsec" % (dXoff,dYoff))
    offStr = "offsetxy %6.3f %6.3f rel" % (dXoff,dYoff)
else:
    print("\nComputed Mask Alignment Offset:")
    print("   dX = %.3f arcsec\n   dY = %.3f arcsec\n  dPA = %.3f deg" % (dXoff,dYoff,dPAoff))
    offStr = "offsetpointing %7.4f %6.3f %6.3f detxy rel" % (dPAoff,dXoff,dYoff)

print("\n%s Offset Command:" % (mods.modsID))
print("   %s" % (offStr))

foo = unbuffered_input("\nSend this offset to the telescope (Y|N)?: ")
ans = foo.lower()
if str.find(ans,"y")>=0:
    offCmd = "%s/%s -t %d '%s'" % (modsDir, mods.modsCmd, modsTO, offStr)

    # Create a mods_lastOffset file with offCmd as part of a pointing
    # recovery procedure

    offFile = "%s_lastOffset" % (mods.modsID)
    if os.path.isfile(offFile):
        os.remove(offFile)
    OF = open(offFile,"w")
    OF.write(offStr)
    OF.close()

    # Send the offset command...

    print("   Sending %s offset..." % (mods.modsID))
    retcode = subprocess.call(offCmd, shell=True)
    if retcode != 0:
        print("** ERROR: %s Offset command failed - modsAlign aborting" % (mods.modsID))
        # clean up
        if os.path.isfile(offFile):
            os.remove(offFile)
        sys.exit(1)

    # Update the pointing reference (sets "0,0" for subsequent absolute offsets)

    absCmd = "%s/%s updatepointing detxy" % (modsDir,mods.modsCmd)
    retcode = subprocess.call(absCmd, shell=True)
    if retcode != 0:
        print("** ERROR: %s UpdatePointing command failed - modsAlign aborting" % (mods.modsID))
    else:
        print("   Done... %s offset completed." % (mods.modsID))
else:
    print("   OK, not sending the offset to %s..." % (mods.modsID))

# All done, clean up working files

print("\nmodsAlign done.")

sys.exit(0)
