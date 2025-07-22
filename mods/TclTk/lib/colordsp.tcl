#!/bin/sh
# \
    exec wish4.1 "$0" ${1+"$@"}
##
## Color Demo
## Copyright 1996 University of Oregon.
## Original concept by Spencer Smith
## Rewrite by Jeffrey Hobbs
## July 1996
##

proc set_color {{rgb {}}} {
  global RGB COLOR

  if [string comp {} $rgb] {
    scan $rgb "\#%2x%2x%2x" red green blue
    foreach c {red green blue} { set COLOR($c) [format %d [set $c]] }
  } else {
    set rgb \#[format "%.2X%.2X%.2X" $COLOR(red) $COLOR(green) $COLOR(blue)]
  }
  $COLOR(label) config -bg $rgb
  set COLOR(rgb) $rgb
  update
}
proc set_color_aux args { set_color }

proc color_demo {} {
  global RGB COLOR
  set cols {
    fffafa snow
    f8f8ff {ghost white}
    f5f5f5 {white smoke}
    dcdcdc gainsboro
    fffaf0 {floral white}
    fdf5e6 {old lace}
    faf0e6 linen
    faebd7 {antique white}
    ffefd5 {papaya whip}
    ffebcd {blanched almond}
    ffe4c4 bisque
    ffdab9 {peach puff}
    ffdead {navajo white}
    ffe4b5 moccasin
    fff8dc cornsilk
    fffff0 ivory
    fffacd {lemon chiffon}
    fff5ee seashell
    f0fff0 honeydew
    f5fffa {mint cream}
    f0ffff azure
    f0f8ff {alice blue}
    e6e6fa lavender
    fff0f5 {lavender blush}
    ffe4e1 {misty rose}
    ffffff white
    000000 black
    2f4f4f {dark slate gray}
    696969 {dim gray}
    708090 {slate gray}
    778899 {light slate gray}
    bebebe gray
    d3d3d3 {light grey}
    191970 {midnight blue}
    000080 navy
    6495ed {cornflower blue}
    483d8b {dark slate blue}
    6a5acd {slate blue}
    7b68ee {medium slate blue}
    8470ff {light slate blue}
    0000cd {medium blue}
    4169e1 {royal blue}
    0000ff blue
    1e90ff {dodger blue}
    00bfff {deep sky blue}
    87ceeb {sky blue}
    87cefa {light sky blue}
    4682b4 {steel blue}
    b0c4de {light steel blue}
    add8e6 {light blue}
    b0e0e6 {powder blue}
    afeeee {pale turquoise}
    00ced1 {dark turquoise}
    48d1cc {medium turquoise}
    40e0d0 turquoise
    00ffff cyan
    e0ffff {light cyan}
    5f9ea0 {cadet blue}
    66cdaa {medium aquamarine}
    7fffd4 aquamarine
    006400 {dark green}
    556b2f {dark olive green}
    8fbc8f {dark sea green}
    2e8b57 {sea green}
    3cb371 {medium sea green}
    20b2aa {light sea green}
    98fb98 {pale green}
    00ff7f {spring green}
    7cfc00 {lawn green}
    00ff00 green
    7fff00 chartreuse
    00fa9a {medium spring green}
    adff2f {green yellow}
    32cd32 {lime green}
    9acd32 {yellow green}
    228b22 {forest green}
    6b8e23 {olive drab}
    bdb76b {dark khaki}
    f0e68c khaki
    eee8aa {pale goldenrod}
    fafad2 {light goldenrod yellow}
    ffffe0 {light yellow}
    ffff00 yellow
    ffd700 gold
    eedd82 {light goldenrod}
    daa520 goldenrod
    b8860b {dark goldenrod}
    bc8f8f {rosy brown}
    cd5c5c {indian red}
    8b4513 {saddle brown}
    a0522d sienna
    cd853f peru
    deb887 burlywood
    f5f5dc beige
    f5deb3 wheat
    f4a460 {sandy brown}
    d2b48c tan
    d2691e chocolate
    b22222 firebrick
    a52a2a brown
    e9967a {dark salmon}
    fa8072 salmon
    ffa07a {light salmon}
    ffa500 orange
    ff8c00 {dark orange}
    ff7f50 coral
    f08080 {light coral}
    ff6347 tomato
    ff4500 {orange red}
    ff0000 red
    ff69b4 {hot pink}
    ff1493 {deep pink}
    ffc0cb pink
    ffb6c1 {light pink}
    db7093 {pale violet red}
    b03060 maroon
    c71585 {medium violet red}
    d02090 {violet red}
    ff00ff magenta
    ee82ee violet
    dda0dd plum
    da70d6 orchid
    ba55d3 {medium orchid}
    9932cc {dark orchid}
    9400d3 {dark violet}
    8a2be2 {blue violet}
    a020f0 purple
    9370db {medium purple}
    d8bfd8 thistle
    eee9e9 snow2 cdc9c9 snow3 8b8989 snow4
    eee5de seashell2 cdc5bf seashell3 8b8682 seashell4
    ffefdb AntiqueWhite1 eedfcc AntiqueWhite2
    cdc0b0 AntiqueWhite3 8b8378 AntiqueWhite4
    eed5b7 bisque2 cdb79e bisque3 8b7d6b bisque4
    eecbad PeachPuff2 cdaf95 PeachPuff3 8b7765 PeachPuff4
    eecfa1 NavajoWhite2 cdb38b NavajoWhite3 8b795e NavajoWhite4
    eee9bf LemonChiffon2 cdc9a5 LemonChiffon3 8b8970 LemonChiffon4
    eee8cd cornsilk2 cdc8b1 cornsilk3 8b8878 cornsilk4
    eeeee0 ivory2 cdcdc1 ivory3 8b8b83 ivory4
    e0eee0 honeydew2 c1cdc1 honeydew3 838b83 honeydew4
    eee0e5 LavenderBlush2 cdc1c5 LavenderBlush3 8b8386 LavenderBlush4
    eed5d2 MistyRose2 cdb7b5 MistyRose3 8b7d7b MistyRose4
    e0eeee azure2 c1cdcd azure3 838b8b azure4
    836fff SlateBlue1 7a67ee SlateBlue2 6959cd SlateBlue3 473c8b SlateBlue4
    4876ff RoyalBlue1 436eee RoyalBlue2 3a5fcd RoyalBlue3 27408b RoyalBlue4
    0000ee blue2 00008b blue4
    1c86ee DodgerBlue2 1874cd DodgerBlue3 104e8b DodgerBlue4
    63b8ff SteelBlue1 5cacee SteelBlue2 4f94cd SteelBlue3 36648b SteelBlue4
    00b2ee DeepSkyBlue2 009acd DeepSkyBlue3 00688b DeepSkyBlue4
    87ceff SkyBlue1 7ec0ee SkyBlue2 6ca6cd SkyBlue3 4a708b SkyBlue4
    b0e2ff LightSkyBlue1 a4d3ee LightSkyBlue2
    8db6cd LightSkyBlue3 607b8b LightSkyBlue4
    c6e2ff SlateGray1 b9d3ee SlateGray2 9fb6cd SlateGray3 6c7b8b SlateGray4
    cae1ff LightSteelBlue1 bcd2ee LightSteelBlue2
    a2b5cd LightSteelBlue3 6e7b8b LightSteelBlue4
    bfefff LightBlue1 b2dfee LightBlue2 9ac0cd LightBlue3 68838b LightBlue4
    d1eeee LightCyan2 b4cdcd LightCyan3 7a8b8b LightCyan4
    bbffff PaleTurquoise1 aeeeee PaleTurquoise2
    96cdcd PaleTurquoise3 668b8b PaleTurquoise4
    98f5ff CadetBlue1 8ee5ee CadetBlue2 7ac5cd CadetBlue3 53868b CadetBlue4
    00f5ff turquoise1 00e5ee turquoise2 00c5cd turquoise3 00868b turquoise4
    00eeee cyan2 00cdcd cyan3 008b8b cyan4
    97ffff DarkSlateGray1 8deeee DarkSlateGray2
    79cdcd DarkSlateGray3 528b8b DarkSlateGray4
    76eec6 aquamarine2 458b74 aquamarine4
    c1ffc1 DarkSeaGreen1 b4eeb4 DarkSeaGreen2
    9bcd9b DarkSeaGreen3 698b69 DarkSeaGreen4
    54ff9f SeaGreen1 4eee94 SeaGreen2 43cd80 SeaGreen3
    9aff9a PaleGreen1 90ee90 PaleGreen2 7ccd7c PaleGreen3 548b54 PaleGreen4
    00ee76 SpringGreen2 00cd66 SpringGreen3 008b45 SpringGreen4
    00ee00 green2 00cd00 green3 008b00 green4
    76ee00 chartreuse2 66cd00 chartreuse3 458b00 chartreuse4
    c0ff3e OliveDrab1 b3ee3a OliveDrab2 698b22 OliveDrab4
    caff70 DarkOliveGreen1 bcee68 DarkOliveGreen2
    a2cd5a DarkOliveGreen3 6e8b3d DarkOliveGreen4
    fff68f khaki1 eee685 khaki2 cdc673 khaki3 8b864e khaki4
    ffec8b LightGoldenrod1 eedc82 LightGoldenrod2
    cdbe70 LightGoldenrod3 8b814c LightGoldenrod4
    eeeed1 LightYellow2 cdcdb4 LightYellow3 8b8b7a LightYellow4
    eeee00 yellow2 cdcd00 yellow3 8b8b00 yellow4
    eec900 gold2 cdad00 gold3 8b7500 gold4
    ffc125 goldenrod1 eeb422 goldenrod2 cd9b1d goldenrod3 8b6914 goldenrod4
    ffb90f DarkGoldenrod1 eead0e DarkGoldenrod2
    cd950c DarkGoldenrod3 8b6508 DarkGoldenrod4
    ffc1c1 RosyBrown1 eeb4b4 RosyBrown2 cd9b9b RosyBrown3 8b6969 RosyBrown4
    ff6a6a IndianRed1 ee6363 IndianRed2 cd5555 IndianRed3 8b3a3a IndianRed4
    ff8247 sienna1 ee7942 sienna2 cd6839 sienna3 8b4726 sienna4
    ffd39b burlywood1 eec591 burlywood2 cdaa7d burlywood3 8b7355 burlywood4
    ffe7ba wheat1 eed8ae wheat2 cdba96 wheat3 8b7e66 wheat4
    ffa54f tan1 ee9a49 tan2 8b5a2b tan4
    ff7f24 chocolate1 ee7621 chocolate2 cd661d chocolate3
    ff3030 firebrick1 ee2c2c firebrick2 cd2626 firebrick3 8b1a1a firebrick4
    ff4040 brown1 ee3b3b brown2 cd3333 brown3 8b2323 brown4
    ff8c69 salmon1 ee8262 salmon2 cd7054 salmon3 8b4c39 salmon4
    ee9572 LightSalmon2 cd8162 LightSalmon3 8b5742 LightSalmon4
    ee9a00 orange2 cd8500 orange3 8b5a00 orange4
    ff7f00 DarkOrange1 ee7600 DarkOrange2
    cd6600 DarkOrange3 8b4500 DarkOrange4
    ff7256 coral1 ee6a50 coral2 cd5b45 coral3 8b3e2f coral4
    ee5c42 tomato2 cd4f39 tomato3 8b3626 tomato4
    ee4000 OrangeRed2 cd3700 OrangeRed3 8b2500 OrangeRed4
    ee0000 red2 cd0000 red3 8b0000 red4
    ee1289 DeepPink2 cd1076 DeepPink3 8b0a50 DeepPink4
    ff6eb4 HotPink1 ee6aa7 HotPink2 cd6090 HotPink3 8b3a62 HotPink4
    ffb5c5 pink1 eea9b8 pink2 cd919e pink3 8b636c pink4
    ffaeb9 LightPink1 eea2ad LightPink2
    cd8c95 LightPink3 8b5f65 LightPink4
    ff82ab PaleVioletRed1 ee799f PaleVioletRed2
    cd6889 PaleVioletRed3 8b475d PaleVioletRed4
    ff34b3 maroon1 ee30a7 maroon2 cd2990 maroon3 8b1c62 maroon4
    ff3e96 VioletRed1 ee3a8c VioletRed2
    cd3278 VioletRed3 8b2252 VioletRed4
    ee00ee magenta2 cd00cd magenta3 8b008b magenta4
    ff83fa orchid1 ee7ae9 orchid2 cd69c9 orchid3 8b4789 orchid4
    ffbbff plum1 eeaeee plum2 cd96cd plum3 8b668b plum4
    e066ff MediumOrchid1 d15fee MediumOrchid2
    b452cd MediumOrchid3 7a378b MediumOrchid4
    bf3eff DarkOrchid1 b23aee DarkOrchid2
    9a32cd DarkOrchid3 68228b DarkOrchid4
    9b30ff purple1 912cee purple2 7d26cd purple3 551a8b purple4
    ab82ff MediumPurple1 9f79ee MediumPurple2
    8968cd MediumPurple3 5d478b MediumPurple4
    ffe1ff thistle1 eed2ee thistle2 cdb5cd thistle3 8b7b8b thistle4
    030303 gray1 050505 gray2 080808 gray3 0a0a0a gray4 0d0d0d gray5
    0f0f0f gray6 121212 gray7 141414 gray8 171717 gray9 1a1a1a gray10
    1c1c1c gray11 1f1f1f gray12 212121 gray13 242424 gray14 262626 gray15
    292929 gray16 2b2b2b gray17 2e2e2e gray18 303030 gray19 333333 gray20
    363636 gray21 383838 gray22 3b3b3b gray23 3d3d3d gray24 404040 gray25
    424242 gray26 454545 gray27 474747 gray28 4a4a4a gray29 4d4d4d gray30
    4f4f4f gray31 525252 gray32 545454 gray33 575757 gray34 595959 gray35
    5c5c5c gray36 5e5e5e gray37 616161 gray38 636363 gray39 666666 gray40
    6b6b6b gray42 6e6e6e gray43 707070 gray44 737373 gray45 757575 gray46
    787878 gray47 7a7a7a gray48 7d7d7d gray49 7f7f7f gray50 828282 gray51
    858585 gray52 878787 gray53 8a8a8a gray54 8c8c8c gray55 8f8f8f gray56
    919191 gray57 949494 gray58 969696 gray59 999999 gray60 9c9c9c gray61
    9e9e9e gray62 a1a1a1 gray63 a3a3a3 gray64 a6a6a6 gray65 a8a8a8 gray66
    ababab gray67 adadad gray68 b0b0b0 gray69 b3b3b3 gray70 b5b5b5 gray71
    b8b8b8 gray72 bababa gray73 bdbdbd gray74 bfbfbf gray75 c2c2c2 gray76
    c4c4c4 gray77 c7c7c7 gray78 c9c9c9 gray79 cccccc gray80 cfcfcf gray81
    d1d1d1 gray82 d4d4d4 gray83 d6d6d6 gray84 d9d9d9 gray85 dbdbdb gray86
    dedede gray87 e0e0e0 gray88 e3e3e3 gray89 e5e5e5 gray90 e8e8e8 gray91
    ebebeb gray92 ededed gray93 f0f0f0 gray94 f2f2f2 gray95 f7f7f7 gray97
    fafafa gray98 fcfcfc gray99
  }
  array set RGB $cols

  pack [frame .top] -fill both -expand 1
  pack [frame .top.left] -side left -fill both
  pack [frame .top.right] -side left -fill both -expand 1
  set arg {-from 0 -to 255 -showvalue 1 -orient horizontal}
  foreach i {red green blue} {
    set COLOR($i) 0
    eval scale .top.left.$i $arg -var COLOR($i) -troughcolor $i \
	-label [string toup $i]: -command set_color_aux
    pack .top.left.$i -fill y -expand 1
  }

  pack [set COLOR(label) [label .top.right.color]] -fill both -expand 1
  pack [entry .top.right.cname -width 7 -textvar COLOR(rgb) -state disabled]

  pack [frame .c -relief ridge -bd 2] -fill both -expand 1
  set canvas [canvas .c.canvas -yscrollcommand ".c.sy set" \
		  -width 200 -height 400 -relief raised -bd 2]
  scrollbar .c.sy -orient vert -command "$canvas yview" -bd 1
  pack .c.sy -side right -fill y
  pack $canvas -fill both -expand 1

  set mark 0
  foreach {i j} $cols {
    scan $i "%2x%2x%2x" r g b
    if {($r+$g+$b)<350} { set col white } else { set col black }

    set i \#$i
    $canvas create rect 0 $mark 400 [incr mark 30] \
	-fill $i -outline {} -tags $i
    $canvas create text 100 [expr $mark-15] -text $j -fill $col -tags $i
    $canvas bind $i <ButtonPress-1> "set_color $i"
  }
  $canvas config -scrollregion "0 0 200 $mark"
  focus .c.sy
  return
}

color_demo
