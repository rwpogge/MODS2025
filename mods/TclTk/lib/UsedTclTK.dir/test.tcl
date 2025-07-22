#!/usr/bin/wish -f 
# MODS Tcl/TK module
#-----------------------------------------------defaults
set LOGDIR /home2/log
set LIBDIR /home2/isl/TclTK/lib

array set Fonts {
    main   {Sans 12 bold}
    bold   {Helvetica 12 bold}
    title  {Helvetica 18 normal}
    title2 {Helvetica 18 bold}
    status {Helvetica 10}
    vars   {Helvetica 14}
    ariel  {Ariel 12 }
    abold  {Ariel 12 bold}
    tbold  {century 12}
}
set fc grey85
set osucolor red4
set fcoff Darkgrey
set bcolor RoyalBlue3
set fcready black

set pfnm imcs_default.log
global mAval mBval mCval xnew ynew ynew2 ival fval t1val tmval
global d1val xdsp ttfA ttfB ttfC ttfF ttfT cALL loopcolor
global pids
global q1val q2val q3val q4val q5val q6val q7val q8val

#----------------------------------------------- globals
set gblst {cnvs minx maxx miny xnew ynew ynew2 \
	       i garr cgarr xscale yscale \
	       xred yred colourq xreda yreda }
#----------------------------------------------- Main
 wm title . "MODS GUI"
 wm deiconify .
 foreach f1 {1 2 3 4 5} {
    frame .$f1 -borderwidth 2 -relief groove
    pack .$f1 -side top -pady 1
    foreach f2 {1 2 3 4} {
        frame .$f1.$f2 -borderwidth 4
        pack .$f1.$f2 -side left
    }
 }
 set w .1
 button $w.fsmotors -text "WAGO" \
    -relief {raise} -background navy -fg white -command { exec $LIBDIR/vue_islmotors &; exec $LIBDIR/monitor $pfnm & }
 button $w.fshelp -text "HELP" \
    -relief {raise} -background navy -fg white -command { exec vueinfo help imcsLog &};
 button $w.cf -text "Copy files" \
    -relief {raise} -background red -fg white -command { exec $LIBDIR/vue_islcpy & }  
 button $w.fso -text "Clear" \
    -relief {raise} -background navy -fg white -command { erase disc }
 button $w.q -text "Exit" \
    -relief {raise} -background red -fg white -command exit
 
 pack $w.fsmotors $w.fshelp $w.cf $w.fso $w.q -side left -fill both
 set w .2

 button $w.lfilebut -text Start\nLogging -relief {raise} \
    -background linen -fg black -command { global pids; set pids [exec logimcs $cALL $fval $hmval > $LOGDIR/$pfnm &] }
 button $w.sfilebut -text Stop\nLogging -relief {raise} \
    -background linen -fg black -command { exec vueinfo killLOGGING }

 set cGC 0
 set cM 0
 set cT 0
 set cGC2 0
 set cM2 0
 set cT2 0
 set cALL 0
 checkbutton $w.cloggc -text "Argon" \
    -variable cGC -background linen \
    -highlightthickness 0 -disabledforeground black -anchor w
 checkbutton $w.clogmotor -text "Motors" \
    -variable cM -background linen \
    -highlightthickness 0 -disabledforeground black -anchor w
 checkbutton $w.clogtemp -text "Temperature" \
    -variable cT -background linen \
    -highlightthickness 0 -disabledforeground black -anchor w
 entry $w.filent -textvariable pfnm -width 38
 entry $w.hment -textvariable hmval -width 5
 label $w.hmlab -text "Data in SECONDS:"
 pack $w.sfilebut $w.lfilebut -side left
 pack $w.filent -side top
#
 pack $w.hmlab -side left
 pack $w.hment -side left
 pack $w.cloggc -side left
 pack $w.clogmotor -side left
 pack $w.clogtemp -side left

# bind $w.filent <Return> {graph 0 $pfnm $gblst}
 bind $w.filent <F1> {
    if [winfo exists .1.5] {
        destroy .1.5
    } else  {
        frame .1.5 -borderwidth 4
        pack .1.5 -side top
        label .1.5.text -text "File of the type: <x value>,<y value>"
        pack .1.5.text
    }
 }


