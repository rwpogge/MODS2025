MODS Front AGW Focus Data Sheet
General Description
The MODS Front AGW Focus mechanism is required to maintain focus on the guide camera as the Front AGW X-Y stage scans guide pickoff mirror across the spherical MODS focal surface.  The MODS focal surface has a sag of 19.5mm when the pickoff prism is placed at the extreme corner of the off-axis guide field.  	

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
MAC = 50 		Acceleration Current = 50% = 3.5 amps peak
MRC = 34 		Run Current = 1.7 amps RMS *1.4 =2.4 amps peak  (2.4/7 = 34% )
MHC  = 0		Motor Hold Current is zero

ACLT=1			linear acceleration (default
ACCL=DECL = 50		acceleration rate (rev/sec^2)
LDECL = 500			limit deceleration rate (rev/sec^2)
Overtravel at Limit = ScrewPitch * VM2/(2*LDECL)  =  0.05mm (pitch =2mm and VM=5 rev/sec, LDECL=500 rev/sec^2)
Beware overtravel when seeking limits, must not decelerate hard into limits
Maximum Permitted Overtravel ~ 0.5mm ~ 0.020

VM= 5				running speed   (rev/sec)
VM= 0.5			Homing speed to assert limits 

Motor Controller  I/O Connections
Vpull:		not used 
GND:		24 volt Gnd
I/O 21:		CW LIMIT sensor     (CW shaft rotation as viewed from motor front)
I/O 22:		CCW LIMIT sensor 
I/O 23:		not used
I/O 24:		not used 
I/O 25: 		not used
I/O 26: 		not used

Input Sensors
Model  P&F # NJ0.8-5GM25-E
5mm Inductive proximity sensor, Normally Open Sinking output (Type E0), 24 VDC supply
Used for CW Limit, CCW limit

Output Devices
None

Drive Mechanics
The Front AGW Focus mechanism uses a THK Model KR2602A+150L0-0000 linear actuator with a 2mm pitch ballscrew.   This screw is directly coupled to the 200fullstep/rev motor described above. 

Position Datum		Away from motor end of actuator   (CCW End of Travel)
Range of Motion	~30mm   (19.5mm required for focus compensation plus an offset)
Axis Resolution		10 microns per motor fullstep = (2mm)*(1/200)

Performance
Maximum Travel Time				3 seconds  (30mm* 1rev/2mm  * 1sec/5revs)
Position Repeatability				?? micron error	
Position Hysteresis  				?? micron

Software Notes
The position of the Front AGW focus stage must be computed based on the X-Y position of the stage and and focus offset value.
The radius of curvature of the LBT Gregorian focal surface is 1040mm
