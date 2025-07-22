#!/usr/bin/wish -f 
#source buttons.tcl
# ISL Analysis tool
#-----------------------------------------------defaults
set gwth 650
set ghght 180
set numtc 12
set nxtc 8
set LOGDIR /home2/log
set LIBDIR /home2/isl/TclTK/lib
set IMGPATH /home2/isl/TclTK/images
set testO "00"
set testC "11"
set testN "01"
set bgc 0;
set logd 0;

set pfnm islana_default.log
global mAval mBval mCval xnew ynew ynew2 ival fval t1val tmval
global d1val xdsp ttfA ttfB ttfC ttfF ttfT cALL loopcolor textcolor
global testO testC testN bgc logd pids
global q1val q2val q3val q4val q5val q6val q7val q8val
set bfont {-*-arial-bold-r-*-sans-16-*-*-*-*-*-*-*}
set bfont2 {-*-lucida-bold-r-*-sans-20-*-*-*-*-*-*-*}

image create photo longslit -file "$IMGPATH/long_slit05.gif"

#-----------------------------------------------globals
set gblst {cnvs gwth ghght minx maxx miny xnew ynew ynew2 \
	       i numtc nxtc garr cgarr xscale yscale \
	       xred yred colourq xreda yreda }
#-----------------------------------------------Main
 wm title . "ISL Analysis Tool"
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
 button $w.fsmotors -text "Motors" \
    -relief {raise} -background navy -fg white -command { exec $LIBDIR/vue_islmotors & }
 button $w.fshelp -text "ISL Tool HELP" \
    -relief {raise} -background navy -fg white -command { exec vueinfo help islLog &};
 button $w.cf -text "Copy files" \
    -relief {raise} -background red -fg white -command { exec $LIBDIR/vue_islfcp & }  
 button $w.fso -text "Clear" \
    -relief {raise} -background navy -fg white -command { erase disc }
 button $w.q -text "Exit" \
    -relief {raise} -background red -fg white -command exit
 
 pack $w.fsmotors $w.fshelp $w.cf $w.fso $w.q -side left -fill both

 set w .2

 button $w.lfilebut -text Start\nLogging -relief {raise} \
    -background linen -fg black -command { global pids; set pids [puts $cALL $fval $hmval > $LOGDIR/$pfnm &] }
 button $w.sfilebut -text Stop\nLogging -relief {raise} \
    -background linen -fg black -command { exec vueinfo killLOGGING }

 set cGC 0
 set cM 0
 set cT 0
 set cGC2 0
 set cM2 0
 set cT2 0
 set cALL 0
 checkbutton $w.cloggc -text "XY" \
    -variable cGC -background linen \
    -highlightthickness 0 -disabledforeground black -anchor w
 checkbutton $w.clogmotor -text "Motors" \
    -variable cM -background linen \
    -highlightthickness 0 -disabledforeground black -anchor w
 checkbutton $w.clogtemp -text "Focus" \
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
 bind $w.filent <Return> {graph 0 $pfnm $gblst}
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

# set w .4.2
# set cnvs $w.gcvs1
# frame $w.gp -borderwidth 4; pack $w.gp -side top
# set w .4.2.gp
#
#
# label $w.mAlab -text "Xs:"
# entry $w.mAent -textvariable ttfA -relief flat -bg white -width 8
# label $w.mBlab -text "Ys:"
# entry $w.mBent -textvariable ttfB -relief flat -bg white -width 8
# label $w.mClab -text "Focus:"
# entry $w.mCent -textvariable ttfC -relief flat -bg white -width 8
# label $w.mTlab -text "XY Threshold:"
# entry $w.mTent -textvariable ttfT -relief flat -bg grey -width 6
# pack  $w.mAlab $w.mAent $w.mBlab $w.mBent $w.mClab $w.mCent -side left
# pack  $w.mTlab $w.mTent -side left
# bind $w.mAent <Return> { exec agwvalX mova $ttfA }
# bind $w.mBent <Return> { exec agwvalY mova $ttfB }
# bind $w.mCent <Return> { exec agwvalFP mova $ttfC }
# bind $w.mTent <Return> { exec vueinfo sthreshold $ttfT }
# bind $w.mAent <Return> { exec vueinfo agwvalX $ttfA }
# bind $w.mBent <Return> { exec vueinfo agwvalY $ttfB }
# bind $w.mCent <Return> { exec vueinfo agwvalFP $ttfC }
# bind $w.mTent <Return> { exec vueinfo sthreshold $ttfT }

# label $w.wdlab -text "Graph Width:"
# entry $w.wdent -textvariable gwth -relief flat -bg grey -width 8
# label $w.htlab -text "Graph Height:"
# entry $w.htent -textvariable ghght -relief flat -bg grey -width 8
# pack $w.lfilebut $w.wdlab $w.wdent $w.htlab $w.htent -side left
# pack $w.lfilebut -side left

# set w .5.2
# frame $w.gp -borderwidth 4; pack $w.gp -side top
# set w .5.2.gp
 label $w.glab -text "Time Move:"
 entry $w.gent -textvariable gval -relief flat -bg grey -width 8
 label $w.ilab -text "Average data points:"
 entry $w.ient -textvariable ival -relief flat -bg grey -width 8
 label $w.flab -text "Sampling Time(msecs):"
 entry $w.fent -textvariable fval -relief flat -bg grey -width 8
# pack $w.glab $w.gent -side left
# pack $w.ilab $w.ient -side left
# pack $w.flab $w.fent -side left
# bind $w.gent <Return> { exec vueinfo imcsg1 $gval; }
# bind $w.ient <Return> { exec vueinfo rate1 $ival; }
# bind $w.fent <Return> { exec vueinfo freq1 $fval; }

 set w .5.2
 frame $w.gp1 -borderwidth 4; pack $w.gp1 -side top
 set w .5.2.gp1
# label $w.t1lab -text "Fustep(float):"
# entry $w.t1ent -textvariable t1val -relief flat -bg grey -width 8
# label $w.tm1lab -text "MAX-XY-move(float):"
# entry $w.tm1ent -textvariable tm1val -relief flat -bg grey -width 8
 label $w.d1lab -text "Display Update(msec):"
 entry $w.d1ent -textvariable d1val -relief flat -bg grey -width 8
