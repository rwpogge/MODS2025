# MODS MicroLYNX Microcode (aka PLC)

MODS mechanisms use Intelligent Motion System MicroLYNX 4/7 high-performance
programmable microstepping drives. Since the MODS were built c.2010, IMS has 
become part of Schneider Electric and reorganized as Novanta IMS.  The
MicroLYNX are now a legacy product but we have sufficient spares to keep the 
MODS running for ~20-25 years.  

## MicroLYNX PLC programming

 * How do I start writting a program for a MicroLYNX PLC?
 * How do I write microcode for the PLC?
 * What commands are acceptable by this PLC?
 * What format are commands in and what are they?

### How do I start writting a program for a MicroLYNX PLC?

First make sure you start by looking at the template.plc in this
directory. Any microcode for the PLC will be after the template.plc
header.
```shell
 cp template.plc <plc_name>.plc
```

### How do I write microcode for the PLC?

Use the `template.plc` to start with and maybe look at one that is in use as an example.)

Now that you have sucessfully copied the template.plc to a name of your choice. Start writting code.
PLC Programs usally start with: Start at address >= 100:
```shell
PGM 100
LBL XXX
.
your
code
.
END
PGM  
```
For ERRORS use a '?' with the PRINT statement.
```shell
PRINT "? Something is in ERROR"
PRINT "? Not in position POSITION=",pos
```

### What commands are acceptable by this controller?

#### MicroLYXN-7 Commands
```shell
ACCL=n		Peak acceleration value, mm/sec2
ACLT=n		Acceleration type
DCLT=n		Deceleration type
DECL=n		Peak deceleration rate, mm/sec2
SLEW n,m	Slew the motor at a constant velocity, 
		m=0 use acceleration ramp, 
		m=1 do not use acceleration ramp, 
		velocity in mm/sec
SSTP m		Stop the current motion, 
		m=0 stop motion only, 
		m=1 stop motion and program, 
		m omitted is the same as m=0
VI=n		Initial mechanism velocity, mm
VM=n		Maximum velocity of mechanism during a mechanism action, mm
MOVA n,m	Move to an absolute mechanism position, n is position in mm 
		sign of n indicated direction, 
		m=0 with deceleration, 
		m=1 no deceleration, m=0 if omitted
MOVR n,m	Dispense / aspirate, n is volume in mm, 
		sign of n indicates direction, 
		m=0 with deceleration, 
		m=1 no deceleration, 
		m=0 if omitted
MVG		Flag that indicates whether the mechanism is moving,	
		TRUE=moving, 
		FALSE=stopped. 
		Use the PRINT command to see the present value
POS=n		Register that contains the present mechanism position in mm
MAC=n		Motor acceleration current used when the mechanism is 
		accelerating, n=% of 7A
MHC=n		Motor holding current used when the mechanism is stationary, 
		n=% of 7A
MRC=n		Motor run current used when the mechanism is running, n=% of 7A
MSEL=n		Micro stepping resolution, n=2,4,8,16,32,64,128,256
MUNIT=n		Conversion factor from motor pulses to user units,
		n=19 for MSEL=2, n=304 for MSEL=32
BLE=n		Backlash compensation enable flag, n=1 enable, n=0 disable
BLM=n		Backlash mode, n=0 mathematical, n=1 mechanical
BLSH=n		Backlash compensation amount, mm
CTR1		Counter of pulses sent to motor
ALL		Variable used with GET, PRINT, and IP instruction
ERR		Read only flag that indicates whether an error has occurred
ERROR		Read only variable that gives the error code of the 
		most recent error
FLAGS		Used with GET, PRINT, and IP instruction
IP n		Initialize specified parameters to factory default settings, 
		you will need to manually update 
		variables given in Table 7, n omitted initializes ALL, 
		n=ALL, VARS, FLAGS, IOS
PGM		Keyword to place MicroLYNX into program mode
PRINT n		Instruction to output variable value to screen
SAVE		Instruction to save ALL variables to non-volatile memory
SER		Read only variable that contains the serial number of 
		the MicroLYNX
VARS		Used with GET, PRINT, and IP instruction
VER		Read only variable that contains the present 
		MicroLYNX firmware version
CP=n		Clears the program space
EXEC n		Execute the program located at address n
LIST n		List the program starting at address n
<ESC>		Terminate all active operations and running programs
Ctrl-C		Terminate all active operations and running programs, 
		forces a partial reset
BAUD=n		Sets the baud rate
DN="n"		Set the device address for PARTY mode operation
ECHO=n		Specifies whether commands will be echoed back over 
		comm.. line, n=0 Full duplex, 
		n=1 Half duplex, n=2 Only respond to PRINT and LIST commands
PRMT		Specifies the character to be used as a prompt character
PARTY		Enables / disables PARTY mode for multi drop applications
```

### What format are commands in?

For PLC's:

These commands will apply to all mechanisms.
```shell
initial - initialize mechanism position and set CW/CCW.
home    - move mechanism to position 0.000. Position 0.000 is dependent 
          on the mechanisms the mechanism data sheet (MDS).
relbrk  - release brake (if brake are being used )
setbrk  - set brake.
```
