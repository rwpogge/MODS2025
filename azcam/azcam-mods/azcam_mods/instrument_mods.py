# Contains the interface class to get MODS instrument information stored
# on the observatory Instrument Interface (IIF) data dictionary.  This
# adapts code from the custom Telescope() class instance for LBT in
# this package (telescope_lbt.py)

import lbto.iif as iif

import yaml
import os
import math

# import time # uncomment if we need to run timing experiments

import azcam
import azcam.utils
import azcam.exceptions
from azcam.tools.instrument import Instrument


class MODSInstrument(Instrument):
    '''
    MODS instrument interface 
    
    Built on the azcam Instrument() base class, which includes hooks for
    the azcam Header() base class.
    
    We don't enable active control of a MODS instrument (MODS1 or MODS2)
    from azcam.  Instead, we use the Instrument tool class to provide access to
    instrument configuration for building the image FITS headers. We do
    this by interogating the LBTO Instrument Interface data dictionary (aka DD)
    where the MODS instrument control and data-acquisition system exports 
    real-time instrument state information via the `modsDD` agent that runs on
    the instrument servers as a systemd service.  
    
    For that reason, the internals of MODSInstrument look a lot like
    the LBTTelescope() Telescope tool class instance in telescope_lbt.py in
    this package.
    
    Author: R. Pogge, OSU Astronomy Dept (pogge.1@osu.edu)
    
    Updated: 2025 September 23 [rwp/osu]
    '''
    
    def __init__(self, tool_id="instrument", description="Multi-Object Double Spectrograph (MODS)",instID="mods",side="left"):
        super().__init__(tool_id, description)

        self.lbtSide = side
        self.instID = instID

        self.initialized = 0
        
        # IIF instance and proxy info
        
        self.modsIIF = None
        self.proxy = iif.model['PROXIES'].get(self.instID)

        # header configuration file is in the systemfolder/template directory
        # named "modsInst_left.txt" or "modsInst_right.txt"
        
        self.cfgFile = os.path.join(azcam.db.systemfolder, 
                                    "templates", 
                                    f"{self.instID}Inst_{self.lbtSide}.txt"
                                    )

        # This uses the configuration file to build other information we'll need.  

        try:        
            self.readHeaderConfig(self.cfgFile)
        except Exception as exp:
            azcam.log(f"ERROR: MODSInstrument init cannot read {self.cfgFile} - {exp}")
            return
        
        self.define_keywords()

        return


    def initialize(self):
        '''
        Initialize the LBT IIF connection

        Returns
        -------
        None.
        
        '''

        if self.is_initialized:
            return

        if not self.is_enabled:
            azcam.exceptions.warning(f"{self.description} is not enabled")
            return

        # header file is in the systemfolder/template directory
        
        self.cfgFile = os.path.join(azcam.db.systemfolder, 
                                    "templates", 
                                    f"{self.instID}Inst_{self.lbtSide}.txt"
                                    )

        # add keywords

        try:        
            self.readHeaderConfig(self.cfgFile)
        except Exception as exp:
            azcam.log(f"ERROR: MODS IIF init failed {exp}")
            self.is_initialized = 0
            return
        
        self.define_keywords()
        
        self.is_initialized = 1

        return
    

    def _verify(self):
        '''
        Verify that the LBT IIF interface is enabled and running

        Returns
        -------
        None.

        '''

        if not self.is_enabled:
            azcam.exceptions.warning(f"{self.description} LBT IIF link is not enabled")

        if not self.is_initialized:
            self.initialize()

        return

    # **************************************************************************************************
    # header
    # **************************************************************************************************
    
    def define_keywords(self):
        '''
        Define keywords on the LBT IIF data dictionary (DD) we read to 
        store instrument configuration info in the image FITS headers

        Returns
        -------
        None.

        Description
        -----------
        The contents of the header are read in from a yaml-format configuration
        file with the readHeaderConfig() method. After we read its contents, 
        we call define_keywords() to populate elements azcan Header() class used 
        deeper in azcam to build the instrument FITS header block.
        '''

        # add keywords to header

        for key in self.fitsList:
            self.header.keywords[key] = self.modsIIFKeywords[key]
            self.header.comments[key] = self.modsIIFComments[key]
            self.header.typestrings[key] = self.modsIIFTypes[key]
            
        azcam.log("MODS instrument header keywords defined")
        
        return
    

    def get_keyword(self, keyword):
        '''
        Retrieve a single keyword from the LBT IIF Data Dictionary

        Parameters
        ----------
        keyword : string
            A valid IIF DD keyword in the fitsList.

        Raises
        ------
        azcam
            AzcamError exception on problems

        Returns
        -------
        list
            List with the keyword data: [value,comment,dtype]

        '''

        self._verify()

        reqKey = keyword.upper()
        
        if reqKey not in self.fitsList:
            raise azcam.exceptions.AzcamError(f"ERROR: invalid {reqKey} not in list")
            return
        
        # retrieve just reqKey.  You have to send it a list even if just one...
        
        try:
            instData = self.get_InstData([self.modsIIFKeywords[reqKey]])
        except Exception as exp:
            azcam.log(f"ERROR: get_keyword() Cannot query IIF DD - {exp}")
            return

        # see if additional processing is needed

        try:
            ddKey = self.modsIIFKeywords[reqKey]
            key = reqKey
            c = self.modsIIFComments[key]
            t = self.modsIIFTypes[key]
            iift = self.ddDict[ddKey][1]
            if iift == 'str':
                v = instData[0]
            elif iift == 'int':
                t = 'int'
                v = int(instData[0])
            elif iift == 'float':
                t = 'float'
                v = float(instData[0])
            elif iift == 'float-as':
                t = 'float'
                v = float(instData[0])/3600.0
            elif iift == 'float-rad':
                t = 'float'
                v = math.degrees(float(instData[0]))
            else:
                t = 'str'
                v = instData[0]
 
        except Exception as exp:
            azcam.log(f"ERROR: get_keyword() Cannot procees {key} data - {exp}")
            return
                                       
        # store value in Header

        self.set_keyword(key,v)

        # return the keyword list: [value, comment, typestring]
        
        return [v, c, t]


    def read_header(self):
        '''
        Query the LBT IIF data dictionary and populate the instrument FITS header        

        Raises
        ------
        azcam
            AzcamError exception on errors

        Returns
        -------
        header : list of lists with the TCS data
            Each header element is a list of [keyword,value,comment,dtype]
            dtype is one of 'int', 'float', or 'str'.

        Description
        -----------
        This is called by the data acquisition portions of the azcam
        server to populate the FITS header database with the MODS instrument
        configuration at the start of an exposure.
        
        '''

        self._verify()

        header = []

        try:
            instData = self.get_InstData(self.ddList)
        except Exception as exp:
            azcam.log(f"ERROR read_header() - {exp}")
            return header
            
        # We have TCS data, page through and pre-process as needed
        
        for i, ddKey in enumerate(self.ddList):
            try:
                key = self.fitsList[i]
                c = self.modsIIFComments[key]
                t = self.modsIIFTypes[key]
                iift = self.ddDict[ddKey][1]
                if iift == 'str':
                    v = instData[i]
                elif iift == 'int':
                    t = 'int'
                    v = int(instData[i])
                elif iift == 'float':
                    t = 'float'
                    v = float(instData[i])
                elif iift == 'float-as':
                    t = 'float'
                    v = float(instData[i])/3600.0
                elif iift == 'float-rad':
                    t = 'float'
                    v = math.degrees(float(instData[i]))
                else:
                    t = 'str'
                    v = instData[i]
                
                header.append([key,v,c,t])
                
            except Exception as exp:
                azcam.log("ERROR", key, exp)
                continue

            # store they keyword in the header

            self.header.set_keyword(key,v,c,t)
            
        return header

    ###################################################################
    # methods for loading the TCS header spec from a yaml config file
    # and retrieving data from the IIF
    ###################################################################
    
    def readHeaderConfig(self,configFile):
        '''
        Read and parse a yaml configuration file to set up the MODS
        instrument FITS header keyword dictionaries

        Parameters
        ----------
        configFile : string
            Full path and name of a yaml-format header configuration file.

        Returns
        -------
        None.

        Description
        -----------
        A MODS instrument header configuration file is a yaml-format file
        named like "modsInst_left.txt" or "modsInst_right.txt" that
        contains two required dictionaries: 
            * IIFConfig: with the setup info (instrument, side, client file)
            * DDEntires: mapping of IIF data dictionary entries to instrument FITS header entries
            
        See the offline documentation or worked examples for the format.  The
        format is strict and if badly formatted could break things.
        
        We use the yaml.safe_load() method
        
        '''
        self.cfgData = None
        
        if os.path.exists(configFile):
            try:
                stream = open(configFile,'r')
            except Exception as exp:
                raise azcam.exceptions.AzcamError(f"Cannot open MODS instrument header config file {configFile} - {exp}")
                return
        
            try:
                self.cfgData = yaml.safe_load(stream)
            except yaml.YAMLError as exp:
                stream.close()
                self.cfgData = None
                raise RuntimeError(f"ERROR: Cannot load MODS instrument header config file {configFile} - {exp}")
                return
    
            if len(self.cfgData)==0:
                self.cfgData = None
                stream.close()
                raise RuntimeError(f"ERROR: MODS instrument header config file {configFile} is empty!")
                return

            stream.close()
    
        else:
            raise ValueError(f"ERROR: MODS instrument header config file {configFile} not found")
            return
        
        # We have a config file, pick it apart and get what we need
        
        azcam.log(f"Opened MODS instrument FITS header configuration file {configFile}")
        
        # Get the IIF configuration info
        
        try:
            self.instID = self.cfgData["IIFConfig"]["instID"]
            self.lbtSide = self.cfgData["IIFConfig"]["side"]
            self.clientFile =self.cfgData["IIFConfig"]["client_config"]
            self.proxyName = f"py_{self.instID}_{self.lbtSide}"
        except Exception as exp:
            self.instID = None
            self.lbtSide = None
            self.clientFile = None
            self.proxyName = None
            
            raise azcam.exceptions.AzcamError(f"Cannot read IIFConfig glock data from {configFile} - {exp}")
        
        # Extract the DD dictionary and list of DD entries.  We use these to load the 
        # the header definition dictionaries (keywords, typestrings, and comments)
        
        try:
            self.ddDict = self.cfgData['DDEntries']
            self.ddList = list(self.ddDict.keys())
        except Exception as exp:
            self.ddDict = None
            self.ddList = None
            raise azcam.exceptions.AzcamError(f"Cannot read DD entries from {configFile} - {exp}")
        
        # build the dictionaries that map FITS keywords to IIF DD entries and their attributes: 
        #      fitsList = list of FITS header keywords
        #   iifKeywords = dictionary of IIF DD entries corresponding to fitsList
        #   iifTypes = dictionary of typestrings corresponding to IIF DD data (removing float type units conversion tags)
        #   iifComments = dictionary of comments corresponding to IIF DD data

        self.fitsList = []
        self.modsIIFKeywords = {}
        self.modsIIFComments = {}
        self.modsIIFTypes = {}
        for key in self.ddList:
            fitsKey = self.ddDict[key][0].upper() # enforce uppercase for FITS keywords
            self.fitsList.append(fitsKey)
            self.modsIIFKeywords[fitsKey] = key
            if self.ddDict[key][1].startswith("float"):
                self.modsIIFTypes[fitsKey] = "float"
            else:
                self.modsIIFTypes[fitsKey] = self.ddDict[key][1]
            self.modsIIFComments[fitsKey] = self.ddDict[key][2]

        azcam.log(f"Loaded TCS header configuration from {configFile}")
        
        return
    
        
    def get_InstData(self,keyList):
        '''
        Get MODS instrument configuration data from the LBT IIF data dictionary

        Parameters
        ----------
        keyList : string list
            List of strings with valid DD keywords to retreive

        Returns
        -------
        None.

        Description
        -----------
        Start the IIF proxy, query the data dictionary, and close the proxy.  
        This way we don't have an active IIF proxy live during long periods of 
        idleness.
            
        Experiments show it adds negligibly to the IIF query which 
        for the usual query of 70+ DD parameters takes 120-150msec with
        of the proxy setup/teardown overhead.
            
        We intend to only use the IIF interface as an "instantaneous"
        instrument status query.
        
        '''
        
        try:
            self.modsIIF.proxy_destroy(self.proxyName)
        except:
            pass
        
        try:
            self.modsIIF = iif.iifproxy(proxyName=self.proxyName,
                                    instrumentID=self.proxy['instrument'],
                                    focalStation=self.proxy['focalstation'],
                                    side=self.lbtSide,
                                    config_client=self.clientFile)
        except Exception as exp:
            azcam.log(f"ERROR: IIF proxy init failed - cannot start IIF link - {exp}")
            self.is_initialized = 0
            self.modsIIF = None
            raise azcam.exceptions.AzcamError(f"get_InstData() query failed - {exp}")
            return

        # We have a link, send the request

        try:
            iifData = self.modsIIF.GetParameter(keyList)
        except Exception as exp:
            azcam.log(f"ERROR: IIF GetParameter() query failed - {exp}")
            self.modsIIF.proxy_destroy(self.proxyName)
            raise azcam.exceptions.AzcamError(f"get_InstData() query failed - {exp}")
            return
            
        # all done, close the proxy and return the data
            
        try:
            self.modsIIF.proxy_destroy(self.proxyName)
        except:
            pass
            
        return iifData

    #########################################################
    #  methods for direct access to the IIF Data Dictionary 
    #########################################################
    
    def set_parameter(self,ddDict):
        '''
        Set data dictionary parameters in the LBT IIF        

        Parameters
        ----------
        ddDict : dictionary
            IIF data dictionary parameters

        Raises
        ------
        azcam
            AzcamError exception if there are errors.

        Returns
        -------

        Description
        -----------
        Start the IIF proxy, send the parameter, close the proxy.  This way
        we don't have an active IIF proxy live during long periods of 
        idleness.
        
        ddDict has structure {ddName1:val1,ddName2:val2} we want to
        send to the DD.  Values must be string representations of
        the parameters, the IIF DD proper takes care of typing.
        
        Unknown ddNames will raise an AzcamError exception because
        the keyword is unknown to the DD.
        
        This is exposed to other parts of the azcam server as
        `instrument.set_parameter()`
        
        '''
        
        try:
            self.modsIIF.proxy_destroy(self.proxyName)
        except:
            pass
        
        try:
            self.modsIIF = iif.iifproxy(proxyName=self.proxyName,
                                    instrumentID=self.proxy['instrument'],
                                    focalStation=self.proxy['focalstation'],
                                    side=self.lbtSide,
                                    config_client=self.clientFile)
        except Exception as exp:
            azcam.log(f"ERROR: IIF proxy init failed - cannot start IIF link - {exp}")
            self.is_initialized = 0
            self.modsIIF = None
            raise azcam.exceptions.AzcamError(f"instrument set_parameter() failed - {exp}")
            return

        # We have a link, send the parameter
        
        try:
            self.modsIIF.SetParameter(ddDict)
        except Exception as exp:
            azcam.log(f"ERROR: IIF SetParameter() failed - {exp}")
            self.modsIIF.proxy_destroy(self.proxyName)
            raise azcam.exceptions.AzcamError(f"instrument set_parameter() query failed - {exp}")
            return
            
        # all done, close the proxy and return the data
            
        try:
            self.modsIIF.proxy_destroy(self.proxyName)
        except:
            pass
            
        return

    # probably not useful in operations, but somtimes useful for engineering
    
    def get_parameter(self,ddKey: str):
        '''
        Get a data dictionary parameter from the LBT IIF

        Parameters
        ----------
        ddKey : string
            IIF data dictionary keyword

        Raises
        ------
        azcam
            AzcamError exception if there are errors.

        Returns
        -------
        ddVal : string
            data dictionary value retrieved
            
        Description
        -----------
        Start the IIF proxy, get the parameter, close the proxy. This way we 
        don't have an active IIF proxy live during long periods of 
        idleness.
            
        Somewhat inefficient, but we aren't doing this very often.            
        '''
        
        try:
            self.modsIIF.proxy_destroy(self.proxyName)
        except:
            pass
        
        try:
            self.modsIIF = iif.iifproxy(proxyName=self.proxyName,
                                    instrumentID=self.proxy['instrument'],
                                    focalStation=self.proxy['focalstation'],
                                    side=self.lbtSide,
                                    config_client=self.clientFile)
        except Exception as exp:
            azcam.log(f"ERROR: IIF proxy init failed - cannot start IIF link - {exp}")
            self.is_initialized = 0
            self.modsIIF = None
            raise azcam.exceptions.AzcamError(f"instrument get_parameter() failed - {exp}")
            return

        # We have a link, get the DD parameter
        
        try:
            iifData = self.modsIIF.GetParameter([ddKey])
        except Exception as exp:
            azcam.log(f"ERROR: IIF GetParameter() failed - {exp}")
            self.modsIIF.proxy_destroy(self.proxyName)
            raise azcam.exceptions.AzcamError(f"instrument get_parameter() query failed - {exp}")
            return
            
        # all done, close the proxy and return the data
            
        try:
            self.modsIIF.proxy_destroy(self.proxyName)
        except:
            pass
            
        return iifData
