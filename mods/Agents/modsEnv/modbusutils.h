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

  int wagoSetGetRegisters(int setGet, char *wagoAddr, int regAddr, int regLen, uint16_t regData[]);
  int wagoSetGetCoils(int setGet, char *wagoAddr, int coilAddr, int coilLen, uint8_t coilData[]);
#endif
