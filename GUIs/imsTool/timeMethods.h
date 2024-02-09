#ifndef TIMEMETHODS_H
#define TIMEMETHODS_H
/*!
  \file timeMethods.h
  \brief time definitions for xmis GUI.

  \author Staff, OSU Dept. of Astronomy (rgonzale@astronomy.ohio-state.edu)
  \date 2011 Oct 10 [rdg]

  \par Modification History:
  2011 Oct 10 - Initial[rdg]
<pre>
</pre>

*/

#include <QtWidgets>
#include <QThread>
class misWindow;

#define CMD_ERR -1
#define CMD_OK   0
#define CMD_WARN 1

/*!
  \brief multiple timers
  2011 Oct 10 - Initial[rdg]
*/

class Timers: public QThread {

public:
  static void usleep(unsigned long usecs){QThread::usleep(usecs);}; //!< microsecs
  static void msleep(unsigned long msecs){QThread::msleep(msecs);}; //!< millisecs
  static void sleep(unsigned long secs){QThread::sleep(secs);}; //!< secs

};

#endif //TIMEMETHODS_H
