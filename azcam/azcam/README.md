# AzCam

AzCam is a software framework for the acquisition and analysis of image data from scientific imaging systems as well as the control of instrumentation. It is intended to be customized for specific hardware, observational, and data reduction requirements.

Operation is based on the concept of *tools* which are the interfaces to both hardware and software code.  Examples of tools are *instrument* which controls instrument hardware, *telescope* which interfaces to a telescope, *linearity* which acquires and analyzes images to determine sensor linearity, and *exposure* which controls a scientific observation by interfacing with a variety of other tools. As an example, the *exposure* tool may move a telescope and multiple filter wheels, control a camera shutter, operate the camera by taking an exposure, display the resultant image, and begin data reduction of that image.

The *azcam* python package currently supports Astronomical Research Cameras, Inc. Gen3, Gen2, and Gen1 CCD controllers, Magellan Guider controllers, STA Archon controllers, and CMOS cameras using ASCOM. It also supports a variety of temperature controollers, telecopes, and image displays.

AzCam is not appropriate for consumer-level cameras and is not intended to have a common API across all systems. It's primary design principle is to allow interfacing to a wide variety of custom instrumentation which is required to acquire and analyze scientific image data.

## MODS2025 Project

For the MODS2025 detector controller upgrade, we have made a working copy of the original azcam code from Mike Lesser's GitHub (https://github.com/mplesser)
and are modifying the system for the MODS instruments. We are starting by trying to make no changes to the core `azcam` code, and focusing all our development
on the `azcam-mods` code, with the hope that we do not have to disturb the core code, at least at the outset.

MODS is being updated to use STA Archon controllers for the CCDs (replacing the now 20-year old OSU controllers which are reaching end of life), and 
all computers are being updated to AlmaLinux 9.x servers, consolidating the data-taking system from 34U of rack space to 16U (including spares), and 
modernizing integration with mountain telemetry systems among other changes. 


## Documentation

See https://azcam.readthedocs.io.

See https://github.com/mplesser/azcam-tool.git for the standard GUI used by most telescope observers.

See https://github.com/mplesser/azcam-console.git for a python package which supports a local or remote command line interface and sensor characterization tools.

## Installation Example

Example from Mike Lesser's GitHub version for reference:

```shell
git clone https://github.com/mplesser/azcam
git clone https://github.com/mplesser/azcam-mods
git clone https://github.com/mplesser/azcam-console [**optional**]
python -m pip install -e ./azcam
python -m pip install -e ./azcam-mods
[python -m pip install -e ./azcam-console]
```
