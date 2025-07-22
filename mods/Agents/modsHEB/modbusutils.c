/*!
  \file modbusutils.c
  \brief Utility functions for working with libmodbus and WAGO fieldbus controllers.
  \author Xander Carroll

  \date 2025 May 7
*/

#include "modbusutils.h"

  /*!
    \brief Write (set) or read (get) data from WAGO modbus registers/coils
  
    \param setget     (int) 1 = set (write), 0 = get (read)
    \param isRegister (int) 1 = register, 0 = coil
    \param wagoAddr   (char*) The address of the WAGO device that should be queried.
    \param addr       (int) register address (range: 1 - 0x10000)
    \param len        (int) register length = number of data values (range: 1-100)
    \param data       (uint8/uint16) array of data to write or read
  
    \return 0 on send success, value or error code on faults
  
    This version rewritten for libmodbus to replace defunct and unsupported proprietary 
    FieldTalk code. [rwp/osu - 2024 Feb 20]
  */
int wagoSetGet(int setGet, int isRegister, char *wagoAddr, int addr, int len, void* data){
  // using libmodbus
  modbus_t *modbus;
  int result;

  // Open a Modbus/TCP connection to the WAGO
  modbus = modbus_new_tcp(wagoAddr,502);
  if (modbus_connect(modbus) == -1) {
    printf("ERROR: Cannot connect to WAGO host %s: %s\n",wagoAddr,modbus_strerror(errno));
    modbus_free(modbus);
    return -1;
  }

  // Slight pause to give a slow TCP link a chance to catch up
  usleep(10000);

  // If setGet = 1 (true), we are writing data to WAGO registers/coils
  if (setGet){
    if(isRegister) result = modbus_write_registers(modbus,addr,len,(uint16_t*)data);
    else result = modbus_write_bits(modbus,addr,len,(uint8_t*)data);
  } 

  // If setGet = 0 (false), we are reading data from WAGO registers/coils
  else{
    if(isRegister) result = modbus_read_registers(modbus,addr,len,(uint16_t*)data);
    else result = modbus_read_bits(modbus,addr,len,(uint8_t*)data);
  } 
    
  // close connection and free libmodbus context
  modbus_close(modbus);
  modbus_free(modbus);

  // Return status: 0 on success, -1 on errors with errno set by modbus_xxx_registers()
  if (result == -1) return -1;
  return 0;
}

int wagoSetGetCoils(int setGet, char *wagoAddr, int coilAddr, int coilLen, uint8_t coilData[]){
  return wagoSetGet(setGet, 0, wagoAddr, coilAddr, coilLen, coilData);
}

int wagoSetGetRegisters(int setGet, char *wagoAddr, int regAddr, int regLen, uint16_t regData[]){
  return wagoSetGet(setGet, 1, wagoAddr, regAddr, regLen, regData);
}