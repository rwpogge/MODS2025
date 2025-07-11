"""
AzCam environment for the LBT MODS CCD cameras

Updated: 2025 July 10 [rwp/osu]
"""

from importlib import metadata

__version__ = metadata.version(__package__)
__version_info__ = tuple(int(i) for i in __version__.split(".") if i.isdigit())
