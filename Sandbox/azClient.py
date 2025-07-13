#
# a stupid simple TCP client to talk to the azcam server on port 2402
#
# this is for testing client/server interaction with the azcam-mods
# development
#
# Use: ipython -i azClient.py
#
#      cs = openAzCam()
#      rep = azcmd("mods.somecmd arg arg",cs)
#      print(repStr(rep))
#      closeAzCam(cs)
#
# R. Pogge, OSU Astronomy Dept.
# pogge.1@osu.edu
#
# 2025 July 12
#
import socket
import select

def openAzCam():
    sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    sock.connect(('localhost',2402))
    return sock

def closeAzCam(sock):
    sock.close()

def azcmd(cmd,sock,msgLen=1024,timeout=5.0):
    cmdStr = cmd
    if not cmd.endswith("\n"):
        cmdStr += "\n"
    print(f"IC>AZ {cmd}")
    sock.send(bytes(cmdStr,'UTF-8'))
    ready = select.select([sock],[],[],timeout)
    if ready[0]:
        rep = sock.recv(msgLen)
        repStr = rep.decode('UTF-8')
        print(f"AZ>IC {repStr.strip()}")
        return repStr.strip()
    else:
        raise TimeoutError(f"socket read timed out after {timeout:.1f} seconds")

def procRep(repStr):
    if repStr.startswith("OK"):
        sep,code,msg = repStr.partition("OK")
        return msg.strip()
    elif repStr.startswith("ERROR:"):
        sep,code,msg = repStr.partition("ERROR:")
        return msg.strip()
    else:
        print(f'Unknown status tag: {repStr}')
        return

