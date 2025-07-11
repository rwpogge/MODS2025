"""
AzCamMonitor process start file
"""

import subprocess

OPTIONS = "./parameters_monitor_mods.ini"

CMD = f"python -m azcam.monitor -- -configfile {OPTIONS}"
# CMD = f"ipython -i -m azcam.monitor -- -configfile {OPTIONS}"
# CMD = f"ipython -i -m azcam.monitor -- -configfile {OPTIONS}"

p = subprocess.Popen(
    CMD,
    creationflags=subprocess.CREATE_NEW_CONSOLE,
)
