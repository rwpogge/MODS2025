#########################################################
# 
# tk_getDirectory [option value ...]
#
#  options are :
#   [-initialdir dir]     display in dir
#   [-title string]       make string title of dialog window
#   [-ok string]          make string the label of OK button
#   [-open string]        make string the label of OPEN button
#   [-cancel string]      make string the label of CANCEL button
#   [-msg1 string]        make string the label of the first directory message
#   [-msg2 string]        make string the label of the second directory message
#   [-parent string]      make string the parent of tkgetdir for transient window
#   [-geometry string]    make string the geometry (WxH+X+Y)
#   [-listbg string]      make string the list and entry background color
#   [-listfg string]      make string the list and entry foreground color
#   [-selectbg string]    make string the background color for selected directory
#   [-selectfg string]    make string the foreground color for selected directory
#   [-hilightbg string]   make string the background color for highlighted dir
#   [-hilightfg string]   make string the foreground color for highlighted dir
#   [-endcolor string]    make string the color for the last directory flag
#   [-autovalid]          if it is set : selecting no directory in the window
#                          will return the directory the window shows currently
#   [-font string]        make string the font name or font description
#                          for directory window
#
#########################################################

namespace eval tkgetdir {
    variable drives
    variable fini
    variable svwd
    variable msg
    variable geometry
    variable parent
    variable colors
    variable font
    variable myfont
    variable autovalid
    
    namespace export tk_getDirectory
}

