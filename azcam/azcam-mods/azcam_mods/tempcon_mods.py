"""
Contains the TempConMODS class.

base on TempConArchon() class which is itself built on TempCon.

Like TempConArchon() we keep most of the TempCon base class and
only overload define_keywords() and get_temperatures, changing
the keywords we assign to the two temperatures, and both the
heater board and IDs in __init__() to the MODS versions

Updated: 2025 July 23 [rwp/osu]

"""

import azcam
import azcam.exceptions
from azcam.tools.tempcon import TempCon


class TempConMODS(TempCon):
    '''
    Defines the Archon temperature control tool for the MODS dewars
    
    There are two temperature sensors:
     * ccdtemp - CCD detector temperature 
     * basetemp - temperature at the base of the CCD mount where the old finger from the LN2 reservoir attaches
     
    These are sensors 0 and 2, respectively.
    The heater board is MOD1 in the MODS archons
    '''
    

    def __init__(self, tool_id="tempcon", description=None):
        super().__init__(tool_id, description)

        self.num_temp_reads = 1
        self.heaterx_board = "MOD1"

        self.temperature_ids = [0, 2]  # ccdtemp, basetemp

        return

    def define_keywords(self):
        '''
        Defines and resets the tempcon keywords

        Returns
        -------
        None.

        We redefine the header keywords associated with the sensors
        for the detector temperature controller for MODS.
        
        We rename CAMTEMP to CCDTEMP to conform to long-standing MODS
        and LBT practice.
        
        We rename DEWTEMP to BASETEMP because in MODS, this sensor
        is located at the connection between the base of the detector
        mount and the cold finger from the dewar to the right-angle
        MODS detector box. There is a separate DEWTEMP sensor attached to 
        the LN2 reservoir and readout through the MODS HEB electronics not the 
        Archon.
        
        '''

        self.set_keyword("CCDTEMP", 0.0, "CCD detector temperature [deg C]", "float")
        self.set_keyword("BASETEMP", 0.0, "CCD detector mount base temperature [deg C]", "float")

        return

    def get_temperature(self, temperature_id=0):
        """
        Read a camera temperature.
        TemperaureID's are:
        0 => TEMPA
        1 => TEMPB
        2 => TEMPC
        """

        if not self.is_enabled:
            return -999.9

        if not self.is_initialized:
            return -999.9

        if not azcam.db.tools["controller"].heater_board_installed:
            return self.bad_temp_value

        if not azcam.db.tools["controller"].is_reset:
            return self.bad_temp_value

        temperature_id = int(temperature_id)
        if temperature_id == 0:
            Address = f"{self.heaterx_board}/TEMPA"
        elif temperature_id == 1:
            Address = f"{self.heaterx_board}/TEMPB"
        elif temperature_id == 2:
            Address = f"{self.heaterx_board}/TEMPC"
        else:
            raise azcam.exceptions.AzcamError("bad temperature_id in get_temperature")

        # Don't read hardware while exposure is in progess
        flag = azcam.db.tools["exposure"].exposure_flag
        if flag != azcam.db.tools["exposure"].exposureflags["NONE"]:
            return self.last_temps[temperature_id]

        # read temperature
        avetemp = 0
        for _ in range(self.num_temp_reads):
            temp = float(azcam.db.tools["controller"].get_status()[Address])
            avetemp += temp
        temp = avetemp / self.num_temp_reads

        temp = self.apply_corrections(temp, temperature_id)

        # make nice float
        temp = float(int(temp * 1000.0) / 1000.0)

        # use some limits
        if temp > 100.0 or temp < -300.0:
            temp = -999.9

        # save temp
        self.last_temps[temperature_id] = temp

        return temp
