'''
dataMan - MODS data post-processor agent

UDP socket server demo

'''

import os
import sys
import socket

dmHost = 'localhost'
dmPort = 10301 # for blue, 10302 for red

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
    
   if len(cmdStr)>0:
      print(f"{addr[0]}:{addr[1]}>> {cmdStr}")        

   if cmdStr.lower() == 'quit':
      break

print("Done: dataMan server demo shutdown")
s.close()
