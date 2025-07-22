#!/usr/bin/env app-wish-f
proc newop {type text} {
    global opnum
    "new$type"
    .ops.$opnum.text insert 1.0 $text
}

source [exec hunt xe $env(PATH)]

toplevel .c
canvas .c.c -width 600 -height 800
pack .c.c
bind .c.c <Button-1> {.text insert insert "%x %y "
                      .c.c delete lastclick
                      .c.c create text %x %y -text X -fill red -tag lastclick}
bind .c.c <Button-2> {.text insert insert "[expr (%x+5)/10]0 [expr (%y+5)/10]0 "
                      .c.c delete lastclick
                      .c.c create text [expr (%x+5)/10]0 [expr (%y+5)/10]0 \
                           -text X -fill red -tag lastclick}
bind .c.c <Button-3> {.c.c delete lastclick
                      .c.c create text %x %y -text X -fill red -tag lastclick}

newop tcl  {global fn;.c.c postscript -file "$fn.ps"}
newop tcl  {.text insert insert [eval [selection get]]\n}
newop tcl  {fresh-canvas}
newop tcl  {draw-canvas}
newop tcl  {fresh-text}
newop tcl  {insert-canvas}
newop tcl  {finditem}
newop tcl  {deleteitem}
newop tcl  {renewitem}
newop tcl  {eval [selection get]}

proc creation-cmd {id} {
    set cmd {drawitem}
    set cmd "$cmd [lindex [.c.c gettags $id] 0]"
    set cmd "$cmd [.c.c type $id]"
    set cmd "$cmd [.c.c coords $id]"
    foreach x [.c.c itemconfig $id] {
        if [string length [lindex $x 4]] {
            set cmd "$cmd [lindex $x 0] {[lindex $x 4]}"
        }
    }
    return $cmd
}

proc fresh-canvas {} {
    foreach x [.c.c find all] {
        .c.c delete $x
    }
}

proc fresh-text {} {
    .text delete 1.0 end
}

proc draw-canvas {} {
    eval [.text get 1.0 end]
}

proc insert-canvas {} {
    .c.c delete lastclick
    foreach x [.c.c find all] {
        .text insert insert "[creation-cmd $x]\n\n"
    }
}

proc newitem {id type args} {
    set cmd "drawitem $id $type $args -tag $id\n\n"
    .text insert insert $cmd
    eval $cmd
}

proc renewitem {args} {
    if [string length $args] {
        .text insert insert [creation-cmd $args]\n
    } {
        .text insert insert [creation-cmd [lindex [selection get] 1]]\n
    }
}

proc deleteitem {args} {
    if [string length $args] {
        .c.c delete $args
    } {
        .c.c delete [lindex [selection get] 1]
    }
}

proc drawitem {id type args} {
    eval ".c.c delete $id; .c.c create $type $args -tags $id"
}

proc moveitem {tag x1 y1 x2 y2} {
    .c.c move $tag [xdiff $x1 $y1 $x2 $y2] [ydiff $x1 $y1 $x2 $y2]
}

proc scaleitem {tag xscale yscale} {
  .c.c scale $tag [lindex [.c.c coords $tag] 0] [lindex [.c.c coords $tag] 1] \
              $xscale $yscale
}

proc finditem {} {
    set x [lindex [.c.c coords lastclick] 0]
    set y [lindex [.c.c coords lastclick] 1]
    .c.c delete lastclick
    newop search "item [.c.c gettags [.c.c find closest $x $y]]"
}

proc xdiff {x1 y1 x2 y2} {return [expr $x2-$x1]}
proc ydiff {x1 y1 x2 y2} {return [expr $y2-$y1]}
proc xysq  {x1 y1 x2 y2} {return "$x1 $y1 $x2 [expr $y1+$x2-$x1]"}
