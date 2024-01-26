#ifndef BISTATEMECHANISM_H
#define BISTATEMECHANISM_H

#include <QObject>

class QString;
class QStringList;

class BiStateMechanism : public QObject
{
  Q_OBJECT

public:
  BiStateMechanism(const QString &mName, const QString &mCmd,
		   const QString &mID0, const QString &mID1,
		   int mTO);

  // Public methods for accessing basic mechanism parameters

  QString name() const;    //!< Mechanism long name
  QString command() const; //!< Mechanism command verb string
  int state() const;       //!< Current mechanism state (0 or 1)
  QString stateID() const; //!< Current ID of the mechanism state

  void setName(const QString &);    //!< Set the mechanism name
  void setCommand(const QString &); //!< Set the command verb
  void setStateIDs(const QString &, const QString &); //!< Set mechanism state IDs
  void setStateID(const int &, const QString &);   //!< Set a given mechanism state ID
  QString ID(const int &);          //!< ID of a given state
  void setState(const int &);       //!< Set the current mechanism state

  int timeout() const;  //!< Mechanism timeout interval in seconds
  void setTimeout(const int &); //!< Set the mechanism timeout interval
  
  // Mechanism state methods (enabled|disabled|move pending)

  bool isEnabled() const; //!< Is the mechanism enabled or disabled?
  void enable();          //!< Enable the mechanism
  void disable();         //!< Disable the mechanism

  bool isPending() const; //!< Is a move pending?
  void setPending(const bool &);  //!< Set the pending move flag

  // Error handling

  QString error() const;  //!< Return the last error message string
  void setErrorStr(const QString &);  //!< Set the error string and emit error() signal
  void clearError();      //!< Clears the error state flags/strings
  
signals:
  void stateChanged(); //!< Signal emitted when the state changes
  void error();        //!< Signal emitted on errors (check errStr for what happened)

private:
  QString mechName;       //!< Name of the mechanism (e.g., descriptive name)
  QString cmdStr;         //!< Command verb for this mechanism
  QStringList *stateIDs;  //!< Command state IDs
  int curState;           //!< Current mechanism state (0, 1, or -1 = unknown)
  int commTimeOut;        //!< Mechanism communications timeout interval in seconds
  QString errStr;         //!< Last error string
  bool movePending;       //!< True if a requested move is pending, false if mechanism is idle
  bool enabled;           //!< True if mechanisms is enabled, false if disabled
};

#endif
