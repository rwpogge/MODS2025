#!/usr/bin/wish -f
# Program: ISL_DSP
# Tcl version: 7.3 (Tcl/Tk/XF)
# Tk version: 3.6
# XF version: 2.3
#


# procedure to show window .
proc ShowWindow. {args} {# xf ignore me 7

  # Window manager configurations
  global tkVersion
  wm positionfrom . user
  wm sizefrom . ""
  wm maxsize . 1152 900
  wm title . {ISL(Displays)}

  set bfont {-*-lucida-bold-r-*-sans-16-*-*-*-*-*-*-*}
  set bfg {black}
  set bbg {goldenrod}

  # build widget .frame0
  frame .frame0 \
    -background {black} \
    -borderwidth {5} \
    -relief {raised}

  # build widget .frame0.button1
  button .frame0.button1 \
    -font $bfont \
    -command { helper } \
    -foreground $bfg \
    -background $bbg \
    -text {HELP}
#    -text {ISL StripChart}

  # build widget .frame0.button2
  button .frame0.button2 \
    -font $bfont \
    -command {exec /usr/bin/X11/netscape &} \
    -foreground $bfg \
    -background $bbg \
    -text {Netscape}

  # build widget .frame0.button3
  button .frame0.button3 \
    -font $bfont \
    -command {exec /usr/bin/X11/xterm &} \
    -foreground $bfg \
    -background $bbg \
    -text {xterm}

  # build widget .frame0.button4
  button .frame0.button4 \
    -background {blue} \
    -font $bfont \
    -foreground {white} \
    -text { - }

  # build widget .frame0.button5
  button .frame0.button5 \
    -background {blue} \
    -font $bfont \
    -foreground {white} \
    -text { - }

  # build widget .frame0.button6
  button .frame0.button6 \
    -background {blue} \
    -command {exec /users/rgonzale/kitts/xf.dir/Xcdisplay.dir/artslite &} \
    -font $bfont \
    -foreground {white} \
    -text {SENSORS}

  # build widget .frame0.button7
  button .frame0.button7 \
    -background {blue} \
    -font $bfont \
    -foreground {white} \
    -text { - }

  # build widget .frame0.button8
  button .frame0.button8 \
    -background {blue} \
    -font $bfont \
    -foreground {white} \
    -text { - }

  # build widget .frame0.button9
  button .frame0.button9 \
    -background {blue} \
    -font $bfont \
    -foreground {white} \
    -text { - }

  # build widget .frame0.button10
  button .frame0.button10 \
    -background {blue} \
    -font $bfont \
    -foreground {white} \
    -text {BLANK}

  # build widget .frame0.button11
  button .frame0.button11 \
    -background {blue} \
    -font $bfont \
    -foreground {white} \
    -text {BLANK}

  # build widget .frame0.button12
  button .frame0.button12 \
    -background {blue} \
    -font $bfont \
    -foreground {white} \
    -text {BLANK}

  # build widget .frame0.button13
  button .frame0.button13 \
    -command {destroy .} \
    -font $bfont \
    -foreground $bfg \
    -background {red} \
    -text {EXIT}

  # pack widget .frame0
  pack append .frame0 \
    .frame0.button1 {top frame center expand fillx} \
    .frame0.button13 {top frame center fillx}
#    .frame0.button2 {top frame center fillx} \
#    .frame0.button3 {top frame center fillx} \
#   .frame0.button4 {top frame center fillx} \
#    .frame0.button5 {top frame center fillx} \
#    .frame0.button6 {top frame center fillx} \
#    .frame0.button7 {top frame center fillx} \
#    .frame0.button8 {top frame center fillx} \
#    .frame0.button9 {top frame center fillx} \
#    .frame0.button10 {top frame center fillx} \
#    .frame0.button11 {top frame center fillx} \
#    .frame0.button12 {top frame center fillx} \

  # pack widget .
  pack append . \
    .frame0 {top frame center}

  if {"[info procs XFEdit]" != ""} {
    catch "XFMiscBindWidgetTree ."
    after 2 "catch {XFEditSetShowWindows}"
  }
}


