import os
import sys
import socket

from time import sleep

# Version info

version = "igTTY v1.0.3 [2026 May 14]"

# globals

modsIDs = ["MODS1B","MODS1R","MODS2B","MODS2R"]

modsIPs = {"MODS1B":"192.168.139.113",
           "MODS1R":"192.168.139.103",
           "MODS2B":"192.168.139.213",
           "MODS2R":"192.168.139.203"}

modsID = ""

modsPort = 8018  # comtrol RTS-2 default comm port on MODS

ttyChan = 5 # MODS ion gauges are configured for RS485 channel 5

MESSAGE_SIZE = 16 # maximum length string returned by ion gauge
TIMEOUT = 5 # maximum time to wait before aborting socket I/O

# status returned by read ion gauge read commands

statusID = {"IGS":"Gauge Power",
            "KBS":"Keyboard Lock",
            "RD":"Pressure",
            "RE":"Emission Current",
            "RF":"Filament",
            "RS":"Module"}

# -----------------------------------

# Useful functions

def Usage():
    '''
    Print usage message

    Returns
    -------
    None.

    '''
    print("Usage: igTTY [modsID|[ipAddr ipPort]")
    return


def showHelp():
    '''
    show help message

    Returns
    -------
    None.

    '''
    print("\nigTTY commands:")
    print("  quit - disconnect and end session")
    print("  help - this message")
    print("  connect [modsID|ipAddr ipPort] - change ion gauge connection")
    print("  chan [N] - change RS485 channel")
    print("  info - show igTTY status")
    print("  status - gauge status")
    print(" ")
    print("Ion gauge commands:")
    print("  IGS - read gauge status")
    print("  KBS - read keyboard lock/unlock status")
    print("   RD - read vacuum pressure (torr)")
    print("   RE - read emission current (mA)")
    print("   RF - read filament status")
    print("   RS - read module operating status")
    print("  VER - read module firmware version")
    print(" ")
    return


def sendIonCommand(ionSocket, command, twait=0.5):
    '''
    Send a command to the ion gauge

    Parameters
    ----------
    ionSocket : socket object
        active socket file descriptor (see initIonSocket())
    command : string
        ion gauge serial command (case-insensitive)
    twait : float, optional
        pause time between send and receive in seconds. The default is 0.5 sec

    Raises
    ------
    Exception
        returns exception on socket I/O errors

    Returns
    -------
    data : string
        data string returned by the ion gauge
            
    '''
    
    # Send and receive data.

    try:
        ionSocket.send((command).encode())
        sleep(twait)
        data = ionSocket.recv(MESSAGE_SIZE).decode()
    except socket.error as err:
        raise Exception(f"socket.error: {err}")

    # Return the collected data.

    return data


def initIonSocket(address, port, timeout="None",twait=0.5):
    '''
    Initialize socket connection to IG

    Parameters
    ----------
    address : string
        IP address of the ion gauge TCP/RS485 interface
    port : integer
        Port number of the ion gauge TCP/RS485 interface
    timeout : float, optional
        Socket I/O timeout in seconds. The default is "None".
    twait : float, optional
        pause time between send and receive in seconds. The default is 0.5 sec

    Raises
    ------
    Exception
        raises exceptions on socket I/O errors

    Returns
    -------
    ionSocket : socket object
        python socket() object on success.

    '''
    
    # Create the socket

    ionSocket = socket.socket()

    # Set a timeout in seconds

    if (timeout != "None"):
        ionSocket.settimeout(timeout)
    
    # Open the TCP connection.

    try:
        ionSocket.connect((address, port))
        sleep(twait)
    except socket.error:
        ionSocket.close()
        raise Exception("Could not connect to ion-gauge.")

    # wait before sending for socket to open
    
    
    # Attempt initial snd/rcv to test connection.

    try:
        sendIonCommand(ionSocket, "#05VER\r")
    except Exception as err:
        ionSocket.close()
        raise Exception(f"IG socket I/O error - {err}")
    
    # Return the connected socket file descriptor
    
    return ionSocket

#-----------------------------------

# process command line

if len(sys.argv) < 2:
    Usage()
    sys.exit(0)
elif len(sys.argv) > 3:
    Usage()
    sys.exit(1)
elif len(sys.argv) == 2:
    modsID = sys.argv[1].upper()
    if modsID in modsIDs:
        igHost = modsIPs[modsID]
        igPort = modsPort
        promptStr = f"{modsID}> "
    else:
        print(f"Error: unrecognized MODS instance {modsID}, must be one of {modsIDs}")
        Usage()
        sys.exit(1)
else:
    igHost = sys.argv[1]
    igPort = sys.argv[2]
    promptStr = "IG> "