# pack $w.t1lab $w.t1ent -side left
# pack $w.tm1lab $w.tm1ent -side left
 pack $w.d1lab $w.d1ent -side left

 bind . <Escape> exit
# bind $w.t1ent <Return> { exec vueinfo islanat1 $t1val; }
# bind $w.tm1ent <Return> { exec vueinfo imcstm2 $tm1val; }
# bind $w.d1ent <Return> { set xdsp $d1val;}
 set d1val 1000

 set q1val [expr [exec vueinfo agwval2]] 
 set q2val [expr [exec vueinfo agwval1]]
 set q3val [expr [exec vueinfo agwval3]]
 set q4val [expr [exec vueinfo agwval4]]
 set q5val [expr [exec vueinfo average_qcell1]]
 set q6val [expr [exec vueinfo average_qcell2]]
 set q7val [expr [exec vueinfo average_qcell3]]
 set q8val [expr [exec vueinfo average_qcell4]]
 set mAval [expr [exec vueinfo osci1]] 
 set mBval [expr [exec vueinfo osci2]]
 set mCval [expr [exec vueinfo osci3]]
 set ttfA [expr [exec vueinfo imcsm1]] 
 set ttfB [expr [exec vueinfo imcsm2]]
 set ttfC [expr [exec vueinfo imcsm3]]
 set xdsp $d1val
 set w .5.2
 frame $w.gp2 -borderwidth 4; pack $w.gp2 -side top
 set w .5.2.gp2

 label $w.q1lab -font $bfont -text "  Xs:"
 label $w.q1val -textvariable q1val -font $bfont -background black -fg white -width 8
 label $w.q2lab -font $bfont -text "  Ys:"
 label $w.q2val -textvariable q2val -font $bfont -background black -fg white -width 8
# label $w.q3lab -font $bfont -text " XAG:"
# label $w.q3val -textvariable q3val -font $bfont -background black -fg white -width 8
# label $w.q4lab -font $bfont -text " YAG:"
# label $w.q4val -textvariable q4val -font $bfont -background black -fg white -width 8

 pack $w.q1lab $w.q1val -side left
 pack $w.q2lab $w.q2val -side left
# pack $w.q3lab $w.q3val -side left
# pack $w.q4lab $w.q4val -side left

 set w .5.2
 frame $w.gp3 -borderwidth 4; pack $w.gp3 -side top
 set w .5.2.gp3

# label $w.q5lab -font $bfont -text "XWFS:"
# label $w.q5val -textvariable q5val -font $bfont -background black -fg white -width 8
# label $w.q6lab -font $bfont -text "YWFS:"
# label $w.q6val -textvariable q6val -font $bfont -background black -fg white -width 8
# label $w.q7lab -font $bfont -text "AVG :"
# label $w.q7val -textvariable q7val -font $bfont -background black -fg white -width 8
# label $w.q8lab -font $bfont -text "AVG :"
# label $w.q8val -textvariable q8val -font $bfont -background black -fg white -width 8

# pack $w.q5lab $w.q5val -side left
# pack $w.q6lab $w.q6val -side left
# pack $w.q7lab $w.q7val -side left
# pack $w.q8lab $w.q8val -side left

 set w .5.2
 frame $w.gp4 -borderwidth 4; pack $w.gp4 -side top
 set w .5.2.gp4

 button $w.cloop -text "Stop Tracking" -relief {raise} \
    -background linen -fg black -command { \
        global bgc;
	if { $bgc == 1 } { 
	    exec vueinfo closeLoopOFF; 
	    set bgc 0; 
	} else {
	    set bgc 1; 
	    exec vueinfo closeLoopON; 
	}
	    changeColor $w.cloop $bgc };
 
 proc changeColor { w chgc } { 
     global bfont2
     if { $chgc ==  1 } {
         $w configure -text "Stop Tracking" -background red;
     } else {
         $w configure -text "Tracking     " -background green;
     }
 }

 label $w.ctarget -text "OFF SOURCE" -font $bfont2 -relief {raise} \
    -background red -fg black

 label $w.mAlab -text "Xs:"
 label $w.mAval -textvariable mAval -background black -fg white -width 8
 label $w.mBlab -text "Ys:"
 label $w.mBval -textvariable mBval -background black -fg white -width 8
# label $w.mClab -text "Z:"
# label $w.mCval -textvariable mCval -background black -fg white -width 8

# pack $w.ctarget $w.cloop -side left
# pack $w.ctarget $w.cloop $w.mAlab $w.mAval
# pack $w.mBlab $w.mBval -side left

