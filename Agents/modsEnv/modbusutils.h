#ifndef MODBUSUTILS_H
    //libmodbus include file.
    #include <modbus.h>

    //Headers necessary for the utility functions.
    #include <errno.h>
    #include <stdio.h>
    #include <unistd.h>

    //Utility functions
    int wagoSetGet(int setGet, char *wagoAddr, int regAddr, int regLen, uint16_t regData[]);
#endif