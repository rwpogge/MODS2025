proc main {} {
    
   canvas .c -bg white -borderwidth 0
   bind   .c <Motion> {displayXY .info %x %y}
   frame  .f
     label  .f.1 -text "agw position = "
     entry  .f.f -textvar ::function -width 40
       bind .f.f <Return> {plotf .c $::function}
     button .f.demo -text " ? " -pady 0 -command {demo .c}
     label  .f.2 -text " Zoom: "
     entry  .f.fac -textvar ::factor -width 4
       set                  ::factor 32
       bind .f.fac <Return>               {zoom .c 1.0}
     button .f.plus  -text " + " -pady 0 -command {zoom .c 2.0}
     button .f.minus -text " - " -pady 0 -command {zoom .c 0.5}
     eval pack [winfo children .f] -side left -fill both
   label  .info -textvar ::info -just left
   pack .info .f -fill x -side bottom
   pack .c -fill both -expand 1
   demo .c
}

set ::demos {
       "cos x3" 2 1-x 0.5x2 x3/5 "sin x" "sin x2" 1/x sqrt(x)
       "tan x/5" x+1/x x abs(x) "exp x" "log x" "log x2"
       round(x) "int x%2" "x-int x" "0.2tan x+1/tan x" x*(rand()-0.5)
       x2/5-1/(2x) "atan x" sqrt(1-x2) "abs(x-int(x*2))" (x-1)/(x+1)
       "sin x-tan x" "sin x-tan x2" "x-abs(int x)" 0.5x-1/x
       -0.5x3+x2+x-1 3*sin(2x) -0.05x4-0.2x3+1.5x2+2x-3 "9%int x"
       0.5x2/(x3-3x2+4) "abs x2-3 int x" "int x%3"
}

proc displayXY {w cx cy} {
       set x [expr {double($cx-$::dx)/$::factor}]
       set y [expr {double(-$cy+$::dy)/$::factor}]
       set ::info [format "x=%.2f y=%.2f" $x $y]
       catch {
       $w config -fg [expr {abs([expr $::fun]-$y)<0.01?"white":"black"}]
       } ;# may divide by zero, or other illegal things
}

proc zoom {w howmuch} {
   set ::factor [expr round($::factor*$howmuch)]
   plotf $w $::function
}

proc plotf {w function} {
   foreach {re subst} {
       {([a-z]) +(x[0-9]?)} {\1(\2)}   " " ""   {([0-9])([a-z])} {\1*\2}
       x2 x*x   x3 x*x*x    x4 x*x*x*x   x \$x   {e\$xp} exp
   } {regsub -all $re $function $subst function}
   set ::fun $function
   set ::info "Tcl: expr $::fun"
   set color [lpick {red blue purple brown green}]
   plotline $w [fun2points $::fun] -fill $color
}

proc lpick L {lindex $L [expr {int(rand()*[llength $L])}]}

proc fun2points {fun args} {
   array set opt {-from -10.0 -to 10.0 -step .01}
   array set opt $args
   set res "{"
   for {set x $opt(-from)} {$x<= $opt(-to)} {set x [expr {$x+$opt(-step)}]} {
       if {![catch {expr $fun} y]} {
           if {[info exists lasty] && abs($y-$lasty)>100} {
               append res "\} \{" ;# incontinuity
           }
           append res " $x $y"
           set lasty $y
       } else {append res "\} \{"}
   }
   append res "}"
}

proc plotline {w points args} {
   $w delete all
   foreach i $points {
       if {[llength $i]>2} {eval $w create line $i $args -tags f}
   }
   set fac $::factor
   $w scale all 0 0 $fac -$fac
   $w create line -10000 0 10000 0      ;# X axis
   $w create line 0 -10000 0 10000      ;# Y axis
   $w create line $fac 0     $fac -3    ;# x=1 tick
   $w create line -3   -$fac 0    -$fac ;# y=1 tick
   set ::dx [expr {[$w cget -width]/2}]
   set ::dy [expr {[$w cget -height]/2}]
   $w move all $::dx $::dy
   $w raise f
}

proc demo {w} {
   set ::function [lindex $::demos 0] ;# cycle through...
   set ::demos [concat [lrange $::demos 1 end] [list $::function]]
   set ::factor 32
   plotf $w $::function
}

main
