"""
# database_console.py

Contains the azcam database class for azcamconsole.

There is only one instance of this class which is referenced as `azcam.db` and contains
temporary data for this current process.
"""

from azcam.database import AzcamDatabase
from azcam_console.parameters_console import ParametersConsole
from azcam_console.server_comm import ServerCommunication


class AzcamDatabaseConsole(AzcamDatabase):
    """
    The azcam database class.
    """

    parameters: ParametersConsole

    server: ServerCommunication
