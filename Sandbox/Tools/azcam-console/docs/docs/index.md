# Home

azcam-console is an azcam client designed for use in a python command line environment.

AzCam is a software framework for the acquisition and analysis of image data from scientific imaging systems as well as the control of instrumentation. It is intended to be customized for specific hardware, observational, and data reduction requirements.

AzCam is not appropriate for consumer-level cameras and is not intended to have a common API across all systems. It's primary design principle is to allow interfacing to a wide variety of custom instrumentation which is required to acquire and analyze scientific image data.

## Installation

- Move to azcam install folder such as `/azcam` or `~/azcam`.
- `git clone https://github.com/mplesser/azcam-console`
- pip install -e azcam-console

## Links

  - Main links
    - [AzCamConsole documentation](https://azcam-console.readthedocs.io) - this document
    - [AzCam documentation](https://azcam.readthedocs.io)
    - [GitHub repos](https://github.com/mplesser)

  - Code details and links
    - [Code Docs](autocode.md)
    - [Focus Command -- console](autocode/focus_console.md)

## Tools

AzCam is based on the concept of *tools* which are the interfaces to both hardware and software code.  Examples of tools are *instrument* which controls instrument hardware, *telescope* which interfaces to a telescope, *linearity* which acquires and analyzes images to determine sensor linearity, and *exposure* which controls a scientific observation by interfacing with a variety of other tools. As an example, the *exposure* tool may move a telescope and multiple filter wheels, control a camera shutter, operate the camera by taking an exposure, display the resultant image, and begin data reduction of that image. There are many supported tools within azcam.

## Operation

There are three main operational modes of AzCam:

  1. A server, usually called *azcamserver*, which communicates directly or indirectly with system hardware.

  2. A console, usually called *azcamconsole*, which is typically implemented as an IPython command line interface that communicates with *azcamserver* over a socket connection.  It is used to acquire and analyze image data through the command line and python code.

  3. Applications, which are usually client programs that communicate with *azcamserver* over sockets or the web API.

While there are multiple pythonic ways to access the various tools in code, they are always accessible from the *database* object `db`, which can always be accessed as `azcam.db`. For example, when defined, the `controller` tool can be accessed as `db.tools["controller"]` and the `qe` tool can be accessed as `db.tools["qe"]`.  In most environments these tool are mapped directly into the command namespace, so in practice commands are executed directly as `object.method`, e.g. `exposure.expose(2.5, "dark", "dark image")`.

Tools defined on the server side may or may not be available as remote commands from a client. 

In *azcamconsole*, the available tools usually communication from the console to an *azcamserver* application over a socket interface.  These client-side tools may only expose a limited set of methods as compared to the server-side tools. So while the command `exposure.reset` may be available from a client the command `exposure.set_video_gain` may not be.  These less commonly used commands are still accessible, but only with lower level code such as `server.command("controller.set_video_gain 2")` where `server` is a client's server communication tool (when allowed by the server configuration).

As an specific example, the code below can be used to set the current system wavelength and take an exposure.  For this example, it is assumed here that the *azcam-itl* environment package has been installed.

```python
# client-side (azcamconsole)
import azcam
import azam_itl.console
instrument = azcam.db.tools["instrument"]
exposure = azcam.db.tools["exposure"]
instrument.set_wavelength(450)
wavelength = instrument.get_wavelength()
print(f"Current wavelength is {wavelength}")
exposure.expose(2., 'flat', "a 450 nm flat field image")
```

*azcamconsole* may also be called in a manner similar to:

```python
python -m azcam_itl.console - -- -system LVM
```

Other examples:
```python
azcam azcam_itl.console -system DESI
```

Example configuration code may be found in the various environment packages with names like `console.py`.

When working in a command line environment, it is often convenient to import commonly used commands into the CLI namespace. This provides direct access to objects and tools such as *db*, *exposure*, *controller*, and various pre-defined shortcuts. To do this, after configuring the environment, execute the commandfrom `from azcam.cli import *`.

And then the code above could be executed as:
```python
from azcam.cli import *
instrument.set_wavelength(450)
exposure.expose(2., 'flat', "a 450 nm flat field image")
```

## Testers

The *testers* subpackage contains classes which are used to acquire and analyze sensor data for the purpose of characterizing sensor performance.

See [testers](testers.md).

## Code Documentation
Much of the python code is autodocumented from the internal doc strings. See [Code Docs](autocode.md).

## Shortcuts
When using IPython, auto parenthesis mode allows typing commands without 
requiring the normal python syntax of ``command(par1, par2, ...)``. The equivalent 
shortcut/alias syntax is ``command par1 par2``. With IPython in this mode all commands may 
use this syntax.

There are some simple but useful command line commands which can be optionally installed within
console or server applications as typing shortcuts. Shortcuts are intended for command line use only
and can be found in the `db.cli` dictionary (see `azcam.shortcuts`). Examples include:

  * *sav* - save the current parameters to the parameter file
  * *gf* - try and go to current image folder.
  * *sf* - try and set the image folder to the current directory.
  * *bf* - browse for a file or folder using a GUI.

## Scripts
Scripts are functions contained in python code modules of the same name. They may be loaded automatically during enviroment configuration and can be found in the `db.scripts` dictionary. Scripts defined on the server side are not available as remote commands. An example script to measure system pressures might be:

```python
get_pressures(2.0, "get_pressures.log", 1)
```

## Configuration Folders
There are two important folders which are defined by most environments:

  * *systemfolder* - the main folder where configuration code is located. It is often the root of the environment's python package.
  * *datafolder* - the root folder where data and parameters are saved. Write access is required. It is often similar to `/data/sytemfolder`.

## Help
AzCam is commonly used with IPython.  Help is then available by typing `?xxx`, `xxx?`, `xxx??` or `help(xxx)` where `xxx` is an AzCam class, command, or object instance.

## External Links

Useful external links include:
  
 * IPython <https://ipython.org>
 * Python programming language <https://www.python.org>
