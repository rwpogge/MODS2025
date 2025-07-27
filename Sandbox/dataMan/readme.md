# dataMan - MODS Data Manager

Development sandbox for a python-based data manager for the Archon MODS 
system that will post-process raw images taken with MODS.

Post-processing steps include
 * fixing fits header issues
 * logging the images
 * pushing the raw images to the data repository
 * converting raw MEF to standard FITS?
 * inserting telemetry metadata (guider, wfs, weather?) as FITS bintable extensions
 * other?

The idea is to let the azcam server continue taking data moving post-processing "offline" from
the azcam server and the modsCCD interface client.  

This is to replace functions performed by the `modslogger` script executed by the `caliban` data
transfer agents in the pre-2025 MODS data-taking system. Faster machines and greater python
FITS handling capability gives us many more options.

## contents

 * `dataMan_server.py` - simple UDP socket server demo, run as `python dataMan_server.py`
 * `clientDemo.py` - simple dm client - run in ipython shell, and use `sendToDM()` to send messages. `quit` will shutdown the server demo


