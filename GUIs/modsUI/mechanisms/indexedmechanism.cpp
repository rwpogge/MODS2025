//---------------------------------------------------------------------------
//
// IndexedMechanism - indexed mechanism object
//
// A container class for an indexed mechanism.
//
// R. Pogge, OSU Astronomy Dept.
// pogge@astronomy.ohio-state.edu
// 2009 January 24
//
// Modification History
//   2009 Feb 16 - added idsChanged() signal and setIDList() method [rwp/osu]
//   2009 Mar 01 - added loadIDTable() method to read mechanism ID files [rwp/osu]
//
//---------------------------------------------------------------------------

#include <QtGui>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextStream>

#include "indexedmechanism.h"

// IndexedMechanism Object Constructor

IndexedMechanism::IndexedMechanism(const QString &mName, const QString &mCmd, 
				   int mMin, int mMax, int mTO)
{
  mechName = mName;
  cmdStr = mCmd;
  minPos = mMin;
  maxPos = mMax;
  commTimeOut = mTO;
  
  numPos = mMax - mMin + 1;

  // Reset various state flags

  enabled = true;       // assume if they're instantiating it, it is enabled
  movePending = false;  // no moves pending
  
  // Create an ID list and put in the position number as a placeholder
  // for the position IDs.

  idStrList = new QStringList;
  idStrList->clear();
  for (int i=0;i<numPos;i++) 
    idStrList->append(QString::number(minPos+i));

  // Clear the error string

  errStr.clear();

}

// Get the mechanism name

QString IndexedMechanism::name() const
{
  return mechName;
}

// Set the mechanism name

void IndexedMechanism::setName(const QString &str)
{
  mechName = str;
}

// Get the mechanism command verb

QString IndexedMechanism::command() const
{
  return cmdStr;
}

// Set the mechanism command verb

void IndexedMechanism::setCommand(const QString &str)
{
  cmdStr = str;
}

// Set the mechanism position limits

void IndexedMechanism::setLimits(const int &pmin, const int &pmax)
{
  minPos = pmin;
  maxPos = pmax;
}

// Get the mechnanism minimum position limit

int IndexedMechanism::min() const
{
  return minPos;
}

// Set the mechanism minimum position limit

void IndexedMechanism::setMin(const int &pmin)
{
  minPos = pmin;
  numPos = maxPos - minPos + 1;
}

// Get the mechanism maximum position limit

int IndexedMechanism::max() const
{
  return maxPos;
}

// Set the mechanism maximum position limit 

void IndexedMechanism::setMax(const int &pmax)
{
  maxPos = pmax;
  numPos = maxPos - minPos + 1;
}

// Get the number of discrete positions (convenience function)

int IndexedMechanism::npos() const
{
  return numPos;
}

// Get the current mechanism position

int IndexedMechanism::pos() const
{
  return curPos;
}

// Set the current mechanism position and emit the posChanged() signal

void IndexedMechanism::setPos(const int &newPos)
{
  if (newPos != curPos) {
    curPos = newPos;
    emit posChanged();
  }
  errStr.clear();
}

// Get the mechanism communications timeout interval (units: seconds)

int IndexedMechanism::timeout() const
{
  return commTimeOut;
}

// Set the mechanism communications timeout interval (in seconds).  A
// negative timeout is the same as setting it to zero.

void IndexedMechanism::setTimeout(const int &reqTO)
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

bool IndexedMechanism::isValid(const int &testPos)
{
  if (testPos < minPos || testPos > maxPos) {
    errStr = tr("Invalid %1 position %2 - must be %3..%4").arg(cmdStr).arg(testPos).arg(minPos).arg(maxPos);
    return false;
  }
  else {
    errStr.clear();
    return true;
  }
}

// Is a move pending (true) or is the mechanism idle (false)?

bool IndexedMechanism::isPending() const
{
  return movePending;
}

// Set the move-pending flag true (move in progress) or false
// (mechanism is idle)

void IndexedMechanism::setPending(const bool &isPending)
{
  movePending = isPending;
}

// Is the mechanism enabled?

bool IndexedMechanism::isEnabled() const
{
  return enabled;
}

// Enable the mechanism

void IndexedMechanism::enable()
{
  enabled = true;
}

// Disable the mechanism

void IndexedMechanism::disable()
{
  enabled = false;
}

// Return the most recent error string

QString IndexedMechanism::errorString() const
{
  return errStr;
}

// Set the error string and emit the error() signal

void IndexedMechanism::setErrorStr(const QString &newerr)
{
  errStr = newerr;
  emit error();
}

// Clear the error string

void IndexedMechanism::clearError()
{
  errStr.clear();
}

// Return the ID List in its entirety

QStringList IndexedMechanism::IDList() const
{
  return *idStrList;
}

// Return the ID of a given mechanism position.

QString IndexedMechanism::ID(const int &pos)
{

  if (idStrList->isEmpty())
    return "Empty ID List!";

  if (!isValid(pos)) {
    errStr.prepend("ID() ");
    emit error();
    return errStr;
  }

  int i = pos - minPos;

  // If the ID list is empty, return a string representation of the device
  // position number.

  if (i >= idStrList->size()) 
    return QString::number(pos);
  else 
    return idStrList->at(i);

}

// Set the ID of a given position.  If the index is invalid, set
// errStr and emit the error() signal.  If we're valid, emit
// the idsChanged() signal.

void IndexedMechanism::setID(const int &pos, const QString &newID)
{

  // Is the position in range?

  if (!isValid(pos)) {  // oops, invalid pos!
    errStr.prepend("setID() ");
    emit error();
  }
  else {  // We're good, put it into the ID list
    int i = pos - minPos;
    idStrList->replace(i,newID);
    emit idsChanged();
  }
}

// Clear the ID list and replace all entries with their device
// position numbers as placeholders (resets to the initial state set
// in the constructor).  When done, emit the idsChanged() signal.

void IndexedMechanism::clearIDs()
{
  idStrList->clear();
  for (int i=0;i<numPos;i++)
    idStrList->append(QString::number(i+minPos));
  emit idsChanged();
}

// Add position IDs using a QStringList, and emit the idsChanged() signal

void IndexedMechanism::setIDList(QStringList *newList)
{
  if (newList->size() < numPos) {
    idStrList = newList;
    for (int i=newList->size();i<numPos;i++)
      idStrList->append(QString::number(i+minPos));
  }
  else
    idStrList = newList;
  emit idsChanged();
}

bool IndexedMechanism::loadIDList(const QString &tabName)
{
  bool ok;

  // Create a QFile for the table name - exit on any problems

  QFile inFile(tabName);
  if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    errStr = inFile.errorString();
    return false;
  }

  // Read in the table line-by-line, parsing as we go.  # denotes
  // comments (either whole lines or inline comments), and we skip
  // spaces.  Valid ID records take the form "position IDstring"

  QString itemStr;
  QString line;
  QTextStream in(&inFile);
  do {
    line = in.readLine();
    if (!line.isEmpty()) {  // skip blanks
      line = line.simplified();
      if (!line.startsWith("#")) { // skip comments
	if (line.contains("#")) // remove any inline comments
	  itemStr = line.section('#',0,0);
	else
	  itemStr = line;
	int pos = itemStr.section(' ',0,0).toInt(&ok,10);
	if (ok) {
	  int ipos = pos-minPos;  // convert to index 0..numPos-1
	  idStrList->replace(ipos,itemStr.section(' ',1,-1));
	}
      }
    }
  } while (!line.isNull());
  inFile.close();

  // Emit the IDs-have-changed signal

  emit idsChanged();

  return true;

}
