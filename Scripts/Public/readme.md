# Public MODS scripts

**Updated: 2026 Jan 27**

These are the MODS observing and support scripts that are exposed to
general LBT mountain users (hence "public" on the mountain
workstations (robsN and obsN machines).  They are installed in the
shared `/lbt/lbto/mods/bin` path.

## Obsering Scripts

### `modsTerm`
```
Usage:
  modsTerm [start|attach|stop|kill]

where:
  start or attach - start or (re)attach to a modsTerm session
  stop or kill - stop a user's running modsTerm session
</pre>
`modsTerm` launches a tmux session used to run binocular MODS observing scripts with `acqBinoMODS` or `execBinoMODS`

While it may be invoked by itself, if the session is not running
it will be executed by the acqBinoMODS and execBinoMODS scripts.
The modsTerm tmux session is configured with 3 window panes
<pre>
      +---------------------+---------------------+
      |        MODS1        |        MODS2        |
      |         1.0         |         1.1         |
      |                     |                     |
      |                     |                     |
      |                     |                     |
      |                     |                     |
      |                     |                     |
      |                     |                     |
      |                     |                     |
      +---------------------+---------------------+
      |                  command                  |
      |                    1.2                    |
      +---------------------+---------------------+
</pre>
One `modsTerm` session is allowed at a time.

You can leave the `modsTerm` session running in the
background by typing `detach` in the command pane at
the prompt.

You re-join the detached session by typing `modsTerm`.

To close the `modsTerm` and stop the tmux session and
all programs running in the panes by typing
<pre>
   modsTerm stop
</pre>
this may be typed in the command pane or in any xterm
logged in as the same user on the same machine as it
was started in.

### `acqBinoMODS`
<pre>
Usage:
   acqBinoMODS modsScript.acq
or
   acqBinoMODS mods1Script.acq mods2Script.acq
</pre>
Executes a binocular MODS target acquisition.  acqBinoMODS takes one
or two command-line arguments: the name(s) of acquisition script(s) to
execute in binocular context.

If only one .acq script is given, it is executed on both MODS
instruments ("identical twinning").

If two .acq scripts are given, they are executed on MODS1 and MODS2 in
the order `MODS1 MODS2` ("fraternal twinning").

Must be run in an active `modsTerm` session.

### `execBinoMODS`
<pre>
Usage:
   execBinoMODS mods.obs
or
   execBinoMODS mods1.obs mods2.obs
</pre>
Executes binocular MODS data acquisition or instrument configuration
scripts.  execBinoMODS takes one or two command-line arguments: the
name(s) of observing script(s) to execute in binocular context.

If only one .obs script is given, it is executed on both MODS
instruments ("identical twinning").

If two .obs scripts are given, they are executed on MODS1 and MODS2 in
the order `MODS1 MODS2` ("fraternal twinning").

Must be run in an active `modsTerm` session.

### `binoMODS`
<pre>
Usage:
   binoMODS response
or
   binoMODS [1|2] response
or
   binoMODS halt
</pre>
Binocular MODS target acquisition and science observing scripts are
executed using `acqBinoMODS` and `execBinoMODS` inside a `modsTerm`.

During MODS script execution, users will sometimes be asked
to respond to keyboard prompts, for example
<pre>
   ** Abort, Retry, or Ignore? >
</pre>
or
<pre>
   Wait for WFS collimation to converge, then hit Enter to RESUME
</pre>
The `binoMODS` command will let you respond to these prompts
from the `modsTerm` command pane without having to click on the
active pane and type directly, and also let you respond to prompts
in both panes simultaneously (but with the same response).

For example, to respond to 
<pre>
   Wait for WFS collimation to converge, then hit Enter to RESUME
</pre>
after both MODS1 and MODS2 are ready to resume, you would type
<pre>
   binoMODS enter
</pre>
in the command pane and the two `execMODS` sessions for both MODS
would resume at essentially the same time.

The specail `halt` directive
<pre>
   binoMODS halt
</pre>
sends a Ctrl+C to both scripts (or just one if 1 or 2 is given before halt).
Ctrl+C will immediately halt script execution.

### `modsAlign`

<pre>
Usage: modsAlign [options] maskImage fieldImage [mmsFile]

Where:
    maskImage = thru-mask FITS image
   fieldImage = acquisition field FITS image
     mmsFile  = MODS Mask Specification (.mms) file
Options:
   -y dY long-slit acquisition to dY offset from center in arcsec
   -r    long-slit acquisition to preset LS60x5 center
   -l    fully manual long-slit acquisition
   -v    verbose debugging output (very noisy)
   -B    suppress quick bias subtraction for bright targets
 Advanced Options:
   --info  print version info and exit
   --slit=ID use named slit ID in mmsFile as the acquistion slit
   --sw f  change the star search width factor [default: 1.5]
   --mw f  change the star-to-box matching width factor [default: 2.0]
   --turbo turbo mode, don't ask questions, except at the end
   --maxiter N iterate MOS fit up to N times [default: 1 = don't iterate]
   --reject S  reject S*sigma outliers [default: 4.0] (requires --maxiter > 1)
</pre>
Runs the `modsAlign` program for target acqusition in long-slit and multi-slit masks.

### `modsDisp`

<pre>
Usage: modsDisp [options]

Options:
  -d/--dir PATH     watch for files in PATH [default: /lbt/data/new/]
  -t/--time NUMBER  check the raw data directory every NUMBER seconds [default: 2 sec]
  -h/--help         show help information (this message) and exit.
  -V/--version      show modsDisp version and exit.
  --exp             adjust display settings for "exp" mode.
  --mods1           only watch for MODS1 images.
  --mods2           only watch for MODS2 images.
  --debug           show verbose debugging info.
Notes:
  The default raw-data directory path may be re-defined using the MODS_RAWDIR environment variable.
</pre>
MODS auto display program run during observing to watch the raw data directory and
displays new MODS image as they arrive (within a few seconds).

## Instrument Support Scripts

### `mods1` and `mods2`

<pre>
Usage: mods1 {start|stop} {modsUI|gui}
       mods1 {status}
       mods1 {temps}
</pre>
Used to start/stop the `modsUI` dashboard (aka GUI), get quick status, and
read temperatures for mods1 or mods2

### `modsAdmin`
<pre>
Usage:
   modsAdmin [start|attach|stop|kill]

where:
   start or attach - start new session or (re)attach to modsAdmin session
   stop or kill - stop any running modsAdmin sessions
</pre>
This script launches a tmux session that will be used to remotely
display MODS1/2 system status, and provide a local terminal shell for
support users to issue low-level commands to the MODS instruments via
`isisCmd` or `modsCdm`. The intended user is the LBTO support user.

The tmux session has the usual MODS 3-pane layout:
<pre>
      +---------------------+---------------------+
      |     MODS1 status    |     MODS2 status    |
      |         1.0         |         1.1         |
      |                     |                     |
      |                     |                     |
      |                     |                     |
      |                     |                     |
      |                     |                     |
      |                     |                     |
      |                     |                     |
      +---------------------+---------------------+
      |                  command                  |
      |                    1.2                    |
      +---------------------+---------------------+
</pre>
The status panes above are remotely logged into the MODS instrument
servers and running the `modsStatus` display programs that will
update status every 2-3 seconds.

The command pane across the bottom is logged into the local workstation
where this session was started (e.g., robs1 or robs2), and commands
issued in this pane run on the **local machine**.

If you need to remotely login to a MODS machine proper, you should
do so in a separate terminal (e.g., using the `go_mods` convenience
command).

### `updateSlits`
<pre>
Usage: updateSlits
</pre>
Instructs the MMC servers for each MODS to update the slit mask tables
by reloading their respective 'slitmask.tab' files. This needs to be
done after a slit mask exchange.

`updateSlits` does this for both MODS at the same time. It is harmless
if it is run on an MMC server if the slit mask table hasn't changed.

This is a convenience function meant to be run only by instrument
support scientists and techs responsible for exchanging slit masks.

### `pingMODS1` and `pingMODS2`
<pre>
Usage: pingMODS1
</pre>
Ping all of the MODS1 or MODS2 networked hardware systems (WAGO units and
Comtrol port servers) to verify all are online and communicating.

Used to diagnose problems with a MODS instrument. For example, a WAGO
node not responding could be because power is turned off to that
subsystem.  No response from the IUB WAGO (first one checked) could
mean either network or power to the entire instrument is disabled.

### `pokeMODS1` and `pokeMODS2`
<pre>
Usage: pokeMODS1
</pre>
Does a hardware level status query of every mechanism individually
for the named MODS instrument using the `isisCmd` command.

Used by support personnel to check mechanism integrity.  Error
message (e.g., indication of PWRFLT condition) or timeouts
can be used to suggest corrective actions.

### `imcsTools`
<pre>
Usage: imcsTools {mods1|mods2} {start|stop}
</pre>
Start or stop the MODS IMCS engineering GUIs.  Execution permission
is limited to the `lbto` support user.

Launches the IMCS "radar-screen" and "quadrant counter" GUIs on
the named machine.  Should only be used as needed, since the
GUIs are designed with big fonts so we can read them when
working on the hardware.

