'''
Defines the MODS class for azcam

Original by Mike Lesser

Updated: 2025 July 23 [rwp/osu]

Additions:
    expose(): take an exposure (async)
    expwait(): take an exposure and wait until done (blocking)
    shopen(): open the shutter
    shclose(): close the shutter
    set/get_exptime(): set/get the exposure time
    ccdTemps(): read the CCD detector temperatures read by Archon
    abortReadout(): abort readout
    pctRead(): return percentage of CCD readout (inverse of pixels_remaining)
    set/get_roi(): set/get the CCD readout region of interest and binning factor
    set/get_ccdbin(): set CCD binning in x and y
    reset_roi() restore full-frame unbinned readout (alias: roi_off())
    reset_ccdbin() restore 1x1 binning, leave ROI unchanged
    set/get_filename(): set/get the next file to be written
    get_nextfile(): get the name of the next file to be written (alias for get_filename)
    get_lastfile(): get the name of the last file written
    set_path() and get_path(): data path, set_ checks for validity and access
    set_expnum() and get_expnum(): set/get number of next image to be written
    set/get_keyword(): set/get a header keyword, ensure proper sytnax
    set/get_imageInfo(): set/get IMAGETYP and OBJECT for the next image
    set_istatus(): process instrument ISTATUS info into the instrument FITS header database
    obsDate(): return the observing date CCYYMMDD noon-to-noon local time
    modsFilename(): split a filename string into dataPath, rootName, and expNum
    
'''

