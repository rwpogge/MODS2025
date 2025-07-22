/**
 * @internal
 * @file hmplatf.h
 *
 * Platform differentation macros
 *
 * @if NOTICE
 *
 * $Id: hmplatf.h,v 1.15 2004/05/26 01:02:46 henrik Exp $
 *
 * Copyright (c) 2000-2003 FOCUS Software Engineering, Australia.
 * All rights reserved. <www.focus-sw.com>
 *
 * USE OF THIS SOFTWARE IS GOVERNED BY THE TERMS AND CONDITIONS OF A
 * SEPARATE LICENSE STATEMENT AND LIMITED WARRANTY.
 *
 * IN PARTICULAR, YOU WILL INDEMNIFY AND HOLD FOCUS SOFTWARE ENGINEERING,
 * ITS RELATED COMPANIES AND ITS SUPPLIERS, HARMLESS FROM AND AGAINST ANY
 * CLAIMS OR LIABILITIES ARISING OUT OF THE USE, REPRODUCTION, OR
 * DISTRIBUTION OF YOUR PROGRAMS, INCLUDING ANY CLAIMS OR LIABILITIES
 * ARISING OUT OF OR RESULTING FROM THE USE, MODIFICATION, OR DISTRIBUTION
 * OF PROGRAMS OR FILES CREATED FROM, BASED ON, AND/OR DERIVED FROM THIS
 * SOURCE CODE FILE.
 *
 * @endif
 */


#ifndef _HMPLATF_H_INCLUDED
#define _HMPLATF_H_INCLUDED


/**
 * @defgroup hm_platf Platform and Compiler Specific Coding Support
 *
 * It is good practice to avoid compiler or platform specific
 * implementations as much as reasonable. However if some program sections
 * have to be implemented differently for particular compilers, then the
 * following macros definitions are available to embed compiler specific
 * code in #ifdef/#endif preprocessor blocks.
 *
 * @remark Platforms currently supported:
 * - RedHat Linux 6.2, 7.2, 8.0, 9.0
 * - uClinux 2.5
 * - SGI IRIX 6.5 using MIPSpro compiler
 * - DEC True 64/Digital UNIX V4.0F using DEC CXX
 * - SCO Openserver 3.2 using skunkware gcc compiler or UDK C++ Compiler
 * - SUN Solaris 8/SunOS 5.8 using SunPro compiler
 * - QNX 6.0/6.2
 * - QNX 4.25
 * - VxWorks 5.4
 * - Lynx OS 4.0
 * - Palm OS 3.5 using Metrowerks and GNU CC compiler
 * - Cygwin B20
 * - Windows 95/98/NT/Me/2000/XP
 * - MS-DOS
 * - Keil C51 6.0
 * - IBM AIX using xlC compiler
 * - HP-UX using aCC compiler
 * - MAC OS X
 *
 * <h2>Compiler Specific Coding</h2>
 *
 * Definitions which can be used to implement code compiler specific:
 *
 * <table>
 *   <tr><td><b>Macro</b></td><td><b>Compiler</b></td></tr>
 *   <tr><td>__TURBOC__</td><td>Turbo C/C++</td></tr>
 *   <tr><td>__BORLANDC__</td><td>Borland C++</td></tr>
 *   <tr><td> __GNUC__</td><td>GNU GCC/EGCS</td></tr>
 *   <tr><td>__WATCOMC__</td><td>Watcom C/C++</td></tr>
 *   <tr><td>__MWERKS__</td><td>Metrowerks C/C++</td></tr>
 *   <tr><td>__SC__</td><td>Symantec C/C++</td></tr>
 *   <tr><td>__MRC__</td><td>Mac Programmer's Workbench</td></tr>
 *   <tr><td>_MSC_VER</td><td>MS C/Visual C++</td></tr>
 *   <tr><td>__SUNPRO_CC</td><td>SUN Pro C++</td></tr>
 *   <tr><td>__DECC</td><td>DEC CXX</td></tr>
 *   <tr><td>__xlC__</td><td>IBM xlC</td></tr>
 *   <tr><td>__IBMC__</td><td>IBM Visual Age</td></tr>
 *   <tr><td>__HP_aCC</td><td>HP ANSI C++ Compiler</td></tr>
 *   <tr><td>__USLC__</td><td>SCO UDK 7 C++ Compiler</td></tr>
 *   <tr><td>__KEIL__</td><td>Keil C51</td></tr>
 *   <tr><td>__HIGHC__</td><td>MetaWare High C</td></tr>
 * </table>
 *
 * <h2>Platform Specific Coding</h2>
 *
 * The following set of platform macros helps writing modules which
 * can be compiled differently on various platforms.
 *
 * <table>
 *   <tr><td><b>Macro</b></td><td><b>Platform</b></td></tr>
 *   <tr><td>__UNIX__</td><td>Standard UNIX/POSIX.1 apps</td></tr>
 *   <tr><td>__LINUX__</td><td>Linux applications</td></tr>
 *   <tr><td>__NETBSD__</td><td>NetBSD applications</td></tr>
 *   <tr><td>__FREEBSD__</td><td>FreeBSD applications</td></tr>
 *   <tr><td>__OPENBSD__</td><td>OpenBSD applications</td></tr>
 *   <tr><td>__IRIX__</td><td>SGI IRIX applications</td></tr>
 *   <tr><td>__OSF__</td><td>DEC Alpha OSF (Tru64 UNIX) applications</td></tr>
 *   <tr><td>__ULTRIX__</td><td>DEC Ultrix applications</td></tr>
 *   <tr><td>__SOLARIS__</td><td>SUN Solaris applications</td></tr>
 *   <tr><td>__SCO_VERSION__</td><td>SCO OpenServer applications</td></tr>
 *   <tr><td>__QNX__</td><td>QNX 4 and QNX 6 applications</td></tr>
 *   <tr><td>__QNX4__</td><td>QNX 4 applications</td></tr>
 *   <tr><td>__QNXNTO__</td><td>QNX 6 applications</td></tr>
 *   <tr><td>__VXWORKS__</td><td>VxWorks applications</td></tr>
 *   <tr><td>__LYNXOS__</td><td>LynxOS applications</td></tr>
 *   <tr><td>__MAC__</td><td>Mac OS (non OS X) applications</td></tr>
 *   <tr><td>__MACOSX__</td><td>Mac OS X applications</td></tr>
 *   <tr><td>__PALMOS__</td><td>Palm OS applications</td></tr>
 *   <tr><td>__WIN32__</td><td>Windows 95/98/Me/NT/2000/XP applications </td>
 *   <tr><td>__CYGWIN__</td><td>Cygwin applications </td>
 *   <tr><td>__MSDOS__</td><td>DOS 16-bit console applications</td></tr>
 *   <tr><td>__C51__</td><td>8051 embedded applications</td></tr>
 * </table>
 *
 * <h2>Target Types</h2>
 *
 * The following macros should be used to compile different types of
 * targets. They have to be defined manually in the
 * makefile or in the IDE compiler settings.
 *
 * <table>
 *   <tr><td><b>Macro</b></td><td><b>Target Type</b></td></tr>
 *   <tr><td>__GUI__</td><td>Running on a graphical user interface,
 *      no character I/O available</td></tr>
 *   <tr><td>__CONSOLE__</td><td>Standard character I/O
 *      user interface</td></tr>
 *   <tr><td>__EMBEDDED__</td><td>Running without operating system</td></tr>
 *   <tr><td>__DLL__ or _WINDLL</td><td>Compiling as DLL</td></tr>
 * </table>
 *
 */


