# MODS runtime configuration files

Last Update: 2025 July 16 [rwp/osu]

This directory contains the active runtime configuration files
for the MODS instrument control programs.  These are the
files we must protect from tampering, so they are kept here
in the code tree instead of the /home/mods/ support directory

## Files:
<pre>
   changeConfig   - This shell will configure to LBTO of OSU addresses
      Example: link mechanisms.ini to the instrument osu1.
      prompt% sudo changeConfig
      prompt% Change mechanisms.ini link to: osu1, osu2, lbt1, lbt2: osu1

   mechanisms.ini - Link for MODS microlynx mechanism controller (mmc) agent
   mechanisms_MODS1.ini - MODS1 mechanism table
   mechanisms_MODS2.ini - MODS2 mechanism table
</pre>

The MODS1/ and MODS2/ directories have the configuration files for
each MODS instrument.

IIF/ contains copies of the files needed by the lbttcs and other IIF client 
programs that rely on the ICE version of the IIF provided by LBTO. 