# islanaplot.tcl --
#    Facilities to draw a islanaplot in a dedicated canvas
#
# Islplot --
#    namespace to hold the procedures and the private data
#
 namespace eval ::Islplot {
    variable scaling
    variable data_series
# Could be:
#    variable mode   "cartesian"
#    variable mode   "polar"
# or set later.
    variable mode  
    variable canvas     .c
    variable colour     red
    variable fillcolour yellow
    variable textcolor blue
    variable textfont   "Times 10"
    variable delay   300
    variable xcurr     0.0
    variable ycurr     0.0
    variable width     10.0
    variable height    10.0
    variable xmin 
    variable xmax
    variable ymin
    variable ymax
    variable degtorad
    set degtorad [expr {3.1415926/180.0}]
#
#    namespace export worldCoordinates viewPort createIslplot \
#                     coordsToPixel
    namespace export draw display moveto moverel colour mode \
	textfont textcolor erase worldCoordinates viewPort createIslplot \
	coordsToPixel colourq DrawXaxis DrawYaxis
 }
 # mode --
 #    Set the coordinates mode (cartesian or polar)
 #
 # Arguments:
 #    type         New mode
 #
 # Result:
 #    None
 #
 # Side effect:
 #    The interpretation of coordinate arguments is changed, if the
 #    type is a valid type. Otherwise it is left unchanged
 #
 proc ::Islplot::mode {type} {
    variable mode

    if { $type == "cartesian" || $type == "polar" } {
       set mode $type
    }
 }

 # textcolor --
 #    Set the colour for text
 #
 # Arguments:
 #    newcolour     New colour to use
 #
 # Result:
 #    None
 #
 # Side effect:
 #    Set a new colour for subsequent text drawing actions
 #
 proc ::Islplot::textcolor {newcolour} {
    variable textcolor

    set textcolor $newcolour
 }

 # textfont --
 #    Set the font for text
 #
 # Arguments:
 #    newfont       New font to use
 #
 # Result:
 #    None
 #
 # Side effect:
 #    Set a new font for subsequent text drawing actions
 #
 proc ::Islplot::textfont {newfont} {
    variable textfont

    set textfont $newfont
 }

 # colour --
 #    Set the current colour
 #
 # Arguments:
 #    newcolour    New colour to be used for outlines
 #    newfill      New colour to be used for filling (defaults to newcolour)
 #
 # Result:
 #    None
 #
 # Side effect:
 #    Set a new colour for subsequent drawing actions
 #
 proc ::Islplot::colour {newcolour {newfill "same"}} {
    variable colour
    variable fillcolour

    set colour $newcolour

    if { $newfill == "same" } {
       set fillcolour $newcolour
    } else {
       set fillcolour $newfill
    }
 }

 # moveto --
 #    Set the current coordinates
 #
 # Arguments:
 #    newx         New x coordinate or distance from origin
 #    newy         New y coordinate or angle to positive x-axis
 #
 # Result:
 #    None
 #
 # Side effect:
 #    Set a new "current" position for subsequent drawing actions
 #
 proc ::Islplot::moveto {newx newy} {
    variable mode
    variable xcurr
    variable ycurr
    variable degtorad

    if { $mode == "cartesian" } {
       set xcurr $newx
       set ycurr $newy
    } else {
       set dist  $newx
       set angle $newy
       set xcurr [expr {$dist*cos($angle*$degtorad)*5.0}]
       set ycurr [expr {$dist*sin($angle*$degtorad)*5.0}]
       #set xcurr [expr {$dist*5.0}]
       #set ycurr [expr {$angle*5.0}]
    }
 }

 # moverel --
 #    Move the current coordinates by the given vector
 #
 # Arguments:
 #    delx         X coordinate of vector over which to move or distance
 #    dely         Y coordinate or angle
 #
 # Result:
 #    None
 #
 # Side effect:
 #    Set a new "current" position for subsequent drawing actions
 #
 proc ::Islplot::moverel {delx dely} {
    variable xcurr
    variable ycurr

    if { $mode == "cartesian" } {
       set xcurr [expr {$xcurr+$delx}]
       set ycurr [expr {$ycurr+$dely}]
    } else {
       set dist  $delx
       set angle $dely

       set xcurr [expr {$xcurr+$dist*cos($angle*$degtorad)}]
       set ycurr [expr {$ycurr+$dist*sin($angle*$degtorad)}]
    }
 }

 # erase --
 #    Erase items from the canvas
 #
 # Arguments:
 #    tagorid       Tag or ID of item(s) to erase
 #
 # Result:
 #    None
 #
 # Side effect:
 #    Removes items from the canvas
 #
 proc ::Islplot::erase {tagorid} {
    variable canvas

    $canvas delete $tagorid
 }

 # draw --
 #    Draw an object into the canvas
 #
 # Arguments:
 #    objtype      Type of object
 #    args         List of arguments, appropriate for type
 #
 # Result:
 #    ID of object that was created (or a specific tag)
 #
 proc ::Islplot::draw {objtype args} {
    variable mode
    variable xcurr
    variable ycurr
    variable xmin
    variable xmax
    variable ymin
    variable ymax
    variable canvas
    variable degtorad
    variable colour
    variable fillcolour
    variable textcolor
    variable textfont
    variable delay
    variable go_on

    switch -- $objtype {
    "grid" {
       for { set x $xmin } { $x < $xmax } { set x [expr {$x+1.0}] } {
	 $canvas create line ${x}c ${ymin}c ${x}c ${ymax}c -tag grid -fill gray
       }

	for { set y $ymin } { $y < $ymax } { set y [expr {$y+1.0}] } {
          $canvas create line ${xmin}c ${y}c ${xmax}c ${y}c -tag grid -fill gray
       }
       $canvas move grid ${xmax}c ${ymax}c
       return grid
    }
    "axes" {
	set xmax [expr {$xmax/2}];
	set ymax [expr {$ymax*1.0}];
       $canvas create line ${xmin}c 0.0c ${xmax}c 0.0c -tag axes -fill black
       $canvas create line 0.0c ${ymin}c 0.0c ${ymax}c -tag axes -fill black
       $canvas move axes ${xmax}c ${ymax}c
       return axes
    }

    "line" {
       if { $mode == "cartesian" } {
          set xp     [lindex $args 0]
          set yp     [lindex $args 1]
          set xcurr  [lindex $args 2]
          set ycurr  [lindex $args 3]
       } else {
          set dist1  [lindex $args 0]
          set angle1 [lindex $args 1]
          set dist2  [lindex $args 2]
          set angle2 [lindex $args 3]
          set xp     [expr {$dist1*cos($angle1*$degtorad)}]
          set yp     [expr {$dist1*sin($angle1*$degtorad)}]
          set xcurr  [expr {$dist2*cos($angle2*$degtorad)}]
          set ycurr  [expr {$dist2*sin($angle2*$degtorad)}]
       }
       set x1    "${xp}c"
       set y1    "[expr {-$yp}]c"
       set x2    "${xcurr}c"
       set y2    "[expr {-$ycurr}]c"

       set obj [\
          $canvas create line $x1 $y1 $x2 $y2 -fill $colour]
    }
    "linerel" {
       set x1 "${xcurr}c"
       set y1 "[expr {-$ycurr}]c"
       if { $mode == "cartesian" } {
          set xcurr [lindex $args 0]
          set ycurr [lindex $args 1]
       } else {
          set dist  [lindex $args 0]
          set angle [lindex $args 1]

          set xcurr [expr {$xcurr+$dist*cos($angle*$degtorad)}]
          set ycurr [expr {$ycurr+$dist*sin($angle*$degtorad)}]
       }
       set x2    "${xcurr}c"
       set y2    "[expr {-$ycurr}]c"

       set obj [\
          $canvas create line $x1 $y1 $x2 $y2 -fill $colour]
    }
    "circle" -
    "disc"   {
       set rad   [lindex $args 0]
       set x1    "[expr {$xcurr-$rad}]c"
       set y1    "[expr {-$ycurr+$rad}]c"
       set x2    "[expr {$xcurr+$rad}]c"
       set y2    "[expr {-$ycurr-$rad}]c"

       if { $objtype == "circle" } {
          set fill {}
       set obj [\
          $canvas create oval $x1 $y1 $x2 $y2 -outline $colour -fill $fill]
       } else {
          set fill $fillcolour
	  set obj [\
	  $canvas create oval $x1 $y1 $x2 $y2 -outline $colour -tag disc -fill $fill]
       }

    }
    "arc" -
    "pie" {
       set rad   [lindex $args 0]
       set start [lindex $args 1]
       set stop  [lindex $args 2]
       set x1    "[expr {$xcurr-$rad}]c"
       set y1    "[expr {-$ycurr+$rad}]c"
       set x2    "[expr {$xcurr+$rad}]c"
       set y2    "[expr {-$ycurr-$rad}]c"

       if { $objtype == "arc" } {
          set fill {}
          set style arc
       } else {
          set fill $fillcolour
          set style pie
       }

       set obj [\
          $canvas create arc $x1 $y1 $x2 $y2 -outline $colour \
             -start $start -extent [expr {$stop-$start}] \
             -style $style -fill $fill]
    }

    "text" {
       set x1    "${xcurr}c"
       set y1    "[expr {-$ycurr}]c"
       set text  [lindex $args 0]

       set obj [\
          $canvas create text $x1 $y1 -text $text -fill $textcolor \
             -font $textfont]
    }

    default {return {}}
    }

    #
    # Move the newly created object to the centre of the window,
    # that is, correct for the origin
    #
    $canvas move $obj ${xmax}c ${ymax}c

    #
    # Wait a while before returning - gives a nice animated effect
    #
    set go_on 0
    after $delay {set ::Islplot::go_on 1}
    vwait ::Islplot::go_on
    return $obj
 }

 # display --
 #    Create the initial canvas
 #
 # Arguments:
 #    None
 # Result:
 #    None
 #
 proc ::Islplot::display {} {
    variable canvas
    variable xmin
    variable xmax
    variable ymin
    variable ymax
    variable width
    variable height

#    canvas $canvas -background linen -width ${width}c -height ${height}c
     canvas $canvas -background linen -width [expr {$width*2.0}]c -height [expr {$height*1.50}]c
    pack   $canvas -fill both -expand 1

     set xmin [expr {-$width}]
     set xmax [expr {+$width}]
     set ymin [expr {-$height}]
     set ymax [expr {+$height}]
#     set xmin [expr {-$width/1.5}]
#     set xmax [expr {+$width/1.5}]
#     set ymin [expr {-$height/1.5}]
#     set ymax [expr {+$height/1.5}]

     draw grid 
     draw axes
 }

 # viewPort --
 #    Set the pixel extremes for the graph
 # Arguments:
 #    w           Name of the canvas window
 #    pxmin       Minimum X-coordinate
 #    pxmax       Maximum X-coordinate
 #    pymin       Minimum Y-coordinate
 #    pymax       Maximum Y-coordinate
 # Result:
 #    None
 # Side effect:
 #    Array scaling filled
 #
 proc ::Islplot::viewPort { w pxmin pxmax pymin pymax } {
    variable scaling

    set scaling($w,pxmin)    $pxmin
    set scaling($w,pymin)    $pymin
    set scaling($w,pxmax)    $pxmax
    set scaling($w,pymax)    $pymax
    set scaling($w,new)      1

 }

 # worldCoordinates --
 #    Set the extremes for the world coordinates
 # Arguments:
 #    w           Name of the canvas window
 #    width       Width of the canvas window
 #    height      Height of the canvas window
 #    xmin        Minimum X-coordinate
 #    xmax        Maximum X-coordinate
 #    ymin        Minimum Y-coordinate
 #    ymax        Maximum Y-coordinate
 # Result:
 #    None
 # Side effect:
 #    Array scaling filled
 #
 proc ::Islplot::worldCoordinates { w xmin xmax ymin ymax } {
    variable scaling

    set scaling($w,xmin)    $xmin
    set scaling($w,ymin)    $ymin
    set scaling($w,xmax)    $xmax
    set scaling($w,ymax)    $ymax
    set scaling($w,new)     1
 }

 # coordsToPixel --
 #    Convert world coordinates to pixel coordinates
 # Arguments:
 #    w           Name of the canvas
 #    xcrd        X-coordinate
 #    ycrd        Y-coordinate
 # Result:
 #    List of two elements, x- and y-coordinates in pixels
 #
 proc ::Islplot::coordsToPixel { w xcrd ycrd } {
    variable scaling

    if { $scaling($w,new) == 1 } {
       set scaling($w,new)     1
       set width               [expr {$scaling($w,pxmax)-$scaling($w,pxmin)+1}]
       set height              [expr {$scaling($w,pymax)-$scaling($w,pymin)+1}]

       set dx                  [expr {$scaling($w,xmax)-$scaling($w,xmin)}]
       set dy                  [expr {$scaling($w,ymax)-$scaling($w,ymin)}]
       set scaling($w,xfactor) [expr {$width/$dx}]
       set scaling($w,yfactor) [expr {$height/$dy}]
    }

    set xpix [expr {$scaling($w,pxmin)+($xcrd-$scaling($w,xmin))*$scaling($w,xfactor)}]
    set ypix [expr {$scaling($w,pymin)+($scaling($w,ymax)-$ycrd)*$scaling($w,yfactor)}]
    return [list $xpix $ypix]
 }

 # createIslplot --
 #    Create a command for drawing a islanaplot
 # Arguments:
 #    w           Name of the canvas
 #    xscale      Minimum, maximum and step for x-axis (initial)
 #    yscale      Minimum, maximum and step for y-axis
 # Result:
 #    Name of a new command
 # Note:
 #    The entire canvas will be dedicated to the islanaplot.
 #    The islanaplot will be drawn with axes
 #
 proc ::Islplot::createIslplot { w xscale yscale } {
    variable data_series

    foreach s [array names data_series "$w,*"] {
       unset data_series($s)
    }

    set newchart "islanaplot_$w"
    interp alias {} $newchart {} ::Islplot::DrawData $w

    set pxmin 80
    set pymin 20
    set pxmax [expr {[$w cget -width]  - 40}]
    set pymax [expr {[$w cget -height] - 20}]


    foreach {xmin xmax xdelt} $xscale {break}
    foreach {ymin ymax ydelt} $yscale {break}

    viewPort         $w $pxmin $pxmax $pymin $pymax
    worldCoordinates $w $xmin  $xmax  $ymin  $ymax

    DrawYaxis        $w $ymin  $ymax  $ydelt
    DrawXaxis        $w $xmin  $xmax  $xdelt
    DrawMask         $w

    return $newchart
 }

 # DrawYaxis --
 #    Draw the y-axis
 # Arguments:
 #    w           Name of the canvas
 #    ymin        Minimum y coordinate
 #    ymax        Maximum y coordinate
 #    ystep       Step size
 # Result:
 #    None
 # Side effects:
 #    Axis drawn in canvas
 #
 proc ::Islplot::DrawYaxis { w ymin ymax ydelt } {
    variable scaling 

     $w create line \
     [expr $scaling($w,pymin)+$scaling($w,pymax)-$scaling($w,pxmin)] \
     $scaling($w,pymin) \
     [expr $scaling($w,pymin)+$scaling($w,pymax)-$scaling($w,pxmin)] \
     $scaling($w,pymax) \
     -fill black -tag yaxis

    set y $ymin

     while { $y < $ymax+0.5*$ydelt } {
       foreach {xcrd ycrd} [coordsToPixel $w $scaling($w,xmin) $y] {break}
       $w create text $xcrd $ycrd -text $y -tag yaxis -anchor e
       set y [expr {$y+$ydelt}]
    }
 }

 # DrawXaxis --
 #    Draw the x-axis
 # Arguments:
 #    w           Name of the canvas
 #    xmin        Minimum x coordinate
 #    xmax        Maximum x coordinate
 #    xstep       Step size
 # Result:
 #    None
 # Side effects:
 #    Axis drawn in canvas
 #
 proc ::Islplot::DrawXaxis { w xmin xmax xdelt } {
    variable scaling

    $w delete xaxis

    $w create line $scaling($w,pxmin) \
               [expr $scaling($w,pxmax)-$scaling($w,pymax)+$scaling($w,pymin)]\
               $scaling($w,pxmax) \
               [expr $scaling($w,pxmax)-$scaling($w,pymax)+$scaling($w,pymin)]\
                  -fill black -tag xaxis

    set x $xmin
    while { $x < $xmax+0.5*$xdelt } {
       foreach {xcrd ycrd} [coordsToPixel $w $x $scaling($w,ymin)] {break}
       $w create text $xcrd $ycrd -text $x -tag xaxis -anchor n
 	set x [expr {$x+$xdelt}]
   }
     set scaling($w,xdelt) $xdelt
 }

 # DrawMask --
 #    Draw the stuff that masks the data lines outside the graph
 # Arguments:
 #    w           Name of the canvas
 # Result:
 #    None
 # Side effects:
 #    Several polygons drawn in the background colour
 #
 proc ::Islplot::DrawMask { w } {
    variable scaling

    set width  [$w cget -width]
    set height [expr {[$w cget -height] + 1}]
    set colour [$w cget -background]
    set pxmin  $scaling($w,pxmin)
    set pxmax  $scaling($w,pxmax)
    set pymin  $scaling($w,pymin)
    set pymax  $scaling($w,pymax)

    $w create rectangle 0 0 $pxmin $height -fill $colour -outline $colour -tag mask
    $w create rectangle 0 0 $width $pymin  -fill $colour -outline $colour -tag mask
    $w create rectangle 0 $pymax $width $height -fill $colour -outline $colour -tag mask

    $w lower mask
 }

 # DrawData --
 #    Draw the x-axis
 # Arguments:
 #    w           Name of the canvas
 #    series      Data series
 #    xcrd        Next x coordinate
 #    ycrd        Next y coordinate
 # Result:
 #    None
 # Side effects:
 #    Axis drawn in canvas
 #
 proc ::Islplot::DrawData { w series xcrd ycrd } {
    variable data_series
    variable scaling

    #
    # Draw the line piece
    #
     if { [info exists data_series($w,$series,x)] } {
	 set xold $data_series($w,$series,x)
	 set yold $data_series($w,$series,y)
	 foreach {pxold pyold} [coordsToPixel $w $xold $yold] {break}
	 foreach {pxcrd pycrd} [coordsToPixel $w $xcrd $ycrd] {break}
	 $w create oval $pxold $pyold [expr $pxold+5] [expr $pyold+5] -fill blue -tag data
	 $w create line $pxold $pyold $pxcrd $pycrd \
	     -fill red -tag data
	 $w create oval [ expr $pxcrd+7] [expr $pycrd+7] $pxcrd $pycrd -fill red -tag data

	 $w lower data
     }

     set data_series($w,$series,x) $xcrd
     set data_series($w,$series,y) $ycrd
 }

