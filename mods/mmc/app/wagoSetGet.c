// Platform header

#include <stdio.h>
#include <unistd.h>
#include <errno.h>

// libmodbus include file

#include <modbus.h>

//---------------------------------------------------------------------------
//
// wagoSetGet function
//
// Modbus function 16 (10 hex),
// Preset Multiple Registers/Write Multiple Registers.
//
// Writes values into a sequence of output registers
// (holding registers, 4:00000 table).
//
// @param gs        1=set (write to registers), 0=get (read from registers)
// @param host      The address of the WAGO device that should be queried
// @param slaveAddr Modbus address of slave device or unit identifier (Range: 0 - 255)
// @param startRef  Start reference (Range: 1 - 0x10000)
// @param regArr    Buffer with the data to be sent
// @param refCnt    Number of references to be written (Range: 1-100)
// 
// @return          0 on send success, value or error code on faults
//
// This version rewritten for libmodbus to replace defunct and unsupported FieldTalk 
// code [xc/osu - Jun 2025].
//
// Updates:
//   2025 Oct 4 - added 10ms pause between modsbus_new_tcp() and modsbus_connect()
//                for occasional timing race seen in live testing [rwp/osu]
//   2026 Jan 30 - increased pause to 50ms as seeing condition more often
//                 in an active network environment [rwp/osu]
//
//---------------------------------------------------------------------------

int
wagoSetGet(int gs, char* host, int slaveAddr, int startRef, short regArr[], int refCnt)
{
  
  modbus_t* modbus;
  uint16_t data[refCnt];
  int result = 0;

  // Open a Modbus/TCP connection to the WAGO

  modbus = modbus_new_tcp(host,502);

  usleep(50000); // allow open to finish before connect

  // Connect to the WAGO.  Allow one retry before exiting with an error
  
  if (modbus_connect(modbus) == -1) {
    ierr = modbus_connect(modbus);
    if (ierr < 0) {
      printf("ERROR: Cannot connect to WAGO host %s: %s\n",host,modbus_strerror(errno));
      modbus_free(modbus);
      return -1;
    }
  }

  // Pause to give a slow TCP link a chance to catch up

  usleep(10000);

  // Set the slave device

  if (modbus_set_slave(modbus,slaveAddr) == -1) {
    printf("ERROR: Slave address %d is invalid: %s\n",slaveAddr,modbus_strerror(errno));
    modbus_close(modbus);
    modbus_free(modbus);
    return -1;
  }

  // If setGet = 1 (true), we are writing data to WAGO registers/coils
  
  if (gs) {
    // This function takes an array of shorts, but libmodbus requires an array of uint16_t.
    for(int i=0; i<refCnt; i++) data[i] = static_cast<uint16_t>(regArr[i]);
    
    // Write 'data' to the registers.
    result = modbus_write_registers(modbus,startRef,refCnt,data);
  }
  
  // If setGet = 0 (false), we are writing data to WAGO registers/coils

  else {
    // Read the 'data' from the regsiters.
    result = modbus_read_registers(modbus,startRef,refCnt,data);

    // This function fills an array of shorts, but libmodbus provides an array of uint16_t.
    for(int i=0; i<refCnt; i++) regArr[i] = static_cast<short>(data[i]);
  }

  // close connection and free libmodbus context
  
  modbus_close(modbus);
  modbus_free(modbus);

  // Return status: 0 on success, -1 on errors with errno set by modbus_xxx_registers()
  
  return result;
}
