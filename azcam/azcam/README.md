# AzCam

Updated: 2025 Aug 18

AzCam is a software framework for the acquisition and analysis of image data from scientific imaging systems as well as the control of instrumentation. It is intended to be customized for specific hardware, observational, and data reduction requirements.

Operation is based on the concept of *tools* which are the interfaces to both hardware and software code.  Examples of tools are *instrument* which controls instrument hardware, *telescope* which interfaces to a telescope, *linearity* which acquires and analyzes images to determine sensor linearity, and *exposure* which controls a scientific observation by interfacing with a variety of other tools. As an example, the *exposure* tool may move a telescope and multiple filter wheels, control a camera shutter, operate the camera by taking an exposure, display the resultant image, and begin data reduction of that image.

The *azcam* python package currently supports Astronomical Research Cameras, Inc. Gen3, Gen2, and Gen1 CCD controllers, Magellan Guider controllers, STA Archon controllers, and CMOS cameras using ASCOM. It also supports a variety of temperature controllers, telescopes, and image displays.

AzCam is not appropriate for consumer-level cameras and is not intended to have a common API across all systems. It's primary design principle is to allow interfacing to a wide variety of custom instrumentation which is required to acquire and analyze scientific image data.

## MODS2025 Project

For the MODS2025 detector controller upgrade, we have made a working copy of the original azcam code from Mike Lesser's GitHub (https://github.com/mplesser)
and are modifying the system for the MODS instruments. We are starting by trying to make no changes to the core `azcam` code, and focusing all our development
on the `azcam-mods` code, with the hope that we do not have to disturb the core code, at least at the outset.  The exception will be if we find actual bugs in the
core code that inhibit function, not extensions to the functionality.

MODS is being updated to use STA Archon controllers for the CCDs (replacing the now 20-year old OSU controllers which are reaching end of life), and 
all computers are being updated to AlmaLinux 9.x servers, consolidating the data-taking system from 34U of rack space to 16U (including spares), and 
modernizing integration with mountain telescope control and data telemetry systems among other changes. 

### MODS Notes

 * 2025 Aug 18 - minor changes after return to OSU, continued lab testing and tweaking based on notes from the LBTO run.
 * 2025 Aug 1 - extensive testing with the live Archon + lab test dewar in the LBTO Tucson lab.  Still have not needed to modify the core code beyond the issue noted below. 
 * 2025 July 25 - made first change in core code, fixed bug in ``azcam/azcam/header.py``, the `read_file()` method used <code>break</code> in the header template read loop if a # comment or blank line instead of <code>continue</code>, so the template read was truncated at the first comment or blank line. 

## Documentation

See https://azcam.readthedocs.io.

See https://github.com/mplesser/azcam-tool.git for the standard GUI used by most telescope observers.

See https://github.com/mplesser/azcam-console.git for a python package which supports a local or remote command line interface and sensor characterization tools.

## Installation
Copy from the MODS2025 repository (/make sure there is no existing /home/dts/azcam folder or if so, back it up first:
```shell
cd /home/dts
cp -r ~/MODS2025/azcam .
```
then
```
cp /home/dts/azcam
python -m pip install -e ./azcam       <- core azcam server
python -m pip install -e ./azcam-mods  <- custom MODS azcam server
```

## Running

For test purposes, run the MODS azcam server in an interactive ipython shell
```shell
ipython -i -m azcam_mods.server -- -mods1b
```
which starts MODS azcam for the MODS1B detector system.