# Procedure: Alias
if {"[info procs Alias]" == ""} {
proc Alias { args} {
# xf ignore me 7
##########
# Procedure: Alias
# Description: establish an alias for a procedure
# Arguments: args - no argument means that a list of all aliases
#                   is returned. Otherwise the first parameter is
#                   the alias name, and the second parameter is
#                   the procedure that is aliased.
# Returns: nothing, the command that is bound to the alias or a
#          list of all aliases - command pairs. 
# Sideeffects: internalAliasList is updated, and the alias
#              proc is inserted
##########
  global internalAliasList

  if {[llength $args] == 0} {
    return $internalAliasList
  } {
    if {[llength $args] == 1} {
      set xfTmpIndex [lsearch $internalAliasList "[lindex $args 0] *"]
      if {$xfTmpIndex != -1} {
        return [lindex [lindex $internalAliasList $xfTmpIndex] 1]
      }
    } {
      if {[llength $args] == 2} {
        eval "proc [lindex $args 0] {args} {#xf ignore me 4
return \[eval \"[lindex $args 1] \$args\"\]}"
        set xfTmpIndex [lsearch $internalAliasList "[lindex $args 0] *"]
        if {$xfTmpIndex != -1} {
          set internalAliasList [lreplace $internalAliasList $xfTmpIndex $xfTmpIndex "[lindex $args 0] [lindex $args 1]"]
        } {
          lappend internalAliasList "[lindex $args 0] [lindex $args 1]"
        }
      } {
        error "Alias: wrong number or args: $args"
      }
    }
  }
}
}


# Procedure: GetSelection
if {"[info procs GetSelection]" == ""} {
proc GetSelection {} {
# xf ignore me 7
##########
# Procedure: GetSelection
# Description: get current selection
# Arguments: none
# Returns: none
# Sideeffects: none
##########

  # the save way
  set xfSelection ""
  catch "selection get" xfSelection
  if {"$xfSelection" == "selection doesn't exist or form \"STRING\" not defined"} {
    return ""
  } {
    return $xfSelection
  }
}
}


# Procedure: MenuPopupAdd
if {"[info procs MenuPopupAdd]" == ""} {
proc MenuPopupAdd { xfW xfButton xfMenu {xfModifier ""} {xfCanvasTag ""}} {
# xf ignore me 7
# the popup menu handling is from (I already gave up with popup handling :-):
#
# Copyright 1991,1992 by James Noble.
# Everyone is granted permission to copy, modify and redistribute.
# This notice must be preserved on all copies or derivates.
#
##########
# Procedure: MenuPopupAdd
# Description: attach a popup menu to widget
# Arguments: xfW - the widget
#            xfButton - the button we use
#            xfMenu - the menu to attach
#            {xfModifier} - a optional modifier
#            {xfCanvasTag} - a canvas tagOrId
# Returns: none
# Sideeffects: none
##########
  global tk_popupPriv

  set tk_popupPriv($xfMenu,focus) ""
  set tk_popupPriv($xfMenu,grab) ""
  if {"$xfModifier" != ""} {
    set press "$xfModifier-"
    set motion "$xfModifier-"
    set release "Any-"
  } {
    set press ""
    set motion ""
    set release ""
  }

  bind $xfMenu "<${motion}B${xfButton}-Motion>"  "MenuPopupMotion $xfMenu %W %X %Y"
  bind $xfMenu "<${release}ButtonRelease-${xfButton}>"  "MenuPopupRelease $xfMenu %W"
  if {"$xfCanvasTag" == ""} {
    bind $xfW "<${press}ButtonPress-${xfButton}>"  "MenuPopupPost $xfMenu %X %Y"
    bind $xfW "<${release}ButtonRelease-${xfButton}>"  "MenuPopupRelease $xfMenu %W"
  } {
    $xfW bind $xfCanvasTag "<${press}ButtonPress-${xfButton}>"  "MenuPopupPost $xfMenu %X %Y"
    $xfW bind $xfCanvasTag "<${release}ButtonRelease-${xfButton}>"  "MenuPopupRelease $xfMenu %W"
  }
}
}


# Procedure: MenuPopupMotion
if {"[info procs MenuPopupMotion]" == ""} {
proc MenuPopupMotion { xfMenu xfW xfX xfY} {
# xf ignore me 7
##########
# Procedure: MenuPopupMotion
# Description: handle the popup menu motion
# Arguments: xfMenu - the topmost menu
#            xfW - the menu
#            xfX - the root x coordinate
#            xfY - the root x coordinate
# Returns: none
# Sideeffects: none
##########
  global tk_popupPriv

  if {"[info commands $xfW]" != "" && [winfo ismapped $xfW] &&
      "[winfo class $xfW]" == "Menu" &&
      [info exists tk_popupPriv($xfMenu,focus)] &&
      "$tk_popupPriv($xfMenu,focus)" != "" &&
      [info exists tk_popupPriv($xfMenu,grab)] &&
      "$tk_popupPriv($xfMenu,grab)" != ""} {
    set xfPopMinX [winfo rootx $xfW]
    set xfPopMaxX [expr $xfPopMinX+[winfo width $xfW]]
    if {$xfX >= $xfPopMinX && $xfX <= $xfPopMaxX} {
      $xfW activate @[expr $xfY-[winfo rooty $xfW]]
      if {![catch "$xfW entryconfig @[expr $xfY-[winfo rooty $xfW]] -menu" result]} {
        if {"[lindex $result 4]" != ""} {
          foreach binding [bind $xfMenu] {
            bind [lindex $result 4] $binding [bind $xfMenu $binding]
          }
        }
      }
    } {
      $xfW activate none
    }
  }
}
}


# Procedure: MenuPopupPost
if {"[info procs MenuPopupPost]" == ""} {
proc MenuPopupPost { xfMenu xfX xfY} {
# xf ignore me 7
##########
# Procedure: MenuPopupPost
# Description: post the popup menu
# Arguments: xfMenu - the menu
#            xfX - the root x coordinate
#            xfY - the root x coordinate
# Returns: none
# Sideeffects: none
##########
  global tk_popupPriv

  if {"[info commands $xfMenu]" != ""} {
    if {![info exists tk_popupPriv($xfMenu,focus)]} {
      set tk_popupPriv($xfMenu,focus) [focus]
    } {
      if {"$tk_popupPriv($xfMenu,focus)" == ""} {
        set tk_popupPriv($xfMenu,focus) [focus]
      }
    }
    set tk_popupPriv($xfMenu,grab) $xfMenu

    catch "$xfMenu activate none"
    catch "$xfMenu post $xfX $xfY"
    catch "focus $xfMenu"
    catch "grab -global $xfMenu"
  }
}
}


# Procedure: MenuPopupRelease
if {"[info procs MenuPopupRelease]" == ""} {
proc MenuPopupRelease { xfMenu xfW} {
# xf ignore me 7
##########
# Procedure: MenuPopupRelease
# Description: remove the popup menu
# Arguments: xfMenu - the topmost menu widget
#            xfW - the menu widget
# Returns: none
# Sideeffects: none
##########
  global tk_popupPriv
  global tkVersion

  if {"[info commands $xfW]" != "" && [winfo ismapped $xfW] &&
      "[winfo class $xfW]" == "Menu" &&
      [info exists tk_popupPriv($xfMenu,focus)] &&
      "$tk_popupPriv($xfMenu,focus)" != "" &&
      [info exists tk_popupPriv($xfMenu,grab)] &&
      "$tk_popupPriv($xfMenu,grab)" != ""} {
    if {$tkVersion >= 3.0} {
      catch "grab release $tk_popupPriv($xfMenu,grab)"
    } {
      catch "grab none"
    }
    catch "focus $tk_popupPriv($xfMenu,focus)"
    set tk_popupPriv($xfMenu,focus) ""
    set tk_popupPriv($xfMenu,grab) ""
    if {"[$xfW index active]" != "none"} {
      $xfW invoke active; catch "$xfMenu unpost"
    }
  }
  catch "$xfMenu unpost"
}
}


# Procedure: NoFunction
if {"[info procs NoFunction]" == ""} {
proc NoFunction { args} {
# xf ignore me 7
##########
# Procedure: NoFunction
# Description: do nothing
# Arguments: args - any kind of arguments
# Returns: none
# Sideeffects: none
##########
}
}


# Procedure: SN
if {"[info procs SN]" == ""} {
proc SN { {xfName ""}} {
# xf ignore me 7
##########
# Procedure: SN
# Description: map a symbolic name to the widget path
# Arguments: xfName
# Returns: the symbolic name
# Sideeffects: none
##########

  SymbolicName $xfName
}
}


# Procedure: SymbolicName
if {"[info procs SymbolicName]" == ""} {
proc SymbolicName { {xfName ""}} {
# xf ignore me 7
##########
# Procedure: SymbolicName
# Description: map a symbolic name to the widget path
# Arguments: xfName
# Returns: the symbolic name
# Sideeffects: none
##########

  global symbolicName

  if {"$xfName" != ""} {
    set xfArrayName ""
    append xfArrayName symbolicName ( $xfName )
    if {![catch "set \"$xfArrayName\"" xfValue]} {
      return $xfValue
    } {
      if {"[info commands XFProcError]" != ""} {
        XFProcError "Unknown symbolic name:\n$xfName"
      } {
        puts stderr "XF error: unknown symbolic name:\n$xfName"
      }
    }
  }
  return ""
}
}


# Procedure: Unalias
if {"[info procs Unalias]" == ""} {
proc Unalias { aliasName} {
# xf ignore me 7
##########
# Procedure: Unalias
# Description: remove an alias for a procedure
# Arguments: aliasName - the alias name to remove
# Returns: none
# Sideeffects: internalAliasList is updated, and the alias
#              proc is removed
##########
  global internalAliasList

  set xfIndex [lsearch $internalAliasList "$aliasName *"]
  if {$xfIndex != -1} {
    rename $aliasName ""
    set internalAliasList [lreplace $internalAliasList $xfIndex $xfIndex]
  }
}
}



# application parsing procedure
proc XFLocalParseAppDefs {xfAppDefFile} {
  global xfAppDefaults

  # basically from: Michael Moore
  if {[file exists $xfAppDefFile] &&
      [file readable $xfAppDefFile] &&
      "[file type $xfAppDefFile]" == "link"} {
    catch "file type $xfAppDefFile" xfType
    while {"$xfType" == "link"} {
      if {[catch "file readlink $xfAppDefFile" xfAppDefFile]} {
        return
      }
      catch "file type $xfAppDefFile" xfType
    }
  }
  if {!("$xfAppDefFile" != "" &&
        [file exists $xfAppDefFile] &&
        [file readable $xfAppDefFile] &&
        "[file type $xfAppDefFile]" == "file")} {
    return
  }
  if {![catch "open $xfAppDefFile r" xfResult]} {
    set xfAppFileContents [read $xfResult]
    close $xfResult
    foreach line [split $xfAppFileContents "\n"] {
      # backup indicates how far to backup.  It applies to the
      # situation where a resource name ends in . and when it
      # ends in *.  In the second case you want to keep the *
      # in the widget name for pattern matching, but you want
      # to get rid of the . if it is the end of the name. 
      set backup -2  
      set line [string trim $line]
      if {[string index $line 0] == "#" || "$line" == ""} {
        # skip comments and empty lines
        continue
      }
      set list [split $line ":"]
      set resource [string trim [lindex $list 0]]
      set i [string last "." $resource]
      set j [string last "*" $resource]
      if {$j > $i} { 
        set i $j
        set backup -1
      }
      incr i
      set name [string range $resource $i end]
      incr i $backup
      set widname [string range $resource 0 $i]
      set value [string trim [lindex $list 1]]
      if {"$widname" != "" && "$widname" != "*"} {
        # insert the widget and resourcename to the application
        # defaults list.
        if {![info exists xfAppDefaults]} {
          set xfAppDefaults ""
        }
        lappend xfAppDefaults [list $widname [string tolower $name] $value]
      }
    }
  }
}

# application loading procedure
proc XFLocalLoadAppDefs {{xfClasses ""} {xfPriority "startupFile"} {xfAppDefFile ""}} {
  global env

  if {"$xfAppDefFile" == ""} {
    set xfFileList ""
    if {[info exists env(XUSERFILESEARCHPATH)]} {
      append xfFileList [split $env(XUSERFILESEARCHPATH) :]
    }
    if {[info exists env(XAPPLRESDIR)]} {
      append xfFileList [split $env(XAPPLRESDIR) :]
    }
    if {[info exists env(XFILESEARCHPATH)]} {
      append xfFileList [split $env(XFILESEARCHPATH) :]
    }
    append xfFileList " /usr/lib/X11/app-defaults"
    append xfFileList " /usr/X11/lib/X11/app-defaults"

    foreach xfCounter1 $xfClasses {
      foreach xfCounter2 $xfFileList {
        set xfPathName $xfCounter2
        if {[regsub -all "%N" "$xfPathName" "$xfCounter1" xfResult]} {
          set xfPathName $xfResult
        }
        if {[regsub -all "%T" "$xfPathName" "app-defaults" xfResult]} {
          set xfPathName $xfResult
        }
        if {[regsub -all "%S" "$xfPathName" "" xfResult]} {
          set xfPathName $xfResult
        }
        if {[regsub -all "%C" "$xfPathName" "" xfResult]} {
          set xfPathName $xfResult
        }
        if {[file exists $xfPathName] &&
            [file readable $xfPathName] &&
            ("[file type $xfPathName]" == "file" ||
             "[file type $xfPathName]" == "link")} {
          catch "option readfile $xfPathName $xfPriority"
          if {"[info commands XFParseAppDefs]" != ""} {
            XFParseAppDefs $xfPathName
          } {
            if {"[info commands XFLocalParseAppDefs]" != ""} {
              XFLocalParseAppDefs $xfPathName
            }
          }
        } {
          if {[file exists $xfCounter2/$xfCounter1] &&
              [file readable $xfCounter2/$xfCounter1] &&
              ("[file type $xfCounter2/$xfCounter1]" == "file" ||
               "[file type $xfCounter2/$xfCounter1]" == "link")} {
            catch "option readfile $xfCounter2/$xfCounter1 $xfPriority"
            if {"[info commands XFParseAppDefs]" != ""} {
              XFParseAppDefs $xfCounter2/$xfCounter1
            } {
              if {"[info commands XFLocalParseAppDefs]" != ""} {
                XFLocalParseAppDefs $xfCounter2/$xfCounter1
              }
            }
          }
        }
      }
    }
  } {
    # load a specific application defaults file
    if {[file exists $xfAppDefFile] &&
        [file readable $xfAppDefFile] &&
        ("[file type $xfAppDefFile]" == "file" ||
         "[file type $xfAppDefFile]" == "link")} {
      catch "option readfile $xfAppDefFile $xfPriority"
      if {"[info commands XFParseAppDefs]" != ""} {
        XFParseAppDefs $xfAppDefFile
      } {
        if {"[info commands XFLocalParseAppDefs]" != ""} {
          XFLocalParseAppDefs $xfAppDefFile
        }
      }
    }
  }
}

# application setting procedure
proc XFLocalSetAppDefs {{xfWidgetPath "."}} {
  global xfAppDefaults

  if {![info exists xfAppDefaults]} {
    return
  }
  foreach xfCounter $xfAppDefaults {
    if {"$xfCounter" == ""} {
      break
    }
    set widname [lindex $xfCounter 0]
    if {[string match $widname ${xfWidgetPath}] ||
        [string match "${xfWidgetPath}*" $widname]} {
      set name [string tolower [lindex $xfCounter 1]]
      set value [lindex $xfCounter 2]
      # Now lets see how many tcl commands match the name
      # pattern specified.
      set widlist [info command $widname]
      if {"$widlist" != ""} {
        foreach widget $widlist {
          # make sure this command is a widget.
          if {![catch "winfo id $widget"] &&
              [string match "${xfWidgetPath}*" $widget]} {
            catch "$widget configure -$name $value" 
          }
        }
      }
    }
  }
}


# initialize global variables
global {fontInFile}
set {fontInFile} {file4}
global {fontLine}
set {fontLine} {}
global {fontReadList}
set {fontReadList} {-*-menu-medium-r-normal--*-120-*-*-p-*-iso8859-1
-*-terminal-*-*-*-*-*-180-*-*-*-*-*-*
-*-terminal-medium-*-normal--36-*-*-*-*-*-*-1
-*-terminal-medium-r-narrow--*-140-*-*-c-*-iso8859-1
--------0------
-adobe-courier-bold-o-normal--10-100-75-75-m-60-hp-roman8
-adobe-courier-bold-o-normal--11-80-100-100-m-60-iso8859-1
-adobe-courier-bold-o-normal--12-120-75-75-m-70-hp-roman8
-adobe-courier-bold-o-normal--14-100-100-100-m-90-iso8859-1
-adobe-courier-bold-o-normal--14-140-75-75-m-90-hp-roman8
-adobe-courier-bold-o-normal--17-120-100-100-m-100-iso8859-1
-adobe-courier-bold-o-normal--18-180-75-75-m-110-hp-roman8
-adobe-courier-bold-o-normal--20-140-100-100-m-110-iso8859-1
-adobe-courier-bold-o-normal--24-240-75-75-m-150-hp-roman8
-adobe-courier-bold-o-normal--25-180-100-100-m-150-iso8859-1
-adobe-courier-bold-o-normal--34-240-100-100-m-200-iso8859-1
-adobe-courier-bold-o-normal--8-80-75-75-m-50-hp-roman8
-adobe-courier-bold-r-normal--10-100-75-75-m-60-hp-roman8
-adobe-courier-bold-r-normal--11-80-100-100-m-60-iso8859-1
-adobe-courier-bold-r-normal--12-120-75-75-m-70-hp-roman8
-adobe-courier-bold-r-normal--14-100-100-100-m-90-iso8859-1
-adobe-courier-bold-r-normal--14-140-75-75-m-90-hp-roman8
-adobe-courier-bold-r-normal--17-120-100-100-m-100-iso8859-1
-adobe-courier-bold-r-normal--18-180-75-75-m-110-hp-roman8
-adobe-courier-bold-r-normal--20-140-100-100-m-110-iso8859-1
-adobe-courier-bold-r-normal--24-240-75-75-m-150-hp-roman8
-adobe-courier-bold-r-normal--25-180-100-100-m-150-iso8859-1
-adobe-courier-bold-r-normal--34-240-100-100-m-200-iso8859-1
-adobe-courier-bold-r-normal--8-80-75-75-m-50-hp-roman8
-adobe-courier-medium-o-normal--10-100-75-75-m-60-hp-roman8
-adobe-courier-medium-o-normal--11-80-100-100-m-60-iso8859-1
-adobe-courier-medium-o-normal--12-120-75-75-m-70-hp-roman8
-adobe-courier-medium-o-normal--14-100-100-100-m-90-iso8859-1
-adobe-courier-medium-o-normal--14-140-75-75-m-90-hp-roman8
-adobe-courier-medium-o-normal--17-120-100-100-m-100-iso8859-1
-adobe-courier-medium-o-normal--18-180-75-75-m-110-hp-roman8
-adobe-courier-medium-o-normal--20-140-100-100-m-110-iso8859-1
-adobe-courier-medium-o-normal--24-240-75-75-m-150-hp-roman8
-adobe-courier-medium-o-normal--25-180-100-100-m-150-iso8859-1
-adobe-courier-medium-o-normal--34-240-100-100-m-200-iso8859-1
-adobe-courier-medium-o-normal--8-80-75-75-m-50-hp-roman8
-adobe-courier-medium-r-normal--10-100-75-75-m-60-hp-roman8
-adobe-courier-medium-r-normal--11-80-100-100-m-60-iso8859-1
-adobe-courier-medium-r-normal--12-120-75-75-m-70-hp-roman8
-adobe-courier-medium-r-normal--14-100-100-100-m-90-iso8859-1
-adobe-courier-medium-r-normal--14-140-75-75-m-90-hp-roman8
-adobe-courier-medium-r-normal--17-120-100-100-m-100-iso8859-1
-adobe-courier-medium-r-normal--18-180-75-75-m-110-hp-roman8
-adobe-courier-medium-r-normal--20-140-100-100-m-110-iso8859-1
-adobe-courier-medium-r-normal--24-240-75-75-m-150-hp-roman8
-adobe-courier-medium-r-normal--25-180-100-100-m-150-iso8859-1
-adobe-courier-medium-r-normal--34-240-100-100-m-200-iso8859-1
-adobe-courier-medium-r-normal--8-80-75-75-m-50-hp-roman8
-adobe-helvetica-bold-o-normal--10-100-75-75-p-60-hp-roman8
-adobe-helvetica-bold-o-normal--11-80-100-100-p-60-iso8859-1
-adobe-helvetica-bold-o-normal--12-120-75-75-p-69-hp-roman8
-adobe-helvetica-bold-o-normal--14-100-100-100-p-82-iso8859-1
-adobe-helvetica-bold-o-normal--14-140-75-75-p-82-hp-roman8
-adobe-helvetica-bold-o-normal--17-120-100-100-p-92-iso8859-1
-adobe-helvetica-bold-o-normal--18-180-75-75-p-104-hp-roman8
-adobe-helvetica-bold-o-normal--20-140-100-100-p-103-iso8859-1
-adobe-helvetica-bold-o-normal--24-240-75-75-p-138-hp-roman8
-adobe-helvetica-bold-o-normal--25-180-100-100-p-138-iso8859-1
-adobe-helvetica-bold-o-normal--34-240-100-100-p-182-iso8859-1
-adobe-helvetica-bold-o-normal--8-80-75-75-p-50-hp-roman8
-adobe-helvetica-bold-r-normal--10-100-75-75-p-60-hp-roman8
-adobe-helvetica-bold-r-normal--11-80-100-100-p-60-iso8859-1
-adobe-helvetica-bold-r-normal--12-120-75-75-p-70-hp-roman8
-adobe-helvetica-bold-r-normal--14-100-100-100-p-82-iso8859-1
-adobe-helvetica-bold-r-normal--14-140-75-75-p-82-hp-roman8
-adobe-helvetica-bold-r-normal--17-120-100-100-p-92-iso8859-1
-adobe-helvetica-bold-r-normal--18-180-75-75-p-103-hp-roman8
-adobe-helvetica-bold-r-normal--20-140-100-100-p-105-iso8859-1
-adobe-helvetica-bold-r-normal--24-240-75-75-p-138-hp-roman8
-adobe-helvetica-bold-r-normal--25-180-100-100-p-138-iso8859-1
-adobe-helvetica-bold-r-normal--34-240-100-100-p-182-iso8859-1
-adobe-helvetica-bold-r-normal--8-80-75-75-p-50-hp-roman8
-adobe-helvetica-medium-o-normal--10-100-75-75-p-57-hp-roman8
-adobe-helvetica-medium-o-normal--11-80-100-100-p-57-iso8859-1
-adobe-helvetica-medium-o-normal--12-120-75-75-p-67-hp-roman8
-adobe-helvetica-medium-o-normal--14-100-100-100-p-78-iso8859-1
-adobe-helvetica-medium-o-normal--14-140-75-75-p-78-hp-roman8
-adobe-helvetica-medium-o-normal--17-120-100-100-p-88-iso8859-1
-adobe-helvetica-medium-o-normal--18-180-75-75-p-98-hp-roman8
-adobe-helvetica-medium-o-normal--20-140-100-100-p-98-iso8859-1
-adobe-helvetica-medium-o-normal--24-240-75-75-p-130-hp-roman8
-adobe-helvetica-medium-o-normal--25-180-100-100-p-130-iso8859-1
-adobe-helvetica-medium-o-normal--34-240-100-100-p-176-iso8859-1
-adobe-helvetica-medium-o-normal--8-80-75-75-p-47-hp-roman8
-adobe-helvetica-medium-r-normal--10-100-75-75-p-56-hp-roman8
-adobe-helvetica-medium-r-normal--11-80-100-100-p-56-iso8859-1
-adobe-helvetica-medium-r-normal--12-120-75-75-p-67-hp-roman8
-adobe-helvetica-medium-r-normal--14-100-100-100-p-76-iso8859-1
-adobe-helvetica-medium-r-normal--14-140-75-75-p-77-hp-roman8
-adobe-helvetica-medium-r-normal--17-120-100-100-p-88-iso8859-1
-adobe-helvetica-medium-r-normal--18-180-75-75-p-98-hp-roman8
-adobe-helvetica-medium-r-normal--20-140-100-100-p-100-iso8859-1
-adobe-helvetica-medium-r-normal--24-240-75-75-p-130-hp-roman8
-adobe-helvetica-medium-r-normal--25-180-100-100-p-130-iso8859-1
-adobe-helvetica-medium-r-normal--34-240-100-100-p-176-iso8859-1
-adobe-helvetica-medium-r-normal--8-80-75-75-p-46-hp-roman8
-adobe-new century schoolbook-bold-i-normal--10-100-75-75-p-66-hp-roman8
-adobe-new century schoolbook-bold-i-normal--11-80-100-100-p-66-iso8859-1
-adobe-new century schoolbook-bold-i-normal--12-120-75-75-p-76-hp-roman8
-adobe-new century schoolbook-bold-i-normal--14-100-100-100-p-88-iso8859-1
-adobe-new century schoolbook-bold-i-normal--14-140-75-75-p-88-hp-roman8
-adobe-new century schoolbook-bold-i-normal--17-120-100-100-p-99-iso8859-1
-adobe-new century schoolbook-bold-i-normal--18-180-75-75-p-111-hp-roman8
-adobe-new century schoolbook-bold-i-normal--20-140-100-100-p-111-iso8859-1
-adobe-new century schoolbook-bold-i-normal--24-240-75-75-p-148-hp-roman8
-adobe-new century schoolbook-bold-i-normal--25-180-100-100-p-148-iso8859-1
-adobe-new century schoolbook-bold-i-normal--34-240-100-100-p-193-iso8859-1
-adobe-new century schoolbook-bold-i-normal--8-80-75-75-p-56-hp-roman8
-adobe-new century schoolbook-bold-r-normal--10-100-75-75-p-66-hp-roman8
-adobe-new century schoolbook-bold-r-normal--11-80-100-100-p-66-iso8859-1
-adobe-new century schoolbook-bold-r-normal--12-120-75-75-p-77-hp-roman8
-adobe-new century schoolbook-bold-r-normal--14-100-100-100-p-87-iso8859-1
-adobe-new century schoolbook-bold-r-normal--14-140-75-75-p-87-hp-roman8
-adobe-new century schoolbook-bold-r-normal--17-120-100-100-p-99-iso8859-1
-adobe-new century schoolbook-bold-r-normal--18-180-75-75-p-113-hp-roman8
-adobe-new century schoolbook-bold-r-normal--20-140-100-100-p-113-iso8859-1
-adobe-new century schoolbook-bold-r-normal--24-240-75-75-p-149-hp-roman8
-adobe-new century schoolbook-bold-r-normal--25-180-100-100-p-149-iso8859-1
-adobe-new century schoolbook-bold-r-normal--34-240-100-100-p-193-iso8859-1
-adobe-new century schoolbook-bold-r-normal--8-80-75-75-p-56-hp-roman8
-adobe-new century schoolbook-medium-i-normal--10-100-75-75-p-60-hp-roman8
-adobe-new century schoolbook-medium-i-normal--11-80-100-100-p-60-iso8859-1
-adobe-new century schoolbook-medium-i-normal--12-120-75-75-p-70-hp-roman8
-adobe-new century schoolbook-medium-i-normal--14-100-100-100-p-81-iso8859-1
-adobe-new century schoolbook-medium-i-normal--14-140-75-75-p-81-hp-roman8
-adobe-new century schoolbook-medium-i-normal--17-120-100-100-p-92-iso8859-1
-adobe-new century schoolbook-medium-i-normal--18-180-75-75-p-104-hp-roman8
-adobe-new century schoolbook-medium-i-normal--20-140-100-100-p-104-iso8859-1
-adobe-new century schoolbook-medium-i-normal--24-240-75-75-p-136-hp-roman8
-adobe-new century schoolbook-medium-i-normal--25-180-100-100-p-136-iso8859-1
-adobe-new century schoolbook-medium-i-normal--34-240-100-100-p-182-iso8859-1
-adobe-new century schoolbook-medium-i-normal--8-80-75-75-p-50-hp-roman8
-adobe-new century schoolbook-medium-r-normal--10-100-75-75-p-60-hp-roman8
-adobe-new century schoolbook-medium-r-normal--11-80-100-100-p-60-iso8859-1
-adobe-new century schoolbook-medium-r-normal--12-120-75-75-p-70-hp-roman8
-adobe-new century schoolbook-medium-r-normal--14-100-100-100-p-82-iso8859-1
-adobe-new century schoolbook-medium-r-normal--14-140-75-75-p-82-hp-roman8
-adobe-new century schoolbook-medium-r-normal--17-120-100-100-p-91-iso8859-1
-adobe-new century schoolbook-medium-r-normal--18-180-75-75-p-103-hp-roman8
-adobe-new century schoolbook-medium-r-normal--20-140-100-100-p-103-iso8859-1
-adobe-new century schoolbook-medium-r-normal--24-240-75-75-p-137-hp-roman8
-adobe-new century schoolbook-medium-r-normal--25-180-100-100-p-136-iso8859-1
-adobe-new century schoolbook-medium-r-normal--34-240-100-100-p-181-iso8859-1
-adobe-new century schoolbook-medium-r-normal--8-80-75-75-p-50-hp-roman8
-adobe-symbol-medium-r-normal--11-80-100-100-p-61-adobe-fontspecific
-adobe-symbol-medium-r-normal--14-100-100-100-p-85-adobe-fontspecific
-adobe-symbol-medium-r-normal--17-120-100-100-p-95-adobe-fontspecific
-adobe-symbol-medium-r-normal--20-140-100-100-p-107-adobe-fontspecific
-adobe-symbol-medium-r-normal--25-180-100-100-p-142-adobe-fontspecific
-adobe-symbol-medium-r-normal--34-240-100-100-p-191-adobe-fontspecific
-adobe-times-bold-i-normal--10-100-75-75-p-57-hp-roman8
-adobe-times-bold-i-normal--11-80-100-100-p-57-iso8859-1
-adobe-times-bold-i-normal--12-120-75-75-p-68-hp-roman8
-adobe-times-bold-i-normal--14-100-100-100-p-77-iso8859-1
-adobe-times-bold-i-normal--14-140-75-75-p-77-hp-roman8
-adobe-times-bold-i-normal--17-120-100-100-p-86-iso8859-1
-adobe-times-bold-i-normal--18-180-75-75-p-98-hp-roman8
-adobe-times-bold-i-normal--20-140-100-100-p-98-iso8859-1
-adobe-times-bold-i-normal--24-240-75-75-p-128-hp-roman8
-adobe-times-bold-i-normal--25-180-100-100-p-128-iso8859-1
-adobe-times-bold-i-normal--34-240-100-100-p-170-iso8859-1
-adobe-times-bold-i-normal--8-80-75-75-p-47-hp-roman8
-adobe-times-bold-r-normal--10-100-75-75-p-57-hp-roman8
-adobe-times-bold-r-normal--11-80-100-100-p-57-iso8859-1
-adobe-times-bold-r-normal--12-120-75-75-p-67-hp-roman8
-adobe-times-bold-r-normal--14-100-100-100-p-76-iso8859-1
-adobe-times-bold-r-normal--14-140-75-75-p-77-hp-roman8
-adobe-times-bold-r-normal--17-120-100-100-p-88-iso8859-1
-adobe-times-bold-r-normal--18-180-75-75-p-99-hp-roman8
-adobe-times-bold-r-normal--20-140-100-100-p-100-iso8859-1
-adobe-times-bold-r-normal--24-240-75-75-p-132-hp-roman8
-adobe-times-bold-r-normal--25-180-100-100-p-132-iso8859-1
-adobe-times-bold-r-normal--34-240-100-100-p-177-iso8859-1
-adobe-times-bold-r-normal--8-80-75-75-p-47-hp-roman8
-adobe-times-medium-i-normal--10-100-75-75-p-52-hp-roman8
-adobe-times-medium-i-normal--11-80-100-100-p-52-iso8859-1
-adobe-times-medium-i-normal--12-120-75-75-p-63-hp-roman8
-adobe-times-medium-i-normal--14-100-100-100-p-73-iso8859-1
-adobe-times-medium-i-normal--14-140-75-75-p-73-hp-roman8
-adobe-times-medium-i-normal--17-120-100-100-p-84-iso8859-1
-adobe-times-medium-i-normal--18-180-75-75-p-94-hp-roman8
-adobe-times-medium-i-normal--20-140-100-100-p-94-iso8859-1
-adobe-times-medium-i-normal--24-240-75-75-p-125-hp-roman8
-adobe-times-medium-i-normal--25-180-100-100-p-125-iso8859-1
-adobe-times-medium-i-normal--34-240-100-100-p-168-iso8859-1
-adobe-times-medium-i-normal--8-80-75-75-p-42-hp-roman8
-adobe-times-medium-r-normal--10-100-75-75-p-54-hp-roman8
-adobe-times-medium-r-normal--11-80-100-100-p-54-iso8859-1
-adobe-times-medium-r-normal--12-120-75-75-p-64-hp-roman8
-adobe-times-medium-r-normal--14-100-100-100-p-74-iso8859-1
-adobe-times-medium-r-normal--14-140-75-75-p-74-hp-roman8
-adobe-times-medium-r-normal--17-120-100-100-p-84-iso8859-1
-adobe-times-medium-r-normal--18-180-75-75-p-94-hp-roman8
-adobe-times-medium-r-normal--20-140-100-100-p-96-iso8859-1
-adobe-times-medium-r-normal--24-240-75-75-p-124-hp-roman8
-adobe-times-medium-r-normal--25-180-100-100-p-125-iso8859-1
-adobe-times-medium-r-normal--34-240-100-100-p-170-iso8859-1
-adobe-times-medium-r-normal--8-80-75-75-p-44-hp-roman8
-b&h-lucida-bold-i-normal-sans-11-80-100-100-p-69-iso8859-1
-b&h-lucida-bold-i-normal-sans-14-100-100-100-p-90-iso8859-1
-b&h-lucida-bold-i-normal-sans-17-120-100-100-p-108-iso8859-1
-b&h-lucida-bold-i-normal-sans-20-140-100-100-p-127-iso8859-1
-b&h-lucida-bold-i-normal-sans-25-180-100-100-p-159-iso8859-1
-b&h-lucida-bold-i-normal-sans-26-190-100-100-p-166-iso8859-1
-b&h-lucida-bold-i-normal-sans-34-240-100-100-p-215-iso8859-1
-b&h-lucida-bold-r-normal-sans-*-90-*-*-*-*-*-*
-b&h-lucida-bold-r-normal-sans-11-80-100-100-p-70-iso8859-1
-b&h-lucida-bold-r-normal-sans-14-100-100-100-p-89-iso8859-1
-b&h-lucida-bold-r-normal-sans-17-120-100-100-p-108-iso8859-1
-b&h-lucida-bold-r-normal-sans-20-140-100-100-p-127-iso8859-1
-b&h-lucida-bold-r-normal-sans-25-180-100-100-p-158-iso8859-1
-b&h-lucida-bold-r-normal-sans-26-190-100-100-p-166-iso8859-1
-b&h-lucida-bold-r-normal-sans-34-240-100-100-p-216-iso8859-1
-b&h-lucida-medium-i-normal-sans-11-80-100-100-p-62-iso8859-1
-b&h-lucida-medium-i-normal-sans-14-100-100-100-p-80-iso8859-1
-b&h-lucida-medium-i-normal-sans-17-120-100-100-p-97-iso8859-1
-b&h-lucida-medium-i-normal-sans-20-140-100-100-p-114-iso8859-1
-b&h-lucida-medium-i-normal-sans-25-180-100-100-p-141-iso8859-1
-b&h-lucida-medium-i-normal-sans-26-190-100-100-p-147-iso8859-1
-b&h-lucida-medium-i-normal-sans-34-240-100-100-p-192-iso8859-1
-b&h-lucida-medium-r-normal-sans-*-90-*-*-*-*-*-*
-b&h-lucida-medium-r-normal-sans-11-80-100-100-p-63-iso8859-1
-b&h-lucida-medium-r-normal-sans-14-100-100-100-p-80-iso8859-1
-b&h-lucida-medium-r-normal-sans-17-120-100-100-p-96-iso8859-1
-b&h-lucida-medium-r-normal-sans-20-140-100-100-p-114-iso8859-1
-b&h-lucida-medium-r-normal-sans-25-180-100-100-p-142-iso8859-1
-b&h-lucida-medium-r-normal-sans-26-190-100-100-p-147-iso8859-1
-b&h-lucida-medium-r-normal-sans-34-240-100-100-p-191-iso8859-1
-b&h-lucidabright-demibold-i-normal--11-80-100-100-p-66-iso8859-1
-b&h-lucidabright-demibold-i-normal--14-100-100-100-p-84-iso8859-1
-b&h-lucidabright-demibold-i-normal--17-120-100-100-p-101-iso8859-1
-b&h-lucidabright-demibold-i-normal--20-140-100-100-p-119-iso8859-1
-b&h-lucidabright-demibold-i-normal--25-180-100-100-p-149-iso8859-1
-b&h-lucidabright-demibold-i-normal--26-190-100-100-p-156-iso8859-1
-b&h-lucidabright-demibold-i-normal--34-240-100-100-p-203-iso8859-1
-b&h-lucidabright-demibold-r-normal--11-80-100-100-p-66-iso8859-1
-b&h-lucidabright-demibold-r-normal--14-100-100-100-p-84-iso8859-1
-b&h-lucidabright-demibold-r-normal--17-120-100-100-p-101-iso8859-1
-b&h-lucidabright-demibold-r-normal--20-140-100-100-p-118-iso8859-1
-b&h-lucidabright-demibold-r-normal--25-180-100-100-p-149-iso8859-1
-b&h-lucidabright-demibold-r-normal--26-190-100-100-p-155-iso8859-1
-b&h-lucidabright-demibold-r-normal--34-240-100-100-p-202-iso8859-1
-b&h-lucidabright-medium-i-normal--11-80-100-100-p-63-iso8859-1
-b&h-lucidabright-medium-i-normal--14-100-100-100-p-80-iso8859-1
-b&h-lucidabright-medium-i-normal--17-120-100-100-p-96-iso8859-1
-b&h-lucidabright-medium-i-normal--20-140-100-100-p-113-iso8859-1
-b&h-lucidabright-medium-i-normal--25-180-100-100-p-142-iso8859-1
-b&h-lucidabright-medium-i-normal--26-190-100-100-p-148-iso8859-1
-b&h-lucidabright-medium-i-normal--34-240-100-100-p-194-iso8859-1
-b&h-lucidabright-medium-r-normal--11-80-100-100-p-63-iso8859-1
-b&h-lucidabright-medium-r-normal--14-100-100-100-p-80-iso8859-1
-b&h-lucidabright-medium-r-normal--17-120-100-100-p-96-iso8859-1
-b&h-lucidabright-medium-r-normal--20-140-100-100-p-114-iso8859-1
-b&h-lucidabright-medium-r-normal--25-180-100-100-p-142-iso8859-1
-b&h-lucidabright-medium-r-normal--26-190-100-100-p-149-iso8859-1
-b&h-lucidabright-medium-r-normal--34-240-100-100-p-193-iso8859-1
-b&h-lucidatypewriter-bold-r-normal-sans-11-80-100-100-m-70-iso8859-1
-b&h-lucidatypewriter-bold-r-normal-sans-14-100-100-100-m-80-iso8859-1
-b&h-lucidatypewriter-bold-r-normal-sans-17-120-100-100-m-100-iso8859-1
-b&h-lucidatypewriter-bold-r-normal-sans-20-140-100-100-m-120-iso8859-1
-b&h-lucidatypewriter-bold-r-normal-sans-25-180-100-100-m-150-iso8859-1
-b&h-lucidatypewriter-bold-r-normal-sans-26-190-100-100-m-159-iso8859-1
-b&h-lucidatypewriter-bold-r-normal-sans-34-240-100-100-m-200-iso8859-1
-b&h-lucidatypewriter-medium-r-normal-sans-11-80-100-100-m-70-iso8859-1
-b&h-lucidatypewriter-medium-r-normal-sans-14-100-100-100-m-80-iso8859-1
-b&h-lucidatypewriter-medium-r-normal-sans-17-120-100-100-m-100-iso8859-1
-b&h-lucidatypewriter-medium-r-normal-sans-20-140-100-100-m-120-iso8859-1
-b&h-lucidatypewriter-medium-r-normal-sans-25-180-100-100-m-150-iso8859-1
-b&h-lucidatypewriter-medium-r-normal-sans-26-190-100-100-m-159-iso8859-1
-b&h-lucidatypewriter-medium-r-normal-sans-34-240-100-100-m-200-iso8859-1
-bigelow & holmes-menu-medium-r-normal--16-120-100-100-p-92-iso8859-1
-bitstream-charter-bold-i-normal--11-80-100-100-p-68-iso8859-1
-bitstream-charter-bold-i-normal--14-100-100-100-p-86-iso8859-1
-bitstream-charter-bold-i-normal--17-120-100-100-p-105-iso8859-1
-bitstream-charter-bold-i-normal--19-140-100-100-p-117-iso8859-1
-bitstream-charter-bold-i-normal--25-180-100-100-p-154-iso8859-1
-bitstream-charter-bold-i-normal--33-240-100-100-p-203-iso8859-1
-bitstream-charter-bold-r-normal--11-80-100-100-p-69-iso8859-1
-bitstream-charter-bold-r-normal--14-100-100-100-p-88-iso8859-1
-bitstream-charter-bold-r-normal--17-120-100-100-p-107-iso8859-1
-bitstream-charter-bold-r-normal--19-140-100-100-p-119-iso8859-1
-bitstream-charter-bold-r-normal--25-180-100-100-p-157-iso8859-1
-bitstream-charter-bold-r-normal--33-240-100-100-p-206-iso8859-1
-bitstream-charter-medium-i-normal--11-80-100-100-p-60-iso8859-1
-bitstream-charter-medium-i-normal--14-100-100-100-p-76-iso8859-1
-bitstream-charter-medium-i-normal--17-120-100-100-p-92-iso8859-1
-bitstream-charter-medium-i-normal--19-140-100-100-p-103-iso8859-1
-bitstream-charter-medium-i-normal--25-180-100-100-p-136-iso8859-1
-bitstream-charter-medium-i-normal--33-240-100-100-p-179-iso8859-1
-bitstream-charter-medium-r-normal--11-80-100-100-p-61-iso8859-1
-bitstream-charter-medium-r-normal--14-100-100-100-p-78-iso8859-1
-bitstream-charter-medium-r-normal--17-120-100-100-p-95-iso8859-1
-bitstream-charter-medium-r-normal--19-140-100-100-p-106-iso8859-1
-bitstream-charter-medium-r-normal--25-180-100-100-p-139-iso8859-1
-bitstream-charter-medium-r-normal--33-240-100-100-p-183-iso8859-1
-bitstream-prestige-medium-r-normal--16-100-72-72-m-97-hp-roman8
-bitstream-prestige-medium-r-normal--16-120-72-72-m-97-hp-roman8
-bitstream-prestige-medium-r-normal--17-110-85-85-m-70-hp-roman8
-bitstream-prestige-medium-r-normal--17-128-72-72-m-103-hp-roman8
-bitstream-prestige-medium-r-normal--19-160-72-72-m-110-hp-roman8
-bitstream-prestige-medium-r-normal--19-160-85-85-m-80-hp-roman8
-bitstream-prestige-medium-r-normal--23-179-85-85-m-139-hp-roman8
-bitstream-prestige-medium-r-normal--26-171-110-110-m-140-hp-roman8
-bitstream-swiss 742-bold-r-normal--16-120-72-72-p-94-hp-roman8
-bitstream-swiss 742-bold-r-normal--17-140-85-85-p-100-hp-roman8
-bitstream-swiss 742-medium-r-normal--13-100-72-72-p-75-hp-roman8
-bitstream-swiss 742-medium-r-normal--14-90-85-85-p-81-hp-roman8
-bitstream-swiss 742-medium-r-normal--19-140-85-85-p-110-hp-roman8
-bitstream-swiss 742-medium-r-normal--23-196-85-85-p-133-hp-roman8
-bitstream-swiss 742-medium-r-normal--26-171-110-110-p-150-hp-roman8
-bitstream-terminal-bold-r-narrow--18-140-100-100-c-70-iso8859-1
-bitstream-terminal-bold-r-narrow--18-180-75-75-c-70-dec-dectech
-bitstream-terminal-bold-r-narrow--18-180-75-75-c-70-iso8859-1
-bitstream-terminal-bold-r-narrow--36-360-75-75-c-140-iso8859-1
-bitstream-terminal-bold-r-normal--18-140-100-100-c-110-dec-dectech
-bitstream-terminal-bold-r-normal--18-140-100-100-c-110-iso8859-1
-bitstream-terminal-bold-r-normal--18-180-75-75-c-110-dec-dectech
-bitstream-terminal-bold-r-normal--18-180-75-75-c-110-iso8859-1
-bitstream-terminal-bold-r-normal--36-360-75-75-c-220-iso8859-1
-bitstream-terminal-medium-r-narrow--18-140-100-100-c-70-dec-dectech
-bitstream-terminal-medium-r-narrow--18-180-75-75-c-70-iso8859-1
-bitstream-terminal-medium-r-normal--18-140-100-100-c-110-dec-dectech
-bitstream-terminal-medium-r-normal--18-140-100-100-c-110-iso8859-1
-bitstream-terminal-medium-r-normal--18-180-75-75-c-110-iso8859-1
-bitstream-terminal-medium-r-normal-gs-18-140-100-100-c-110-iso8859-1
-dec-terminal-bold-r-double wide--14-100-100-100-c-160-dec-dectech
-dec-terminal-bold-r-double wide--14-100-100-100-c-160-iso8859-1
-dec-terminal-bold-r-narrow--14-100-100-100-c-60-dec-dectech
-dec-terminal-bold-r-narrow--14-100-100-100-c-60-dec-dectech
-dec-terminal-bold-r-narrow--14-100-100-100-c-60-iso8859-1
-dec-terminal-bold-r-narrow--14-100-100-100-c-60-iso8859-1
-dec-terminal-bold-r-narrow--14-140-75-75-c-60-dec-dectech
-dec-terminal-bold-r-narrow--14-140-75-75-c-60-iso8859-1
-dec-terminal-bold-r-narrow--28-200-100-100-c-120-dec-dectech
-dec-terminal-bold-r-narrow--28-200-100-100-c-120-iso8859-1
-dec-terminal-bold-r-narrow--28-280-75-75-c-120-iso8859-1
-dec-terminal-bold-r-normal--14-100-100-100-c-80-dec-dectech
-dec-terminal-bold-r-normal--14-100-100-100-c-80-iso8859-1
-dec-terminal-bold-r-normal--14-100-100-100-c-80-iso8859-1
-dec-terminal-bold-r-normal--28-200-100-100-c-160-dec-dectech
-dec-terminal-bold-r-normal--28-200-100-100-c-160-iso8859-1
-dec-terminal-bold-r-normal--28-280-75-75-c-160-iso8859-1
-dec-terminal-bold-r-wide--14-100-100-100-c-120-dec-dectech
-dec-terminal-bold-r-wide--14-100-100-100-c-120-iso8859-1
-dec-terminal-medium-r-double wide--14-100-100-100-c-160-dec-dectech
-dec-terminal-medium-r-double wide--14-100-100-100-c-160-iso8859-1
-dec-terminal-medium-r-narrow--14-100-100-100-c-60-iso8859-1
-dec-terminal-medium-r-narrow--14-140-75-75-c-60-iso8859-1
-dec-terminal-medium-r-narrow--28-200-100-100-c-120-dec-dectech
-dec-terminal-medium-r-narrow--28-200-100-100-c-120-iso8859-1
-dec-terminal-medium-r-normal--14-100-100-100-c-80-dec-dectech
-dec-terminal-medium-r-normal--14-100-100-100-c-80-iso8859-1
-dec-terminal-medium-r-normal--14-140-75-75-c-80-iso8859-1
-dec-terminal-medium-r-normal--28-200-100-100-c-160-dec-dectech
-dec-terminal-medium-r-normal--28-200-100-100-c-160-iso8859-1
-dec-terminal-medium-r-wide--14-100-100-100-c-120-dec-dectech
-dec-terminal-medium-r-wide--14-100-100-100-c-120-iso8859-1
-ibm--medium-r-medium--15-10-100-100-c-70-iso8859-2
-ibm--medium-r-medium--22-150-100-100-c-70-iso8859-1
-ibm--medium-r-medium--23-160-100-100-c-110-iso8859-1
-ibm--medium-roman-medium--14-10-100-100-c-80-ibm-special
-ibm--medium-roman-medium--20-14-100-100-c-90-ibm-special
-ibm-roman-medium-roman-medium--14-10-100-100-c-80-ibm-850
-ibm-roman-medium-roman-medium--20-14-100-100-c-90-ibm-850
-misc-fixed-bold-r-normal--13-120-75-75-c-70-iso8859-1
-misc-fixed-bold-r-normal--13-120-75-75-c-80-iso8859-1
-misc-fixed-bold-r-normal--14-120-75-75-c-80--ibm pc
-misc-fixed-bold-r-normal--14-130-75-75-c-70-iso8859-1
-misc-fixed-bold-r-normal--15-140-75-75-c-90-iso8859-1
-misc-fixed-bold-r-normal--16-140-75-75-c-80--ibm pc
-misc-fixed-bold-r-normal--8-90-75-75-c-80--ibm pc
-misc-fixed-bold-r-semicondensed--13-120-75-75-c-60-iso8859-1
-misc-fixed-medium-r-normal--10-100-75-75-c-60-iso8859-1
-misc-fixed-medium-r-normal--13-120-75-75-c-60--ibm pc
-misc-fixed-medium-r-normal--13-120-75-75-c-70-iso8859-1
-misc-fixed-medium-r-normal--13-120-75-75-c-80-iso8859-1
-misc-fixed-medium-r-normal--13-120-75-75-c-80-iso8859-8
-misc-fixed-medium-r-normal--14-130-75-75-c-70-iso8859-1
-misc-fixed-medium-r-normal--15-140-75-75-c-90-iso8859-1
-misc-fixed-medium-r-normal--20-200-75-75-c-100-iso8859-1
-misc-fixed-medium-r-normal--7-70-75-75-c-50-iso8859-1
-misc-fixed-medium-r-normal--8-80-75-75-c-50-iso8859-1
-misc-fixed-medium-r-normal--9-90-75-75-c-60-iso8859-1
-misc-fixed-medium-r-semicondensed--12-110-75-75-c-60-iso8859-1
-misc-fixed-medium-r-semicondensed--13-120-75-75-c-60-iso8859-1
-misc-fixed-medium-r-semicondensed--13-120-75-75-c-60-iso8859-8
-misc-nil-medium-r-normal--2-20-75-75-c-10-misc-fontspecific
-monotype-times new roman-medium-i-normal--11-80-100-100-p-57-ibm-850
-monotype-times new roman-medium-i-normal--13-100-100-100-p-72-ibm-850
-monotype-times new roman-medium-i-normal--16-120-100-100-p-87-ibm-850
-monotype-times new roman-medium-i-normal--19-140-100-100-p-98-ibm-850
-monotype-times new roman-medium-i-normal--24-180-100-100-p-128-ibm-850
-monotype-times new roman-medium-r-normal--11-80-100-100-p-57-ibm-850
-monotype-times new roman-medium-r-normal--13-100-100-100-p-73-ibm-850
-monotype-times new roman-medium-r-normal--16-120-100-100-p-87-ibm-850
-monotype-times new roman-medium-r-normal--19-140-100-100-p-98-ibm-850
-monotype-times new roman-medium-r-normal--24-180-100-100-p-128-ibm-850
-schumacher-clean-bold-r-normal--10-100-75-75-c-60-iso8859-1
-schumacher-clean-bold-r-normal--10-100-75-75-c-80-iso8859-1
-schumacher-clean-bold-r-normal--12-120-75-75-c-60-iso8859-1
-schumacher-clean-bold-r-normal--12-120-75-75-c-80-iso8859-1
-schumacher-clean-bold-r-normal--13-130-75-75-c-80-iso8859-1
-schumacher-clean-bold-r-normal--14-140-75-75-c-80-iso8859-1
-schumacher-clean-bold-r-normal--15-150-75-75-c-90-iso8859-1
-schumacher-clean-bold-r-normal--16-160-75-75-c-80-iso8859-1
-schumacher-clean-bold-r-normal--8-80-75-75-c-80-iso8859-1
-schumacher-clean-medium-i-normal--12-120-75-75-c-60-iso8859-1
-schumacher-clean-medium-i-normal--8-80-75-75-c-80-iso8859-1
-schumacher-clean-medium-r-normal--10-100-75-75-c-50-iso8859-1
-schumacher-clean-medium-r-normal--10-100-75-75-c-60-iso8859-1
-schumacher-clean-medium-r-normal--10-100-75-75-c-70-iso8859-1
-schumacher-clean-medium-r-normal--10-100-75-75-c-80-iso8859-1
-schumacher-clean-medium-r-normal--12-120-75-75-c-60-iso8859-1
-schumacher-clean-medium-r-normal--12-120-75-75-c-70-iso8859-1
-schumacher-clean-medium-r-normal--12-120-75-75-c-80-iso8859-1
-schumacher-clean-medium-r-normal--13-130-75-75-c-60-iso8859-1
-schumacher-clean-medium-r-normal--13-130-75-75-c-80-iso8859-1
-schumacher-clean-medium-r-normal--14-140-75-75-c-70-iso8859-1
-schumacher-clean-medium-r-normal--14-140-75-75-c-80-iso8859-1
-schumacher-clean-medium-r-normal--15-150-75-75-c-90-iso8859-1
-schumacher-clean-medium-r-normal--16-160-75-75-c-80-iso8859-1
-schumacher-clean-medium-r-normal--6-60-75-75-c-40-iso8859-1
-schumacher-clean-medium-r-normal--6-60-75-75-c-50-iso8859-1
-schumacher-clean-medium-r-normal--6-60-75-75-c-60-iso8859-1
-schumacher-clean-medium-r-normal--8-80-75-75-c-50-iso8859-1
-schumacher-clean-medium-r-normal--8-80-75-75-c-60-iso8859-1
-schumacher-clean-medium-r-normal--8-80-75-75-c-70-iso8859-1
-schumacher-clean-medium-r-normal--8-80-75-75-c-80-iso8859-1
-sony-fixed-medium-r-normal--16-120-100-100-c-80-iso8859-1
-sony-fixed-medium-r-normal--24-170-100-100-c-120-iso8859-1
-sun-open look cursor-----12-120-75-75-p-160-sunolcursor-1
-sun-open look glyph-----10-100-75-75-p-101-sunolglyph-1
-sun-open look glyph-----12-120-75-75-p-113-sunolglyph-1
-sun-open look glyph-----14-140-75-75-p-128-sunolglyph-1
-sun-open look glyph-----19-190-75-75-p-154-sunolglyph-1
10x20
12x24
5x7
5x8
6x10
6x12
6x13
6x13b
6x13bold
6x9
7x13
7x13b
7x13bold
7x14
7x14b
8x13
8x13b
8x13bold
8x16
9x15
9x15b
9x15bold
andrew120
andy10
andy10b
andy10bi
andy10i
andy12
andy12b
andy12bi
andy12i
andy14
andy14b
andy14bi
andy14i
andy16
andy16b
andy16bi
andy16i
andy22
andy22b
andy22bi
andy22i
andy8
andy8b
andy8bi
andy8i
andysans10
andysans10b
andysans10bi
andysans10i
andysans12
andysans12b
andysans12bi
andysans12i
andysans14
andysans14b
andysans14bi
andysans14i
andysans16
andysans16b
andysans16bi
andysans16i
andysans22
andysans22b
andysans22bi
andysans22i
andysans8
andysans8b
andysans8bi
andysans8i
andysymbol10
andysymbol12
andysymbol16
andysymbol22
andysymbol8
andysymbola10
andysymbola12
andysymbola16
andysymbola22
andysymbola8
andytype10bf
andytype10bif
andytype10f
andytype10if
andytype12bf
andytype12bif
andytype12f
andytype12if
andytype14bf
andytype14bif
andytype14f
andytype14if
andytype8bf
andytype8bif
andytype8f
andytype8if
asyma10
asyma12
asyma16
asyma22
asyma8
asys10
asys12
asys16
asys22
asys8
charb08
charb10
charb12
charb14
charb18
charb24
charbi08
charbi10
charbi12
charbi14
charbi18
charbi24
chari08
chari10
chari12
chari14
chari18
chari24
charr08
charr10
charr12
charr14
charr18
charr24
clb6x10
clb6x12
clb8x10
clb8x12
clb8x13
clb8x14
clb8x16
clb8x8
clb9x15
cli6x12
cli8x8
clr4x6
clr5x10
clr5x6
clr5x8
clr6x10
clr6x12
clr6x13
clr6x6
clr6x8
clr7x10
clr7x12
clr7x14
clr7x8
clr8x10
clr8x12
clr8x13
clr8x14
clr8x16
clr8x8
clr9x15
con10
con12
console10
console12
courb08
courb08
courb10
courb10
courb12
courb12
courb14
courb14
courb18
courb18
courb24
courb24
courbo08
courbo08
courbo10
courbo10
courbo12
courbo12
courbo14
courbo14
courbo18
courbo18
courbo24
courbo24
couro08
couro08
couro10
couro10
couro12
couro12
couro14
couro14
couro18
couro18
couro24
couro24
courr08
courr08
courr10
courr10
courr12
courr12
courr14
courr14
courr18
courr18
courr24
courr24
cursor
deccurs
decsess
decw$cursor
decw$session
fixed
heb6x13
heb8x13
helvb08
helvb08
helvb10
helvb10
helvb12
helvb12
helvb14
helvb14
helvb18
helvb18
helvb24
helvb24
helvbo08
helvbo08
helvbo10
helvbo10
helvbo12
helvbo12
helvbo14
helvbo14
helvbo18
helvbo18
helvbo24
helvbo24
helvo08
helvo08
helvo10
helvo10
helvo12
helvo12
helvo14
helvo14
helvo18
helvo18
helvo24
helvo24
helvr08
helvr08
helvr10
helvr10
helvr12
helvr12
helvr14
helvr14
helvr18
helvr18
helvr24
helvr24
icon12
lookg10
lookg10b
lubb08
lubb10
lubb12
lubb14
lubb18
lubb19
lubb24
lubbi08
lubbi10
lubbi12
lubbi14
lubbi18
lubbi19
lubbi24
lubi08
lubi10
lubi12
lubi14
lubi18
lubi19
lubi24
lubis08
lubis10
lubis12
lubis14
lubis18
lubis19
lubis24
lubr08
lubr10
lubr12
lubr14
lubr18
lubr19
lubr24
lubs08
lubs10
lubs12
lubs14
lubs18
lubs19
lubs24
luis08
luis10
luis12
luis14
luis18
luis19
luis24
lurs08
lurs10
lurs12
lurs14
lurs18
lurs19
lurs24
lutbs08
lutbs10
lutbs12
lutbs14
lutbs18
lutbs19
lutbs24
lutrs08
lutrs10
lutrs12
lutrs14
lutrs18
lutrs19
lutrs24
menu12
messages10
messages14
msgs10
msgs14
ncenb08
ncenb08
ncenb10
ncenb10
ncenb12
ncenb12
ncenb14
ncenb14
ncenb18
ncenb18
ncenb24
ncenb24
ncenbi08
ncenbi08
ncenbi10
ncenbi10
ncenbi12
ncenbi12
ncenbi14
ncenbi14
ncenbi18
ncenbi18
ncenbi24
ncenbi24
nceni08
nceni08
nceni10
nceni10
nceni12
nceni12
nceni14
nceni14
nceni18
nceni18
nceni24
nceni24
ncenr08
ncenr08
ncenr10
ncenr10
ncenr12
ncenr12
ncenr14
ncenr14
ncenr18
ncenr18
ncenr24
ncenr24
nil2
olcursor
oldeng
olgl10
olgl12
olgl14
olgl19
olglyph-10
olglyph-12
olglyph-14
olglyph-19
pc6x13
pc8x14
pc8x16
pc8x8
rom10
rom11
rom14
rom16is
rom17is
spec14
special1
symb08
symb10
symb12
symb14
symb18
symb24
symbol10
symbol12
symbol16
symbol22
symbol8
symbola10
symbola12
symbola16
symbola22
symbola8
system13
system14
system16
system17
system19
system23
system26
t10
t2i
tb2i
tbd2d
tbdwdd
tbdwi
tbn14
tbn2i
tbnd2d
tbndd
tbni
tbonade
tbw1i
tbwd1d
td10
td2d
tdw1i
tdwd1d
tebode18
tebona14
tebona18
tebona28
tebonad4
tebonade
tebonar
tebono28
tech14
techb14
teno18
ter14
ter14nar
ter18
ter18nar
terbo
terbo36
terbodec
terbol
terbonar
term14
termb14
ternar
tgs14
tim120
timb08
timb08
timb10
timb10
timb12
timb12
timb14
timb14
timb18
timb18
timb24
timb24
timbi08
timbi08
timbi10
timbi10
timbi12
timbi12
timbi14
timbi14
timbi18
timbi18
timbi24
timbi24
times120
timi08
timi08
timi10
timi10
timi12
timi12
timi14
timi14
timi18
timi18
timi24
timi24
timr08
timr08
timr10
timr10
timr12
timr12
timr14
timr14
timr18
timr18
timr24
timr24
tn2i
tnd14
tnd2d
tnri08
tnri10
tnri12
tnri14
tnri18
tnrr08
tnrr10
tnrr12
tnrr14
tnrr18
toolkit10
toolkit10b
tw1i
twd1d
user6x16
user7x17
user8x16
user8x19
user9x17
usr11x19
usr12x23
usr14x26
variable
vt10x20
vt12x24
vt220r10x20
vt220r12x24
vt220r5x7
vt220r6x13
vt220r7x13
vt220r8x13
vt220r9x15
vt5x7
vt6x13
vt7x13
vt8x13
vt9x15
vtsingle
}

# please don't modify the following
# variables. They are needed by xf.
global {autoLoadList}
set {autoLoadList(main.tcl)} {0}
global {internalAliasList}
set {internalAliasList} {}
global {moduleList}
set {moduleList(main.tcl)} {}
global {preloadList}
set {preloadList(xfInternal)} {}
global {symbolicName}
set {symbolicName(root)} {.}
global {xfWmSetPosition}
set {xfWmSetPosition} {}
global {xfWmSetSize}
set {xfWmSetSize} {}
global {xfAppDefToplevels}
set {xfAppDefToplevels} {}

# display/remove toplevel windows.
ShowWindow.

# load default bindings.
if {[info exists env(XF_BIND_FILE)] &&
    "[info procs XFShowHelp]" == ""} {
  source $env(XF_BIND_FILE)
}

# parse and apply application defaults.
XFLocalLoadAppDefs Main
XFLocalSetAppDefs

# eof
#

