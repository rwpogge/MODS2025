/*!
  \file modbusutils.c
  \brief Utility functions for working with libmodbus and WAGO fieldbus controllers.

  \date 2024 Oct 24
*/

#include "modbusutils.h"

int wagoSetGet(int setGet, char *wagoAddr, int regAddr, int regLen, uint16_t regData[]){
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

int wagoSetGetCoils(int setGet, char *wagoAddr, int coilAddr, int coilLen, uint16_t coilData[]){
  uint16_t state = 0;

  // If setGet = 1 (true), we are writing data to WAGO coils
  if(setGet){
    for(int i=coilLen-1; i>=0; i--){
      state = state << 1;
      if(coilData[i] != 0) state += 1;
    }

    return wagoSetGet(1, wagoAddr, coilAddr, 1, &state);
  }

  // If setGet = 0 (false), we are reading data from WAGO coils
  else{
    int error = wagoSetGet(0, wagoAddr, coilAddr, 1, &state);
    if(error != 0) return error;

    for(int i=0; i<coilLen; i++){
      *(coilData+i) = ((state & (1 << i)) == (1 << i));
    }
    
    return 0;
  }
}