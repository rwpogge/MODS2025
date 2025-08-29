'''
dataMan - MODS data post-processor agent

UDP socket server demo

'''

import os
import sys
import socket
import threading
import time

dmHost = 'localhost'
dmPort = 10301 # for blue, 10302 for red

# modProc for threading

def modsProc(filename):
    print(f"   processing {filename}...")
    time.sleep(10)
    print(f"   ...done, {filename} sent to /newdata")
    
# Datagram (udp) socket

print("Starting dataMan server demo")
try:
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    print("  UDP socket created")
except Exception as err:
    print(f"Failed to create UDP socket - {err}")
    sys.exit(-1)

# Bind socket to local host and port

try:
    s.bind((dmHost,dmPort))
except Exception as err:
    print(f"UDP socket binding failed - {err}")
    sys.exit(-1)
        
print(f"  Server started on {dmHost}:{dmPort}")

# Talk to the client until we are killed off

while 1:
    # received data from client
    
    d = s.recvfrom(1024)
    
    cmdStr = d[0].decode('utf-8')
    addr = d[1]
    
    cmdBits = cmdStr.split()
    cmdWord = cmdBits[0]
    if len(cmdBits) > 1:
        cmdArgs = cmdBits[1]
    else:
        cmdArgs = ''
        
    if len(cmdStr)>0:
        print(f">> {cmdStr}")        

        if cmdWord.lower() == 'quit':
            break

        elif cmdWord.lower() == 'proc':
            filename = cmdArgs
            t = threading.Thread(target=modsProc,args=[filename])
            t.start()
            
        else:
            print(f"don't know command {cmdWord}")


print("Done: dataMan threaded server demo shutdown")
s.close()
