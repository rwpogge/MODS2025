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
  
    \param setget   (int) 1 = set (write), 0 = get (read)
    \param wagoAddr (char*) The address of the WAGO device that should be queried.
    \param regAddr  (int) register address (range: 1 - 0x10000)
    \param regLen   (int) register length = number of data values (range: 1-100)
    \param regData  (uint16) array of data to write or read
  
    \return 0 on send success, value or error code on faults
  */
  int wagoSetGetRegisters(int setGet, char *wagoAddr, int regAddr, int regLen, uint16_t regData[]);

  /*!
    \brief Write (set) or read (get) data from WAGO modbus coils

    \param setget   (int) 1 = set (write), 0 = get (read)
    \param wagoAddr (char*) The address of the WAGO device that should be queried.
    \param coilAddr (int) coil address (range: 1 - 0x10000)
    \param coilLen  (int) coil length = number of data values (range: 1-16)
    \param coilData (uint8) array of data to write or read (0 values will write false, 1 values will write true).

    \return 0 on send success, value or error code on faults
  */
  int wagoSetGetCoils(int setGet, char *wagoAddr, int coilAddr, int coilLen, uint8_t coilData[]);
#endif