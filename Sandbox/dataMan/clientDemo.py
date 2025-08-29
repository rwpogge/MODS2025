#
# launch as ipython -i clientDemo.py
#
import sys
import socket

s = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)

dmHost = "localhost"
dmPort = 10301

def sendToDM(msg):
    s.sendto(msg.encode('utf-8'),(dmHost,dmPort))

sendToDM("proc /home/data/mods1b.20250727.0021.fits")