#undef _HMPLATF_DETECTED


/*
 * Linux applications
 */
#if defined(linux) || defined(__linux__)
#  if !defined(__LINUX__)
#     define __LINUX__
#  endif
#  if !defined(__UNIX__)
#     define __UNIX__
#  endif
#  define _HMPLATF_DETECTED
#endif


/*
 * Free BSD
 */
#if defined(__FreeBSD__)
#  if !defined(__FREEBSD__)
#     define __FREEBSD__
#  endif
#  if !defined(__UNIX__)
#     define __UNIX__
#  endif
#  define _HMPLATF_DETECTED
#endif


/*
 * Open BSD
 */
#if defined(__OpenBSD__)
#  if !defined(__OPENBSD__)
#     define __OPENBSD__
#  endif
#  if !defined(__UNIX__)
#     define __UNIX__
#  endif
#  define _HMPLATF_DETECTED
#endif



/*
 * Net BSD
 */
#if defined(__NetBSD__)
#  if !defined(__NETBSD__)
#     define __NETBSD__
#  endif
#  if !defined(__UNIX__)
#     define __UNIX__
#  endif
#  define _HMPLATF_DETECTED
#endif



/*
 * SGI IRIX applications
 *
 * __sgi and __unix is defined by MIPSPro C/C++
 */
#if defined(sgi) || defined(__sgi)
#  if !defined(__IRIX__)
#     define __IRIX__
#  endif
#  if !defined(__UNIX__)
#     define __UNIX__
#  endif
#  define _HMPLATF_DETECTED
#endif


/*
 *  Solaris applications
 */
#if defined(sun) || defined(__sun__)
#  if !defined(__SOLARIS__)
#     define __SOLARIS__
#  endif
#  if !defined(__UNIX__)
#     define __UNIX__
#  endif
#  define _HMPLATF_DETECTED
#endif


/*
 *  SCO applications
 */
#if defined(__SCO_VERSION__)
#  if !defined(__SCO__)
#     define __SCO__
#  endif
#  if !defined(__UNIX__)
#     define __UNIX__
#  endif
#  define _HMPLATF_DETECTED
#endif


/*
 *  DEC Alpha OSF (Tru64 UNIX) applications
 */
