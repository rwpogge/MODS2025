# Archon Configuration File Release Notes

**Last Release: 2026 Feb 21**

## 2026 Feb 26
 * TCS headers changed to use `S_AchievedALT` and `S_AchievedAZ` and convert datum to floating point degrees (from float radians). Thi sis for LBT Archive back compatibility requested by Martinia Vicinanza (INAF)

## 2026 Feb 21
 * TCS headers now usin `S_AchievedALTDMS` and `S_AchievedAZDMS` instead of the `MountAchieved...` which gives confusing info.g
 * TCS header template removed `S_RotAngle` as it is redundant (modulo 360 or 2*360 deg) with `S_DGRPosition` and could be confused with `POSANGLE` for celestial position angle.

## 2026 Jan 14
 * Updated `modsTCS_left.txt` and `modsTCS_right.txt` header templates based on feedback from the LBT Archive and LBTO
 * Updated static header templates (`header_MODS1B.txt` et al.) based on feedback from LBTO

## 2026 Jan 02
Critical updates:
 * Updated Archon .nfc config files for correct readout (50 pre-scan instead of 48)

## 2025 Dec 19
 * Set `ContinousExposures=0` in the updated .ncf files
 * moved old files out of the way into Old/ folders

## 2025 Dec 16
 * New Archon .ncf files from Mike Lesser after extensive live testing.

## 2025 Oct 23
 * Commited new Archon ..ncf files with PID control
 * Updated test Archon acf and ncf files

## 2025 Oct 20
Live testing with FITS images taken with live TCS telemetry at the telescope:
 * Cleaned up the static header templates
 * TCS header cleanup after live testing
 * Various typos and other problem fixed in headers

### 2025 Sept 23
 * Various updates to FITS header configuration and htead templates
 * updated TCS headers for Build 2025B updates to the DD (new keywords)

## 2025 Aug 26
 * First-light Archon configuration files setup

## 2025 Aug 7
 * Changes from LBTO work on-site
 * First round of Archon files from initial warm MODS work

