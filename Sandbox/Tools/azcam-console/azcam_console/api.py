import typing

import azcam
import azcam.exceptions
import azcam.utils


class API(object):
    """
    API class for console to communicate with azcamserver.
    """

    def __init__(self) -> None:
        pass

    def command(self, command: str):
        """
        Send a command to azcamserver API and return the reply.
        """

        reply = azcam.db.server.command(command)

        return reply

    # *************************************************************************
    #   parameters
    # *************************************************************************

    def get_par(self, parameter: str, subdict: str | None = None) -> typing.Any:
        """
        Return the current attribute value of a parameter in the parameters dictionary.
        If subdict is not specified then the default sub-dictionary is used.

        Args:
            parameter: name of the parameter
            subdict: name of the sub-dictionary containing the parameter

        Returns:
            value: value of the parameter
        """

        return self.command(f"get_par {parameter} {subdict}")

    def set_par(
        self, parameter: str, value: typing.Any = "None", subdict: str | None = None
    ) -> None:
        """
        Set the value of a parameter in a parameters dictionary.
        Also attempts to set actual attribute value.

        Args:
            parameter: name of the parameter
            value: value of the parameter. Defaults to None.
            subdict: name of sub-dictionary in which to set paramater
        """

        return self.command(f"set_par {parameter} {value} {subdict}")

    def save_pars(self) -> None:
        """
        Writes the par_dict to the par_file using current values.
        """

        return self.command(f"save_pars")

    # *************************************************************************
    #   temperatures
    # *************************************************************************

    def get_temperatures(self) -> list[float]:
        """
        Return all system temperatures.

        Returns:
            temperatures: list of temperatures read
        """

        temps = self.command(f"get_temperatures")

        if type(temps) == list:
            reply = [float(x) for x in temps]
        else:
            reply = [float(temps)]

        return reply

    def set_control_temperature(
        self, temperature: float | None = None, temperature_id: int = -1
    ) -> None:
        """
        Set the control temperature (set point).

        Args:
            temperature: control temperature in Celsius. If not specified, use saved value
            temperature_id: control temperature sensor number
        """

        if temperature is None:
            temperature = "None"

        return self.command(f"set_control_temperature {temperature} {temperature_id}")

    def get_control_temperature(self, temperature_id: int = -1) -> float:
        """
        Get the control temperature (set point).

        Args:
            temperature_id: temperature sensor identifier

        Returns:
            control_temperature: control temperature
        """

        temp = self.command(f"get_control_temperature {temperature_id}")

        return float(temp)

    # *************************************************************************
    #   exposures
    # *************************************************************************

    def expose(self, exposure_time: float, imagetype: str = "", title: str = ""):
        """
        Make a complete exposure.

        Args:
            exposure_time: exposure time in seconds
            imagetype: type of exposure ('zero', 'object', 'flat', ...)
            title: image title.
        """

        return self.command(f"expose {exposure_time} {imagetype} {title}")

    def expose1(
        self, exposure_time: float, image_type: str = "", image_title: str = ""
    ):
        """
        Make a complete exposure with immediate return to caller.

        Args:
            exposure_time: exposure time in seconds
            image_type: type of exposure ('zero', 'object', 'flat', ...)
            image_title: image title, usually surrounded by double quotes
        """

        return self.command(f"expose1 {exposure_time} {image_type} {image_title}")

    def sequence(
        self, number_exposures: int = 1, flush_array_flag: int = -1, delay: float = -1
    ):
        """
        Take an exposure sequence.
        Uses pre-set exposure time, image type and image title.

        Args:
            number_exposures: number of exposures to make.
            flush_array_flag: defines detector flushing:
                -1 => current value defined by exposure.exposure_sequence_flush [default]
                0 => flush for each exposure
                1 => flush after first exposure only
                2 => no flush
            delay: delay between exposures in seconds
                -1 => no change
        """

        return self.command(f"sequence {number_exposures} {flush_array_flag} {delay}")

    def sequence1(
        self, number_exposures: int = 1, flush_array_flag: int = -1, delay: float = -1
    ):
        """
        Take an exposure sequence.
        Uses pre-set exposure time, image type and image title.

        Args:
            number_exposures: number of exposures to make.
            flush_array_flag: defines detector flushing:
                -1 => current value defined by exposure.exposure_sequence_flush [default]
                0 => flush for each exposure
                1 => flush after first exposure only
                2 => no flush
            delay: delay between exposures in seconds
                -1 => no change
        """

        return self.command(f"sequence1 {number_exposures} {flush_array_flag} {delay}")

    def test(self, exposure_time: float = 0.0, shutter: int = 0):
        """
        Make a test exposure.

        Args:
            exposure_time: exposure time in seconds
            shutter: 0 for closed and 1 for open
        """

        return self.command(f"test {exposure_time} {shutter}")

    def flush(self, cycles: int = 1):
        """
        Flush/clear sensor.

        Args:
            cycles: number of times to flush the sensor
        """

        return self.command(f"flush {cycles}")

    def set_filename(self, filename: str):
        """
        Set the filename components based on a simple filename.

        Args:
            filename: complete filename to be set
        """

        return self.command(f"set_filename {filename}")

    def initialize_exposure(self):
        """
        Initialize exposure.
        """

        return self.command(f"initialize_exposure")

    def reset_exposure(self):
        """
        Reset exposure.
        """

        return self.command(f"reset_exposure")

    def get_roi(self, roi_num: int = 0) -> list:
        """
        Returns a list of the ROI parameters for the roi_num specified.
        Currently only one ROI (0) is supported.

        Args:
            roi_num: ROI number to return

        Returns: list format is (first_col,last_col,first_row,last_row,col_bin,row_bin).
        """

        reply = self.command(f"get_roi {roi_num}")

        return [int(x) for x in reply]

    def set_roi(
        self,
        first_col: int = -1,
        last_col: int = -1,
        first_row: int = -1,
        last_row: int = -1,
        col_bin: int = -1,
        row_bin: int = -1,
        roi_num: int = 0,
    ):
        """
        Sets the ROI values for subsequent exposures.
        Currently only one ROI [0] is supported.
        These values are for the entire focal plane, not just one detector.
        All values are in unbinned coordinates.

        Args:
            first_col: first column
            last_col: last  column
            first_row: first row
            last_row: last row
            col_bin: : column binning
            row_bin: row binning
            roi_num: ROI number [0]
        """

        return self.command(
            f"set_roi {first_col} {last_col} {first_row} {last_row} {col_bin} {row_bin} {roi_num}"
        )

    def roi_reset(self):
        """
        Resets detector ROI values to full frame, current binning.
        """

        return self.command(f"roi_reset")

    def get_format(self) -> list:
        """
        Return the current detector format parameters.
        """

        reply = self.command(f"get_format")

        return [int(x) for x in reply]

    def set_format(
        self,
        ns_total: int,
        ns_predark: int,
        ns_underscan: int,
        ns_overscan: int,
        np_total: int,
        np_predark: int,
        np_underscan: int,
        np_overscan: int,
        np_frametransfer: int,
    ) -> None:
        """
        Set the sensor format for subsequent exposures.
        Must call set_roi() after using this command and before starting exposure.

        Args:
            ns_total: number of visible columns.
            ns_predark: number of physical dark underscan columns.
            ns_underscan: desired number of desired dark underscan columns.
            ns_overscan: number of dark overscan columns.
            np_total: number of visible rows.
            np_predark: number of physical dark underscan rows.
            np_underscan: number of desired dark underscan rows.
            np_overscan: number of desired dark overscan rows.
            np_frametransfer: number rows to frame transfer shift.
        """

        return self.command(
            f"set_format {ns_total} {ns_predark} {ns_underscan} {ns_overscan} {np_total} {np_predark} {np_underscan} {np_overscan} {np_frametransfer}"
        )

    def abort(self):
        """
        Abort an operation in progress.
        """

        return self.command(f"abort")

    def pause(self):
        """
        Pause an exposure inegration (only) in progress.
        """

        return self.command(f"pause")

    def resume(self):
        """
        Resume a paused exposure.
        """

        return self.command(f"resume")

    def set_shutter(self, state: int = 0, shutter_id: int = 0):
        """
        Open or close a shutter.

        Args:
            state: 1 to open shutter or 0 to close
            shutter_id: shutter ID flag

                * 0 => controller default shutter.
                * 1 => instrument default shutter.
        """

        return self.command(f"set_shutter {state} {shutter_id}")

    def read_header_file(self, filename):
        """
        Read header file located on the server machine.
        """

        return self.command(f"read_header_file {filename}")

    def get_exposuretime(self) -> float:
        """
        Return current exposure time in seconds.

        Returns:
            exposure_time: exposure time in seconds.
        """

        reply = self.command(f"get_exposuretime")

        return float(reply)

    def set_exposuretime(self, exposure_time: float):
        """
        Set current exposure time.

        Args:
            exposure_time: exposure time in seconds.
        """

        return self.command(f"set_exposuretime {exposure_time}")

    def get_exposuretime_remaining(self) -> float:
        """
        Return remaining exposure time in seconds.

        Returns:
            exposure_time_remaining: exposure time remaining in seconds.
        """

        reply = self.command(f"get_exposuretime_remaining")

        return float(reply)

    def get_pixels_remaining(self) -> int:
        """
        Return number of remaining pixels to be read (counts down).

        Returns:
            pixels_remaining: number of pixels remaining to be readout.
        """

        reply = self.command(f"get_pixels_remaining")

        return int(reply)

    def get_status(self) -> dict:
        """
        Return a variety of system status data in one dictionary.

        Returns:
            data: dictionary of exposure related data
        """

        # ToDo: parse string to dict
        reply = self.command(f"get_status")

        return reply

    def set_image_title(self, title: str):
        """
        Set the image title.

        Args:
            title: image title
        """

        title = azcam.utils.quoter(title)

        return self.command(f"set_image_title {title}")

    def get_image_title(self):
        """
        Return the image title.
        """

        reply = self.command(f"get_image_title")

        if type(reply) == list:
            reply = " ".join(reply)

        return reply

    def set_image_type(self, imagetype: str = "zero"):
        """
        Set image type for an exposure.

        Args:
            imagetype: system defined, and typically includes: zero, object, dark, flat.
        """

        return self.command(f"set_image_type {imagetype}")

    def get_image_type(self) -> str:
        """
        Get current image type for an exposure.
        imagetype is system defined, and typically includes:
        zero, object, dark, flat.

        Returns:
            image_type: image type string
        """

        return self.command(f"get_image_type")

    def get_image_types(self) -> list[str]:
        """
        Return a list of valid imagetypes.

        Returns:
            image_types: list of valid iamge types
        """

        return self.command(f"get_image_types")

    # *************************************************************************
    #   instruments
    # *************************************************************************

    def get_filters(self, filter_id=0):
        """
        Return a list of all available/loaded filters.
        """

        return self.command(f"get_filters {filter_id}")

    def set_filter(self, filter_name: str, filter_id: int = 0) -> typing.Optional[str]:
        """
        Set instrument filter position.

        :param filter_name: filter value to set
        :param filter_id: filter ID flag
        """

        return self.command(f"set_filter {filter_name} {filter_id}")

    def get_filter(self, filter_id: int = 0) -> str:
        """
        Get instrument filter position.

        :param filter_id: filter ID flag (use negative value for a list of all filters)
        """

        return self.command(f"get_filter {filter_id}")

    # ***************************************************************************
    # wavelengths
    # ***************************************************************************
    def set_wavelength(
        self, wavelength: float, wavelength_id: int = 0, nd: int = -1
    ) -> typing.Optional[str]:
        """
        Set wavelength, optionally changing neutral density.

        :param wavelength: wavelength value, may be a string such as 'clear' or 'dark'
        :param wavelength_id: wavelength ID flag
        :param nd: neutral density value to set
        """

        return self.command(f"set_wavelength {wavelength} {wavelength_id}")

    def get_wavelength(self, wavelength_id: int = 0) -> float:
        """
        Get instrument wavelength.

        :param wavelength_id: wavelength ID flag  (use negative value for a list of all wavelengths)
        """

        reply = float(self.command(f"get_wavelength {wavelength_id}"))

        return reply

    # *************************************************************************
    #   focus
    # *************************************************************************
    def focus_initalize(self):
        """
        Initialize focus routine.
        """

        return self.command(f"focus_initialize")

    def focus_abort(self):
        """
        Abort focus exposure.
        """

        return self.command(f"focus_abort")

    def focus_set_pars(
        self,
        exposure_time: float,
        number_exposures: int = 7,
        focus_step: float = 30,
        detector_shift: int = 10,
    ):
        """
        Set focus related parameters.
        Args:
            number_exposures: Number of exposures in focus sequence.
            focus_step: Number of focus steps between each exposure in a frame.
            detector_shift: Number of rows to shift detector for each focus step.
            exposuretime: Exposure time i seconds.

        """

        return self.command(
            f"focus_set_pars {exposure_time} {number_exposures} {focus_step} {detector_shift}"
        )

    def focus_run(
        self,
        exposure_time: float,
        number_exposures: int,
        focus_step: float,
        detector_shift: int,
    ):
        """
        Execute the focus sequence.
        If focus.set_pars() was previously called then those values are used and input here is ignored.
        Args:
            exposure_time: Exposure time in seconds.
            number_exposures: Number of exposures in focus sequence.
            focus_step: Number of focus steps between each exposure in a frame.
            detector_shift: Number of rows to shift detector for each focus step.
        """

        return self.command(
            f"focus_run {exposure_time} {number_exposures} {focus_step} {detector_shift}"
        )
