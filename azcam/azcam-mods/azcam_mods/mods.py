"""
Contains the MODS class for azcam.

Updated: 2025 July 11 [rwp/osu]

Additions:
    expose() takes wait boolean, default False (no-wait)
    shopen(): open the shutter
    shclose(): close the shutter
    binning() -> ccdbin()
    setexp() -> set_expTime()
    camstat() -> ccdTemps()
    readout_abort() -> abortReadout()
    pctRead(): return percentage of CCD readout (inverse of pixels_remaining)
    roiOff(): restore full-frame unbinned readout
    set_fileroot(): set file rootname (w/o path, number, or extension)
    get_fileroot(): get the file rootname
    get_filename(): get the next file to be written (alias: get_nextfile())
    get_lastfile(): get the name of the last file written
    set_path() and get_path(): data path, set_ checks for validity and access
    set_expnum() and get_expnum(): set/get number of next image to be written
    obsDate(): return the observing date CCYYMMDD noon-to-noon local time
"""

import datetime
import os

import typing

import azcam


class MODS(object):
    """
    Class definition of LBT MODS.
    
    These methods are called remotely through the command server with syntax such as:
    mods.expose 1.0 "zero" "some image title".
    
    Many additions over the basic methods provided by Mike Lesser's original
    draft version to provide features for the MODS spectrographs at LBT
    without having to hack the main azcam module.
    
    Updated: 2025 July 10 [rwp/osu]
    """

    def __init__(self):
        """
        Creates mods tool.
        """

        azcam.db.mods = self
        azcam.db.tools["mods"] = self
        azcam.db.cli["mods"] = self

        return

    def initialize(self):
        """
        Initialize AzCam system.
        """

        azcam.db.tools["exposure"].reset()

        return

    def reset(self):
        """
        Reset the exposure
        """

        azcam.db.tools["exposure"].reset()

        return

    def set_par(self, parameter, value):
        """
        Set a server parameter
        """

        if parameter == "remote_imageserver_host":
            azcam.db.tools["exposure"].sendimage.remote_imageserver_host = value
        elif parameter == "remote_imageserver_port":
            azcam.db.tools["exposure"].sendimage.remote_imageserver_port = int(value)
        else:
            azcam.db.parameters.set_par(parameter, value)

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
        Set the CCD region of interest to (ROI) to readout
        and binning

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
            binning factor in columns. The default is -1.
        row_bin : int, optional
            binning factor in rows. The default is -1.
        roi_num : int, optional
            ROI number. The default is 0, the only value at present
            

        Returns
        -------
        None.

        Description
        -----------
        Sets the CCD region of interest for readout and binning.
        
        -1 for any parameter means "do not change"
        
        Thus:
            mods.set_roi -1 -1 -1 -1 2 2 
        leave ROI the same but sets 2x2 binning
            mods.set_roi -1 -1 100 500 -1 -1
        leaves ROI columns the same, sets rows to 100:500, binning unchanged
        
        See also: ccdbin()
        
        '''

        azcam.db.tools["exposure"].set_roi(
            first_col, last_col, first_row, last_row, col_bin, row_bin
        )

        return
    

    def roiOff(self):
        '''
        Turn "Off" region of interest readout and binning, restoring
        full-frame unbinned operation.
        
        Returns
        -------
        str
            return status, "OK" if ok
        
        '''
            
        azcam.db.tools["exposure"].roi_reset()
        azcam.db.tools["exposure"].set_roi(-1,-1,-1,-1,1,1)

        return "OK"


    def set_expTime(self, et: float = 1.0) -> str:
        """
        Set camera exposure time in seconds.
        """

        azcam.db.tools["exposure"].set_exposuretime(et)

        return "OK"


    def expose(self, 
               exposure_time: float = -1, 
               image_type: str = "", 
               image_title: str = "", 
               wait: bool = False) -> typing.Optional[str]:
        """
        Make a complete exposure.

        :param exposure_time: the exposure time in seconds
        :param image_type: str type of exposure ('zero', 'object', 'flat', ...)
        :param image_title: str image title, usually surrounded by double quotes
        :param wait: bool wait for exposure complete (default: False)
        
        """

        if wait:
            reply = azcam.db.tools["exposure"].expose(
                exposure_time, image_type, image_title
                )           

        else:
            reply = azcam.db.tools["exposure"].expose1(
                exposure_time, image_type, image_title
                )

        return reply
    

    def timeleft(self) -> float:
        """
        Return remaining exposure time (in seconds).
        """

        reply = azcam.db.tools["exposure"].get_exposuretime_remaining()

        etr = "%.3f" % reply

        return etr


    def ccdTemps(self) -> list:
        '''
        Query the CCD and dewar temperature sensors through the
        Archon controller.

        Returns
        -------
        list
            floats: CCD and Dewar temperatures in decimal celsius.

        '''

        reply = azcam.db.tools["tempcon"].get_temperatures()

        ccdtemp = "%.2f" % reply[0]
        dewtemp = "%.2f" % reply[1]

        return ["OK", ccdtemp, dewtemp]
    

    def ccdbin(self, colbin=1, rowbin=1):
        """
        Set on-ship binning
        
        Was binning()
        """

        azcam.db.tools["exposure"].set_roi(-1, -1, -1, -1, colbin, rowbin)

        return

    
    def geterror(self):
        """
        Return and clear current error status.
        """

        return ["OK", ""]


    def flush(self, cycles=1):
        """
        Flush sensor "cycles" times.
        """

        azcam.db.tools["exposure"].flush(cycles)

        return


    def abort(self):
        """
        abort exposure
        """

        try:
            azcam.db.tools["exposure"].abort()
        except AttributeError:
            return

        return


    def abortReadout(self):
        """
        abort readout
        """

        try:
            azcam.db.tools["controller"].readout_abort()
        except AttributeError:
            return

        return


    def pixels_remaining(self):
        """
        Pixels remaining till readout finished.
        """

        reply = azcam.db.tools["controller"].get_pixels_remaining()

        return reply
    

    def pctRead(self):
        '''
        Return the percentage of the CCD readout

        Returns
        -------
        None.

        '''
        numPix = azcam.db.tools["exposure"].image.focalplane.numpix_image
        fracLeft = azcam.db.tools["controller"].get_pixels_remaining()/numPix
        
        return 100*(1.0-fracLeft)


    def shopen(self):
        '''
        Open the MODS camera shutter
        
        Returns
        -------
        None.

        '''
        
        reply = azcam.db.tools["controller"].archon_command("TRIGOUTINVERT=1")
        
        return reply
    

    def shclose(self):
        '''
        Close the MODS camera shutter
        
        Returns
        -------
        None.

        '''
        
        reply = azcam.db.tools["controller"].archon_command("TRIGOUTINVERT=0")
        
        return reply

    
    def set_fileroot(self,filestr):
        '''
        Set the root filename for MODS files.  Should end in a "."
        but we make sure it does if omitted.  Make sure it does not
        include a sequence number or path spec or .fits, those are all
        set elsewhere.
        
        Parameters
        ----------
        filestr : string

        Returns
        -------
        string: "OK" or not
        
        See also: set_path(), set_expnum()

        '''
    
        if len(filestr) == 0:
            modsID = azcam.db.systemname
            azcam.db.tools["exposure"].root = f"{modsID.lower()}.{self.obsDate()}."
            
        if not filestr.endswith("."):
            azcam.db.tools["exposure"].root = f"{filestr}."
        else:
            azcam.db.tools["exposure"].root = filestr
            
        # set the file sequence number to 1, can be reset later using
        # set_expnum()
        
        azcam.db.tools["exposure"].sequence_number = 1
            
        return "OK"


    def get_fileroot(self):
        '''
        Get the rootname for image files

        Returns
        -------
        fileRoot: file rootname
        
        Rootname does not include data folder path, sequence number, or
        .fits extension

        '''
        return azcam.db.tools["exposure"].root
    
    
    def get_filename(self):
        '''
        Get the name of the next file to be written

        Returns
        -------
        nextfile: string
            next file to be written in sequence

        '''
        return azcam.db.tools["exposure"].get_filename()
    
    # alias for get_nextfile is get_filename
    
    get_nextfile = get_filename


    def get_lastfile(self):
        '''
        Retreive the name of the last file written

        Returns
        -------
        lastfile: string
            name of the last file written.  Will be blank if the server
            was restarted.

        '''

        return azcam.db.tools["exposure"].last_filename
    

    def set_expnum(self,expnum):
        '''
        Set the exposure sequence number for the next image.
        
        Must be 1..9999

        Parameters
        ----------
        expnum : int
            exposure sequence number, range: 1...9999
            
        Returns
        -------
        string: "OK" or error

        '''
        
        if expnum < 1 or expnum > 9999:
            return "ERROR: expnum must be 1..9999"
        
        azcam.db.tools["exposure"].sequence_number = expnum
        
        return "OK"
    
    
    def get_expnum(self):
        '''
        Get the images sequence number of the next image

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
        '''

        if os.path.exists(dataPath):
            if os.access(dataPath,os.W_OK):
                if dataPath.endswith("/"): 
                    azcam.db.tools["exposure"].folder = dataPath
                else:
                    azcam.db.tools["exposure"].folder = f"{dataPath}/"
            else:
                return f"ERROR: {dataPath} is not writable"
        else:
            return f"ERROR: {dataPath} not found"
        
        return "OK"


    def get_path(self):
        '''
        Get the image file path

        Returns
        -------
        dataPath: string
            data file path for raw images

        '''
        return azcam.db.tools["exposure"].folder

            
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
          
        For example, the observing date for the night starting at sunset
        on 2024 Dec 17 and ending at sunrise on 2024 Dec 18 is 20241217

        We use this for filenames for data and logs.
        '''

        if float(datetime.datetime.now().strftime("%H")) < 12.0:  # before noon
            return (datetime.date.today() - datetime.timedelta(days=1)).strftime("%Y%m%d")
        else:
            return datetime.date.today().strftime("%Y%m%d")
