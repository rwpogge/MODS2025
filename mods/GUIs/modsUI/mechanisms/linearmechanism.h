#ifndef LINEARMECHANISM_H
#define LINEARMECHANISM_H

#include <QObject>

class QString;

class LinearMechanism : public QObject
{
  Q_OBJECT

public:
  LinearMechanism(const QString &nName, const QString &mCmd, 
	          double mMin, double mMax, int mDec, int mTO);

  // Public methods for accessing basic mechanism parameters

  QString name() const;    //!< Mechanism long name
  QString command() const; //!< Mechanism command verb string

  double min() const;   //!< Minimum allowed mechanism position
  double max() const;   //!< Maximum allowed mechanism position
  int decimals() const; //!< Decimals of device precision
  double pos() const;   //!< Current mechanism position
  int timeout() const;  //!< Communications timeout interval in seconds

  void setName(const QString &); //!< Set the mechanism name
  void setCommand(const QString &); //!< Set the command verb
  void setLimits(const double &, const double &, const int &); //!< Set mechanism limits
  void setMin(const double &); //!< Set the minimum allowed position
  void setMax(const double &); //!< Set the maximum allowed position
  void setDecimals(const int &);  //!< Set the number of decimals of device precision
  void setPos(const double &);    //!< Set the current mechanism position
  void setTimeout(const int &);   //!< Set the mechanism communications timeout interval
  
  bool isValid(const double &);    //!< Tests to see if a position is valid

  // Mechanism state methods (enabled|disabled|move pending)

  bool isEnabled() const; //!< Is the mechanisms enabled or disabled?
  void enable();          //!< Enable the mechanism
  void disable();         //!< Disable the mechanism

  bool isPending() const; //!< Is a move pending?
  void setPending(const bool &);  //!< Set the pending move flag

  // Error handling

  QString error() const;  //!< Return the last error message string
  void setErrorStr(const QString &);  //!< Set the error string
  void clearError();      //!< Clears the error state flags/strings
  
signals:
  void posChanged(); //!< Signal emitted when curPos changes
  void error();      //!< Signal emitted on errors, errStr says what happened

private:
  QString mechName;  //!< Name of the mechanism (e.g., longish descriptive name)
  QString cmdStr;    //!< Command verb for this mechanism
  double minPos;     //!< Minimum mechanism position
  double maxPos;     //!< Maximum mechanism position
  int precision;     //!< Decimals of precision (0=integer, 1=0.1, 2=0.01, etc.)
  double curPos;     //!< Current mechanism position
  int commTimeOut;   //!< Mechanism communications timeout interval in seconds
  QString errStr;    //!< Last error string
  bool movePending;  //!< True if a requested move is pending, false if mechanism is idle
  bool enabled;      //!< True if mechanisms is enabled, false if disabled
};

#endif
