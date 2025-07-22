#!/bin/sh
# the next line restarts using wish \
exec wish "$0" "$@"
#########################################################
# TkGetDir Sample Program
#
# Daniel Roche, <daniel.roche@bigfoot.com>
#########################################################

source getDir.tcl
namespace import tkgetdir::*

option add *highlightThickness 0
#option add *font 12x24

wm geometry . 300x150

entry .e0
button .b0 -text "Select Directory" -command {
    .e0 delete 0 end
    .e0 insert 0 [tk_getDirectory -initialdir /tmp -title sample \
	    -listbg #C0C0C0 ]

}

button .b1 -text "Quit" -command exit

pack .e0 -fill x -padx 10 -pady 10
pack .b0 -fill x -padx 10 -pady 10
pack .b1 -side bottom -padx 10 -pady 10
