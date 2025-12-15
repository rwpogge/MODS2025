"""
Contains the base TempCon class.
"""

from typing import Union, List, Optional

import azcam
from azcam_console.tools.console_tools import ConsoleTools


class TempconConsole(ConsoleTools):
    """
    Temperature controller tool for consoles.
    Usually implemented as the "tempcon" tool.
    """

    def __init__(self) -> None:
        super().__init__("tempcon")

    def set_control_temperature(
        self, control_temperature: float, temperature_id: int = 0
    ) -> Optional[str]:
        """
        Set the control temperature (set point).
        Args:
            control_temperature: control temperature in Celsius.
            temperature_id: temperature sensor identifier
        """

        return azcam.db.api.set_control_temperature(control_temperature, temperature_id)

    def get_control_temperature(self, temperature_id: int = -1) -> Union[str, float]:
        """
        Get the control temperature (set point).
        Args:
            temperature_id: temperature sensor identifier
        Returns:
            control_temperature: control temperature
        """

        return azcam.db.api.get_control_temperature(temperature_id)

    def get_temperatures(self) -> List[float]:
        """
        Return all system temperatures.
        Returns:
            temperatures: list of temperatures read
        """

        return azcam.db.api.get_temperatures()

    def get_temperature(self, temperature_id: int = 0) -> float:
        """
        Returns a system temperature.
        Args:
            temperature_id: temperature sensor identifier
        Returns:
            temperature: temperature read
        """

        return azcam.db.api.get_temperature(temperature_id)
