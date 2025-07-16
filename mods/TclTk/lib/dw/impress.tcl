#!/bin/sh
#
# Copyright © 1996,1997,1998 Christopher Jay Cox, All rights reserved.
#
# $Header: /home/ccox/impress/RCS/impress.tcl,v 1.9 1998/08/15 03:04:53 ccox Exp $
# restart trick \
exec wish "$0" "$@"
option add *Radiobutton*borderWidth 1
option add *Button*borderWidth 1

global Group GroupNo
global Global

set Global(PointsInch) [winfo fpixels . 1i]

set Global(FontList) {
	{-adobe-avantgarde-demi-i-normal--*-240-72-72-p-0-iso8859-1}
	{-adobe-avantgarde-demi-r-normal--*-240-72-72-p-0-iso8859-1}
	{-adobe-avantgarde-medium-i-normal--*-240-72-72-p-0-iso8859-1}
	{-adobe-avantgarde-medium-r-normal--*-240-72-72-p-0-iso8859-1}
	{-adobe-cheq-medium-r-normal--*-240-72-72-m-0-adobe-fontspecific}
	{-adobe-courier-bold-i-normal--*-240-72-72-m-0-iso8859-1}
	{-adobe-courier-bold-i-normal--*-240-72-72-p-0-iso8859-1}
	{-adobe-courier-bold-r-normal--*-240-72-72-m-0-iso8859-1}
	{-adobe-courier-bold-r-normal--*-240-72-72-p-0-iso8859-1}
	{-adobe-courier-medium-i-normal--*-240-72-72-m-0-iso8859-1}
	{-adobe-courier-medium-i-normal--*-240-72-72-p-0-iso8859-1}
	{-adobe-courier-medium-r-normal--*-240-72-72-m-0-iso8859-1}
	{-adobe-courier-medium-r-normal--*-240-72-72-p-0-iso8859-1}
	{-adobe-helvetica-bold-i-normal--*-240-72-72-p-0-iso8859-1}
	{-adobe-helvetica-bold-r-normal--*-240-72-72-p-0-iso8859-1}
	{-adobe-helvetica-medium-i-normal--*-240-72-72-p-0-iso8859-1}
	{-adobe-helvetica-medium-r-normal--*-240-72-72-p-0-iso8859-1}
	{-adobe-lubalingraph-demi-i-normal--*-240-72-72-p-0-iso8859-1}
	{-adobe-lubalingraph-demi-r-normal--*-240-72-72-p-0-iso8859-1}
	{-adobe-lubalingraph-medium-i-normal--*-240-72-72-p-0-iso8859-1}
	{-adobe-lubalingraph-medium-r-normal--*-240-72-72-p-0-iso8859-1}
	{-adobe-newcenturyschlbk-bold-i-normal--*-240-72-72-p-0-iso8859-1}
	{-adobe-newcenturyschlbk-bold-r-normal--*-240-72-72-p-0-iso8859-1}
	{-adobe-newcenturyschlbk-medium-i-normal--*-240-72-72-p-0-iso8859-1}
	{-adobe-newcenturyschlbk-medium-r-normal--*-240-72-72-p-0-iso8859-1}
	{-adobe-souvenir-demi-i-normal--*-240-72-72-p-0-iso8859-1}
	{-adobe-souvenir-demi-r-normal--*-240-72-72-p-0-iso8859-1}
	{-adobe-souvenir-light-i-normal--*-240-72-72-p-0-iso8859-1}
	{-adobe-souvenir-light-r-normal--*-240-72-72-p-0-iso8859-1}
	{-adobe-symbol-medium-r-normal--*-240-72-72-p-0-adobe-fontspecific}
	{-adobe-times-bold-i-normal--*-240-72-72-p-0-iso8859-1}
	{-adobe-times-bold-r-normal--*-240-72-72-p-0-iso8859-1}
	{-adobe-times-medium-i-normal--*-240-72-72-p-0-iso8859-1}
	{-adobe-times-medium-r-normal--*-240-72-72-p-0-iso8859-1}
	{-adobe-utopia-bold-i-normal--*-240-72-72-p-0-iso8859-1}
	{-adobe-utopia-bold-r-normal--*-240-72-72-p-0-iso8859-1}
	{-adobe-utopia-medium-i-normal--*-240-72-72-p-0-iso8859-1}
	{-adobe-utopia-medium-r-normal--*-240-72-72-p-0-iso8859-1}
	{-adobe-zapfdingbats-medium-r-normal--*-240-72-72-p-0-adobe-fontspecific}
	{-bitstream-charter-black-i-normal--*-240-72-72-p-0-iso8859-1}
	{-bitstream-charter-black-r-normal--*-240-72-72-p-0-iso8859-1}
	{-bitstream-charter-bold-i-normal--*-240-72-72-p-0-iso8859-1}
	{-bitstream-charter-bold-r-normal--*-240-72-72-p-0-iso8859-1}
	{-bitstream-charter-medium-i-normal--*-240-72-72-p-0-iso8859-1}
	{-bitstream-charter-medium-i-normal--*-240-72-72-p-0-iso8859-1}
	{-bitstream-charter-medium-r-normal--*-240-72-72-p-0-iso8859-1}
	{-bitstream-charter-medium-r-normal--*-240-72-72-p-0-iso8859-1}
	{-bitstream-courier-bold-i-normal--*-240-72-72-m-0-iso8859-1}
	{-bitstream-courier-bold-i-normal--*-240-72-72-m-0-iso8859-1}
	{-bitstream-courier-bold-r-normal--*-240-72-72-m-0-iso8859-1}
	{-bitstream-courier-bold-r-normal--*-240-72-72-m-0-iso8859-1}
	{-bitstream-courier-medium-i-normal--*-240-72-72-m-0-iso8859-1}
	{-bitstream-courier-medium-i-normal--*-240-72-72-m-0-iso8859-1}
	{-bitstream-courier-medium-r-normal--*-240-72-72-m-0-iso8859-1}
	{-bitstream-courier-medium-r-normal--*-240-72-72-m-0-iso8859-1}
	{-brecher-shalomoldstyle-medium-r-normal--*-240-72-72-p-0-iso8859-1}
	{-brecher-shalomscript-medium-r-normal--*-240-72-72-p-0-iso8859-1}
	{-brecher-shalomstick-medium-r-normal--*-240-72-72-p-0-iso8859-1}
	{-fontbank-muriel-medium-r-normal--*-240-72-72-p-0-adobe-fontspecific}
	{-fontbank-roostheavy-medium-r-normal--*-240-72-72-p-0-adobe-fontspecific}
	{-meit-goodcitymodern-medium-r-normal--*-240-72-72-p-0-adobe-fontspecific}
	{-meit-goodcitymodern-medium-r-normal--*-240-72-72-p-0-iso8859-1}
	{-paradise-agate-bold-r-normal--*-240-72-72-p-0-iso8859-1}
	{-paradise-agate-medium-i-normal--*-240-72-72-p-0-iso8859-1}
	{-paradise-agate-medium-r-normal--*-240-72-72-p-0-iso8859-1}
	{-paradise-arnoldboecklin-extrabold-r-normal--*-240-72-72-p-0-iso8859-1}
	{-paradise-baskerville-bold-r-normal--*-240-72-72-p-0-iso8859-1}
	{-paradise-baskerville-medium-i-normal--*-240-72-72-p-0-iso8859-1}
	{-paradise-baskerville-medium-r-normal--*-240-72-72-p-0-iso8859-1}
	{-paradise-becker-medium-r-normal-medium-*-240-72-72-p-0-iso8859-1}
	{-paradise-blippo-heavy-r-normal--*-240-72-72-p-0-iso8859-1}
	{-paradise-bodoni-bold-r-normal--*-240-72-72-p-0-iso8859-1}
	{-paradise-bodoni-medium-i-normal--*-240-72-72-p-0-iso8859-1}
	{-paradise-bodoni-medium-r-normal--*-240-72-72-p-0-iso8859-1}
	{-paradise-cooper-heavy-r-normal--*-240-72-72-p-0-iso8859-1}
	{-paradise-eras-bold-r-normal--*-240-72-72-p-0-iso8859-1}
	{-paradise-eras-medium-r-normal--*-240-72-72-p-0-iso8859-1}
	{-paradise-erasblack-demibold-r-normal--*-240-72-72-p-0-iso8859-1}
	{-paradise-eraslight-light-r-normal--*-240-72-72-p-0-iso8859-1}
	{-paradise-erasmedium-medium-r-normal-medium-*-240-72-72-p-0-iso8859-1}
	{-paradise-erasultrablk-heavy-r-normal--*-240-72-72-p-0-iso8859-1}
	{-paradise-garamond-medium-i-normal-medium-*-240-72-72-p-0-iso8859-1}
	{-paradise-goudyoldstyle-bold-r-normal--*-240-72-72-p-0-iso8859-1}
	{-paradise-goudyoldstyle-medium-i-normal--*-240-72-72-p-0-iso8859-1}
	{-paradise-goudyoldstyle-medium-r-normal--*-240-72-72-p-0-iso8859-1}
	{-paradise-parkavenue-medium-r-normal--*-240-72-72-p-0-iso8859-1}
	{-paradise-slogan-medium-r-normal--*-240-72-72-p-0-iso8859-1}
	{-rakowski-ann-medium-r-normal-stone-*-240-72-72-p-0-adobe-fontspecific}
	{-rakowski-crackling-medium-r-normal-fire-*-240-72-72-p-0-adobe-fontspecific}
	{-rakowski-davysbigkeycaps-medium-r-normal--*-240-72-72-p-0-adobe-fontspecific}
	{-rakowski-davysdingbats-medium-r-normal--*-240-72-72-p-0-adobe-fontspecific}
	{-rakowski-davysotherdingbats-medium-r-normal--*-240-72-72-p-0-adobe-fontspecific}
	{-rakowski-dobkin-medium-r-normal-script-*-240-72-72-p-0-iso8859-1}
	{-rakowski-dragonwick-medium-r-normal--*-240-72-72-p-0-adobe-fontspecific}
	{-rakowski-elizabeth-medium-r-normal-alternate-*-240-72-72-p-0-iso8859-1}
	{-rakowski-green-medium-r-normal-caps-*-240-72-72-p-0-adobe-fontspecific}
	{-rakowski-polosemiscript-medium-r-normal--*-240-72-72-p-0-adobe-fontspecific}
	{-rakowski-rechtman-medium-r-normal-script-*-240-72-72-p-0-adobe-fontspecific}
	{-rakowski-rudelsberg-medium-r-normal--*-240-72-72-p-0-adobe-fontspecific}
	{-rakowski-salter-medium-r-normal--*-240-72-72-p-0-adobe-fontspecific}
	{-rakowski-shohl-medium-r-normal-fold-*-240-72-72-p-0-iso8859-1}
	{-rakowski-tempofont-medium-r-normal--*-240-72-72-p-0-adobe-fontspecific}
	{-rakowski-tribeca-medium-r-normal--*-240-72-72-p-0-adobe-fontspecific}
	{-rakowski-upperwestside-medium-r-normal--*-240-72-72-p-0-adobe-fontspecific}
	{-rakowski-zaleskicaps-medium-r-normal--*-240-72-72-p-0-adobe-fontspecific}
	{-southern-tekton display ssi-medium-r-normal--*-240-72-72-p-0-iso8859-1}
	{-southern-victoriana display ssi-medium-r-normal--*-240-72-72-p-0-iso8859-1}
	{-unknown--medium-r-normal--*-240-72-72-p-0-adobe-fontspecific}
	{-unknown--medium-r-normal--*-240-72-72-p-0-adobe-fontspecific}
	{-unknown--medium-r-normal--*-240-72-72-p-0-iso8859-1}
	{-unknown-blackforest-medium-r-normal--*-240-72-72-p-0-adobe-fontspecific}
	{-unknown-bodidlybold-medium-r-normal--*-240-72-72-p-0-adobe-fontspecific}
	{-unknown-braillehc-medium-r-normal--*-240-72-72-m-0-iso8859-1}
	{-unknown-brushstroke-medium-r-normal--*-240-72-72-p-0-adobe-fontspecific}
	{-unknown-burgundian-medium-r-normal--*-240-72-72-p-0-adobe-fontspecific}
	{-unknown-caligula-medium-r-normal--*-240-72-72-p-0-iso8859-1}
	{-unknown-carrickcaps-medium-r-normal--*-240-72-72-p-0-adobe-fontspecific}
	{-unknown-cartwright-medium-r-normal--*-240-72-72-p-0-adobe-fontspecific}
	{-unknown-classicheavy-light-r-normal--*-240-72-72-p-0-iso8859-1}
	{-unknown-comicscartoon-medium-r-normal--*-240-72-72-p-0-adobe-fontspecific}
	{-unknown-coronet-demibold-i-normal--*-240-72-72-p-0-iso8859-1}
	{-unknown-crillee-medium-r-normal--*-240-72-72-p-0-adobe-fontspecific}
	{-unknown-cuneifont-light-r-normal--*-240-72-72-p-0-iso8859-1}
	{-unknown-duncan-medium-r-normal--*-240-72-72-p-0-adobe-fontspecific}
	{-unknown-engraver-light-r-normal--*-240-72-72-p-0-iso8859-1}
	{-unknown-florence-light-r-normal--*-240-72-72-p-0-iso8859-1}
	{-unknown-frizquadrata-bold-r-normal--*-240-72-72-p-0-iso8859-1}
	{-unknown-frizquadrata-thin-r-normal--*-240-72-72-p-0-iso8859-1}
	{-unknown-futura_poster-light-r-normal--*-240-72-72-p-0-iso8859-1}
	{-unknown-genoaitalic-medium-r-normal--*-240-72-72-p-0-iso8859-1}
	{-unknown-genoaroman-medium-r-normal--*-240-72-72-p-0-iso8859-1}
	{-unknown-igloolaser-medium-r-normal--*-240-72-72-p-0-iso8859-1}
	{-unknown-lauramccrary-medium-r-normal--*-240-72-72-p-0-adobe-fontspecific}
	{-unknown-linotext-medium-r-normal--*-240-72-72-p-0-iso8859-1}
	{-unknown-luxembourg-medium-r-normal--*-240-72-72-p-0-adobe-fontspecific}
	{-unknown-manzanita-medium-r-normal--*-240-72-72-p-0-adobe-fontspecific}
	{-unknown-mazama-medium-r-normal-plain-*-240-72-72-p-0-iso8859-1}
	{-unknown-mesozoicgothic-medium-r-normal--*-240-72-72-p-0-adobe-fontspecific}
	{-unknown-oldtown-medium-r-normal--*-240-72-72-p-0-iso8859-1}
	{-unknown-oswaldblack-medium-r-normal--*-240-72-72-p-0-adobe-fontspecific}
	{-unknown-perrygothic-medium-r-normal--*-240-72-72-p-0-iso8859-1}
	{-unknown-postantiqua-regular-r-normal--*-240-72-72-p-0-iso8859-1}
	{-unknown-romeodn-medium-r-normal--*-240-72-72-p-0-iso8859-1}
	{-unknown-sharktooth-medium-r-normal--*-240-72-72-p-0-adobe-fontspecific}
	{-unknown-sinaloa-medium-r-normal--*-240-72-72-p-0-iso8859-1}
	{-unknown-spooky-medium-r-normal-plain-*-240-72-72-p-0-adobe-fontspecific}
	{-unknown-sumdumgoi-medium-r-normal--*-240-72-72-p-0-iso8859-1}
	{-unknown-tempofontitalic-medium-r-normal--*-240-72-72-p-0-adobe-fontspecific}
	{-unknown-westernslant-medium-r-normal--*-240-72-72-p-0-iso8859-1}
	{-unknown-windsordemi-medium-r-normal--*-240-72-72-p-0-adobe-fontspecific}
	{-urw-oldtown-medium-r-normal--*-240-72-72-p-0-adobe-fontspecific}
	{-urw-oldtowncnd-medium-r-normal--*-240-72-72-p-0-adobe-fontspecific}
	{-urw-oldtownext-medium-r-normal--*-240-72-72-p-0-adobe-fontspecific}
}

image create bitmap stripe -data "
#define stripe4_width 4
#define stripe4_height 4
static char stripe4_bits[] = {
   0x09, 0x0c, 0x06, 0x03};
"

set Global(Timeout) 40000
set Global(sbg) ""
set Global(F_Outline) ""
set Global(F_Fill) ""
set Global(F_SlideBG) ""
set Global(colortypeSave) Outline
set Global(PSColor) color
set Global(Circulate) 1
set Global(EmbedPage) 1
set Global(CoordFactorX) 1
set Global(CoordFactorY) 1
set Global(Xpos) 0
set Global(Ypos) 0
set Global(CurrentPageId) 1
set Global(curline) ""
set Global(colortype) Outline
set Global(selectnum) 0
set Global(CurrentPage) ""
set Global(FixedAspect) True
set Global(Grid) 0
set Global(GridX) .25i
set Global(GridY) .25i
set Global(UnitX) i
set Global(UnitY) i
set Global(ScreenH) [winfo screenheight .]
set Global(ScreenW) [winfo screenwidth .]
set Global(Start) ""
set Global(top_left_corner) bottom_right_corner
set Global(top_side) bottom_side
set Global(top_right_corner) bottom_left_corner
set Global(right_side) left_side
set Global(bottom_right_corner) top_left_corner
set Global(bottom_side) top_side
set Global(bottom_left_corner) top_right_corner
set Global(left_side) right_side
set Global(Duplicate) ""
set Global(Cut) ""
set Global(Copy) ""
set Global(Message) "Select"
set Global(Smooth) 0
set Global(Msg_Smooth) "Switch to Smooth Lines"
set Global(Msg_miter) "Miter Line Joints"
set Global(Msg_round) "Round Line Joints"
set Global(Msg_bevel) "Bevel Line Joints"
set Global(Msg_first) "Arrow at First Point"
set Global(Msg_last) "Arrow at Last Point"
set Global(Msg_both) "Arrow at Both Ends"
set Global(Msg_none) "No Arrows"
set Global(Msg_Rounded_Rectangle) "Rounded Rectangle"
set Global(Msg_Flip_Vert) "Flip Vertical"
set Global(Msg_Flip_Horiz) "Flip Horizontal"
set Global(Msg_PageProp) "Page Properities"
set Global(Spline) 0
set Global(Sub-Cursor) ""
set Global(Fill) ""
set Global(Outline) black
set Global(Stipple) ""
set Global(OutStipple) ""
set Global(JoinStyle) miter
set Global(Arrow) none
set Global(Width) 1.0
set Global(PageWidth) 8.5i
set Global(PageHeight) 11i
set Global(Landscape) 0
set Global(PageVisX) 3i
set Global(PageVisY) 2i
set Global(PageBg) white
set Global(Units) i
set Global(ScrollDir) ""
set Global(OpenFile) ""
set Global(ParaStyle) Normal
set Global(AnchorChosen) 0
set Global(Gravity) 1
set Global(GravityVal) 30
set Global(MenuSelect) 0
set Global(Font) {-*-apollo-*-r-*-*-*-240-72-72-*-*-*-*}
set Global(PrintCmd) {lpr -Plp2}
set Global(FontScale) 1
set Global(PrivateCmap) ""
set GroupNo [clock clicks]
set Global(ButtonSize) .35i
set Global(ButtonRelief) flat
set Global(ButtonOrient) top
set Global(ButtonCount)  10
set Global(ColorWidth) 400
set Global(ColorHeight) 100
set Global(SlideBG) white
button .b
set Global(Background) [.b cget -bg]
set Global(ActiveBackground) [.b cget -activebackground]
destroy .b

