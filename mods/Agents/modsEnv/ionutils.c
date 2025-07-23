/*!
    \author X. Carroll
    \date 2024 August 6

    A library for interacting with a vaccum ion gauge. Provides functions to connect to the ion gauge
    and send it commands.

    See the header file for instructions.
*/

#include "ionutils.h"

/*!
    \brief Send a command and recieve a response from the ion gauge.
    
    \param socket TCP socket connected to the ion gauge
    \param response string where the response data will be written.
    \param command string with the command to send.
    
    \return 0 on success, -1 on errors.  
    
    Sends the given command to the ion gauge, and stores the response.
*/
int sendIonCommand(int socket, char* response, char* command){
    //Clearing the response string.
    memset(response, '\0', ION_MESSAGE_SIZE);

    //Send and recieve data.
    if(send(socket, command, strlen(command), 0) < 0) return -1;
    if(recv(socket, response, ION_MESSAGE_SIZE, 0) < 0) return -1;

    return 0;
}

/*!
    \brief Create a TCP socket which is connected to an ion-gauge at the given address:port.
    
    \param sock pointer to where the connected socket will be stored.
    \param address string with the IP address to connect to.
    \param port integer with the port to connect to.
    \param timeout integer with the maximum number of seconds that should be spent trying to connect.
    
    \return 0 on success, -1 on errors.  
*/
int initIonSocket(int* sock, char* address, int port, int timeout){
    //Creating the TCP socket.
    (*sock) = socket(AF_INET, SOCK_STREAM, 0);

    //Setting the TCP socket address and port.
    struct sockaddr_in ionAddr;
    ionAddr.sin_family = AF_INET;
    ionAddr.sin_addr.s_addr = inet_addr(address);
    ionAddr.sin_port = htons(port);

    //Setting the timeout.
    struct timeval timeoutStruct; 
    timeoutStruct.tv_sec = timeout;
    timeoutStruct.tv_usec = 0;

    //Variables used for making the socket connection.
    int result, opt;
    
	//Get socket flags.
	if ((opt = fcntl ((*sock), F_GETFL, NULL)) < 0){
		return -1;
	}

	//Set socket non-blocking flag.
	if (fcntl ((*sock), F_SETFL, opt | O_NONBLOCK) < 0){
		return -1;
	}

	//Try to connect with a timeout.
	if((result = connect(*sock, (struct sockaddr*) &ionAddr, sizeof(ionAddr)) < 0)){
		if(errno == EINPROGRESS){
			fd_set wait_set;

			//Make file descriptor set with socket
			FD_ZERO (&wait_set);
			FD_SET ((*sock), &wait_set);

			//Wait for socket to be writable; return after given timeout.
			result = select ((*sock) + 1, NULL, &wait_set, NULL, &timeoutStruct);
		}

    //Connection was successful immediately.
	}else{
		result = 1;
	}

	//Reset the socket non-blocking flag.
	if (fcntl ((*sock), F_SETFL, opt) < 0) {
		return -1;
	}

	//An error occured in connect or select.
	if(result < 0){
		return -1;

    //Select timed out.
	}else if(result == 0){
		errno = ETIMEDOUT;
		return -1;
    
    //No error occured in connect or select.
	}else{
		socklen_t len = sizeof(opt);

        setsockopt((*sock), SOL_SOCKET, SO_RCVTIMEO, (const char*) &timeoutStruct, sizeof(timeoutStruct));
		setsockopt((*sock), SOL_SOCKET, SO_SNDTIMEO, (const char*) &timeoutStruct, sizeof(timeoutStruct));

		//Check for errors in socket layer.
		if(getsockopt ((*sock), SOL_SOCKET, SO_ERROR, &opt, &len) < 0){
			return -1;
		}

		//There was an error.
		if(opt){
			errno = opt;
			return -1;
		}
	}

    //Attempting an initial query to see if the gauge is connected.
    char response[ION_MESSAGE_SIZE];
    if(sendIonCommand((*sock), response, "#05VER\r") < 0){
        close(*sock);
        return -1;
    }

    return 0;
}


//// Wrapper Functions for Common Ion Operations.

/*!
    \brief Connectes to an ion gauge and reads the pressure.
    
    \param address string with the IP address to connect to.
    \param port integer with the port to connect to.
    \param timeout integer with the maximum number of seconds that should be spent trying to connect.
    
    \return the pressure as a float, NULL on errors.  
    
    Opens a connection to the ion gauge, reads the pressure, returns it as a float, and closes the ion connection.
*/
float getIonPressure(char* address, int port, int timeout){
    char responseMessage[ION_MESSAGE_SIZE];     //The ion response message will be collected in this variable.
 
    int sock;           //The TCP socket with a connection to the ion.
    float response;     //The parsed reponse from the ion gauge.

    //Form a TCP connection with the ion.
    if (initIonSocket(&sock, address, port, timeout) < 0){
        printf("ERROR: Could not connect to ion-gauge %s:%d.\n", address, port);
        return NULL;
    }

    //Get the current pressure reading (returned as a string).
    if(sendIonCommand(sock, responseMessage, "#05RD\r") != 0){
        printf("ERROR: Could not send command.\n");
        close(sock);
        return NULL;
    }

    //Convert the response to a float. (Note: there will be four characters before the desired float).
    response = strtof(responseMessage+4, NULL);
    if(response == NULL){
        printf("ERROR: Could not parse ion response.\n");
        close(sock);
        return NULL;
    }

    //Close the socket.
    close(sock);

    //Return success.
    return response;
}
