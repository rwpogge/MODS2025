#
# a stupid simple TCP client to talk to the azcam server on port 2402
#
# this is for testing client/server interaction with the azcam-mods
# development
#
# Use: ipython -i azClient.py
#
#      az = AzClient()
#      az.startClient()
#      rep = az.cmd("mods.somecmd arg arg")
#      print(repStr(rep))
#      az.stopClient()
#
# R. Pogge, OSU Astronomy Dept.
# pogge.1@osu.edu
#
# 2025 July 12
#
import socket
import select

class AzClient(object):

    def __init__(self):
        self.sock = None
        self.azHost = 'localhost'
        self.azPort = 2402
        self.timeout = 5.0
        self.msgLen = 1024
        self.encoding = 'UTF-8'
        self.repStr = ''

    def startClient(self):
        try:
            self.sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        except Exception as exp:
            print(f"Cannot create socket - {exp}")
            self.sock = None

        try:
            self.sock.connect((self.azHost,self.azPort))
        except Exception as exp:
            print(f"Cannot connect to azcam server - {exp}")
            self.sock.close()
            self.sock = None

    def stopClient(self):
        if self.sock is not None:
            self.sock.close()
            self.sock = None
            print(f"AzCam client stopped")
        else:
            print(f"AzCam client not running, nothing to stop...")

    def cmd(self,cmd,timeout=None):
        if timeout is None:
            timeout = self.timeout
        if self.sock is not None:
            cmdStr = cmd
            if not cmd.endswith("\n"):
                cmdStr += "\n"
            print(f"IC>AZ {cmd}")
            self.sock.send(bytes(cmdStr,self.encoding))
            ready = select.select([self.sock],[],[],timeout)
            if ready[0]:
                rep = self.sock.recv(self.msgLen)
                self.repStr = rep.decode(self.encoding).strip()
                print(f"AZ>IC {self.repStr}")
                return self.repStr
            else:
                raise TimeoutError(f"socket read timed out after {timeout:.1f} seconds")

    def procRep(self,repStr=None):
        if repStr is None:
            repStr = self.repStr
        if repStr.startswith("OK"):
            sep,code,msg = repStr.partition("OK")
            return msg.strip()
        elif repStr.startswith("ERROR:"):
            sep,code,msg = repStr.partition("ERROR:")
            return msg.strip()
        else:
            print(f'Unknown status tag: {repStr}')
            return

# main

az = AzClient()


