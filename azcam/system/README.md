# MODS azcam system files

**Updated: 2026 Feb 26 [rwp/osu]**

See the [Release Notes](releases.md) for details.

We restructured the configuration for azcam for MODS to segregate
system configuration files and logs from raw imaging data.  This is to
help streamline our configuration management (4 Archon controllers
across 2 instruments in the same rack), and to make getting at raw
science image transparent without exposing critical system
configuration files.

We use `system/` as the root folder for all system-specific files
 * `parameters/` for azcam server parameter files
 * `templates/` for FITS header templates and IIF data dictionary files
 * `archon/` for the Archon controller files (.acf and .ncf)

In a change from the usual config, we don't have a `logs/` folder in
the `azcam/system` tree.  Instead, log files are sent to
`/home/Logs/azcam` on each

Each MODS instance has its own directory:
 * `MODS1B` - MODS1 Blue Channel
 * `MODS1R` - MODS1 Red Channel
 * `MODS2B` - MODS2 Blue Channel
 * `MODS2R` - MODS2 Red Channel

In addition, there is a separate entry for the MODS lab test dewar
 * `test` - MODS lab test dewar

## Parameters

Parameter initialization files for the azcam server and console
apps. Each MODS instrument has its own set of parameters.  `<modsID>`
= (MODS1B/R,MODS2B/R):
 * `server_<modsID>.ini` - azcam server parameters for `<modsID>`
 * `console_<modsID>.ini` - azcam console parameters for `<modsID>`

## Templates

Header and other templates used by the azcam server.  The FITS header
template is the static header structure for each `<modsID>` system.
The azcam server will add various other parameters as it will, and
populate the headers with dates, times, and specific data like actual
exposure time, total dark time, etc.

 * `header_<modsID>.txt` - static exposure info FITS header template for `<modsID>` channel.
 * `instHdr_<modsID>.txt` - LBT IIF data dictionary to Instrument FITS header keyword mapping used by `instrument_mods.py` for `<modsID>` channel.
 * `modsTCS_<side>.txt` - LBT IIF data dictionary to TCS FITS header keyword mapping used by `telescope_lbt.py`. <side> is left (SX) or right (DX), same for both channels in a single MODS

## Archon Control Files

An Archon controller uses `.acf` or `.ncf` files for the detailed
detector readout configuration. There may be many test or alternative
files in this folder, but the `<modsID>.ncf` file will be considered
the "flight" configuration.  Use symbolic links or judicious copying
(careful!) to enable an alternative or test configuration if needed.

### Lab Test Dewar

The Lab Test Dewar with the MODS e2v electrical sample CCD has
different serial clock wiring than the flight dewars, and so its
Archon control files are incompatible for use with the MODS science
CCDS.

Figuring this out was the rate-limiting step to getting the new
system up on its feet.

### MODS Science Dewars

Mike Lesser in 2025 November/December worked to get the Archons
working with the MODS science CCDs with the MODS on their handling
carts on the floor in the LBT high-bay area.  This resulted in a major
overhaul of all of the `.ncf` files. These were released for use with
MODS to resume on-telescope recommissioning in December.

## CCD configuration details

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

#### MODS1B
HeaterX board control parameters
 * `HEATERATARGET=-120` - set point
 * `HEATERAP=15` - PID P parameter (D and I are 0)
 * `HEATERALIMIT=8.0` volts
 * `HEATERAIL=1000`
 
#### MODS1R
HeaterX board control parameters
 * `HEATERATARGET=-100` - set point
 * `HEATERAP=15` - PID P parameter (D and I are 0)
 * `HEATERALIMIT=8.0` volts
 * `HEATERAIL=1000`

#### MODS2B
HeaterX board control parameters
 * `HEATERATARGET=-106` - set point
 * `HEATERAP=15` - PID P parameter (D and I are 0)
 * `HEATERALIMIT=8.0` volts
 * `HEATERAIL=1000`
 
#### MODS2R
HeaterX board control parameters
 * `HEATERATARGET=-115` - set point
 * `HEATERAP=15` - PID P parameter (D and I are 0)
 * `HEATERALIMIT=8.0` volts
 * `HEATERAIL=1000`



