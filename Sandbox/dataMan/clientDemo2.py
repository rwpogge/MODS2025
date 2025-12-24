#
# launch as ipython -i clientDemo.py
#
import sys
import socket

import time

def sendToDM(msg):
    dmHost = "localhost"
    dmPort = 10301
    t0 = time.perf_counter()
    s = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
    s.sendto(msg.encode('utf-8'),(dmHost,dmPort))
    s.close()
    dt = 1000.0*(time.perf_counter() - t0)
    print(f"exec time: {dt:.3f} msec")

sendToDM("proc /home/data/mods1b.20250727.0021.fits")

