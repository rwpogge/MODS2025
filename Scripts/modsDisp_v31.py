#!/usr/bin/env python
#
# outside of LBTO use: /usr/bin/env python
#
# modsDisp.py - watch the raw data directory for new MODS images and
#               display them in independent DS9 windows
#
# Usage: modsDisp [options]
#
# Description:
#
#   Watches MODS-specific files in the watchDir directory where
#   incoming FITS images are staged for archiving every watchTime
#   seconds.  The MODS data handling agents (via the modslogger
#   script) create files with names like MODS1B.new, MODS2R.new, etc. in
#   watchDir containing the names of the most recent images written by
#   the MODS1 blue and red channels, respectively.  modsDisp watches
#   these files and looks for changes to the creation time (st_ctime
#   from os.stat).  If one of the watched files changes it is opened
#   and the name of the new file extracted.  If this file (a) exists
#   and (b) is a readable FITS file, it is sent to a named DS9 window
#   for display based on the contents of the INSTRUME header keyword.
#
#   modsDisp will also print onto its terminal screen useful info from
#   the FITS headers to allow the observer to know which images are
#   available in the watchDir staging disk.  This is convenient, for
#   example, for using the mouse to cut-and-paste the filenames into
#   other programs when they become readable in watchDir.
#
#   By only watching a small number of particular files, we don't have
#   to spend valuable time examining every file in watchDir one by
#   one, and easily hundreds of FITS files can accumulate in the
#   archive staging directory during a busy night.  This makes
#   modsDisp very responsive, independent of how full watchDir is.  It
#   also means that when the program launches it will start by
#   displaying the most recent image rather than sitting there blank,
#   unless the image listed in the .new files has already moved from
#   the staging disk into the archiver proper (which takes at most 12
#   hours from time of acquisition).  The watched files are only
#   written after the NFS copy from a MODS data server is complete,
#   so there is minimal (but not zero) risk of accidentally trying to
#   display an image that hasn't finished being copied to the staging
#   disk.  We try to trap this condition to avoid crashing either
#   modsDisp or one of the ds9 windows.
#
#   However, ultimately the performance of modsDisp is limited by the
#   NFS latencies between the local host and the archive machine that
#   hosts the physical disk with watchDir on it.  We have seen random
#   and machine-dependent delays of up to minute or more at times.
#   Finding a solution to this has been elusive, mainly because it has
#   been hard to convince LBTO network personnel that there is a
#   problem.
#
#   watchDir is usually /newdata (see default below), or it can be set
#   in one of two ways:
#     1) by defining the MODS_RAWDIR environment variable with the path
#        to watch.
#     2) on the command-line with the -d/--dir option.
#   You can change the default below at need.
#
#   The watchTime is currently set at 2.0 seconds by default, which is
#   a reasonable cadence given the relatively slow pace that images
#   arrive on the staging disk, and will give only a small time lag
#   between close arrivals of red or blue images when observing with
#   MODS in binocular mode.  This watch time may be changed at
#   runtime by using the -t/--time command-line option.  You can also
#   change the default below at need.
#
#   If the --debug option is given, additional diagnostic output is
#   produced.  This is mainly useful for LBTO support personnel or
#   members of the MODS Team.  In particular, it calculates the
#   delay between the timestamp on the watch file and when its ctime
#   was seen to change.
#
# Notes:
#   Because watchDir is usually an NFS-mounted disk, we cannot use the
#   inotify facility of the Linux kernel.  This is because disk
#   information is specific to the kernel of the disk's physical host,
#   and that info isn't shared via NFS (at least not yet, anyway).
#
#   This program uses astropy.io.fits to extract information from the image
#   headers and astropy-samp-ds9 to display the images and set the display scale,
#   color maps, zoom, etc.  If you are adopting this program, you need
#   to make sure astropy-samp-ds9 and astropy.io.fits are installed on your system, or if
#   a personal installation that they are in PYTHONPATH.
#
#   This program was inspired by RB_Science written for the LBCs, but
#   MODS is fundamentally different and some special requirements, so
#   we have only adopted the python-book dircache trick from
#   RB_Science and built a very different display tool from there.
#
# Dependencies:
#   astropy.io fits (now comes with Anaconda)
#   astropy-samp-ds9 (https://github.com/mdcb/astropy-samp-ds9)
#
# Author:
#   R. Pogge, OSU Astronomy Dept
#   pogge.1@osu.edu
#   2011 May 27
#
# Modification History:
#   2011 May 28 - made exit with Ctrl+C more graceful [rwp/osu]
#   2011 Jul 10 - added smart ds9 window launch [rwp/osu]
#   2012 Oct 11 - added getopt command-line processing, put in smarter
#                 try/except traps for fault conditions, and better
#                 clean-up on aborts [rwp/osu]
#   2014 Apr 22 - removed deprecated dircache and instead use a more
#                 direct watchList.  Also now computing time delay
#                 between detection and ctime update. [rwp/osu]
#   2018 Jul 20 - Merged MODS1+2 version (one program to rule them all) and
#                 fully migrated to astropy.io for FITS I/O and with new hooks
#                 for pyds9 import (future-proofing) [rwp/osu]
#   2024 Dec 24 - add a new --exp option to change the default scaling & color [opk/lbto]
#                 Updated to astropy-samp-ds9 [mdcb/lbto]
#
# -------------------
#   2026 Jan 18 - New version (3.1) for the Archon CCD controllers [rwp/osu]
#
#---------------------------------------------------------------------------

