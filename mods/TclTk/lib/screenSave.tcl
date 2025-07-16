 # Handle command line options.
 switch -glob -- [lindex $argv 0] {
	"" { # Do nothing special. Start the screen saver }
	/s { # Same as above }
	/p {
		# No preview mode
		exit
	}
	/c* {
		# No configuration options. Show a message box.
		wm withdraw .
		tk_messageBox -title "TclScreenSaver Configuration" -message "There are no configuration options available for this screen saver."
		exit
	}
 }

 proc InitScreenSaver {} {
	global CursorStartX
	global CursorStartY

	# Make the main window solid black.
	. configure -background black

	# Make the main window fill the entire screen.
	wm geometry . [winfo screenwidth .]x[winfo screenheight .]+0+0

	# Take away the window decoration.
	wm overrideredirect . 1

	# Make the screensaver ontop of all other windows.  Catch any errors in case we´re not running under Windows.
	catch {wm attributes . -topmost 1}

	# Make sure the window has the current focus.
	raise .

	# Save the starting position of the mouse pointer.
	set CursorStartX [winfo pointerx .]
	set CursorStartY [winfo pointery .]

	# Hide the mouse pointer in the lower-right corner of the screen.
	event generate . <Motion> -warp true -x [winfo screenwidth .] -y [winfo screenheight .]
	update

	# Exit the application when mouse or keyboard activity is detected.
	bind . <KeyPress> 	ExitScreenSaver
	bind . <ButtonPress> 	ExitScreenSaver
	bind . <Motion> 		ExitScreenSaver

	return
 }

 proc ExitScreenSaver {} {
	global CursorStartX
	global CursorStartY

	# Erase previous binding on mouse motion so that we don´t get an infinite loop with the next line.
	bind . <Motion> {}

	# Move the mouse pointer back to the starting position.
	event generate . <Motion> -warp true -x $CursorStartX -y $CursorStartY

	# Close the application.  The 'after idle' is needed or the mouse pointer won't be re-positioned.
	after idle exit
 }

 proc CreateAnimation {widget} {
	global PositionX PositionY
	global IncrementX IncrementY
	global IncrementThreshold

	# This is the maximum number of pixels the widget will move in any direction at a time.
	set IncrementThreshold	4

	# Set the amount the widget will initially move in the X and Y directions. Make sure both are not zero or the widget will stand still..
	set IncrementX($widget) 0
	set IncrementY($widget) 0
	while { $IncrementX($widget) == 0  &&  $IncrementY($widget) == 0 } {
		set IncrementX($widget) [expr { int(rand()*($IncrementThreshold*2)-$IncrementThreshold) }]
		set IncrementY($widget) [expr { int(rand()*($IncrementThreshold*2)-$IncrementThreshold) }]
	}

	# The initital starting position coordinates of the widget, which will be a random spot on the screen.
	set PositionX($widget) [expr { int(rand()*[winfo screenwidth .]) }]
	set PositionY($widget) [expr { int(rand()*[winfo screenheight .]) }]

	RedisplayAnimation $widget

	return
 }

 proc RedisplayAnimation {widget} {
	global PositionX PositionY
	global IncrementX IncrementY
	global IncrementThreshold

	if { ![winfo exists $widget] } {
		return
	}

	# Increase/decrease the X and Y coordinates of the widget, according to the current increment values.
	incr PositionX($widget) $IncrementX($widget)
	incr PositionY($widget) $IncrementY($widget)

	if {$PositionX($widget) <= 0} {
		# The widget we are animating has hit the left edge of the screen.

		# Make sure it isn´t past the edge.
		set PositionX($widget) 0

		# The new X increment value must be positive, but the new Y increment value can be positive, negative, or zero.
		set IncrementX($widget) [expr { int(rand()*$IncrementThreshold)+1 }]
		set IncrementY($widget) [expr { int(rand()*($IncrementThreshold*2)-$IncrementThreshold) }]
	}

	if {$PositionX($widget) >= ([winfo screenwidth .] - [winfo width $widget]) } {
		# The widget has hit the right edge of the screen.

		set PositionX($widget) [expr { [winfo screenwidth .] - [winfo width $widget] }]

		# X must be negative, but Y can be any.
		set IncrementX($widget) [expr { int(rand()*-$IncrementThreshold)-1 }]
		set IncrementY($widget) [expr { int(rand()*($IncrementThreshold*2)-$IncrementThreshold) }]
	}

	if {$PositionY($widget) <= 0} {
		# The widget has hit the top edge of the screen.

		set PositionY($widget) 0

		# Y must be positive, but X can be any.
		set IncrementX($widget) [expr { int(rand()*($IncrementThreshold*2)-$IncrementThreshold) }]
		set IncrementY($widget) [expr { int(rand()*$IncrementThreshold)+1 }]
	}

	if {$PositionY($widget) >= ([winfo screenheight .] - [winfo height $widget]) } {
		# The widget has hit the bottom edge of the screen.

		set PositionY($widget) [expr { [winfo screenheight .] - [winfo height $widget] }]

		# Y must be negative, but X can be any.
		set IncrementX($widget) [expr { int(rand()*($IncrementThreshold*2)-$IncrementThreshold) }]
		set IncrementY($widget) [expr { int(rand()*-$IncrementThreshold)-1 }]
	}

	# Adjust the position of the widget.
	place $widget -x $PositionX($widget) -y $PositionY($widget)

	# Call this procedure again in the near future.
	after 10 [list RedisplayAnimation $widget]

	return
 }

 proc UpdateTime {label} {
	$label configure -text [clock format [clock seconds] -format %c]
	after 1000 [list UpdateTime $label]
	return
 }

 InitScreenSaver
 CreateAnimation [label .title -text "Tcl Screen Saver" -font {-weight bold} -bg black -fg white -padx 0 -pady 0]
 CreateAnimation [label .author -text "by 33Delta" -font {-weight bold} -bg black -fg white -padx 0 -pady 0]
 CreateAnimation [label .time -text "" -font {-weight bold} -bg black -fg green -padx 0 -pady 0]
 UpdateTime .time