proc execAndLog {cmd} {
    global newfile_filename inpipe pids

    set inpipe [open "|/bin/sh -c \"$cmd &1; :\""]
    set pids [pid $inpipe]
    fileevent $inpipe readable [list gendata $inpipe $cmd]
 }

 #-----------------------------------------------Plot difference
 proc diffPlt {gblst} {
    set cmd "global"
    foreach v $gblst {append cmd " $v"}
    eval $cmd
    for {set p 0} {$p < $i} {incr p} {
        set garr($p,y3) [expr $garr($p,y)-$garr($p,y2)]
        set cgarr($p,y3) [expr $ghght -($garr($p,y3)-$miny)*$yscale]
    }
    for {set p 1} {$p < $i} {incr p} {
        set q [expr $p -1]
        $cnvs create line $cgarr($q,x) $cgarr($q,y3) \
                $cgarr($p,x) $cgarr($p,y3) -width 1 -fill \#ff55aa
    }
    destroy .2.pfun .2.funlab .2.funent .2.difbut
 }

 #--------------------------------------------Plot file or function
 proc graph {flg fun gblst} {
    set cmd "global"
# Get rid of this switch, for testing only.
     set from_log 0
    foreach v $gblst {append cmd " $v"}
    eval $cmd
    if { $flg == 0 } {
        destroy $cnvs
        canvas $cnvs -width $gwth -height $ghght \
                -borderwidth 2 -relief sunken -bg white
        pack $cnvs -side bottom
        set c0y $ghght
        set c0x 0
	if { $from_log == 0 } {
	    set fid [open $fun r]
	    set pvtlst [split [read $fid] \n]
	    close $fid
	    foreach ptpr $pvtlst {
		if {$ptpr != ""} {lappend pvtlst2 [split $ptpr ,]}
	    #---------------------------------------note: comma delimited
	    set pvtlst [lsort -real -index 0 $pvtlst2];#----------------
	    set minx [lindex [lindex $pvtlst 0] 0]
	    set numelems [llength $pvtlst]
	    set lastelem [incr numelems -1]
	    set maxx [lindex [lindex $pvtlst $lastelem] 0]
	    #  get all cartesian pairs to plot
	    set i 0
	    foreach ptpair $pvtlst {
		set garr($i,x) [lindex $ptpair 0]
		set garr($i,y) [lindex $ptpair 1]
		incr i
	    }
	} else {
	    set lastelem [incr colourq -1]
	    set minx $xreda(1)
	    set maxx $xreda($lastelem)
	    set i 0
	    for {set p 1} {$p<$lastelem} {incr p} {
		set garr($i,x) $xreda($p);
		set garr($i,y) $yreda($p);
		incr i
	    }
	}
        #now "i" is the number of array elements since it starts w/ 0 and goes to i-1
        # turn cartesian pairs into canvas coordinates
        # first find ymax and ymin
	set maxy $garr(0,y)
	set miny $garr(0,y)
	for {set p 1} {$p<$i} {incr p} {
	    if {$garr($p,y)<$miny} then {set miny $garr($p,y)}
	    if {$garr($p,y)>$maxy} then {set maxy $garr($p,y)}
	}
	set yscale [expr 1.00*$ghght/($maxy-$miny)]
	set xscale [expr 1.00*$gwth/($maxx-$minx)]
	for {set p 0} {$p < $i} {incr p} {
	    set cgarr($p,x) [expr $c0x +($garr($p,x)-$minx)*$xscale]
	    set cgarr($p,y) [expr $c0y -($garr($p,y)-$miny)*$yscale]
	}
        # create lines in canvas
        set c $cnvs
#        draw Yaxis
        draw Yaxis
        set xmd [expr $gwth/2]
        set tcinc [expr $ghght/$numtc]
        $c create line $xmd $c0y $xmd 0 -width 1 -fill white
        for {set p 0} {$p<$numtc} {incr p} {
            set tcy [expr $ghght - $tcinc*$p]
            $c create line 0 $tcy $gwth $tcy -width 1 -fill grey
            set yval [format "%3.2f" [expr $miny+$p*$tcinc/$yscale]]
            $c create text $xmd $tcy -text $yval -fill grey
        }
        set xl [expr $gwth/($i*3)]
        $c create text $xl $tcinc -text $minx -fill grey -anchor w
        $c create text $gwth $tcinc -text $maxx -fill grey -anchor e
        set ntx [expr $nxtc - 1]
        set tcd [expr int($gwth/$ntx)]
        set xdl [expr int($maxx-$minx)/$ntx]
        incr ntx -1
        for {set p 1} {$p<=$ntx} {incr p} {
            incr xl $tcd
            set xtx [expr {$minx+$p*$xdl}]
            $c create text $xl $tcinc -text $xtx -fill grey
        }
        for {set p 1} {$p < $i} {incr p} {
            set q [expr $p -1]
            $c create line $cgarr($q,x) $cgarr($q,y) $cgarr($p,x) $cgarr($p,y) -width 1
        }
        set w .3.2
        destroy $w.pfun $w.funlab $w.funent
        button $w.pfun -text Plot\nFunction \
                -command {graph 1 $funstr $gblst} -fg blue 
        label $w.funlab -text "enter function; tcl format; x is independent var."
        entry $w.funent -textvariable funstr -width 50
        pack $w.pfun -side left -padx 6
        pack $w.funlab $w.funent -side top
        bind $w.funent <Return> {graph 1 $funstr $gblst}
        bind $w.funent <F1> {
            if [winfo exists .3.5] {
                destroy .3.5
            } else  {
                frame .3.5 -borderwidth 4
                pack .3.5 -side top
                label .3.5.text -text {
                    FUNCTIONS
                    acos        cos     hypot   sinh    asin    cosh    log     sqrt
                    atan        exp     log10   tan     atan2   floor   pow     tanh
                    ceil        fmod    sin

                    OPERATORS
                    -,+,~,!    *,/    +,-
                    <<,>>    <,>,<=,>=
                    ==,!=,&,^,|,&&,||,x?y:z
                } -justify left
                pack .3.5.text
            }
        }
    } else  {
        for  {set p 0} {$p<$i} {incr p} {
            # Assume that input function uses "x" as independant variable
            set fun2 [string map {x $garr($p,x)} $fun]
            # make sure "exp" function not clobberd
            set fun2 [string map {e$garr($p,x)p exp} $fun2]
            set garr($p,y2) [expr $fun2]
        }
        for {set p 0} {$p < $i} {incr p} {
            set cgarr($p,y2) [expr {$ghght -($garr($p,y2)-$miny)*$yscale}]
        }
        for {set p 1} {$p < $i} {incr p} {
            set q [expr $p -1]
            $cnvs create line $cgarr($q,x) $cgarr($q,y2) \
                    $cgarr($p,x) $cgarr($p,y2) -width 1 -fill blue
        }
        set w .3.2
        destroy $w.difbut
        button $w.difbut -text "Plot (file-minus-function)" \
                -command {diffPlt $gblst} -fg red
        pack $w.difbut -side bottom -pady 4 
        bind $w.difbut <Return> {diffPlt $gblst}
    }
    }
 }