proc tkgetdir::tk_getDirectory {args} {
    global tcl_platform

    #
    # default
    #
    set tkgetdir::msg(title) "Directory Selector"
    set tkgetdir::msg(ldir) "Directory:"
    set tkgetdir::msg(ldnam) "Directory Name:"
    set tkgetdir::msg(open) " Ok "
    set tkgetdir::msg(expand) "Open"
    set tkgetdir::msg(cancel) "Cancel"
    set tkgetdir::geometry "500x250"
    set tkgetdir::colors(lstfg) "#000000"
    if [string compare $tcl_platform(platform) windows] {
	set tkgetdir::colors(lstbg) ""
	set tkgetdir::font {-family lucida -size 14 -weight bold}
    } else {
	set tkgetdir::colors(lstbg) "#FFFFFF"
	set tkgetdir::font {-family courier -size 10}
    }
    set tkgetdir::colors(hilfg) "#FFFF00"
    set tkgetdir::colors(hilbg) "#808080"
    set tkgetdir::colors(selfg) "#FFFFFF"
    set tkgetdir::colors(selbg) "#000080"
    set tkgetdir::colors(endcol) "#FF8000"
    set tkgetdir::parent ""
    set tkgetdir::autovalid 0
    set tkgetdir::svwd [pwd]
    
    #
    # arguments
    #
    set ind 0
    set max [llength $args]
    while { $ind < $max } {
	switch -exact -- [lindex $args $ind] {
	    "-initialdir" {
		incr ind
		cd [lindex $args $ind]
		incr ind
	    }
	    "-title" {
		incr ind
		set tkgetdir::msg(title) [lindex $args $ind]
		incr ind
	    }
	    "-ok" {
		incr ind
		set tkgetdir::msg(open) [lindex $args $ind]
		incr ind
	    }
	    "-open" {
		incr ind
		set tkgetdir::msg(expand) [lindex $args $ind]
		incr ind
	    }
	    "-cancel" {
		incr ind
		set tkgetdir::msg(cancel) [lindex $args $ind]
		incr ind
	    }
	    "-msg1" {
		incr ind
		set tkgetdir::msg(ldir) [lindex $args $ind]
		incr ind
	    }
	    "-msg2" {
		incr ind
		set tkgetdir::msg(ldnam) [lindex $args $ind]
		incr ind
	    }
	    "-geometry" {
		incr ind
		set tkgetdir::geometry [lindex $args $ind]
		incr ind
	    }
	    "-parent" {
		incr ind
		set tkgetdir::parent [lindex $args $ind]
		incr ind
	    }
	    "-listfg" {
		incr ind
		set tkgetdir::colors(lstfg) [lindex $args $ind]
		incr ind
	    }
	    "-listbg" {
		incr ind
		set tkgetdir::colors(lstbg) [lindex $args $ind]
		incr ind
	    }
	    "-selectfg" {
		incr ind
		set tkgetdir::colors(selfg) [lindex $args $ind]
		incr ind
	    }
	    "-selectbg" {
		incr ind
		set tkgetdir::colors(selbg) [lindex $args $ind]
		incr ind
	    }
	    "-hilightfg" {
		incr ind
		set tkgetdir::colors(hilfg) [lindex $args $ind]
		incr ind
	    }
	    "-hilightbg" {
		incr ind
		set tkgetdir::colors(hilbg) [lindex $args $ind]
		incr ind
	    }
	    "-endcolor" {
		incr ind
		set tkgetdir::colors(endcol) [lindex $args $ind]
		incr ind
	    }
	    "-autovalid" {
		incr ind
		set tkgetdir::autovalid 1
	    }
	    "-font" {
		incr ind
		set tkgetdir::font [lindex $args $ind]
		incr ind
	    }
	    default {
		puts "unknown option [lindex $args $ind]"
		return ""
	    }
	}
    }
    
    #
    # variables et data
    #
    set tkgetdir::fini 0
    
    image create photo b_up -data {
	R0lGODlhFgATAMIAAHt7e9/fX////gAAAK6uSv///////////yH+Dk1hZGUgd2l0aCBHSU1QACH5
	BAEAAAcALAAAAAAWABMAAANVeArcoDBKEKoNT2p6b9ZLJzrkAQhoqq4qMJxi3LnwRcjeK9jDjWM6
	C2FA9Mlou8CQWMQhO4Nf5XmJSqkW6w9bYXqZFq40HBzPymYyac1uDA7fuJyZAAA7
    }

    image create photo b_dir -data {
	R0lGODlhEAAQAMIAAHB/cN/fX////gAAAP///////////////yH+Dk1hZGUgd2l0aCBHSU1QACH5
	BAEAAAQALAAAAAAQABAAAAM2SLrc/jA2QKkEIWcAsdZVpQBCaZ4lMBDk525r+34qK8x0fOOwzfcy
	Xi2IG4aOoRVhwGw6nYQEADs=
    }

    if {[lsearch -exact $tkgetdir::font -family] >= 0} {
	eval font create dlistfont $tkgetdir::font
	set tkgetdir::myfont dlistfont
    } else {
	set tkgetdir::myfont $tkgetdir::font
    }

    #
    # widgets
    #
    toplevel .dirsel
    grab set .dirsel
    wm geometry .dirsel $tkgetdir::geometry
    if {$tkgetdir::parent != ""} {
        set par $tkgetdir::parent
	set xOrgWin [expr [winfo rootx $par] + [winfo width $par] / 2 ]
	set yOrgWin [expr [winfo rooty $par] + [winfo height $par] / 2 ]
	wm geometry .dirsel +$xOrgWin+$yOrgWin
	wm transient .dirsel $tkgetdir::parent
    }
    wm title .dirsel $tkgetdir::msg(title)

    event add <<RetEnt>> <Return> <KP_Enter>
    
    frame .dirsel.f1 -relief flat -borderwidth 0
    frame .dirsel.f2 -relief sunken -borderwidth 2 
    frame .dirsel.f3 -relief flat -borderwidth 0
    frame .dirsel.f4 -relief flat -borderwidth 0
    
    pack .dirsel.f1 -fill x
    pack .dirsel.f2 -fill both -expand 1 -padx 6 -pady 6
    pack .dirsel.f3 -fill x
    pack .dirsel.f4 -fill x
    
    label .dirsel.f1.lab -text $tkgetdir::msg(ldir)
    menubutton .dirsel.f1.dir -relief raised -indicatoron 1 -anchor w \
	    -menu .dirsel.f1.dir.m
    menu .dirsel.f1.dir.m -tearoff 0
    button .dirsel.f1.up -image b_up -command tkgetdir::UpDir
    bind .dirsel.f1.up <<RetEnt>> {.dirsel.f1.up invoke}

    pack .dirsel.f1.up -side right -padx 4 -pady 4
    pack .dirsel.f1.lab -side left -padx 4 -pady 4
    pack .dirsel.f1.dir -side right -padx 4 -pady 4 -fill x -expand 1
    
    canvas .dirsel.f2.cv -borderwidth 0 -yscrollcommand ".dirsel.f2.sb set"
    if { $tkgetdir::colors(lstbg) != "" } {
	.dirsel.f2.cv configure -background $tkgetdir::colors(lstbg)
    }
    scrollbar .dirsel.f2.sb -command ".dirsel.f2.cv yview"
    set scw 16
    place .dirsel.f2.cv -x 0 -relwidth 1.0 -width [expr -$scw ] -y 0 \
	    -relheight 1.0
    place .dirsel.f2.sb -relx 1.0 -x [expr -$scw ] -width $scw -y 0 \
	    -relheight 1.0
    unset scw
    
    .dirsel.f2.cv bind TXT <Any-Enter> tkgetdir::EnterItem
    .dirsel.f2.cv bind TXT <Any-Leave> tkgetdir::LeaveItem
    .dirsel.f2.cv bind TXT <Any-Button> tkgetdir::ClickItem
    .dirsel.f2.cv bind TXT <Double-Button> tkgetdir::DoubleClickItem
    .dirsel.f2.cv bind IMG <Any-Enter> tkgetdir::EnterItem
    .dirsel.f2.cv bind IMG <Any-Leave> tkgetdir::LeaveItem
    .dirsel.f2.cv bind IMG <Any-Button> tkgetdir::ClickItem
    .dirsel.f2.cv bind IMG <Double-Button> tkgetdir::DoubleClickItem
    
    label .dirsel.f3.lnam -text $tkgetdir::msg(ldnam)
    entry .dirsel.f3.chosen -takefocus 0
    if { $tkgetdir::colors(lstbg) != "" } {
	.dirsel.f3.chosen configure -background $tkgetdir::colors(lstbg)
    }
    pack .dirsel.f3.lnam -side left -padx 4 -pady 4
    pack .dirsel.f3.chosen -side right -fill x -expand 1 -padx 4 -pady 4
    
    button .dirsel.f4.open -text $tkgetdir::msg(open) -command { 
	set tmp [.dirsel.f3.chosen get]
	if { ( [ string length $tmp ] ) || ( $tkgetdir::autovalid == 1 ) } {
	    set tkgetdir::fini 1 
	}
    }
    bind .dirsel.f4.open <<RetEnt>> {.dirsel.f4.open invoke}

    button .dirsel.f4.expand -text $tkgetdir::msg(expand) -command tkgetdir::DownDir
    bind .dirsel.f4.expand <<RetEnt>> {.dirsel.f4.expand invoke}

    button .dirsel.f4.cancel -text $tkgetdir::msg(cancel) -command { 
	set tkgetdir::fini -1 
    }
    bind .dirsel.f4.cancel <<RetEnt>> {.dirsel.f4.cancel invoke}
    
    pack .dirsel.f4.open .dirsel.f4.expand -side left -padx 10 -pady 4
    pack .dirsel.f4.cancel -side right -padx 10 -pady 4
    
    bind .dirsel.f1 <Destroy> tkgetdir::CloseDirSel

    #
    # realwork
    #
    tkgetdir::ShowDir [pwd]
    
    #
    # wait user
    #
    tkwait variable tkgetdir::fini

    if { $tkgetdir::fini == 1 } {
	set curdir [.dirsel.f1.dir cget -text]
	set nnam [.dirsel.f3.chosen get]
	set retval [ file join $curdir $nnam ]
    } else {
	set retval ""
    }
    
    destroy .dirsel
    
    return $retval
}