import time
import getopt
import signal
import os, sys
import threading
from astropy.io import fits
from datetime import datetime
from lbto.sciops.misc import logger, slack, beep, DS9IgnoreTimeoutWithLogger

# Version number and date, update as needed

versNum  = '3.1.0'
versDate = '2026-01-18'

log = logger(f"modsDisp-{os.environ.get('USER','anon')}")

# List of MODS instruments/channels to watch

modsList = ['MODS1R','MODS1B','MODS2R','MODS2B']
watchList = [f"{m}.new" for m in modsList]

# The default watch directory, the default watch time in seconds

watchDir = os.getenv('MODS_ARCHDIR','/lbt/data/new')
watchTime = 2.0

# The Archon controllers and azcam create multi-extension FITS
# files.  The merged and bias-subtracted image is in extension 6

imExt = 6

# The default "exp" display mode

dispConfigs = {
    'default' : {
        'ds9args': '-colorbar no',
        'presets': {
            'init': ['width 800', 'height 800', 'frame clear all'],
            'frame': ['scale mode zscale', 'scale linear', 'cmap grey', 'zoom to fit'],
        },
    },
    'exp' : {
        'ds9args': '',
        'presets': {
            'init': ['width 800', 'height 800', 'frame clear all'],
            'frame': ['scale mode minmax', 'scale log', 'cmap b', 'zoom to fit'],
        },
    },
}

dispMode = 'default'

# Empty dictionary for the ds9 display instances

modsDisp = {}

# Default debug state is off, can be enabled with the
# --debug flag.  superDebug must be altered in code.

debug = False
superDebug = False

# Silence nuisance FITS warnings

import warnings
warnings.filterwarnings('ignore',category=UserWarning, append=True)
warnings.filterwarnings('ignore',category=RuntimeWarning, append=True)
warnings.filterwarnings('ignore',category=FutureWarning, append=True)
from astropy.utils.exceptions import AstropyWarning
warnings.filterwarnings('ignore',category=AstropyWarning, append=True)

#----------------------------------------------------------------
#
# printUsage() - Print the program command usage message
#

def printUsage():
    print(f'\nUsage: modsDisp [options]\n')
    print(f'Start the MODS raw-image auto-display utility.')
    print(f'\nOptions:')
    print(f'  -d/--dir PATH     watch for files in dirPath')
    print(f'                      [default: {watchDir}]')
    print(f'  -t/--time NUMBER  check the raw data directory every NUMBER seconds')
    print(f'                      [default: {watchTime} seconds]')
    print(f'  -h/--help         show help information (this message) and exit.')
    print(f'  -V/--version      show modsDisp version and exit.')
    print(f'  --exp             adjust display settings for "exp" mode.')
    print(f'  --mods1           only watch for MODS1 images.')
    print(f'  --mods2           only watch for MODS2 images.')
    print(f'  --debug           show verbose debugging info.\n')
    print(f'Notes:')
    print(f'  The default raw-data directory path may be defined using the')
    print(f'  MODS_RAWDIR environment variable.\n')