# --------------------
#       Help procedure
# --------------------
#
#
proc Help {sel} {
        global cmd
# long lines to allow msg to format things

 set h(ISLAlog) {
    ISLA Logging and Transfering files.

    Logging ISLA Data to a file.
    At the top of the ISLA Pixelator you will see the button
    [Start Logging]
    Before you press the button:
    1.) Type the name of the file.
    2.) The number of Minutes.
    3.) Select XYFocus, Motors, Filter Wheel. one,two or All.
    4.) En Fin; press the [Start Logging] Button.

    Transfering files to your PC.
    press the [Copy files] button. 
    This will bring up different.
     1.) Type the PC destination.
     2.) Press the [Send logs to] button.

   This will bring up another xterm window.
   You will see a window with password prompt. 
   Press <CR> or <ENTER>	
   At the prompt smb>
   Type: put /home2/log/yourfile .
              (Don't forget the dot.) ^
   Type: exit


The Help Menu prints Help.}

        if { $sel == "commands" } {
                set h(commands) {Key    Alias
        }
                foreach name [lsort [array names cmd]] {
                        set h(commands) \
                                [format "%s\n%-7s\t%s" $h(commands) $name $cmd($
name)]
                }
        }
        tk_dialog .help "Information" $h($sel) {} -1 OK
}
 #
 # Main code
 #
    set gtemp [split [exec vueinfo gain] "\n"];
    set g1temp [split [exec vueinfo rate] ,];
    set g2temp [split [exec vueinfo freq] ,];
    set gval [expr [lindex $gtemp 0]];
    set ival [expr [lindex $g1temp 0]]; 
    set fval [expr [lindex $g2temp 0]];
    set t1val [expr [lindex $gtemp 2]];
    set tm1val [expr [lindex $gtemp 1]]; 

    set xnew [expr [ exec vueinfo agwval2 ]];
    set ynew [expr [ exec vueinfo agwval1 ]];
    set znew [expr [ exec vueinfo agwval3 ]];

 namespace import ::Islplot::*

 if { 1 } {

# End to end is 255mm.
# Each segment is 36mm
# Science Field end to end is 216mm
#
     mode "cartesian"
     display
     textfont "Times 10 bold"
     textcolor "red"
     colour "red"
     moveto 1.5 3.5
     draw text "Science"
     moveto 4.0 3.5
     draw text "Field"
     moveto  2.54 5.00
     # Science Field
     draw circle 3.0 3.0 3.0 3.0

# Petrol Field
     draw circle 5.00 5.00 5.00 5.00

     colour "yellow"
     draw circle 0.20 0.20 0.20 0.20
     colour "orange"
     draw circle 0.10 0.10 0.10 0.10
     colour "red"
     draw circle 0.01 0.01 0.01 0.01

     # AXIS for X Y HOME SWEET HOME!!
     textfont "Times 10"
     moveto      0.0    0.25
     colour "yellow"
     draw circle 0.22 0.22 0.22 0.22
     colour "orange"
     draw circle 0.12 0.12 0.12 0.12
     colour "red"
     draw circle 0.02 0.02 0.02 0.02
     
     moveto -1.00 0.25
     draw text "HOME-->"
     textcolor "black"
     colour "black"
     moveto -4.0    7.0
     textfont "Times 15 bold"
     draw text "N (Y)"
     draw line -4.0 6.5 -4.0 4.0
     moveto  -2.0   4.0
     draw text "E (X)"
     draw line -4.0 4.0 -3.0 4.0

     # Slit
     colour "black"
     draw line 2.50 2.25 2.50 7.75
#     draw line 3.55 2.25 3.55 7.75
     draw line 2.60 2.25 2.60 7.75
# red in the center
     colour "yellow"
     draw line 2.525 2.25 2.525 7.75

     textcolor "blue"
#     colour "black"
     textfont "Times 12 bold"
     moveto       2.0    0.75
     draw text "AGW Patrol Field"
     colour "blue"

# draw the top edge of the plate that holds the Mask( Y direction).
     draw line -0.46 8.50 5.50 8.50
     draw line -0.46 8.00 5.50 8.00
# angle at plate top
     draw line 5.50 8.50 6.00 8.00
# Left
     draw line -1.00 -10.00 -1.0 10.0
     draw line -0.46 -10.0 -0.46 10.00
# Right
     draw line 6.00 8.00 6.00 2.0
     draw line 5.50 8.00 5.50 2.0
# angle at plate bottom
     draw line 5.50 1.50 6.00 2.00
# draw from center of the plate that holds the Mask( Y direction).
#     draw line 0.54 5.00 6.50 5.0
     draw line -0.46 4.96 5.50 4.96

# draw from edge of Stage to the beginning of the plate
# that holds the Mask( Y direction).
     draw line -0.46 2.00 5.50 2.00

# draw edge of Stage ( Y direction ) 
     draw line -0.46 1.50 5.50 1.50
# draw edge of Stage ( Y direction ) 
     colour "red"
     draw line -0.46 1.44 5.50 1.44

     colour "black"
     textcolor "black"
     moveto 3.50 5.25     
     textfont "Times 10 bold"
     draw text "Science"
     moveto 3.50 4.75     
     draw text "center"
 #
 # Reset the drawing position - all is relative
 #

     moveto       0.0    0.0
#     textcolor "black"
#     colour "red"
     set colourq 1;
 }

 if { 0 } {

 mode   "polar"
 colour "blue"
 textcolor "black"
 textfont   "Times 20 bold"
 display
 set colourq 1;
 draw axes
 draw grid
 moveto      0.0  0.0         ;# move to these coords.
 draw pie    0.0  0.0 0.0    ;# draw a pie with these coords.
 }

 if { 0 } {
# set colourq 1;
#     label .image -image longslit
#     pack .image
#     display
#     worldCoordinates $w 0.0 10.0 0.0 10.0
 }

 label .m4Xlab -text "AGW COMMANS - X: " -font $bfont
 label .m4Ylab -text "Y: " -font $bfont
 label .m4Flab -text "Focus: " -font $bfont
 label .m4Wlab -text "Filter Wheel: " -font $bfont
 entry .m4Xent -textvariable ttfX -font $bfont -relief flat -bg white -width 10
 entry .m4Yent -textvariable ttfY -font $bfont -relief flat -bg white -width 10
 entry .m4Fent -textvariable ttfF -font $bfont -relief flat -bg white -width 10
 entry .m4Went -textvariable ttfW -font $bfont -relief flat -bg red -width 10

 pack  .m4Xlab .m4Xent -side left
 pack  .m4Ylab .m4Yent -side left
 pack  .m4Flab .m4Fent -side left
 pack  .m4Wlab .m4Went -side left
 bind  .m4Yent <Return> { exec smc04 M1 $ttfY &};
 bind  .m4Xent <Return> { exec smc05 M1 $ttfX & };
 bind  .m4Fent <Return> { exec smc06 M1 $ttfF & };
 bind  .m4Went <Return> { set ttfW "NON-OPER" };