import datetime
import os
import re

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

    def __init__(self):
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

        # MODS parameters we need to expose via azcam.db.tools["mods"]
        
        # MODS instrument channel and LBT telescope side
        
        self.modsID = azcam.db.systemname
        try:
            self.lbtSide = azcam.db.parameters.get_par("side","lbttcs")
        except:
            self.lbtSide = "unknown"
        
        # MODS allowed image_types.  These are stored in the FITS headers
        # as the IMAGETYP keyword values expected by the LBTO data archive
        # and data reduction pipelines.
        
        self.image_types = ['object','bias','flat','dark','comp','zero']
        
        # The MODS shutter_dict tells the azcam exposure tool if the shutter 
        # should be open (1) or closed (0) for each allowed MODS IMAGETYP:
        
        self.shutter_dict = {'object':1,
                             'bias':0,
                             'flat':1,
                             'dark':0,
                             'comp':1,
                             'zero':0
                             }
        
        # Overload the azcam allowed image types and shutter data in
        # the exposure tool with the MODS definitions
        
        azcam.db.tools["exposure"].image_types = self.image_types
        azcam.db.tools["exposure"].shutter_dict = self.shutter_dict
        
        # anything else?
        
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

        return


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
        None.

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

        return
    
    
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

        return "OK"

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
        
        return

    
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
        
        return
        

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
            return "ERROR: set_expTime() expTime={expTime} invalid, must be >= 0.0 seconds"

        try:        
            azcam.db.tools["exposure"].set_exposuretime(et)
        except Exception as e:
            return f"ERROR: set_exptime() - {e}"

        return "OK"


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

        See also: expwait()        
        '''

        # strip extraneous quotes from image_title
        
        if len(image_title) > 0:
            image_title = re.sub("[\"\']","",image_title)

        try:
            reply = azcam.db.tools["exposure"].expose1(exposure_time,
                                                       image_type,
                                                       image_title
                                                       )
            return reply
        except Exception as e:                                                       
            return f"ERROR: expose() - {e}"
    
    
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

        See also: exppse()        
        '''

        if len(image_title) > 0:
            image_title = re.sub("[\"\']","",image_title)

        try:
            reply = azcam.db.tools["exposure"].expose(exposure_time,
                                                      image_type,
                                                      image_title
                                                      )
            return reply
        except Exception as e:                                                       
            return f"ERROR: expwait() - {e}"
    

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
        and the dictionary of string translations is `azcam.db.exposureflags`.
        
        Examples:
           * '0 NONE'
           * '8 SETUP'
           * '1 EXPOSING'
           * '7 READOUT'
           * '9 WRITING'

        See also: timeleft(), pixelsLeft()
        '''
        
        expFlag = azcam.db.tools["exposure"].exposure_flag

        # translate the integer code into a meaningful string
        
        efd = azcam.db.exposureflags
        statStr = list(filter(lambda key: efd[key]==expFlag,efd))[0]
        
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

        Returns
        -------
        list of floats: 
            CCD detector and mount temperatures in decimal celsius.

        Temperatures are read to the nearest 0.1C
        
        '''

        reply = azcam.db.tools["tempcon"].get_temperatures()

        ccdTemp = "%.1f" % reply[0]
        mountTemp = "%.1f" % reply[1]

        return ["OK", ccdTemp, mountTemp]
    
    
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
        return
    
    
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

        '''
        
        try:
            reply = azcam.db.tools["controller"].archon_command("TRIGOUTINVERT=1")
        except Exception as e:
            reply = f"ERROR: shopen() - {e}"
            
        return reply
    

    def shclose(self):
        '''
        Close the MODS camera shutter
        
        Returns
        -------
        Archon controller response or error message if exception.

        See also: shopen()
        '''
        
        try:
            reply = azcam.db.tools["controller"].archon_command("TRIGOUTINVERT=0")
        except Exception as e:
            reply = f"ERROR: shclose() - {e}"
            
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
        
        See also: set_path(), set_fileroot(), set_expnum()
        '''

        if fileStr is not None and len(fileStr) > 0:
            dataPath,rootName,expNum = self.modsFilename(fileStr)
        else:
            # build the default name based on obsDate (CCYYMMDD at local noon)
            rootName = f"{self.modsID.lower()}.{self.obsDate()}."
            expNum = 1
            
        if not rootName.endswith('.'): rootName += '.'

        azcam.db.tools["exposure"].root = rootName
        azcam.db.tools["exposure"].sequence_number = expNum
            
        return "OK"
       
    
    def get_filename(self):
        '''
        Get the name of the next file to be written

        Returns
        -------
        nextfile: string
            next file to be written in sequence

        '''
        return azcam.db.tools["exposure"].get_filename()
    
    # get_nextfile() and nextfile() are aliases for get_filname()
    
    get_nextfile = get_filename
    nextfile = get_filename
        
    
    def get_lastfile(self):
        '''
        Retreive the name of the last file written during the
        current azcam server session.

        Returns
        -------
        lastfile: string
            name of the last file written.
            
        `lastfile` will be blank ("") if the server was restarted.

        '''

        return azcam.db.tools["exposure"].last_filename
    
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
        
        exn = int(expnum)
        if exn < 1 or exn > 9999:
            return f"ERROR: set_expnum() expnum={expnum} invalid, must be 1..9999"
        
        azcam.db.tools["exposure"].sequence_number = exn
        
        return "OK"
    
    
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
                return f"ERROR: set_path() directory {dataPath} exists but is not writable"
        else:
            return f"ERROR: set_path() directory {dataPath} was not found"
        
        return "OK"


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
            
        azcam.db.tools["exposure"].header.set_keyword(newKey,value,comment,dataType)
        
        return
    
    
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
            return f"ERROR: get_keyword() header keyword {fitsKey} not found"
        
        
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
            if imgType.lower() in self.image_types:
                azcam.db.tools["exposure"].set_image_type(imgType.lower())
                self.set_keyword("IMAGETYP",imgType.upper())
            else:
                return f"ERROR: Unrecognized image type {imgType} must be one of {str(self.image_types)}"

        # if imgTitle is not given, don't change it.  We might have changed imgType

        if imgTitle is None:
            return

        # strip extraneous quotes that might be in imgTitle
        
        imgTitle = re.sub("[\"\']","",imgTitle)
        
        # imgTitle="" means "clear the image title".  Set None in the exposure
        # tool, as "" just retains the current title.
        
        if len(imgTitle) == 0:
            azcam.db.tools["exposure"].set_image_title(None)
            self.set_keyword("OBJECT","")
        else:
            azcam.db.tools["exposure"].set_image_title(imgTitle)
            
        return
        

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
    
        return
    
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
            dataPath = dataPath = azcam.db.tools["exposure"].folder
        
        # split basename on .
        
        fileBits = baseName.split('.')
        numBits = len(fileBits)
        
        # if no . anywhere in baseMame, return default parts
        
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
                # is last bit a number?
                try:
                    expNum = int(fileBits[-1])
                    rootName = '.'.join(fileBits[:-1])
                except:
                    rootName = '.'.join(fileBits[:-1])
                    expNum = 1
            
        return dataPath, rootName, expNum
                

    def obsDate(self):
        '''
        Return the observing date string

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
        '''

        if float(datetime.datetime.now().strftime("%H")) < 12.0:  # before noon
            return (datetime.date.today() - datetime.timedelta(days=1)).strftime("%Y%m%d")
        else:
            return datetime.date.today().strftime("%Y%m%d")


    # modest test function, as in "wtf does _this_ do?"

    def wtf(self,cmdStr: str,wait: bool=False) -> str:
        try:
            if wait:
                return f"wtf is {cmdStr} wait"
            else:
                return f"wtf is {cmdStr} nowait"
        except Exception as e:
            return f"ERROR: wtf~? - {e}"
        