proc tkgetdir::CloseDirSel {} {
    set wt [font names]
    if {[lsearch -exact $wt dlistfont] >= 0} {
	font delete dlistfont 
    }
    event delete <<RetEnt>>
    cd $tkgetdir::svwd
    set tkgetdir::fini 0
}

proc tkgetdir::ShowDir {curdir} {

    global tcl_platform
    variable drives 
    
    cd $curdir
    .dirsel.f1.dir configure -text $curdir
    
    set hi1 [font metrics $tkgetdir::myfont -linespace]
    set hi2 [image height b_dir]
    if { $hi1 > $hi2 } {
	set hi $hi1
    } else {
	set hi $hi2
    }
    set wi1 [image width b_dir]
    incr wi1 4
    set wi2 [winfo width .dirsel.f2.cv]
    
    set lidir [list]
    foreach file [ glob -nocomplain * ] {
	if [ file isdirectory [string trim $file "~"] ] { 
	    lappend lidir $file
	}
    }
    set sldir [lsort $lidir]
    
    .dirsel.f2.cv delete all
    set ind 0
    # Adjust the position of the text wi1 with an offset.
    if { $hi1 < $hi2 } {
	set offset [expr $hi2 - $hi1]
    } else {
	set offset 0
    }
    foreach file $sldir {
	if [ file isdirectory $file ] { 
	    .dirsel.f2.cv create image 2 [expr $ind * $hi] \
		    -anchor nw -image b_dir -tags IMG
	    .dirsel.f2.cv create text $wi1 [expr ($ind * $hi) + $offset] \
		    -anchor nw -text $file -fill $tkgetdir::colors(lstfg) \
		    -font $tkgetdir::myfont -tags TXT
	    set ind [ expr $ind + 1 ]
	}
    }

    set ha [expr $ind * $hi]
    .dirsel.f2.cv configure -scrollregion [list 0 0 $wi2 $ha]
    
    set curlst [file split $curdir]
    set nbr [llength $curlst]
    
    .dirsel.f1.dir.m delete 0 last
    incr nbr -2
    for {set ind $nbr} {$ind >= 0} {incr ind -1} {
	set tmplst [ lrange $curlst 0 $ind] 
	set tmpdir [ eval file join $tmplst] 
	.dirsel.f1.dir.m add command -label $tmpdir \
		-command "tkgetdir::ShowDir {$tmpdir}"
    }
    if {[info exist drives] == 0} {
	set drives [file volume]
    }
    if ![string compare $tcl_platform(platform) windows] {
	foreach drive $drives {
	    .dirsel.f1.dir.m add command -label $drive \
		    -command "tkgetdir::ShowDir {$drive}"
	}
    }
    
}

