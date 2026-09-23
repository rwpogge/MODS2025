# MODS Ginga Display Notes

**Updated: 2026 Sept 22 [rwp]**

Ginga version 7.4.0

Development Start: 2026 Sept 19 [rwp]

## Overview

Sandbox to test adopting Ginga as our raw image display tool to replace
SAOImage ds9 which is now using SAMP and is not always reliable or the
best choice going forward (has internal dependency on LBTO libraries
for SAMP handling).

Began work on 2026 Sept 19, largely in response to on-going complaints
about latency and mis-display with modsDisp, not being able to look back
at images, or do sensible quick-look (PSF measurement, line plots) with
ds9.

### Reference Docs

 * `Ginga` github: https://github.com/ejeschke/ginga
 * `Ginga` readthedocs: https://ginga.readthedocs.io/en/stable/

## Ginga Startup Notes

Start app with remote control and port defined
```
ginga --modules=RC --rcport=11711
```
Add the image picker (measuring stars) and cuts (plot along lines), and zoom
tools on startup
```
ginga --modules=RC,Zoom --rcport=11711
```
Can add others (Pick, Cuts) but it confuses the initial setup.  Better to
start those as needed.

Change app geometry, add
```
ginga --modules=RC,Zoom --rcport=11711 --geometry=1600x600
```
works pretty well for MODS 8x3K full frame.  Starts with 
Thumbnails at the right, which if you shrink a little
horizontally looks good, and you can navigate the last 10
images displayed with the thumbnails.

### Configuration

Arrangement of the contents of the app and its size are better dealt
with using the `general.cfg` and `layout.json` files in the `~/.ginga`
folder.  This takes some playing, like manipulationg `save_layout` in `general.cfg` to capture the JSON layout file then setting a custom layout
to use (`layout_file = 'modsDisp.json'`) is the best way to ensure
that have a workable layout and simplify the startup commands.

### Remote Ports

Which ports?  Follow convention for B/R using 1/2 in range
```
   MODSxB - 11711
   MODSxR - 11712
```

## Code stubs
 
 * `testGinga.py` - python stub for playing with Ginga and python
 * `imDisp.py` - ipython stub, defines a test `modsDisp(filepath)` function for testing
 * `imDisp2.py` - ipython stub supporting 2 ginga instances

### Color Maps

For distinctive blue/red appearance
```
out = channel.set_color_map('ds9_cool')
out = channel.set_color_map('ds9_b')
```
to reset to grayscale, use `gray` or `gray_r` for inverse

### change display cut levels
```
out = channel.cut_levels(-20,100)
```

### enable initializations for new images
```
out = channel.enable_auto_orient(True)
out = channel.enable_autocenter('on')
out = channel.enable_autocuts('override')
out = channel.enable_autozoom('on')
```
Options for the last 3 are:
 * 'on' - apply to every new image
 * 'off' - never apply
 * 'once' - only to first image in the set, then off
 * 'override' - apply to each image until manual override by user, then 'off'

### restore color algorithm, color map (after user mucks with it)
```
out = channel.set_color_algorithm('linear')
out = channel.restore_contrast()
out = channel.restore_cmap()
out = channel.set_autocut_params('zscale')
```

### chnage the autocut algorithm and its parameters
```
out = channel.set_autocut_params('zscale')
out = channel.set_autocut_params('zscale',contrast=0.1)

out = channel.set_autocut_params('median')
```

Lots more image viewer options: https://ginga.readthedocs.io/en/stable/dev_manual/image_viewer.html


### loading numpy array

```
channel.load_np(filePath.name,hdul[6].data,'fits',dict(hdul[0].header))
```

### Zoom and Header plugins

Add Zoom and Header global plugins to the app, send Zoom
last so it is focused

```
sh = viewer.shell()
out = sh.start_global_plugin('Header')
out = sh.start_global_plugin('Zoom')

```

### Kill the `ginga` app
```
sh = viewer.shell()
out = sh.stop()
```

### Inspiration/examples

 * https://pypeit.readthedocs.io/en/latest/_modules/pypeit/display/display.html


