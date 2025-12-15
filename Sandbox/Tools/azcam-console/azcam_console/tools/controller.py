"""
Contains the ControllerConsole classe.
"""

import azcam
from azcam_console.tools.console_tools import ConsoleTools


class ControllerConsole(ConsoleTools):
    """
    Controller tool for consoles.
    Usually implemented as the "controller" tool.
    """

    def __init__(self) -> None:
        super().__init__("controller")

    def set_shutter(self, state: int = 0, shutter_id: int = 0):
        """
        Open or close a shutter.

        Args:
            state: 1 to open shutter or 0 to close
            shutter_id: shutter ID flag

                * 0 => controller default shutter.
                * 1 => instrument default shutter.
        """

        return azcam.db.api.command(f"set_shutter {state} {shutter_id}")

    def set_shutter(self, state: int = 0, shutter_id: int = 0):
        """
        Open or close a shutter.

        :param state:
        :param shutter_id: Shutter ID flag

          * 0 => controller default shutter.
          * 1 => instrument default shutter.
        """
