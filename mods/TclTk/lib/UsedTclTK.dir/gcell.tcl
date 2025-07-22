# Example generator cell (Gcell) to draw a simple fet.
# This procedure draws a simple fet given a list of properties
proc gcell_make_simplefet {properties} -desc {
    Gcell procedure to make a fet
} {
    # get values out of properties list
    set type [get_assoc type $properties]
    set width [get_assoc width $properties]
    set length [get_assoc length $properties]
    # fet gate is vertical.  Origin is lower left corner of gate
    # what layers make up this fet?
    setl {poly diff} [techinfo device $type]
    if {$poly == ""} {
	puts "Aborting, Illegal fet type $type."
	return
    }
    # get some key params
    set poly_ext_gate [max [techinfo enclose $poly $type opt] 0.1]
    set diff_ext_gate [max [techinfo enclose $diff $type opt] 0.1]
    set x -$diff_ext_gate
    # draw the poly
    gcell_paint $poly 0 -$poly_ext_gate $length [expr $width + \
						     $poly_ext_gate]
    # draw the diffusion
    gcell_paint $diff -$diff_ext_gate 0 [expr $length + \
					     $diff_ext_gate] $width
    # the return string will show up when the Gcell is selected
    return "$width/$length"
}
# Fix up the props used to make a fet Gcell.  
# Allow fet width only to stretch.
proc gcell_make_props_simplefet {prop_list} {
    # get layers and key parameters
    set type [get_assoc type $prop_list]
    setl {poly diff} [techinfo device $type]
    if {$poly == ""} {
	# illegal type
	return $prop_list
    }
    set minwidth 0.4
    if {[set bbox [get_assoc _BBOX_  $prop_list]] != ""} {
	# compute width from bbox
	set prop_list [rm_assoc _BBOX_ $prop_list]
	set prop_list [rm_assoc width $prop_list]
	setl {x1 y1 x2 y2} $bbox
	set poly_ext_gate [max [techinfo enclose $poly $type opt] 0.1]
	set newwidth [expr $y2 - $y1 - 2.0 * $poly_ext_gate]
	lappend prop_list [list width [uusnap [max $newwidth \
						   $minwidth]]]
    } elseif {[set newwidth [get_assoc width $prop_list]] != ""} {
	# Check width validity.
	set prop_list [rm_assoc width $prop_list]
	lappend prop_list [list width [uusnap [max $newwidth \
						   $minwidth]]]
    }
    return $prop_list
}
# now define it and give default arguments
set fets [techinfo devices]
set properties ""
lappend properties "type [lindex $fets 0] choice \{$fets\}"
lappend properties "width 1 -number -incr 0.1"
set min_length [techinfo width [lindex [techinfo device \
					    $default_fet] 0]]
lappend properties "length $min_length -number \
            	-incr [res] -snap 0.1"
define_gcell simplefet $properties
