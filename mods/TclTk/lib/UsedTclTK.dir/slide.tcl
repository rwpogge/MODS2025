catch { destroy .menu }
menu .menu
bind .menu <ButtonPress-1> { .menu post 300 10 };
#bind .c.c <ButtonPress-1> { .menu post 300 10 };

proc tb24 {} {return "-*-times-bold-r-normal--*-240-*"}
proc tm18 {} {return "-*-times-medium-r-normal--*-180-*"}
proc tm14 {} {return "-*-courier-medium-r-normal--*-140-*"}
proc tm12 {} {return "-*-times-medium-r-normal--*-120-*"}
proc cm18 {} {return "-*-courier-medium-r-normal--*-180-*"}
proc cm6 {} {return "-*-helvetica-medium-r-normal--*-80-*"}


proc defslide {name items} {
    .menu add command -label $name -command [
        append cmd fresh-canvas\n
        append cmd "drawitem logo bitmap 5 5 -bitmap @alc.xbm -anchor nw\n"
        append cmd "drawitem title text 200 15 -text [list $name] "
        append cmd     "-font [tm18] -anchor nw\n"
        append cmd $items\n
        append cmd ".menu unpost"
    ]
}

defslide cover {
drawitem face bitmap 320 340 -anchor s -bitmap @face.xbm
drawitem slide text 320 70 \
    -anchor n -width 630 -justify center -font [tb24] -text {
X Windows Scripting
with
Tcl/Tk}
drawitem name text 320 345 -anchor n -font [tm12] -text {John Ousterhout}}

defslide outline {
drawitem s1 text 5 40 -anchor nw -width 630 -font [tm18] -text {
          tcl language concepts
              - general architecture
              - everything is text
              - simple C interface
              - simplified expansion rules
              - peculiarities
              - packages

          overview of the tk widget set
              - the hello program
              - general concepts
              - geometry management
              - bitmaps and xv
}
drawitem s2 text 310 40 -anchor nw -width 630 -font [tm18] -text {
          demo
              - xd, xe, xls, vxref, panel
              - edv-browse, xnearest
              - newsreader, mailreader
              - /apps/Unsupported/lib/tk/demos
}}


defslide tcl_general_architecture {
drawitem s1 text 5 40 -anchor nw -width 630 -font [tm18] -text {
Everything is organized into shell-like commands:}
drawitem s2 text 5 100 -anchor nw -width 630 -font [cm18] -text {
    variables:             set a foo
    files:                 puts $file "hello world\n"
    processes/pipes:       exec du -s | sort -n
    arithmetic:            expr (23+56)/162.0
    flow control:          if [expr $value<0] {
                               puts "It was negative"
                           } {
                               puts "It was not negative"
                           }
}}

defslide tcl_everything_text {
drawitem s1 text 5 60 -anchor nw -width 630 -font [tm18] -text {
Though shell-like, the command structure is *very* simple,
      organized as lists of white-space separated words,
            with two quoting conventions: {} and "",
                and two substitution notations:  [command] and $variable
}
drawitem s2 text 5 150 -anchor nw -width 630 -font [cm18] -text {
expr pow($a*$a+$b*$b,.5)
puts stderr {Now is the time for all good errors...}
puts "a is $a"
set a [if [expr $a<0] {expr 0-$a} {expr $a}]
}}

defslide tcl_C_interface {
drawitem s1 text 5 40 -anchor nw -width 630 -font [tm18] -text {
Tcl is intended to be extendable in C,
so tcl commands are implemented as C procedures
following the usual interface:
}
drawitem s2 text 5 150 -anchor nw -width 630 -font [cm18] -text {
        int cmd( int argc, char **argv );
}}

