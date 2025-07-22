# MODS Tcl/Tk Apps

MODS Tcl/Tk Apps used by the data-taking system

## Important apps

 * bimcsGUI - blue channel IMCS "radar screen" monitor GUI
 * rimcsGUI - red channel IMCS "radar screen" monitor GUI
 * blueQC - blue channel IMCS quad cell raw data monitor GUI
 * redQ - red channel IMCS quad cell raw data monitor GUI
 * vueinfo.c - access to MODS shm sector for Tcl/Tk apps above
 * vuecalq.c and what_help.c - used by vueinfo.c

## A brief history

The Tcl/Tk development effort dates from early in the MODS project at OSU circa 2005 intended 
to support lab integration and testing. An early concept was for the MODS UIs to be coded in 
Tcl/Tk, but when LBTO adopted Qt as the GUI building framework, much of the work was abandoned 
in place.  The `images/` and `libs/` folders here contain the remains of that effort.

The only products we use in the "flight" MODS system are the Tcl/Tk tooks developed to visualize 
real-time performance of the Image Motion Compensation System (IMCS, aka the flexure compensation 
system).  These proved so useful at the observatory we have retained them.  The `vueinfo` program
provides the interface between the Tcl/Tk GUIs and the data in the MODS data-taking system shared 
memory sector (see `modsalloc` elsewhere in this repository).

Future updates of this repository will determine what of the legacy code may be retired and
removed.
