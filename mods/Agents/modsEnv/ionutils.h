/*!
    \file ionutils.h
    \brief Utility functions for working with the vacuum ion gauge.
  
    A library for interacting with a vaccum ion gauge. Provides functions to connect to the ion gauge
    and send it commands.
  
    Typical operation would be as follows:
      1) initIonSocket(&sock, ...);
      2) sendIonCommand(&sock, response, command);
      3) close(sock);
  
    Wrapper methods are provided for common ion operations that will do these steps for you.
    See the ion documentation for details on how your commands should be formatted. (Don't 
    forget the '\r' ).
  
    \date 2025 July 22
*/

#ifndef IONUTILS_H

    #include <sys/socket.h>
    #include <sys/select.h>
    #include <arpa/inet.h>
    #include <errno.h>
    #include <fcntl.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <unistd.h>

    #define ION_MESSAGE_SIZE 16     //The maximum size string that the ion gauge can return.
    #define ION_PORT 8018

    /*!
        \brief Send a command and recieve a response from the ion gauge.

        \param socket TCP socket connected to the ion gauge
        \param response string where the response data will be written.
        \param command string with the command to send.

        \return 0 on success, -1 on errors.  

        Sends the given command to the ion gauge, and stores the response.
    */
    int sendIonCommand(int socket, char* response, char* command);

    /*!
        \brief Create a TCP socket which is connected to an ion-gauge at the given address:port.

        \param sock pointer to where the connected socket will be stored.
        \param address string with the IP address to connect to.
        \param port integer with the port to connect to.
        \param timeout integer with the maximum number of seconds that should be spent trying to connect.

        \return 0 on success, -1 on errors.  
    */
    int initIonSocket(int* sock, char* address, int port, int timeout);


    //// Wrapper Functions for Common Ion Operations.

    /*!
        \brief Connectes to an ion gauge and reads the pressure.
        
        \param address string with the IP address to connect to.
        \param port integer with the port to connect to.
        \param timeout integer with the maximum number of seconds that should be spent trying to connect.
        
        \return the pressure as a float, NULL on errors.  
        
        Opens a connection to the ion gauge, reads the pressure, returns it as a float, and closes the ion connection.
    */
    float getIonPressure(char* address, int port, int timeout);

#endif