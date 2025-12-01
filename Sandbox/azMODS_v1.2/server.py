"""
Setup method for LBTO MODS azcamserver

Usage example:
  python -i -m azcam_mods.server -- -mods1r
  
Updates:
    2025 Oct 15 - "flight" server beta version [rwp/osu]
    2025 Nov 28 - changes during CCD tune-up [mlesser]
    2025 Dec 01 - start of v1.2 changes
"""

import os
import sys
import glob
import datetime

import azcam
import azcam.utils
import azcam.exceptions
from azcam.server import setup_server
import azcam.shortcuts
from azcam.cmdserver import CommandServer
from azcam.header import System

# from azcam.tools.ds9display import Ds9Display

from azcam.tools.display import Display
from azcam.tools.telescope import Telescope
from azcam.tools.instrument import Instrument
from azcam.tools.archon.controller_archon import ControllerArchon
from azcam.tools.archon.exposure_archon import ExposureArchon

# azcam_mods classes

from azcam_mods.mods import MODS

from azcam_mods.detector_mods import detector_mods

# These overload Telescope, TempCon, and Instrument classes 
# with functions needed for MODS and Archon

from azcam_mods.telescope_lbt import LBTTelescope
from azcam_mods.instrument_mods import MODSInstrument
from azcam_mods.tempcon_mods import TempConMODS

# other stuff

from azcam.web.fastapi_server import WebServer

# read a YAML configuration file

import yaml

# Modern path-handling

from pathlib import Path

# load the azcam server runtime configuration file

def loadConfig(fileName):
    '''
    Load the contents of a YAML-format file using safe_load()   

    Parameters
    ----------
    fileName : string
        Name of the YAML-format configuration file with fully-qualified path.

    Raises
    ------
    RuntimeError
        Raised if file cannot be opened, read, or is empty.
    ValueError
        Raised if the file does not exist

    Returns
    -------
    data : dict
        Dictionary containing the file contents (nested dictionaries).

    '''

    if os.path.exists(fileName):
        try:
            stream = open(fileName,'r')
        except Exception as exp:
            raise RuntimeError(f"ERROR: could not open {fileName}: {exp}")
            
        try:
            data = yaml.safe_load(stream)
        except yaml.YAMLError as exp:
            raise RuntimeError(f"ERROR: could not load {fileName}: {exp}")
        
        if len(data)==0:
            raise RuntimeError(f"YAML file {fileName} is empty!")
        
        return data
    else:
        raise ValueError(f"YAML file {fileName} does not exist")
        

# setup the azcam server

