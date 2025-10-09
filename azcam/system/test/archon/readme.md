# MODS Archon Configuration Files

This is where we keep MODS Archon configuration files (acf and ncf)
developed during lab testing with the Archon and MODS Lab Test
Dewar in Tucson, and later testing at the LBT with the MODS science
detectors and Archons.

These are designed primarily to be used with the `archongui` engineering
tool

## IMPORTANT NOTE - READ THIS NOW!!!

Do not use any of these files on a live system except at the
direct instruction of Rick Pogge at OSU.  These include test
and example configurations that are **NOT** valid for operating
the actual MODS detectors.

These contain "bare metal" instructions to the controller to
operate the CCD.  Changes made without care could damage the
CCD or controller.

## Contents

 * `MODS_Actual.ncf` is the current MODS flight or near-flight testing configuration file, but is formatted for use with the `archongui` program only.  If used with `azcam`, it will fail to load.

 * `MODS_Actual.acf` is an edited ACF file based on `MODS_Actual.ncf` that is edited to use with `azcam`, but will not load with `archongui`.

 * `MODS_Example_2.ncf` is the NCF file used for Tucson lab testing with the MODS Lab Test Dewar system.  It is formatted for use with the `archongui` and will nto work with `azcam`.  `MODS_Actual.xxx` are derived from this template, but there are important differences, so do not use for "flight".
 
 * `mods_test.ncf` is a copy of `MODS_Example_2.ncf` that may be used for rapid testing work without touching the reference copy of the file.

## `MODS_Actual.acf` Archon Configuration File

**Updated: 2025 Oct 9 [rwp/osu]**

The MODS common Archon configuration file is `MODS_Actual.acf`.  We start with the same ACF for all 4 sensors, then may modify individually if needed.
It was derived from NCFs from work with the Lab Test Dewar system in Tucson, and then modified in the Tucson Lab and on the mountain with the
science CCD systems.

### CCD Temperature Control

The HeaterX board is installed in the **MOD10** slot

#### Temperature Sensors

`SENSORA` is the CCD detector, a 100-ohm Pt RTD run at 0.1mA
```
MOD10\SENSORALABEL=CCD
MOD10\SENSORATYPE=2
MOD10\SENSORACURRENT=100000
MOD10\SENSORAFILTER=3
MOD10\SENSORALOWERLIMIT=-150.0
MOD10\SENSORAUPPERLIMIT=50.0
```
`SENSORB` is the CCD mount base, where the cold strap from the LN2 dewar attaches to the detector mount.  It is the same type of RTD as sensor A
```
MOD10\SENSORBLABEL=BASE
MOD10\SENSORBTYPE=2
MOD10\SENSORBCURRENT=100000
MOD10\SENSORBFILTER=3
MOD10\SENSORBLOWERLIMIT=-150.0
MOD10\SENSORBUPPERLIMIT=50.0
```
`SENSORC` is not used.

#### PID Control Parameters

We run the CCD heater from Heater A.  

The CCD heater is a 50-ohm resistor.
We set a heater limit of 7V, which corresponds to ~1 watt max (49/50).  This is a conservative protection value.

The temperature set-point is **-100C**, but can be changed from the `azcam` server.

We only set the proportional gain on the heater, leaving I and D at 0.  An I limit of 1000 was recommended by STA.

```
MOD10\HEATERAENABLE=1
MOD10\HEATERAD=0
MOD10\HEATERAFORCE=0
MOD10\HEATERAFORCELEVEL=0
MOD10\HEATERAI=0
MOD10\HEATERAIL=1000
MOD10\HEATERALABEL=MODS
MOD10\HEATERALIMIT=7.0
MOD10\HEATERAP=30
MOD10\HEATERARAMP=0
MOD10\HEATERARAMPRATE=1
MOD10\HEATERASENSOR=0
MOD10\HEATERATARGET=-100
MOD10\HEATERUPDATETIME=1000
```

Heater B is not used.


### The `Old/` folder:

These are config files of historial interest developed during the early stages
of MODS Archon development.  They are kept here in case we need to revisit some of the earlier work.

 * `MODS_mode_3k.ncf` and `MODS_mode_3K_1.ncf" are files developed while testing ROI and binning modes. **These are here for historical reference only and are never to be used with a live system**

 * `MODS_Example_1.ncf` is an early full-frame readout file that does not include the extended timing multiplier to allow exposure times longer than about 17.4 minutes.  **Do Not Use**