# try to initialize the ion-gauge socket connection

try:
    ionSock = initIonSocket(igHost,igPort,TIMEOUT)
    if len(modsID) == 0:
        print(f"\nConnected to ion gauge on {igHost}:{igPort} on RS485 channel {ttyChan}")
    else:
        print(f"\nConnected to {modsID} ion gauge on {igHost}:{igPort} RS485 channel {ttyChan}")
    igCmd = f"#{ttyChan:02d}VER\r"
    data = sendIonCommand(ionSock,igCmd)
    bits = data.strip().split(" ")
    print(f"Ion gauge version: {bits[1]}")
    print("Type 'help' or '?' to see commands\n")
    
    ionSock.close()
    
except Exception as err:
    print(f"Error: cannot connect - {err}")
    ionSock.close()
    sys.exit(1)

# Command prompt. Expects valid Micro-Ion Plus gauge RS485 command
# if quit, closes socket and exits
    
while(1):
    cmdStr = input(promptStr)
    
    if len(cmdStr) > 0:
        cmdWord = cmdStr.split(" ")[0]
    else:
        cmdWord = "none"
        
    # quit session
    
    if cmdWord.lower() == "quit":
        print("Done: shutting down connection")
        sys.exit(0)

    # show help
    
    elif cmdWord.lower() == "help" or cmdStr == "?":
        showHelp()
    
    # change/show RS485 channel
    
    elif cmdWord.lower().startswith("chan"):
        bits = cmdStr.split(" ")
        if len(bits) == 2:
            try:
                newChan = int(bits[1])
                ttyChan = newChan
                print(f"Reading RS485 channel {ttyChan}")
            except:
                print(f"Error: channel must be integer, gave {newChan}")
        else:
            print(f"Reading RS485 channel {ttyChan}")
    
    # change/show IG gauge port/channel connection
    
    elif cmdWord.lower().startswith("conn"):
        bits = cmdStr.split(" ")
        if len(bits) == 1:
            # report current open port
            if len(modsID) == 0:
                print(f"Connected to ion gauge on {igHost}:{igPort} RS485 channel {ttyChan}")
            else:
                print(f"Connected to {modsID} ion gauge on {igHost}:{igPort} RS485 channel {ttyChan}")
            
        elif len(bits) == 2:
            # open ion gauge by MODS name
            newMODS = bits[1].upper()
            if newMODS in modsIDs:
                modsID = newMODS
                igHost = modsIPs[modsID]
                igPort = modsPort
                print(f"Connecting to {modsID} ion gauge on {igHost}:{igPort}")
                promptStr = f"{modsID}> "
            else:
                print(f"Error: invalid MODS instance {newMODS}, must be one of {modsIDs}")
        elif len(bits) == 3: 
            # open ion gauge by IP address and port
            try:
                modsID = ""
                igPort = int(bits[2])
                igHost = bits[1]
                promptStr = "IG> "
                print(f"Connecting to ion gauge on {igHost}:{igPort}")
            except:
                print(f"ERROR: port ID must be an integer, gave {bits[2]}")
            
        else:
            print(f"ERROR: unrecognized open directive '{cmdStr}'")

    # current connection info
    
    elif cmdWord == "info":
        print(f"\n{version}")
        if len(modsID) == 0:
            print(f"Connected to ion gauge on {igHost}:{igPort} RS485 channel {ttyChan}")
        else:
            print(f"Connected to {modsID} ion gauge on {igHost}:{igPort} RS485 channel {ttyChan}")
        print("")

    # Status summary
    
    elif cmdWord == "status":
        for statCmd in ["IGS","RD","RF","RE","RS","KBS"]:
            sleep(1.5) # pause between commands 
            igCmd = f"#{ttyChan:02d}{statCmd}\r"
            try:
                ionSock = initIonSocket(igHost,igPort,TIMEOUT)
                data = sendIonCommand(ionSock,igCmd)
                igData = data.strip()
                print(f"  {statusID[statCmd]}: {igData[4:]}")
                ionSock.close()
            except Exception as err:
                print(f"ERROR: {err}")
                ionSock.close()
        print(" ")

    # no-op, do nothing
    
    elif cmdWord == "none":
        pass

    
    # Assume a valid MicroIon command - build the command string and send it
    else:
    
        igCmd = f"#{ttyChan:02d}{cmdStr.upper()}\r"
    
        # send it

        try:
            ionSock = initIonSocket(igHost,igPort,TIMEOUT)
            data = sendIonCommand(ionSock,igCmd)
            print(f"{data.strip()}")
            ionSock.close()
        except Exception as err:
            print(f"ERROR: {err}")
            ionSock.close()


sys.exit(0)
