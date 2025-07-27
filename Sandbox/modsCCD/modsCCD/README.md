# modsCCD - MODS Archon CCD controller agent

**Last Update:** 2025 July 25 [rwp/osu]

**Heritage:** Y4KCam at the CTIO 1m with a Windows AzCamServer and ARC Gen3 (May 2005).

**Language:** C/C++

## Overview

`modsCCD` is an ISIS client application to operate the MODS CCD cameras at LBTO with Archon controllers. The Archons are 
operated by a python `azcam` server running on a Linux workstation that has a fiber network interface board that provides a
dedicated fiber gigabit ethernet link between the Linux host and the Archon controller.

In 2005 we wrote the `y4kcam` software to operate a 4Kx4K CCD camera (aka Y4KCam) with an ARC Gen3 controller at the CTIO Yale 1m 
telescope using an earlier Windows version of AzCamServer. While much has changed in 20 years (mostly for the better) there was 
enough similarity we could use the `y4kcam` code as a foundation on which to build `modsCCD`.  It was not quite a full-gut renovation,
but pretty close. Aside from the diffent syntax of the python `azcam` server commands, because the new server is written in python
and has an open architecture (unlike the older AzCam which was pre-compiled for Windows), we can customize the server to meet us
part way.

The purpose of `modsCCD` is to replicate the function and command syntax of the OSU DOS IC program which has run OSU's CCD 
cameras with Bruce Atwood's controller hardware.  This allows us to integrate `modsCCD` nearly seamlessly into the existing
MODS data taking system (vintage 2010) without having to gut the core data-taking system.  `modsCCD` uses the same IMPv2
messaging protocol (sometimes called "ISIS protocol" for the `isis` message-passing server we use) as the current MODS
instrument control and data-acquisition system.

## Contents

 * main.c
 * clientutils.c
 * commands.c
 * config.c
 * headers: client.h, commands.h
 * build and Makefiles

links to `libazcam.a` in another part of this repository folder with the azcam interface routines.

## Sample status string

This is an M1.BC status string from a pre-2025 DOS IC system for MODS1 Blue
```
STATUS Inst=MODS1B Det=(e2v 8Kx3K) DW=T M1.CBSYNCH=1  Scale=0 DMin=0 DMax=500 DSat=65000 Exp=1200 ImTyp=OBJECT Obj=(SBS0948+532) XBin=1 YBin=1 OverX=0 OverY=0 ROI=(1,8288,1,3088) +Seq +HE TEDPower=ON LEDPower=OFF GaugePower=ON IEBLen=1 Mode=Integrating
```
The new format will be
```
STATUS Inst=MODS1B Det=(e2v 8Kx3K) Exp=1200 ImTyp=OBJECT Obj=(SBS0948+532) XBin=1 YBin=1 ROI=(1,8288,1,3088) CCDTemp=-95.3 BaseTemp=-132.5 Mode=Integrating
```

