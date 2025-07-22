#!/usr/local/bin/wish -f

listbox .list -yscroll ".scroll set"
# -geometry 20x20

pack .list -side left
scrollbar .scroll -command ".list yview"

pack .scroll -side right -fill y

if {$argc > 0} {
    set dir [lindex $argv 0]
} else {
    set dir .
}

foreach i [lsort [glob * .*]] {
    .list insert end $i
}

bind .list <Double-ButtonPress-1> { browse $dir [selection get] }

bind all <Control-c> { destroy . }

proc browse {dir file} {
    if {$dir != "."} {
	set file $dir/$file
    }
    
    if {file isdirectory $file] {
	exec browse $file &
    } else {
	if [file isfile $file] {
	    exec xedit $file &
	} else { 
	    error "can't browse $file"
	}
    }
}