# bind  .m4Went <Return> { exec smc07 M1 $ttfW & };

proc islanadata { } {
    global xdelx ydely colourq xred yred xreda yreda pfnm gblst ival
    global mAval mBval mCval dimmer
    global t1val tm1val xdsp ttfA ttfB ttfC 
    global cGC cGC2 cM cM2 cT cT2 cALL  w.5.2.gp4 
    global q1val q2val q3val q4val q5val q6val q7val q8val 

    if { [expr fmod(5.0,$colourq)] } {
	#colour "blue";
	colour "linen";
#	moveto  $xreda([expr [$colourq]-1]) $yreda([expr [$colourq]-1])
	draw disc    0.25;
    }

    if { $colourq == 10 } {
	erase disc;
	set colourq 1;
    }

# Plot
# X = East, Y = North, cell = 36mm x 36mm = 1' x 1' = 60" x 60"
# In millimeters
# MASK frame edge ( frame = 255x255 )
# a = east, b = west, c = north, d = south
# MASK edge ( MASK = 216x216 )
# For calculation of position.
# X=X/36.00
# for Y and Z only one edge needs to be concidered.
# Y=(Y-22.5)/36.00
# Z(focus)=Z/36.00
#
#    set xred [expr abs(([expr [exec vueinfo agwval2]])/[expr 25.50])]
    set xred [expr abs(([expr [exec vueinfo agwval2]])/[expr 36.00])]
    set yred [expr abs(([expr [exec vueinfo agwval1]])/[expr 36.00])]
    set zred [expr abs(([expr [exec vueinfo agwval3]])/[expr 36.00])]
#    set xred [expr abs(([expr [exec vueinfo agwval2]-39.0])/[expr 36.00])]
#    set yred [expr abs(([expr [exec vueinfo agwval1]-36.0])/[expr 36.00])]
#    set zred [expr abs(([expr [exec vueinfo agwval3]-25.0])/[expr 36.00])]
#    set zred [expr abs(([expr [exec vueinfo agwval3]+19.5])/[expr 36.00])]
#    puts "$xred $yred $zred"

# Subtract Z from Y to allow for the 22.5mm extra travel on Focus.
# allow for center of pickoff field 0.556
    set zred [expr {$zred}]
    set yred [expr {$yred-$zred-0.225}]
#    set yred [expr {$yred-$zred-0.556}]
#    puts "$xred $yred $zred"

    moveto  [expr abs($xred)] [expr abs($yred)]

    colour "blue"
    draw disc    0.20
    colour "red";
    draw disc    0.05;

    textfont   "Times 14 bold"

    set q1val [expr [exec vueinfo agwval2]]
    set q2val [expr [exec vueinfo agwval1]]
    set q3val [expr [exec vueinfo agwval3]]
    set q4val [expr [exec vueinfo agwval4]]
    set q5val [expr [exec vueinfo average_qcell1]]
    set q6val [expr [exec vueinfo average_qcell2]]

    set mAval $xred
    set mBval $yred
    set mCval $zred

    set xreda($colourq) $xred;
    set yreda($colourq) $yred;

    incr colourq

    set targetChangeStatus [expr [exec vueinfo targetStatus]];
    if { $targetChangeStatus == 1} {
	set w .5.2.gp4
	$w.ctarget configure -text "ON SOURCE" -background green;
     } else {
	set w .5.2.gp4
	$w.ctarget configure -text "OFF SOURCE" -background red;
     }

    set testO "00"
    set testC "11"
    set testN "01"
    set loopStatus [exec vueinfo closeLoop];
    if { $loopStatus == $testO } {
	set w .5.2.gp4
	$w.cloop configure -text "IN Position" -background green;
    } elseif { $loopStatus == $testC } {
	set w .5.2.gp4
	$w.cloop configure -text "Acquiring  " -background red;
    } elseif { $loopStatus == $testN } {
	set w .5.2.gp4
	$w.cloop configure -text "Recover(HOME)" -background red;
    }

    # Are we logging? If we are, then what are we logging?
    if { $cGC == 1 } { 
	# Filter Wheel in volts
	set cGC2 1 
    } else {
	set cGC2 0;
    }
    if { $cM == 1 } { 
	# Motors absolute positions
	set cM2 3
    } else {
	set cM2 0;
    }
    if { $cT == 1 } { 
	# Temperature in degC?
	set cT2 5;
    } else {
	set cT2 0;
    }
    set cALL [expr {$cGC2+$cM2+$cT2}] 
    #
    # Wait a while before returning - gives a nice animated effect
    #
    after $xdsp islanadata
}
after $xdsp islanadata
