"""
*azcam_console.console* is imported to define console mode, create the azcamconsole parameters dictionary, and define a logger.
"""

import azcam
from azcam.logger import AzCamLogger
from azcam_console.parameters_console import ParametersConsole
from azcam_console.database_console import AzcamDatabaseConsole


def setup_console():

    azcam.mode = "console"
    azcam.db = AzcamDatabaseConsole()  # overwrite default db
    azcam.db.parameters = ParametersConsole()
    azcam.db.logger = AzCamLogger()
    azcam.log = azcam.db.logger.log  # to allow azcam.log()


setup_console()
