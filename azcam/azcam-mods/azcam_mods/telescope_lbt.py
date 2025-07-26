# Contains the interface class to the LBT Telescope Control System
# using the Instrument InterFace (IFF) facility built on ZeroC Ice

import lbto.iif as iif

import yaml
import os
import math

# import time - use if we need to run timing experiments

import azcam
import azcam.utils
import azcam.exceptions
from azcam.tools.telescope import Telescope


class LBTTCS(Telescope):
    '''
    Interface to the Large Binocular Telescope TCS server
    
    Built on the azcam Telescope() base class, which includes
    hooks for the azcam Header() base class.
    
    Author: R. Pogge, OSU Astronomy Dept (pogge.1@osu.edu)
    
    Updated: 2025 July 21
    '''
    
    def __init__(self, tool_id="telescope", description="Large Binocular Telescope",instID="mods",side="left"):
        super().__init__(tool_id, description)

        self.lbtSide = side
        self.instID = instID

        return

    def initialize(self):
        '''
        Initialize the LBT telescope interface

        Returns
        -------
        None.
        
        Builds the header configuration file name, which is in the system
        templates folder with names like <instID>TCS_<lbtSide>.txt
        (e.g. `modsTCS_left.txt`).

        '''

        if self.is_initialized:
            return

        if not self.is_enabled:
            azcam.exceptions.warning(f"{self.description} is not enabled")
            return

        # header file is in the systemfolder/template directory
        
        self.hdrFile = os.path.join(azcam.db.systemfolder, 
                                    "templates", 
                                    f"{self.instID}TCS_{self.lbtSide}.txt"
                                    )

        # add keywords

        try:        
            self.readHeaderConfig(self.hdrFile)
        except Exception as exp:
            azcam.log(f"ERROR: TCS init failed {exp}")
            self.is_initialized = 0
            return
        
        self.define_keywords()


        # instantiate the iif proxy
        
        try:
            self.proxy = iif.model['PROXIES'].get(self.instID)
        except Exception as exp:
            azcam.log(f"ERROR: TCS init failed - could not get IIF proxy - {exp}")
            self.is_initialized = 0
            self.tcs = None
            self.proxy = None
            return
        
        try:
            self.tcs = iif.iifproxy(proxyName=self.proxyName,
                                    instrumentID=self.proxy['instrument'],
                                    focalStation=self.proxy['focalstation'],
                                    side=self.lbtSide,
                                    config_client=self.clientFile)
        except Exception as exp:
            azcam.log(f"ERROR: TCS init failed - could start IIF link - {exp}")
            self.is_initialized = 0
            self.tcs = None
            return
            
        # We have a link to the LBT TCS, get some info that shut it
        # down until we need it (prevents a broken proxy taking down)
        # the azcam server
        
        azcam.log("LBT TCS Link initialized")
        
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
            azcam.exceptions.warning(f"{self.description} TCS link is not enabled")

        if not self.is_initialized:
            self.initialize()

        return

    # **************************************************************************************************
    # header
    # **************************************************************************************************
    
    def define_keywords(self):
        '''
        Define keywords on the LBT IIF data dictionary (DD) we read to 
        store telescope status in the image FITS headers

        Returns
        -------
        None.

        Description
        -----------
        The contents of the header are read in from a yaml-format
        configuration file with the readHeaderConfig() method.  After
        we read that data in, define_keywords() is called to populate elements
        of the Header() class used deeper in they system to build the headers
        '''

        # add keywords to header

        for key in self.fitsList:
            self.header.keywords[key] = self.iifKeywords[key]
            self.header.comments[key] = self.iifComments[key]
            self.header.typestrings[key] = self.iifTypes[key]
            
        azcam.log("TCS header keywords defined")
        
        return
    

    def get_keyword(self, keyword):
        '''
        Retrieve a single keyword from the LBT TCS

        Parameters
        ----------
        keyword : string
            A valid telescope keyword in the fitsList.

        Raises
        ------
        azcam
            AzCamError exception on problems

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
            tcsData = self.get_TCSData([self.iifKeywords[reqKey]])
        except Exception as exp:
            azcam.log(f"ERROR: get_keyword() could not query TCS DD - {exp}")
            return

        # see if additional processing is needed

        try:
            ddKey = self.iifKeywords[reqKey]
            key = reqKey
            c = self.iifComments[key]
            t = self.iifTypes[key]
            iift = self.ddDict[ddKey][1]
            if iift == 'str':
                v = tcsData[0]
            elif iift == 'int':
                t = 'int'
                v = int(tcsData[0])
            elif iift == 'float':
                t = 'float'
                v = float(tcsData[0])
            elif iift == 'float-as':
                t = 'float'
                v = float(tcsData[0])/3600.0
            elif iift == 'float-rad':
                t = 'float'
                v = math.degrees(float(tcsData[0]))
            else:
                t = 'str'
                v = tcsData[0]
 
        except Exception as exp:
            azcam.log(f"ERROR: get_keyword() could not procees {key} data - {exp}")
            return
                                       
        # store value in Header

        self.set_keyword(key,v)

        # return the keyword list: [value, comment, typestring]
        
        return [v, c, t]


    def read_header(self):
        '''
        Query the LBT TCS and populate the telescope header        

        Raises
        ------
        azcam
            AzCamError exception on errors

        Returns
        -------
        header : list of lists with the TCS data
            Each header element is a list of [keyword,value,comment,dtype]
            dtype is one of 'int', 'float', or 'str'.

        Description
        -----------
        This is called by the data acquisition portions of the azcam
        server to populate the FITS header database with TCS information
        at the start of an exposure.
        
        '''

        self._verify()

        header = []

        try:
            tcsData = self.get_TCSData(self.ddList)
        except Exception as exp:
            azcam.log(f"ERROR read_header() - {exp}")
            return header
            
        # We have TCS data, page through and pre-process as needed
        
        for i, ddKey in enumerate(self.ddList):
            try:
                key = self.fitsList[i]
                c = self.iifComments[key]
                t = self.iifTypes[key]
                iift = self.ddDict[ddKey][1]
                if iift == 'str':
                    v = tcsData[i]
                elif iift == 'int':
                    t = 'int'
                    v = int(tcsData[i])
                elif iift == 'float':
                    t = 'float'
                    v = float(tcsData[i])
                elif iift == 'float-as':
                    t = 'float'
                    v = float(tcsData[i])/3600.0
                elif iift == 'float-rad':
                    t = 'float'
                    v = math.degrees(float(tcsData[i]))
                else:
                    t = 'str'
                    v = tcsData[i]
                
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
        Reads and parses a yaml configuration file that sets up the
        header keyword dictionaries

        Parameters
        ----------
        configFile : string
            Full path and name of a yaml-format header configuration file.

        Returns
        -------
        None.

        Description
        -----------
        An LBT TCS header configuration file is a yaml-format file
        usually named something like "modsTCS_left.txt" that
        contains two required dictionaries: 
            * IIFConfig: with the setup info (instrument, side, client file)
            * DDEntires: mapping of IIF data dictionary entries to FITS header entries
            
        See the offline documentation or worked examples for the format.  The
        format is strict and if badly formatted could break things.
        
        We use the yaml.safe_load() method
        
        '''
        self.cfgData = None
        
        if os.path.exists(configFile):
            try:
                stream = open(configFile,'r')
            except Exception as exp:
                raise azcam.exceptions.AzcamError(f"could not open LBT TCS header config file {configFile} - {exp}")
                return
        
            try:
                self.cfgData = yaml.safe_load(stream)
            except yaml.YAMLError as exp:
                stream.close()
                self.cfgData = None
                raise RuntimeError(f"ERROR: could not load LBT TCS header config file {configFile} - {exp}")
                return
    
            if len(self.cfgData)==0:
                self.cfgData = None
                stream.close()
                raise RuntimeError(f"ERROR: LBT TCS header config file {configFile} is empty!")
                return

            stream.close()
    
        else:
            raise ValueError(f"ERROR: LBT TCS header config file {configFile} not found")
            return
        
        # We have a config file, pick it apart and get what we need
        
        azcam.log(f"Opened LBT TCS FITS header configuration file {configFile}")
        
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
            
            raise azcam.exceptions.AzcamError(f"Could not read IIFConfig data from {configFile} - {exp}")
        
        # Extract the DD dictionary and list of DD entries.  We use these to load the 
        # the header definition dictionaries (keywords, typestrings, and comments)
        
        try:
            self.ddDict = self.cfgData['DDEntries']
            self.ddList = list(self.ddDict.keys())
        except Exception as exp:
            self.ddDict = None
            self.ddList = None
            raise azcam.exceptions.AzcamError(f"Could not read DD entries from {configFile} - {exp}")
        
        # build the dictionaries that map FITS keywords to IIF DD entries and their attributes: 
        #      fitsList = list of FITS header keywords
        #   iifKeywords = dictionary of IIF DD entries corresponding to fitsList
        #   iifTypes = dictionary of typestrings corresponding to IIF DD data (removing float type units conversion tags)
        #   iifComments = dictionary of comments corresponding to IIF DD data

        self.fitsList = []
        self.iifKeywords = {}
        self.iifComments = {}
        self.iifTypes = {}
        for key in self.ddList:
            fitsKey = self.ddDict[key][0].upper() # enforce uppercase for FITS keywords
            self.fitsList.append(fitsKey)
            self.iifKeywords[fitsKey] = key
            if self.ddDict[key][1].startswith("float"):
                self.iifTypes[fitsKey] = "float"
            else:
                self.iifTypes[fitsKey] = self.ddDict[key][1]
            self.iifComments[fitsKey] = self.ddDict[key][2]

        azcam.log(f"Loaded TCS header configuration from {configFile}")
        
        return
    
        
    def get_TCSData(self,keyList):
        '''
        start proxy, get data, close proxy return data

        Parameters
        ----------
        keyList : string list
        List of strings with valid DD keywords to retreive

        Returns
        -------
        None.

        Description
        -----------
        Starts the IIF proxy, does the query, closes the proxy.  This way
        we don't have an active IIF proxy live during long periods of 
        idleness.
            
        Experiments show it adds negligibly to the IIF query which 
        for the usual query of 70+ DD parameters takes 120-150msec with
        of the proxy setup/teardown overhead.
            
        We intend to only use the IIF interface as an "instantaneous"
        TCS status query, no active operation of the telescope.  If
        we ever got it in our heads to do that, we need to investigate
        async methods in the python ZeroC Ice implementation.
            
        '''
        
        try:
            self.tcs.proxy_destroy(self.proxyName)
        except:
            pass
        
        try:
            self.tcs = iif.iifproxy(proxyName=self.proxyName,
                                    instrumentID=self.proxy['instrument'],
                                    focalStation=self.proxy['focalstation'],
                                    side=self.lbtSide,
                                    config_client=self.clientFile)
        except Exception as exp:
            azcam.log(f"ERROR: TCS IIF proxy init failed - could start IIF link - {exp}")
            self.is_initialized = 0
            self.tcs = None
            raise azcam.exceptions.AzCamError(f"getTCSData() query failed - {exp}")
            return

        # We have a link, send the request

        try:
            iifData = self.tcs.GetParameter(keyList)
        except Exception as exp:
            azcam.log(f"ERROR: TCS IIF GetParameter() query failed - {exp}")
            self.tcs.proxy_destroy(self.proxyName)
            raise azcam.exceptions.AzcamError(f"getTCSData() query failed - {exp}")
            return
            
        # all done, close the proxy and return the data
            
        try:
            self.tcs.proxy_destroy(self.proxyName)
        except:
            pass
            
        return iifData


    def tcsSetParameter(self,ddKey: str,ddVal: str):
        '''
        Set a data dictionary parameter on the LBT TCS        

        Parameters
        ----------
        ddKey : string
            IIF data dictionary keyword
        ddV : string
            IIF data value to set as ddKey

        Raises
        ------
        azcam
            AzcamError exception if there are errors.

        Returns
        -------

        Description
        -----------
        Starts the IIF proxy, sends the parameter, closes the proxy.  This way
        we don't have an active IIF proxy live during long periods of 
        idleness.
            
        somewhat inefficient, but we aren't sending much very often.            
        '''
        
        try:
            self.tcs.proxy_destroy(self.proxyName)
        except:
            pass
        
        try:
            self.tcs = iif.iifproxy(proxyName=self.proxyName,
                                    instrumentID=self.proxy['instrument'],
                                    focalStation=self.proxy['focalstation'],
                                    side=self.lbtSide,
                                    config_client=self.clientFile)
        except Exception as exp:
            azcam.log(f"ERROR: TCS IIF proxy init failed - could start IIF link - {exp}")
            self.is_initialized = 0
            self.tcs = None
            raise azcam.exceptions.AzcamError(f"tcsSetParameter() failed - {exp}")
            return

        # We have a link, send the parameter

        iifData = {}
        iifData[ddKey]=ddVal
        
        try:
            self.tcs.SetParameter(iifData)
        except Exception as exp:
            azcam.log(f"ERROR: TCS IIF SetParameter() failed - {exp}")
            self.tcs.proxy_destroy(self.proxyName)
            raise azcam.exceptions.AzcamError(f"tcsSetParameter() query failed - {exp}")
            return
            
        # all done, close the proxy and return the data
            
        try:
            self.tcs.proxy_destroy(self.proxyName)
        except:
            pass
            
        return


    def tcsGetParameter(self,ddKey: str):
        '''
        Get a data dictionary parameter from the LBT TCS        

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
        Starts the IIF proxy, requests the parameter, closes the proxy.  
        This way we don't have an active IIF proxy live during long periods of 
        idleness.
            
        Somewhat inefficient, but we aren't doing this very often.            
        '''
        
        try:
            self.tcs.proxy_destroy(self.proxyName)
        except:
            pass
        
        try:
            self.tcs = iif.iifproxy(proxyName=self.proxyName,
                                    instrumentID=self.proxy['instrument'],
                                    focalStation=self.proxy['focalstation'],
                                    side=self.lbtSide,
                                    config_client=self.clientFile)
        except Exception as exp:
            azcam.log(f"ERROR: TCS IIF proxy init failed - could start IIF link - {exp}")
            self.is_initialized = 0
            self.tcs = None
            raise azcam.exceptions.AzcamError(f"tcsGetParameter() failed - {exp}")
            return

        # We have a link, get the DD parameter
        
        try:
            iifData = self.tcs.GetParameter([ddKey])
        except Exception as exp:
            azcam.log(f"ERROR: TCS IIF GetParameter() failed - {exp}")
            self.tcs.proxy_destroy(self.proxyName)
            raise azcam.exceptions.AzcamError(f"tcsGetParameter() query failed - {exp}")
            return
            
        # all done, close the proxy and return the data
            
        try:
            self.tcs.proxy_destroy(self.proxyName)
        except:
            pass
            
        return iifData
