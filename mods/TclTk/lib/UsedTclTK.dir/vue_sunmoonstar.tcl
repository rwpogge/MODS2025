namespace eval geom {
    variable p360 [expr atan(1.)*8/360]

    proc crosspoint {xa ya xb yb xc yc xd yd} {
        # compute crossing between two straight lines ("" if parallel)
        if {$xa==$xb} {
            set xres $xa ;# vertical - couldn't divide by deltax
        } else {
            set a [expr double($yb-$ya)/($xb-$xa)]
            set b [expr $yb-($a*$xb)]
        }
        if {$xc==$xd} {
            set xres $xc ;# vertical - couldn't divide by deltax
        } else {
            set c [expr double($yd-$yc)/($xd-$xc)]
            set d [expr $yd-($c*$xd)]
        }
        if {[info exists a] && [info exists c] && $a==$c} {return ""}
        if {![info exists a] && ![info exists c]} {return ""}
        if ![info exists xres] {
            set xres [expr double($d-$b)/($a-$c)]
        }
        if [info exists a] {
            set yres [expr $a*$xres+$b]
        } else {
            set yres [expr $c*$xres+$d]
        }
        list $xres $yres
    }

    proc star5 {x y r {skew 0}} {
        # compute coordinates for a five-point star
        variable p360
        foreach {p angle} {A 0 B 72 C 144 D 216 E 288} {
            set rad [expr ($angle-$skew)*$p360]
            set $p [list [expr $x+$r*sin($rad)] [expr $y-$r*cos($rad)]]
        }
        set F [eval crosspoint $A $C $B $E]
        set G [eval crosspoint $B $D $A $C]
        set H [eval crosspoint $C $E $B $D]
        set I [eval crosspoint $D $A $C $E]
        set J [eval crosspoint $E $B $A $D]
        concat $A $F $B $G $C $H $D $I $E $J
    }
    proc sunrays {x y r {n 12}} {
        # rotated triangles around a circle
        variable p360
        for {set i 0} {$i<$n} {incr i} {
            set rad [expr ($i*360./$n)*$p360]
            set rad1 [expr ($i*360./$n-170./$n)*$p360]
            set rad2 [expr ($i*360./$n+170./$n)*$p360]
            lappend res [list \
                [expr $x+$r*sin($rad)] [expr $y-$r*cos($rad)] \
                [expr $x+$r*0.67*sin($rad1)] [expr $y-$r*0.67*cos($rad1)] \
                [expr $x+$r*0.67*sin($rad2)] [expr $y-$r*0.67*cos($rad2)] \
            ]
        }
        set res
    }
 }
# star --

 # compute coordinates for an n-point star

 proc geom::star {x y r n} {

   variable p360

   # Count how many nodes to skip
   set s [expr {($n - 1) / 2}]

   set n_even [string is integer [expr {$n / 2}]]
   set s_even [string is integer [expr {$s / 2}]]

   # Calculate all the nodes
   for {set node 0} {$node < $n} {incr node} {
     set angle [expr $node * 360.0 / $n]
     set rad [expr {$angle * $p360}]
     set nodex($node) [expr {$x+$r*sin($rad)}]
     set nodey($node) [expr {$y-$r*cos($rad)}]
   }

   # Get intersection of lines
   if { $n > 4 } {
     for {set node1 0} {$node1 < $n} {incr node1} {
       set node2 [expr $node1 + $s]
       if { $node2 >= $n } { set node2 [expr $node2 - $n] }
       set node4 [expr $node1 + 1]
       if { $node4 >= $n } { set node4 [expr $node4 - $n] }
       set node3 [expr $node4 - $s + $n]
       if { $node3 >= $n } { set node3 [expr $node3 - $n] }
       foreach {X Y} [crosspoint \
         $nodex($node1) $nodey($node1) \
         $nodex($node2) $nodey($node2) \
         $nodex($node3) $nodey($node3) \
         $nodex($node4) $nodey($node4)] { break }
       set sectx($node1) $X
       set secty($node1) $Y
     }
   }

   set return {}
   if { $n > 4 } {
     for {set node 0} {$node < $n} {incr node} {
       lappend return \
         $nodex($node) $nodey($node) \
         $sectx($node) $secty($node)
     }
   } else {
     for {set node 0} {$node < $n} {incr node} {
       lappend return \
         $nodex($node) $nodey($node)
     }
   }

   return $return

 }

 # Demo code
   set W .test
   toplevel $W
   grid columnconfigure $W 1 -weight 1

   canvas    $W.c -width 200 -height 200 -xscrollcommand [list $W.x set]
   scrollbar $W.x -orient horizontal -command [list $W.c xview]

   grid $W.c -column 1 -row 1 -sticky nsew
   grid $W.x -column 1 -row 2 -sticky ew

   for {set n 3} {$n <= 20} {incr n} {
     set x [expr ($n - 2) * 100]
     set y 100
     $W.c create polygon [geom::star $x $y 50 $n] \
       -fill blue -outline red
     $W.c create text $x $y \
       -text $n \
       -fill yellow -anchor c \
       -font {Helvetica 20 bold}
   }

   $W.c configure -scrollregion [$W.c bbox all]

proc MakeStar {x y delta} {
    set pi [expr {atan(1) * 4}]

    # Compute distance to inner corner
    #set x1 [expr {$delta * cos(54 * $pi/180)}]  ;# Unit vector to inner point
    set y1 [expr {sin(54 * $pi/180)}]
    set y2 [expr {$delta * sin(18 * $pi/180)}]  ;# Y value to match
    set delta2 [expr {$y2 / $y1}]

    # Now get all coordinates of the 5 outer and 5 inner points
    for {set i 0} {$i < 10} {incr i} {
	set d [expr {($i % 2) == 0 ? $delta : $delta2}]
	set theta [expr {(90 + 36 * $i) * $pi / 180}]
	set x1 [expr {$x + $d * cos($theta)}]
	set y1 [expr {$y - $d * sin($theta)}]

	lappend coords $x1 $y1
    }
    return $coords
 }
