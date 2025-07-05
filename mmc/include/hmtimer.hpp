/**
 * @internal
 * @file hmtimer.hpp
 *
 * Timer utility routines and classes header file
 *
 * @if NOTICE
 *
 * $Id: hmtimer.hpp,v 1.13 2004/05/26 01:02:08 henrik Exp $
 *
 * Copyright (c) 2002-2003 FOCUS Software Engineering Pty Ltd, Australia.
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


#ifndef _HMTIMER_H_INCLUDED
#define _HMTIMER_H_INCLUDED

#ifndef __cplusplus
#  error Must use C++ to compile this module!
#endif

// HM platform detection
#include "hmplatf.h"

#if defined(__WIN32__)
#  include <windows.h>
#elif defined(__VXWORKS__)
#  include <timers.h> // timerLib and clockLib
#  include <selectLib.h> // selectLib
#elif defined(__PALMOS__)
#  include <PalmOS.h>
#  include <sys_socket.h>
#elif defined(__MACOS__) && !defined(__MACOSX__)
#  include <utime.h> // Part of Metrowerks libs
#elif defined(__UNIX__)
#  include <unistd.h>
#  include <time.h>
// Include all headers where a timeval could hide:
#  include <sys/time.h>
#  include <sys/select.h>
#else
#  error "Platform not yet supported. Please add a new condition to this #if block or update hmplatf.h"
#endif


/*****************************************************************************
 * Timer interface
 *****************************************************************************/

/**
 * Timer class. This class implements a platform independent access
 * to timers.
 *
 * @version 1.1
 */
class
#if defined (_WINDLL) || defined(__DLL__)
   __declspec(dllexport)
#endif
Timer
{
  public:

   static void sleepMillis(unsigned long msTime);

   Timer();

   Timer(unsigned long msTime);

   int isExpired();

   void startMillis(unsigned long msTime);

   void sleepUntilExpiry();

   unsigned long left();

   unsigned long over();

#if defined(__WIN32__)
   void remain(FILETIME *remainTime);
#else // Unix, VxWorks, Palm OS, ...
   void remain(timeval *remainTime);
#endif


  protected:

#if defined(__WIN32__)
   union
   {
      FILETIME expireTime;
      LONGLONG expireLongLong;
   };
#else
#  if (defined(_POSIX_TIMERS) && !defined(__LINUX__)) || defined(__VXWORKS__)
   struct timespec expireTime;
#  elif defined(__UNIX__)
   struct timeval expireTime;
#  endif
#endif

  private:

   // Disable default operator and copy constructor
   Timer &operator=(Timer &);
   Timer(const Timer &);

};


#endif // ifdef ..._H_INCLUDED
