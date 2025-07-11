"""
Python process start file
"""

import subprocess

OPTIONS = "-mods"
# OPTIONS = "-testdewar"
CMD = f"ipython --ipython-dir=~/data/ipython --profile azcamserver -i -m azcam_mods.server -- {OPTIONS}"

p = subprocess.Popen(
    CMD,
    creationflags=subprocess.CREATE_NEW_CONSOLE,
)
