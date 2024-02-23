# ISIS - Integrated Science Instrument Server

Current version: 3.0

Author: R. Pogge, OSU Astronomy Dept (pogge.1@osu.edu)

## Overview

ISIS is a lightweight message-passing server used by the data-taking system of new-generation OSU instruments for interprocess communication and coordination.
The messaging syntax is the ICIMACS Messaging Protocol v2.5 (IMPv2.5) in used in the older DOS-based instrument systems.  Instrument data-taking processes are 
ISIS client nodes, and communicate with each other through ISIS (a few legacy clients can communicate directly with each other, but ideally clients
are non-routing nodes).

The ISIS server application is written in ANSI C, and can communicate
via UDP/IP sockets (standard INET protocol connection-less UDP
datagram sockets) and serial ports.  It provides a simple command-line
interface (based on the GNU readline/history packages), and provides
runtime communication logging.  The application is designed at present
to be single-threaded, though with some modification it can be made
multi-threaded as many of the low-level routines were designed to make
at least an attempt at being thread-safe.

## Package Organization

The organization of the package is as follows:
 * `isisServer` -- ISIS server source code.  Server app is self-contained.
 * `isisClient` -- ISIS client library (libisis.a) source code and library.
 * `Config` -- Runtime configuration files for the server app.
 * `bin` -- Binary executable(s) for the server, and sample wrapper scripts (e.g., how to run an isis server with log rotation)
 * `Doc` -- documentation for the IMPv2.5 protocol.
 * `RELEASE.md` -- Package release notes

## About ISIS

In the OSU data-taking system, ISIS started out in the mid-1990s
to adopt and extend the function of the old "WC" DOS machine dating 
from the late 1980s.  DOS clients connect via serial ports,
eliminating the need for the tcwrappers code that contributed to the
instability of the WC system.  A "WC" host may still be used, it is
treated as yet another ISIS client, though the fact that it and the IC
are "routing" clients does occasionally emerge as a problem: both the
WC and IC "block" when performing critical realtime tasks, making it
possible for communications to hang up.  ISIS is designed to be
specifically non-blocking.

Prospero version 5.x is designed to run as an ISIS client, eliminating
the need for the old ariel program.  More specifically, Prospero is a
prototype of a type of ISIS client known as a "director" - an
interactive meta-client that is aware of other clients and coordinates
their functions.

Caliban has been recast as an ISIS client, but can still be configured
to communicate directly with a WC DOS pc for backwards compatibility).
This required only modification of the low-level transport interface,
and replacing the old curses-based command-line interface with a GNU
readline/history type interface.  Caliban cannot yet connect directly
to an IC, bypassing a WC, because of some file handling functions
still assumed by the WC.

A new type of application introduced with the prototype ISIS system is
the family of programs called "agents".  Agents are autonomous clients
that perform specific real-time and/or "blocking" functions, often
acting as interfaces between the data-taking system and external
applications (e.g., a telescope control system) that do not use our
messaging protocol. Most agents will have little or no command-line
interface (in this, they somewhat resemble Unix daemons).  Agents are
explicitly non-routing, and in the server/client topology sit at the
termini of communications lines.  The first agent introduced was
TCSAgent, a small C program that provides a serial interface to the
PC-TCS system at the CTIO 1.3m telescope, translating the continous
serial telemetry stream into TCS status strings that can be returned
upon request by various ISIS nodes (e.g., in response to "tcstatus"
commands by the IC to build the CCD image header).

A general client library (libisis.a) has been provided to make the
creation of new ISIS client applications easier.

The MODS spectrographs for the LBT are the last major instruments
built entirely on the ICIMACS communication model. All subsequent
instruments were on contracts for other observatories that integrated
the instruments with existing or new observatory data acquisition
systems that were developed outside OSU.

Version 3 of the ISIS server app and client libraries were ported
to AlmaLinux 9.x using ISO g++ compiles, requiring small changes
to adapt to the stricter ISO compiler.
