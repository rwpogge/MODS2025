"""
Setup method for 90prime azcamconsole.
Usage example:
  python -i -m azcam_90prime.console
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
from azcam_console.tools.focus import FocusConsole


def setup():
    # command line args
    try:
        i = sys.argv.index("-datafolder")
        datafolder = sys.argv[i + 1]
    except ValueError:
        datafolder = None

    # files and folders
    azcam.db.systemname = "90prime"

    azcam.db.systemfolder = f"{os.path.dirname(__file__)}"

    azcam.db.datafolder = azcam.utils.get_datafolder(datafolder)

    parfile = os.path.join(
        azcam.db.datafolder,
        "parameters",
        f"parameters_console_mods.ini",
    )

    # start logging
    logfile = os.path.join(azcam.db.datafolder, "logs", "console.log")
    azcam.db.logger.start_logging(logfile=logfile)
    azcam.log(f"Configuring console for {azcam.db.systemname}")

    # display
    display = Ds9Display()
    dthread = threading.Thread(target=display.initialize, args=[])
    dthread.start()  # thread just for speed

    # console tools
    create_console_tools()

    # observe
    azcam.log("Loading observe")
    from azcam_console.observe.observe_cli.observe_cli import ObserveCli

    observe = ObserveCli()
    observe.move_telescope_during_readout = 1

    # focus tool
    focus = FocusConsole()
    focus.focus_component = "instrument"
    focus.focus_type = "step"

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
