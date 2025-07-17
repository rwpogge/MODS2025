import lbto.iif as iif
import math
import os

side = "left"
client = "pySim.client"

# Read in the list of TCS header entries

ddFile = f"modsDD_{side}.txt"
f = open(ddFile,"r")
lines = f.readlines()
f.close()

# build the dictionary of DD to FITS header mapping
#  ddDict[ddName] = [fitsKey,dtype]

ddDict = {}
for line in lines:
    line = line.strip()
    if line.startswith("#") or len(line)==0:
        continue
    bits = line.split()
    ddDict[bits[0].strip()] = [bits[1].strip(),bits[2].strip()]

ddList = list(ddDict.keys())

# Setup the IIF interface

proxy = iif.model['PROXIES'].get("mods")

tcs = iif.iifproxy(proxyName=f"py_mods_{side}",instrumentID=proxy['instrument'],
                   focalStation=proxy['focalstation'],side=side,
                   config_client=f"/home/dts/Config/IIF/{client}")

# get the TCS data

tcsData = tcs.GetParameter(ddList)

# generate set_keyword() calls

for i, key in enumerate(ddList):
    keyword = ddDict[key][0]
    dtype = ddDict[key][1]
    val = tcsData[i]
    if dtype == 'str':
        print(f'mods.set_keyword({keyword},\"{val}\")')
    elif dtype == 'int':
        print(f'mods.set_keyword({keyword},{int(val)})')
    elif dtype == 'float':
        print(f'mods.set_keyword({keyword},{float(val)})')
    elif dtype == 'float-as':
        deg = float(val)/3600.0
        print(f'mods.set_keyword({keyword},{deg})')
    elif dtype == 'float-rad':
        deg = math.degrees(float(val))
        print(f'mods.set-keyword({keyword},{deg})')

