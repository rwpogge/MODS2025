'''
Defines the MODS class for azcam

Initial version by Mike Lesser (UA ITL)
Later versions by Rick Pogge (OSU Astronomy)

Updated: 2026 Jan 23 [rwp/osu]

Additions:
    expose(): take an exposure (async)
    expwait(): take an exposure and wait until done (blocking)
    shopen(): open the shutter
    shclose(): close the shutter
    set/get_exptime(): set/get the exposure time
    ccdTemps(): read the CCD detector temperatures read by Archon
    archonTemp(): read the Archon backplane temperature
    archonStatus(): read select Archon status info
    abortReadout(): abort readout
    pctRead(): return percentage of CCD readout (inverse of pixels_remaining)
    set/get_roi(): set/get the CCD readout region of interest and binning factor
    set_roiByName(): set the CCD readout region of interest by name
    set/get_ccdbin(): set CCD binning in x and y
    reset_roi() restore full-frame unbinned readout (alias: roi_off())
    reset_ccdbin() restore 1x1 binning, leave ROI unchanged
    set/get_filename(): set/get the next file to be written
    get_nextfile(): get the name of the next file to be written (alias for get_filename)
    get_lastfile(): get the name of the last file written
    set/get_path(): data path, set_ checks for validity and access
    set/get_expnum(): set/get number of next image to be written
    set/get_keyword(): set/get a header keyword, ensure proper sytnax
    set/get_imageInfo(): set/get IMAGETYP and OBJECT for the next image
    set_istatus(): process instrument ISTATUS info into the instrument FITS header database
    obsDate(): return the observing date CCYYMMDD noon-to-noon local time
    modsFilename(): split a filename string into dataPath, rootName, and expNum
'''

import datetime
import pytz

import os
import re
import glob

import typing

import azcam

#============================================================

