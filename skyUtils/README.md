# skyUtils for MODS

Library of basic astronomical calculations used by the MODS LBT Telescope Control System (TCS) interface
agent (`lbttcs`) and other MODS data-taking system apps that need time and celestial coordinate
calculations.

### Functions in `skyutils.c`:
 * `CalToMJD()` - convert UT calendar date to modified Julian date
 * `MJDToCal()` - convert modified Julian date to calendar year, months, and day numbers
 * `MJDToYear()` - convert modified Julian date to decimal year (aka epoch)
 * `YearToMJD()` - convert decimal year to modified Julian date
 * `JDtoHJD()` - compute heliocentric Julian date given Julian date and target RA,Dec coordinates
 * `precess()` - precess RA,Dec coordinates between two MJD equinoxes. Uses the USNO Astronomical Ephemeris B18 rigorous precession algorithm
 * `Range()` - ensures that a variable within in the cyclic range 0..max
 * `UTCtoGST()` - convert MJD at midnight UTC and UTC time to Greenwich Mean Sidereal Time (GMST) in decimal hours
 * `Zenith()` - given HA in decimal hours, Dec, and site Latitude in decimal degrees, compute the zenith distance in decimal degrees and secant(ZD)
 * `AzEl()` - given site latitude, HA and Declination, compute azimuth, elevation (aka "altitude"), and parallactic angles

### Functions in `sexstr.c`:
 * `SexStr()` - convert decimal angle (double) to a sexigesimal string in "dd:mm:xx.xxx" format