proc tkgetdir::UpDir {} {
    set curdir [.dirsel.f1.dir cget -text]
    set curlst [file split $curdir]
    
    set nbr [llength $curlst]
    if { $nbr < 2 } {
	return
    }
    set tmp [expr $nbr - 2]
    
    set newlst [ lrange $curlst 0 $tmp ]
    set newdir [ eval file join $newlst ]
    
    .dirsel.f3.chosen delete 0 end
    tkgetdir::ShowDir $newdir
}

proc tkgetdir::DownDir {} {
    set curdir [.dirsel.f1.dir cget -text]
    set nnam [.dirsel.f3.chosen get]

    set newdir [ file join $curdir $nnam ]

    # change 07/19/99
    # If there are more dirs, permit display of one level down.
    # Otherwise, block display and hilight selection in red.
    set areDirs 0
    foreach f [glob -nocomplain [file join $newdir *]] {
	if {[file isdirectory $f]} {
	    set areDirs 1
	    break
	}
    }
 
    if {$areDirs} {
	.dirsel.f3.chosen delete 0 end
	tkgetdir::ShowDir $newdir
    } else {
	set id [.dirsel.f2.cv find withtag HASBOX ]
	.dirsel.f2.cv itemconfigure $id -fill $tkgetdir::colors(endcol)
    }
}

proc tkgetdir::EnterItem {} {
    global tcl_platform

    set id [.dirsel.f2.cv find withtag current]
    set wt [.dirsel.f2.cv itemcget $id -tags]
    if {[lsearch -exact $wt IMG] >= 0} {
	set id [.dirsel.f2.cv find above $id]
    }

    .dirsel.f2.cv itemconfigure $id -fill $tkgetdir::colors(hilfg)
    set bxr [.dirsel.f2.cv bbox $id]
    eval .dirsel.f2.cv create rectangle $bxr \
	    -fill $tkgetdir::colors(hilbg) -outline $tkgetdir::colors(hilbg) \
	    -tags HILIT
    .dirsel.f2.cv lower HILIT
}

proc tkgetdir::LeaveItem {} {
    .dirsel.f2.cv delete HILIT
    set id [.dirsel.f2.cv find withtag current]
    set wt [.dirsel.f2.cv itemcget $id -tags]
    if {[lsearch -exact $wt IMG] >= 0} {
	set id [.dirsel.f2.cv find above $id]
    }
    set wt [.dirsel.f2.cv itemcget $id -tags]
    if {[lsearch -exact $wt HASBOX] >= 0} {
	.dirsel.f2.cv itemconfigure $id -fill $tkgetdir::colors(selfg)
    } else {
	.dirsel.f2.cv itemconfigure $id -fill $tkgetdir::colors(lstfg)
    }
}

proc tkgetdir::ClickItem {} {
    .dirsel.f2.cv delete HILIT
    # put old selected item in normal state
    .dirsel.f2.cv delete BOX
    set id [.dirsel.f2.cv find withtag HASBOX]
    .dirsel.f2.cv itemconfigure $id -fill $tkgetdir::colors(lstfg)
    .dirsel.f2.cv dtag HASBOX HASBOX
    # put new selected item in selected state
    set id [.dirsel.f2.cv find withtag current]
    set wt [.dirsel.f2.cv itemcget $id -tags]
    if {[lsearch -exact $wt IMG] >= 0} {
	set id [.dirsel.f2.cv find above $id]
    }
     set bxr [.dirsel.f2.cv bbox $id]
    .dirsel.f2.cv addtag HASBOX withtag $id
    .dirsel.f2.cv itemconfigure $id -fill $tkgetdir::colors(selfg)
    eval .dirsel.f2.cv create rectangle $bxr \
	    -fill $tkgetdir::colors(selbg) -outline $tkgetdir::colors(selbg) \
	    -tags BOX
    .dirsel.f2.cv lower BOX
    set nam [.dirsel.f2.cv itemcget $id -text]
    .dirsel.f3.chosen delete 0 end
    .dirsel.f3.chosen insert 0 $nam
}

proc tkgetdir::DoubleClickItem {} {
    set id [.dirsel.f2.cv find withtag current]
    tkgetdir::DownDir
}

