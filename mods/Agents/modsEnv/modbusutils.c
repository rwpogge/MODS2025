/*!
  \file modbusutils.c
  \brief Utility functions for working with libmodbus and WAGO fieldbus controllers.

  \date 2024 July 15
*/

#include "modbusutils.h"

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
int wagoSetGet(int setGet, char *wagoAddr, int regAddr, int regLen, uint16_t regData[]){
  char responseBufSz[256];
  short readArr[1];

  // using libmodbus
  modbus_t *modbus;
  int result;

  // Open a Modbus/TCP connection to the WAGO
  modbus = modbus_new_tcp(wagoAddr,502);
  if (modbus_connect(modbus) == -1) {
    printf("ERROR: Cannot connect to WAGO host %s: %s",wagoAddr,modbus_strerror(errno));
    modbus_free(modbus);
    return -1;
  }

  // Slight pause to give a slow TCP link a chance to catch up
  usleep(10000);

  // If setGet = 1 (true), we are writing data to WAGO registers
  if (setGet) result = modbus_write_registers(modbus,regAddr,regLen,regData);   // set: write data to the WAGO
  else result = modbus_read_registers(modbus,regAddr,regLen,regData);           // get: read data from the WAGO
    
  // close connection and free libmodbus context
  modbus_close(modbus);
  modbus_free(modbus);

  // Return status: 0 on success, -1 on errors with errno set by modbus_xxx_registers()
  if (result == -1) return -1;
  return 0;
}