#if defined(__osf__) || defined(osf)
#  if !defined(__OSF__)
#     define __OSF__
#  endif
#  if !defined(__UNIX__)
#     define __UNIX__
#  endif
#  define _HMPLATF_DETECTED
#endif


/*
 *  DEC Ultrix applications
 */
#if defined(ultrix) || defined(__ultrix__)
#  if !defined(__ULTRIX__)
#     define __ULTRIX__
#  endif
#  if !defined(__UNIX__)
#     define __UNIX__
#  endif
#  define _HMPLATF_DETECTED
#endif


/*
 *  HP-UX
 */
#if defined(hpux) || defined(__hpux__)
#  if !defined(__HPUX__)
#     define __HPUX__
#  endif
#  if !defined(__UNIX__)
#     define __UNIX__
#  endif
#  define _HMPLATF_DETECTED
#endif


/*
 *  IBM AIX Unix
 */
#if defined(_AIX)
#  if !defined(__AIX__)
#     define __AIX__
#  endif
#  if !defined(__UNIX__)
#     define __UNIX__
#  endif
#  define _HMPLATF_DETECTED
#endif


/*
 * QNX4 applications
 *
 * __QNX__ is defined by Watcom C
 */
#if defined(__QNX__) && defined(__WATCOMC__) && !defined(__QNXNTO__)
#  if !defined(__QNX4__)
#     define __QNX4__
#  endif
#  if !defined(__UNIX__)
#     define __UNIX__
#  endif
#  define _HMPLATF_DETECTED
#endif


/*
 * QNX 6 applications
 *
 * __QNX__, __QNXNTO__ are defined by gcc for QNX 6
 */
#if defined(__QNX__) && defined(__QNXNTO__)
#  if !defined(__UNIX__)
#     define __UNIX__
#  endif
#  define _HMPLATF_DETECTED
#endif


/*
 * VxWorks
 */
#if defined(__vxworks__) || defined(vxworks)
#  if !defined(__VXWORKS__)
#     define __VXWORKS__
#  endif
#  define _HMPLATF_DETECTED
#endif


/*
 * Lynx OS
 */
#if defined(__Lynx__) || defined(Lynx)
#  if !defined(__LYNXOS__)
#     define __LYNXOS__
#  endif
#  if !defined(__UNIX__)
#     define __UNIX__
#  endif
#  define _HMPLATF_DETECTED
#endif


/*
 * Windows 95/98/NT/2000/XP applications
 */
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)) && !defined(__CYGWIN__)
#  if !defined(__WIN32__)
#     define __WIN32__
#  endif
#  define _HMPLATF_DETECTED
#endif


/*
 * Cygwin applications
 */
#if defined(__CYGWIN__)
#  if !defined(__UNIX__)
#     define __UNIX__
#  endif
#  define _HMPLATF_DETECTED
#endif


/*
 * Keil C51 embedded applications
 */
#if defined(__C51__)
#  define __EMBEDDED__
#  define _HMPLATF_DETECTED
#endif


/*
 * MAC OS (non OS X)
 *
 * Metrowerks for Palm OS lite defines one of the mac macros!
 */
#if (defined(macintosh) || defined(_MAC) || defined (__macintosh)) && !defined(__PALMOS_TRAPS__) && (defined(__MWERKS__) || defined(__MRC__) || defined(__SC__))
#  if !defined(__MACOS__)
#     define __MACOS__
#  endif
#  define _HMPLATF_DETECTED
#endif


/*
 * MAC OS X / Darwin
 *
 * __APPLE__,  __MACH__ and __POWERPC__ are predefined by GCC
 */
#if defined(__APPLE__) && defined(__MACH__) && defined(__POWERPC__)
#  if !defined(__MACOSX__)
#     define __MACOSX__
#  endif
#  if !defined(__UNIX__)
#     define __UNIX__
#  endif
#  define _HMPLATF_DETECTED
#endif


/*
 * Palm OS applications
 *
 * __palmos__ is defined by gcc of prc-tools 2.0
 * __PALMOS_TRAPS__ is defined by Metrowerks CodeWarrior
 */
#if defined(__palmos__) || defined(__PALMOS_TRAPS__)
#  define __PALMOS__
#  define _HMPLATF_DETECTED
#endif


/*
 * DOS legacy 16-bit console applications
 */
#if defined(MSDOS) || defined(_MSDOS) || defined(__MSDOS__)
#  if !defined(__MSDOS__)
#     define __MSDOS__
#  endif
#  define __CONSOLE__
#  define _HMPLATF_DETECTED
#endif


/*
 * Last resort, try to detect any other UNIX
 */
#if defined(__unix__) || defined (unix) || defined(__unix) || defined(_unix)
#  if !defined(__UNIX__)
#     define __UNIX__
#  endif
#  define _HMPLATF_DETECTED
#endif


#endif /* ifdef ..._H_INCLUDED */
