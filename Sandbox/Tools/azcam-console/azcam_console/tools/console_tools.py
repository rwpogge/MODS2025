"""
*azcam_console.tools.console_tools* contains the `ConsoleTools` base tool class.
"""

from typing import Optional

import azcam
import azcam.exceptions
import azcam_console.console
import azcam.sockets
from azcam.tools.tools import Tools

from azcam_console.server_comm import ServerCommunication
from azcam_console.api import API


class ConsoleTools(Tools):
    """
    Common methods for most console tools and server communication.
    """

    def __init__(self, name: str) -> None:
        """
        Args:
            name (str): name of this tool.
        """

        super().__init__(name)

        self.objname = name

        #: verbosity for debug, > 0 is more verbose
        self.verbosity = 0

    def initialize(self) -> None:
        """
        Initialize this tool.
        """

        return azcam.db.api.command(f"{self.objname}.initialize")

    def reset(self) -> None:
        """
        Reset this tool.
        """

        return azcam.db.api.command(f"{self.objname}.reset")

    def update_header(self):
        """
        Update the header of a tool.
        This command usually reads hardware to get the lastest keyword values.
        """

        return azcam.db.api.command(f"{self.objname}.update_header")

    def read_header(self):
        """
        Returns the current header.

        Returns:
            list of header lines: [Header[]]: Each element Header[i] contains the sublist (keyword, value, comment, and type).
        """

        return azcam.db.api.command(f"{self.objname}.read_header")

    def set_keyword(
        self,
        keyword: str,
        value: str,
        comment: str = "none",
        typestring: str = "none",
    ) -> Optional[str]:
        """
        Set a keyword value, comment, and type.
        Args:
            keyword: keyword
            value: value of keyword
            comment: comment string
            typestring: one of 'str', 'int', 'float,  'none'
        """

        if type(value) == str and " " in value:
            value = f'"{value}"'

        if " " in comment:
            comment = f'"{comment}"'

        s = f"{self.objname}.set_keyword {keyword} {value} {comment} {typestring}"

        return azcam.db.api.command(s)

    def get_keyword(self, keyword: str) -> str:
        """
        Return a keyword value, its comment string, and type.
        Comment always returned in double quotes, even if empty.

        Args:
            keyword: name of keyword
        Returns:
            list of [keyword, comment, type]
        """

        return azcam.db.api.command(f"{self.objname}.get_keyword {keyword}")

    def delete_keyword(self, keyword: str) -> Optional[str]:
        """
        Delete a keyword from a header.

        Args:
            keyword: name of keyword
        """

        return azcam.db.api.command(f"{self.objname}.delete_keyword {keyword}")

    def get_keywords(self) -> list:
        """
        Return a list of all keyword names.

        Returns:
            list of all keywords
        """

        return azcam.db.api.command(f"{self.objname}.get_keywords")

    def get_string(self) -> str:
        """
        Returns the entire header as a formatted string.

        Returns:
            single formatted string of keywords, values, and comments.
        """

        lines = ""

        header = self.read_header()
        for telem in header:
            line = telem[0] + " " + str(telem[1]) + " " + str(telem[2]) + "\n"
            lines += line

        return lines


def create_console_tools() -> None:
    """
    Creates the console tools.
    """

    from azcam_console.tools.controller import ControllerConsole
    from azcam_console.tools.exposure import ExposureConsole
    from azcam_console.tools.instrument import InstrumentConsole
    from azcam_console.tools.telescope import TelescopeConsole
    from azcam_console.tools.tempcon import TempconConsole

    exposure = ExposureConsole()
    controller = ControllerConsole()
    instrument = InstrumentConsole()
    tempcon = TempconConsole()
    telescope = TelescopeConsole()

    azcam.db.server = ServerCommunication()
    azcam.db.cli["server"] = azcam.db.server

    # api
    azcam.db.api = API()
    azcam.db.cli["api"] = azcam.db.api

    return
