ODS Front AGW X-Y Stage Data Sheet
General Description
The MODS Front AGW X-Y Stage places a pickoff mirror for guiding and/or wavefront sensing in the off-axis guide field or in ½ of the science field.  The pickoff mirror is placed at its commanded position by two orthogonally mounted linear stages.  Each linear stage is guided with linear ball bearings and driven by a step motor through a belt reducer and ballscrew.  Limit sensors are placed at both extremes of travel.  The limit sensor at one end of travel is also used as a position datum sensor (HOME sensor).  A failsafe brake is used to lock the stages in position while stationary.


Rendering of Front X-Y Stage Asm.




Drive Motor
Motor Type	Size 17 Step motor, 200 steps per revolution, double ended shaft
Part #		Applied Motion # HT17-075-D	
Rated Current:				1.7 amps/phase parallel  (RMS)
Rated Holding Torque:			62 in*oz			
Rated Torque at operating Speed	35 in*oz @ 70 volts

Motor Connections 
A	Orange		&  Black/Wht		(pin13)
A	Orange/Wht	&  Black		(pin12)
B	Yellow		&  Red/Wht		(pin11)
B	Red		&  Yellow/Wht		(pin10)

Motor Controller Specifications
Manufacturer & Model:		IMS MicroLYNX 7  (#MX-CS100-701)
Rated Current:			5amps RMS/phase, 7 amps peak/phase
Rated Voltage:			24 to 75 VDC
Daughterboards:		None

Motor Controller Settings
MSEL = 10		10usteps/fullstep = 2000 microsteps/rev
MUNIT  = 2000	sets units to (2000 usteps/rev) gives velocity and accelerations in rev/sec
MAC = 50 		Acceleration Current = 50%  = 3.5 amps peak
MRC = 34 		Run Current = 1.7 amps RMS *1.4 = 2.4 amps peak  (2.4/7 = 34% )
MHC  = 0		Motor Hold Current is zero

ACLT=1			linear acceleration (default)
ACCL=DECL = 50		acceleration rate (rev/sec^2)
LDECL = 500			limit deceleration rate (rev/sec^2)
Overtravel at Limit = ScrewPitch * VM2/(2*LDECL)  =  0.10mm (pitch =1mm and VM=10 rev/sec, LDECL=500 rev/sec^2)
Beware overtravel when seeking limits, must not decelerate hard into limits
Maximum Permitted Overtravel ~ 0.5mm ~ 0.020

VM= 10			running speed  (rev/sec)
VM= 0.5			Homing speed to assert limits 

Motor Controller  I/O Connections
Vpull:		not used
GND:		24 volt Gnd
I/O 21:		CW LIMIT sensor     (CW shaft rotation as viewed from motor front)
I/O 22:		CCW LIMIT sensor
I/O 23:		not used 
I/O 24:		not used 
I/O 25: 		not used
I/O 26: 		Brake Relay Coil  (internal pull-up switch opened)

Input Sensors
Model  P&F # NBB1.5-8GM50-E0-V3
8mm Inductive proximity sensor, Normally Open Sinking output (Type E0), 24 VDC supply
Used for CW Limit, CCW limit

Connection for P&F E0 Sensors (3 wire)
Brown 	+24 volts
Blue	24 volt ground
Black	to input of controller

Output Devices
Failsafe Brake directly on ballscrew

Drive Mechanics
The Front AGW X -Y stages use HSR15 rails with recirculating ball linear pillow blocks to guide the stage motion.   A series BNK 10mm ballscrew with a 2mm pitch drives the stage.   This ballscrew is coupled to a 200 fullstep/rev motor through a 2:1 HTD timing belt reducer.

X Axis
X Position Datum	Toward motor end of actuator, CW End of Travel
X Range of Motion	180mm
X Axis Resolution	5 microns per motor fullstep = (2mm)*(1/2)*(1/200)

Y Axis
Y Position Datum	Toward motor end of actuator, CW End of Travel
Y Range of Motion	200mm   (180.5mm motion plus 19.5mm of focus compensation)
Y Axis Resolution	5 microns per motor fullstep = (2mm)*(1/2)*(1/200)

Performance
Maximum Travel Time				20 seconds  (200mm/10mm/sec = 20sec)  
Typical Travel Time for move of ~ 50mm	5 seconds
Position Repeatability				?? micron error	
Position Hysteresis  				?? micron	

Software Notes
Interlock between Y axis and calibration tower.
Y axis must be fully retracted to the HOME position (CW Limit asserted) or calibration tower motion is inhibited.

*  

* The mechanism position register is set to Zero