def setup():
    '''
    Setup the azcam server
    

    Raises
    ------
    azcam
        Raised on azcam errors

    Returns
    -------
    None.

    '''

    # command line args

    option = "menu"
    try:
        i = sys.argv.index("-mods1r")
        option = "MODS1R"
        modsID = option
        lbtSide = "left"
    except ValueError:
        pass

    try:
        i = sys.argv.index("-mods1b")
        option = "MODS1B"
        modsID = option
        lbtSide = "left"
    except ValueError:
        pass

    try:
        i = sys.argv.index("-mods2r")
        option = "MODS2R"
        modsID = option
        lbtSide = "right"
    except ValueError:
        pass

    try:
        i = sys.argv.index("-mods2b")
        option = "MODS2B"
        modsID = option
        lbtSide = "right"
    except ValueError:
        pass

    try:
        i = sys.argv.index("-test")
        option = "test"
        modsID = "Blue"
        lbtSide = "right"
    except ValueError:
        pass
        
    try:
        i = sys.argv.index("-datafolder")
        datafolder = sys.argv[i + 1]
    except ValueError:
        datafolder = "/home/data/"

    try:
        i = sys.argv.index("-remotehost")
        remote_host = sys.argv[i + 1]
    except ValueError:
        remote_host = None

    setup_server()

    # for dev only

    azcam.db.verbosity = 2

    # hardwire the MODS Archon data-taking system configuration
    
    azcamRoot = "/home/dts/azcam"
    
    os.environ["AZCAM_DATAROOT"] = datafolder

    azcam.db.datafolder = datafolder
    
    # define folders for the system

    azcam.db.systemname = option

    azcam.db.rootfolder = os.path.abspath(azcamRoot)

    # We segregate systemfolder from datafolder because the latter is 
    # read/write by unprivileged users, but systemfolder contains
    # critical configuration files unprivileged users must not be
    # able to change
    #
    # System Folders:
    #  <azcamRoot>/system/<option>/parameters/
    #     parameter_server_<option>.ini (also parameter_console_<option>.ini if we use it)
    #     
    #  <azcamRoot>/system/<option>/templates/
    #     header_<option>.txt
    #
    #  <azcamRoot>/system/<option>/archon/
    #     <option>.ncf or <option>.acf
    #
    
    azcam.db.systemfolder = os.path.join(azcamRoot,"system",option)
    azcam.db.systemfolder = azcam.utils.fix_path(azcam.db.systemfolder)

    # Supposed to give a sensible default if datafolder=None on the
    # command line.  Too risky in this application so we comment out
    #azcam.db.datafolder = azcam.utils.get_datafolder(datafolder)

    # configuration menu
    
    menu_options = {"MODS1R": "MODS1R",
                    "MODS1B": "MODS1B",
                    "MODS2R": "MODS2R",
                    "MODS2B": "MODS2B",
                    "MODS test dewar": "test",
                    }

    if option == "menu":
        print("MODS azcam server startup menu:\n")
        option = azcam.utils.show_menu(menu_options)

    # parameters, templates, etc. live in azcam.db.systemfolder in our version
    
    if "MODS" in option:
        
        # paramteter files are parameters_server_<modsID>.ini in system/<modsID>/parameters/
        
        parfile = os.path.join(azcam.db.systemfolder,
                               "parameters", 
                               f"parameters_server_{option}.ini")
        
        # "flight" archon configuration (aka "timing") files are 
        # named <modsID>.acf in system/<modsID>/archon/
        
        timingfile = os.path.join(azcam.db.systemfolder,
                                  "archon",
                                  f"{option}.ncf")

        # exposure information header template
        # filename is header_<systemname>.txt in the system templates folder
        
        expTemplate = os.path.join(azcam.db.systemfolder,
                                   "templates",
                                   f"header_{azcam.db.systemname}.txt")
        
        # instrument information header template
        # name is instHdr_<systemname>.txt in the system templates folder
        
        instTemplate = os.path.join(azcam.db.systemfolder,
                                    "templates",
                                    f"instHdr_{azcam.db.systemname}.txt")

        # server mode and port
        
        azcam.db.servermode = "archon"
        cmdport = 2402

    # testdewar files live in test
    
    elif "test" in option:
        parfile = os.path.join(azcam.db.systemfolder,
                               "parameters",
                               "server_mods.ini")

        expTemplate = os.path.join(azcam.db.systemfolder,   
                                   "templates",    
                                   "header_mods.txt")
        
        timingfile = os.path.join(azcam.db.systemfolder,
                                  "archon",
                                  "mods_test.ncf")
        
        azcam.db.servermode = "MODS"
        cmdport = 2402

    else:
        raise azcam.exceptions.AzcamError(f"Unknown system configuration '{option}'")

    # logging is now set to /home/Logs/azcam/ for all MODS systems
    
    logfile = os.path.join("/home/Logs/azcam","server.log")
                           
    azcam.db.logger.start_logging(logfile=logfile)
    azcam.log(f"MODS Channel: {modsID}")
    azcam.log(f"LBT Side: {lbtSide}")

    # controller based on systemfolder
    
    controller = ControllerArchon()
    controller.timing_file = timingfile
    controller.camserver.port = 4242
    controller.camserver.host = "10.0.0.2"
    controller.reset_flag = 0  # 0 for soft reset, 1 to upload code
    controller.verbosity = 2

    # tempcon
    
    tempcon = TempConMODS(description="MODS Archon")
    tempcon.heaterx_board = "MOD10"
    tempcon.control_temperature = -100.0
    controller.heater_board_installed = 1
    if option == "mods1r" or option == "mods1b":
      tempcon.temperature_ids = [1, 0]  # basetemp, ccdtemp
    else:
      tempcon.temperature_ids = [0, 1]  # ccdtemp, basetemp

    # exposure
 
    exposure = ExposureArchon()
    exposure.filetype = exposure.filetypes["MEF"]
    exposure.image.filetype = exposure.filetypes["MEF"]
    exposure.display_image = 0
    exposure.add_extensions = 1
    exposure.shutter_delay = 250  # not optimized

    exposure.image.focalplane.gains = 4 * [2.0]
    exposure.image.focalplane.rdnoises = 4 * [2.0]

    if remote_host is None:
        pass
    else:
        exposure.send_image = 1
        exposure.sendimage.set_remote_imageserver(remote_host, 6543, "azcam")

    # set server instance identity info in the header
    
    try:
        system = System("MODS", expTemplate)
        system.set_keyword("INSTRUME", option, "MODS Instrument Channel Name")
        system.set_keyword("DEWAR",option,"CCD Dewar")
    except Exception:
        pass

    # detector

    exposure.set_detpars(detector_mods)
    exposure.fileconverter.set_detector_config(detector_mods)

    # image display system if any
    
    # display = Ds9Display()

    display = Display()

    # MODS customized commands

    mods = MODS(modsID,lbtSide)

    # load the exposure header template (static header content)
    
    if len(expTemplate) > 0 and os.path.exists(expTemplate):
        azcam.db.tools["exposure"].header.read_file(expTemplate)
        
    # read the server parameter file.  For MODS this will contain TCS interface 
    # (IIF) info needed later

    azcam.db.parameters.read_parfile(parfile)
    azcam.db.parameters.update_pars()
    
    # filename rules for MODS (some override parfile), 1=True, 0=False
    
    azcam.db.tools["exposure"].test_image = 0
    azcam.db.tools["exposure"].include_sequence_number = 1
    azcam.db.tools["exposure"].auto_increment_sequence_number = 1
    azcam.db.tools["exposure"].overwrite = 0 
    azcam.db.tools["exposure"].autoname = 0
    
    # other defaults
    
    azcam.db.tools["exposure"].auto_title = 0
    
    # default filename root pattern is the instrument ID lower case
    # and the current observing date, e.g., mods1b.20251007.  mind the .
    # at the end of root!
    
    azcam.db.tools["exposure"].root = f"{modsID.lower()}.{mods.obsDate()}."
    azcam.db.tools["exposure"].folder = datafolder

    # This might be a restart on the same obsDate, so see if there are
    # any files matching the intended file pattern in datafolder and
    # if there are, count them and select a safe next sequence number.
    # It looks for a 4-digit number followed by .fits
    # 
    # not bulletproof, but at least a start
    
    flist = glob.glob(f"{os.path.join(exposure.folder,exposure.root)}*.fits")
    flist.sort()
    if len(flist)>0:
        tp,rn,exn = mods.modsFilename(flist[-1])
        nextNum = exn + 1
    else:
        nextNum = 1
    exposure.sequence_number = nextNum
       
    # Instantiate the MODSInstrument() class for instrument interaction
    
    instrument = MODSInstrument(modsID=azcam.db.systemname,side=mods.lbtSide)

    # Instantiate the LBTTelescope() class for LBT TCS interaction

    telescope = LBTTelescope(iifInst="mods",side=mods.lbtSide)
        
    # command server

    cmdserver = CommandServer()
    cmdserver.port = cmdport
    azcam.log(f"Starting cmdserver - listening on port {cmdserver.port}")
    azcam.db.api.initialize()
    cmdserver.start()

    # web server

    if 0:
        webserver = WebServer()
        webserver.logcommands = 0
        webserver.port = 2403
        webserver.start()

    # azcammonitor

    if 0:
        azcam.db.monitor.register()

    # debug for Mike Lesser - disabled instrument and telescope interaction 
    
    instrument.is_enabled=0
    telescope.is_enabled=0

    # finish

    azcam.log(f"{option} configuration complete")


# start

setup()

from azcam.cli import *