class MODS(object):
    '''
    Class definition of LBT MODS.
    
    These methods are called remotely through the command server with syntax such as:
     * mods.expose 1.0 "object" "some image title"
    
    Many additions over the basic methods provided by Mike Lesser's original
    draft version to provide features for the MODS spectrographs at LBT
    without having to hack the main azcam module.
    
    Methods
    -------
    list of methods...
        
    Data Members
    ------------
    modsID (str):
        MODS channel being operated, e.g., MODS1R (MODS1 red channel CCD)
    lbtSide (str):
        Side of the LBT with this MODS instrument (left or right)
    image_types (list): 
        list of allowed image types (object, bias, dark, etc.)
    shutter_dict (dict): 
        dictionary of shutter states (1=open/light, 0=closed/dark) corresponding to image_types
    
    R. Pogge, OSU Astronomy Dept.
    pogge.1@osu.edu
    '''

    def __init__(self,modsID,lbtSide):
        '''
        Instantiate a mods tool object

        Returns
        -------
        None.

        '''

        # register "mods" as an azcam tool
        
        azcam.db.mods = self
        azcam.db.tools["mods"] = self
        azcam.db.cli["mods"] = self

        # MODS instrument channel and LBT telescope side
        
        self.modsID = modsID
        self.lbtSide = lbtSide
        
        # The TCS data dictionary (DD) uses L_ and R_ for left and right
        
        self.ddSide = self.lbtSide.upper()[0]  # TCS DD uses L or R
        
        # expand MODS channel ID, B->Blue, R->Red
        
        modsChannels = {"B":"Blue","R":"Red"}
        self.modsChan = modsChannels[self.modsID.upper()[-1]]  # Blue or Red
        
        # MODS allowed image_types.  These are stored in the FITS headers
        # as the IMAGETYP keyword values expected by the LBTO data archive
        # and data reduction pipelines.
        
        self.image_types = ['object','bias','flat','dark','comp','std','zero']
        
        # The MODS shutter_dict tells the azcam exposure tool  the shutter 
        # should be open (1) or closed (0) for each allowed MODS IMAGETYP
        
        self.shutter_dict = {'object':1,
                             'bias':0,
                             'flat':1,
                             'dark':0,
                             'comp':1,
                             'std':1,
                             'zero':0
                             }
        
        # Overload the azcam allowed image types and shutter data in
        # the exposure tool with the MODS definitions
        
        azcam.db.tools["exposure"].image_types = self.image_types
        azcam.db.tools["exposure"].shutter_dict = self.shutter_dict
        
        return
    

    def initialize(self):
        '''
        Initialize the AzCam system

        Returns
        -------
        None.

        '''

        azcam.db.tools["exposure"].reset()

        # overload the azcam allowed image types and shutter data
        
        azcam.db.tools["exposure"].image_types = self.image_types
        azcam.db.tools["exposure"].shutter_dict = self.shutter_dict
        
        return


    def reset(self):
        '''
        Reset the azcam server

        Returns
        -------
        None.

        '''

        azcam.db.tools["exposure"].reset()

        # overload the azcam allowed image types and shutter data
        
        azcam.db.tools["exposure"].image_types = self.image_types
        azcam.db.tools["exposure"].shutter_dict = self.shutter_dict
        
        return


    def set_par(self, parameter, value):
        '''
        Set a server parameter

        Parameters
        ----------
        parameter : string
            name of the parameter to set.
        value : either float, int, or string
            value of parameter to set

        Returns
        -------
        None.

        '''

        if parameter == "remote_imageserver_host":
            azcam.db.tools["exposure"].sendimage.remote_imageserver_host = value
        elif parameter == "remote_imageserver_port":
            azcam.db.tools["exposure"].sendimage.remote_imageserver_port = int(value)
        else:
            azcam.db.parameters.set_par(parameter, value)

        return


    def geterror(self):
        '''
        Return and clear the current error status

        Returns
        -------
        list

        '''

        return ["OK", ""]


    def flush(self, cycles=1):
        '''
        Flush (erase) the CCD detector

        Parameters
        ----------
        cycles : int, optional
            Number of flush cycles to execute. The default is 1.

        Returns
        -------
        None

        '''

        azcam.db.tools["exposure"].flush(cycles)

        return f"OK CCD flushed {cycles} time(s)"


    def set_roi(self,
                first_col=-1,
                last_col=-1,
                first_row=-1,
                last_row=-1,
                col_bin=-1,
                row_bin=-1,
                roi_num=0):
        '''
        Set the CCD readout region of interest (ROI) and binning factors

        Parameters
        ----------
        first_col : int, optional
            first column to readout. The default is -1.
        last_col : int, optional
            last column to readout. The default is -1.
        first_row : int, optional
            first row to readout. The default is -1.
        last_row : int, optional
            last row to readout. The default is -1.
        col_bin : int, optional
            on-chip binning factor in columns (x). The default is -1.
        row_bin : int, optional
            on-chip binning factor in rows (y). The default is -1.
        roi_num : int, optional
            ROI number. The default is 0, the only value at present
            

        Returns
        -------
        The ROI and binning set.

        Description
        -----------
        Sets the CCD readout region of interest (ROI) and on-chip binning
        factors.
        
        -1 for any parameter means "do not change"
        
        Thus:
            mods.set_roi -1 -1 -1 -1 2 2 
        leave ROI the same but sets 2x2 binning
            mods.set_roi -1 -1 100 500 -1 -1
        leaves ROI columns the same, sets rows to 100:500, binning unchanged
        
        See also: set_ccdbin()
        
        '''

        azcam.db.tools["exposure"].set_roi(
            first_col, last_col, first_row, last_row, col_bin, row_bin
        )

        return azcam.db.tools["exposure"].get_roi()
    
    
    def set_roiByName(self,roiMode):
        '''
        Set the CCD readout region of interest (ROI) by name.

        Parameters
        ----------
        roiMode : string
            Name of the preset ROI mode.
            
        Returns
        -------
        The ROI and binning mode set.
        
        Description
        -----------
        Sets the CCD readout region of interest (ROI) by name using one
        of a set of preset ROI configurations.  These are hardcoded in
        the initializer, but could in future be set at runtime from
        the azcam server runtime config file.
        
        Presets
        -------
         * off, full, 8kx3k = full-frame, unbinned 8292x3088 pixels
         * 3kx3k, 3k = 3088x3088 pixels [2603,5690,1,3088]
         * 1kx1k, 1k = 1024x1024 pixels [3635,4658,1033,2056]
         * 4kx3k = 4096x3088 [2099,6194,1,3088]
         
        This function resets binning to 1x1 pixel, and so it must be followed 
        by set_ccdbin() if another binning mode is desired.
        
        See Also: set_roi(), get_roi(), reset_roi(), set_ccdbin()

        '''
        
        if len(roiMode)==0:
            return "ERROR set_roiByName() requires one argument (roiMode string)"
        
        if roiMode.lower() in ["off","full","8kx3k"]:
            azcam.db.tools["exposure"].roi_reset()
            azcam.db.tools["exposure"].set_roi(-1,-1,-1,-1,1,1)

        elif roiMode.lower() in ["3kx3k","3k"]:
            azcam.db.tools["exposure"].roi_reset()
            #azcam.db.tools["exposure"].set_roi(2601,5688,1,3088,1,1)
            azcam.db.tools["exposure"].set_roi(2603,5690,1,3088,1,1)

        elif roiMode.lower() in ["1kx1k","1k"]:
            azcam.db.tools["exposure"].roi_reset()
            #azcam.db.tools["exposure"].set_roi(3633,4656,1033,2056,1,1)
            azcam.db.tools["exposure"].set_roi(3635,4658,1033,2056,1,1)
            
        elif roiMode.lower() in ["4kx3k"]:
            azcam.db.tools["exposure"].roi_reset()
            #azcam.db.tools["exposure"].set_roi(2097,6192,1,3088,1,1)
            azcam.db.tools["exposure"].set_roi(2099,6194,1,3088,1,1)

        else:
            return f"ERROR set_roiByName() unrecognized ROI mode {roiMode}"
        
        return azcam.db.tools["exposure"].get_roi()
        
        
    def get_roi(self):
        '''
        Get the current CCD readout region of interest (ROI) and binning factors

        Returns
        -------
        list: ints
            6 parameters: [startCol, endCol, startRow, endRow, colBin, rowBin]
            
        See also: set_roi(), reset_roi(), get_ccdbin()
        '''    
    
        return azcam.db.tools["exposure"].get_roi()
        
    
    def reset_roi(self):
        '''
        Reset the CCD ROI and on-chip binning to full-frame, unbinned readout.

        Returns
        -------
        None.

        '''
        azcam.db.tools["exposure"].roi_reset()
        azcam.db.tools["exposure"].set_roi(-1,-1,-1,-1,1,1)

        return azcam.db.tools["exposure"].get_roi()

    # roi_off is an alias for reset_roi()
    
    roi_off = reset_roi


    def set_ccdbin(self, colbin=1, rowbin=1):
        '''
        Set the on-chip binning factors

        Parameters
        ----------
        colbin : int, optional
            columns (x) binning factor. The default is 1.
        rowbin : int, optional
            rows (y) binning factor. The default is 1.

        Returns
        -------
        None.

        See also: get_ccdbin(), reset_ccdbin(), get_roi(), reset_roi()
        '''
       
        azcam.db.tools["exposure"].set_roi(-1, -1, -1, -1, colbin, rowbin)
        
        return f"OK CCD binning {colbin} x {rowbin}"

    
    def get_ccdbin(self):
        '''
        Get the on-chip binning factors

        Returns
        -------
        list: int
            the on-chip binning factors: [colbin,rowbin]
            
        See also: set_ccdbin(), reset_ccdbin(), reset_roi(), get_roi()
        '''
       
        roi = azcam.db.tools["exposure"].get_roi()

        return roi[4:]

    
    def reset_ccdbin(self):
        '''
        Reset on-chip binning to 1x1, leaving ROI unchanged

        Returns
        -------
        None.
        
        See also: reset_roi() to reset ROI and on-chip binning

        '''
        
        azcam.db.tools["exposure"].set_roi(-1, -1, -1, -1, 1, 1)
        
        return "OK binning reset to 1 x 1"
        

    def set_exptime(self, expTime: float) -> str:
        '''
        Set the camera exposure time in seconds

        Parameters
        ----------
        expTime : float, optional
            camera exposure time in seconds. The default is 1.0 seconds

        Returns
        -------
        reply : str
            OK if no errors, error if out of range, and exception if
            the Archon controller is unreachable or not configured.

        '''

        et = float(expTime)
        if et < 0.0:
            return f"ERROR set_expTime() expTime={expTime} invalid, must be >= 0.0 seconds"

        try:        
            azcam.db.tools["exposure"].set_exposuretime(et)
        except Exception as e:
            return f"ERROR set_exptime() - {e}"

        return f"OK exptime is {et:.1f} seconds"


    def get_exptime(self):
        '''
        Get the current camera exposure time in seconds

        Returns
        -------
            float: exposure time in seconds

        '''
        return azcam.db.tools["exposure"].get_exposuretime()
        

    def expose(self, 
               exposure_time: float = -1, 
               image_type: str = "", 
               image_title: str = "") -> typing.Optional[str]:
        '''
        Take a complete exposure

        Parameters
        ----------
        exposure_time : float, optional
            Exposure time in seconds. The default is -1 (use current exptime).
        image_type : str, optional
            Image type, determines shutter open or closed. The default is "".
        image_title : str, optional
            Image title (in OBJECT keyword). The default is "".

        Returns
        -------
        reply : string
            response string from the expose()/expose1() methods.

        Description
        -----------
        AzCam `expose1()` is a non-blocking method that returns immediately
        and breaks off a thread to execute the exposure in the background.
        Clients can poll the azcam server to monitor exposure and readout
        progress using the timeLeft() and pixelsLeft() methods.
        
        expose1() is the default and preferred method for most exposures.
        
        Validation
        ----------
        Because azcam expose1() is asynchronous, we want to make sure we
        aren't already exposing or will overwrite an existing image.  The
        core azcam code fails without grace in ways that can lead to loss
        of data and requiring a hardware Archon and software azcam server restart
        to recover from.  Muy malo!

        See also: expwait()        
        '''

        # Sanity Check 1: 
        #   Only execute if no exposure is in progress *expFlag=IDLE)
        
        expFlag = azcam.db.tools["exposure"].exposure_flag
        
        if expFlag > 0:
            efd = azcam.db.exposureflags
            statStr = list(filter(lambda key: efd[key]==expFlag,efd))[0]  
            return f"ERROR Exposure already in progress (ExpStatus={statStr})"
        
        # Sanity Check 2:
        #   Make sure this exposure won't overwrite an exiting file
        #   with the next filename in sequence
        
        nextFile = self.get_filename()
        if os.path.exists(nextFile):
            return f"ERROR next filename {nextFile} would overwrite existing file, go fix and try again"
        
        # Last check: Verify that the shutter is closed (usual cause: delinquent 
        # shopen not followed by shclose)
        
        try:
            self.shclose()
        except Exception as exp:
            return f"ERROR expose() - {exp}"
        
        # strip extraneous quotes from image_title
        
        if len(image_title) > 0:
            image_title = re.sub("[\"\']","",image_title)

        # do it!
        
        try:
            reply = azcam.db.tools["exposure"].expose1(exposure_time,
                                                       image_type,
                                                       image_title
                                                       )
            return reply
        except Exception as exp:                                                       
            return f"ERROR expose() - {exp}"
    
    
    def expwait(self, 
               exposure_time: float = -1, 
               image_type: str = "", 
               image_title: str = "") -> typing.Optional[str]:
        '''
        Take a complete exposure and wait for completion

        Parameters
        ----------
        exposure_time : float, optional
            Exposure time in seconds. The default is -1 (use current exptime).
        image_type : str, optional
            Image type, determines shutter open or closed. The default is "".
        image_title : str, optional
            Image title (in OBJECT keyword). The default is "".

        Returns
        -------
        reply : string
            response string from the expose()/expose1() methods.

        Description
        -----------
        AzCam `expose()` is a blocking method that does not return
        until the exposure is complete or an error.
        
        This is not the preferred method, see expose()

        See also: expose()        
        '''

        # trust but verify: make sure shutter is closed
        
        try:
            self.shclose()
        except Exception as exp:
            return f"ERROR expose() - {exp}"
        
        # strip extraneous quotes from the title
        
        if len(image_title) > 0:
            image_title = re.sub("[\"\']","",image_title)

        # do it!
        
        try:
            reply = azcam.db.tools["exposure"].expose(exposure_time,
                                                      image_type,
                                                      image_title
                                                      )
            return reply
        except Exception as e:                                                       
            return f"ERROR expwait() - {e}"
    

    def expstatus(self):
        '''
        Query the current exposure status

        Returns
        -------
        reply: str
            string with the numerical exposure flag and its meaning

        Description
        -----------

        Poll the server for the current exposure status, returning
        numerical code (0..13) and a status string.
            
        The current exposure flag in code is in `azcam.db.tools["exposure"].exposure_flag`
        and the dictionary of string translations is `azcam.db.tools["exposure"].exposureflags`.
        
        Table:
           * '0 NONE' (aka IDLE)
           * '1 EXPOSING'
           * '2 ABORT'
           * '3 PAUSE'
           * '4 RESUME'
           * '5 READ'
           * '6 PAUSED' (unused)
           * '7 READOUT'
           * '8 SETUP'
           * '9 WRITING'

        See also: timeleft(), pixelsLeft()
        '''
        
        expFlag = azcam.db.tools["exposure"].exposure_flag

        # translate the integer code into a meaningful string
        
        efd = azcam.db.exposureflags
        statStr = list(filter(lambda key: efd[key]==expFlag,efd))[0]
        
        if expFlag == 0: statStr = "IDLE"  # change to our term of art
        
        # try to push to the TCS DD, no penalty if it fails
        
        #ccdState = {f"{self.ddSide}_MODS{self.modsChan}CCDState":statStr}
        #try:
        #    azcam.db.tools["telescope"].set_parameter(ccdState)
        #except:
        #    pass
               
        return f"{expFlag} {statStr}"

    
    def timeleft(self) -> float:
        '''
        Poll the remaining exposure time in seconds.

        Returns
        -------
        timeLeft: int
            Time remaining on the current exposure in seconds to millisecond
            precision

        If no exposure is running, it returns 0

        Description
        -----------        
        Used to poll exposure countdown progress when expose() is
        called with wait=False
        
        '''

        reply = azcam.db.tools["exposure"].get_exposuretime_remaining()

        timeLeft = f"{reply:.3f}"

        return timeLeft


    def abort(self):
        '''
        Abort an exposure in progress

        Returns
        -------
        None

        '''

        try:
            azcam.db.tools["exposure"].abort()
        except AttributeError:
            return

        return


    def abortReadout(self):
        '''
        Abort a detector readout in progress

        Returns
        -------
        None

        '''

        try:
            azcam.db.tools["controller"].readout_abort()
        except AttributeError:
            return

        return


    def pixelsLeft(self):
        '''
        Poll the controller for the number of pixels remaining to be readout

        Returns
        -------
        reply : int
            Number of pixels remaining to be readout.
            
        Will return 0 if no readout is in progress/complete.

        See also: pctRead()
        '''

        reply = azcam.db.tools["controller"].get_pixels_remaining()

        return reply
    

    def pctRead(self):
        '''
        Return the percentage of the CCD readout

        Returns
        -------
        pctLeft : float
            percentage of the CCD readout, range 0..100%

        Will return 100% is readout is complete.
        
        See also pixelsLeft()
        '''
        
        numPix = azcam.db.tools["exposure"].image.focalplane.numpix_image
        fracLeft = azcam.db.tools["controller"].get_pixels_remaining()/numPix
        
        return 100*(1.0-fracLeft)


    def ccdTemps(self) -> list:
        '''
        Query the CCD temperature sensors readout through the
        Archon controller.  For MODS these are the detector
        and detector mount temperatures used for heat control.
        The dewar temperature and pressure are readout through
        the MODS instrument control system.

        Push these to the telescope DD if it is available.
        
        Returns
        -------
        list of floats: 
            CCD detector and mount temperatures in decimal celsius.

        The Archon reads temperatures to the nearest 0.01C
        
        '''

        # Get the detector temperatures from the Archon.  Won't read temperatures
        # if the Archon is actively exposing

        try:
            statDict = azcam.db.tools["controller"].get_status()
            hxBoard = azcam.db.tools["tempcon"].heaterx_board
            
            if f"{hxBoard}/TEMPA" in statDict:
                ccdTemp = statDict[f"{hxBoard}/TEMPA"]
            else:
                ccdTemp = "-999.9"
                
            if f"{hxBoard}/TEMPB" in statDict:
                baseTemp = statDict[f"{hxBoard}/TEMPB"]
            else:
                baseTemp = "-999.9"
        except:
            return ["OK","-999.9","-999.9"] # no-read values

        # If a good read, push ccdTemp to the telescope DD.

        #setTemp = {f"{self.ddSide}_MODS{self.modsChan}CCDTemp":ccdTemp}
        #try:
        #    azcam.db.tools["telescope"].set_parameter(setTemp)
        #except:
        #    pass
        
        return ["OK", ccdTemp, baseTemp]
    
    
    def archonTemp(self):
        '''
        Read the Archon controller backplane temperature

        Returns
        -------
        archonTemp: float
            temperature of the Archon controller backplane
            
        Description
        -----------
        The liquid glycol recirculation cold plate is connected to the
        backplane of the Archon controller. An RTD on the backplane reports
        the temperature at the cold plate interface, which is our indication
        that the cooling is functioning.
        
        '''
        
        try:
            statDict = azcam.db.tools["controller"].get_status()
            if "BACKPLANE_TEMP" in statDict:
                bpTemp = statDict["BACKPLANE_TEMP"]
            else:
                bpTemp = "-999.9"
        except:
            return "-999.9" # no-read value
            
        # If a good read, push archonTemp to the telescope DD.

        #setTemp = {f"{self.ddSide}_MODS{self.modsChan}ArchonTemp":bpTemp}
        #try:
        #    azcam.db.tools["telescope"].set_parameter(setTemp)
        #except:
        #    pass
        
        return bpTemp
            

    def archonStatus(self):
        '''
        Read the Archon controller status
        
        Returns
        -------
        ccdPower: int
            CCD power state (integer code)
        archonTemp: float
            Archon backplane temperature in C
        ccdTemp: float
            CCD detector temperature in C (TempA)
        baseTemp: float
            CCD detector mount base temperature in C (TempB)
        heaterOut: float
            CCD temperature controller output voltage
        heaterP: int
            CCD temperature controller proportional term for the PID loop
        heaterI: int
            CCD temperature controller integral term for the PID loop
        heaterD: int
            CCD temperature controller derivative term for the PID loop
            
        Description
        -----------
        Reads the Archon controller status and extracts the data associated
        with the Archon backplane, and HeaterX board data we need. This command
        is used to provide essential Archon and CCD temperature controller
        data for logging.
        
        For the MODS CCD system we only use Heater A.
        
        Temperature sensors return -999.9 as the "no-read" datum.
        
        The CCD power state (status POWER keyword) is 
           0 = unknown
           1 = not configured
           2 = off
           3 = intermediate (power-up/down sequence in progress)
           4 = on
           5 = standby mode
        '''
        
        try:
            statDict = azcam.db.tools["controller"].get_status()
            hxBoard = azcam.db.tools["tempcon"].heaterx_board
            
            if "POWER" in statDict:
                ccdPower = statDict["POWER"]
            else:
                ccdPower = "0"
                
            if "BACKPLANE_TEMP" in statDict:
                bpTemp = statDict["BACKPLANE_TEMP"]
            else:
                bpTemp = "-999.9"
                
            if f"{hxBoard}/TEMPA" in statDict:
                ccdTemp = statDict[f"{hxBoard}/TEMPA"]
            else:
                ccdTemp = "-999.9"
                
            if f"{hxBoard}/TEMPB" in statDict:
                baseTemp = statDict[f"{hxBoard}/TEMPB"]
            else:
                baseTemp = "-999.9"

            if f"{hxBoard}/HEATERAOUTPUT" in statDict:
                heaterOut = statDict[f"{hxBoard}/HEATERAOUTPUT"]
            else:
                heaterOut = "-999.9"

            if f"{hxBoard}/HEATERAP" in statDict:
                heaterP = statDict[f"{hxBoard}/HEATERAP"]
            else:
                heaterP = "-999.9"
            
            if f"{hxBoard}/HEATERAI" in statDict:
                heaterI = statDict[f"{hxBoard}/HEATERAI"]
            else:
                heaterI = "-999.9"

            if f"{hxBoard}/HEATERAD" in statDict:
                heaterD = statDict[f"{hxBoard}/HEATERAD"]
            else:
                heaterD = "-999.9"

        except:
            return ["0","-999.9","-999.9","-999.9","-999.9","-999.9","-999.9","-999.9"] # no-read values
                    
        return [ccdPower,bpTemp,ccdTemp,baseTemp,heaterOut,heaterP,heaterI,heaterD]
        

    def set_CCDSetPoint(self,setPoint):
        '''
        Set the CCD temperature control setpoint on the Archon

        Parameters
        ----------
        setPoint : float
            new setpoint temperature in degrees C

        Returns
        -------
        None.

        '''
        azcam.db.tools["tempcon"].set_control_temperature(setPoint)
    
        return azcam.db.tools["tempcon"].control_temperature
    
    
    def get_CCDSetPoint(self):
        '''
        Get the CCD temperature control setpoint temperature 

        Returns
        -------
        setPoint: float
            current setpoint temperature in degrees C

        '''
        return azcam.db.tools["tempcon"].control_temperature
    

    def shopen(self):
        '''
        Open the MODS camera shutter

        Returns
        -------
        reply : string
            Archon controller reply or an error message.
            
        The shutter stays open until shclose() is sent or the Archon
        controller is reset.

        This is a bare-metal azcam controller operation.  We have
        to find the index of system parameter TRIGOUTINVERT from the
        controller tool dict_wconfig, craft the WCONFIG command to 
        set TRIGOUTINVERT=1, send it, then follow with APPLYSYSTEM to set it.
        
        shopen() must always be followed by shclose().  However, to ensure
        we don't leave the shutter open, every expose() command will
        use shclose() to make sure.
        
        See also: shclose()
        '''
        
        try:
            indx = azcam.db.tools["controller"].dict_wconfig["TRIGOUTINVERT"]
            cmd = f"WCONFIG{indx&0xFFFF:04X}TRIGOUTINVERT=1"
            reply = azcam.db.tools["controller"].archon_command(cmd)
            reply = azcam.db.tools["controller"].archon_command("APPLYSYSTEM")
            reply = "shutter open"
        except Exception as exp:
            reply = f"ERROR shopen() - {exp}"
            
        return reply
    

    def shclose(self):
        '''
        Close the MODS camera shutter
        
        Returns
        -------
        Archon controller response or error message if exception.

        This is a bare-metal azcam controller operation.  We have
        to find the index of system parameter TRIGOUTINVERT from the
        controller tool dict_wconfig, craft the WCONFIG command to
        set TRIGOUTINVERT=0, send it, then follow with APPLYSYSTEM to set it.

        See also: shopen()
        '''
        
        try:
            indx = azcam.db.tools["controller"].dict_wconfig["TRIGOUTINVERT"]
            cmd = f"WCONFIG{indx&0xFFFF:04X}TRIGOUTINVERT=0"
            reply = azcam.db.tools["controller"].archon_command(cmd)
            reply = azcam.db.tools["controller"].archon_command("APPLYSYSTEM")
            reply = "shutter closed"
        except Exception as e:
            reply = f"ERROR shclose() - {e}"
            
        return reply
        

    def set_filename(self,fileStr=None):
        '''
        Set the filename and exposure number

        Parameters
        ----------
        fileStr : string
            A filename string to be processed

        Returns
        -------
        None.
        
        Description
        -----------
        Uses modsFilename() to deconstruct into the components needed
        by the azcam server.
        
        If filename is None or blank, it builds a default name from
        the instrument ID and observing date string generated by obsDate()
        and sets the sequence number to 1.
        
        Validation
        ----------
        All filenames are tested against existing files in the raw
        data folder. If this filename belongs to an existing file,
        we enforce strict no-overwrite and ask them to try again, because
        if we try to overwrite from azcam.expose() methods, bad things can (will)
        happen.
        
        See also: set_path(), set_fileroot(), set_expnum()
        '''

        # if no filename given, autogenerate a filename, otherwise use fileStr
        
        if fileStr is not None and len(fileStr) > 0:
            dataPath,rootName,expNum = self.modsFilename(fileStr)
            autoName = False
        else:
            # build the default name based on obsDate (CCYYMMDD at local noon)
            rootName = f"{self.modsID.lower()}.{self.obsDate()}."
            expNum = 1
            autoName = True
            
        # rootnames for MODS have to end with ., make sure it does
        
        if not rootName.endswith('.'): rootName += '.'

        # Validation
        # Build the full filename with path and make sure does not already exist in the
        # raw data folder.  If the requested name exists, two options:
        #   1) if this was a user-requested filename, exit and scold them for their bad choice
        #   2) if auto-generated using obsDate, make one attempt to find the next unused name
        
        reqFilename = os.path.join(azcam.db.tools["exposure"].folder,
                               f"{rootName}{expNum:04d}.fits")
        if os.path.exists(reqFilename):
            if not autoName:
                return f"ERROR requested filename {reqFilename} would overwrite existing image file - try again"
            else:
                curPath = azcam.db.tools["exposure"].folder
                curRoot = azcam.db.tools["exposure"].root
                flist = glob.glob(f"{os.path.join(curPath,curRoot)}*.fits")
                flist.sort()
                if len(flist)>0:
                    tp,tr,exn = self.modsFilename(flist[-1])
                    expNum = exn + 1
                else:
                    return f"ERROR filename {reqFilename} would overwrite existing file - check and try again"
                
        # we have a new, unused image filename, commit
        
        azcam.db.tools["exposure"].root = rootName
        azcam.db.tools["exposure"].sequence_number = expNum
        
        # build the full /folder/filename to report
        
        reqFilename = os.path.join(azcam.db.tools["exposure"].folder,f"{rootName}{expNum:04d}.fits")
            
        # return with the full filename including path for info
        
        return reqFilename
       
    
    def get_filename(self):
        '''
        Get the name of the next file to be written

        Returns
        -------
        nextfile: string
            next file to be written in sequence

        '''

        fullName = azcam.db.tools["exposure"].get_filename()
        return os.path.basename(fullName)
    
    # get_nextfile() and nextfile() are aliases for get_filname()
    
    get_nextfile = get_filename
    nextfile = get_filename
        
    
    def get_lastfile(self):
        '''
        Retreive the name of the last file written
        
        Returns
        -------
        lastfile: string
            name of the last file written.
            
        Description
        -----------
        Returns the name of the last image file written during
        the current azcam server session. If none, it returns
        the name of the last file in the raw data folder chronogically.
        If there are no files in the raw data folder, return None

        '''

        lastFile = azcam.db.tools["exposure"].last_filename
        if len(lastFile) == 0:
            curPath = azcam.db.tools["exposure"].folder
            fitsList = glob.glob(f"{os.path.join(curPath)}*.fits")
            fitsList.sort(key=os.path.getmtime)
            if len(fitsList) > 0:
                return fitsList[-1]
            else:
                return 'None'
        else:
            return lastFile
    
    
    # lastfile() is an alias for get_lastfile()
    
    lastfile = get_lastfile


    def set_expnum(self,expnum: int=1) -> str :
        '''
        Set the exposure sequence number for the next image to be written.
        
        `expnum` must be 1..9999 as expected by the LBTO data archive.

        Parameters
        ----------
        expnum : int
            exposure sequence number, range: 1...9999
            
        Returns
        -------
        string: "OK" or error

        '''
        
        # Validation 1: is exn in the range 1...9999?
        
        exn = int(expnum)
        if exn < 1 or exn > 9999:
            return f"ERROR set_expnum() expnum={expnum} invalid, must be 1..9999"
        
        # Validation 2: is the requested exposure number already in use?
        
        rootName = azcam.db.tools["exposure"].root
        dataPath = azcam.db.tools["exposure"].folder
        if os.path.exists(os.path.join(dataPath,f"{rootName}{exn:04d}.fits")):
            return f"ERROR an image file with expnum {exn:04d} already exists, try again"
        
        # we're good to go, commit
        
        azcam.db.tools["exposure"].sequence_number = exn
        
        return f"OK expnum is {exn:04d}"
    
    
    def get_expnum(self):
        '''
        Get the images sequence number of the next image to be written

        Returns
        -------
        int: image sequence number

        '''
        return azcam.db.tools["exposure"].sequence_number
    

    def set_path(self,dataPath="/home/data"):
        '''
        Set the file path for raw images.

        Parameters
        ----------
        dataPath : string, optional
           Valid, writable file path for raw imaging data
           The default is "/home/data".

        Returns
        -------
        string: "OK" or error message
        
        Description
        -----------
        Sets the full path to the local system folder where the azcam
        server will write raw FITS images.
        
        This function validates `dataPath` to ensure that:
            * the path exists on the system running the azcam server
            * the path is writeable by the user that started azcam
        before changing the path on the azcam server.
        
        See also: get_path(), get_filename(), set_filename()
        '''

        if os.path.exists(dataPath):
            if os.access(dataPath,os.W_OK):
                if dataPath.endswith("/"): 
                    azcam.db.tools["exposure"].folder = dataPath
                else:
                    azcam.db.tools["exposure"].folder = f"{dataPath}/"
            else:
                return f"ERROR set_path() directory {dataPath} exists but is not writable"
        else:
            return f"ERROR set_path() directory {dataPath} was not found"
        
        return f"OK image path is {dataPath}"


    def get_path(self):
        '''
        Get the file path for where raw FITS files will be written.

        Returns
        -------
        dataPath: string
            data file path for raw images

        '''
        return azcam.db.tools["exposure"].folder


    def set_keyword(self,fitsKey,value,comment=None):
        '''
        Set a FITS header keyword

        Parameters
        ----------
        fitsKey : string
            FITS header keyword, must be 8 chars or less.
        value : 
            value of the keyword (float, int, or str)
        comment : string, optional
            comment. The default is None

        Returns
        -------
        None.

        If fitsKey is more than 8 characters long, it will truncate
        it to the first 8 characters.
        '''
        
        newKey = f"{fitsKey.upper():.08s}".strip()
        dataType = type(value).__name__
        if comment is not None and len(comment)==0:
            comment = None

        # strip extraneous quotes that might come in from a client string

        if dataType == 'str':
            value = re.sub("[\"\']","",value)

        # set the keyword
        
        azcam.db.tools["exposure"].header.set_keyword(newKey,value,
                                                      azcam.utils.dequote(comment)
                                                      ,dataType)
        
        return f"OK set header keyword {newKey}"
    
    
    def get_keyword(self,fitsKey):
        '''
        Get a keyword from the current FITS header template

        Parameters
        ----------
        fitsKey : string
            FITS header keyword to retrieve.

        Returns
        -------
        Value of the FITS header keyword value or error message if not

        '''
        try:
            val,comment,ktype = azcam.db.tools["exposure"].header.get_keyword(fitsKey.upper())
            return val
        except:
            return f"ERROR get_keyword() header keyword {fitsKey} not found"
        
        
    def clearKeywords(self,toolID="exposure"):
        '''
        Clear a keyword database.  Default is the exposure database

        Returns
        -------
        None.

        '''
        try:
            azcam.db.tools[toolID].header.delete_all_keys()
            return
        except:
            return
        
        
    def set_imageInfo(self,imgType=None,imgTitle=None):
        '''
        Set the image type and title for the next exposure

        Parameters
        ----------
        imgType : string
            Image type, one of {object,bias,dark,flat,comp,zero}
        imgTitle : string
            Title for the image FITS header OBJECT dard

        Returns
        -------
        list: strings
            OK or an error message.
            
        Description
        -----------
        Sets the image type and title for the next exposure.
        
        imgType is recorded in the IMAGETYP keyword in the FITS header, and setting
        this also sets the shutter state (open/light or closed/dark) for
        the next exposure.  Only allowed types can be given.
        
        imgTitle is the image title recorded in the OBJECT keyword in the FITS
        header.
        
        If imgType is None or "", it retains the current image type
        If imgTitle is None, it retains the current image title
        If imgTitle is "", it clears the image title
        
        '''
        
        # if imgType is not given, don't change it, but maybe change title
        # because imgType must take one of certain allowed values, we force
        # lower case and validate against self.image_types for the class
        
        if imgType is not None and len(imgType) > 0:
            imgType = re.sub("[\"\']","",imgType.lower()) # lowercase and cleaned of quotes
            if imgType in self.image_types:
                azcam.db.tools["exposure"].set_image_type(imgType)
                self.set_keyword("IMAGETYP",imgType.upper())
            else:
                return f"ERROR Unrecognized image type {imgType} must be one of {str(self.image_types)}"

        # if imgTitle is not given, don't change it.  We might have changed imgType

        if imgTitle is None:
            imgType = azcam.db.tools["exposure"].get_image_type()
            imgTitle = azcam.db.tools["exposure"].get_image_title()
            return f"OK {imgType.upper()} {imgTitle}"

        # strip extraneous quotes that might be in imgTitle
        
        imgTitle = re.sub("[\"\']","",imgTitle)
        
        # imgTitle="" means "clear the image title".  Set None in the exposure
        # tool, as "" just retains the current title.
        
        if len(imgTitle) == 0:
            azcam.db.tools["exposure"].set_image_title(None)
            self.set_keyword("OBJECT","")
        else:
            azcam.db.tools["exposure"].set_image_title(imgTitle)
            
        imgType = azcam.db.tools["exposure"].get_image_type()
        imgTitle = azcam.db.tools["exposure"].get_image_title()
        return f"OK {imgType.upper()} {imgTitle}"
        

    def get_imageInfo(self):
        '''
        Get the image type and image title for the next image

        Returns
        -------
        imgType : string
            Image type, one of {object,bias,dark,flat,comp,zero}.
        imgTitle : string
            Title for the image (OBJECT keyword in the FITS header)

        if imgTitle is empty, returns "None".
        '''
        
        imgType = azcam.db.tools["exposure"].get_image_type()
        imgTitle = azcam.db.tools["exposure"].get_image_title()
        if len(imgTitle) == 0:
            return f"{imgType.upper()} None"
        else:
            return f"{imgType.upper()} {imgTitle}"
        
            
    def set_istatus(self,isisStr):
        '''
        Parse an ISTATUS string from the MODS IE and set the instrument
        tool header keyword dictionaries for the image FITS haders.

        Parameters
        ----------
        isisStr : string
            Raw ISTATUS command message string from an IE (mmc server) client

        Returns
        -------
        None.

        '''    
        # Recognized data types.  The standard typestrings dict carries
        # strings (e.g., 'int') we translate to data type for type casting

        knownTypes = {'int':int,'float':float,'str':str}
        
        # if nothing to parse, return
        
        if len(isisStr) == 0:
            return
        
        # strip residual extraneous quotes from isisStr
        
        isisStr = re.sub("[\"\']","",isisStr)

        # dice up isisStr and make a dictionary of values
        
        isisBits = isisStr.split('|')
        statusDict = {}
        for bit in isisBits:
            keyval = bit.strip().split(':')
            statusDict[keyval[0].upper()] = keyval[1]

        # current keyword list from the instrument header template
        
        keys = list(azcam.db.tools["instrument"].header.keywords.keys())
        
        # current header keyword value and typestring dicts 
        
        values = azcam.db.tools["instrument"].header.values
        types = azcam.db.tools["instrument"].header.typestrings
        
        # Only change the values of keywords in the ISTATUS message we
        # are expecting.  Typecase as needed.
        
        for key in keys:
            if key in statusDict:
                if types[key] != 'str':
                    values[key] = knownTypes[types[key]](statusDict[key])
                else:
                    values[key] = statusDict[key]
                
        # We have values, pass them back to the server
        
        azcam.db.tools["instrument"].header.values = values
    
        return "OK loaded istatus into header"
    
    #----------------------------------------------------
    #  various utilities
    #    

    def modsFilename(self,fileStr=None):
        '''
        Break a filename string down into the components
        
        Parameters
        ----------
        fileStr : string
            Filename string to decompose into primary components

        Returns
        -------
        dataPath: string
            Path to the data directory folder
        rootName: string
            File rootname (no sequence number of .fits extension)
        expNum: int
            Image sequence number (or 1 if none given)

        Description
        -----------
        A well-formed MODS-style filename looks like
            
            /dataPath/rootName.expNum:%04d.fits
            
        where:
            * dataPath = full path to a writeable data directory folder
            * rootName = rootname (e.g. mods1b.20251011 or m1bTest) without sequence number or extension
            * expNum = sequence number, 0..9999
        
        If the `fileStr` argument is None or blank, it returns the current file 
        components of the next file to be written by the azcam server.
        
        We ignore any .fits extension included in the filename

        This is a utility function that helps us avoid making malformed
        MODS filenames that will cause problems for the observatory systems
        and LBT data archive later.
        
        See also: set_filename(), set_expnum(), set_path()
        '''
        
        # if blank or None, respond with current components
        
        if fileStr is None or len(fileStr)==0:
            dataPath = azcam.db.tools["exposure"].folder
            rootName = azcam.db.tools["exposure"].root
            expNum = azcam.db.tools["exposure"].sequence_number
            return [dataPath, rootName, expNum]
        
        # Split into path and basename
        
        dataPath, baseName = os.path.split(fileStr)
        if len(dataPath) == 0:
            dataPath = azcam.db.tools["exposure"].folder
        
        # split basename on .
        
        fileBits = baseName.split('.')
        numBits = len(fileBits)
        
        # if no . anywhere in baseName, return default parts
        
        if numBits == 1:
            rootName = baseName
            expNum = 1

        # if more than one part, look at last part.  Is it "fits"?
        
        else:
            if fileBits[-1] == "fits":
                if numBits == 2:
                    # fileStr is rootName.fits without number, set expNum=1
                    rootName = fileBits[0]
                    expNum = 1
                else:
                    # is penultimate bit a number?
                    try:
                        expNum = int(fileBits[-2])
                        rootName = '.'.join(fileBits[:-2])
                    except:
                        rootName = '.'.join(fileBits[:-1])
                        expNum = 1
            else:
                # doesn't end in fits, is last bit a number < 9999?
                try:
                    isInt = int(fileBits[-1])
                    if isInt <= 9999:
                        expNum = isInt
                        rootName = '.'.join(fileBits[:-1])
                    else:
                        expNum = 1
                        rootName = '.'.join(fileBits[:])
                except:
                    rootName = '.'.join(fileBits[:])
                    expNum = 1

        return dataPath, rootName, expNum
                

    def obsDate(self):
        '''
        Return the observing date string in CCYYMMDD format

        Returns
        -------
        string: observing date in CCYYMMD format, see description.

        Description
        -----------        
        Returns the observing date in CCYYMMDD format.  We define the
        an "observing date" as running from noon to noon local time.
        This ensures that all data taken for a given night have 
        contiguous filenames that do not change at local or UT midnight.
          
        For example, the observing date for the night starting at sunset
        on 2025 July 13 and ending at sunrise on 2025 July 14 is `20250714`.
        
        We use this in the filenames of raw data and logs for MODS.

        Important
        ---------
        This function assumes the system clock has been set to localtime.
        If the system clocks are set to UTC, this fails badly because of
        how timedelta() works.
        '''
        
        if float(datetime.datetime.now().strftime("%H")) < 12.0:  # is it before noon local time
            return (datetime.datetime.now().today() - datetime.timedelta(days=1)).strftime("%Y%m%d")
        else:
            return datetime.datetime.now().today().strftime("%Y%m%d")

    #-------------------------------------------------------------------------
    #
    # odd bits
    #
    
    # modest test function, as in "wtf does _this_ do?"

    def wtf(self,cmdStr: str,wait: bool=False) -> str:
        try:
            if wait:
                return f"wtf is {cmdStr} wait?"
            else:
                return f"wtf is {cmdStr} nowait?"
        except Exception as e:
            return f"ERROR wtf? - {e}"
