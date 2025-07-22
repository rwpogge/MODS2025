# ISLUtils - ISL utility library

**Updated: 2025 July 2 [rwp/osu]**

Makes `libislutils.a`

## Overview

`libislutils` comprises a number of low-level functions used throughout
the MODS data-taking system. "ISL" in the name is historical.  While
the original programmer envisioned this would become software
infrastructure used by all instruments built the ISL starting
with MODS, the vision was greater than the reality.  The origins of
much of this code is real-time (or quasi-realtime) code written by
the original programmer when he worked at Arecibo and NASA/GSFC before
coming to OSU.  It has code for using shared memory, semaphores (both
mechanisms used by some multithreaded applications for managing access
to shared resources and synchronization among the threads), and for
runtime environment (rte) code for process control.  There is also
code for messaging queues, signal handling

Only a subset of this code is used in the actual MODS instrument control system. It contains many ideas that were never fully realized, as the original programmer was thinking in terms of
relatively high-speed (~1 kHz) real-time hardware used in radio astronomy, whereas faint-object spectroscopy at visible light with CCDs is very slow by comparison (minutes at the
fastest).

All of it undocumented by the original programmer, and with only minimal comments in the code proper.  We'll update documentation as we proceed through handover to LBTO.

Nonetheless, because elements of these routines are critical to the operation of the MODS instrument control system code, retiring them would require a fundamental overhaul of the system which is not likely to happen given available resources.

## What *is* useful?

Principal among these is the use of shared memory for holding runtime configuration and state information used by the `mmcServer` and `agwServer` apps that operate all MODS mechanisms (mmc) and provide an observatory-facing interface to the MODS acquisition, guide, and wavefront sensing (AGw) camera system that is used by the LBT telescope control system (TCS) to operate the AGw units embedded inside both MODS.

`setup_ids()` in `setup_ids.c` is critical to this, it must be run by all code that uses the MODS shared memory sector.

We'll flesh this out more after we get past the initial hurdles of the MODS2025 controller upgrade and port of the data-taking system to AlmaLinux 9 systems in the summer/fall of 2025.