# if any ds9 window is killed, close the others and terminate the program

exit_lock = threading.RLock()

def close_all_ds9(main_thread=True):
    with exit_lock:
        for mods in modsList:
            if not modsDisp[mods].alive(): continue
            try: modsDisp[mods].exit(use_callback=False, main_thread=main_thread)
            except: pass
        alives = list(filter(lambda d: modsDisp[d].alive(), modsList))
        if not alives:
            print('modsDisp ,,, ', end='', flush=True)
            os.kill(os.getpid(), signal.SIGTERM) # a sad way to leave

# a procedure that starts a DS9 window

def start_ds9(title, timeout):
    kwdsargs = {
        'title': title,
        'timeout': timeout,
        'debug': superDebug,
        'exit_callback': lambda: close_all_ds9(main_thread=False),
        'kill_on_exit': False,
        'ds9args': dispConfigs[dispMode]['ds9args'],
        'samp_conf_use_internet': False,
        'samp_conf_n_retries': 100, # 100 * 0.01 = 1sec
    }
    try:
        return DS9IgnoreTimeoutWithLogger(log, **kwdsargs)
    except:
        raise RuntimeError(f"failed to detect any ds9 window with title '{title}'") from None

#----------------------------------------------------------------
#
# watcherLoop() - The directory watcher loop run by main
#

def watcherLoop():
    # initialize the ctime monitor dictionary
    ctimes = {}
    for watchFile in watchList:
        ctimes[watchFile] = 1

    # Look in the watchDir, if it exists...
    
    if not os.path.exists(os.path.abspath(watchDir)):
        if debug: print(f"watchDir {watchDir} does not exist.")
        time.sleep(watchTime)
        return

    while True:
        time.sleep(watchTime)  # sleep watchTime seconds
        newFile = False
        # Look at each watchFile in watchList
        for watchFile in watchList:
            fileName = os.path.join(watchDir,watchFile)
            if os.path.exists(fileName):
                new_ctime = os.stat(fileName).st_ctime
                if ctimes[watchFile] != new_ctime:
                    ctimes[watchFile] = new_ctime  # watchFile has changed
                    ctime = datetime.fromtimestamp(new_ctime).strftime('%H:%M:%S')
                    timeNow = time.time()
                    detTime = datetime.fromtimestamp(timeNow).strftime('%H:%M:%S')
                    delay = timeNow - new_ctime
                    newFile = True
                else:
                    newFile = False

                if newFile:
                    file = open(fileName,'r')
                    rawFile = file.readline().strip()
                    fitsFile = os.path.join(watchDir,rawFile)
                    file.close()
                    if os.path.exists(fitsFile):
                        # We have a FITS file to read, open the header and extract info we need
                        try:
                            hdr = fits.getheader(fitsFile,ext=imExt)
                        except Exception as errStr:
                            print(f'*** ERROR: Cannot read FITS header of {rawFile}')
                            print(f'           Reason: {errStr!r}')
                            log.error(f"getheader {fitsFile} exception: {errStr!r}")
                        else:
                            imgType = hdr['imagetyp']
                            objName = hdr['object']
                            instID = hdr['instrume']
                            chanID = hdr['channel']
                            filtName = hdr['filtname']
                            maskName = hdr['maskname']
                            maskPos = hdr['maskpos']

                            # If debugging, print the computed delay between when the watchfile was
                            # created and when we picked it up. This was added to help demonstrate a
                            # problem with the LBTO mountain NFS having random and sometimes
                            # substantial latencies.

                            if debug:
                                print(f' {instID}> {rawFile} {imgType} {objName} {filtName} {maskName} {maskPos} [delay: {delay:.1f} sec]')
                            else:
                                print(f' {instID}> {rawFile} {imgType} {objName} {filtName} {maskName} {maskPos}')

                            # Try to display it using the file command
                            
                            if instID in modsList:
                                md = modsDisp[instID]
                                try:
                                    # md.set(f"file {fitsFile}")
                                    md.set(f"file {fitsFile}[{imExt}]") # is this the correct syntax (like the ds9 command line)? [rwp/osu]
                                    # md.set(f"fits {fitsFile}[{imExt}]") # alt is use "fits" instead of "file" (also like ds9 command line) [rwp/osu]
                                except Exception as e:
                                    print(f'*** ERROR: Cannot display image {rawFile}')
                                    print(f'           Reason: {e!r}')
                                    print(f'*** modsDisp must abort - please restart ***')
                                    log.error(f'displaying {fitsFile} Exception: {e!r}')
                                    # abort by raising a KeyboardInterrupt exception to break watchLoop() & clean up.
                                    raise KeyboardInterrupt
                                else:
                                    #XXX this could also raise ...
                                    md.set(*dispConfigs[dispMode]['presets']['frame'])

                            else:
                                print(f'** {rawFile} FITS header has unrecognized INSTRUME keyword {instID}')
                else:
                    if superDebug:
                        print(f'No modification of {watchFile} seen...')


