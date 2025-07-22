"""
API for azcam
"""

import typing

import azcam


class API(object):
    """
    AzCam API class.
    """

    def __init__(self):

        self.mock = 0
        """True for mock mode which uses no hardware"""

    def initialize(self, mock: int = 0):
        """
        Initialize the API.

        Args:
            mock: flag set true for development with no hardware

        """

        self.mock = mock
        self.parameters = azcam.db.parameters

        try:
            self.exposure = azcam.db.tools["exposure"]
        except Exception:
            pass
        try:
            self.tempcon = azcam.db.tools["tempcon"]
        except Exception:
            pass
        try:
            self.instrument = azcam.db.tools["instrument"]
        except Exception:
            pass
        try:
            self.telescope = azcam.db.tools["telescope"]
        except Exception:
            pass
        try:
            self.controller = azcam.db.tools["controller"]
        except Exception:
            pass
        try:
            self.focus = azcam.db.tools["focus"]
        except Exception:
            pass

        self.is_initialized = 1

        return

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

        return self.parameters.get_par(parameter, subdict)

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

        return self.parameters.set_par(parameter, value, subdict)

    def save_pars(self) -> None:
        """
        Writes the par_dict to the par_file using current values.
        """

        return self.parameters.save_pars()

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

        return self.exposure.expose(exposure_time, imagetype, title)

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

        return self.exposure.expose1(exposure_time, image_type, image_title)

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

        return self.exposure.sequence(number_exposures, flush_array_flag, delay)

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

        return self.exposure.sequence1(number_exposures, flush_array_flag, delay)

    def test(self, exposure_time: float = 0.0, shutter: int = 0):
        """
        Make a test exposure.

        Args:
            exposure_time: exposure time in seconds
            shutter: 0 for closed and 1 for open
        """

        return self.exposure.test(exposure_time, shutter)

    def flush(self, cycles: int = 1):
        """
        Flush/clear sensor.

        Args:
            cycles: number of times to flush the sensor
        """

        return self.exposure.flush(cycles)

    def set_filename(self, filename: str):
        """
        Set the filename components based on a simple filename.

        Args:
            filename: complete filename to be set
        """

        return self.exposure.set_filename(filename)

    def initialize_exposure(self):
        """
        Initialize exposure.
        """

        return self.exposure.initialize()

    def reset_exposure(self):
        """
        Reset exposure.
        """

        return self.exposure.reset()

    def get_roi(self, roi_num: int = 0) -> list:
        """
        Returns a list of the ROI parameters for the roi_num specified.
        Currently only one ROI (0) is supported.

        Args:
            roi_num: ROI number to return

        Returns: list format is (first_col,last_col,first_row,last_row,col_bin,row_bin).
        """

        return self.exposure.get_roi(roi_num)

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

        return self.exposure.set_roi(
            first_col, last_col, first_row, last_row, col_bin, row_bin, roi_num
        )

    def roi_reset(self):
        """
        Resets detector ROI values to full frame, current binning.
        """

        return self.exposure.roi_reset()

    def get_format(self) -> list:
        """
        Return the current detector format parameters.
        """

        return self.exposure.get_format()

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

        return self.exposure.set_format(
            ns_total,
            ns_predark,
            ns_underscan,
            ns_overscan,
            np_total,
            np_predark,
            np_underscan,
            np_overscan,
            np_frametransfer,
        )

    def abort(self):
        """
        Abort an operation in progress.
        """

        return self.exposure.abort()

    def pause(self):
        """
        Pause an exposure inegration (only) in progress.
        """

        return self.exposure.pause()

    def resume(self):
        """
        Resume a paused exposure.
        """

        return self.exposure.resume()

    def set_shutter(self, state: int = 0, shutter_id: int = 0):
        """
        Open or close a shutter.

        :param state:
        :param shutter_id: Shutter ID flag

          * 0 => controller default shutter.
          * 1 => instrument default shutter.
        """

        state = int(state)
        shutter_id = int(shutter_id)

        return self.exposure.set_shutter(state, shutter_id)

    def read_header_file(self, filename):
        """
        Read header file located on the server machine.
        """

        return self.exposure.read_header_file(filename)

    def get_exposuretime(self) -> float:
        """
        Return current exposure time in seconds.

        Returns:
            exposure_time: exposure time in seconds.
        """

        return self.exposure.get_exposuretime()

    def set_exposuretime(self, exposure_time: float):
        """
        Set current exposure time.

        Args:
            exposure_time: exposure time in seconds.
        """

        return self.exposure.set_exposuretime(exposure_time)

    def get_exposuretime_remaining(self) -> float:
        """
        Return remaining exposure time in seconds.

        Returns:
            exposure_time_remaining: exposure time remaining in seconds.
        """

        return self.exposure.get_exposuretime_remaining()

    def get_pixels_remaining(self) -> int:
        """
        Return number of remaining pixels to be read (counts down).

        Returns:
            pixels_remaining: number of pixels remaining to be readout.
        """

        return self.exposure.get_pixels_remaining()

    def get_status(self) -> dict:
        """
        Return a variety of system status data in one dictionary.

        Returns:
            data: dictionary of exposure related data
        """

        return azcam.db.tools["exposure"].get_status()

    def set_image_title(self, title: str):
        """
        Set the image title.

        Args:
            title: image title
        """

        return self.exposure.set_image_title(title)

    def get_image_title(self):
        """
        Return the image title.
        """

        return self.exposure.get_image_title()

    def set_image_type(self, imagetype: str = "zero"):
        """
        Set image type for an exposure.

        Args:
            imagetype: system defined, and typically includes: zero, object, dark, flat.
        """

        return self.exposure.set_image_type(imagetype)

    def get_image_type(self) -> str:
        """
        Get current image type for an exposure.
        imagetype is system defined, and typically includes:
        zero, object, dark, flat.

        Returns:
            image_type: image type string
        """

        return self.exposure.get_image_type()

    def get_image_types(self) -> list[str]:
        """
        Return a list of valid imagetypes.

        Returns:
            image_types: list of valid iamge types
        """

        return self.exposure.get_image_types()

    # *************************************************************************
    #   temperatures
    # *************************************************************************

    def get_temperatures(self) -> list[float]:
        """
        Return all system temperatures.

        Returns:
            temperatures: list of temperatures read
        """

        return self.tempcon.get_temperatures()

    def set_control_temperature(
        self, temperature: float | None = None, temperature_id: int = -1
    ) -> None:
        """
        Set the control temperature (set point).

        Args:
            temperature: control temperature in Celsius. If not specified, use saved value
            temperature_id: control temperature sensor number
        """

        return self.tempcon.set_control_temperature(temperature, temperature_id)

    def get_control_temperature(self, temperature_id: int = -1) -> float:
        """
        Get the control temperature (set point).

        Args:
            temperature_id: temperature sensor identifier

        Returns:
            control_temperature: control temperature
        """

        return self.tempcon.get_control_temperature(temperature_id)

    # *************************************************************************
    #   instruments
    # *************************************************************************

    def get_filters(self, filter_id=0):
        """
        Return a list of all available/loaded filters.
        Args:
            filter_id: filter mechanism ID
        """

        return self.instrument.get_filters(filter_id)

    def get_filter(self, filter_id=0):
        """
        Return the current/loaded filter, typically the filter in the beam.
        Args:
            filter_id: filter mechanism ID
        """

        return self.instrument.get_filter(filter_id)

    def set_filter(self, filter_name, filter_id=0):
        """
        Set the current/loaded filter, typically the filter in the beam.
        Args:
            filter_name: filter name to set. Could be a number or filter name.
            filter_id: filter mechanism ID
        """

        return self.instrument.set_filter(filter_name, filter_id)

    def get_wavelengths(self, wavelength_id: int = 0):
        """
        Returns a list of valid wavelengths.
        Used for filter and LED based systems.
        Args:
            wavelength_id: wavelength mechanism ID
        """

        return self.instrument.get_wavelengths(wavelength_id)

    def get_wavelength(self, wavelength_id: int = 0):
        """
        Returns the current wavelength.
        Args:
            wavelength_id: wavelength mechanism ID
        """

        return self.instrument.get_wavelength(wavelength_id)

    def set_wavelength(self, wavelength: typing.Any, wavelength_id: int = 0):
        """
        Set the current wavelength, typically for a filter or grating.
        Args:
            wavelength: wavelength value to set. Could be a number or filter name.
            wavelength_id: wavelength mechanism ID
        """

        return self.instrument.set_wavelength(wavelength, wavelength_id)

    # *************************************************************************
    #   focus
    # *************************************************************************
    def focus_initalize(self):
        """
        Initialize focus routine.
        """

        return self.focus.initialize()

    def focus_abort(self):
        """
        Abort focus exposure.
        """

        return self.focus.abort()

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

        return self.focus.set_pars(
            exposure_time,
            number_exposures,
            focus_step,
            detector_shift,
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

        return self.focus.run(
            exposure_time, number_exposures, focus_step, detector_shift
        )
