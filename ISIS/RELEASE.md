# This is ISIS version 3.0 

Updated: 2024 Feb 19

Author: R. Pogge, OSU Astronomy Dept. (pogge.1@osu.edu)

## Contents

 * `isisServer` - ISIS server application (interactive and non-interactive)
 * `isisClient` - libisis client library

## Release Notes

### Version 3.0.0 [2024 Feb 19]

Port of the ISIS server apps and client libraries to Alma Linux 9.3 using ISO g++ v11.4.1 20230605
 * Rewrote `build` csh scripts as bash, tested, retired csh build scripts
 * In server, removed ISO C++ disallowed `if(strstr(s1,s2)>0)` and using instead `if(strstr(s1,s2))` test.
 * In server and client code, ISO C++ compiler complained about passing literal strings as `char*`. While it compiles fine using `-fpermissive`, we fixed the problem by making an explicit cast on all literal strings passed to functions like `broadcastMessage()` and `logMessage()` making the code more compliant
 * Edited the `Makefile` for new directory structure

Code was tested with a simple test client compiled and linked to `libisis` and the ISIS server, and with the `iTerm` Qt5 gui app 
after it was ported successfully from Qt4.