set Global(DefButtonColor) [. cget -bg]
set Global(ButtonColor) DarkCyan
set Global(ButtonStipple1) gray50
set Global(ButtonStipple2) {}
set Global(Buttons) {
	{Button PageProp {
		set i [$Global(CurrentCanvas) create polygon 0.0 0.0 25.0 0.0 \
			25.0 0.0 175.073846154 0.0 175.073846154 0.0 200.073846154 0.0 \
			200.073846154 25.0 200.073846154 25.0 200.073846154 175.073846154 \
			200.073846154 175.073846154 200.073846154 200.073846154 \
			175.073846154 200.073846154 175.073846154 200.073846154 25.0 \
			200.073846154 25.0 200.073846154 0.0 200.073846154 0.0 \
			175.073846154 0.0 175.073846154 0.0 25.0 0.0 25.0 \
			-fill {} -outline {} -smooth 0 -splinesteps 12 -stipple {} \
			-tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon 36.0 172.0 36.0 47.0 \
			61.0 22.0 161.0 22.0 161.0 172.0 \
			-fill #FFFFFFFFFFFF -outline black -smooth 0 -splinesteps 12 \
			-stipple $Global(ButtonStipple1) -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 36.0 47.0 61.0184615385 \
			47.0184615385 61.0184615385 22.0092307692 -arrow none \
			-arrowshape {8 10 3} -capstyle butt -fill black -joinstyle miter \
			-smooth 0 -splinesteps 12 -stipple $Global(ButtonStipple1) \
			-tags {} -width 1]
	}}
	{Button Grid {
		set i [$Global(CurrentCanvas) create polygon 0.0 0.0 25.0 0.0 25.0 \
			0.0 175.073846154 0.0 175.073846154 0.0 200.073846154 0.0 \
			200.073846154 25.0 200.073846154 25.0 200.073846154 \
			175.073846154 200.073846154 175.073846154 200.073846154 \
			200.073846154 175.073846154 200.073846154 175.073846154 \
			200.073846154 25.0 200.073846154 25.0 200.073846154 0.0 \
			200.073846154 0.0 175.073846154 0.0 175.073846154 0.0 25.0 \
			0.0 25.0 \
			-fill {} -outline {} -smooth 0 -splinesteps 12 -stipple {} \
			-tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon 24.0092307692 \
			26.0184615385 49.0092307692 26.0184615385 49.0092307692 \
			26.0184615385 149.064615385 26.0184615385 149.064615385 \
			26.0184615385 174.064615385 26.0184615385 174.064615385 \
			51.0184615385 174.064615385 51.0184615385 174.064615385 \
			151.073846154 174.064615385 151.073846154 174.064615385 \
			176.073846154 149.064615385 176.073846154 149.064615385 \
			176.073846154 49.0092307692 176.073846154 49.0092307692 \
			176.073846154 24.0092307692 176.073846154 24.0092307692 \
			151.073846154 24.0092307692 151.073846154 24.0092307692 \
			51.0184615385 24.0092307692 51.0184615385 \
			-fill $Global(ButtonColor) -outline black -smooth 0 \
			-splinesteps 12 -stipple $Global(ButtonStipple1) \
			-tags Group-759492866 -width 2]
		set Group($Global(CurrentCanvas),$i) {Group-759492866}
		set i [$Global(CurrentCanvas) create line 24.0 101.0 174.064615385 \
			101.046153846 \
			-arrow none -arrowshape {8 10 3} -capstyle butt -fill black \
			-joinstyle miter -smooth 0 -splinesteps 12 -stipple {} \
			-tags Group-759492866 -width 2]
		set Group($Global(CurrentCanvas),$i) {Group-759492866}
		set i [$Global(CurrentCanvas) create line 99.0 176.0 99.036923077 \
			26.0184615385 \
			-arrow none -arrowshape {8 10 3} -capstyle butt -fill black \
			-joinstyle miter -smooth 0 -splinesteps 12 -stipple {} \
			-tags Group-759492866 -width 2]
		set Group($Global(CurrentCanvas),$i) {Group-759492866}
	}}
	{RadioButton Select {
		set i [$Global(CurrentCanvas) create polygon -1.0 -1.0 24.0 -1.0 \
			24.0 -1.0 146.0 -1.0 146.0 -1.0 171.0 -1.0 171.0 24.0 171.0 \
			24.0 171.0 136.0 171.0 136.0 171.0 161.0 146.0 161.0 146.0 \
			161.0 24.0 161.0 24.0 161.0 -1.0 161.0 -1.0 136.0 -1.0 136.0 \
			-1.0 24.0 -1.0 24.0 \
			-fill {} -outline {} -smooth 0 -splinesteps 12 -stipple {} \
			-tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon 37.9541566013 \
			22.1761496591 61.9830972933 102.92577573 73.664874718 \
			69.3468499177 109.153797554 67.2150576124 \
			-fill $Global(ButtonColor) -outline #000000000000 -smooth 0 \
			-splinesteps 12 -stipple $Global(ButtonStipple1) -tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon 73.664874718 \
			69.3468499177 113.417382164 138.192903283 129.140948917 \
			126.289330577 \
			-fill $Global(ButtonColor) -outline #000000000000 -smooth 0 \
			-splinesteps 12 -stipple $Global(ButtonStipple1) -tags {} -width 1]
	}}
	{RadioButton Oval {
		set i [$Global(CurrentCanvas) create polygon -1.0 -1.0 24.0 -1.0 \
			24.0 -1.0 146.0 -1.0 146.0 -1.0 171.0 -1.0 171.0 24.0 171.0 \
			24.0 171.0 136.0 171.0 136.0 171.0 161.0 146.0 161.0 146.0 \
			161.0 24.0 161.0 24.0 161.0 -1.0 161.0 -1.0 136.0 -1.0 136.0 \
			-1.0 24.0 -1.0 24.0 \
			-fill {} -outline {} -smooth 0 -splinesteps 12 -stipple {} \
			-tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon 83.0723583615 \
			21.1233836897 108.415159222 24.7089434403 130.701244279 \
			35.0331512701 147.242582766 50.850755345 156.044045139 \
			70.2539191706 156.044045139 90.9023348301 147.242582766 \
			110.305498656 130.701244279 126.123102731 108.415159222 \
			136.44731056 83.0723583615 140.032870311 57.7295575006 \
			136.44731056 35.4434724439 126.123102731 18.9021339567 \
			110.305498656 10.1006715836 90.9023348301 10.1006715836 \
			70.2539191706 18.9021339567 50.850755345 35.4434724439 \
			35.0331512701 57.7295575006 24.7089434403 \
			-fill $Global(ButtonColor) -outline black -smooth 1 \
			-splinesteps 12 -stipple $Global(ButtonStipple1) -tags {} -width 1]
	}}
	{RadioButton Rectangle {
		set i [$Global(CurrentCanvas) create polygon -1.0 -1.0 24.0 -1.0 24.0 \
			-1.0 146.0 -1.0 146.0 -1.0 171.0 -1.0 171.0 24.0 171.0 24.0 171.0 \
			136.0 171.0 136.0 171.0 161.0 146.0 161.0 146.0 161.0 24.0 161.0 \
			24.0 161.0 -1.0 161.0 -1.0 136.0 -1.0 136.0 -1.0 24.0 -1.0 24.0 \
			-fill {} -outline {} -smooth 0 -splinesteps 12 -stipple {} \
			-tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon 20.0 32.0 45.0 32.0 45.0 \
			32.0 126.0 32.0 126.0 32.0 151.0 32.0 151.0 57.0 151.0 57.0 151.0 \
			104.0 151.0 104.0 151.0 129.0 126.0 129.0 126.0 129.0 45.0 129.0 \
			45.0 129.0 20.0 129.0 20.0 104.0 20.0 104.0 20.0 57.0 20.0 57.0 \
			-fill $Global(ButtonColor) -outline black -smooth 0 \
			-splinesteps 12 -stipple $Global(ButtonStipple1) -tags {} -width 1]
	}}
	{RadioButton Rounded_Rectangle {
		set i [$Global(CurrentCanvas) create polygon 0.0 -1.0 25.0 -1.0 25.0 \
			-1.0 147.0 -1.0 147.0 -1.0 172.0 -1.0 172.0 24.0 172.0 24.0 172.0 \
			136.0 172.0 136.0 172.0 161.0 147.0 161.0 147.0 161.0 25.0 161.0 \
			25.0 161.0 0.0 161.0 0.0 136.0 0.0 136.0 0.0 24.0 0.0 24.0 \
			-fill {} -outline {} -smooth 0 -splinesteps 12 -stipple {} \
			-tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon 18.7164223548 \
			93.1459545703 18.7164223548 78.0842279201 18.7164223548 \
			55.4916379449 39.8880947215 32.8990479696 61.0597670883 \
			32.8990479696 110.460335944 32.8990479696 131.632008311 \
			32.8990479696 152.803680677 55.4916379449 152.803680677 \
			78.0842279201 152.803680677 93.1459545703 152.803680677 \
			115.738544546 131.632008311 138.331134521 110.460335944 \
			138.331134521 61.0597670883 138.331134521 39.8880947215 \
			138.331134521 18.7164223548 115.738544546 18.7164223548 \
			93.1459545703 \
			-fill $Global(ButtonColor) -outline black -smooth 1 \
			-splinesteps 12 -stipple $Global(ButtonStipple1) -tags {} -width 1]
	}}
	{RadioButton Polygon {
		set i [$Global(CurrentCanvas) create polygon -1.0 -1.0 24.0 -1.0 24.0 \
			-1.0 146.0 -1.0 146.0 -1.0 171.0 -1.0 171.0 24.0 171.0 24.0 171.0 \
			136.0 171.0 136.0 171.0 161.0 146.0 161.0 146.0 161.0 24.0 161.0 \
			24.0 161.0 -1.0 161.0 -1.0 136.0 -1.0 136.0 -1.0 24.0 -1.0 24.0 \
			-fill {} -outline {} -smooth 0 -splinesteps 12 -stipple {} \
			-tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon 15.0 130.0 53.0 15.0 \
			143.0 70.0 154.0 141.0 84.0 100.0 \
			-fill $Global(ButtonColor) -outline black -smooth 0 \
			-splinesteps 12 -stipple $Global(ButtonStipple1) -tags {} -width 1]
	}}
	{RadioButton Line {
		set i [$Global(CurrentCanvas) create polygon -1.0 -1.0 24.0 -1.0 24.0 \
			-1.0 146.0 -1.0 146.0 -1.0 171.0 -1.0 171.0 24.0 171.0 24.0 171.0 \
			136.0 171.0 136.0 171.0 161.0 146.0 161.0 146.0 161.0 24.0 161.0 \
			24.0 161.0 -1.0 161.0 -1.0 136.0 -1.0 136.0 -1.0 24.0 -1.0 24.0 \
			-fill {} -outline {} -smooth 0 -splinesteps 12 -stipple {} \
			-tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 49.0 135.0 116.0 23.0 \
			-arrow none -arrowshape {8 10 3} -capstyle butt \
			-fill $Global(ButtonColor) -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 2]
	}}
	{RadioButton Draw {
		set i [$Global(CurrentCanvas) create polygon 0.0 -1.0 25.0 -1.0 25.0 \
			-1.0 147.0 -1.0 147.0 -1.0 172.0 -1.0 172.0 24.0 172.0 24.0 172.0 \
			136.0 172.0 136.0 172.0 161.0 147.0 161.0 147.0 161.0 25.0 161.0 \
			25.0 161.0 0.0 161.0 0.0 136.0 0.0 136.0 0.0 24.0 0.0 24.0 \
			-fill {} -outline {} -smooth 0 -splinesteps 12 -stipple {} \
			-tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 33.365796242 145.814066335 \
			36.5148450736 144.251781322 42.8129427365 142.68949631 \
			50.1607233434 142.68949631 57.5085039502 142.68949631 \
			64.8562845571 142.68949631 70.1046992763 141.127211298 \
			74.3034310516 139.564926285 80.6015287147 138.002641273 \
			84.80026049 136.440356261 87.9493093215 134.878071249 \
			88.9989922654 134.096928743 92.1480410969 130.972358718 \
			92.6728825688 130.191216212 92.6728825688 129.410073706 \
			92.6728825688 127.847788693 92.1480410969 127.847788693 \
			91.0983581531 127.847788693 90.5735166811 127.066646187 \
			89.5238337373 127.066646187 88.4741507934 127.066646187 \
			87.4244678496 127.066646187 86.8996263777 127.847788693 \
			86.3747849058 127.847788693 85.8499434338 128.6289312 \
			85.8499434338 129.410073706 86.3747849058 129.410073706 \
			87.4244678496 130.191216212 91.623199625 131.753501224 \
			97.9212972879 131.753501224 104.219394951 131.753501224 \
			108.418126727 131.753501224 111.567175558 128.6289312 \
			113.666541446 125.504361175 114.71622439 120.817506139 \
			114.71622439 119.255221126 114.191382918 118.47407862 \
			111.042334086 115.349508596 107.893285255 113.787223583 \
			103.694553479 112.224938572 98.4461387599 112.224938572 \
			97.396455816 112.224938572 96.8716143441 112.224938572 \
			97.396455816 113.006081078 100.545504648 114.568366089 \
			103.694553479 117.692936114 107.893285255 119.255221126 \
			112.09201703 120.817506139 117.340431749 122.379791151 \
			122.588846468 122.379791151 127.837261188 120.817506139 \
			132.035992963 117.692936114 134.135358851 113.006081078 \
			134.660200323 111.443796066 134.135358851 109.881511053 \
			133.085675907 105.194656016 130.986310019 100.50780098 \
			127.837261188 98.9455159677 124.688212356 97.3832309552 \
			123.11368794 97.3832309552 119.964639109 98.9455159677 \
			119.439797637 99.7266584736 118.914956165 100.50780098 \
			119.439797637 101.288943486 122.588846468 105.975798522 \
			129.936627075 107.538083535 136.234724738 109.100368547 \
			140.433456514 110.662653559 148.830920064 112.224938572 \
			154.079334783 110.662653559 154.604176255 109.881511053 \
			-arrow none -arrowshape {8 10 3} -capstyle butt -fill black \
			-joinstyle miter -smooth 1 -splinesteps 12 -stipple {} \
			-tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon 78.7435236121 \
			33.9985878915 78.7435236121 33.9985878915 106.368114916 \
			50.9983363864 106.368114916 50.9983363864 80.7643270484 \
			106.560014371 80.4934977219 106.39335017 67.7228533257 \
			111.976600902 55.5771996827 120.184812798 43.6815423415 \
			130.41382813 31.9733822263 143.371969751 25.7859737669 \
			150.392699215 25.7859737669 150.392699215 32.8900353314 \
			134.601266178 39.4941042931 114.768226267 41.7649040308 \
			97.1226539937 41.2024123527 84.0811802709 40.2440931973 \
			81.9978777593 40.2440931973 81.9978777593 78.7435236121 \
			33.9985878915 78.7435236121 33.9985878915 \
			-fill $Global(ButtonColor) -outline black -smooth 1 \
			-splinesteps 12 -stipple $Global(ButtonStipple1) -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 26.0568030934 \
			150.559363416 61.5562778913 92.87271687 \
			-arrow none -arrowshape {8 10 3} -capstyle butt -fill black \
			-joinstyle miter -smooth 1 -splinesteps 12 -stipple {} \
			-tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon 67.2853597982 \
			82.9561969146 68.5915312389 84.1540647575 69.388371286 \
			85.7791073015 69.5797692697 87.6353204321 69.1426397684 \
			89.4988174513 68.1297070511 91.144833117 66.6631457534 \
			92.3748336461 64.9198448115 93.0404628225 63.1100720471 \
			93.0614359441 61.4521127656 92.4352233425 60.1459413251 \
			91.2373554997 59.3491012779 89.6123129557 59.1577032942 \
			87.756099825 59.5948327955 85.8926028059 60.6077655128 \
			84.2465871402 62.0743268104 83.016586611 63.8176277524 \
			82.3509574347 65.6274005169 82.3299843131 \
			-fill {} -outline black -smooth 1 -splinesteps 12 -stipple {} \
			-tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon 79.8476739433 \
			13.3947260516 86.618407106 17.5613310748 86.618407106 \
			17.5613310748 117.222121002 36.3943857799 117.222121002 \
			36.3943857799 123.992854165 40.5609908033 119.826249142 \
			47.3317239661 119.826249142 47.3317239661 118.659599735 \
			49.2275292516 118.659599735 49.2275292516 114.492994711 \
			55.9982624145 107.722261548 51.831657391 107.722261548 \
			51.831657391 77.118547653 32.9986026859 77.118547653 \
			32.9986026859 70.3478144903 28.8319976627 74.5144195135 \
			22.0612644999 74.5144195135 22.0612644999 75.6810689201 \
			20.1654592143 75.6810689201 20.1654592143 \
			-fill $Global(ButtonColor) -outline black -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon 115.0 34.0 115.0 34.0 \
			135.0 6.0 135.0 6.0 92.0 6.0 92.0 6.0 87.0 16.0 87.0 16.0 \
			114.0 34.0 114.0 34.0 \
			-fill white -outline black -smooth 1 -splinesteps 12 \
			-stipple $Global(ButtonStipple2) -tags {} -width 1]
	}}
	{RadioButton Text {
		set i [$Global(CurrentCanvas) create polygon 117.745882712 \
			20.1519264665 120.580883712 20.1519264665 112.23091983 \
			111.647154041 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {} -width 1]
		set i [$Global(CurrentCanvas) create polygon 117.745882712 \
			20.1519264665 112.23091983 111.647154041 111.992824043 \
			114.033649023 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {} -width 1]
		set i [$Global(CurrentCanvas) create polygon 117.745882712 \
			20.1519264665 111.992824043 114.033649023 111.815636481 \
			116.071305992 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {} -width 1]
		set i [$Global(CurrentCanvas) create polygon 117.745882712 \
			20.1519264665 111.815636481 116.071305992 111.682745809 \
			117.760124947 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {} -width 1]
		set i [$Global(CurrentCanvas) create polygon 123.498941382 \
			129.604006076 125.857750807 129.653840078 124.960738772 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {} -width 1]
		set i [$Global(CurrentCanvas) create polygon 121.433598856 \
			129.471115404 123.498941382 129.604006076 124.960738772 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {} -width 1]
		set i [$Global(CurrentCanvas) create polygon 119.661723232 \
			129.24409384 121.433598856 129.471115404 124.960738772 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {} -width 1]
		set i [$Global(CurrentCanvas) create polygon 118.172240285 \
			128.922941383 119.661723232 129.24409384 124.960738772 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {} -width 1]
		set i [$Global(CurrentCanvas) create polygon 116.976224238 \
			128.518732256 118.172240285 128.922941383 124.960738772 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {} -width 1]
		set i [$Global(CurrentCanvas) create polygon 115.962932865 \
			128.014855125 116.976224238 128.518732256 124.960738772 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {} -width 1]
		set i [$Global(CurrentCanvas) create polygon 115.962932865 \
			128.014855125 124.960738772 132.654954418 81.6494539688 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 115.038235273 \
			127.411309991 115.962932865 128.014855125 81.6494539688 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 114.202131463 \
			126.713633963 115.038235273 127.411309991 81.6494539688 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 113.454621434 \
			125.921827043 114.202131463 126.713633963 81.6494539688 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 112.795705186 \
			125.03035212 113.454621434 125.921827043 81.6494539688 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 112.358273391 \
			124.288379202 112.795705186 125.03035212 81.6494539688 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 112.0205096 \
			123.419052723 112.358273391 124.288379202 81.6494539688 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 111.776876701 \
			122.427909796 112.0205096 123.419052723 81.6494539688 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 111.627374696 \
			121.314950419 111.776876701 122.427909796 81.6494539688 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 111.577540694 \
			120.080174593 111.627374696 121.314950419 81.6494539688 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 81.6494539688 \
			132.654954418 82.5409288925 129.653840078 84.4069354098 \
			129.653840078 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 81.6494539688 \
			132.654954418 84.4069354098 129.653840078 86.439055267 \
			129.559709186 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 81.6494539688 \
			132.654954418 86.439055267 129.559709186 88.316136007 \
			129.288390731 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 81.6494539688 \
			132.654954418 88.316136007 129.288390731 90.0326405185 \
			128.834347602 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 111.577540694 \
			120.080174593 81.6494539688 132.654954418 90.0326405185 \
			128.834347602 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 111.577540694 \
			120.080174593 90.0326405185 128.834347602 91.5996430242 \
			128.197579799 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 111.60522625 \
			119.094568777 111.577540694 120.080174593 91.5996430242 \
			128.197579799 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 111.682745809 \
			117.760124947 111.60522625 119.094568777 91.5996430242 \
			128.197579799 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 111.682745809 \
			117.760124947 91.5996430242 128.197579799 93.0060693014 \
			127.383624434 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 111.682745809 \
			117.760124947 93.0060693014 127.383624434 93.9030813364 \
			126.685948407 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 111.682745809 \
			117.760124947 93.9030813364 126.685948407 94.7170367015 \
			125.82215904 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 111.682745809 \
			117.760124947 94.7170367015 125.82215904 95.4479353968 \
			124.803330555 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 111.682745809 \
			117.760124947 95.4479353968 124.803330555 96.0957774221 \
			123.618388731 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 111.682745809 \
			117.760124947 96.0957774221 123.618388731 96.6550256661 \
			122.272870679 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 111.682745809 \
			117.760124947 96.6550256661 122.272870679 97.0094007911 \
			120.993797962 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 117.745882712 \
			20.1519264665 111.682745809 117.760124947 97.0094007911 \
			120.993797962 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 117.745882712 \
			20.1519264665 97.0094007911 120.993797962 97.3582388047 \
			119.172088335 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 117.745882712 \
			20.1519264665 97.3582388047 119.172088335 97.6960025957 \
			116.802204687 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 117.745882712 \
			20.1519264665 97.6960025957 116.802204687 98.0337663867 \
			113.889684129 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 117.745882712 \
			20.1519264665 98.0337663867 113.889684129 98.360455955 \
			110.428989549 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 117.745882712 \
			20.1519264665 98.360455955 110.428989549 99.6561400056 \
			96.4754690041 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 117.745882712 \
			20.1519264665 99.6561400056 96.4754690041 100.303982031 \
			90.556296995 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 117.745882712 \
			20.1519264665 100.303982031 90.556296995 104.523260862 \
			47.1619555218 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 117.745882712 \
			20.1519264665 104.523260862 47.1619555218 72.6461109503 \
			90.556296995 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 72.6461109503 \
			90.556296995 100.303982031 90.556296995 99.6561400056 \
			96.4754690041 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 72.6461109503 \
			90.556296995 99.6561400056 96.4754690041 68.1887363317 \
			96.4754690041 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 117.745882712 \
			20.1519264665 72.6461109503 90.556296995 68.1887363317 \
			96.4754690041 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 117.745882712 \
			20.1519264665 68.1887363317 96.4754690041 56.9926972276 \
			111.647154041 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 117.745882712 \
			20.1519264665 56.9926972276 111.647154041 55.5807338391 \
			113.574068783 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 56.8653436671 \
			129.249630951 58.4821801747 129.520949406 59.4234891004 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 55.4699916126 \
			128.845421824 56.8653436671 129.249630951 59.4234891004 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 54.2961240111 \
			128.302784914 55.4699916126 128.845421824 59.4234891004 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 53.3437408628 \
			127.621720221 54.2961240111 128.302784914 59.4234891004 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 53.3437408628 \
			127.621720221 59.4234891004 132.654954418 26.8984971629 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 52.585156611 \
			126.846524635 53.3437408628 127.621720221 26.8984971629 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 51.9926856989 \
			126.015957936 52.585156611 126.846524635 26.8984971629 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 51.571865238 \
			125.124483012 51.9926856989 126.015957936 26.8984971629 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 51.317158117 \
			124.172099864 51.571865238 125.124483012 26.8984971629 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 51.2341014471 \
			123.164345602 51.317158117 124.172099864 26.8984971629 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 51.2728612264 \
			122.372538683 51.2341014471 123.164345602 26.8984971629 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 51.3891405642 \
			121.558583317 51.2728612264 122.372538683 26.8984971629 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 51.5829394607 \
			120.722479507 51.3891405642 121.558583317 26.8984971629 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 51.8542579157 \
			119.869764363 51.5829394607 120.722479507 26.8984971629 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 52.2086330407 \
			118.98382655 51.8542579157 119.869764363 26.8984971629 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 52.7125101715 \
			117.976072289 52.2086330407 118.98382655 26.8984971629 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 26.8984971629 \
			132.654954418 27.795509198 129.653840078 30.1377072895 \
			129.415744291 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 26.8984971629 \
			132.654954418 30.1377072895 129.415744291 32.3691631544 \
			128.895255826 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 26.8984971629 \
			132.654954418 32.3691631544 128.895255826 34.4843396815 \
			128.092374684 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 26.8984971629 \
			132.654954418 34.4843396815 128.092374684 36.4887739821 \
			127.007100864 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 52.7125101715 \
			117.976072289 26.8984971629 132.654954418 36.4887739821 \
			127.007100864 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 53.4378717554 \
			116.741296463 52.7125101715 117.976072289 36.4887739821 \
			127.007100864 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 54.395792015 \
			115.273961961 53.4378717554 116.741296463 36.4887739821 \
			127.007100864 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 54.395792015 \
			115.273961961 36.4887739821 127.007100864 38.3769289447 \
			125.639434366 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 54.395792015 \
			115.273961961 38.3769289447 125.639434366 40.3924374679 \
			123.784502071 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 54.395792015 \
			115.273961961 40.3924374679 123.784502071 42.7623211161 \
			121.248505083 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 54.395792015 \
			115.273961961 42.7623211161 121.248505083 45.4976541118 \
			118.031443402 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 54.395792015 \
			115.273961961 45.4976541118 118.031443402 48.5928993439 \
			114.127779916 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 54.395792015 \
			115.273961961 48.5928993439 114.127779916 52.0425197009 \
			109.537514626 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 54.395792015 \
			115.273961961 52.0425197009 109.537514626 117.745882712 \
			20.1519264665 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 117.745882712 \
			20.1519264665 120.580883712 20.1519264665 112.23091983 \
			111.647154041 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 117.745882712 \
			20.1519264665 112.23091983 111.647154041 111.992824043 \
			114.033649023 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 117.745882712 \
			20.1519264665 111.992824043 114.033649023 111.815636481 \
			116.071305992 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 117.745882712 \
			20.1519264665 111.815636481 116.071305992 111.682745809 \
			117.760124947 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 123.498941382 \
			129.604006076 125.857750807 129.653840078 124.960738772 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 121.433598856 \
			129.471115404 123.498941382 129.604006076 124.960738772 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 119.661723232 \
			129.24409384 121.433598856 129.471115404 124.960738772 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 118.172240285 \
			128.922941383 119.661723232 129.24409384 124.960738772 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 116.976224238 \
			128.518732256 118.172240285 128.922941383 124.960738772 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 115.962932865 \
			128.014855125 116.976224238 128.518732256 124.960738772 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 115.962932865 \
			128.014855125 124.960738772 132.654954418 81.6494539688 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 115.038235273 \
			127.411309991 115.962932865 128.014855125 81.6494539688 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 114.202131463 \
			126.713633963 115.038235273 127.411309991 81.6494539688 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 113.454621434 \
			125.921827043 114.202131463 126.713633963 81.6494539688 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 112.795705186 \
			125.03035212 113.454621434 125.921827043 81.6494539688 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 112.358273391 \
			124.288379202 112.795705186 125.03035212 81.6494539688 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 112.0205096 \
			123.419052723 112.358273391 124.288379202 81.6494539688 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 111.776876701 \
			122.427909796 112.0205096 123.419052723 81.6494539688 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 111.627374696 \
			121.314950419 111.776876701 122.427909796 81.6494539688 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 111.577540694 \
			120.080174593 111.627374696 121.314950419 81.6494539688 \
			132.654954418 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 81.6494539688 \
			132.654954418 82.5409288925 129.653840078 84.4069354098 \
			129.653840078 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 81.6494539688 \
			132.654954418 84.4069354098 129.653840078 86.439055267 \
			129.559709186 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 81.6494539688 \
			132.654954418 86.439055267 129.559709186 88.316136007 \
			129.288390731 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 81.6494539688 \
			132.654954418 88.316136007 129.288390731 90.0326405185 \
			128.834347602 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 111.577540694 \
			120.080174593 81.6494539688 132.654954418 90.0326405185 \
			128.834347602 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 111.577540694 \
			120.080174593 90.0326405185 128.834347602 91.5996430242 \
			128.197579799 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 111.60522625 \
			119.094568777 111.577540694 120.080174593 91.5996430242 \
			128.197579799 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 111.682745809 \
			117.760124947 111.60522625 119.094568777 91.5996430242 \
			128.197579799 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 111.682745809 \
			117.760124947 91.5996430242 128.197579799 93.0060693014 \
			127.383624434 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 111.682745809 \
			117.760124947 93.0060693014 127.383624434 93.9030813364 \
			126.685948407 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 45.4976541118 \
			118.031443402 48.5928993439 114.127779916 48.5928993439 \
			114.127779916 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 48.5928993439 \
			114.127779916 48.5928993439 114.127779916 52.0425197009 \
			109.537514626 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 48.5928993439 \
			114.127779916 52.0425197009 109.537514626 52.0425197009 \
			109.537514626 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 52.0425197009 \
			109.537514626 52.0425197009 109.537514626 117.745882712 \
			20.1519264665 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 52.0425197009 \
			109.537514626 117.745882712 20.1519264665 117.745882712 \
			20.1519264665 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 117.745882712 \
			20.1519264665 117.745882712 20.1519264665 120.580883712 \
			20.1519264665 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 117.745882712 \
			20.1519264665 120.580883712 20.1519264665 120.580883712 \
			20.1519264665 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 104.523260862 \
			47.1619555218 104.523260862 47.1619555218 72.6461109503 \
			90.556296995 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 104.523260862 \
			47.1619555218 72.6461109503 90.556296995 72.6461109503 \
			90.556296995 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 72.6461109503 \
			90.556296995 72.6461109503 90.556296995 100.303982031 \
			90.556296995 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 72.6461109503 \
			90.556296995 100.303982031 90.556296995 100.303982031 \
			90.556296995 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 100.303982031 \
			90.556296995 100.303982031 90.556296995 104.523260862 \
			47.1619555218 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon 100.303982031 \
			90.556296995 104.523260862 47.1619555218 104.523260862 \
			47.1619555218 \
			-fill $Global(ButtonColor) -outline {} -smooth 0 \
			-splinesteps 12 -stipple {}  -width 1]
		set i [$Global(CurrentCanvas) create polygon -1.0 -1.0 24.0 -1.0 \
			24.0 -1.0 146.0 -1.0 146.0 -1.0 171.0 -1.0 171.0 24.0 171.0 24.0 \
			171.0 136.0 171.0 136.0 171.0 161.0 146.0 161.0 146.0 161.0 24.0 \
			161.0 24.0 161.0 -1.0 161.0 -1.0 136.0 -1.0 136.0 -1.0 24.0 -1.0 \
			24.0 \
			-fill {} -outline {} -smooth 0 -splinesteps 12 -stipple {} \
			-tags {} -width 1]
	}}
	{RadioButton Rotate {
		set i [$Global(CurrentCanvas) create polygon -1.0 -1.0 \
			24.0 -1.0 24.0 -1.0 146.0 -1.0 146.0 -1.0 171.0 -1.0 171.0 24.0 \
			171.0 24.0 171.0 136.0 171.0 136.0 171.0 161.0 146.0 161.0 146.0 \
			161.0 24.0 161.0 24.0 161.0 -1.0 161.0 -1.0 136.0 -1.0 136.0 -1.0 \
			24.0 -1.0 24.0 -fill {} -outline {} -smooth 0 -splinesteps 12 \
			-stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon \
			35.9696969695 95.484848484 35.9696969695 95.484848484 \
			19.4848484848 76.0909090903 19.4848484848 76.0909090903 \
			27.2424242423 76.0909090903 27.2424242423 76.0909090903 \
			30.3939393938 63.2424242419 38.8787878785 47.4848484846 \
			50.2727272723 35.8484848483 63.8484848479 27.8484848484 \
			79.8484848477 23.4848484848 96.8181818172 24.2121212121 \
			109.909090908 28.3333333333 122.272727271 35.3636363635 \
			133.18181818 46.2727272724 138.030303029 52.8181818178 \
			138.030303029 52.8181818178 119.363636362 59.848484848 \
			119.363636362 59.848484848 109.666666666 49.6666666663 \
			98.2727272718 43.8484848482 83.9696969689 41.424242424 \
			69.1818181812 44.8181818179 57.545454545 52.333333333 \
			50.0303030299 62.0303030298 46.1515151512 70.757575757 \
			44.9393939391 76.5757575751 44.9393939391 76.5757575751 \
			53.4242424238 76.5757575751 53.4242424238 76.5757575751 \
			35.9696969695 95.484848484 35.9696969695 95.484848484 -fill \
			$Global(ButtonColor) -outline black -smooth 1 -splinesteps 12 \
			-stipple $Global(ButtonStipple1) -width 1]
		set i [$Global(CurrentCanvas) create polygon \
			32.3333333332 107.363636363 32.3333333332 107.363636363 \
			49.0606060602 101.303030302 48.8181818178 101.303030302 \
			58.2727272722 113.909090908 73.0606060599 122.393939393 \
			85.6666666659 124.333333332 100.939393938 121.181818181 \
			112.818181817 113.666666666 120.818181817 103.484848484 \
			125.181818181 93.3030303022 126.15151515 87.2424242416 \
			126.15151515 87.2424242416 118.15151515 87.2424242416 \
			118.15151515 87.2424242416 135.848484847 69.06060606 \
			135.848484847 69.06060606 152.575757574 88.2121212113 \
			152.575757574 88.2121212113 145.303030301 88.2121212113 \
			145.303030301 88.2121212113 142.15151515 103.969696969 \
			132.696969696 120.454545453 118.878787878 132.575757574 \
			104.818181817 139.606060605 87.1212121204 142.515151514 \
			69.6666666661 140.333333332 53.4242424238 132.575757574 \
			41.7878787876 122.15151515 34.9999999998 112.696969696 \
			32.3333333332 107.363636363 -fill $Global(ButtonColor) -outline \
			black -smooth 1 -splinesteps 12 -stipple $Global(ButtonStipple1) \
			-width 1]
	}}
	{Button Duplicate {
		set i [$Global(CurrentCanvas) create polygon \
			47.4759175308 12.4301808516 55.8584904465 13.4585960037 \
			63.2300013546 16.4197994153 68.7013372479 20.956626252 \
			71.61257427 26.5218682407 71.61257427 32.4442750639 68.7013372479 \
			38.0095170526 63.2300013546 42.5463438893 55.8584904465 \
			45.5075473009 47.4759175308 46.535962453 39.0933446153 \
			45.5075473009 31.7218337072 42.5463438893 26.2504978137 \
			38.0095170526 23.3392607919 32.4442750639 23.3392607919 \
			26.5218682407 26.2504978137 20.956626252 31.7218337072 \
			16.4197994153 39.0933446153 13.4585960037 -fill \
			$Global(ButtonColor) -outline black -smooth 1 -splinesteps 12 \
			-stipple $Global(ButtonStipple1) -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 47.7512995935 \
			46.535962453 47.7512995935 101.263844557 19.1115650792 \
			128.231206754 -arrow none -arrowshape {8 10 3} -capstyle butt \
			-fill black -joinstyle miter -smooth 0 -splinesteps 12 -stipple \
			{} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 48.3020637188 \
			73.8999035053 87.9570807386 51.6914875788 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 47.7512995935 \
			73.5033246495 12.5023955758 59.2264858396 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 116.046051128 \
			73.8999035053 116.046051128 128.62778561 87.4063166134 \
			155.595147806 -arrow none -arrowshape {8 10 3} -capstyle butt \
			-fill black -joinstyle miter -smooth 0 -splinesteps 12 -stipple \
			{} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 116.596815253 \
			129.024364466 144.685785641 153.612253528 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 116.596815253 \
			101.263844557 156.251832272 79.0554286311 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 116.046051128 \
			100.867265702 80.7971471101 86.5904268918 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon -1.0 -1.0 \
			24.0 -1.0 24.0 -1.0 146.0 -1.0 146.0 -1.0 171.0 -1.0 171.0 24.0 \
			171.0 24.0 171.0 136.0 171.0 136.0 171.0 161.0 146.0 161.0 146.0 \
			161.0 24.0 161.0 24.0 161.0 -1.0 161.0 -1.0 136.0 -1.0 136.0 -1.0 \
			24.0 -1.0 24.0 -fill {} -outline {} -smooth 0 -splinesteps 12 \
			-stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 49.6668789479 \
			102.974072534 77.7558493372 127.561961596 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 49.6001644466 \
			30.9036809488 58.138044364 33.983196081 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 66.6193681583 \
			37.0310374934 75.1635320896 40.1128150343 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 83.6385718701 \
			43.1583940379 92.189019815 46.2424339874 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 100.657775582 \
			49.2857505826 109.21450754 52.3720529407 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 48.6957058477 \
			101.890726463 57.2335857658 104.970241595 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 65.7149095594 \
			108.018083008 74.2590734905 111.099860548 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 82.7341132712 \
			114.145439552 91.284561216 117.229479501 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 99.7533169831 \
			120.272796096 108.310048942 123.359098455 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon \
			115.770669065 39.7941219039 124.153241981 40.822537056 \
			131.524752889 43.7837404676 136.996088783 48.3205673043 \
			139.907325804 53.885809293 139.907325804 59.8082161163 \
			136.996088783 65.3734581049 131.524752889 69.9102849416 \
			124.153241981 72.8714883532 115.770669065 73.8999035053 \
			107.38809615 72.8714883532 100.016585241 69.9102849416 \
			94.545249348 65.3734581049 91.6340123261 59.8082161163 \
			91.6340123261 53.885809293 94.545249348 48.3205673043 \
			100.016585241 43.7837404676 107.38809615 40.822537056 -fill \
			$Global(ButtonColor) -outline black -smooth 1 -splinesteps 12 \
			-stipple $Global(ButtonStipple1) -tags {} -width 1]
	}}
	{Button Flip_Horiz {
		set i [$Global(CurrentCanvas) create polygon -1.0 -1.0 24.0 -1.0 \
			24.0 -1.0 146.0 -1.0 146.0 -1.0 171.0 -1.0 171.0 24.0 171.0 24.0 \
			171.0 136.0 171.0 136.0 171.0 161.0 146.0 161.0 146.0 161.0 24.0 \
			161.0 24.0 161.0 -1.0 161.0 -1.0 136.0 -1.0 136.0 -1.0 24.0 -1.0 \
			24.0 -fill {} -outline {} -smooth 0 -splinesteps 12 -stipple {} \
			-tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon 128.12386206 \
			92.3378232918 47.9129114389 92.3378232922 47.9129114391 \
			132.443298603 -fill $Global(ButtonColor) -outline black -smooth 0 \
			-splinesteps 12 -stipple $Global(ButtonStipple1) -tags {} -width \
			1]
		set i [$Global(CurrentCanvas) create polygon 128.12386206 \
			65.5959036929 47.9129114388 65.5959036933 47.9129114386 \
			25.4904283825 -fill #FFFFFFFFFFFF -outline black -smooth 0 \
			-splinesteps 12 -stipple $Global(ButtonStipple2) -tags {} -width \
			1]
		set i [$Global(CurrentCanvas) create line 141.492353831 \
			78.9643954631 128.118926002 78.9841396972 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 114.75537029 \
			78.9643954632 101.372070344 78.9841396973 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 88.01838675 \
			78.9643954634 74.6252146869 78.9841396974 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 61.2814032089 \
			78.9643954635 47.8783590289 78.9841396976 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 34.5444196689 \
			78.9643954637 34.5049312009 78.9841396977 21.1315033719 \
			78.9841396977 -arrow none -arrowshape {8 10 3} -capstyle butt \
			-fill black -joinstyle miter -smooth 0 -splinesteps 12 -stipple \
			{} -tags {} -width 1]
	}}
	{Button Flip_Vert {
		set i [$Global(CurrentCanvas) create polygon 96.337823292 \
			36.8761379395 96.337823292 117.087088561 136.443298603 \
			117.087088561 -fill $Global(ButtonColor) -outline black -smooth 0 \
			-splinesteps 12 -stipple $Global(ButtonStipple1) -tags {} -width \
			1]
		set i [$Global(CurrentCanvas) create polygon 69.5959036931 \
			36.8761379395 69.5959036931 117.087088561 29.4904283823 \
			117.087088561 -fill #FFFFFFFFFFFF -outline black -smooth 0 \
			-splinesteps 12 -stipple $Global(ButtonStipple2) -tags {} -width \
			1]
		set i [$Global(CurrentCanvas) create line 82.9643954634 \
			23.5076461692 82.9841396974 36.881073998 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 82.9643954634 \
			50.2446297098 82.9841396974 63.6279296555 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 82.9643954634 \
			76.9816132503 82.9841396974 90.374785313 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 82.9643954634 \
			103.718596791 82.9841396974 117.121640971 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 82.9643954634 \
			130.455580331 82.9841396974 130.495068799 82.9841396974 \
			143.868496628 -arrow none -arrowshape {8 10 3} -capstyle butt \
			-fill black -joinstyle miter -smooth 0 -splinesteps 12 -stipple \
			{} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon -1.0 -1.0 24.0 -1.0 \
			24.0 -1.0 146.0 -1.0 146.0 -1.0 171.0 -1.0 171.0 24.0 171.0 24.0 \
			171.0 136.0 171.0 136.0 171.0 161.0 146.0 161.0 146.0 161.0 24.0 \
			161.0 24.0 161.0 -1.0 161.0 -1.0 136.0 -1.0 136.0 -1.0 24.0 -1.0 \
			24.0 -fill {} -outline {} -smooth 0 -splinesteps 12 -stipple {} \
			-tags {} -width 1]
	}}
	{Button Raise {
		set i [$Global(CurrentCanvas) create polygon -1.0 -1.0 \
			24.0 -1.0 24.0 -1.0 146.0 -1.0 146.0 -1.0 171.0 -1.0 171.0 24.0 \
			171.0 24.0 171.0 136.0 171.0 136.0 171.0 161.0 146.0 161.0 146.0 \
			161.0 24.0 161.0 24.0 161.0 -1.0 161.0 -1.0 136.0 -1.0 136.0 -1.0 \
			24.0 -1.0 24.0 -fill {} -outline {} -smooth 0 -splinesteps 12 \
			-stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon \
			24.5407392605 100.0 24.5407392605 25.0 127.76001998 25.0 \
			127.76001998 50.0 45.1845954043 50.0 45.1845954043 100.0 -fill \
			$Global(ButtonColor) -outline black -smooth 0 -splinesteps 12 \
			-stipple $Global(ButtonStipple1) -tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon \
			45.2913082605 50.0184615385 65.9351644043 50.0184615385 \
			65.9351644043 50.0184615385 127.90484457 50.0184615385 \
			127.90484457 50.0184615385 148.548700714 50.0184615385 \
			148.548700714 75.0184615385 148.548700714 75.0184615385 \
			148.548700714 100.046153846 148.548700714 100.046153846 \
			148.548700714 125.046153846 127.90484457 125.046153846 \
			127.90484457 125.046153846 65.9351644043 125.046153846 \
			65.9351644043 125.046153846 45.2913082605 125.046153846 \
			45.2913082605 100.046153846 45.2913082605 100.046153846 \
			45.2913082605 75.0184615385 45.2913082605 75.0184615385 -fill \
			#FFFFFFFFFFFF -outline black -smooth 0 -splinesteps 12 -stipple \
			$Global(ButtonStipple2) -tags {} -width 1]
	}}
	{Button Lower {
		set i [$Global(CurrentCanvas) create polygon -1.0 -1.0 \
			24.0 -1.0 24.0 -1.0 146.0 -1.0 146.0 -1.0 171.0 -1.0 171.0 24.0 \
			171.0 24.0 171.0 136.0 171.0 136.0 171.0 161.0 146.0 161.0 146.0 \
			161.0 24.0 161.0 24.0 161.0 -1.0 161.0 -1.0 136.0 -1.0 136.0 -1.0 \
			24.0 -1.0 24.0 -fill {} -outline {} -smooth 0 -splinesteps 12 \
			-stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon 24.63982977 \
			25.0092307692 45.2836859138 25.0092307692 45.2836859138 \
			25.0092307692 107.253366079 25.0092307692 107.253366079 \
			25.0092307692 127.897222223 25.0092307692 127.897222223 \
			50.0092307692 127.897222223 50.0092307692 127.897222223 \
			75.036923077 127.897222223 75.036923077 127.897222223 \
			100.036923077 107.253366079 100.036923077 107.253366079 \
			100.036923077 45.2836859138 100.036923077 45.2836859138 \
			100.036923077 24.63982977 100.036923077 24.63982977 75.036923077 \
			24.63982977 75.036923077 24.63982977 50.0092307692 24.63982977 \
			50.0092307692 -fill $Global(ButtonColor) -outline black -smooth 0 \
			-splinesteps 12 -stipple $Global(ButtonStipple1) -tags {} -width \
			1]
		set i [$Global(CurrentCanvas) create polygon \
			45.1845954043 100.0 45.1845954043 125.0 148.403876123 125.0 \
			148.403876123 50.0 127.760019979 50.0 127.760019979 100.0 \
			45.1845954043 100.0 -fill #FFFFFFFFFFFF -outline black -smooth 0 \
			-splinesteps 12 -stipple $Global(ButtonStipple2) -tags {} -width \
			1]
		set i [$Global(CurrentCanvas) create polygon -1.0 -1.0 \
			24.0 -1.0 24.0 -1.0 146.0 -1.0 146.0 -1.0 171.0 -1.0 171.0 24.0 \
			171.0 24.0 171.0 136.0 171.0 136.0 171.0 161.0 146.0 161.0 146.0 \
			161.0 24.0 161.0 24.0 161.0 -1.0 161.0 -1.0 136.0 -1.0 136.0 -1.0 \
			24.0 -1.0 24.0 -fill {} -outline {} -smooth 0 -splinesteps 12 \
			-stipple {} -tags {} -width 1]
	}}
	{Button Group {
		set i [$Global(CurrentCanvas) create polygon 0.0 -1.0 \
			25.0 -1.0 25.0 -1.0 147.0 -1.0 147.0 -1.0 172.0 -1.0 172.0 24.0 \
			172.0 24.0 172.0 136.0 172.0 136.0 172.0 161.0 147.0 161.0 147.0 \
			161.0 25.0 161.0 25.0 161.0 0.0 161.0 0.0 136.0 0.0 136.0 0.0 \
			24.0 0.0 24.0 -fill {} -outline {} -smooth 0 -splinesteps 12 \
			-stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon \
			19.4872975781 12.4155418363 38.9211441513 12.4155418363 \
			38.9211441513 12.4155418363 136.140606036 12.4155418363 \
			136.140606036 12.4155418363 155.574452609 12.4155418363 \
			155.574452609 31.8493884095 155.574452609 31.8493884095 \
			155.574452609 129.068850295 155.574452609 129.068850295 \
			155.574452609 148.502696868 136.140606036 148.502696868 \
			136.140606036 148.502696868 38.9211441513 148.502696868 \
			38.9211441513 148.502696868 19.4872975781 148.502696868 \
			19.4872975781 129.068850295 19.4872975781 129.068850295 \
			19.4872975781 31.8493884095 19.4872975781 31.8493884095 -fill {} \
			-outline black -smooth 0 -splinesteps 12 -stipple {} -tags {} \
			-width 2]
		set i [$Global(CurrentCanvas) create polygon \
			140.020199776 43.9055034438 139.429494156 46.7899733119 \
			137.675325586 49.5868000225 134.810993592 52.2110034117 \
			130.92352945 54.5828483873 126.131051794 56.6302676392 \
			120.579177629 58.2910513706 114.436597844 59.5147375074 \
			107.889951617 60.2641449661 101.138155481 60.5165033797 \
			94.3863593463 60.2641449661 87.8397131192 59.5147375074 \
			81.6971333342 58.2910513706 76.1452591699 56.6302676392 \
			71.352781513 54.5828483873 67.4653173715 52.2110034117 \
			64.6009853772 49.5868000225 62.8468168077 46.7899733119 \
			62.2561111871 43.9055034438 62.8468168077 41.0210335757 \
			64.6009853772 38.224206865 67.4653173715 35.6000034759 \
			71.352781513 33.2281585006 76.1452591699 31.1807392483 \
			81.6971333342 29.5199555171 87.8397131192 28.2962693802 \
			94.3863593463 27.5468619217 101.138155481 27.2945035079 \
			107.889951617 27.5468619217 114.436597844 28.2962693802 \
			120.579177629 29.5199555171 126.131051794 31.1807392483 \
			130.92352945 33.2281585006 134.810993592 35.6000034759 \
			137.675325586 38.224206865 139.429494156 41.0210335757 -fill \
			$Global(ButtonColor) -outline black -smooth 1 -splinesteps 12 \
			-stipple $Global(ButtonStipple1) -tags oval -width 1]
		set i [$Global(CurrentCanvas) create polygon \
			31.8460280693 105.690829814 51.2798746425 47.389290094 \
			129.015260935 105.690829814 -fill $Global(ButtonColor) -outline \
			black -smooth 0 -splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon \
			58.5278024676 70.7386082783 79.8302881343 70.7386082783 \
			79.8302881343 70.7386082783 122.466721601 70.7386082783 \
			122.466721601 70.7386082783 143.769207267 70.7386082783 \
			143.769207267 90.1724548516 143.769207267 90.1724548516 \
			143.769207267 109.627828147 143.769207267 109.627828147 \
			143.769207267 129.061674721 122.466721601 129.061674721 \
			122.466721601 129.061674721 79.8302881343 129.061674721 \
			79.8302881343 129.061674721 58.5278024676 129.061674721 \
			58.5278024676 109.627828147 58.5278024676 109.627828147 \
			58.5278024676 90.1724548516 58.5278024676 90.1724548516 -fill \
			#FFFFFFFFFFFF -outline black -smooth 0 -splinesteps 12 -stipple \
			{} -tags {} -width 1]
	}}
	{Button UnGroup {
		set i [$Global(CurrentCanvas) create line 18.1860465116 \
			39.3023255812 18.297030412 12.1805605247 45.6327012525 \
			12.1805605247 -arrow none -arrowshape {8 10 3} -capstyle butt \
			-fill black -joinstyle miter -smooth 0 -splinesteps 12 -stipple \
			{} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 72.8372093023 \
			12.1705426356 100.304042933 12.1805605247 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 127.488372093 \
			12.1705426356 154.975384616 12.1805605247 154.975384616 \
			39.3223613595 -arrow none -arrowshape {8 10 3} -capstyle butt \
			-fill black -joinstyle miter -smooth 0 -splinesteps 12 -stipple \
			{} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 154.813953488 \
			66.4341085268 154.975384616 93.6059630289 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 154.813953488 \
			120.697674418 154.975384616 147.889564698 127.639713774 \
			147.889564698 -arrow none -arrowshape {8 10 3} -capstyle butt \
			-fill black -joinstyle miter -smooth 0 -splinesteps 12 -stipple \
			{} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 100.162790698 \
			147.829457364 72.968372093 147.889564698 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 45.511627907 \
			147.829457364 18.297030412 147.889564698 18.297030412 \
			120.747763863 -arrow none -arrowshape {8 10 3} -capstyle butt \
			-fill black -joinstyle miter -smooth 0 -splinesteps 12 -stipple \
			{} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 18.1860465116 \
			93.5658914724 18.297030412 66.4641621941 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon 0.0 -1.0 \
			25.0 -1.0 25.0 -1.0 147.0 -1.0 147.0 -1.0 172.0 -1.0 172.0 24.0 \
			172.0 24.0 172.0 136.0 172.0 136.0 172.0 161.0 147.0 161.0 147.0 \
			161.0 25.0 161.0 25.0 161.0 0.0 161.0 0.0 136.0 0.0 136.0 0.0 \
			24.0 0.0 24.0 -fill {} -outline {} -smooth 0 -splinesteps 12 \
			-stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon \
			140.020199776 43.9055034438 139.429494156 46.7899733119 \
			137.675325586 49.5868000225 134.810993592 52.2110034117 \
			130.92352945 54.5828483873 126.131051794 56.6302676392 \
			120.579177629 58.2910513706 114.436597844 59.5147375074 \
			107.889951617 60.2641449661 101.138155481 60.5165033797 \
			94.3863593463 60.2641449661 87.8397131192 59.5147375074 \
			81.6971333342 58.2910513706 76.1452591699 56.6302676392 \
			71.352781513 54.5828483873 67.4653173715 52.2110034117 \
			64.6009853772 49.5868000225 62.8468168077 46.7899733119 \
			62.2561111871 43.9055034438 62.8468168077 41.0210335757 \
			64.6009853772 38.224206865 67.4653173715 35.6000034759 \
			71.352781513 33.2281585006 76.1452591699 31.1807392483 \
			81.6971333342 29.5199555171 87.8397131192 28.2962693802 \
			94.3863593463 27.5468619217 101.138155481 27.2945035079 \
			107.889951617 27.5468619217 114.436597844 28.2962693802 \
			120.579177629 29.5199555171 126.131051794 31.1807392483 \
			130.92352945 33.2281585006 134.810993592 35.6000034759 \
			137.675325586 38.224206865 139.429494156 41.0210335757 -fill \
			$Global(ButtonColor) -outline black -smooth 1 -splinesteps 12 \
			-stipple $Global(ButtonStipple1) -tags oval -width 1]
		set i [$Global(CurrentCanvas) create polygon \
			31.8460280693 105.690829814 51.2798746425 47.389290094 \
			129.015260935 105.690829814 -fill $Global(ButtonColor) -outline \
			black -smooth 0 -splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon \
			58.5278024676 70.7386082783 79.8302881343 70.7386082783 \
			79.8302881343 70.7386082783 122.466721601 70.7386082783 \
			122.466721601 70.7386082783 143.769207267 70.7386082783 \
			143.769207267 90.1724548516 143.769207267 90.1724548516 \
			143.769207267 109.627828147 143.769207267 109.627828147 \
			143.769207267 129.061674721 122.466721601 129.061674721 \
			122.466721601 129.061674721 79.8302881343 129.061674721 \
			79.8302881343 129.061674721 58.5278024676 129.061674721 \
			58.5278024676 109.627828147 58.5278024676 109.627828147 \
			58.5278024676 90.1724548516 58.5278024676 90.1724548516 -fill \
			#FFFFFFFFFFFF -outline black -smooth 0 -splinesteps 12 -stipple \
			{} -tags {} -width 1]
	}}
	{RadioButton Cut {
		set i [$Global(CurrentCanvas) create polygon -1.0 -2.0 \
			24.0 -2.0 24.0 -2.0 146.0 -2.0 146.0 -2.0 171.0 -2.0 171.0 23.0 \
			171.0 23.0 171.0 135.0 171.0 135.0 171.0 160.0 146.0 160.0 146.0 \
			160.0 24.0 160.0 24.0 160.0 -1.0 160.0 -1.0 135.0 -1.0 135.0 -1.0 \
			23.0 -1.0 23.0 -fill {} -outline {} -smooth 0 -splinesteps 12 \
			-stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon \
			63.5388025603 6.9140133338 75.8988998203 77.453529439 \
			60.0668341612 70.8645988339 44.229360387 62.4943797334 \
			28.3270045365 53.8115600292 12.3008923946 46.2880351751 \
			16.6934038006 51.5511429489 21.7484124676 56.3004646424 \
			27.3362349823 60.6346877942 33.3273977037 64.6514344314 \
			39.5908844671 68.4476335052 45.9966330027 72.1213384293 \
			52.4146464512 75.7705546807 58.7146009345 79.493527403 \
			64.7661861443 83.3875800132 70.4395360295 87.5506221311 \
			75.604719127 92.080611307 80.1309673092 97.075206478 83.888414522 \
			102.632317268 86.7466850558 108.849315026 88.5755858388 \
			115.824349027 89.245120006 123.655424754 88.9390819701 \
			124.711246498 88.1899034817 125.887593214 85.0587164493 \
			126.96146656 81.9162149024 128.649961096 79.3727228435 \
			130.977178405 78.0363677075 133.967006319 78.2920498845 \
			140.948898406 81.1387372648 146.92000033 84.4369342348 \
			149.710915407 87.9357554379 151.056434961 96.8059144196 \
			146.65288348 97.7731426938 141.441699615 96.9262229088 \
			136.360148001 94.9145764852 131.356264886 92.3887367034 \
			126.377271678 90.0001253411 121.371562181 88.3979947574 \
			116.285472997 88.2334532841 111.066715874 90.1568898244 \
			105.663529798 92.6751898339 102.551502676 95.6331489542 \
			100.271569609 98.8543932576 98.94295938 102.163933398 \
			98.687533161 107.030055872 99.792335968 111.40010308 \
			101.998319533 115.425441998 104.907343727 119.257818444 \
			108.121902549 123.048795605 111.243712084 126.948590342 \
			113.873651536 131.108504223 115.613628705 135.680086862 \
			116.066281396 138.541342157 115.019730004 141.009415097 \
			113.517321311 142.827397836 111.585038474 143.738251727 \
			109.248960512 143.105776486 102.407352701 138.606045263 \
			97.510031144 135.679562378 96.215539786 132.537415684 \
			95.643488515 129.295139508  \
			95.794912928 126.06698727 96.669963826 124.073367001 \
			98.082694536 122.467101186 99.60452094 121.084832186 \
			101.270367238 119.761136598 103.11484801 90.480227109 \
			88.1028765011 88.2978844307 82.1691691271 86.838656662 \
			76.1832852353 81.4975130574 58.5542458718 76.9810262708 \
			40.4402866063 71.6667017213 22.8089161852 63.9311563203 \
			6.6264709607 63.5388025603 6.9140133338 -fill \
			$Global(ButtonColor) -outline black -smooth 0 -splinesteps 12 \
			-stipple $Global(ButtonStipple1) -tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon \
			126.510482112 99.782778734 125.281930984 100.886679259 \
			124.333027471 102.199358547 123.745148065 103.666993989 \
			123.600632868 105.238379854 125.004350593 108.340225379 \
			127.15512505 110.754179219 129.757078443 112.432895488 \
			132.512569926 113.328658829 134.822988095 113.179832755 \
			137.065645556 112.38032825 138.962406678 111.055058193 \
			140.234813146 109.330833488 140.765639214 108.058702627 \
			141.196281095 107.296977514 139.950422302 104.259719468 \
			137.714152321 101.738945526 134.893578486 99.754387391 \
			131.896308071 98.328000582 126.510482112 99.782778734 -fill \
			$Global(DefButtonColor) -outline $Global(DefButtonColor) -smooth \
			0 -splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon 85.124365772 \
			130.833572526 83.1687403834 132.206525778 81.6843201596 \
			133.939782386 80.754074319 135.965401489 80.4586971855 \
			138.217109414 81.5139673823 141.256536376 83.2077851804 \
			143.813722859 85.3504539501 145.804108261 87.7534145098 \
			147.142298379 89.3845585661 146.918126511 91.162689649 \
			146.110525232 92.7119136198 144.93551076 93.6563363455 \
			143.609099314 94.215893959 142.435508131 94.4235737431 \
			141.936833055 94.2390419018 138.807921963 93.3440933132 \
			135.463889179 91.5789640029 132.503870659 88.7857195863 \
			130.528832913 87.1583159762 130.204785312 85.124365772 \
			130.833572526 -fill $Global(DefButtonColor) -outline \
			$Global(DefButtonColor) -smooth 0 -splinesteps 12 -stipple {} \
			-tags {} -width 1]
	}}
	{RadioButton Paste {
		set i [$Global(CurrentCanvas) create polygon 3.0 2.0 28.0 \
			2.0 28.0 2.0 152.0 2.0 152.0 2.0 177.0 2.0 177.0 27.0 177.0 27.0 \
			177.0 141.0 177.0 141.0 177.0 166.0 152.0 166.0 152.0 166.0 28.0 \
			166.0 28.0 166.0 3.0 166.0 3.0 141.0 3.0 141.0 3.0 27.0 3.0 27.0 \
			-fill {} -outline {} -smooth 0 -splinesteps 12 -stipple {} -tags \
			{} -width 1]
		set i [$Global(CurrentCanvas) create polygon \
			67.9240825065 39.915838224 67.9240825065 35.0770838995 \
			67.9240825065 35.0770838995 71.7703623055 35.0770838995 \
			71.7703623055 35.0770838995 74.3345488381 20.5608209258 \
			82.0271084359 6.04455795217 85.8733882348 6.04455795217 \
			93.5659478326 20.5608209258 97.4122276315 35.0770838995 \
			99.9764141641 35.0770838995 99.9764141641 35.0770838995 \
			99.9764141641 39.915838224 67.9240825065 39.915838224 -fill \
			$Global(ButtonColor) -outline black -smooth 1 -splinesteps 12 \
			-stipple $Global(ButtonStipple2) -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 69.2061757729 \
			36.0448347644 75.6417316834 37.0250919481 85.90226492 \
			37.0250919481 93.5976648475 37.0250919481 97.4453648113 \
			36.0569837599 -arrow none -arrowshape {8 10 3} -capstyle butt \
			-fill $Global(ButtonColor) -joinstyle miter -smooth 1 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon \
			42.5939143386 73.5625787978 50.2306882263 64.5549266551 \
			65.5042360016 61.5523759408 69.3226229455 55.5472745123 \
			99.8697184961 55.5472745123 103.68810544 61.5523759408 \
			118.961653215 64.5549266551 126.598427103 73.5625787978 -fill \
			white -outline black -smooth 1 -splinesteps 12 -stipple \
			$Global(ButtonStipple2) -tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon 66.744924848 \
			40.5334123072 66.744924848 40.5334123072 66.744924848 \
			52.5436151642 66.744924848 52.5436151642 84.5571027282 \
			55.5461658784 102.369280608 52.5436151642 102.369280608 \
			52.5436151642 102.369280608 40.5334123072 102.369280608 \
			40.5334123072 84.5571027282 43.5359630214 66.744924848 \
			40.5334123072 66.744924848 40.5334123072 -fill \
			$Global(ButtonColor) -outline black -smooth 1 -splinesteps 12 \
			-stipple $Global(ButtonStipple2) -tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon \
			45.3703113914 70.5589194493 45.3703113914 70.5589194493 \
			45.3703113914 148.62523802 45.3703113914 148.62523802 \
			84.5571027279 151.627788734 123.743894064 148.62523802 \
			123.743894064 148.62523802 123.743894064 70.5589194493 \
			123.743894064 70.5589194493 80.9946671518 73.5614701636 \
			45.3703113914 70.5589194493 45.3703113914 70.5589194493 -fill \
			white -outline black -smooth 1 -splinesteps 12 -stipple \
			$Global(ButtonStipple2) -tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon \
			52.5285150975 79.5654629582 52.5285150975 79.5654629582 \
			52.5285150975 139.616477243 52.5285150975 139.616477243 \
			86.0904001986 142.619027957 115.923186955 139.616477243 \
			115.923186955 139.616477243 115.923186955 79.5654629582 \
			115.923186955 79.5654629582 86.0904001986 82.5680136724 \
			52.5285150975 79.5654629582 -fill $Global(ButtonColor) -outline \
			black -smooth 1 -splinesteps 12 -stipple $Global(ButtonStipple2) \
			-tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 70.307360424 \
			52.5436151642 70.3402444446 43.5514838991 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 1 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 73.8697960001 \
			52.5436151642 73.9039953816 43.5514838991 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 1 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon \
			45.3703113917 148.62523802 45.3703113917 148.62523802 \
			84.5571027282 151.627788734 123.743894065 148.62523802 \
			123.743894065 148.62523802 120.181458489 154.630339448 \
			84.5571027282 157.632890163 48.9327469678 154.630339448 \
			45.3703113917 148.62523802 -fill white -outline black -smooth 1 \
			-splinesteps 12 -stipple $Global(ButtonStipple2) -tags {} -width \
			1]
	}}
	{JoinButton round {
		set i [$Global(CurrentCanvas) create polygon 0.0 0.0 \
			25.0 0.0 25.0 0.0 175.073846154 0.0 175.073846154 0.0 \
			200.073846154 0.0 200.073846154 25.0 200.073846154 25.0 \
			200.073846154 175.073846154 200.073846154 175.073846154 \
			200.073846154 200.073846154 175.073846154 200.073846154 \
			175.073846154 200.073846154 25.0 200.073846154 25.0 200.073846154 \
			0.0 200.073846154 0.0 175.073846154 0.0 175.073846154 0.0 25.0 \
			0.0 25.0 -fill {} -outline {} -smooth 0 -splinesteps 12 -stipple \
			{} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon  28.0830769234 \
			177.083076923 28.0830769234 177.083076923  29.0830769234 \
			90.0830769232 28.0830769234 89.0830769232  32.0830769234 \
			31.0830769232  90.0830769234 27.0830769232 90.0830769234 \
			27.0830769232  178.083076923 27.0830769232 178.083076923 \
			27.0830769232  178.083076923 77.0830769232 178.083076923 \
			77.0830769232  78.0830769234 77.0830769232 78.0830769234 \
			77.0830769232  78.0830769234 177.083076923 78.0830769234 \
			177.083076923  28.0830769234 177.083076923 28.0830769234 \
			177.083076923 -fill $Global(ButtonColor) -outline black -smooth 1 \
			-splinesteps 12 -stipple $Global(ButtonStipple1) -tags {} -width \
			1]
		set i [$Global(CurrentCanvas) create line 52.0184615385 \
			185.046153846 52.036923077 135.101538461 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 1 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 68.0646153847 \
			53.036923077 118.101538462 53.0553846155 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 1 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 52.0184615385 \
			116.046153846 52.036923077 66.101538461 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 1 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 137.064615385 \
			53.036923077 187.101538462 53.0553846155 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 1 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
	}}
	{JoinButton bevel {
		set i [$Global(CurrentCanvas) create polygon 0.0 0.0 \
			25.0 0.0 25.0 0.0 175.073846154 0.0 175.073846154 0.0 \
			200.073846154 0.0 200.073846154 25.0 200.073846154 25.0 \
			200.073846154 175.073846154 200.073846154 175.073846154 \
			200.073846154 200.073846154 175.073846154 200.073846154 \
			175.073846154 200.073846154 25.0 200.073846154 25.0 200.073846154 \
			0.0 200.073846154 0.0 175.073846154 0.0 175.073846154 0.0 25.0 \
			0.0 25.0 -fill {} -outline {} -smooth 0 -splinesteps 12 -stipple \
			{} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 52.0184615385 \
			185.046153846 52.036923077 135.101538461 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 1 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 68.0646153847 \
			53.036923077 118.101538462 53.0553846155 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 1 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 52.0184615385 \
			116.046153846 52.036923077 66.101538461 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 1 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 137.064615385 \
			53.036923077 187.101538462 53.0553846155 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 1 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon 27.0092307692 \
			177.009230769 27.0092307692 77.0092307692 77.0092307692 \
			27.0092307692 177.009230769 27.0092307692 177.009230769 \
			77.0092307692 77.0092307692 77.0092307692 77.0092307692 \
			177.009230769 -fill $Global(ButtonColor) -outline black -smooth 0 \
			-splinesteps 12 -stipple $Global(ButtonStipple1) -tags {} -width \
			1]
	}}
	{JoinButton miter {
		set i [$Global(CurrentCanvas) create polygon 0.0 0.0 \
			25.0 0.0 25.0 0.0 175.073846154 0.0 175.073846154 0.0 \
			200.073846154 0.0 200.073846154 25.0 200.073846154 25.0 \
			200.073846154 175.073846154 200.073846154 175.073846154 \
			200.073846154 200.073846154 175.073846154 200.073846154 \
			175.073846154 200.073846154 25.0 200.073846154 25.0 200.073846154 \
			0.0 200.073846154 0.0 175.073846154 0.0 175.073846154 0.0 25.0 \
			0.0 25.0 -fill {} -outline {} -smooth 0 -splinesteps 12 -stipple \
			{} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 52.0184615385 \
			185.046153846 52.036923077 135.101538461 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 1 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 68.0646153847 \
			53.036923077 118.101538462 53.0553846155 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 1 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 52.0184615385 \
			116.046153846 52.036923077 66.101538461 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 1 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 137.064615385 \
			53.036923077 187.101538462 53.0553846155 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 1 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon 27.0092307692 \
			177.009230769 27.0092307692 27.0092307692 177.009230769 \
			27.0092307692 177.009230769 77.0092307692 77.0092307692 \
			77.0092307692 77.0092307692 177.009230769 -fill \
			$Global(ButtonColor) -outline black -smooth 0 -splinesteps 12 \
			-stipple $Global(ButtonStipple1) -tags {} -width 1]
	}}
	{ArrowButton none {
		set i [$Global(CurrentCanvas) create polygon \
			-0.852307692096 -0.935384615596 24.1476923079 -0.935384615596 \
			24.1476923079 -0.935384615596 174.221538462 -0.935384615596 \
			174.221538462 -0.935384615596 199.221538462 -0.935384615596 \
			199.221538462 24.0646153844 199.221538462 24.0646153844 \
			199.221538462 174.138461538 199.221538462 174.138461538 \
			199.221538462 199.138461538 174.221538462 199.138461538 \
			174.221538462 199.138461538 24.1476923079 199.138461538 \
			24.1476923079 199.138461538 -0.852307692096 199.138461538 \
			-0.852307692096 174.138461538 -0.852307692096 174.138461538 \
			-0.852307692096 24.0646153844 -0.852307692096 24.0646153844 -fill \
			{} -outline {} -smooth 0 -splinesteps 12 -stipple {} -tags {} \
			-width 1]
		set i [$Global(CurrentCanvas) create polygon 184.696113424 \
			18.5089433898 182.854879064 19.4099729702 182.854879064 \
			19.4099729702 14.3819350668 101.854179608 14.3819350668 \
			101.854179608 12.5407007052 102.755209189 13.0984781326 \
			103.895015236 13.0984781326 103.895015236 13.3773668463 \
			104.46491826 13.3773668463 104.46491826 13.9351442737 \
			105.604724307 15.7763786354 104.703694726 15.7763786354 \
			104.703694726 184.249322633 22.2594880885 184.249322633 \
			22.2594880885 186.090556993 21.3584585081 185.532779566 \
			20.2186524607 185.532779566 20.2186524607 185.253890852 \
			19.6487494371 185.253890852 19.6487494371 -fill \
			$Global(ButtonColor) -outline $Global(ButtonColor) -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon 6.75368204265 \
			110.753682043 37.4077288844 177.011127039 40.1218892818 \
			154.978530872 59.1210120639 179.246317955 61.8351724613 \
			176.851470545 42.9957061733 152.424026968 65.0283023407 \
			154.49956139 -fill $Global(ButtonColor) -outline black -smooth 0 \
			-splinesteps 12 -stipple $Global(ButtonStipple2) -tags {} -width \
			1]
	}}
	{ArrowButton first {
		set i [$Global(CurrentCanvas) create polygon \
			-0.852307692096 -0.935384615596 24.1476923079 -0.935384615596 \
			24.1476923079 -0.935384615596 174.221538462 -0.935384615596 \
			174.221538462 -0.935384615596 199.221538462 -0.935384615596 \
			199.221538462 24.0646153844 199.221538462 24.0646153844 \
			199.221538462 174.138461538 199.221538462 174.138461538 \
			199.221538462 199.138461538 174.221538462 199.138461538 \
			174.221538462 199.138461538 24.1476923079 199.138461538 \
			24.1476923079 199.138461538 -0.852307692096 199.138461538 \
			-0.852307692096 174.138461538 -0.852307692096 174.138461538 \
			-0.852307692096 24.0646153844 -0.852307692096 24.0646153844 -fill \
			{} -outline {} -smooth 0 -splinesteps 12 -stipple {} -tags {} \
			-width 1]
		set i [$Global(CurrentCanvas) create polygon 184.696113424 \
			18.5089433898 182.854879064 19.4099729702 182.854879064 \
			19.4099729702 14.3819350668 101.854179608 14.3819350668 \
			101.854179608 12.5407007052 102.755209189 13.0984781326 \
			103.895015236 13.0984781326 103.895015236 13.3773668463 \
			104.46491826 13.3773668463 104.46491826 13.9351442737 \
			105.604724307 15.7763786354 104.703694726 15.7763786354 \
			104.703694726 184.249322633 22.2594880885 184.249322633 \
			22.2594880885 186.090556993 21.3584585081 185.532779566 \
			20.2186524607 185.532779566 20.2186524607 185.253890852 \
			19.6487494371 185.253890852 19.6487494371 -fill \
			$Global(ButtonColor) -outline $Global(ButtonColor) -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon 191.713913476 \
			17.2134035549 127.751393269 30.8546993336 148.971186702 \
			38.1300570825 141.695828954 59.3498505155 -fill \
			$Global(ButtonColor) -outline $Global(ButtonColor) -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon 6.75368204265 \
			110.753682043 37.4077288844 177.011127039 40.1218892818 \
			154.978530872 59.1210120639 179.246317955 61.8351724613 \
			176.851470545 42.9957061733 152.424026968 65.0283023407 \
			154.49956139 -fill $Global(ButtonColor) -outline black -smooth 0 \
			-splinesteps 12 -stipple $Global(ButtonStipple2) -tags {} -width \
			1]
	}}
	{ArrowButton last {
		set i [$Global(CurrentCanvas) create polygon \
			-0.852307692097 -0.935384615597 24.1476923079 -0.935384615597 \
			24.1476923079 -0.935384615597 174.221538462 -0.935384615597 \
			174.221538462 -0.935384615597 199.221538462 -0.935384615597 \
			199.221538462 24.0646153844 199.221538462 24.0646153844 \
			199.221538462 174.138461538 199.221538462 174.138461538 \
			199.221538462 199.138461538 174.221538462 199.138461538 \
			174.221538462 199.138461538 24.1476923079 199.138461538 \
			24.1476923079 199.138461538 -0.852307692097 199.138461538 \
			-0.852307692097 174.138461538 -0.852307692097 174.138461538 \
			-0.852307692097 24.0646153844 -0.852307692097 24.0646153844 -fill \
			{} -outline {} -smooth 0 -splinesteps 12 -stipple {} -tags {} \
			-width 1]
		set i [$Global(CurrentCanvas) create polygon 184.696113424 \
			18.5089433898 182.854879064 19.4099729702 182.854879064 \
			19.4099729702 14.3819350668 101.854179608 14.3819350668 \
			101.854179608 12.5407007052 102.755209189 13.0984781326 \
			103.895015236 13.0984781326 103.895015236 13.3773668463 \
			104.46491826 13.3773668463 104.46491826 13.9351442737 \
			105.604724307 15.7763786354 104.703694726 15.7763786354 \
			104.703694726 184.249322633 22.2594880885 184.249322633 \
			22.2594880885 186.090556993 21.3584585081 185.532779566 \
			20.2186524607 185.532779566 20.2186524607 185.253890852 \
			19.6487494371 185.253890852 19.6487494371 -fill \
			$Global(ButtonColor) -outline $Global(ButtonColor) -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon 5.82513715908 \
			108.18025154 55.8432216805 66.0438045788 48.5678639327 \
			87.2635980128 69.7876573655 94.5389557608 -fill \
			$Global(ButtonColor) -outline $Global(ButtonColor) -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon 6.75368204266 \
			110.753682043 37.4077288844 177.011127039 40.1218892818 \
			154.978530872 59.121012064 179.246317955 61.8351724614 \
			176.851470545 42.9957061733 152.424026968 65.0283023408 \
			154.49956139 -fill $Global(ButtonColor) -outline black -smooth 0 \
			-splinesteps 12 -stipple $Global(ButtonStipple2) -tags {} -width \
			1]
	}}
	{ArrowButton both {
		set i [$Global(CurrentCanvas) create polygon \
			-0.852307692097 -0.935384615597 24.1476923079 -0.935384615597 \
			24.1476923079 -0.935384615597 174.221538462 -0.935384615597 \
			174.221538462 -0.935384615597 199.221538462 -0.935384615597 \
			199.221538462 24.0646153844 199.221538462 24.0646153844 \
			199.221538462 174.138461538 199.221538462 174.138461538 \
			199.221538462 199.138461538 174.221538462 199.138461538 \
			174.221538462 199.138461538 24.1476923079 199.138461538 \
			24.1476923079 199.138461538 -0.852307692097 199.138461538 \
			-0.852307692097 174.138461538 -0.852307692097 174.138461538 \
			-0.852307692097 24.0646153844 -0.852307692097 24.0646153844 -fill \
			{} -outline {} -smooth 0 -splinesteps 12 -stipple {} -tags {} \
			-width 1]
		set i [$Global(CurrentCanvas) create polygon 184.696113424 \
			18.5089433898 182.854879064 19.4099729702 182.854879064 \
			19.4099729702 14.3819350668 101.854179608 14.3819350668 \
			101.854179608 12.5407007052 102.755209189 13.0984781326 \
			103.895015236 13.0984781326 103.895015236 13.3773668463 \
			104.46491826 13.3773668463 104.46491826 13.9351442737 \
			105.604724307 15.7763786354 104.703694726 15.7763786354 \
			104.703694726 184.249322633 22.2594880885 184.249322633 \
			22.2594880885 186.090556993 21.3584585081 185.532779566 \
			20.2186524607 185.532779566 20.2186524607 185.253890852 \
			19.6487494371 185.253890852 19.6487494371 -fill \
			$Global(ButtonColor) -outline $Global(ButtonColor) -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon 191.713913476 \
			17.2134035549 127.751393269 30.8546993336 148.971186702 \
			38.1300570825 141.695828954 59.3498505156 -fill \
			$Global(ButtonColor) -outline $Global(ButtonColor) -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon 5.82513715908 \
			108.18025154 55.8432216805 66.0438045788 48.5678639327 \
			87.2635980128 69.7876573655 94.5389557608 -fill \
			$Global(ButtonColor) -outline $Global(ButtonColor) -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon 6.75368204266 \
			110.753682043 37.4077288844 177.011127039 40.1218892818 \
			154.978530872 59.121012064 179.246317955 61.8351724614 \
			176.851470545 42.9957061733 152.424026968 65.0283023408 \
			154.49956139 -fill $Global(ButtonColor) -outline black -smooth 0 \
			-splinesteps 12 -stipple $Global(ButtonStipple2) -tags {} -width \
			1]
	}}
	{Button Smooth {
		set i [$Global(CurrentCanvas) create polygon \
			-0.852307692096 -0.935384615596 24.1476923079 -0.935384615596 \
			24.1476923079 -0.935384615596 174.221538462 -0.935384615596 \
			174.221538462 -0.935384615596 199.221538462 -0.935384615596 \
			199.221538462 24.0646153844 199.221538462 24.0646153844 \
			199.221538462 174.138461538 199.221538462 174.138461538 \
			199.221538462 199.138461538 174.221538462 199.138461538 \
			174.221538462 199.138461538 24.1476923079 199.138461538 \
			24.1476923079 199.138461538 -0.852307692096 199.138461538 \
			-0.852307692096 174.138461538 -0.852307692096 174.138461538 \
			-0.852307692096 24.0646153844 -0.852307692096 24.0646153844 -fill \
			{} -outline {} -smooth 0 -splinesteps 12 -stipple {} -tags {} \
			-width 1]
		set i [$Global(CurrentCanvas) create line 86.3401486816 \
			100.316868031 115.86971549 100.360304043 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle round -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 145.312185136 \
			100.316868031 174.863526234 100.360304043 174.863526234 \
			70.9396457192 -arrow none -arrowshape {8 10 3} -capstyle butt \
			-fill black -joinstyle round -smooth 0 -splinesteps 12 -stipple \
			{} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 174.798203363 \
			70.9070687107 174.863526234 70.9396457192 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill $Global(ButtonColor) -joinstyle \
			round -smooth 0 -splinesteps 12 -stipple $Global(ButtonStipple2) \
			-tags {} -width 1]
		set Global(Smooth_Line) [$Global(CurrentCanvas) create line 25.0 \
			175.0 25.0092307692 100.036923077 100.036923077 100.036923077 \
			175.064615385 100.036923077 175.064615385 25.0092307692 -arrow \
			none -arrowshape {8 10 3} -capstyle butt -fill \
			$Global(ButtonColor) -joinstyle miter -smooth 0 -splinesteps 12 \
			-stipple $Global(ButtonStipple2) -tags {} -width 4]
		set i [$Global(CurrentCanvas) create line 25.0 125.0 \
			25.0092307692 100.036923077 50.0184615385 100.036923077 -arrow \
			none -arrowshape {8 10 3} -capstyle butt -fill black -joinstyle \
			miter -smooth 0 -splinesteps 12 -stipple {} -tags {} -width 1]
	}}
	{Button Rulers {
		set i [$Global(CurrentCanvas) create polygon 3.0 3.0 28.0 \
			3.0 28.0 3.0 150.0 3.0 150.0 3.0 175.0 3.0 175.0 28.0 175.0 28.0 \
			175.0 140.0 175.0 140.0 175.0 165.0 150.0 165.0 150.0 165.0 28.0 \
			165.0 28.0 165.0 3.0 165.0 3.0 140.0 3.0 140.0 3.0 28.0 3.0 28.0 \
			-fill {} -outline {} -smooth 0 -splinesteps 12 -stipple {} -tags \
			{} -width 1]
		set i [$Global(CurrentCanvas) create polygon \
			23.1891100047 21.8326710294 44.4914676605 21.8326710294 \
			44.4914676605 21.8326710294 55.1426464886 21.8326710294 \
			55.1426464886 21.8326710294 76.4450041446 21.8326710294 \
			76.4450041446 36.3892820942 76.4450041446 36.3892820942 \
			76.4450041446 134.646406785 76.4450041446 134.646406785 \
			76.4450041446 149.203017849 55.1426464886 149.203017849 \
			55.1426464886 149.203017849 44.4914676605 149.203017849 \
			44.4914676605 149.203017849 23.1891100047 149.203017849 \
			23.1891100047 134.646406785 23.1891100047 134.646406785 \
			23.1891100047 36.3892820942 23.1891100047 36.3892820942 -fill \
			$Global(ButtonColor) -outline black -smooth 0 -splinesteps 12 \
			-stipple $Global(ButtonStipple1) -tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon \
			150.149425814 22.2067661339 150.149425814 44.2377862974 \
			150.149425814 44.2377862974 150.149425814 55.2532963793 \
			150.149425814 55.2532963793 150.149425814 77.2843165428 \
			141.662699325 77.2843165428 141.662699325 77.2843165428 \
			84.3772955216 77.2843165441 84.3772955216 77.2843165441 \
			75.8905690324 77.2843165441 75.8905690324 55.2532963806 \
			75.8905690324 55.2532963806 75.8905690324 44.2377862985 \
			75.8905690324 44.2377862985 75.8905690324 22.206766135 \
			84.3772955216 22.2067661348 84.3772955216 22.2067661348 \
			141.662699325 22.2067661341 141.662699325 22.2067661341 -fill \
			$Global(ButtonColor) -outline black -smooth 0 -splinesteps 12 \
			-stipple $Global(ButtonStipple1) -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 140.131665428 \
			77.2080834672 140.121591494 63.5149289413 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 131.037141491 \
			77.2080834672 131.023709578 49.7455413384 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 121.942617553 \
			77.2080834672 121.925827663 63.5149289413 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 112.848093615 \
			77.2080834672 112.827945747 49.7455413384 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 103.753569678 \
			77.2080834672 103.730063832 63.5149289413 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 94.65904574 \
			77.2080834672 94.632181916 49.7455413384 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 85.5645218024 \
			77.2080834672 85.5343 63.5149289413 -arrow none -arrowshape {8 10 \
			3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 76.4699978647 \
			77.2080834672 76.436418084 49.7455413384 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 76.4843170578 \
			77.2080834672 76.4253476873 49.7455413384 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 76.3712924314 \
			30.9204790109 63.1310306103 30.9305529451 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 76.3712924314 \
			40.0150029485 49.8170570746 40.0284348608 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 76.3712924314 \
			49.1095268862 63.1310306103 49.1263167766 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 76.3712924314 \
			58.2040508238 49.8170570746 58.2241986923 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 76.3712924314 \
			67.2985747615 63.1310306103 67.3220806079 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 76.3712924314 \
			76.3930986992 49.8170570746 76.4199625237 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 76.3712924314 \
			85.4876226368 63.1310306103 85.5178444394 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 76.3712924314 \
			94.5821465745 49.8170570746 94.6157263552 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 76.3712924314 \
			103.676670512 63.1310306103 103.713608271 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 76.3712924314 \
			112.77119445 49.8170570746 112.811490186 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 76.3712924314 \
			121.865718387 63.1310306103 121.909372102 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 76.3712924314 \
			130.960242325 49.8170570746 131.007254018 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
		set i [$Global(CurrentCanvas) create line 76.3712924314 \
			140.054766263 63.1310306103 140.105135933 -arrow none -arrowshape \
			{8 10 3} -capstyle butt -fill black -joinstyle miter -smooth 0 \
			-splinesteps 12 -stipple {} -tags {} -width 1]
	}}
	{Button Print {
		set i [$Global(CurrentCanvas) create polygon 4.0 2.0 29.0 \
			2.0 29.0 2.0 153.0 2.0 153.0 2.0 178.0 2.0 178.0 27.0 178.0 27.0 \
			178.0 141.0 178.0 141.0 178.0 166.0 153.0 166.0 153.0 166.0 29.0 \
			166.0 29.0 166.0 4.0 166.0 4.0 141.0 4.0 141.0 4.0 27.0 4.0 27.0 \
			-fill {} -outline {} -smooth 0 -splinesteps 12 -stipple {} -tags \
			{} -width 1]
		set i [$Global(CurrentCanvas) create polygon 18.112852546 \
			78.1413823784 29.689679783 78.1413823784 29.689679783 \
			78.1413823784 145.509246403 78.1413823784 145.509246403 \
			78.1413823784 157.08607364 78.1413823784 157.08607364 \
			92.3229957436 157.08607364 92.3229957436 157.08607364 \
			106.520317973 157.08607364 106.520317973 157.08607364 \
			120.701931339 145.509246403 120.701931339 145.509246403 \
			120.701931339 29.689679783 120.701931339 29.689679783 \
			120.701931339 18.112852546 120.701931339 18.112852546 \
			106.520317973 18.112852546 106.520317973 18.112852546 \
			92.3229957436 18.112852546 92.3229957436 -fill \
			$Global(ButtonColor) -outline #000000000000 -smooth 0 \
			-splinesteps 12 -stipple $Global(ButtonStipple1) -tags {} -width \
			1]
		set i [$Global(CurrentCanvas) create polygon \
			29.6939543042 121.928678622 34.3263950074 121.928678622 \
			34.3263950074 121.928678622 140.872531179 121.928678622 \
			140.872531179 121.928678622 145.504971882 121.928678622 \
			145.504971882 130.395811007 145.504971882 130.395811007 \
			145.504971882 134.629377198 145.504971882 134.629377198 \
			145.504971882 143.096509583 140.872531179 143.096509583 \
			140.872531179 143.096509583 34.3263950074 143.096509583 \
			34.3263950074 143.096509583 29.6939543042 143.096509583 \
			29.6939543042 134.629377198 29.6939543042 134.629377198 \
			29.6939543042 130.395811007 29.6939543042 130.395811007 -fill \
			$Global(ButtonColor) -outline #000000000000 -smooth 0 \
			-splinesteps 12 -stipple $Global(ButtonStipple2) -tags {} -width \
			1]
		set i [$Global(CurrentCanvas) create polygon \
			37.0084235148 76.6096100677 37.0084235148 76.6096100677 \
			136.511731999 76.6096100677 136.511731999 76.6096100677 \
			136.511731999 54.2857931245 148.949645559 31.9619761813 \
			148.949645559 31.9619761813 49.4463370753 31.9619761813 \
			49.4463370753 31.9619761813 37.0084235148 54.2857931245 \
			37.0084235148 76.6096100677 -fill #FFFFFFFFFFFF -outline \
			#000000000000 -smooth 1 -splinesteps 12 -stipple \
			$Global(ButtonStipple2) -tags {} -width 1]
		set i [$Global(CurrentCanvas) create polygon \
			148.978487615 31.8672316384 142.932094742 42.6870925685 \
			150.25141243 42.6870925685 -fill #FFFFFFFFFFFF -outline black \
			-smooth 0 -splinesteps 12 -stipple $Global(ButtonStipple1) -tags \
			{} -width 1]
	}}
}
 

if {[info exists embed_args] || [info exists env(PLUGIN)]} {
	set Global(Plugin) 1
	set Global(ButtonSize) 25
	set Global(ButtonOrient) left
	set Global(ButtonCount) 10
} else {
	set Global(Plugin) 0
}

proc RotateF { r x y args } {
	set newshape ""
	set count [llength $args]
	set c 0
	set sr [expr sin($r*3.14159265358979323846/180.0)]
	set cr [expr cos($r*3.14159265358979323846/180.0)]
	while {$c < $count} {
		set xc [lindex $args $c] 
		set yc [lindex $args [expr $c + 1]]
		set c [expr $c + 2]
		lappend newshape [expr ($xc - $x) * $cr - ($yc - $y) * $sr + $x] \
			[expr ($xc - $x) * $sr + ($yc - $y) * $cr + $y]
	}
	return $newshape
}
set Global(CornerUL) { 5 5 25 5 25 15 15 15 15 25 5 25 5 5 }
set Global(CornerUR) [eval RotateF 90 20 20 $Global(CornerUL)]
set Global(CornerLR) [eval RotateF 90 20 20 $Global(CornerUR)]
set Global(CornerLL) [eval RotateF 90 20 20 $Global(CornerLR)]

proc setTags {w reqName} {
	global windowtag reqtag CjNames

	if {[info exists CjNames($w)]} {
		set windowtag $CjNames($w)
	}
	set reqtag "$reqName"
}

proc initRequestor {name w class title trans} {
	global reqName

	toplevel $w -class $class
	tk_bindForTraversal $w
	if {$trans != "" && $trans != 0} {
		wm transient $w .
	}
	wm title $w "$title"
	wm iconname $w "$name"
	set reqName "$name"
}
 
proc bindEntry {entry type width} {
	global CjNames CjWindows CjWidth
	
	set name $CjNames($entry)
	set CjWidth($entry) $width
	switch [string tolower $type] {
		hex {
		bind $entry <Key> {
			if {[expr [string length [%W get]] + 1] > $CjWidth(%W) &&\
				"%A" != "{}" && "%A" >= " " && "%A" <= "~" } {
				if {[%W selection present]} {
					%W icursor sel.first
					%W delete sel.first sel.last
				} else {
					break
				}
			}
			set a [string toupper "%A"]
			if { ("%A" < "0" || "%A" > "9") && ("$a" < "A" || "$a" > "F") } {
				if { "%A" != "{}" && "%A" >= " " && "%A" <= "~" } {
					if {[%W selection present]} {
						%W icursor sel.first
						%W delete sel.first sel.last
					} else {
						break
					}
				}
			}
		}
		proc set_$name { val } "
			$entry delete 0 end
			$entry insert 0 \[format \"%0${width}X\" \$val\]
		"
		proc get_$name {} "
			set val \[$entry get\]
			scan \${val} %x tvalue
			return \$tvalue
		"
		}
		unit {
		bind $entry <Key> {
			if {[expr [string length [%W get]] + 1] > $CjWidth(%W) &&\
				"%A" != "{}" && "%A" >= " " && "%A" <= "~" } {
				break
			}
			if { "%A" < "0" || "%A" > "9" } {
				if { "%A" == "." } {
					if {[string first "." "[%W get]"] != -1} {
						break
					}
				} elseif { "%A" == "m" } {
					if {[string first "m" "[%W get]"] != -1} {
						break
					}
				} elseif { "%A" == "c" } {
					if {[string first "c" "[%W get]"] != -1} {
						break
					}
				} elseif { "%A" == "i" } {
					if {[string first "i" "[%W get]"] != -1} {
						break
					}
				} elseif { "%A" == "p" } {
					if {[string first "p" "[%W get]"] != -1} {
						break
					}
				} elseif { "%A" != "{}" && "%A" >= " " && "%A" <= "~" } {
					break
				}
			}
		}
		proc set_$name { val } "
			$entry delete 0 end
			$entry insert 0 \[format \$val]
		"
		proc get_$name {} "
			set val \[$entry get\]
			return \$val
		"
		}
		float {
		bind $entry <Key> {
			if {[expr [string length [%W get]] + 1] > $CjWidth(%W) &&\
				"%A" != "{}" && "%A" >= " " && "%A" <= "~" } {
				break
			}
			if { "%A" < "0" || "%A" > "9" } {
				if { "%A" == "." } {
					if {[string first "." "[%W get]"] != -1} {
						break
					}
				} elseif { "%A" != "{}" && "%A" >= " " && "%A" <= "~" } {
					break
				}
			}
		}
		proc set_$name { val } "
			$entry delete 0 end
			$entry insert 0 \[format \$val]
		"
		proc get_$name {} "
			set val \[$entry get\]
			return \$val
		"
		}
		int {
		bind $entry <Key> {
			if {[expr [string length [%W get]] + 1] > $CjWidth(%W) &&\
				"%A" != "{}" && "%A" >= " " && "%A" <= "~" } {
				break
			}
			if { "%A" < "0" || "%A" > "9" } {
				if { "%A" != "{}" && "%A" >= " " && "%A" <= "~" } {
					break
				}
			}
		}
		proc set_$name { val } "
			$entry delete 0 end
			$entry insert 0 \$val
		"
		proc get_$name {} "
			set val \[$entry get\]
			return \$val
		"
		}
		text -
		default {
		proc set_$name { val } "
			$entry delete 0 end
			$entry insert 0 \$val
		"
		proc get_$name {} "
			set val \[$entry get\]
			return \$val
		"
		}
	}
}

proc createEntry {w name type width args} {
	global CjNames CjWindows
	set e [eval entry $w.${name}_e $args]
	set CjNames($e) $name
	set CjWindows($name) $e
	bindEntry $w.${name}_e $type $width
	return $e
}

proc createScale {w name args} {
	global CjNames CjWindows
	set s [eval scale $w.${name}_sc $args]

	set CjNames($s) $name
	set CjWindows($name) $s
	return $s
}

proc createComboScale {w name label scale entry} {
	global CjNames CjWindows

	frame $w.${name}_f -highlightthickness 0
	frame $w.${name}_f.top -highlightthickness 0
	frame $w.${name}_f.bottom -highlightthickness 0
	set l [label $w.${name}_l -text "$label" -highlightthickness 0]
	raise $scale
	raise $entry
	
	pack $l -in $w.${name}_f.top -anchor sw -side left
	pack $scale -in $w.${name}_f.bottom -anchor nw -side left \
		-fill x -expand true
	pack $entry -in $w.${name}_f.bottom -padx 2m -anchor sw -side right 
	pack $w.${name}_f.top $w.${name}_f.bottom -in $w.${name}_f -side top \
		-padx 2m -fill x -expand true

	set ename $CjNames($entry)
	$scale configure -command set_${ename}
	bind $entry <Key-Return> "
		set_${ename} \[get_${ename}\]
		$scale set \[get_${ename}\]
	"
	bind $entry <FocusOut> "
		if {\[$entry get\] == \"\"} {
			set_${ename} \[$scale get\]
		} else {
			set_${ename} \[get_${ename}\]
			$scale set \[get_${ename}\]
		}
	"
	set CjNames($w.${name}_f) $name
	set CjWindows($name) $w.${name}_f
	return $w.${name}_f
}

proc createButton {w name default args} {
	global CjNames CjWindows 

	set b [eval button $w.${name}_b $args]
	if {"$default" == 1} {
		bind $b <Key-Return> {
			%W invoke
		}
		focus $b
	}

	set CjNames($b) $name
	set CjWindows($name) $b
	return $b
}

proc deleteComboBox {combobox} {
	global CjNames CjWindows

	eval .$CjNames($combobox)_tflb.lb delete 0 end
}
proc insertComboBox {combobox pos args} {
	global CjNames CjWindows

	eval .$CjNames($combobox)_tflb.lb insert $pos $args
}

proc createComboBox {w name label entry button height} {
	global CjNames CjWindows 

	frame $w.${name}_fcb -highlightthickness 0
	frame $w.${name}_fcbe -highlightthickness 0
	frame $w.${name}_dummy -highlightthickness 0
	set d $w.${name}_dummy
	if { "$label" != "" } {
		set l [label $w.${name}_l -text "$label" -highlightthickness 0]
		pack $l -in $w.${name}_fcb -side top -anchor w
	}
	$button configure -command "
		set rhght \[winfo reqheight .${name}_tflb.lb\]
		set rwdth \[expr \[winfo reqwidth .${name}_tflb.lb\] + \
			\[winfo reqwidth $button\]\]
		raise .${name}_tflb
		wm geometry .${name}_tflb \
			\${rwdth}x\${rhght}+\[winfo rootx $d\]\+\[winfo rooty $d\]
		wm deiconify .${name}_tflb
		focus .${name}_tflb.lb
		set CjPriv(background) \[.${name}_tflb.lb cget -selectbackground\]
	"
	pack $entry -in $w.${name}_fcbe -side left -fill x -expand true -anchor w
	pack $button -in $w.${name}_fcbe -side right -fill both -anchor w -padx .05c
	pack $w.${name}_fcbe $d -in $w.${name}_fcb -side top -anchor w
	raise $entry
	raise $button
	
	toplevel .${name}_tflb -bd 1 -relief raised -class ComboBox \
		-highlightthickness 0
	wm overrideredirect .${name}_tflb 1
	
	listbox .${name}_tflb.lb -height $height -width [$entry cget -width] \
		-yscrollcommand ".${name}_tflb.sb set" -relief flat
	scrollbar .${name}_tflb.sb -command ".${name}_tflb.lb yview" -relief ridge
	pack .${name}_tflb.lb -in .${name}_tflb -side left -pady 2 -ipadx 1
	pack .${name}_tflb.sb -in .${name}_tflb -side right -fill y -pady 2 
	wm withdraw .${name}_tflb

	bind .${name}_tflb <Key-Escape> "
		grab release \[grab current\]
		.${name}_tflb.lb configure -selectbackground \$CjPriv(background)
		wm withdraw .${name}_tflb
		if {\[info exists tkPriv(relief)\]} {
			$button configure -relief \$tkPriv(relief)
		}
		$button configure -state normal
		if {\"$w\" != \"\"} {
			focus -lastfor $w
		} else {
			focus [focus -lastfor .]
		}
		set tkPriv(buttonWindow) \"\"
		tkCancelRepeat
		focus [focus -lastfor $w]
	"
	bind .${name}_tflb.lb <Key-Return> "
		set ${name}_e_val [$entry cget -textvariable]
		set current \[.${name}_tflb.lb curselection\]
		if {\"\$current\" != \"\"} {
			set \$${name}_e_val \[.${name}_tflb.lb get \
				\$current\] 
		}
		eval \[bind .${name}_tflb <Key-Escape>\]
	"
	bind .${name}_tflb <ButtonRelease-1> "
		if {\"%W\" != \".${name}_tflb.sb\"} {
			eval \[bind .${name}_tflb <Key-Escape>\]
			focus [focus -lastfor $w]
			break
		}
	"
	bind .${name}_tflb.lb <ButtonRelease-1> "
		eval \[bind .${name}_tflb.lb <Key-Return>\]
		set tkPriv(buttonWindow) \"\"
	"
	bind .${name}_tflb.lb <Leave> "
		if {\$tkPriv(buttonWindow) != \"\"} {
			.${name}_tflb.lb configure -selectbackground \
				\[.${name}_tflb.lb cget -background\]
		}
		focus .${name}_tflb
	"
	bind .${name}_tflb.lb <Enter> "
		.${name}_tflb.lb configure -selectbackground \
			\$CjPriv(background)
	"
	bind .${name}_tflb.lb <Any-Key> "
		.${name}_tflb.lb configure -selectbackground \$CjPriv(background)
	"
	bind $button <Key-Return> "
		tkButtonUp %W
		%W invoke
		focus .${name}_tflb.lb
		grab -global .${name}_tflb
		update
	"
	bind $button <ButtonRelease-1> {
		break
	}
	bind $button <Button-1> {
		tkButtonUp %W
		%W invoke
	}
	bind $button <Leave> "
		if {\[wm state .${name}_tflb\] == \"normal\"} {
			focus .${name}_tflb
			grab -global .${name}_tflb
			update
			break
		}
	"

	set CjNames($w.${name}_fcb) $name
	set CjNames(.${name}_tflb.lb) $name
	set CjNames(.${name}_tflb) $name
	set CjWindows($name) $w.${name}_fcb
	return $w.${name}_fcb
}

proc checkUnit {entry} {
	set value [$entry get]
	if {[regexp {^[.0-9][.0-9]*[ipcm]$} $value]} {
		if {[regexp {^[^.]*[.][^.]*[.].*$} $value]} {
			return 0
		} else {
			return 1
		}
	} else {
		return 0
	}
}

proc labeledEntry { pathname label type args} {
	frame ${pathname}_f
	label ${pathname}_l -text "$label"
	eval entry ${pathname} $args
	pack ${pathname}_l -side left -in ${pathname}_f -fill x
	pack ${pathname} -side right -in ${pathname}_f -fill x
		
	return ${pathname}_f
}

proc textLower {name} {
	global Global

	set w ${name}
	if { $Global(Plugin) } {
		pack forget $w
		pack [winfo parent $Global(TextReplace)] -fill both -expand true
	} else {
		wm withdraw $w
	}
}

proc propLower {name} {
	global Global

	set w ${name}
	if { $Global(Plugin) } {
		pack forget $w
		pack [winfo parent $Global(PropReplace)] -fill both -expand true
	} else {
		wm withdraw $w
	}
}

proc slideLower {name} {
	global Global

	set w ${name}
	if { $Global(Plugin) } {
		pack forget $w
		pack [winfo parent $Global(SlideReplace)] -fill both -expand true
	} else {
		wm withdraw $w
	}
	set Global(colortype) $Global(colortypeSave)
}
		
proc printLower {name} {
	global Global

	set w ${name}
	if { $Global(Plugin) } {
		pack forget $w
		pack [winfo parent $Global(PrintReplace)] -fill both -expand true
	} else {
		wm withdraw $w
	}
}
		
proc gridLower {name} {
	global Global

	set w ${name}
	if { $Global(Plugin) } {
		pack forget $w
		pack [winfo parent $Global(GridReplace)] -fill both -expand true
	} else {
		wm withdraw $w
	}
}
		
proc msgLower {name} {
	global Global

	set w ${name}
	if { $Global(Plugin) } {
		pack forget $w
		pack [winfo parent $Global(MsgReplace)] -fill both -expand true
	} else {
		wm withdraw $w
	}
}
		
proc msgRequestor {name} {
	global Global

	set w ${name}
	if { $Global(Plugin) } {
		frame $w -borderwidth 0
	} else {
		toplevel $w
#		wm transient $w .
		wm protocol $w WM_DELETE_WINDOW {set dummy 1}
	}
	frame $w.f -borderwidth 0 -highlightthickness 0
	frame $w.texttop -borderwidth 0 -highlightthickness 0
	frame $w.textbottom -relief raised -bd 1
	label $w.message
	button $w.ok -text "Dismiss" -command "msgLower $w"
	pack $w.message -in $w.texttop -fill both -expand true
	pack $w.ok -in $w.textbottom -side left -padx 2m -pady 2m
	pack $w.texttop $w.textbottom -anchor sw -in $w -fill x -expand true
	pack $w.f -in $w
	if { !$Global(Plugin) } {
		wm withdraw $w
	}
}
proc msgRaise {name message} {
	global Global

	set w ${name}
	set Global(MsgReplace) $Global(CurrentCanvas)

	if {![winfo exists $name]} {
		msgRequestor $name
	}
	$w.message configure -text "$message"
	if { $Global(Plugin) } {
		pack forget [winfo parent $Global(MsgReplace)]
		pack $w -fill both -expand true
	} else {
		wm deiconify $w
		focus $w
	}
}
proc textRaise {name} {
	global Global

	set w ${name}
	set Global(TextReplace) $Global(CurrentCanvas)

	if {![winfo exists $name]} {
		textRequestor $name
	}
	if { $Global(Plugin) } {
		pack forget [winfo parent $Global(TextReplace)]
		pack $w -fill both -expand true
	} else {
		wm deiconify $w
		raise $w
		focus $w
	}
	$w.text delete 1.0 end
}
proc propDisplay {name item} {
	global Global concmd

	if {[winfo exists $name.p.p]} {
		eval destroy [pack slaves $name.p.p]
	} else {
		frame $name.p.p
	}
	$Global(CurrentCanvas) dtag current
	set props [$Global(CurrentCanvas) itemconfigure $item]
	set type [$Global(CurrentCanvas) type $item]
	set i 0
	set concmd ""
	foreach prop $props {
		set propname [string range [lindex $prop 0] 1 end]
		global var${propname}
		set var${propname} [lindex $prop 4]
		frame $name.p.p.f$i
		label $name.p.p.f$i.l${propname} -text "$propname"
		if {"$type" == "text" && "$propname" == "text"} {
			text $name.p.p.f$i.e${propname} -width 20 -height 2
			$name.p.p.f$i.e${propname} insert 0.0 [lindex $prop 4]
			set textwidget $name.p.p.f$i.e${propname}
		} else {
			entry $name.p.p.f$i.e${propname} -textvariable var${propname} \
				-width 20
		}
		pack $name.p.p.f$i.l${propname} -in $name.p.p.f$i -side left -fill x
		pack $name.p.p.f$i.e${propname} -in $name.p.p.f$i -side right
		pack $name.p.p.f$i -in $name.p.p -side top -fill x -expand true
		incr i
		if {"$type" == "text" && "$propname" == "text"} {
			append concmd " -${propname} \"\[${textwidget} get 0.0 end\]\""
		} else {
			append concmd " -${propname} \"\$var${propname}\""
		}
	}
	button $name.p.p.b -text "Apply" \
		-command "eval \$Global(CurrentCanvas) itemconfigure $item \$concmd;
		"
	pack $name.p.p.b -in $name.p.p
	pack $name.p.p -in $name.p
}
proc propDraw {name sc or items index} {
	global Global

	set w ${name}
	set Global(PropDraw) ""
	eval $w.c delete [$w.c find withtag all]
	set item [lindex $items [expr $index - 1]]
	set props [getProperties $Global(CurrentCanvas) $item Esc 0]
	$w.l configure -text "Object $index of [llength $items]"
	set coords [$Global(CurrentCanvas) coords $item]
	set type [$Global(CurrentCanvas) type $item]
	append Global(PropDraw) \
		"set i \[eval $w.c create $type $coords $props -tags this\]
		eval $w.c scale this 0 0 $sc $sc;"
	if {"$type" == "text"} {
		append Global(PropDraw) \
		"set fn \[$w.c itemcget \$i -font\];
		regexp {(\[-\]\[^-\]*-\[^-\]*-\[^-\]*-\[^-\]*-\[^-\]*-\[^-\]*-\[^-\]*-)(\[^-\]*)(-.*)} \$fn dummy d1 size d2;
		if {\$dummy != \"\"} {
			set nsize \[expr round(\$size * $sc\)];
			if { \$nsize < 20 } {
				set nsize 20
			}
			$w.c itemconfigure \$i -font \${d1}\${nsize}\${d2};
			set fnn \[$w.c itemcget \$i -font\];
			regexp {(\[-\]\[^-\]*-\[^-\]*-\[^-\]*-\[^-\]*-\[^-\]*-\[^-\]*-\[^-\]*-)(\[^-\]*)(-.*)} \$fnn dummy d1 nsize d2;
			if {(\$sc < 1 && \$nsize < \$size) || \
				(\$sc > 1 && \$nsize > \$size)} {
				$w.c itemconfigure \$i -width \[expr \[$w.c itemcget \$i \
					-width\] * $sc\];
			} else {
				$w.c itemconfigure \$i -font \$fn;
			}
		}"
	}
	if {"$type" != ""} {
		eval $Global(PropDraw)
		propDisplay $name $item
	}
}

proc propRaise {name items} {
	global Global
	
	set w ${name}
	set Global(PropReplace) $Global(CurrentCanvas)
	
	if {![winfo exists ${name}]} {
		propRequestor $name 
	}
	set ph [winfo fpixels $Global(CurrentCanvas) $Global(PageHeight)]
	set pw [winfo fpixels $Global(CurrentCanvas) $Global(PageWidth)]
	if {$ph > $pw} {
		set pix [expr 200.0 / $ph]
	} else {
		set pix [expr 200.0 / $pw]
	}
	set ch [expr $pix * $ph]
	set cw [expr $pix * $pw]
	set sc $pix
	
	$w.c configure -width $cw -height $ch
	set or [getOrigin "$items"]
	$w.s configure -to [expr [llength $items]] \
		-command "propDraw $w $sc {$or} {$items}"
	$w.s set 1
	propDraw $w $sc "$or" "$items" 1
	if { $Global(Plugin) } {
		pack forget [winfo parent $Global(PropReplace)]
		pack $w -fill both -expand true
	} else {
		wm deiconify $w
		raise $w
		focus $w
	}
}

proc slideRaise {name} {
	global Global

	set w ${name}

	if {![winfo exists ${name}]} {
		slideRequestor $name
	}
	set Global(F_SlideBG) $w
	if { $Global(Plugin) } {
		set Global(colortypeSave) $Global(colortype)
		set Global(colortype) SlideBG
		pack forget [winfo parent $Global(SlideReplace)]
		pack $w -fill both -expand true
	} else {
		wm deiconify $w
		raise $w
		focus $w
	}
}

proc printRaise {name} {
	global Global

	set w ${name}

	if {![winfo exists ${name}]} {
		printRequestor $name
	}
	if { $Global(Plugin) } {
		pack forget [winfo parent $Global(PrintReplace)]
		pack $w -fill both -expand true
	} else {
		wm deiconify $w
		raise $w
		focus $w
	}
}

proc gridRaise {name} {
	global Global

	set w ${name}

	set Global(GridReplace) $Global(CurrentCanvas)
	if {![winfo exists ${name}]} {
		gridRequestor $name
	}
	if { $Global(Plugin) } {
		pack forget [winfo parent $Global(GridReplace)]
		pack $w -fill both -expand true
	} else {
		wm deiconify $w
		raise $w
		focus $w
	}
}
proc gridCheck {name} {
	global Global
	set w ${name}
	if {$Global(Grid)} {
		$w.gridx configure -state normal
		$w.gridy configure -state normal
		$w.gridx configure -relief sunken -bg white
		$w.gridy configure -relief sunken -bg white
	} else {
		$w.gridx configure -state disabled
		$w.gridy configure -state disabled
		$w.gridx configure -relief flat -bg [$w cget -bg]
		$w.gridy configure -relief flat -bg [$w cget -bg]
	}
}
proc fontSel {fontl} {
	global Global

	set fsel ""
	catch {set fsel [exec tkfontsel.tcl]}
	if { "$fsel" != "" } {
		$fontl insert end $fsel
	}
}
proc textRequestor {name} {
	global Global

	set w ${name}
	if { $Global(Plugin) } {
		frame $w -borderwidth 0
	} else {
		toplevel $w
		wm protocol $w WM_DELETE_WINDOW {set dummy 1}
	}
	frame $w.f -borderwidth 0 -highlightthickness 0
	frame $w.top -width 5i -height 4i -borderwidth 0 -highlightthickness 0
	frame $w.topr
	frame $w.bottom -borderwidth 0 -highlightthickness 0
	
	listbox $w.fonts -relief sunken -height 3 -selectmode single \
		-yscrollcommand "$w.sb set" -relief flat
	scrollbar $w.sb -orient v -command "$w.fonts yview"
	
	foreach font $Global(FontList) {
		$w.fonts insert end $font
	}
	text $w.text -height 1 -width 20

	bind $w.fonts <Button-1> "
		set Global(Font) \[%W get \[%W nearest %y\]\]
		$w.text configure -font \$Global(Font)
	" 
	if { !$Global(Plugin) } {
		button $w.addfont -text "Add Fonts..." -command "fontSel $w.fonts"
	}
	button $w.ok -text "Ok" -command "
		regsub -all {(\[^
\])
(\[^
\])} \[$w.text get 1.0 end\] {\\1 \\2} match
		\$Global(CurrentCanvas) itemconfigure \$Global(Text) \
			-text \$match -font \$Global(Font);textLower $w"

	button $w.cancel -text "Cancel" -command "textLower $w"
	bind $w <Key-Escape> "textLower $w"
	bind $w <Key-Escape> "textLower $w"

	if { !$Global(Plugin) } {
		button $w.open -text "Open" -command "
			if { \[openFile \"\"\ \"\"] != \"\" } {
				set f \[open \$Global(OpenFile) r\]
				set in \[read -nonewline \$f\]
				$w.text insert 0.0 \$in
			}"
	}

	pack $w.sb -in $w.topr -side right -fill y
	pack $w.fonts -in $w.topr -side left -expand true -fill both
	pack $w.topr -in $w.top -expand true -side top -fill both
	pack $w.text -in $w.top -expand true -fill both
	if { !$Global(Plugin) } {
		pack $w.addfont -in $w.bottom -side left -padx 1m -pady 1m
	}
	pack $w.ok $w.cancel -in $w.bottom -side left -padx 1m -pady 1m
	if { !$Global(Plugin) } {
		pack $w.open -in $w.bottom -side right -padx 1m -pady 1m
	}
	pack $w.top -side top -expand true -fill both -in $w.f
	pack $w.bottom -side top -fill x -in $w.f
	pack $w.f -in $w -fill both -expand true

	if { !$Global(Plugin) } {
		wm withdraw $w
	}
	return $w
}

proc propRequestor {name} {
	global Global

	set w ${name}
	
	if { $Global(Plugin) } {
		frame $w -borderwidth 0
	} else {
		toplevel $w
#		wm transient $w .
		wm protocol $w WM_DELETE_WINDOW {set dummy 1}
	}
	frame $w.f -borderwidth 0 -highlightthickness 0
	frame $w.p
	frame $w.fsel -borderwidth 0 -highlightthickness 0
	frame $w.texttop -borderwidth 0 -highlightthickness 0
	frame $w.textbottom -relief raised -bd 1
	canvas $w.c -width 1.5i -height 1.5i -relief sunken -bd 1 -bg white
	label $w.l
	scale $w.s -orient horiz -resolution 1 -from 1 -to 1 \
        -variable Global(PropItem) -bd 2 -relief raised -highlightthickness 0 \
        -width 8 -showvalue true 

	button $w.ok -text "Ok" -command "propLower $w"
	button $w.cancel -text "Cancel" -command "propLower $w"
	bind $w <Key-Escape> "propLower $w"

	pack $w.c -in $w.texttop
	pack $w.l -in $w.texttop -fill x
	pack $w.s -in $w.texttop -fill x
	pack $w.ok $w.cancel -in $w.textbottom -side left -padx 1m -pady 1m
	pack $w.texttop -anchor sw -in $w.fsel 
	pack $w.fsel $w.textbottom -anchor sw -in $w.f -fill x
	pack $w.f $w.p -in $w -anchor sw -side left

	if { !$Global(Plugin) } {
		wm withdraw $w
	}
	return $w
}

proc gridRequestor {name} {
	global Global GridX GridY

	set w ${name}
	
	if { $Global(Plugin) } {
		frame $w -borderwidth 0
	} else {
		toplevel $w
#		wm transient $w .
		wm protocol $w WM_DELETE_WINDOW {set dummy 1}
	}
	frame $w.f -borderwidth 0 -highlightthickness 0
	frame $w.textleft -borderwidth 0 -highlightthickness 0
	frame $w.texttop -borderwidth 0 -highlightthickness 0
	frame $w.gridgravf -bd 1 -relief raised
	frame $w.textbottom -relief raised -bd 1

	set gridB [checkbutton $w.gridb -variable Global(Grid) -text "Grid On"\
		-command "gridCheck $w"]
	set gridX [labeledEntry $w.gridx "Grid X:" unit  \
		-textvariable GridX -width 5 -bg #FF00FF00FF00 ]
	set gridY [labeledEntry $w.gridy "Grid Y:" unit  \
		-textvariable GridY -width 5 -bg #FF00FF00FF00 ]
	gridCheck $w

	label $w.gridgravl -text "Gravity Strength"
	scale $w.gridgrav -variable Global(GravityVal) -from 0 -to 50 \
		-showvalue true -resolution 1 -orient horiz -width 2m

	button $w.ok  -text "Ok" -command \
		"if {\[checkUnit $w.gridx\]} {
			set Global(GridX) \$GridX
			set Global(GridY) \$GridY
			gridLower $w
		} else {
			msgRaise .diagmsg \"Wrong Format: should be number followed by:
    m-(millimeters)
    c-(centimeters)
    i-(inches) or
    p-(points)\"
			if {!$Global(Plugin)} {
				wm deiconify $w
			}
			raise $w
			focus $w
		}"
	button $w.cancel -text "Cancel" -command "gridLower $w"
	bind $w <Key-Escape> "gridLower $w"

	pack $gridB $gridX $gridY -in $w.textleft -side top -padx 1m -pady 1m
	pack $w.gridgravl $w.gridgrav -in $w.gridgravf -side top -fill x
	pack $w.textleft $w.gridgravf -in $w.texttop -side top
	pack $w.ok $w.cancel -in $w.textbottom -side left -padx 1m -pady 1m
	pack $w.texttop $w.textbottom -anchor sw -in $w -fill x -expand true
	pack $w.f -in $w
	set GridX $Global(GridX)
	set GridY $Global(GridY)

	if { !$Global(Plugin) } {
		wm withdraw $w
	}
	return $w
}

proc pagePropOpenLower {name} {
	global Global

	set w ${name}
	if { $Global(Plugin) } {
		pack forget $w
		pack [winfo parent $Global(PagePropReplace)] -fill both -expand true
	} else {
		wm withdraw $w
	}
}

proc pagePropLower {name} {
	global Global

	set w ${name}
	if { $Global(Plugin) } {
		pack forget $w
		pack [winfo parent $Global(PagePropReplace)] -fill both -expand true
	} else {
		wm withdraw $w
	}
	set Global(colortype) $Global(colortypeSave)
}

proc pagePropRaise {name} {
	global Global pageHeight pageWidth

	set w ${name}

	set pageWidth $Global(PageWidth)
	set pageHeight $Global(PageHeight)
	set Global(PagePropReplace) $Global(CurrentCanvas)

	if {![winfo exists ${name}]} {
		pagePropRequestor $name
	}
	set Global(F_SlideBG) $w
	if { $Global(Plugin) } {
		set Global(colortypeSave) $Global(colortype)
		set Global(colortype) SlideBG
		pack forget [winfo parent $Global(PagePropReplace)]
		pack $w -fill both -expand true
	} else {
		wm deiconify $w
		raise $w
		focus $w
	}
}

proc pagePropOpenRaise {name} {
	global Global

	set w ${name}

	set Global(URL) ""
	if {![winfo exists ${name}]} {
		pagePropOpenRequestor $name
	}
	if { $Global(Plugin) } {
		pack forget [winfo parent $Global(PagePropReplace)]
		pack $w -fill both -expand true
	} else {
		wm deiconify $w
		raise $w
		focus $w
	}
}

proc pagePropOpenRequestor {name} {
	global Global

	set w ${name}

	set Global(PagePropReplace) $Global(CurrentCanvas)
	if { $Global(Plugin) } {
		frame $w -borderwidth 0
	} else {
		toplevel $w
#		wm transient $w .
		wm protocol $w WM_DELETE_WINDOW {set dummy 1}
	}
	frame $w.f -borderwidth 0 -highlightthickness 0
	frame $w.textleft -borderwidth 0 -highlightthickness 0
	frame $w.textright -borderwidth 0 -highlightthickness 0
	frame $w.texttop -borderwidth 0 -highlightthickness 0
	frame $w.textbottom -relief raised -bd 1

	set url [labeledEntry $w.url "URL:" unit -textvariable Global(URL) \
		-width 50 -bg #FF00FF00FF00 ]
	set tout [scale $w.tout -from 1000 -to 900000 -resolution 1000 -bd 1 \
		-orient h -showvalue true -variable Global(Timeout) \
		-label "Timeout in milliseconds"]

	button $w.ok  -text "Ok" -command "pagePropOpenLower $w;openURL"
	button $w.cancel -text "Cancel" -command "pagePropOpenLower $w"
	bind $w <Key-Escape> "pagePropOpenLower $w"

	pack $url $tout -in $w.textleft -side top -fill x -padx 1m -pady 1m
	pack $w.textleft -in $w.texttop -side top
	pack $w.ok $w.cancel -in $w.textbottom -side left -padx 1m -pady 1m
	pack $w.texttop $w.textbottom -anchor sw -in $w -fill x -expand true
	pack $w.f -in $w

	if { !$Global(Plugin) } {
		wm withdraw $w
	}
	return $w
}

proc pagePropRequestor {name} {
	global Global

	set w ${name}
	
	if { $Global(Plugin) } {
		frame $w -borderwidth 0
	} else {
		toplevel $w
#		wm transient $w .
		wm protocol $w WM_DELETE_WINDOW {set dummy 1}
	}
	frame $w.f -borderwidth 0 -highlightthickness 0
	frame $w.textleft -borderwidth 0 -highlightthickness 0
	frame $w.textright -borderwidth 0 -highlightthickness 0
	frame $w.texttop -borderwidth 0 -highlightthickness 0
	frame $w.textbottom -relief raised -bd 1

	set landscapeB [checkbutton $w.landscapeb -variable Global(Landscape) \
		-text "Landscape"]
	set pageW [labeledEntry $w.pagewidth "Page Width:" unit  \
		-textvariable pageWidth -width 5 -bg #FF00FF00FF00 ]
	set pageH [labeledEntry $w.pageheight "Page Height:" unit  \
		-textvariable pageHeight -width 5 -bg #FF00FF00FF00 ]

	button $w.ok  -text "Ok" -command \
		"if {\[checkUnit $w.pagewidth\]} {
			set Global(PageWidth) \$pageWidth
			set Global(PageHeight) \$pageHeight
			changePageProp \$Global(PageWidth) \$Global(PageHeight) \
				\$Global(Landscape)
			pagePropLower $w
		} else {
			msgRaise .diagmsg \"Wrong Format: should be number followed by:
    m-(millimeters)
    c-(centimeters)
    i-(inches) or
    p-(points)\"
			if {!$Global(Plugin)} {
				wm deiconify $w
			}
			raise $w
			focus $w
		}"
	button $w.cancel -text "Cancel" -command "pagePropLower $w"
	bind $w <Key-Escape> "pagePropLower $w"

	if {$Global(Plugin)} "
		button $w.new -text {New} -anchor w\
			-command {pagePropLower $w;clearAll}
		button $w.open -text {Open...} -anchor w\
			-command {pagePropLower $w;pagePropOpenRaise .pagePropOpen}
		button $w.save -text {Save As...} -anchor w\
			-command {pagePropLower $w;saveToPlugin _parent}
		frame $w.sbgf
		pack $w.new $w.open $w.save $w.sbgf -in $w.textleft -side top -expand 1 -fill x
	"
	pack $landscapeB $pageW $pageH -in $w.textright -side top -padx 1m -pady 1m
	pack $w.textleft $w.textright -in $w.texttop -side left -expand 1
	pack $w.ok $w.cancel -in $w.textbottom -side left -padx 1m -pady 1m
	pack $w.texttop $w.textbottom -anchor sw -in $w -fill x -expand true
	pack $w.f -in $w
	set pageHeight $Global(PageHeight)
	set pageWidth $Global(PageWidth)

	if { !$Global(Plugin) } {
		wm withdraw $w
	}
	return $w
}

proc slideRequestor {name} {
	global Global

	set w ${name}
	
	set Global(SlideReplace) $Global(CurrentCanvas)
	if { $Global(Plugin) } {
		frame $w -borderwidth 0
	} else {
		toplevel $w
#		wm transient $w .
		wm protocol $w WM_DELETE_WINDOW {set dummy 1}
	}
	frame $w.f -borderwidth 0 -highlightthickness 0
	frame $w.texttop -borderwidth 0 -highlightthickness 0
	frame $w.textbottom -relief raised -bd 1

	frame $w.sbgf
	set slideBG [radiobutton $w.sbg -value SlideBG -text "Background Color" \
		-variable Global(colortype) \
		-pady 1m -anchor sw -indicatoron false -selectcolor ""]
	set slideCirc [checkbutton $w.scirc -text "Circulate" \
		-variable Global(Circulate)]

	bind $slideBG <Double-Button-1> "
		colorPaletteReq .colorpalette \
			{0000 3300 6600 9900 CC00 FF00} \
			{0000 3300 6600 9900 CC00 FF00} \
			{0000 3300 6600 9900 CC00 FF00} \
			.colorsp"
	set slideBGC [frame $w.f_SlideBG -bg $Global(SlideBG) -relief sunken -bd 1 -width 5m -height 5m]

	button $w.dismiss -text "Dismiss" -command "slideLower $w"
	bind $w <Key-Escape> "slideLower $w"

	pack $slideBGC $slideBG -side left -in $w.sbgf -fill x -expand true
	pack $w.sbgf $slideCirc -in $w.texttop -fill both -expand true
	pack $w.dismiss -in $w.textbottom -side left -padx 1m -pady 1m
	pack $w.texttop -in $w -fill x -expand true
	pack $w.textbottom -anchor sw -in $w -fill x -expand true
	pack $w.f -in $w

	if { !$Global(Plugin) } {
		wm withdraw $w
	}
	return $w
}

proc printRequestor {name} {
	global Global

	set w ${name}
	
	set Global(PrintReplace) $Global(CurrentCanvas)
	if { $Global(Plugin) } {
		frame $w -borderwidth 0
	} else {
		toplevel $w
#		wm transient $w .
		wm protocol $w WM_DELETE_WINDOW {set dummy 1}
	}
	frame $w.f -borderwidth 0 -highlightthickness 0
	frame $w.texttop -borderwidth 0 -highlightthickness 0
	frame $w.textbottom -relief raised -bd 1

	
	set e [labeledEntry $w.e "Print Cmd:" unit -width 30 -textvariable Global(PrintCmd)]
	set landscapeB [checkbutton $w.landscapeb -variable Global(Landscape) \
		-text "Landscape"]
	set printToFileB [checkbutton $w.ptf -variable Global(ToFile) \
		-text "Print To File"]
	set printColor [radiobutton $w.pc -variable Global(PSColor) \
		-text "Color" -value {color}]
	set printGrayScale [radiobutton $w.pg -variable Global(PSColor) \
		-text "Gray Scale" -value {gray}]

	button $w.ok -text "Ok" -command "printCanvas \$Global(PrintCmd);printLower $w"
	button $w.cancel -text "Cancel" -command "printLower $w"
	bind $w <Key-Escape> "printLower $w"

	pack $e -in $w.texttop -fill both -expand true
	pack $landscapeB $printToFileB $printColor $printGrayScale -anchor w -in $w.texttop -expand true
	pack $w.ok $w.cancel -in $w.textbottom -side left -padx 1m -pady 1m
	pack $w.texttop -in $w -fill x -expand true
	pack $w.textbottom -anchor sw -in $w -fill x -expand true
	pack $w.f -in $w

	if { !$Global(Plugin) } {
		wm withdraw $w
	}
	return $w
}

proc printCanvas {cmd} {
	global Global

	if {$Global(ToFile)} {
		printToFile
	} else {
		printToStream
	}
}

proc printToFile {} {
	global Global

	set filename [tk_getSaveFile \
		-filetypes {{{Postscript} {.ps .eps}}} \
		-parent .]
	set saveit $Global(CurrentCanvas)
	if {"$filename" != ""} {
		set f [open $filename w]
		for {set page 1} {[winfo exists .can.c${page}]} {incr page} {
			set Global(CurrentCanvas) .can.c${page}
			set out [doPostscript]
			regsub -all {%%Page: [0-9][0-9]* [0-9][0-9]*} $out \
				"%%Page: $page $page" newout
			regsub -all {%%EOF} $newout {%%} newout
			if { ! [info exists f] } {
				msgRaise .diagmsg "ERROR: Cannot open $filename for writing."
				set $Global(CurrentCanvas) $saveit
				return
			} else {
				puts $f $newout
			}
		}
		close $f
		set $Global(CurrentCanvas) $saveit
	}
}

proc printToStream {} {
	global Global

	set saveit $Global(CurrentCanvas)
	if {"$Global(PrintCmd)" != ""} {
		set f [open "|$Global(PrintCmd)" "w"]
		for {set page 1} {[winfo exists .can.c${page}]} {incr page} {
			set Global(CurrentCanvas) .can.c${page}
			set out [doPostscript]
			regsub -all {%%Page: [0-9][0-9]* [0-9][0-9]*} $out \
				"%%Page: $page $page" newout
			regsub -all {%%EOF} $newout {%%} newout
			if { ! [info exists f] } {
				msgRaise .diagmsg "ERROR: Cannot open stream for writing."
				set $Global(CurrentCanvas) $saveit
				return
			} else {
				append newout2 "$newout"
			}
		}
		regsub -all {%%Pages: [0-9][0-9]*} $newout2 \
			"%%Pages: [expr $page - 1]" newout3
		puts $f $newout3
		close $f
		set $Global(CurrentCanvas) $saveit
	}
}

proc saveAsFile {} {
	global Global

	set filename [tk_getSaveFile \
		-filetypes {{{Tcl} {.tcl .tk}} {{ImPress} {.tki}}} \
		-parent .]
	if {"$filename" != ""} {
		set out [saveAll]
		set f [open $filename w]
		if { ! [info exists f] } {
			msgRaise .diagmsg "ERROR: Cannot open $filename for writing."
		} else {
			puts $f "$out"
			close $f
		}
	}
}

proc openfile {cmd filename} {
	global Global Group
	if {"$filename" != ""} {
		if {[file readable "$filename"]} {
			set Global(OpenFile) "$filename"
			if {"$cmd" != ""} {
				. configure -cursor watch
				update idletasks
				eval $cmd "$filename"
				update idletasks
				. configure -cursor ""
				update idletasks
			}
			return $Global(OpenFile)
		} else {
			set filename ""
			msgRaise .diagmsg "ERROR: Cannot open file: $filename"
			return ""
		}
	}
}

proc openFile {cmd type} {
	global Global Group

	if {"$type" == "Tk"} {
		set ftype {{{Tcl} {.tcl .tk}} {{ImPress} {.tki}}} \
	} else {
		set ftype "$type"
	}
	set filename [tk_getOpenFile \
		-filetypes "$ftype" \
		-parent .]
	return [openfile $cmd "$filename"]
}

proc explodeList {menu list parentmenu} {
	global menu_buttons Global

	set menuname [lindex $list 0]
	set menutitle [lindex $list 1]
	set menuopts [lindex $list 2]
	set menucmd [lindex $list 3]
	set newlist [lindex $list 4]
	
	# Menubutton
	if {"${parentmenu}" == "${menu}"} {
        menubutton ${parentmenu}.${menuname}_b -text "$menutitle" \
			-menu ${parentmenu}.${menuname}_b.${menuname} -padx 4m 
		if { "$menuopts" != ""} {
			eval ${parentmenu}.${menuname}_b configure $menuopts
		}
		pack ${parentmenu}.${menuname}_b -side left -fill x
        menu ${parentmenu}.${menuname}_b.${menuname}
		lappend menu_buttons ${parentmenu}.${menuname}_b
		set menuname ${menuname}_b.${menuname}
	# MenuItem
	} elseif {"$newlist" == ""} {
		if {"${menutitle}" == ""} {
			eval ${parentmenu} add separator ${menuopts}
		} else {
			eval ${parentmenu} add command -label \"${menutitle}\" \
				-command \"${menucmd}\" ${menuopts}
		}
	# MenuCascade (subMenu)
	} else {
		eval ${parentmenu} add cascade -label \"$menutitle\" \
			-menu ${parentmenu}.${menuname} ${menuopts}
		menu ${parentmenu}.${menuname}
	}
	set len [llength "$newlist"]
	if {$len} {
		for {set i 0} {$i < $len} {incr i} {
			set l [lindex "$newlist" $i]
			explodeList "$menu" "$l" "${parentmenu}.${menuname}"
		}
	}
	return $menu_buttons
}

proc createMenu {w name menulist} {
	global Global

	frame $w.${name}_frame
	pack $w.${name}_frame -fill x

	set newlist [lindex $menulist 4]
	set len [llength "$newlist"]
	for {set i 0} {$i < $len} {incr i} {
		set l [lindex "$newlist" $i]
		set menu_buttons \
			[explodeList "$w.${name}_frame" "$l" "$w.${name}_frame"]
	}
	bind . <Any-FocusIn> "
		if {(\"%d\" == \"NotifyVirtual\") && (\"%m\" == \"NotifyNormal\")} {
			focus $w.${name}_frame
		}
	"

	return "$w.${name}_frame $menu_buttons"
}

proc handleOrigin { handle } {
	global Global

	set bbox [ $Global(CurrentCanvas) bbox \
		[ $Global(CurrentCanvas) find withtag $handle ] ]
	if { "$bbox" != "" } {
		set x1 [lindex $bbox 0]
		set y1 [lindex $bbox 1]
		set x2 [lindex $bbox 2]
		set y2 [lindex $bbox 3]
		switch "$handle" {
			"top_left_corner" {
				return "$x2 $y2"
			}
			"top_side" {
				return "[expr $x1 + 5] $y2"
			}
			"top_right_corner" {
				return "$x1 $y2"
			}
			"right_side" {
				return "$x1 [expr $y1 + 5]"
			}
			"bottom_right_corner" {
				return "$x1 $y1"
			}
			"bottom_side" {
				return "[expr $x1 + 5] $y1"
			}
			"bottom_left_corner" {
				return "$x2 $y1"
			}
			"left_side" {
				return "$x2 [expr $y1 + 5]"
			}
		}
	}
}

proc disableButton { name } {
	global Global

	$name create rectangle 0 0 $Global(ButtonSize) $Global(ButtonSize) \
		-fill $Global(Background) -stipple gray50 -outline {} -tags disabled
}
proc enableButton { name } {
	$name delete disabled
}

proc disablePaste {} {
	global Global

	if {$Global(Cut) == ""} {
		disableButton .buttons.b_Paste
	}
}

proc enablePaste {} {
	.buttons.b_Paste delete disabled
}

proc enableSelectionButtons {} {
	enableButton .buttons.b_Rotate
	enableButton .buttons.b_Cut
	enableButton .buttons.b_Duplicate
	enableButton .buttons.b_Raise
	enableButton .buttons.b_Lower
	enableButton .buttons.b_Group
	enableButton .buttons.b_UnGroup
	enableButton .buttons.b_Flip_Vert
	enableButton .buttons.b_Flip_Horiz
}

proc disableSelectionButtons {} {
	disableButton .buttons.b_Rotate
	disableButton .buttons.b_Cut
	disableButton .buttons.b_Duplicate
	disableButton .buttons.b_Raise
	disableButton .buttons.b_Lower
	disableButton .buttons.b_Group
	disableButton .buttons.b_UnGroup
	disableButton .buttons.b_Flip_Vert
	disableButton .buttons.b_Flip_Horiz
}

proc setCursor { value } {
	global Global

	set Global(Cursor) $value
	setRadioButton Global(Cursor) $value
}

proc handleBindings {} {
	global Global

	$Global(CurrentCanvas) bind top_side <Enter> {
		set Global(CursorSave) $Global(Cursor)
		setCursor Scale
		%W configure -cursor top_side
		set Global(Message) Scale
	}
	$Global(CurrentCanvas) bind top_side <Leave> {
		setCursor $Global(CursorSave)
		%W configure -cursor ""
		set Global(Message) $Global(CursorSave)
	} 
	$Global(CurrentCanvas) bind right_side <Enter> {
		set Global(CursorSave) $Global(Cursor)
		setCursor Scale
		%W configure -cursor right_side
		set Global(Message) Scale
	}
	$Global(CurrentCanvas) bind right_side <Leave> {
		setCursor $Global(CursorSave)
		%W configure -cursor ""
		set Global(Message) $Global(CursorSave)
	}
	$Global(CurrentCanvas) bind left_side <Enter> {
		set Global(CursorSave) $Global(Cursor)
		setCursor Scale
		%W configure -cursor left_side
		set Global(Message) Scale
	}
	$Global(CurrentCanvas) bind left_side <Leave> {
		setCursor $Global(CursorSave)
		%W configure -cursor ""
		set Global(Message) $Global(CursorSave)
	}
	$Global(CurrentCanvas) bind bottom_side <Enter> {
		set Global(CursorSave) $Global(Cursor)
		setCursor Scale
		%W configure -cursor bottom_side
		set Global(Message) Scale
	}
	$Global(CurrentCanvas) bind bottom_side <Leave> {
		setCursor $Global(CursorSave)
		%W configure -cursor ""
		set Global(Message) $Global(CursorSave)
	}
	$Global(CurrentCanvas) bind top_left_corner <Enter> {
		set Global(CursorSave) $Global(Cursor)
		setCursor Scale
		%W configure -cursor top_left_corner
		set Global(Message) Scale
	}
	$Global(CurrentCanvas) bind top_left_corner <Leave> {
		setCursor $Global(CursorSave)
		%W configure -cursor ""
		set Global(Message) $Global(CursorSave)
	}
	$Global(CurrentCanvas) bind top_right_corner <Enter> {
		set Global(CursorSave) $Global(Cursor)
		setCursor Scale
		%W configure -cursor top_right_corner
		set Global(Message) Scale
	}
	$Global(CurrentCanvas) bind top_right_corner <Leave> {
		setCursor $Global(CursorSave)
		%W configure -cursor ""
		set Global(Message) $Global(CursorSave)
	}
	$Global(CurrentCanvas) bind bottom_right_corner <Enter> {
		set Global(CursorSave) $Global(Cursor)
		setCursor Scale
		%W configure -cursor bottom_right_corner
		set Global(Message) Scale
	}
	$Global(CurrentCanvas) bind bottom_right_corner <Leave> {
		setCursor $Global(CursorSave)
		%W configure -cursor ""
		set Global(Message) $Global(CursorSave)
	}
	$Global(CurrentCanvas) bind bottom_left_corner <Enter> {
		set Global(CursorSave) $Global(Cursor)
		setCursor Scale
		%W configure -cursor bottom_left_corner
		set Global(Message) Scale
	}
	$Global(CurrentCanvas) bind bottom_left_corner <Leave> {
		setCursor $Global(CursorSave)
		%W configure -cursor ""
		set Global(Message) $Global(CursorSave)
	}
}

proc unHandleBindings {} {
	$Global(CurrentCanvas) bind top_side <Enter> {}
	$Global(CurrentCanvas) bind top_side <Leave> {} 
	$Global(CurrentCanvas) bind right_side <Enter> {}
	$Global(CurrentCanvas) bind right_side <Leave> {}
	$Global(CurrentCanvas) bind left_side <Enter> {}
	$Global(CurrentCanvas) bind left_side <Leave> {}
	$Global(CurrentCanvas) bind bottom_side <Enter> {}
	$Global(CurrentCanvas) bind bottom_side <Leave> {}
	$Global(CurrentCanvas) bind top_left_corner <Enter> {}
	$Global(CurrentCanvas) bind top_left_corner <Leave> {}
	$Global(CurrentCanvas) bind top_right_corner <Enter> {}
	$Global(CurrentCanvas) bind top_right_corner <Leave> {}
	$Global(CurrentCanvas) bind bottom_right_corner <Enter> {}
	$Global(CurrentCanvas) bind bottom_right_corner <Leave> {}
	$Global(CurrentCanvas) bind bottom_left_corner <Enter> {}
	$Global(CurrentCanvas) bind bottom_left_corner <Leave> {}
}

proc bboxToCorners { coords } {
	return "[lindex $coords 0]  [lindex $coords 1]  [lindex $coords 2]  [lindex $coords 1]  [lindex $coords 2]  [lindex $coords 3]  [lindex $coords 0]  [lindex $coords 3]"
}

proc createHandles {} {
	global Global

	$Global(CurrentCanvas) delete handles
	set bb [$Global(CurrentCanvas) bbox Imp_Selected]
	if {"$bb" != ""} {
	set bboxcoords [bboxToCorners $bb]
	set x2_1 [lindex $bboxcoords 0]
	set y2_1 [lindex $bboxcoords 1] 
	set x1_1 [eval expr $x2_1 - 10]
	set y1_1 [eval expr $y2_1 - 10]
	set tlc [eval $Global(CurrentCanvas) create polygon \
		[bboxToCorners "$x1_1 $y1_1 $x2_1 $y2_1"] \
		-tags \"selectorbox handles disabled Imp_Selected top_left_corner\" \
		-stipple gray50 \
		-outline white -fill black]
	set x2_2 [eval expr ([lindex $bboxcoords 2] - $x2_1)/2 + $x2_1 + 5]
	set y2_2 $y2_1
	set x1_2 [eval expr $x2_2 - 10]
	set y1_2 [eval expr $y2_2 - 10]
	set ts [eval $Global(CurrentCanvas) create polygon \
		[bboxToCorners "$x1_2 $y1_2 $x2_2 $y2_2"] \
		-tags \"selectorbox handles disabled Imp_Selected top_side\" \
		-stipple gray50 \
		-outline white -fill black]
	set x2_3 [eval expr [lindex $bboxcoords 2] + 10]
	set y2_3 $y2_1
	set x1_3 [eval expr $x2_3 - 10]
	set y1_3 [eval expr $y2_3 - 10]
	set trc [eval $Global(CurrentCanvas) create polygon \
		[bboxToCorners "$x1_3 $y1_3 $x2_3 $y2_3"] \
		-tags \"selectorbox handles disabled Imp_Selected top_right_corner\" \
		-stipple gray50 \
		-outline white -fill black]
	set x2_4 [eval expr $x1_3 + 10]
	set y2_4 [eval expr ([lindex $bboxcoords 5] - $y2_3)/2 + $y2_3 + 5]
	set x1_4 $x1_3
	set y1_4 [eval expr $y2_4 - 10]
	set rs [eval $Global(CurrentCanvas) create polygon \
		[bboxToCorners "$x1_4 $y1_4 $x2_4 $y2_4"] \
		-tags \"selectorbox handles disabled Imp_Selected right_side\" \
		-stipple gray50 \
		-outline white -fill black]
	set x1_5 [lindex $bboxcoords 4]
	set y1_5 [lindex $bboxcoords 5]
	set x2_5 [eval expr $x1_5 + 10]
	set y2_5 [eval expr $y1_5 + 10]
	set brc [eval $Global(CurrentCanvas) create polygon \
		[bboxToCorners "$x1_5 $y1_5 $x2_5 $y2_5"] \
		-tags \"selectorbox handles disabled Imp_Selected bottom_right_corner\" \
		-stipple gray50 \
		-outline white -fill black]
	set x2_6 $x1_2
	set y2_6 [eval expr $y1_5 + 10]
	set x1_6 [eval expr $x2_6 + 10]
	set y1_6 $y1_5
	set bs [eval $Global(CurrentCanvas) create polygon \
		[bboxToCorners "$x1_6 $y1_6 $x2_6 $y2_6"] \
		-tags \"selectorbox handles disabled Imp_Selected bottom_side\" \
		-stipple gray50 \
		-outline white -fill black]
	set x1_7 $x1_1
	set y1_7 $y1_6
	set x2_7 [eval expr $x1_7 + 10]
	set y2_7 [eval expr $y1_7 + 10]
	set blc [eval $Global(CurrentCanvas) create polygon \
		[bboxToCorners "$x1_7 $y1_7 $x2_7 $y2_7"] \
		-tags \"selectorbox handles disabled Imp_Selected bottom_left_corner\" \
		-stipple gray50 \
		-outline white -fill black]
	set x1_8 $x1_1
	set y1_8 $y1_4
	set x2_8 [eval expr $x1_8 + 10]
	set y2_8 [eval expr $y1_8 + 10]
	set ls [eval $Global(CurrentCanvas) create polygon \
		[bboxToCorners "$x1_8 $y1_8 $x2_8 $y2_8"] \
		-tags \"selectorbox handles disabled Imp_Selected left_side\" \
		-stipple gray50 \
		-outline white -fill black]
	handleBindings 
	}
}

proc doRuler { ruler len th m1 m2 step orient mark_list mastertemplate} {
# ruler is a canvase for the ruler to reside in.
# m1 (margin left, top)
# m2 (margin right, bottom) define margin sizes on each end of the ruler.
# len  is the length of the ruler.
# th is the thickness (width) of the ruler.
# step  numerical entry at every increment of step units (e.g. 1 for every 1i).
# orient h=horizontal, v=vertical.
# mark_list specifies the breakdown of ruler (max 4).
#   (e.g. {.0625, .125, .25, .5})
# mastertemplate specifies (optional) a list of values indicating the length
#   of the hash mark at each breaddown in mark_list (percentage of th).
#   defaults to $th * .1, $th * .2, $th * .5

	regexp {(.*)(.)$} "$len" match len unit
	regexp {(.*).$} "$th" match th

	if {"$mark_list" == ""} {
		switch "$unit" {
			i {
				set mark_list {.0625 .125 .25 .5}
			}
			c { 
				set mark_list {.1 .5 1}
			}
			m {
				set mark_list {5 20}
			}
			p {
				set mark_list {50 100 200}
			}
		}
	}

	# Clear ruler
	eval $ruler delete all

	# Put down the base of ruler for size.

	if {$mastertemplate == ""} {
		# define lengths for divisions in units
		set mastertemplate \
			"[expr $th * .2] [expr $th * .3] [expr $th * .5] [expr $th * .8]"
	}
	set template [lrange $mastertemplate [expr 4 - [llength $mark_list]] end]
	set largest [lindex $mastertemplate 3]

	set ti 0
	foreach b $mark_list {
		if {"$b" == ""} {
			incr ti
			continue
		}
		set mlength [lindex $template $ti]
		for {set i 0} {$i <= $len} {set i [expr $i + $b]} {
			if {"$orient" == "h"} {
				$ruler create line ${i}${unit} [expr $th - ${mlength}]i \
					${i}${unit} ${th}i
				if {$i == [expr int($i)]} {
					$ruler create text ${i}${unit} [expr $th - ${largest} - \
						.05]i \
						-anchor ne -text [expr int($i)]
				}
			} else {
				$ruler create line [expr $th - ${mlength}]i ${i}${unit} ${th}i \
					${i}${unit}
				if {$i == [expr int($i)]} {
					$ruler create text [expr $th - ${largest} - \
						.05]i ${i}${unit} \
						-anchor sw -text [expr int($i)]
				}
			}
		}
		incr ti
	}
	if {$orient == "h"} {
    	$ruler configure -scrollregion \
			"${m1}${unit} 0i [expr ${len} - ${m2}]${unit} ${th}i"
		$ruler configure -xscrollincrement 1
		$ruler create line 0 ${th}i 0 0 -tags h -fill red
	} else {
    	$ruler configure -scrollregion \
			"0i ${m1}${unit} ${th}i [expr ${len} - ${m2}]${unit}"
		$ruler configure -yscrollincrement 1
		$ruler create line 0 0 ${th}i 0 -tags v -fill red
	}

}

proc trans { x y shape } {
	set newshape ""
	set count [llength $shape]
	set c 0
	while {$c < $count} {
		set xc [lindex $shape $c]
		set yc [lindex $shape [expr $c + 1]]
		set c [expr $c + 2]
		lappend newshape [expr $xc + $x] [expr $yc + $y]
	}
	return $newshape
}

proc doPostscript {args} {
	global Global

	deselectAll 
	if { "$args" == "" } {
		set ps [$Global(CurrentCanvas) postscript -x -0i -y -0i\
			-width $Global(PageWidth) -colormode $Global(PSColor) \
			-height $Global(PageHeight) -rotate $Global(Landscape)]
	} else {
		set ps [$Global(CurrentCanvas) postscript -channel $args -x -0i -y -0i\
			-width $Global(PageWidth) -colormode $Global(PSColor) \
			-height $Global(PageHeight) -rotate $Global(Landscape)]
	}
	return $ps
}

proc display {topname args} {
	catch {
		set f [browser::openStream $topname application/postscript]
		set out [doPostscript]
		browser::writeToStream $f $out
		browser::closeStream $f
	}
}

proc saveToPlugin {topname} {
	global Global

	if {[catch {policy javascript} errMsg]} {
		$Global(CurrentCanvas) create text 20 100 -text "$errMsg
Could not activate javascript policy.
Check tclplug config/plugin.cfg." \
			-anchor sw -tags Imp_Text -fill red \
			-font {-adobe-helvetica-bold-r-normal--*-240-72-72-p-0-iso8859-1}
		update
	} else {
		catch {
			set f [browser::openStream $topname application/impress]
			set out [saveAll]
			browser::writeToStream $f $out
			browser::closeStream $f
		}
	}
}

proc printToPlugin {topname} {
	global Global

	set saveit $Global(CurrentCanvas)
	if {"$Global(PrintCmd)" != ""} {
		catch {
			set f [browser::openStream $topname application/postscript]
			for {set page 1} {[winfo exists .can.c${page}]} {incr page} {
				set Global(CurrentCanvas) .can.c${page}
				set out [doPostscript]
				regsub -all {%%Page: [0-9][0-9]* [0-9][0-9]*} $out \
					"%%Page: $page $page" newout
				regsub -all {%%EOF} $newout {%%} newout
				if { ! [info exists f] } {
					msgRaise .diagmsg "ERROR: Cannot open stream for writing."
					set $Global(CurrentCanvas) $saveit
					return
				} else {
					browser::writeToStream $f $newout
				}
			}
			browser::closeStream $f
		}
		set $Global(CurrentCanvas) $saveit
	}
}

proc newOne {url mimetype lastmodified size} {
	global Global

	set Global(MessageSave) $Global(Message)
	set Global(Total) $size
	set Global(TotalSize) 0
	set Global(Message) "Loading: $Global(Total) of $Global(TotalSize)"
	update idletasks
}
proc writeOne {size data} {
	global Global

	set Global(TotalSize) [expr $Global(TotalSize) + $size]
	set Global(Message) "Loading: $Global(Total) of $Global(TotalSize)"
	update idletasks
}
proc openURL {} {
	global Global
	
	if {"$Global(URL)" != ""} {
		if {[catch {policy javascript} errMsg]} {
			$Global(CurrentCanvas) create text 20 100 -text "$errMsg
Could not activate javascript policy.
Check tclplug config/plugin.cfg." \
				-anchor sw -tags Imp_Text -fill red \
				-font {-adobe-helvetica-bold-r-normal--*-240-72-72-p-0-iso8859-1}
			update
		} else {
			if {![regexp {[^/:][^/:]*:/.*} $Global(URL) m]} {
				set Global(URL) "[getattr originHomeDirURL]$Global(URL)"
			}
			. configure -cursor watch
			update idletasks
			set Global(MessageSave) $Global(Message)
			catch {
				set url_in [browser::getURL $Global(URL) $Global(Timeout) newOne writeOne]
				set t 0
			} errcode
			if {$errcode != 0} {
				. configure -cursor {}
				set Global(Message) "URL error: $errcode"
				update idletasks
			} else {
				set Global(Message) "Rendering"
				update idletasks
				after idle "
					$url_in
					. configure -cursor {}
					set Global(Message) $Global(MessageSave)
					update idletasks"
			}
	   	}
	}
}

proc printPlugin {} {
	global Global

	if {[catch {policy javascript} errMsg]} {
		$Global(CurrentCanvas) create text 20 100 -text "$errMsg
Could not activate javascript policy.
Check tclplug config/plugin.cfg." \
			-anchor sw -tags Imp_Text -fill red \
			-font {-adobe-helvetica-bold-r-normal--*-240-72-72-p-0-iso8859-1}
		update
	} else {
		printToPlugin "_parent"
	}
}
proc popupCanvasSet { type } {
	global Global

	setCursor "$type"
	switch "$Global(Cursor)" {
		Rotate {
			if {[$Global(CurrentCanvas) find withtag selectorbox] != ""} {
				set Global(Message) Rotate
				$Global(CurrentCanvas) delete selectorbox
				$Global(CurrentCanvas) configure -closeenough 0.0
				eval $Global(CurrentCanvas) create polygon \
					[bboxToCorners [$Global(CurrentCanvas) bbox Imp_Selected]] \
					-tags selectorbox -stipple gray25 -outline \"\" -fill black
			} else {
				setCursor Select
				set Global(Message) Select
				enableSelectionButtons
			}
		}
		Group {
			set Global(Message) Group
			setGroup "[$Global(CurrentCanvas) find withtag Imp_selected]"
			setCursor "Select"
			set Global(Message) Select
			enableSelectionButtons
		}
		UnGroup {
			set Global(Message) UnGroup
			unGroup "[$Global(CurrentCanvas) find withtag Imp_selected]"
			setCursor "Select"
			set Global(Message) Select
			enableSelectionButtons
			focus $Global(CurrentCanvas)
		}
		Grid {
			set Global(Message) Grid
			gridRaise .grid
		}
		PageProp {
			set Global(Message) PageProp
			pagePropRaise .pageProp
			deselectAll
			setCursor "Select"
			set Global(Message) $Global(Cursor)
			enableSelectionButtons
		}
		Raise {
			set Global(Message) Raise
			$Global(CurrentCanvas) raise Imp_selected
			$Global(CurrentCanvas) raise selectorbox
			setCursor "Select"
			set Global(Message) Select
			enableSelectionButtons
		}
		Lower {
			set Global(Message) Lower
			$Global(CurrentCanvas) lower Imp_selected
			catch "$Global(CurrentCanvas) raise Imp_selected Disabled"
			$Global(CurrentCanvas) raise selectorbox
			setCursor "Select"
			set Global(Message) Select
			enableSelectionButtons
		}
		Rulers {
			if { [$Global(CurrentCanvas) find withtag $Global(RulerX)] == "" } {
				set Global(RulerX) [$Global(CurrentCanvas) create window \
					0i .25i -anchor sw \
					-window $Global(CurrentCanvas).rulerx -tags {rulerx disabled}]
				set Global(RulerY) [$Global(CurrentCanvas) create window \
					0i 0i -anchor nw \
					-window $Global(CurrentCanvas).rulery -tags {rulery disabled}]
			}
			set Global(Message) Rulers
			$Global(CurrentCanvas) coords rulerx 0i \
				[expr [lindex [$Global(CurrentCanvas) yview] 0] * \
				[winfo fpixels $Global(CurrentCanvas) $Global(PageHeight)] + \
				[winfo fpixels $Global(CurrentCanvas) .25i] - 2]
			$Global(CurrentCanvas) coords rulery \
				[expr [lindex [$Global(CurrentCanvas) xview] 0] * \
				[winfo fpixels $Global(CurrentCanvas) $Global(PageWidth)] - 2] \
				0i
			setCursor "Select"
			set Global(Message) Select
			enableSelectionButtons
		}
		Cut {
			set Global(Message) Cut
			cutCopySelected Cut Imp_Selected 1
			setCursor "Select"
			set Global(Message) Select
			enableSelectionButtons
		}
		Paste {
			if {$Global(Cut) == ""} {
				setCursor "Select"
				set Global(Message) Select
				enableSelectionButtons
			}
		}
		Duplicate {
			set Global(Message) Duplicate
			duplicateSelected
			createHandles
			setCursor "Select"
			set Global(Message) Select
			enableSelectionButtons
		}
		Flip_Horiz {
			flip Imp_Selected h
			setCursor "Select"
			set Global(Message) Select
			enableSelectionButtons
		}
		Flip_Vert {
			flip Imp_Selected v
			setCursor "Select"
			set Global(Message) Select
			enableSelectionButtons
		}
		Smooth {
			if {$Global(Smooth)} {
				set Global(Msg_Smooth) "Switch to Smooth Lines"
			} else {
				set Global(Msg_Smooth) "Switch to Straight Lines"
			}
			set Global(Message) $Global(Msg_Smooth)
			set Global(Smooth) [expr ! $Global(Smooth)]
			.buttons.b_Smooth itemconfigure $Global(Smooth_Line) \
				-smooth $Global(Smooth)
			setCursor Select
		}
		Line -
		Oval -
		Rectangle -
		Rounded_Rectangle -
		Polygon -
		Text -
		Draw {
			set Global(Sub-Cursor) ""
			$Global(CurrentCanvas) delete Global(curline)
			$Global(CurrentCanvas) delete selector
			set Global(Message) $Global(Cursor)
			deselectAll
			disableSelectionButtons
		}
		Select {
			set Global(Message) $Global(Cursor)
			deselectAll
			enableSelectionButtons
		}
		Printt {
			.text delete 0.0 end
			set out [saveAll]
			.text insert 0.0 $out
			pack forget [winfo parent $Global(CurrentCanvas)]
			pack .textframe -fill both -expand true
			focus .text
		}
	
		Print {
			if {$Global(Plugin)} {
				printPlugin
			} else {
				printRaise .print
			}
			return
		}
	}
	focus $Global(CurrentCanvas)
}
proc setGroup { items } {
	global Group GroupNo Global

	if { "$items" != "" } {
		set itemList [getGroupItems "$items"] 
		foreach item $itemList {
			if {![info exists done($item)]} {
				$Global(CurrentCanvas) addtag Group${GroupNo} withtag $item
				if {[info exists Group($Global(CurrentCanvas),$item)]} {
					set Group($Global(CurrentCanvas),$item) "Group${GroupNo} $Group($Global(CurrentCanvas),$item)"
				} else {
					set Group($Global(CurrentCanvas),$item) Group${GroupNo}
				}
				set done($item) 1
			}
		}
		set GroupNo [clock clicks]
	}
}
proc unGroup { items } {
	global Group PrevGroup Global

	if { "$items" != "" } {
		. configure -cursor watch
		update idletasks
		#		set itemList [getGroupItems "$items"]
		set itemList "$items"
		foreach item $itemList {
			if {![info exists done($item)]} {
				if {[info exists Group($Global(CurrentCanvas),$item)]} {
					$Global(CurrentCanvas) dtag $Group($Global(CurrentCanvas),$item)
					if {[lrange $Group($Global(CurrentCanvas),$item) 1 end] != ""} {
						set Group($Global(CurrentCanvas),$item) [lrange $Group($Global(CurrentCanvas),$item) 1 end]
					} else {
						unset Group($Global(CurrentCanvas),$item)
					}
				}
				set done($item) 1
			}
		}
		deselectAll 
		update idletasks
		. configure -cursor ""
		update idletasks
	}
}
proc repositionWindow { X Y } {
	global Global

	set contain [winfo containing $X $Y]
	set parent ""
	catch {set parent [winfo parent $contain]}

	switch "$contain" {
		.can.scrolly {
			set Global(ScrollDir) ".can.scrollx h 1"
			eval tkScrollByUnits $Global(ScrollDir)
		}
		.can.scrollx {
			set Global(ScrollDir) ".can.scrolly v 1"
			eval tkScrollByUnits $Global(ScrollDir)
		}
		.msg {
			set Global(ScrollDir) ".can.scrolly v -1"
			eval tkScrollByUnits $Global(ScrollDir)
		}
		default {
			if {"$parent" == "." && "$contain" != ".can"} {
				set Global(ScrollDir) ".can.scrolly v -1"
			}
			if {"$parent" == ".can" || \
				"$parent" == $Global(CurrentCanvas)} {
				set Global(ScrollDir) ""
			} elseif {$Global(ScrollDir) == ""} {
				set Global(ScrollDir) ".can.scrollx h -1" 
				eval tkScrollByUnits $Global(ScrollDir)
			} else {
				eval tkScrollByUnits $Global(ScrollDir)
			}
		}
	}
}
proc scaleSelected { handle1 x y X Y} {
	global Global

	repositionWindow $X $Y
	set nofontscale 0
	if { "$Global(LastSX)" == ""} {
		set last [handleOrigin $handle1]
		set Global(LastSX) [lindex "$last" 0]
		set Global(LastSY) [lindex "$last" 1]
		set origin \
			[handleOrigin $Global($handle1)]
		set Global(originx) [lindex "$origin" 0]
		set Global(originy) [lindex "$origin" 1]
	}
	if { "$handle1" == "top_side" || "$handle1" == "bottom_side" } {
		set scalex 1
		set nofontscale 1
	} else {
		set e [expr $Global(originx) - $Global(LastSX)]
		if {$e} {
			set scalex [expr ($Global(originx) - $x) / \
				$e]
		}
	}
	if { "$handle1" == "right_side" || "$handle1" == "left_side" } {
		set scaley 1
		set nofontscale 1
	} else {
		set e [expr $Global(originy) - $Global(LastSY)]
		if {$e} {
			set scaley [expr ($Global(originy) - $y) / \
				$e]
			if { "$handle1" != "top_side" && "$handle1" != "bottom_side" \
				&& $Global(FixedAspect) != ""} {
				set scalex $scaley
			}
		}
	}
	set next {set Global(LastSX) $x;set Global(LastSY) $y}
	if {$scalex > 0 && $scaley > 0} {
		$Global(CurrentCanvas) scale Imp_Selected \
			$Global(originx) $Global(originy) $scalex $scaley
		eval $next
		foreach i [$Global(CurrentCanvas) find withtag Imp_SelectedText] {
			if {$Global(FontScale) && !$nofontscale} {
				set fn [$Global(CurrentCanvas) itemcget $i -font]
				regexp \
				{([-][^-]*-[^-]*-[^-]*-[^-]*-[^-]*-[^-]*-[^-]*-)([^-]*)(-.*)}\
					$fn dummy d1 size d2;
				if {"$dummy" != ""} {
					set nsize [expr round($size * $scalex)]
					if {$nsize < 20} {
						set nsize 20
					}
					$Global(CurrentCanvas) itemconfigure $i \
						-font ${d1}${nsize}${d2}
					set fnn [$Global(CurrentCanvas) itemcget $i -font]
					regexp \
				{([-][^-]*-[^-]*-[^-]*-[^-]*-[^-]*-[^-]*-[^-]*-)([^-]*)(-.*)}\
						$fnn dummy d1 nsize d2;
					if { ($scalex < 1 && $nsize < $size ) || \
		 				($scalex > 1 && $nsize > $size) } {
						$Global(CurrentCanvas) itemconfigure $i \
							-width [expr [$Global(CurrentCanvas) itemcget $i \
							-width] * $scalex]
					} else {
						$Global(CurrentCanvas) itemconfigure $i \
							-font $fn
					}
				}
			} else {
				$Global(CurrentCanvas) itemconfigure $i \
					-width [expr [$Global(CurrentCanvas) itemcget $i \
					-width] * $scalex]
			}
		}
		$Global(CurrentCanvas) delete handles
		createHandles
	}
}

proc deselectAll {} {
	global Global

	$Global(CurrentCanvas) delete selector
	$Global(CurrentCanvas) delete selectorbox
	$Global(CurrentCanvas) dtag Imp_selected
	$Global(CurrentCanvas) dtag Imp_Selected
	$Global(CurrentCanvas) dtag Imp_SelectedText
	$Global(CurrentCanvas) dtag Imp_SelectedPhoto
	$Global(CurrentCanvas) dtag currentImp_Selected
}

proc markSpot {x y} {
	global Global

	if {$Global(Grid)} {
		set Global(LastX) [$Global(CurrentCanvas) canvasx $x \
			$Global(GridX)]
	} else {
		set Global(LastX) [$Global(CurrentCanvas) canvasx $x \
			1]
	}
	if {$Global(Grid)} {
		set Global(LastY) [$Global(CurrentCanvas) canvasy $y \
			$Global(GridY)]
	} else {
		set Global(LastY) [$Global(CurrentCanvas) canvasy $y \
			1]
	}
	set Global(LastSX) ""
	set Global(LastSY) ""
	set Global(LastRotX) $Global(LastX)
	set Global(LastRotY) $Global(LastY)
	set Global(LastAngle) 0.0
}

proc findItemUnderCursor {} {
	global Global

	return [$Global(CurrentCanvas) find withtag current]
}

proc shadeCurrentSelected {} {
	global Global

	set bb [$Global(CurrentCanvas) bbox currentImp_Selected]
	if {"$bb" != "" && \
		[$Global(CurrentCanvas) type currentImp_Selected] != "text"} {
		eval $Global(CurrentCanvas) create polygon [bboxToCorners $bb] \
			-tags {"selectorbox Imp_Selected"} \
			-stipple gray25 -outline {""} -fill black
	}
}

proc selectAll {} {
	global Global

	popupCanvasSet Select
	. configure -cursor watch
	update idletasks
	set itemList [eval $Global(CurrentCanvas) find withtag all]
	if {[llength $itemList] > 500} {
		$Global(CurrentCanvas) addtag Imp_selected withtag all
		$Global(CurrentCanvas) addtag Imp_Selected withtag all
		$Global(CurrentCanvas) addtag currentImp_Selected withtag all
		$Global(CurrentCanvas) addtag Imp_SelectedText withtag Imp_Text 
		$Global(CurrentCanvas) addtag Imp_SelectedPhoto withtag Imp_Photo
		$Global(CurrentCanvas) dtag currentImp_Selected disabled
		$Global(CurrentCanvas) dtag Imp_Selected disabled
		$Global(CurrentCanvas) dtag Imp_SelectedText disabled
		$Global(CurrentCanvas) dtag Imp_SelectedPhoto disabled
        shadeCurrentSelected
        $Global(CurrentCanvas) dtag currentImp_Selected   
	} else {
		foreach i $itemList {
			set tags [$Global(CurrentCanvas) gettags $i]
			selectItem $i $tags All
		}
	}
	createHandles
	update idletasks
	. configure -cursor ""
	update idletasks
}

proc selectItem {item tags type} {
	global Global

	if {"$item" != "" && [lsearch -exact "$tags" "disabled"] == -1} {
		if {[lsearch -exact "$tags" "Imp_Selected"] == -1 && \
			[lsearch -exact "$tags" "Imp_SelectedText"] == -1 &&
			[lsearch -exact "$tags" "Imp_SelectedPhoto"] == -1} {
			switch $type {
				New {
					deselectAll
				}
			}
			set itemList [getGroupItems $item]
			foreach i $itemList {
				$Global(CurrentCanvas) addtag Imp_selected withtag $i
				$Global(CurrentCanvas) addtag currentImp_Selected withtag $i
				set t [$Global(CurrentCanvas) type $i]
				if {$t == "text"} {
					$Global(CurrentCanvas) addtag Imp_SelectedText withtag $i
				} elseif {$t == "image"} {
					$Global(CurrentCanvas) addtag Imp_SelectedPhoto withtag $i
				}
			}
			shadeCurrentSelected
			$Global(CurrentCanvas) addtag Imp_Selected withtag currentImp_Selected
			$Global(CurrentCanvas) dtag currentImp_Selected
		} elseif {"$type" == "Add" && \
			[lsearch -exact "$tags" "selectorbox"] != -1} {
			set itemList [eval $Global(CurrentCanvas) find enclosed \
				[$Global(CurrentCanvas) bbox $item]]
			foreach i $itemList {
				$Global(CurrentCanvas) dtag $i Imp_selected
				$Global(CurrentCanvas) dtag $i Imp_Selected
				$Global(CurrentCanvas) dtag $i Imp_SelectedText
				$Global(CurrentCanvas) dtag $i Imp_SelectedPhoto
				if {[lsearch -exact [$Global(CurrentCanvas) gettags $i] \
					"selectorbox"] != -1} {
					$Global(CurrentCanvas) delete $i
				}
			}
			$Global(CurrentCanvas) delete $item
		}
	} elseif {"$type" == "New"} {
		deselectAll
	}
}

proc selectBoundedItems {bb type} {
	global Global

	. configure -cursor watch
	update idletasks
	set itemList [eval $Global(CurrentCanvas) find enclosed $bb]
	if {[llength $itemList] > 500} {
		eval $Global(CurrentCanvas) addtag Imp_selected enclosed $bb
		eval $Global(CurrentCanvas) addtag currentImp_Selected enclosed $bb
        shadeCurrentSelected
        $Global(CurrentCanvas) addtag Imp_Selected withtag currentImp_Selected
        $Global(CurrentCanvas) dtag currentImp_Selected   
	} else {
		foreach i $itemList {
			set tags [$Global(CurrentCanvas) gettags $i]
			selectItem $i $tags $type
		}
	}
	update idletasks
	. configure -cursor ""
	update idletasks
}

proc dragBoxOrMove {item tags x y X Y} { 
	global Global

	# If we don't update the tags here, we won't see the
	# potential new selection we just made.
	set tags [$Global(CurrentCanvas) gettags $item]

	if {[lsearch -exact "$tags" "Imp_Selected"] == -1 && \
		[lsearch -exact "$tags" "Imp_SelectedText"] == -1 && \
		[lsearch -exact "$tags" "Imp_SelectedPhoto"] == -1} {
		# Drag a selection box if there is no evidence of
		# being on a Imp_Selected item.
		$Global(CurrentCanvas) delete selector
		repositionWindow $X $Y
		$Global(CurrentCanvas) create rectangle \
			$Global(LastX) $Global(LastY) $x $y \
			-tags selector -fill "" -outline black
	} else {
		# Move Imp_Selected items if we are dragging on a
		# Imp_Selected item.
		set cursor [lindex [$Global(CurrentCanvas) configure -cursor] 4]
		if { "$cursor" == "" || "$cursor" == "crosshair" } {
			repositionWindow $X $Y
			set possibles [$Global(CurrentCanvas) find enclosed \
				[expr $x - $Global(GravityVal)] \
				[expr $y - $Global(GravityVal)] \
				[expr $x + $Global(GravityVal)] \
				[expr $y + $Global(GravityVal)]]
			set hs [$Global(CurrentCanvas) find withtag handles]
			set tiend ""
			foreach possible $possibles {
				if {"$tiend" != ""} {
					break
				}
				foreach h $hs {
					if {$possible == $h} {
						set tiend $h
						break
					}
				}
			} 
			set ho [handleOrigin [lindex \
				[$Global(CurrentCanvas) itemcget $tiend -tags] end]]
			if { $Global(Gravity) && "$ho" != "" && !$Global(AnchorChosen)} {
				$Global(CurrentCanvas) move Imp_Selected \
					[expr $x - [lindex $ho 0]] \
					[expr $y - [lindex $ho 1]]
				set Global(AnchorChosen) 1
			} else {
				$Global(CurrentCanvas) move Imp_Selected \
					[expr $x - $Global(LastX)] \
					[expr $y - $Global(LastY)]
				set Global(AnchorChosen) 1
			}
			set Global(LastX) $x
			set Global(LastY) $y
		}
	}
}
proc rotateSelected {} {
	global Global Group

	. configure -cursor watch
	update
	set items [$Global(CurrentCanvas) find withtag Imp_Selected]
	foreach item $items {
		set gsave ""
 		if {[info exists Group($Global(CurrentCanvas),$item)]} {
			set gsave $Group($Global(CurrentCanvas),$item)
			unset Group($Global(CurrentCanvas),$item)
		}
		set coords [$Global(CurrentCanvas) coords $item]
		set tags [$Global(CurrentCanvas) gettags $item]
		set props [getProperties $Global(CurrentCanvas) $item none 0]
		set type [$Global(CurrentCanvas) type $item]
		$Global(CurrentCanvas) delete $item
		set i [eval $Global(CurrentCanvas) create $type \
			[eval RotateF $Global(LastAngle) $Global(LastX) $Global(LastY) \
							"$coords"] \
					$props]
		if {$gsave != ""} {
			set Group($Global(CurrentCanvas),$i) "$gsave"
		}
	}
	update idletasks
	. configure -cursor ""
	update idletasks
}

proc pasteCutCopy { x y pastetype cuttype doselect} {
	global Global Group

	. configure -cursor watch
	update idletasks
	set pastebuf $Global($cuttype)
	if {"$pastebuf" != ""} {
		switch "$pastetype" {
			relative {
				append pastebuf "
					\$Global(CurrentCanvas) move Imp_Buffer $x $y;
				"
			}
			absolute {
				append pastebuf "
					originObject Imp_Buffer;
					\$Global(CurrentCanvas) move Imp_Buffer $x $y;
				"
			}
		}
		append pastebuf "
			set itemList \[\$Global(CurrentCanvas) find withtag Imp_Buffer\];
			\$Global(CurrentCanvas) dtag Imp_Buffer;
			deselectAll;"

		if {$doselect} {
			append pastebuf "
				foreach item \$itemList {
					set tags \[\$Global(CurrentCanvas) gettags \$item\]
					selectItem \$item \"\$tags\" Add
				}
				createHandles
			"
		}
		after idle $pastebuf
	}
	update idletasks
	. configure -cursor ""
	update idletasks
}

proc OpasteCutCopy { x y cuttype lve mx my } {
	global Global Group

	. configure -cursor watch
	update idletasks
	deselectAll
	if {"$Global($cuttype)" != ""} {
		if { $lve } {
			append Global($cuttype) "
				set itemList \[\$Global(CurrentCanvas) find withtag cutImp_selected\]
				foreach item \$itemList {
					set tags \[\$Global(CurrentCanvas) gettags \$item\]
					selectItem \$item \"\$tags\" Add
				}
				moveObject cutImp_selected $mx $my
				\$Global(CurrentCanvas) dtag cutImp_selected
				createHandles
			"
			after idle $Global($cuttype)
		} else {
			append Global($cuttype) "
				moveObject cutImp_selected $mx $my
				\$Global(CurrentCanvas) dtag cutImp_selected
			"
			after idle $Global($cuttype)
		}
	}
	set Global($cuttype) ""
	if {"$cuttype" == "Cut"} {
		disablePaste
	}
	update idletasks
	. configure -cursor ""
	update idletasks
}

proc clearAll {} {
	global Global

	if {$Global(Plugin)} {
		. configure -cursor watch
		for {set page 1} {[winfo exists .can.c$page]} {incr page} {
			destroy .can.c$page
		}
		set Global(CurrentPageId) 1
		newCanvas .can c$Global(CurrentPageId)
		. configure -cursor ""
	} elseif {! [tk_dialog .clearAll "New?" \
		"Selecting New will erase everything?" "" "0" "Yes" "No"] } {
		. configure -cursor watch
		for {set page 1} {[winfo exists .can.c$page]} {incr page} {
			destroy .can.c$page
		}
		set Global(CurrentPageId) 1
		newCanvas .can c$Global(CurrentPageId)
		. configure -cursor ""
	}
}

proc saveAll {} {
	global Global Group

	append nativeOut {#!/bin/sh
# restart trick }
	append nativeOut "\\"
	append nativeOut {
exec wish "$0" "$@"
if {![info exists Global(CurrentCanvas)]} {
	proc nextPage {} {
		global Global

		set Global(CurrentPageId) [expr $Global(CurrentPageId) + 1]
		if {![winfo exists .can.c$Global(CurrentPageId)]} {
			if {$Global(Circulate)} {
				set Global(CurrentPageId) 1 
			} else {
				set Global(CurrentPageId) [expr $Global(CurrentPageId) - 1]
			}
		}
		newCanvas .can c$Global(CurrentPageId)
		update idletasks
	}

	proc prevPage {} {
		global Global
	
		if {$Global(CurrentPageId) > 1} {
			set Global(CurrentPageId) [expr $Global(CurrentPageId) - 1]
			newCanvas .can c$Global(CurrentPageId)
			update idletasks
		}
	}
	proc newCanvas { cf name } {
		global Global

		if {[winfo exists $Global(CurrentCanvas)]} {
			pack forget $Global(CurrentCanvas)
		}
		set Global(CurrentCanvas) $cf.$name
		if {[winfo exists $Global(CurrentCanvas)]} {
			pack $Global(CurrentCanvas) -in $cf.top -side left -fill both \
				-expand true
			update idletasks
			if {$Global(Fit)} {
				scaleObject all "" \
				[winfo width $Global(CurrentCanvas)] \
				[winfo height $Global(CurrentCanvas)]
			}
		} else {
			if {!$Global(Plugin)} {
    			wm maxsize . [winfo pixels . $Global(PageWidth)] \
					[winfo pixels . $Global(PageHeight)]
			}
    		canvas $Global(CurrentCanvas) -bg white \
				-scrollregion "0i 0i $Global(PageWidth) $Global(PageHeight)" \
				-xscrollcommand "$cf.scrollx set" \
				-yscrollcommand "$cf.scrolly set"
			bind $Global(CurrentCanvas) <2> {%W scan mark %x %y}
			bind $Global(CurrentCanvas) <B2-Motion> {%W scan dragto %x %y}
			bind . <Key-Next> {nextPage}
			bind . <Key-Prior> {prevPage}
			pack $Global(CurrentCanvas) -in $cf.top -side left -fill both \
				-expand true
		}
	}

	set Global(Circulate) 0
	set Global(Fit) 0
	set Global(PointsInch) [winfo fpixels . 1i]
	if {[info exists embed_args]} {
		set Global(Plugin) 1
	} else {
		set Global(Plugin) 0
	}
    frame .can
    frame .can.top
    frame .can.bottom
	frame .can.buttons
	button .can.origin -text Origin -command \
		{originObject $Global(CurrentCanvas) all}
	checkbutton .can.fit -text Fit -variable Global(Fit)
	checkbutton .can.circ -text Circulate -variable Global(Circulate)
	button .can.next -text Next -command {nextPage}
	button .can.prev -text Prev -command {prevPage}
	button .can.dismiss -text Dismiss -command "exit"
    scrollbar .can.scrolly -bd 2 -width 15 \
		-command {$Global(CurrentCanvas) yview}
    scrollbar .can.scrollx -orient h -bd 2 -width 15 \
		-command {$Global(CurrentCanvas) xview}
    frame .can.corner -height 15 -width 15 -bd 2
	if {!$Global(Plugin)} {
    	wm maxsize . [winfo pixels . 8.5i] [winfo pixels . 11i]
	}
    pack .can.scrolly -in .can.top -anchor se -side right -fill y
    pack .can.scrollx -in .can.bottom -anchor se -side left -expand true -fill x
    pack .can.corner -in .can.bottom -anchor ne -in .can.bottom \
		-side right -fill both
	pack .can.prev .can.next .can.origin .can.fit .can.circ -side left \
		-in .can.buttons
    pack .can.top -side top -in .can -expand true -fill both
    pack .can.bottom -side top -in .can -anchor sw -fill x
    pack .can.buttons -side top -in .can -anchor sw -fill x
    pack .can -fill both -expand true
	proc scaleObject {name ratio width height} {
		global Global

		set c $Global(CurrentCanvas)
		set bb [$c bbox $name]
		if {"$bb" == ""} {
			return
		}
		set x [lindex $bb 2]
		set y [lindex $bb 3]
		if {$ratio != ""} {
			set scale $ratio
		} else {
			if {$x > $y} {
				set scale [expr ( $width + 0.0 ) / $x]
			} else {
				set scale [expr ( $height + 0.0 ) / $y]
			}
		}
		$c scale $name 0 0 $scale $scale

		foreach i [$c find withtag all] {
			set type [$c type $i]
			if {"$type" != "text"} {
				continue
			}
			if {1} {
				set fn [$c itemcget $i -font]
				regexp \
				{([-][^-]*-[^-]*-[^-]*-[^-]*-[^-]*-[^-]*-[^-]*-)([^-]*)(-.*)}\
					$fn dummy d1 size d2;
				if {"$dummy" != ""} {
					set nsize [expr round($size * $scale)]
					if {$nsize < 20} {
						set nsize 20
					}
					$c itemconfigure $i \
						-font ${d1}${nsize}${d2}
					set fnn [$c itemcget $i -font]
					regexp \
					{([-][^-]*-[^-]*-[^-]*-[^-]*-[^-]*-[^-]*-[^-]*-)([^-]*)(-.*)}\
					$fnn dummy d1 nsize d2;
					if { ($scale < 1 && $nsize < $size ) || \
				 		($scale > 1 && $nsize > $size) } {
						$c itemconfigure $i \
							-width [expr [$c itemcget $i \
								-width] * $scale]
					} else {
						$c itemconfigure $i \
							-font $fn
					}
				}
			} else {
				$c itemconfigure $i \
					-width [expr [$c itemcget $i -width] \
						* $scale]
			}
		}
	}
	proc originObject { c name } {
		set bb [$c bbox $name]
		set x [lindex $bb 0]
		set y [lindex $bb 1]
		if {$x > 0} {
			set nx -$x
		} else {
			set nx [expr abs($x)]
		}
		if {$y > 0} {
			set ny -$y
		} else {
			set ny [expr abs($y)]
		}
		$c move $name $nx $ny
	}
}
	if {[info exists Global(EmbedPage)]} {
		if {!$Global(EmbedPage)} {
			set Global(CurrentPageId) 0
			set Global(CurrentCanvas) ""
		} elseif {[info exists Global(CurrentPageId)]} {
			set Global(CurrentPageId) [expr $Global(CurrentPageId) - 1]
		}
	} else {
		set Global(CurrentPageId) 0
		set Global(CurrentCanvas) ""
	}
	set Global(LoadPageId) [expr $Global(CurrentPageId) + 1]
}
	append nativeOut "    set Global(DocPointsInch) $Global(PointsInch)
    set Global(SlideBG) $Global(SlideBG)
	set Global(Circulate) $Global(Circulate)
	"
 
	for {set page 1} {[winfo exists .can.c$page]} {incr page} {
		set Global(CurrentCanvas) .can.c$page
		deselectAll
		. configure -cursor watch
		update idletasks	
		$Global(CurrentCanvas) delete selectorbox
		set itemList [$Global(CurrentCanvas) find withtag all]
		append nativeOut \
			"
			set Global(CurrentPageId) \[expr \$Global(CurrentPageId) + 1\]
			set Global(PageHeight) $Global($Global(CurrentCanvas)_Height)
			set Global(PageWidth) $Global($Global(CurrentCanvas)_Width)
			set Global(Landscape) $Global($Global(CurrentCanvas)_Landscape)
			newCanvas .can c\$Global(CurrentPageId)
			"
		if {$itemList != ""} {
			foreach item $itemList {
				set gsave ""
 				if {[info exists Group($Global(CurrentCanvas),$item)]} {
					set gsave $Group($Global(CurrentCanvas),$item)
				}
				set props [getProperties $Global(CurrentCanvas) $item "" 0]
				set coords [$Global(CurrentCanvas) coords $item]
				set type [$Global(CurrentCanvas) type $item]
				if {"$type" == "window"} {
					continue
				}
				append nativeOut \
				"set i \[\$Global(CurrentCanvas) create $type [regsub -all \
				{([^ ][^ ]* [^ ][^ ]* [^ ][^ ]* [^ ][^ ]*)} $coords {\1 \\
} stuff; set stuff] $props\]\n"
				if {"$gsave" != ""} {
					append nativeOut \
						"set Group(\$Global(CurrentCanvas),\$i) {$gsave}\n"
				}
			}
			append nativeOut {scaleObject all \
				[expr $Global(PointsInch)/$Global(DocPointsInch)] "" ""}
		}
	}
	append nativeOut {
		set Global(CurrentPageId) $Global(LoadPageId)
		newCanvas .can c$Global(CurrentPageId)
	}
	update idletasks	
	. configure -cursor ""
	update idletasks	
	set Global(CurrentCanvas) .can.c$Global(CurrentPageId)
	return $nativeOut
}
proc cutCopySelected { cuttype tagname dogroups } {
	global Global Group

	set Global($cuttype) ""
	. configure -cursor watch
	update idletasks	
	$Global(CurrentCanvas) delete selectorbox
	set itemList [$Global(CurrentCanvas) find withtag $tagname]
	if {$itemList != ""} {
		set bb [$Global(CurrentCanvas) bbox $tagname]
		foreach item $itemList {
			set gsave ""
 			if {$dogroups && [info exists Group($Global(CurrentCanvas),$item)]} {
				foreach g $Group($Global(CurrentCanvas),$item) {
					if {![info exists CopyGroup($g)]} {
						set GroupNo [clock clicks]
						set CopyGroup($g) "Group${GroupNo}"
					}
					lappend gsave $CopyGroup($g)
				}
			}
			set props [getProperties $Global(CurrentCanvas) $item Esc $dogroups]
			set coords [$Global(CurrentCanvas) coords $item]
			set type [$Global(CurrentCanvas) type $item]
			if {"$type" == "window"} {
				continue
			}
			append Global($cuttype) "
			set i \[eval \$Global(CurrentCanvas) create $type $coords $props\];
			\$Global(CurrentCanvas) addtag Imp_Buffer withtag \$i;"
			if { $gsave != "" } {
				append Global($cuttype) "
					set Group(\$Global(CurrentCanvas),\$i) {$gsave};"
				foreach g $gsave {
					append Global($cuttype) "
						\$Global(CurrentCanvas) addtag $g withtag \$i;"
				}
			}
			if {"$cuttype" == "Cut"} {
				$Global(CurrentCanvas) delete $item
			}
		}
		if {"$cuttype" == "Cut"} {
			enablePaste
		}
	}
	update idletasks	
	. configure -cursor ""
	update idletasks	
}
proc OcutCopySelected { cuttype tagname preserve } {
	global Global Group

	set Global($cuttype) ""
	. configure -cursor watch
	update idletasks	
	$Global(CurrentCanvas) delete selectorbox
	set itemList [$Global(CurrentCanvas) find withtag $tagname]
	if {$itemList != ""} {
		set bb [$Global(CurrentCanvas) bbox $tagname]
		set x [lindex $bb 0]
		set y [lindex $bb 1]
		if {!$preserve} {
			if {$x > 0} {
				set nx -$x
			} else {
				set nx [expr abs($x)]
			}
			if {$y > 0} {
				set ny -$y
			} else {
				set ny [expr abs($y)]
			}
		}
		deselectAll
		foreach item $itemList {
			set gsave ""
 			if {[info exists Group($Global(CurrentCanvas),$item)]} {
				set gsave $Group($Global(CurrentCanvas),$item)
			}
			$Global(CurrentCanvas) addtag cutImp_selected withtag $item
			set props [getProperties $Global(CurrentCanvas) $item Esc 0]
			set coords [$Global(CurrentCanvas) coords $item]
			set type [$Global(CurrentCanvas) type $item]
			if {"$type" == "window"} {
				continue
			}
			if {$preserve} {
				append Global($cuttype) \
				"set i \[eval \$Global(CurrentCanvas) create $type $coords $props\];"
			} else {
				append Global($cuttype) \
				"set i \[eval \$Global(CurrentCanvas) create $type \[trans \$x \$y \"[trans $nx $ny $coords]\"\] $props\];"
				if {"$gsave" != ""} {
					append Global($cuttype) \
						"set Group(\$Global(CurrentCanvas),\$i) {$gsave};"
					unset Group($Global(CurrentCanvas),$item)
				}
			}
			if {"$cuttype" == "Cut"} {
				$Global(CurrentCanvas) delete $item
			}
			$Global(CurrentCanvas) dtag cutImp_selected
		}
		if {"$cuttype" == "Cut"} {
			enablePaste
		}
	}
	update idletasks	
	. configure -cursor ""
	update idletasks	
}

proc duplicateSelected {} {
	global Group Global GroupNo

	. configure -cursor watch
	update idletasks
	cutCopySelected Copy Imp_Selected 1
	deselectAll
	update idletasks
	pasteCutCopy 20 20 relative Copy 1
	update idletasks
	. configure -cursor ""
	update idletasks
}

proc OduplicateSelected {} {
	global Group Global GroupNo

	set GroupNo [clock clicks]
	$Global(CurrentCanvas) configure -cursor watch
	update idletasks	
	$Global(CurrentCanvas) delete selectorbox
	set itemList [$Global(CurrentCanvas) find withtag Imp_Selected]
	if {$itemList != ""} {
		if {$Global(Duplicate) != $itemList} {
			set Global(DuplicateX) 20
			set Global(DuplicateY) 20
		}
		deselectAll
		foreach item $itemList {
			set gsave ""
 			if {[info exists Group($Global(CurrentCanvas),$item)]} {
				set gsave $Group($Global(CurrentCanvas),$item)
			}
			set props [getProperties $Global(CurrentCanvas) $item none 1]
			set coords [$Global(CurrentCanvas) coords $item]
			set type [$Global(CurrentCanvas) type $item]
			set i [eval $Global(CurrentCanvas) create $type \
				[trans $Global(DuplicateX) $Global(DuplicateY) $coords] \
				$props]
			$Global(CurrentCanvas) addtag duplicate withtag $i
			if {$gsave != ""} {
				$Global(CurrentCanvas) dtag $i ${gsave}
				$Global(CurrentCanvas) addtag "${gsave}${GroupNo}" withtag $i
				set Group($Global(CurrentCanvas),$i) "${gsave}${GroupNo}"
			}
		}
		incr Global(DuplicateX) 20
		incr Global(DuplicateY) 20
		set itemList [$Global(CurrentCanvas) find withtag duplicate]
		foreach item $itemList {
			set tags [$Global(CurrentCanvas) gettags $item]
			selectItem $item $tags Add
			$Global(CurrentCanvas) dtag duplicate
		}
	}
	update idletasks
	$Global(CurrentCanvas) configure -cursor ""
	update idletasks
}

proc updateDemoLine { args } {
	global Global

	if {"$args" == "spline" } {
		if {$Global(Smooth)} {
			set Global(Smooth) 0
			set Global(Spline) 0
			set Global(Message) "Change to Smooth Lines"
		} else {
			set Global(Smooth) $Global(Spline)
			if {$Global(Spline)} {
				set Global(Message) "Change to Straight Lines"
			}
		}
	}
	.widthc itemconfigure line \
		-fill "$Global(Outline)" -stipple "$Global(Stipple)" \
		-arrow "$Global(Arrow)" -joinstyle "$Global(JoinStyle)" \
		-width "$Global(Width)" -smooth "$Global(Smooth)"
}

proc colorRequestor {name colorslot redlist greenlist bluelist replace} {
	global Global rede grne blue redv grnv bluv

	set w ${name}
	set Global($name) $replace
	if {! $Global(Plugin) } {
		eval toplevel $w $Global(PrivateCmap)
#		wm transient $w .
		wm protocol $w WM_DELETE_WINDOW {set dummy 1}
	} else {
		frame $w -borderwidth 0
	}
	set redv "0000"
	set grnv "0000"
	set bluv "0000"

	frame $w.left -borderwidth 0 -highlightthickness 0
	frame $w.right -borderwidth 0 -highlightthickness 0
	frame $w.color -relief sunken -bd 2 -highlightthickness 0
	frame $w.colortop -borderwidth 0 -highlightthickness 0
	frame $w.colorbottom -borderwidth 0 -highlightthickness 0

	set rede [createEntry $w red "hex" 4 -width 5 -textvariable redv]
	set reds [createScale $w reds -to 65535 -length 2i -orient h \
		-showvalue false]
	set redcs [createComboScale $w redcs Red $reds $rede]
	pack $redcs -in $w.left -side top -fill x -expand true

	set grne [createEntry $w grn "hex" 4 -width 5 -textvariable grnv]
	set grns [createScale $w grns -to 65535 -length 2i -orient h \
		-showvalue false]
	set grncs [createComboScale $w grncs Green $grns $grne]
	pack $grncs -in $w.left -side top -fill x -expand true

	set blue [createEntry $w blu "hex" 4 -width 5 -textvariable bluv]
	set blus [createScale $w blus -to 65535 -length 2i -orient h \
		-showvalue false]
	set blucs [createComboScale $w blucs Blue $blus $blue]
	pack $blucs -in $w.left -side top -fill x -expand true

	set cancel [createButton $w cancel "" -text "Cancel"]
	set ok [createButton $w ok 1 -text "Ok"]
	$ok configure -command "set Global(NewColor) \[setColor {}\]
		destroy .cr
		if {\$Global(NewColor) != \"\"} {
			set Global($colorslot) \$Global(NewColor)
			set Global(\$Global(colortype)) \$Global(NewColor)
			$Global(F_$Global(colortype)).f_\$Global(colortype) \
				configure -bg \$Global(NewColor)
		}
		colorPaletteReq .colorpalette {$redlist} \
			{$greenlist} {$bluelist} $replace
	"
	$cancel configure -command "set Global(NewColor) \"\"
		destroy $w
		colorPaletteReq .colorpalette {$redlist} \
			{$greenlist} {$bluelist} $replace
	"
	bind $w <Control-Key-Escape> "set Global(NewColor) \"\"
		destroy $w
		colorPaletteReq .colorpalette {$redlist} \
			{$greenlist} {$bluelist} $replace
	"

	frame $w.colorc -height 2i -width 2i -highlightthickness 0
	raise $w.color
	raise $w.colorc
	pack $w.colorc -in $w.color -fill both -expand true
	pack $w.color -in $w.right -padx 2m -pady 2m -fill both -expand true
	pack $w.left $w.right -side left -in $w.colortop -fill both -expand true
	pack $cancel $ok -side left -expand true -in $w.colorbottom \
		-padx 2m -pady 2m
	pack $w.colortop -side top -in $w -fill both -expand true
	pack $w.colorbottom -side bottom -in $w

	proc setColor {v} "
		global rede grne blue
		
		if { \"\$v\" != \"\" } {
			set color \$v
			scan \"\$v\" \{#%4x%4x%4x\} redv grnv bluv
			set_red \$redv
			set_grn \$grnv
			set_blu \$bluv
		} else {
			set color \"#\[\$rede get\]\[\$grne get\]\[\$blue get\]\"
		}
		$w.colorc configure -bg \$color
		return \$color
	"

	proc set_red { val } "
		[info body set_red]
		$reds set \$val
		setColor {}
	"
	bind $rede <Key-Return> "+setColor {}"

	proc set_grn { val } "
		[info body set_grn]
		$grns set \$val
		setColor {}
	"
	bind $blue <Key-Return> "+setColor {}"

	proc set_blu { val } "
		[info body set_blu]
		$blus set \$val
		setColor {}
	"
	bind $blue <Key-Return> "+setColor {}"

	setColor [$colorslot cget -bg]

	if { $Global(Plugin) } {
		pack $w -in $replace -fill both -expand true
	}
}


proc clearRadioVariable {var} {
	global Global

	if {[info exists Global($var)] && [winfo exists $Global($var)]} {
		set bd [$Global($var) cget -bd]
		$Global($var) configure -relief $Global(ButtonRelief)
		eval $Global($var) move all -$bd -$bd
	}
	set $var ""
}

proc setRadioButton { var value } {
	global Global

	if {[info exists Global(W_${value})]} {
		set bd [$Global(W_${value}) cget -bd]
		clearRadioVariable $var
		$Global(W_${value}) configure -relief sunken
		$Global(W_${value}) move all $bd $bd
		set Global($var) $Global(W_${value})
	}
	set $var "$value"
}

proc createNormalButton {name width height cmds value args} {
	global Global

	eval canvas $name -width $width -height $height \
		-relief $Global(ButtonRelief) $args

	set saveit $Global(CurrentCanvas)
	set Global(CurrentCanvas) $name
	eval $cmds
	originObject all
	scaleObject all "" [winfo fpixels $name $width] \
		[winfo fpixels $name $height]
	set Global(CurrentCanvas) $saveit
	bind $name <Button-1> "
		if {\[%W find withtag disabled\] == \"\"} {
			set bd \[%W cget -bd\]
			%W configure -relief sunken
			%W move all \$bd \$bd
			update idletasks
		}
	"
	bind $name <ButtonRelease-1> "
		if {\[%W find withtag disabled\] == \"\"} {
			set bd \[%W cget -bd\]
			popupCanvasSet $value
			%W move all -\$bd -\$bd
			%W configure -relief $Global(ButtonRelief)
		}
	"
	bind $name <Enter> "
		if {\[%W find withtag disabled\] == \"\"} {
			%W configure -bg $Global(ActiveBackground)
			if {[info exists Global(Msg_$value)]} {
				set Global(Message) \$Global(Msg_$value)
			} else {
				set Global(Message) $value
			}
		}
	"
	bind $name <Leave> {
		if {[%W find withtag disabled] == ""} {
			%W configure -bg $Global(Background)
			set Global(Message) $Global(Cursor)
		}
	}

	return $name  
}
proc createRadioButton {name width height cmds var value args} {
	global Global

	eval canvas $name -width $width -height $height \
		-relief $Global(ButtonRelief) $args

	set saveit $Global(CurrentCanvas)
	set Global(CurrentCanvas) $name
	eval $cmds
	originObject all
	scaleObject all "" [winfo fpixels $name $width] \
		[winfo fpixels $name $height]
	set Global(CurrentCanvas) $saveit
	bind $name <Button-1> "
		if {\[%W find withtag disabled\] == \"\"} {
			setRadioButton $var $value
			if {\"$var\" == \"Global(Cursor)\"} {
				popupCanvasSet $value
			}
		}
	"
	bind $name <Enter> "
		if {\[%W find withtag disabled\] == \"\"} {
			%W configure -bg $Global(ActiveBackground)
			if {[info exists Global(Msg_$value)]} {
				set Global(Message) \$Global(Msg_$value)
			} else {
				set Global(Message) $value
			}
		}
	"
	bind $name <Leave> {
		if {[%W find withtag disabled] == ""} {
			%W configure -bg $Global(Background)
			set Global(Message) $Global(Cursor)
		}
	}

	return $name  
}

proc buttonReq { name var replace } {
	global Global

	if {$Global(ButtonOrient) == "left"} {
		set suborient top
	} else {
		set suborient left
	}
	set w ${name}
	if {[winfo exists $w]} {
		if {!$Global(Plugin)} {
			wm deiconify $w
			raise $w
			return
		}
		return
	}
	set Global($name) $replace

	if { !$Global(Plugin) } {
		eval toplevel $w
		wm protocol $w WM_DELETE_WINDOW "wm withdraw $w"
	} else {
		frame $w -borderwidth 0
	}
	
	frame $w.f
	set bframes "$w.f1 $w.f2 $w.f3 $w.f4 $w.f5 $w.f6 $w.f7 $w.f8 $w.f9"

	set i 0
	set bframe ""
	foreach buttontype $var {

		if {![expr $i % $Global(ButtonCount)]} {
			if {$bframe != ""} {
				pack $bframe -in $w.f -side $suborient
			}
			set bframe [lindex $bframes [expr $i / $Global(ButtonCount)]]
			frame $bframe
		}
		incr i
		set type [lindex $buttontype 0]
		set buttonName [lindex $buttontype 1]
		set buttonCanv [lindex $buttontype 2]
		switch "$type" {
			RadioButton {
				createRadioButton $w.b_${buttonName} \
					$Global(ButtonSize) $Global(ButtonSize) \
					"$buttonCanv" Global(Cursor) \
					$buttonName -bd 1 -highlightthickness 0
			}
			Button {
				createNormalButton $w.b_${buttonName} \
					$Global(ButtonSize) $Global(ButtonSize) \
					"$buttonCanv" \
					$buttonName -bd 1 -highlightthickness 0
			}
			JoinButton { 
				createRadioButton $w.b_${buttonName} \
					$Global(ButtonSize) $Global(ButtonSize) \
					"$buttonCanv" Global(JoinStyle) \
					$buttonName -bd 1 -highlightthickness 0
			}
			ArrowButton {
				createRadioButton $w.b_${buttonName} \
					$Global(ButtonSize) $Global(ButtonSize) \
					"$buttonCanv" Global(Arrow) \
					$buttonName -bd 1 -highlightthickness 0
			}
		}
		switch "$buttonName" {
			Paste {
				disablePaste
			}
			Rulers {
				bind $w.b_Rulers <Double-Button-1> {
					%W addtag disabled withtag all
                	$Global(CurrentCanvas) delete $Global(RulerX)
                	$Global(CurrentCanvas) delete $Global(RulerY)
        		}     
				bind $w.b_Rulers <ButtonRelease-1> {+
					%W dtag disabled
				}
			}
		}
		set Global(W_${buttonName}) $w.b_${buttonName}
		pack $w.b_${buttonName} -in $bframe -side $Global(ButtonOrient)
	}
	pack $bframe -in $w.f -side left
	pack $w.f -in $w
	if { $Global(Plugin) } {
		pack $w -in $replace -fill both -expand true
		update idletasks
	}
}

proc colorPaletteReq { name redlist greenlist bluelist replace } {
	global Global

	# Setup
	set w ${name}
	if {[winfo exists $w]} {
		if {$Global(Plugin)} {
			destroy $w
		} else {
			wm deiconify $w
			raise $w
			return
		}
	}
	set Global($name) $replace
	if { !$Global(Plugin) } {
		eval toplevel $w $Global(PrivateCmap)
		wm protocol $w WM_DELETE_WINDOW "wm withdraw $w"
	} else {
		frame $w -borderwidth 0
	}
  
	frame $w.f

	foreach red $redlist {
		frame $w.f.rcol_${red}
		foreach green $greenlist {
			frame $w.f.grow_${red}${green}
			foreach blue $bluelist {
				if { [info exists Global($w.f.c${red}${green}${blue})] } {
					frame $w.f.c${red}${green}${blue} \
						-relief raised -height 2m -width 2m \
						-highlightthickness 0 \
						-bd 1 -bg $Global($w.f.c${red}${green}${blue})
				} else {
					frame $w.f.c${red}${green}${blue} \
						-relief raised -height 2m -width 2m \
						-highlightthickness 0 \
						-bd 1 -bg "#${red}${green}${blue}"
				}
				pack $w.f.c${red}${green}${blue} -side left \
					-in $w.f.grow_${red}${green} -fill both -expand true
				bind $w.f.c${red}${green}${blue} <1> {
					%W configure -relief sunken
				}
				bind $w.f.c${red}${green}${blue} <Double-Button-1> "
					set Global(NewColor) {None}
					colorRequestor .cr %W {$redlist} {$greenlist} {$bluelist} \
						$replace
					destroy $w 
				"
				bind $w.f.c${red}${green}${blue} <ButtonRelease-1> {
					%W configure -relief raised
					set Global($Global(colortype)) [%W cget -bg]
					$Global(F_$Global(colortype)).f_$Global(colortype) configure \
						-bg $Global($Global(colortype))
				}
			}
			pack $w.f.grow_${red}${green} -side top \
				-in $w.f.rcol_${red} -fill both -expand true
		}
		pack $w.f.rcol_${red} -side left -in $w.f -fill both \
			-expand true
	}
	if {!$Global(Plugin)} {
		bind $w <Configure> "
			set g \[wm geometry $w\]
			scan \"\$g\" {%%dx%%d%%d%%d} Global(ColorWidth) Global(ColorHeight)\
				Global(ColorPosX) Global(ColorPosY)
		"
	}
	frame $w.f.c_none -width 4m -relief raised -bd 1 \
		-highlightthickness 0
	pack $w.f.c_none -in $w.f -side left -fill y   
	bind $w.f.c_none <1> {
		%W configure -relief sunken
	}
	bind $w.f.c_none <ButtonRelease-1> {
		%W configure -relief raised
		set Global($Global(colortype)) ""
		$Global(F_$Global(colortype)).f_$Global(colortype) configure \
			-bg [lindex [$Global(F_$Global(colortype)).f_$Global(colortype) configure -bg] 3]
	}
	bind $w.f <Enter> {
		set Global(MessageSave) $Global(Message)
		set Global(Message) "$Global(colortype) Color"
	}
	bind $w.f <Leave> {
		set Global(Message) $Global(MessageSave)
	}
  

	pack $w.f -in $w -expand true -fill both

	# Return
	if { $Global(Plugin) } {
		pack $w -in $replace -fill both -expand true
		update idletasks
	} else {
		if {[info exists Global(ColorPosX)]} {
			wm geometry $w \
$Global(ColorWidth)x$Global(ColorHeight)+$Global(ColorPosX)+$Global(ColorPosY)
		} else {
			wm geometry $w $Global(ColorWidth)x$Global(ColorHeight)
		}
	}
}

proc newSlide {} {
	global Global

	if {![winfo exists .slide]} {
		. configure -cursor watch
		update idletasks
		set Global(CanvasSave) $Global(CurrentPageId)
		toplevel .slide -height $Global(ScreenH) -width $Global(ScreenW) \
			-bg $Global(SlideBG) -borderwidth 0 -relief flat -highlightthickness 0
		wm withdraw .slide
		wm overrideredirect .slide true
		.slide configure -cursor watch
		update idletasks
		bind .slide <Double-Button-3> {
			grab release [grab current]
			destroy .slide
			set Global(CurrentCanvas) .can.c$Global(CanvasSave)
			set Global(CurrentPageId) $Global(CanvasSave)
		}
		bind .slide <Control-Button-1> {
			grab release [grab current]
			destroy .slide
			set Global(CurrentCanvas) .can.c$Global(CanvasSave)
			set Global(CurrentPageId) $Global(CanvasSave)
		}
		bind .slide <Key-Escape> {
			grab release [grab current]
			destroy .slide
			set Global(CurrentCanvas) .can.c$Global(CanvasSave)
			set Global(CurrentPageId) $Global(CanvasSave)
		}
		bind .slide <Button-1> {
			set Global(CurrentCanvas) .can.c$Global(CanvasSave)
			set Global(CurrentPageId) [expr $Global(CurrentPageId) + 1]
			if {[winfo exists .can.c$Global(CurrentPageId)]} {
				newSlide
				wm deiconify .slide
				raise .slide
				raise .slide.c$Global(CurrentPageId)
				focus .slide
				grab .slide
			} else {
				set Global(CurrentPageId) [expr $Global(CurrentPageId) - 1]
			}
		}
		bind .slide <Key-Next> {
			set Global(CurrentCanvas) .can.c$Global(CanvasSave)
			set Global(CurrentPageId) [expr $Global(CurrentPageId) + 1]
			if {! [winfo exists .can.c$Global(CurrentPageId)]} {
				if {$Global(Circulate)} {
					set Global(CurrentPageId) 1
				} else {
					set Global(CurrentPageId) [expr $Global(CurrentPageId) - 1]
					break
				}
			}
			newSlide
			wm deiconify .slide
			raise .slide
			raise .slide.c$Global(CurrentPageId)
			focus .slide
			grab .slide
		}
		bind .slide <Button-3> {
			set Global(CurrentCanvas) .can.c$Global(CanvasSave)
			set Global(CurrentPageId) [expr $Global(CurrentPageId) - 1]
			if {[winfo exists .can.c$Global(CurrentPageId)]} {
				newSlide
				wm deiconify .slide
				raise .slide
				raise .slide.c$Global(CurrentPageId)
				focus .slide
				grab .slide
			} else {
				set Global(CurrentPageId) [expr $Global(CurrentPageId) + 1]
			}
		}
		bind .slide <Key-Prior> {
			set Global(CurrentCanvas) .can.c$Global(CanvasSave)
			set Global(CurrentPageId) [expr $Global(CurrentPageId) - 1]
			if {[winfo exists .can.c$Global(CurrentPageId)]} {
				newSlide
				wm deiconify .slide
				raise .slide
				raise .slide.c$Global(CurrentPageId)
				focus .slide
				grab .slide
			} else {
				set Global(CurrentPageId) [expr $Global(CurrentPageId) + 1]
			}
		}
	} else {
		.slide configure -cursor watch
		update idletasks
	}
	if {![winfo exists .slide.c$Global(CurrentPageId)]} {
		canvas .slide.c$Global(CurrentPageId) -bg $Global(SlideBG) \
			-height $Global(ScreenH) \
			-width $Global(ScreenW) -highlightthickness 0 -bd 0
		set Global(CurrentCanvas) .can.c$Global(CurrentPageId)
		cutCopySelected Copy all 0
		set Global(CurrentCanvas) .slide.c$Global(CurrentPageId)
		pasteCutCopy 0 0 normal Copy 0
		pack forget [pack slaves .slide]
		pack .slide.c$Global(CurrentPageId) -in .slide \
			-expand true -fill both
		scaleObject all "" $Global(ScreenW) $Global(ScreenH)
		scaleObject Imp_Background variable $Global(ScreenW)+3 $Global(ScreenH)+3
		update idletasks
		.slide configure -cursor ""
		. configure -cursor ""
		update idletasks
	} else {
		pack forget [pack slaves .slide]
		.slide.c$Global(CurrentPageId) configure -bg $Global(SlideBG)
		pack .slide.c$Global(CurrentPageId) -in .slide \
			-expand true -fill both
		update idletasks
		.slide configure -cursor ""
		. configure -cursor ""
		update idletasks
	}
	wm deiconify .slide
	raise .slide
	raise .slide.c$Global(CurrentPageId)
	focus .slide
	grab .slide
}

proc newCanvas { cf name } {
	global Global

	if {[winfo exists $Global(CurrentCanvas)]} {
		pack forget $Global(CurrentCanvas)
	}
	set Global(CurrentCanvas) "$cf.$name"
	if {![winfo exists $Global(CurrentCanvas)]} {
		canvas $Global(CurrentCanvas) -scrollregion \
			"0i 0i $Global(PageWidth) $Global(PageHeight)" \
			-width $Global(PageVisX) -height $Global(PageVisY) \
			-bg $Global(SlideBG) -xscrollcommand "$cf.scrollx set" \
			-yscrollcommand "$cf.scrolly set" -bd 0
		canvas $Global(CurrentCanvas).rulerx \
			-width $Global(PageWidth) -height .25i \
			-relief raised -bd 1 -highlightthickness 0
		canvas $Global(CurrentCanvas).rulery \
			-width .25i -height $Global(PageHeight) \
			-relief raised -bd 1 -highlightthickness 0
		doRuler $Global(CurrentCanvas).rulerx $Global(PageWidth) .25i 0 0 1 h \
			{.0625 .125 .25 .5} ""
		doRuler $Global(CurrentCanvas).rulery $Global(PageHeight) .25i 0 0 1 v \
			{.0625 .125 .25 .5} ""
		set Global(RulerX) [$Global(CurrentCanvas) create window 0i .25i \
			-anchor sw -window $Global(CurrentCanvas).rulerx -tags rulerx]
		set Global(RulerY) [$Global(CurrentCanvas) create window 0i 0i \
			-anchor nw -window $Global(CurrentCanvas).rulery -tags rulery]
		$Global(CurrentCanvas) delete $Global(RulerX)
		$Global(CurrentCanvas) delete $Global(RulerY)

	bind $Global(CurrentCanvas).rulery <1> {
		set pc [winfo parent %W]
		set rylastx [$pc canvasx %X]
		set rylasty [$pc canvasy %Y]
	}
	bind $Global(CurrentCanvas).rulerx <1> {
		set pc [winfo parent %W]
		set rxlastx [$pc canvasx %X]
		set rxlasty [$pc canvasy %Y]
	}
	bind $Global(CurrentCanvas).rulerx <Motion> {
		if {$Global(Grid)} {
			set x [%W canvasx %x $Global(GridX)]
		} else {
			set x [%W canvasx %x 1]
		}
		set Global(Xpos) \
			"[format {%%3.3f} [expr $x / $Global(CoordFactorX)]]$Global(UnitX)"
		$Global(CurrentCanvas).rulerx coords h \
			$x [$Global(CurrentCanvas) cget -height] $x 0 
	}
	bind $Global(CurrentCanvas).rulery <Motion> {
		if {$Global(Grid)} {
			set y [%W canvasy %y $Global(GridY)]
		} else {
			set y [%W canvasy %y 1]
		}
		set Global(Ypos) \
			"[format {%%3.3f} [expr $y / $Global(CoordFactorY)]]$Global(UnitY)"
		$Global(CurrentCanvas).rulery coords v \
			0 $y [$Global(CurrentCanvas) cget -width] $y 
	}
	bind $Global(CurrentCanvas).rulery <B1-Motion> {
		set pc [winfo parent %W]
		set x [$pc canvasx %X]
		$pc move rulery [expr $x - $rylastx] 0
		set rylastx $x
	}
	bind $Global(CurrentCanvas).rulerx <B1-Motion> {
		set pc [winfo parent %W]
		set y [$pc canvasy %Y]
		$pc move rulerx 0 [expr $y - $rxlasty]
		set rxlasty $y
	}

	bind $Global(CurrentCanvas) <Key-Next> {
		nextPage
	}
	bind $Global(CurrentCanvas) <Key-Prior> {
		prevPage
	}

	bind $Global(CurrentCanvas) <Control-Key-v> {
		pasteCutCopy $Global(LastX) $Global(LastY) absolute Cut 1
	}

	bind $Global(CurrentCanvas) <Key-Delete> {
		set Global(Message) Cut
		cutCopySelected Cut Imp_Selected 1
		setCursor Select
		set Global(Message) Select
		enableSelectionButtons
	}

	if {!$Global(Plugin)} {
		bind $Global(CurrentCanvas) <Alt-Key-s> {
			newSlide
		}
	}

	bind $Global(CurrentCanvas) <ButtonRelease-1> {
		set boxselector [%W coords selector]
		set Global(AnchorChosen) 0
		if {"$boxselector" != ""} {
			switch $Global(Cursor) {
				Select {
					%W delete selector
					selectBoundedItems $boxselector Add
					createHandles
				}
				Oval {
					%W delete selector
					createOval $boxselector
					if {$Global(MenuSelect)} {
						setCursor Select
						set Global(Message) Select
						enableSelectionButtons
						set Global(MenuSelect) 0
					}
				}
				Rounded_Rectangle {
					%W delete selector
					createRRectangle $boxselector 1
					if {$Global(MenuSelect)} {
						setCursor Select
						set Global(Message) Select
						enableSelectionButtons
						set Global(MenuSelect) 0
					}
				}
				Rectangle {
					%W delete selector
					createRRectangle $boxselector 0
					if {$Global(MenuSelect)} {
						setCursor Select
						set Global(Message) Select
						enableSelectionButtons
						set Global(MenuSelect) 0
					}
				}
				Text {
					%W delete selector
					createText $boxselector
					if {$Global(MenuSelect)} {
						setCursor Select
						set Global(Message) Select
						enableSelectionButtons
						set Global(MenuSelect) 0
					}
				}
				Rotate {
					%W delete selector
					rotateSelected
					%W raise selectorbox
					set Global(Sub-Cursor) ""
				}
			}
		} else {
			switch $Global(Cursor) {
				Scale {
					setCursor Select
					set Global(Message) Select
					enableSelectionButtons
					%W configure -cursor ""
				}
				Draw {
					%W delete Global(curline)
					if {[llength $Global(curline)] > 3} {
					eval %W create line $Global(curline) \
						-fill \"$Global(Outline)\" \
						-stipple \"$Global(Stipple)\" \
						-width \"$Global(Width)\" \
						-smooth \"$Global(Smooth)\" \
						-arrow \"$Global(Arrow)\" \
						-joinstyle \"$Global(JoinStyle)\" 
					if {$Global(MenuSelect)} {
						setCursor Select
						set Global(Message) Select
						enableSelectionButtons
						set Global(MenuSelect) 0
					}
					}
				}
			}
		}
	}

	bind $Global(CurrentCanvas) <Button-3> {
		switch $Global(Cursor) {
			Polygon -
			Line { 
				switch "$Global(Sub-Cursor)" {
					Line-Active {
						set Global(Sub-Cursor) ""
						%W delete Global(curline)
						%W delete selector
						createLine $Global(curline)
						if {$Global(MenuSelect)} {
							setCursor Select
							set Global(Message) Select
							enableSelectionButtons
							set Global(MenuSelect) 0
						}
					}
					Polygon-Active {
						set Global(Sub-Cursor) ""
						%W delete Global(curline)
						%W delete selector
						createPolygon $Global(curline)
						if {$Global(MenuSelect)} {
							setCursor Select
							set Global(Message) Select
							enableSelectionButtons
							set Global(MenuSelect) 0
						}
					}
				}
			}
		}
	}

	bind $Global(CurrentCanvas) <Control-Button-1> {
		focus %W
		markSpot %x %y
		switch $Global(Cursor) {
			Select {
				set item [findItemUnderCursor]
				set tags [%W gettags $item]
				. configure -cursor watch
				update idletasks
				selectItem $item $tags Add
				createHandles
				update idletasks
				. configure -cursor ""
				update idletasks
			}
		}
		break
	}

	bind $Global(CurrentCanvas) <Double-Button-1> {
		focus %W
		markSpot %x %y
		switch $Global(Cursor) {
			Select {
				set items [%W find withtag Imp_selected]
				if {"$items" != ""} {
					propRaise .prop $items
				}
			}
		}
	}

	bind $Global(CurrentCanvas) <Button-1> {
		focus %W
		markSpot %x %y
		switch $Global(Cursor) {
			Select {
				set item [findItemUnderCursor]
				set tags [%W gettags $item]
				. configure -cursor watch
				update idletasks
				selectItem $item $tags New
				createHandles
				update idletasks
				. configure -cursor ""
				update idletasks
			}
			Draw {
				set Global(curline) ""
				lappend Global(curline) [eval expr int($Global(LastX))] \
					[eval expr int($Global(LastY))]
			}
			Line {
				switch $Global(Sub-Cursor) {
					Line-Active {
						lappend Global(curline) $Global(LastX) \
							$Global(LastY)
						set l [llength $Global(curline)]
						if {$l > 2} {
							eval %W create line \
								[lrange $Global(curline) \
								[eval expr $l - 4] end] \
								-tags Global(curline)
						}
					}
					default {
						set Global(curline) ""
						lappend Global(curline) \
							[eval expr int($Global(LastX))] \
							[eval expr int($Global(LastY))]
						set Global(Sub-Cursor) Line-Active
					}
				}
			}
			Oval {
			}
			Rectangle {
			}
			Rounded_Rectangle {
			}
			Text {
			}
			Polygon {
				switch $Global(Sub-Cursor) {
					Polygon-Active {
						lappend Global(curline) \
							[eval expr int($Global(LastX))] \
							[eval expr int($Global(LastY))]
						set l [llength $Global(curline)]
						if {$l > 2} {
							eval %W create line \
								[lrange $Global(curline) \
								[eval expr $l - 4] end] \
								-tags Global(curline)
						}
					}
					default {
						set Global(curline) ""
						lappend Global(curline) \
							[eval expr int($Global(LastX))] \
							[eval expr int($Global(LastY))]
						set Global(Sub-Cursor) Polygon-Active
					}
				}
			}
			Rotate {
				set Global(Sub-Cursor) Rotate-Active
			}
			Paste {
				%W configure -cursor watch
				pasteCutCopy $Global(LastX) $Global(LastY) absolute Cut 1
				update idletasks
				createHandles
				cutCopySelected Copy Imp_Selected 1
				set Global(Cut) $Global(Copy)
				%W configure -cursor ""
			}
		}
	}

	bind $Global(CurrentCanvas) <Motion> {
		raise %W
		focus %W
		if {$Global(Grid)} {
			set x [%W canvasx %x $Global(GridX)]
		} else {
			set x [%W canvasx %x 1]
		}
		if {$Global(Grid)} {
			set y [%W canvasy %y $Global(GridY)]
		} else {
			set y [%W canvasy %y 1]
		}
		set Global(Xpos) \
			"[format {%%3.3f} [expr $x / $Global(CoordFactorX)]]$Global(UnitX)"
		set Global(Ypos) \
			"[format {%%3.3f} [expr $y / $Global(CoordFactorY)]]$Global(UnitY)"
		$Global(CurrentCanvas).rulerx coords h \
			$x [$Global(CurrentCanvas) cget -height] $x 0
		$Global(CurrentCanvas).rulery coords v \
			0 $y [$Global(CurrentCanvas) cget -width] $y

		switch $Global(Cursor) {
			Polygon -
			Line {
				switch $Global(Sub-Cursor) {
					Polygon-Active -
					Line-Active {
						%W delete selector
						%W create line $Global(LastX) $Global(LastY) $x $y \
							-tags selector
					}
				}
			}
		}
	}

	bind $Global(CurrentCanvas) <Control-B1-Motion> {
		raise %W
		focus %W
		if {$Global(Grid)} {
			set x [%W canvasx %x $Global(GridX)]
		} else {
			set x [%W canvasx %x 1]
		}
		if {$Global(Grid)} {
			set y [%W canvasy %y $Global(GridY)]
		} else {
			set y [%W canvasy %y 1]
		}
		set Global(Xpos) \
			"[format {%%3.3f} [expr $x / $Global(CoordFactorX)]]$Global(UnitX)"
		set Global(Ypos) \
			"[format {%%3.3f} [expr $y / $Global(CoordFactorY)]]$Global(UnitY)"
		$Global(CurrentCanvas).rulerx coords h \
			$x [$Global(CurrentCanvas) cget -height] $x 0 
		$Global(CurrentCanvas).rulery coords v \
			0 $y [$Global(CurrentCanvas) cget -width] $y 

		switch $Global(Cursor) {
			Scale {
				set cursor [lindex [%W configure -cursor] 4]
				scaleSelected $cursor $x $y %X %Y
			}
			Select {
				dragBoxOrMove $item $tags $x $y %X %Y
			}
		}
	}
	bind $Global(CurrentCanvas) <B1-Motion> {
		focus %W
		if {$Global(Grid)} {
			set x [%W canvasx %x $Global(GridX)]
		} else {
			set x [%W canvasx %x 1]
		}
		if {$Global(Grid)} {
			set y [%W canvasy %y $Global(GridY)]
		} else {
			set y [%W canvasy %y 1]
		}
		set Global(Xpos) \
			"[format {%%3.3f} [expr $x / $Global(CoordFactorX)]]$Global(UnitX)"
		set Global(Ypos) \
			"[format {%%3.3f} [expr $y / $Global(CoordFactorY)]]$Global(UnitY)"
		$Global(CurrentCanvas).rulerx coords h \
			$x [$Global(CurrentCanvas) cget -height] $x 0 
		$Global(CurrentCanvas).rulery coords v \
			0 $y [$Global(CurrentCanvas) cget -width] $y 

		switch $Global(Cursor) {
			Scale {
				set cursor [lindex [%W configure -cursor] 4]
				scaleSelected $cursor $x $y %X %Y
			}
			Select {
				dragBoxOrMove $item $tags $x $y %X %Y
			}
			Draw {
				%W create line $Global(LastX) $Global(LastY) $x $y \
					-tags Global(curline)
				repositionWindow %X %Y
				lappend Global(curline) $x $y
				set Global(LastX) $x
				set Global(LastY) $y
			}
			Oval {
				%W delete selector
				%W create oval $Global(LastX) $Global(LastY) $x $y \
					-tags selector
				repositionWindow %X %Y
			}
			Rounded_Rectangle {
				%W delete selector
				%W create rectangle $Global(LastX) $Global(LastY) $x $y \
					-tags selector
				repositionWindow %X %Y
			}
			Rectangle {
				%W delete selector
				%W create rectangle $Global(LastX) $Global(LastY) $x $y \
					-tags selector
				repositionWindow %X %Y
			}
			Text {
				%W delete selector
				%W create rectangle $Global(LastX) $Global(LastY) $x $y \
					-tags selector
				repositionWindow %X %Y
			}
			Rotate -
			Polygon -
			Line {
				switch $Global(Sub-Cursor) {
					Polygon-Active -
					Line-Active {
						%W delete selector
						%W create line $Global(LastX) $Global(LastY) $x $y \
							-tags selector
						repositionWindow %X %Y
					}
					Rotate-Active {
						%W delete selector
						%W create line $Global(LastX) $Global(LastY) $x $y \
							-arrow last -tags selector
						set xnorm [eval expr $x - $Global(LastX)]
						set ynorm [eval expr $y - $Global(LastY)]
						set rho [eval expr sqrt(pow(($x - $Global(LastX)),2) +\
							 pow(($y - $Global(LastY)),2))]
						if {$rho != 0} {
							set angle [eval expr asin($ynorm/$rho)]
							set degrees [eval expr \
								$angle*180.0/3.14159265358979323846]
							if {$xnorm >= 0} {
								if {$ynorm >= 0} {
									set degrees [eval expr abs($degrees)]
								} else {
									set degrees \
										[eval expr 360.0 - abs($degrees)]
								}
							} else {
								if {$ynorm >= 0} {
									set degrees \
										[eval expr 180.0 - abs($degrees)]
								} else {
									set degrees \
										[eval expr 180.0 + abs($degrees)]
								}
							}
							%W create arc [eval expr $Global(LastX) - 20] \
								[eval expr $Global(LastY) - 20] \
								[eval expr $Global(LastX) + 20] \
								[eval expr $Global(LastY) + 20] \
								-start 0 -extent [eval expr 0 - $degrees] \
								-stipple gray50 -outline black -fill white \
								-tags selector
							set coords [%W coords selectorbox]
							%W delete selectorbox
							eval %W create polygon \
								[eval RotateF [eval expr $degrees - \
								$Global(LastAngle)] \
								$Global(LastX) $Global(LastY) $coords] \
								-tags selectorbox \
								-stipple gray25 -outline \"\" -fill black
							set Global(LastAngle) $degrees
							set Global(Message) $degrees
						}
					}
				}
			}
		}
	}
	}
	pack $Global(CurrentCanvas) -in $cf.top -side left -expand true \
		-fill both
	changePageProp $Global(PageWidth) $Global(PageHeight) $Global(Landscape)

	return $Global(CurrentCanvas)
}

proc nextPage {} {
	global Global

	set Global(CurrentPageId) [expr $Global(CurrentPageId) + 1]
	newCanvas .can c$Global(CurrentPageId)
	update idletasks
}

proc prevPage {} {
	global Global

	if {$Global(CurrentPageId) > 1} {
		set Global(CurrentPageId) [expr $Global(CurrentPageId) - 1]
		newCanvas .can c$Global(CurrentPageId)
		update idletasks
	}
}

proc createCanvasF { name } {
	global Global

	frame $name -bd 2 -relief raised
	regexp {.(.*)} "$name" match tname
	frame $name.top -bd 0 -highlightthickness 0
	frame $name.bottom -bd 0 -highlightthickness 0
	frame $name.corner -height 20 -width 20 -bd 2
	scrollbar $name.scrolly -bd 2 -width 15 \
		-command {eval $Global(CurrentCanvas) yview} 
	scrollbar $name.scrollx -bd 2 -width 15 -orient horiz \
		-command {eval $Global(CurrentCanvas) xview}
	frame .right
	if {$Global(Plugin)} {
		frame .radios -relief raised -bd 2
	}
	frame .colors -relief raised -bd 2
	frame .colorsp
	frame .colorssel
	frame .colorseltop
	frame .colorselbottom
	frame .f_Fill -width 20 -height 20 -bd 1 -relief sunken
	frame .f_Outline -bg black -width 20 -height 20 -bd 1 -relief sunken
	scale .lw -from 1.0 -to 100.0 -bd 1 -orient h \
		-showvalue false -variable Global(Width)
	bind .lw <Enter> {
		set Global(MessageSave) $Global(Message)
		set Global(Message) "Line Width"
	}
	bind .lw <Leave> {
		set Global(Message) $Global(MessageSave)
	}
	label .llw -width 3 -textvariable Global(Width)
	newCanvas .can c$Global(CurrentPageId)
	button .nextpage -text "Next" -pady 1 -command "nextPage"
	button .prevpage -text "Prev" -pady 1 -command "prevPage"
	radiobutton .s_Color -value Fill -text "Fill" \
		-variable Global(colortype) \
		-anchor sw -indicatoron false -selectcolor ""
	radiobutton .s_Outline -value Outline -text "Outline" \
		-variable Global(colortype) \
		-anchor sw -indicatoron false -selectcolor ""
	if {!$Global(Plugin)} {
		bind .s_Color <Double-Button-1> {
			colorPaletteReq .colorpalette \
				{0000 3300 6600 9900 CC00 FF00} \
				{0000 3300 6600 9900 CC00 FF00} \
				{0000 3300 6600 9900 CC00 FF00} \
				.colorsp
		}
		bind .s_Outline <Double-Button-1> {
			colorPaletteReq .colorpalette \
				{0000 3300 6600 9900 CC00 FF00} \
				{0000 3300 6600 9900 CC00 FF00} \
				{0000 3300 6600 9900 CC00 FF00} \
				.colorsp
		}
	}
	label .xpos -textvariable Global(Xpos) -height 1 -width 10
	label .ypos -textvariable Global(Ypos) -height 1 -width 10
	label .msg -relief flat -textvariable Global(Message) -height 1 -width 20
	pack .f_Fill .s_Color -side left -in .colorseltop
	pack .f_Outline .s_Outline -side left -in .colorselbottom
	pack .colorseltop .colorselbottom -side left -anchor sw -in .colorssel \
		-fill x
	pack .colorssel -in .colors -side left
	pack .xpos .ypos -in .colors -side left
	pack .msg -in .colors -side right -fill x -expand true
	colorPaletteReq .colorpalette \
				{0000 3300 6600 9900 CC00 FF00} \
				{0000 3300 6600 9900 CC00 FF00} \
				{0000 3300 6600 9900 CC00 FF00} \
		.colorsp
	if {!$Global(Plugin)} {
		wm withdraw .colorpalette
	}

	frame .radiosr
	frame .rotatef
	entry .rrotatee -width 6 -textvariable Global(LastAngle) -relief flat
	label .copyright -text "ImPress .04alpha, Copyright © 1996,1997,1998 Christopher Jay Cox, All Rights Reserved." -relief raised

	buttonReq .buttons $Global(Buttons) .radiosr
	frame .width -relief raised -bd 1
	canvas .widthc -relief flat -height 8m -width 2.25i
	.widthc create text 2 2 -anchor nw -text "Line Width"
	.widthc create line 1i .14i 1.5i .14i 1.5i .21i 2i .21i \
			-tags line \
			-fill "$Global(Outline)" -stipple "$Global(Stipple)" \
			-arrow "$Global(Arrow)" -joinstyle "$Global(JoinStyle)" \
			-width "$Global(Width)"
		
	frame .space -width .25i
	frame .space2 -width .25i

	frame .textframe -bd 1 -relief raised
	frame .textmid
	text .text -exportselection true -width 20 -height 10 -wrap word \
		-yscrollcommand ".tscrolly set"
	scrollbar .tscrolly -bd 2 -width 15 -command ".text yview" 
	button .tclose -text "Close" -padx 2m -pady 2m \
		-command "pack forget .textframe;pack $name -fill both -expand true"
	button .tcopy -text "Copy" -padx 2m -pady 2m \
		-command ".text tag add sel 1.0 end"
	pack .text -in .textmid -side left -anchor sw -fill both -expand true
	pack .tscrolly -in .textmid -side right -fill y -anchor se
	pack .textmid -in .textframe -side top -anchor sw -fill both -expand true
	pack .tcopy .tclose -padx 1m -pady 1m -in .textframe -side left

	pack $name.scrolly -in $name.top -anchor se -side right -fill y
	pack .nextpage .prevpage -anchor sw -side left -in $name.bottom
	pack $name.scrollx -anchor se -in $name.bottom -side left -expand true \
		-fill x 
	pack $name.corner -anchor ne -in $name.bottom -side right
	pack .copyright -anchor sw -fill x
	pack .lw .llw -in .colors -side left
	pack .colors -fill x -in $name -side top
	pack $name.top -side top -in $name -expand true -fill both
	pack $name.bottom -anchor sw -in $name -side top -fill x

	if {$Global(Plugin)} {
		pack .radiosr -side left -in .radios -anchor sw 
		pack .colorsp -side left -in .radios -anchor sw -fill both -expand true
		pack .radios -anchor sw -fill x
	}

	pack $name -expand true -fill both 
	changePageProp $Global(PageWidth) $Global(PageHeight) $Global(Landscape)
	focus $Global(CurrentCanvas)

	return $name.c
}


proc doProperties { c item } {
	set type [$c type $item]
	switch "$type" {
		line {
		}
	}
}
proc getProperties { c item type nogroups} {
	if { "$item" != "" } {
		set gprops ""
		foreach prop [$c itemconfigure $item] {
			set gpropname [lindex $prop 0]
			set gpropval [lindex $prop 4]
			if {$nogroups && $gpropname == "-tags"} {
				set ntags ""
				foreach tag $gpropval {
					if {![string match "Group*" $tag]} {
						lappend ntags $tag
					}
				}
				set gpropval $ntags
			}
			if {"$type" == "Esc"} {
				lappend gprops $gpropname "{$gpropval}"
			} else {
				lappend gprops $gpropname $gpropval
			}
		}
		return $gprops
	}
}
proc getGroupItems { items } {
	global Group Global

	set l ""
	foreach item $items {
		if {[info exists Group($Global(CurrentCanvas),$item)]} {
			set l [concat $l [$Global(CurrentCanvas) find withtag \
				[lindex $Group($Global(CurrentCanvas),$item) 0]]]
		}
	}
	return [lsort -integer [concat $items $l]]
}

proc createOval {bb} {
	global Global

	set r1 [expr ([lindex $bb 2] - [lindex $bb 0])/2.0]
	set r2 [expr ([lindex $bb 3] - [lindex $bb 1])/2.0]
	set xc [expr [lindex $bb 0] + $r1]
	set yc [expr [lindex $bb 1] + $r2]

	for {set r 0} {$r < 360} {incr r 10} {
		set sr [expr sin($r*3.14159265358979323846/180.0)]
		set cr [expr cos($r*3.14159265358979323846/180.0)]
		lappend shape [expr $xc + $r1 * $cr] [expr $yc + $r2 * $sr]
	}
	set i [eval $Global(CurrentCanvas) create polygon $shape \
		-tags oval -smooth 1 -fill \"$Global(Fill)\" \
		-outline \"$Global(Outline)\" -stipple \"$Global(Stipple)\" \
		-width \"$Global(Width)\"]
	return $i
}

proc createEditText {c textitem} {
	if {"$textitem" != ""} {
	}
}
proc createText {boxselector} {
	global Global

	textRaise .textIn
	set x [lindex $boxselector 0]
	set y [lindex $boxselector 3]
	set Global(TextWidth) [expr [lindex $boxselector 2] - $x]
	if {$Global(Outline) == ""} {
		set fill ""
	} else {
		set fill "-fill {$Global(Outline)}"
	}
	set Global(Text) [eval \$Global(CurrentCanvas) create text $x $y \
		-anchor sw -justify left \
		-font \"\$Global(Font)\" \
		$fill \
		-stipple \"\$Global(Stipple)\" -width \"\$Global(TextWidth)\" \
		-text \" \" -tags Imp_Text]
}
proc createRRectangle {boxselector smooth} {
	global Global
	set bb [bboxToCorners $boxselector]
	set xpix [winfo pixels . .25i]
	set ypix $xpix

	set x1 [lindex $bb 0]
	set y1 [lindex $bb 1]
	set x2 [lindex $bb 2]
	set y2 [lindex $bb 3]
	set x3 [lindex $bb 4]
	set y3 [lindex $bb 5]
	set x4 [lindex $bb 6]
	set y4 [lindex $bb 7]
	if {[expr $x2 - $x1] < [expr 2 * $xpix]} {
		set xpix [expr ($x2 - $x1) * .4]
	}
	if {[expr $y3 - $y1] < [expr 2 * $ypix]} {
		set ypix [expr ($y3 - $y1) * .4]
	} 
	if {$xpix > $ypix} {
		set xpix $ypix
	} else {
		set ypix $xpix
	}
	lappend nb $x1 $y1 [expr $x1 + $xpix] $y1 [expr $x1 + $xpix] $y1 \
		[expr $x2 - $xpix] $y2 [expr $x2 - $xpix] $y2 $x2 $y2 \
		$x2 [expr $y2 + $ypix] $x2 [expr $y2 + $ypix] \
		$x3 [expr $y3 - $ypix] $x3 [expr $y3 - $ypix] \
		$x3 $y3 [expr $x3 - $xpix] $y3 [expr $x3 - $xpix] $y3 \
		[expr $x4 + $xpix] $y4 [expr $x4 + $xpix] $y4 $x4 $y4 \
		$x4 [expr $y4 - $ypix] $x4 [expr $y4 - $ypix] \
		$x1 [expr $y1 + $ypix] $x1 [expr $y1 + $ypix]
	eval $Global(CurrentCanvas) create polygon $nb \
		-smooth $smooth -fill \"$Global(Fill)\" -outline \"$Global(Outline)\" \
		-stipple \"$Global(Stipple)\" -width \"$Global(Width)\"
}
proc createRectangle {boxselector} {
	global Global
	eval $Global(CurrentCanvas) create polygon [bboxToCorners $boxselector] \
		-fill \"$Global(Fill)\" -outline \"$Global(Outline)\" \
		-stipple \"$Global(Stipple)\" -width \"$Global(Width)\"
}
proc createLine {coords} {
	global Global
	if {[llength $coords] > 2} {
		return [eval $Global(CurrentCanvas) create line $coords \
			-fill \"$Global(Outline)\" -stipple \"$Global(Stipple)\" \
			-arrow \"$Global(Arrow)\" -joinstyle \"$Global(JoinStyle)\" \
			-width \"$Global(Width)\" -smooth \"$Global(Smooth)\"] 
	}
}
proc createSelection {} {
	global Global(selectnum)

	#
	# Select if something tagged within box.
	#
	$Global(CurrentCanvas) dtag disabled Imp_selected
	$Global(CurrentCanvas) dtag disabled Imp_selected${Global(selectnum)}

	for {set i 0} {1} {incr i} {
		set bb [$Global(CurrentCanvas) bbox Imp_selected${i}]
		if {"$bb" == ""} {
			break
		}
		eval $Global(CurrentCanvas) create polygon \
			[bboxToCorners "[$Global(CurrentCanvas) bbox Imp_selected${i}]"] \
			-tags {"selectorbox"} \
			-stipple gray25 -outline {""} -fill black
	}
}
proc createPolygon {coords} {
	global Global

	if {[llength $coords] > 5} {
		return [eval $Global(CurrentCanvas) create polygon $coords \
			-fill \"$Global(Fill)\" -outline \"$Global(Outline)\" \
			-stipple \"$Global(Stipple)\" \
			-smooth \"$Global(Smooth)\" \
			-width \"$Global(Width)\"]
	}
}
proc createdBoundedSelection {boxselector} {
	global Global(selectnum)

	$Global(CurrentCanvas) configure -cursor watch
	update idletasks	
	#
	# If a boxselector is present, find everything it encompasses.
	#
	set bb [$Global(CurrentCanvas) bbox $boxselector]
	$Global(CurrentCanvas) delete selector
	set itemList [eval $Global(CurrentCanvas) find enclosed $bb]
	$Global(CurrentCanvas) dtag selector

	set itemList [getGroupItems $itemList]
	foreach item $itemList {
		$Global(CurrentCanvas) addtag Imp_selected withtag $item
		$Global(CurrentCanvas) addtag Imp_selected${Global(selectnum)} withtag $item
	}
	createSelection
	incr Global(selectnum)
	update idletasks
	. configure -cursor ""
	update idletasks
}
proc readFile {c name} {
	global Global

	set f [open $name r]
	gets $f line
	gets $f line
	gets $f line
	gets $f line
	set art [read $f]
	eval $art
}

proc getScale {name width height} {
	global Global

	set bb [eval $Global(CurrentCanvas) bbox $name]
	set x [expr [lindex $bb 2] - [lindex $bb 0]]
	set y [expr [lindex $bb 3] - [lindex $bb 1]]
	if {$x > $y} {
		set scale [expr $width / $x]
	} else {
		set scale [expr $height / $y]
	}
	return $scale
}
proc scaleObject {name ratio width height} {
	global Global

	set bb [eval $Global(CurrentCanvas) bbox $name]
	if {"$bb" != ""} {
		set x [lindex $bb 2]
		set y [lindex $bb 3]
		if {$ratio == "variable"} {
			set scalex [expr ($width + 0.0) / $x]
			set scaley [expr ($height + 0.0) / $y]
			if {$scalex > $scaley} {
				set scale $scaley
			} else {
				set scale $scalex
			}
		} elseif {$ratio != ""} {
			set scalex $ratio
			set scaley $ratio
			set scale $ratio
		} else {
			set scalex [expr ($width + 0.0) / $x]
			set scaley [expr ($height + 0.0) / $y]
			if {$scalex > $scaley} {
				set scalex $scaley
				set scale $scaley
			} else {
				set scaley $scalex
				set scale $scalex
			}
		}
		$Global(CurrentCanvas) scale $name 0 0 $scalex $scaley
		foreach i [$Global(CurrentCanvas) find withtag all] {
			set type [$Global(CurrentCanvas) type $i]
			if {"$type" != "text"} {
				continue
			}
			if {$Global(FontScale)} {
				set fn [$Global(CurrentCanvas) itemcget $i -font]
				regexp \
				{([-][^-]*-[^-]*-[^-]*-[^-]*-[^-]*-[^-]*-[^-]*-)([^-]*)(-.*)}\
					$fn dummy d1 size d2;
				if {"$dummy" != ""} {
					set nsize [expr round($size * $scale)]
					if {$nsize < 20} {
						set nsize 20
					}
					$Global(CurrentCanvas) itemconfigure $i \
						-font ${d1}${nsize}${d2}
					set fnn [$Global(CurrentCanvas) itemcget $i -font]
					regexp \
						{([-][^-]*-[^-]*-[^-]*-[^-]*-[^-]*-[^-]*-[^-]*-)([^-]*)(-.*)}\
						$fnn dummy d1 nsize d2;
					if { ($scale < 1 && $nsize < $size ) || \
						 ($scale > 1 && $nsize > $size) } {
						$Global(CurrentCanvas) itemconfigure $i \
							-width [expr [$Global(CurrentCanvas) itemcget $i \
							-width] * $scale]
					} else {
						$Global(CurrentCanvas) itemconfigure $i \
							-font $fn
					}
				}
			} else {
				$Global(CurrentCanvas) itemconfigure $i \
					-width [expr [$Global(CurrentCanvas) itemcget $i -width] \
						* $scale]
			}
		}
	}
}

proc getOrigin { name } {
	global Global

	set bb [eval $Global(CurrentCanvas) bbox $name]
	set x [lindex $bb 0]
	set y [lindex $bb 1]
	if {$x > 0} {
		set nx -$x
	} else {
		set nx [expr abs($x)]
	}
	if {$y > 0} {
		set ny -$y
	} else {
		set ny [expr abs($y)]
	}
	return "$nx $ny"
}

proc moveObject { name x y } {
	global Global

	$Global(CurrentCanvas) move $name $x $y
}

proc flip { name type } {
	global Global

	set bb [$Global(CurrentCanvas) bbox $name]
	if {"$bb" != ""} {
		. configure -cursor watch
		update idletasks
		$Global(CurrentCanvas) delete handles
		if {"$type" == "v"} {
			set d [expr ([lindex $bb 2] - [lindex $bb 0])/2.0 + [lindex $bb 0]]
		} else {
			set d [expr ([lindex $bb 3] - [lindex $bb 1])/2.0 + [lindex $bb 1]]
		}
		set items [$Global(CurrentCanvas) find withtag $name]
		foreach item $items {
			set coords [$Global(CurrentCanvas) coords $item]
			if {"$type" == "v"} {
				set i 1
			} else {
				set i 0
			}
			set newcoords ""
			foreach coord $coords {
				if {$i} {
					lappend newcoords [expr $coord - (2.0 * ($coord - $d))]
					set i 0
				} else {
					lappend newcoords $coord
					set i 1
				}
			}
			eval $Global(CurrentCanvas) coords $item $newcoords
		}
		createHandles
		update idletasks
		. configure -cursor ""
		update idletasks
	}
}

proc originObject { name } {
	global Global

	set bb [$Global(CurrentCanvas) bbox $name]
	set x [lindex $bb 0]
	set y [lindex $bb 1]
	if {$x > 0} {
		set nx -$x
	} else {
		set nx [expr abs($x)]
	}
	if {$y > 0} {
		set ny -$y
	} else {
		set ny [expr abs($y)]
	}
	$Global(CurrentCanvas) move $name $nx $ny
}

set list {
menu Menu {} {}
{
	{
		file File {-underline 0} {}
		{
			{
				new New {-underline 0} { clearAll }
			}
			{
				open Open... {-underline 0} {openFile source Tk}
			}
			{
				saveas "Save As..." {-underline 4} {saveAsFile}
			}
			{
				print Print... {-underline 0} {printRaise .print}
			}
			{
				slide "Slide Properties..." {-underline 0} {slideRaise .slideP}
			}
			{
				pageprop "Page Properties..." {-underline 0} {set pageWidth \$Global(PageWidth);set pageHeight \$Global(PageHeight);pagePropRaise .pageProp}
			}
			{
				sep1 {} {} {}
			}
			{
				exit Exit {-underline 1} {exit}
			}
		}
	}
	{
		insert Insert {-underline 0} {}
		{
			{
				oval Oval {-underline 0} {set Global(MenuSelect) 1;popupCanvasSet Oval}
			}
			{
				rect Rectangle {-underline 0} {set Global(MenuSelect) 1;popupCanvasSet Rectangle}
			}
			{
				line Line {-underline 0} {set Global(MenuSelect) 1;popupCanvasSet Line}
			}
			{
				poly Polygon {-underline 0} {set Global(MenuSelect) 1;popupCanvasSet Polygon}
			}
			{
				draw Draw {-underline 0} {set Global(MenuSelect) 1;popupCanvasSet Draw}
			}
			{
				text Text {-underline 0} {set Global(MenuSelect) 1;popupCanvasSet Text}
			}
		}
	}
	{
		edit Edit {-underline 0} {}
		{
			{
				selectall {Select All} {-underline 0} {selectAll}
			}
			{
				select Select {-underline 0} {popupCanvasSet Select}
			}
			{
				group Group {-underline 0} {popupCanvasSet Group}
			}
			{
				ungroup Ungroup {-underline 0} {popupCanvasSet UnGroup}
			}
			{
				duplicate Duplicate {-underline 0} \
					{popupCanvasSet Duplicate}
			}
			{
				cut Cut {-underline 0} {popupCanvasSet Cut}
			}
			{
				paste Paste {-underline 0} {popupCanvasSet Paste}
			}
		}
	}
	{
		layout Layout {-underline 0} {}
		{
			{
				grid Grid {-underline 0} {set GridX \$Global(GridX);set GridY \$Global(GridY);gridRaise .grid}
			}
			{
				headfoot Header/Footer {-underline 0} {}
			}
			{
				annotate Annotations {-underline 0} {}
			}
		}
	}
	{
		preferences Preferences {-underline 0} {} 
		{
			{
				buttons ToolBar {-underline 0}
				{buttonReq .buttons \$Global(Buttons) .radiosr}
			}
		}
	}
}
}
proc changePageProp { width height orient args} {
	global Global

	set Global(PageWidth) $width
	set Global(PageHeight) $height
	set Global(Landscape) $orient
	set unit ""

	regexp {(.*)(.)$} "$width" match len Global(UnitX)
	regexp {(.*)(.)$} "$height" match len Global(UnitY)

	if { "$Global(UnitX)" == "" } {
		set Global(CoordFactorX) 1
	} else {
		set Global(CoordFactorX) [winfo fpixels . 1$Global(UnitX)]
	}
	if { "$Global(UnitY)" == "" } {
		set Global(CoordFactorY) 1
	} else {
		set Global(CoordFactorY) [winfo fpixels . 1$Global(UnitY)]
	}

	update idletasks
	if {!$Global(Plugin)} {
		set w [winfo width .]
		set h [winfo height .]
		set wp [winfo pixels . $Global(PageWidth)]
		set hp [winfo pixels . $Global(PageHeight)]
		set junkw [expr $w - [winfo width $Global(CurrentCanvas)]]
		set junkh [expr $h - [winfo height $Global(CurrentCanvas)]]
		set nw [expr $wp + $junkw]
		set nh [expr $hp + $junkh]
		wm maxsize .  $nw $nh
		if {$nw < $w || $nh < $h} {
			wm geometry . ${nw}x${nh}
		}
	}
	$Global(CurrentCanvas) configure \
		-scrollregion "0i 0i $Global(PageWidth) $Global(PageHeight)"

	$Global(CurrentCanvas).rulerx configure \
	 	-width $Global(PageWidth)
	doRuler $Global(CurrentCanvas).rulerx $Global(PageWidth) .25i 0 0 1 h \
		"" ""
	$Global(CurrentCanvas).rulery configure \
	 	-height $Global(PageHeight)
	doRuler $Global(CurrentCanvas).rulery $Global(PageHeight) .25i 0 0 1 v \
		"" ""
	set Global($Global(CurrentCanvas)_Width) $Global(PageWidth)
	set Global($Global(CurrentCanvas)_Height) $Global(PageHeight)
	set Global($Global(CurrentCanvas)_Landscape) $Global(Landscape)
	update idletasks
}

set Global(CurrentCanvas) ""
if { ! $Global(Plugin) } {
	set menuparms [createMenu "" menu $list]
	eval tk_menuBar $menuparms
}
set c [createCanvasF ".can"]
setCursor Select

if { [lindex $argv 0] != "" } {
	openfile source [lindex $argv 0]
}

