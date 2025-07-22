//---------------------------------------------------------------------------
//
// LinearMechanism - Linear (continous) mechanism object
//
// A container class for a linear (continous) mechanism
//
// R. Pogge, OSU Astronomy Dept.
// pogge@astronomy.ohio-state.edu
// 2009 January 24
//
//---------------------------------------------------------------------------

#include <QObject>
#include <QString>

#include "linearmechanism.h"

// Linear LinearMechanism Object Constructor

LinearMechanism::LinearMechanism(const QString &mName, const QString &mCmd, 
		                 double mMin, double mMax, int mDec, int mTO)
{
  mechName = mName;
  cmdStr = mCmd;
  minPos = mMin;
  maxPos = mMax;

  if (mDec < 0)
    precision = 0;
  else
    precision = mDec;

  if (mTO < 0) 
    commTimeOut = 0;
  else
    commTimeOut = mTO;

  // Reset various state flags

  enabled = true;       // assume if they're instantiating it, it is enabled
  movePending = false;  // no moves pending
  
  // Clear the error string

  errStr.clear();

}

// Set the mechanism name

QString LinearMechanism::name() const
{
  return mechName;
}

// Set the mechanism name

void LinearMechanism::setName(const QString &str)
{
  mechName = str;
}

// Get the mechanism command verb

QString LinearMechanism::command() const
{
  return cmdStr;
}

// Set the mechanism command verb

void LinearMechanism::setCommand(const QString &str)
{
  cmdStr = str;
}

// Set the mechanism position limits

void LinearMechanism::setLimits(const double &pmin, const double &pmax,
				const int &pdec)
{
  minPos = pmin;
  maxPos = pmax;
  precision = pdec;
}

// Get the mechnanism minimum position limit

double LinearMechanism::min() const
{
  return minPos;
}

// Set the mechanism minimum position limit

void LinearMechanism::setMin(const double &pmin)
{
  minPos = pmin;
}

// Get the mechanism maximum position limit

double LinearMechanism::max() const
{
  return maxPos;
}

// Set the mechanism maximum position limit 

void LinearMechanism::setMax(const double &pmax)
{
  maxPos = pmax;
}

// Get the mechanism position precision (# decimal places)

int LinearMechanism::decimals() const
{
  return precision;
}

// Set the mechanism position precision (# decimal places)

void LinearMechanism::setDecimals(const int &prec)
{
  if (prec < 0)
    precision = 0;
  else 
    precision = prec;
}

// Get the current mechanism position

double LinearMechanism::pos() const
{
  return curPos;
}

// Set the current mechanism position and emit the posChanged() signal

void LinearMechanism::setPos(const double &newPos)
{
  curPos = newPos;
  emit posChanged();
  errStr.clear();
}

// Get the mechanism communications timeout interval (units: seconds)

int LinearMechanism::timeout() const
{
  return commTimeOut;
}

// Set the mechanism communications timeout interval (in seconds).  A
// negative timeout is the same as setting it to zero.

void LinearMechanism::setTimeout(const int &reqTO)
{
  if (reqTO <= 0)
    commTimeOut = 0;
  else
    commTimeOut = reqTO;
}

// Test a possible device position for validity by comparing it to the
// device limits.  Returns true if valid, false if invalid. If
// invalid, it creates a ready-made report string in errStr, otherwise
// errStr is cleared.  Note that we do not emit an error() signal here.

bool LinearMechanism::isValid(const double &testPos)
{
  if (testPos < minPos || testPos > maxPos) {
    errStr = "Invalid "+cmdStr+" position "+
      QString::number(testPos,'f',precision)+": Must be "+
      QString::number(minPos,'f',precision)+".."+
      QString::number(maxPos,'f',precision);
    return false;
  }
  else {
    errStr.clear();
    return true;
  }
}

// Is a move pending (true) or is the mechanism idle (false)?

bool LinearMechanism::isPending() const
{
  return movePending;
}

// Set the move-pending flag true (move in progress) or false
// (mechanism is idle)

void LinearMechanism::setPending(const bool &isPending)
{
  movePending = isPending;
}

// Is the mechanism enabled?

bool LinearMechanism::isEnabled() const
{
  return enabled;
}

// Enable the mechanism

void LinearMechanism::enable()
{
  enabled = true;
}

// Disable the mechanism

void LinearMechanism::disable()
{
  enabled = false;
}

// Return the most recent error string

QString LinearMechanism::error() const
{
  return errStr;
}

// Set the error string and emit the error() signal

void LinearMechanism::setErrorStr(const QString &newerr)
{
  errStr = newerr;
  emit error();
}

// Clear the error string

void LinearMechanism::clearError()
{
  errStr.clear();
}
