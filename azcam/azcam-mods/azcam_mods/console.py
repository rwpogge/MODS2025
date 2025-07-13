"""
Setup method for LBTO MODS azcam console

Usage example:
  python -i -m azcam_mods.console -- -mods1r
  
  Updated: 2025 July 13 [rwp/osu]
 
"""

import os
import sys
import threading

import azcam
import azcam.utils
import azcam_console.console
from azcam_console.tools.console_tools import create_console_tools
import azcam_console.shortcuts
import azcam_console.tools.console_tools
from azcam.tools.ds9display import Ds9Display
#from azcam_console.tools.focus import FocusConsole


def setup():
    option = "menu"
    # command line args
    try:
        i = sys.argv.index("-datafolder")
        datafolder = sys.argv[i + 1]
    except ValueError:
        datafolder = "/home/data"

    try:
        i = sys.argv.index("-mods1r")
        option = "MODS1R"
        modsID = option
    except ValueError:
        pass

    try:
        i = sys.argv.index("-mods1b")
        option = "MODS1B"
        modsID = option
    except ValueError:
        pass

    try:
        i = sys.argv.index("-mods2r")
        option = "MODS2R"
        modsID = option
    except ValueError:
        pass

    try:
        i = sys.argv.index("-mods2b")
        option = "MODS2B"
        modsID = option
    except ValueError:
        pass

    try:
        i = sys.argv.index("-test")
        option = "test"
        modsID = "mods"
        datafolder = "/home/mods/data"
    except ValueError:
        pass
        
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
    
    # configuration menu
    
    menu_options = {"MODS1R": "MODS1R",
                    "MODS1B": "MODS1B",
                    "MODS2R": "MODS2R",
                    "MODS2B": "MODS2B",
                    "MODS test dewar": "test",
                    }

    if option == "menu":
        print(f"{modsID} azcam console startup menu:\n")
        option = azcam.utils.show_menu(menu_options)

    # parameters, templates, etc. live in azcam.db.systemfolder in our version

    if "MODS" in option:
        
        # parameter files are parameters_console_<modsID>.ini in system/<modsID>/parameters/
        
        parfile = os.path.join(azcam.db.systemfolder,
                               "parameters", 
                               f"parameters_console_{option}.ini"
                               )
    elif "test" in option:
        parfile = os.path.join(azcam.db.systemfolder,
                               "parameters",
                               "parameters_console_mods.ini"
                               )

    # start logging

    # logging - in <azcamRoot>/system/logs/server.log for all configurations
    
    logfile = os.path.join(azcamRoot,
                           "system/logs", 
                           "console.log"
                           )
    azcam.db.logger.start_logging(logfile=logfile)
#    logfile = os.path.join(azcam.db.datafolder, "logs", "console.log")
#    azcam.db.logger.start_logging(logfile=logfile)
    azcam.log(f"Configuring console for {azcam.db.systemname}")

    # display

    # display = Ds9Display()
    # dthread = threading.Thread(target=display.initialize, args=[])
    # dthread.start()  # thread just for speed

    # console tools

    create_console_tools()

    # observe

    azcam.log("Loading observe")
    from azcam_console.observe.observe_cli.observe_cli import ObserveCli

    observe = ObserveCli()
    observe.move_telescope_during_readout = 1

    # focus tool

    #focus = FocusConsole()
    #focus.focus_component = "instrument"
    #focus.focus_type = "step"

    # try to connect to azcamserver
    
    ports = [2402, 2412, 2422, 2432, 2442]
    connected = 0
    for port in ports:
        connected = azcam.db.server.connect(port=port)
        if connected:
            break

    if connected:
        azcam.log("Connected to azcamserver")
    else:
        azcam.log("Not connected to azcamserver")

    # parameter file

    azcam.db.parameters.read_parfile(parfile)
    azcam.db.parameters.update_pars()

# start

setup()
from azcam_console.cli import *
