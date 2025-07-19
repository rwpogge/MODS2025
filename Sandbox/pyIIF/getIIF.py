import lbto.iif as iif
import math
import os

side = "left"
client = "pySim.client"

# Read in the list of IIF data dictionary entries we need
# to build the MODS telescope  FITS header entries

ddFile = f"modsTCS_{side}.txt"
f = open(ddFile,"r")
lines = f.readlines()
f.close()

# build the dictionary of DD to FITS header data mapping
#
#  ddDict[ddName] = [fitsKey,dtype,comment]

ddDict = {}
for line in lines:
    line = line.strip()
    if line.startswith("#") or len(line)==0:
        continue
    cbits = line.split("/")
    kbits = cbits[0]
    ddDict[kbits[0].strip()] = [kbits[1].strip(),kbits[2].strip(),cbits[1].strip()]

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
    comment = ddDict[key][2]
    val = tcsData[i]
    if dtype == 'str':
        print(f'mods.set_keyword({keyword},\"{val}\",\"{comment}\")')
    elif dtype == 'int':
        print(f'mods.set_keyword({keyword},{int(val)},\"{comment}\")')
    elif dtype == 'float':
        print(f'mods.set_keyword({keyword},{float(val)},\"{comment}\")')
    elif dtype == 'float-as':
        deg = float(val)/3600.0
        print(f'mods.set_keyword({keyword},{deg},\"{comment}\")')
    elif dtype == 'float-rad':
        deg = math.degrees(float(val))
        print(f'mods.set-keyword({keyword},{deg},\"{comment}\")')