defslide tcl_expansion_rules {
drawitem s1 text 5 40 -anchor nw -width 630 -font [tm18] -text {
        - there is no rescanning
        - text inserted via $ and [] cannot form a word break,
            even if/when it includes whitespace
        - explicit "eval" cleanly applied
            to constructs of "list"
        - substitutions are only made into "" quoted text,
            and the nesting of {} quoted text is important
}
drawitem s2 text 5 180 -anchor nw -width 630 -font [cm18] -text {
format {string is %s} "foo"
button .foo -command { ... %x,%y ... }

eval "cmd $foo $bar"   vs   eval [list cmd $foo $bar]
    vs   eval {list cmd $foo $bar}

cmd $foo   vs   cmd "$foo"   vs   cmd {$foo}

if [expr $something] {} {}    vs   if {$something} {} {}
         vs if $something {} {}
}}

defslide tcl_peculiarities {
drawitem s1 text 5 40 -anchor nw -width 630 -font [tm18] -text {
most common confusions:

      - no resubstitution
      - confusing [] with {}
      - timing of expansions within {}
      - expecting "expr" to do more than it does
      - comments within {} or []
}
drawitem s2 text 5 180 -anchor nw -width 630 -font [cm18] -text {
    set a "b c";    cmd $a

    while [some-condition] {foo}

    proc p {$args} { ... }
    button .foo -command {manipulate $state_of_the_world}

    expr 1/2
}}

defslide tcl_packages {
drawitem s1 text 5 40 -anchor nw -width 630 -font [tm18] -text {
    tcl		base package, with variables, control structures,
		list manipulation, regular expressions, etc
    tk		X windows widget set
    tclX	extended tcl, such as data structures, time arithmetic, etc
    [incr tcl]	(alias "itcl") object oriented extension
    dp		interface to sockets for distributed programming
    expect	drive interactive character-oriented programs from scripts
    tkSteal	manipulate X interactions of other programs
		(eg: have emacs or xterm as a widget inside a tk app)
    dl		dynamic loading of .o, .a, and shared library files

along with html parsing, video image filtering, telephone answering machine dialing, and many, many more (including tcl versions for DOS, OS/2 and NT, and tk versions for Microsoft Windows)
}}

defslide tk_hello {
drawitem s2 text 5 40 -anchor nw -width 630 -font [cm18] -text {
#!/usr/bin/env wish-f
button .hello -text "hello, world" -command {destroy .}
pack .hello}
drawitem s3 text 5 120 -anchor nw -width 500 -font [cm6] \
    -text [exec grep -n ^ xhello.c | head -26]
drawitem s4 text 300 120 -anchor nw -width 500 -font [cm6] \
    -text [exec grep -n ^ xhello.c | tail -26]
}

defslide tk_general_concepts {
drawitem s1 text 5 40 -anchor nw -width 630 -font [tm18] -text {
    - each widget has a creation command,
        which in turn creates "widget commands"
        (thus, the tk widget namespace is tucked away
                    inside the tcl command namespace)
    - the "bind" command regulates responses of
        the widgets to various events in the X event loop
    - events propogate between widgets via callbacks
}
drawitem s2 text 5 180 -anchor nw -width 630 -font [cm18] -text {

scrollbar .scroll -command ".text yview"
text .text -yscroll ".scroll set"

bind Text <Down>  {%W mark set insert "insert + 1 lines"}
bind Text <ButtonRelease-3> {
    %W mark set insert @%x,%y
    %W insert insert [selection get]
}
}}

defslide tk_geometry_management {
drawitem s1 text 5 40 -anchor nw -width 630 -font [tm18] -text {
    - done via the "pack" command
    - widgets can be instructed to be as small as possible,
         or they can expand to take any available space
    - each widget can be placed relative to any preexisting
         one, or at any orientation within its parent
         (which means that simple placement is easy)
    - XF
}
drawitem s2 text 5 200 -anchor nw -width 630 -font [cm18] -text {

foreach b {quit write commit revert tcl 
           tclop mark search filter} {
    pack .panel.$b -side left
}
pack .panel -side top -fill x
pack .scroll -side left -fill y
pack .text -side left -fill x -fill y
}}

# defslide foo {
# drawitem s1 text 5 40 -anchor nw -width 630 -font [tm18] -text {
# }
# drawitem s2 text 5 180 -anchor nw -width 630 -font [cm18] -text {
# }}

defslide additional_info {

}

.menu post 300 10


