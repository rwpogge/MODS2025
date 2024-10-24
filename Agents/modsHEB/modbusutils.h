/*!
  \file modbusutils.h
  \brief Utility functions for working with libmodbus and WAGO fieldbus controllers.

  \date 2024 July 15
*/

#ifndef MODBUSUTILS_H
  //libmodbus include file.
  #include <modbus.h>
  
  //Headers necessary for the utility functions.
  #include <errno.h>
  #include <stdio.h>
  #include <unistd.h>

  /*!
    \brief Write (set) or read (get) data from WAGO modbus registers
  
    \param setget  (int) 1 = set (write), 0 = get (read)
    \param regAddr (int) register address (range: 1 - 0x10000)
    \param regLen  (int) register length = number of data values (range: 1-100)
    \param regData (uint16) array of data to write or read
  
    \return 0 on send success, value or error code on faults
  
    This version rewritten for libmodbus to replace defunct and unsupported proprietary 
    FieldTalk code. [rwp/osu - 2024 Feb 20]
  */
  int wagoSetGet(int setGet, char *wagoAddr, int regAddr, int regLen, uint16_t regData[]);


  /*!
    \brief Write (set) or read (get) data from WAGO modbus coils

    \param setget   (int) 1 = set (write), 0 = get (read)
    \param coilAddr (int) coil address (range: 1 - 0x10000)
    \param coilLen  (int) coil length = number of data values (range: 1-16)
    \param regData  (uint16) array of data to write or read (0 values will write false, any other value will write true).

    \return 0 on send success, value or error code on faults
  */
  int wagoSetGetCoils(int setGet, char *wagoAddr, int coilAddr, int coilLen, uint16_t coilData[]);
#endif