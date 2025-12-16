"""
Contains the base Telescope class.
"""

import azcam
from azcam_console.tools.console_tools import ConsoleTools


class TelescopeConsole(ConsoleTools):
    """
    Telescope tool for consoles.
    Usually implemented as the "telescope" tool.
    """

    def __init__(self) -> None:
        super().__init__("telescope")

    def get_focus(self, focus_id: int = 0) -> float:
        """
        Get the current telescope focus position.
        Args:
            focus_id: focus sensor ID flag
        """

        reply = azcam.db.api.command(f"telescope.get_focus {focus_id}")

        return float(reply)

    def set_focus(
        self,
        focus_value: float,
        focus_id: int = 0,
        focus_type: str = "absolute",
    ) -> None:
        """
        Set the telescope focus position. The focus value may be an absolute position
        or a relative step if supported by hardware.
        Args:
            focus_value: focus position
            focus_id: focus sensor ID flag
            focus_type: focus type (absolute or step)
        """

        azcam.db.api.command(
            f"telescope.set_focus {focus_value} {focus_id} {focus_type}"
        )

        return
