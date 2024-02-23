# iTerm Release Notes
Last Build: 2024 Feb 23

## Version 1.2.0
Friday, February 23rd, 2024

- Qt 4.X -> Qt 5.X
    - The GUI should now be compiled using Qt 5.
- The build script was removed in favor of the standard `qmake` and `make` commands.
- Compiler warnings are no longer suppressed.
- All compiler warnings were fixed.
    - Several unused variables were removed from client.cpp.
    - While loop condition ambiguity was fixed in client.cpp.

## Version 1.1
Friday, November 16th, 2012

This was the first version committed to the repository.

- Added sensible comments to the code.
- Made the GUI more user friendly.
- Enhanced the IMS Terminal window by introducing microLynx Errors numbers and messages. (the PC-IMS version form IMS Corp. does not have)
- Removed [Download] item from [Transfer] menu item (too dangerous).
- Reworked the [upLoad] and input commands to match PC-IMS. The first version was done in a hurry.

## Version 1.0
December, 2011

This is the first instance of the ims code. It is written in Qt4 and is used to test and/or interrogate microLynx PLC microcode with a microLynx controller with or without hardware.

This GUI is self-explanatory but just incase:

- [File] - load a file. DO NOT SAVE A FILE to many drawbacks
- [Edit] - Edit a file if needed as a test plc file
- [View] - NOT working properly
- [Transfer] - upLoad a file from the File window to MicroLynx controller, or downLoad a file from the MicroLynx controller to the File window.
- [Help] - Help button
- [Find]         - find a file
- [IP]            - input for the IP address
- [Socket]       - Port number
- [clearNVM]     - This clears the previous PLC microcode from the MicroLynx memory
- [Open Socket]  - Open the IP:Socket
- [Close Socket] - Close the socket

The IMS Terminal window shows the command sent and the response.