#----------------------------------------------------------------
#
# main program
#

if __name__ == '__main__':
    # Parse the command-line arguments using GNU-style getopt
    try:
        opts, files = getopt.gnu_getopt(sys.argv[1:],'Vd:t:h',
                                        ['version','dir=','time=','help','debug','exp', 'mods1','mods2'])
    except getopt.GetoptError as err:
        print(f'\n** ERROR: {err!r}')
        printUsage()
        sys.exit(2)

    for opt, arg in opts:
        if opt in ('-V','--version'):
            print(f'modsDisp.py v{versNum} [{versDate}]')
            sys.exit(0)
        elif opt in ('-d','--dir'):
            watchDir = arg
        elif opt in ('-t','--time'):
            watchTime = float(arg)
        elif opt in ('-h','--help'):
            printUsage()
            sys.exit(0)
        elif opt in ('--debug'):
            debug = True
        elif opt in ('--exp'):
            dispMode = 'exp'
        elif opt in ('--mods1'):
            modsList = ['MODS1R','MODS1B']
            watchList = ['MODS1R.new', 'MODS1B.new']
        elif opt in ('--mods2'):
            modsList = ['MODS2R','MODS2B']
            watchList = ['MODS2R.new', 'MODS2B.new']

    # All done, do it..

    if debug:
        print(os.path.abspath(watchDir))

    print('\nStarting the modsDisp session...')

    # Launch four DS9 windows, one for both channels of both MODS.
    # We run the windows with no pipes or ports to prevent IRAF from
    # hijacking them.

    # Instantiate ds9 objects for all displays we need.  We store these in
    # a list

    for mods in modsList:
        print(f'  Launching {mods} ds9 display, please wait...')
        md = modsDisp[mods] = start_ds9(mods, timeout=30)

        # Configure the window geometry and base features for the best way
        # to display MODS images

        print(f'  Configuring the {mods} ds9 display...')
        md.set(*dispConfigs[dispMode]['presets']['init'])

    print('Done.')

    # Start the watcher loop, and gracefully exit on Ctrl+C.  Note
    # that this does not close the ds9 windows...

    print(f'\nWatching for raw MODS images in {watchDir} every {watchTime} s')

    while True:
        try:
            watcherLoop()
        except KeyboardInterrupt:
            print(f'\nmodsDisp stop requested, cleaning up...')
            log.error("modsDisp stop requested (KeyboardInterrupt)")
            break
        except Exception as e:
            print(f'\nmodsDisp Exception {e!r}, cleaning up...')
            log.error("modsDisp crashed: {e!r}")
            slack('modsDisp', f'modsDisp crashed: {e!r}')
            break

    close_all_ds9()
