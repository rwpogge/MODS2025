"""
Setup method for LBTO MODS azcamserver
Usage example:
  python -i -m azcam_mods.server -- -mods1r
  
  Updated: 2025 July 9 [rwp/osu]
  
"""

import os
import sys

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
from azcam.tools.archon.tempcon_archon import TempConArchon
from azcam_mods.detector_mods import detector_mods

from azcam.web.fastapi_server import WebServer

# observing date

import datetime

def obsDate():
    """
    Return the observing date string

    Returns
    -------
    string
        observing date in CCYYMMD format, see description.

    Description
    -----------        
    Returns the observing date in CCYYMMDD format.  We define the
    an "observing date" as running from noon to noon local time.
    
    For example, the observing date for the night starting at sunset
    on 2024 Dec 17 and ending at sunrise on 2024 Dec 18 is 20241217

    We use this for filenames for data and logs.
    """

    if float(datetime.datetime.now().strftime("%H")) < 12.0:  # before noon
        return (datetime.date.today() - datetime.timedelta(days=1)).strftime("%Y%m%d")
    else:
        return datetime.date.today().strftime("%Y%m%d")


# setup

def setup():
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
        modsID = "mods"
        lbtSide = "left"
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
    
    # define folders for the system

    azcam.db.systemname = option

    azcam.db.rootfolder = os.path.abspath(azcamRoot)

    # We segregate systemfolder from datafolder because the latter is 
    # read/write by unprivileged users, but systemfolder contains
    # critical configuration files unprivileged users must not be
    # able to change
    
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
                    "MODS test dewar": "testdewar",
                    }

    if option == "menu":
        print("MODS azcam server startup menu:\n")
        option = azcam.utils.show_menu(menu_options)

    # parameters, templates, etc. live in azcam.db.systemfolder in our version
    
    if "MODS" in option:
        
        # paramteter files are <modsID>_pars.ini in system/<modsID>/parameters/
        
        parfile = os.path.join(azcam.db.systemfolder,
                               "parameters", 
                               f"{option}_pars.ini"
                               )
        
        # FITS header template files are <modsID>_hdr.txt in system/<modsID>/templates/
        
        template = os.path.join(azcam.db.systemfolder, 
                                "templates", 
                                f"{option}_hdr.txt"
                                )
        
        # timing files are <modsID>.acf in system/<modsID>/archon/
        
        timingfile = os.path.join(azcam.db.datafolder,
                                  "archon",
                                  f"{option}.acf",
                                  )
        
        azcam.db.servermode = "archon"
        cmdport = 2402

    # testdewar files live in test
    
    elif "test" in option:
        parfile = os.path.join(azcam.db.systemfolder,
                               "parameters",
                               "parameters_server_mods.ini"
                               )
        template = os.path.join(azcam.db.systemfolder,
                                "templates",
                                "fits_template_mods.txt" 
                                )
        timingfile = os.path.join(azcam.db.systemfolder,
                                  "archon",
                                  "mods_1.acf",
                                  )
        azcam.db.servermode = "MODS"
        cmdport = 2402

    else:
        raise azcam.exceptions.AzcamError("bad system configuration")

    # logging - in <azcamRoot>/system/logs/server.log for all configurations
    
    logfile = os.path.join(azcamRoot,
                           "system/logs", 
                           "server.log"
                           )
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
    
    tempcon = TempConArchon(description="MODS Archon")
    tempcon.temperature_ids = [0, 2]  # camtemp, dewtemp
    tempcon.heaterx_board = "MOD1"
    tempcon.control_temperature = -95.0
    controller.heater_board_installed = 1

    # exposure
 
    exposure = ExposureArchon()
    exposure.filetype = exposure.filetypes["MEF"]
    exposure.image.filetype = exposure.filetypes["MEF"]
    exposure.display_image = 0
    exposure.add_extensions = 1

    exposure.image.focalplane.gains = 4 * [2.0]
    exposure.image.focalplane.rdnoises = 4 * [2.0]

    if remote_host is None:
        pass
    else:
        exposure.send_image = 1
        exposure.sendimage.set_remote_imageserver(remote_host, 6543, "azcam")

    # instrument

    instrument = Instrument()

    # telescope

    telescope = Telescope()

    # system header template

    try:
        system = System("MODS", template)
        system.set_keyword("DETNAME", option, "Detector name")
    except Exception:
        pass

    # detector

    exposure.set_detpars(detector_mods)
    exposure.fileconverter.set_detector_config(detector_mods)

    # filename rules for MODS (some override defaults), 1=True, 0=False
    
    azcam.db.tools["exposure"].test_image = 0
    azcam.db.tools["exposure"].include_sequence_number = 1
    azcam.db.tools["exposure"].auto_increment_sequence_number = 1
    azcam.db.tools["exposure"].overwrite = 0 
    azcam.db.tools["exposure"].autoname = 0
    
    # other defaults
    
    azcam.db.tools["exposure"].auto_title = 0
    
    
    # default filename root pattern is the instrument ID lower case
    # and the current observing date, e.g., mods1b.20251007.  mind the .
    
    azcam.db.tools["exposure"].root = f"{modsID.lower()}.{obsDate()}."
    azcam.db.tools["exposure"].folder = datafolder
    
    # display
    # display = Ds9Display()

    display = Display()

    # MODS customized commands

    from azcam_mods.mods import MODS

    mods = MODS()

    # let the MODS azcam tool know our ID and side
    
    mods.modsID = modsID
    mods.lbtSide = lbtSide
    
    # load the header template
    
    if len(template) > 0 and os.path.exists(template):
        azcam.db.tools["exposure"].header.read_file(template)
        
    # parameter file

    azcam.db.parameters.read_parfile(parfile)
    azcam.db.parameters.update_pars()

    
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

    # finish

    azcam.log(f"{option} configuration complete")


# start
setup()
from azcam.cli import *
