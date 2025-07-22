//---------------------------------------------------------------------------
//
// BiStateMechanism - bi-state mechanism object
//
// A container class for a bi-state mechanism.
//
// R. Pogge, OSU Astronomy Dept.
// pogge@astronomy.ohio-state.edu
// 2009 January 25
//
//---------------------------------------------------------------------------

#include <QObject>
#include <QString>
#include <QStringList>

#include "bistatemechanism.h"

// BiStateMechanism Object Constructor

BiStateMechanism::BiStateMechanism(const QString &mName, const QString &mCmd, 
				   const QString &mID0, const QString &mID1,
				   int mTO)
{
  mechName = mName;
  cmdStr = mCmd;
  stateIDs = new QStringList;
  stateIDs->clear();
  stateIDs->append(mID0);
  stateIDs->append(mID1);
  commTimeOut = mTO;
  
  // Reset various state flags

  enabled = true;       // assume if they're instantiating it, it is enabled
  movePending = false;  // no moves pending
  
  // Clear the error string

  errStr.clear();

}

// Get the mechanism name

QString BiStateMechanism::name() const
{
  return mechName;
}

// Set the mechanism name

void BiStateMechanism::setName(const QString &str)
{
  mechName = str;
}

// Get the mechanism command verb

QString BiStateMechanism::command() const
{
  return cmdStr;
}

// Set the mechanism command verb

void BiStateMechanism::setCommand(const QString &str)
{
  cmdStr = str;
}

// Set the mechanism state IDs (preferred method)

void BiStateMechanism::setStateIDs(const QString &arg0, const QString &arg1)
{
  stateIDs->clear();
  stateIDs->append(arg0);
  stateIDs->append(arg1);
}

// Get a given mechanism state ID

QString BiStateMechanism::ID(const int &istate)
{
  if (istate == 0 || istate == 1) {
    return stateIDs->at(istate);
    errStr.clear();
  }
  else {
    QString reply = tr("ID() Invalid State %1 - must be 0..1").arg(istate);
    errStr = reply;
    emit error();
    return reply;
  }
}

// Set a given mechanism state ID individually

void BiStateMechanism::setStateID(const int &istate, const QString &arg)
{
  if (istate == 0 || istate == 1) {
    stateIDs->replace(istate,arg);
    errStr.clear();
  }
  else {
    QString reply = tr("setStateID() Invalid State %1 - must be 0..1").arg(istate);
    errStr = reply;
    emit error();
  }

}

// Get the current mechanism state (0 or 1)

int BiStateMechanism::state() const
{
  return curState;
}

// Get the current mechanism state ID

QString BiStateMechanism::stateID() const
{
  return stateIDs->at(curState);
}

// Set the current mechanism position and emit the stateChanged() signal

void BiStateMechanism::setState(const int &newState)
{
  if (newState == 0 || newState == 1) {
    if (newState != curState) {
      curState = newState;
      emit stateChanged();
    }
    errStr.clear();
  }
  else {
    errStr = tr("setState() Invalid State %1 - must be 0..1").arg(newState);
    emit error();
  }
}

// Get the mechanism communications timeout interval (units: seconds)

int BiStateMechanism::timeout() const
{
  return commTimeOut;
}

// Set the mechanism communications timeout interval (in seconds).  A
// negative timeout is the same as setting it to zero.

void BiStateMechanism::setTimeout(const int &reqTO)
{
  if (reqTO <= 0)
    commTimeOut = 0;
  else
    commTimeOut = reqTO;
}

// Is a move pending (true) or is the mechanism idle (false)?

bool BiStateMechanism::isPending() const
{
  return movePending;
}

// Set the move-pending flag true (move in progress) or false
// (mechanism is idle)

void BiStateMechanism::setPending(const bool &isPending)
{
  movePending = isPending;
}

// Is the mechanism enabled?

bool BiStateMechanism::isEnabled() const
{
  return enabled;
}

// Enable the mechanism

void BiStateMechanism::enable()
{
  enabled = true;
}

// Disable the mechanism

void BiStateMechanism::disable()
{
  enabled = false;
}

// Return the most recent error string

QString BiStateMechanism::error() const
{
  return errStr;
}

// Set the error string and emit the error() signal

void BiStateMechanism::setErrorStr(const QString &newerr)
{
  errStr = newerr;
  emit error();
}

// Clear the error string

void BiStateMechanism::clearError()
{
  errStr.clear();
}

