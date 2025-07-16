#ifndef INDEXEDMECHANISM_H
#define INDEXEDMECHANISM_H

#include <QObject>

class QString;
//class QStringList;
class QFile;
class QTextStream;

class IndexedMechanism : public QObject
{
  Q_OBJECT

public:
  IndexedMechanism(const QString &mName, const QString &mCmd, 
		   int mMin, int mMax, int mTO);

  // Public methods for accessing basic mechanism parameters

  QString name() const;    //!< Mechanism long name
  QString command() const; //!< Mechanism command verb string

  int min() const;     //!< Minimum allowed mechanism position
  int max() const;     //!< Maximum allowed mechanism position
  int pos() const;     //!< Current mechanism position
  int timeout() const; //!< Mechanism timeout interval in seconds
  int npos() const;    //!< Number of discrete mechanism positions

  void setName(const QString &); //!< Set the mechanism name
  void setCommand(const QString &); //!< Set the command verb
  void setLimits(const int &, const int &); //!< Set mechanism limits
  void setMin(const int &);  //!< Set the minimum allowed position
  void setMax(const int &);  //!< Set the maximum allowed position
  void setPos(const int &);     //!< Set the current mechanism position
  void setTimeout(const int &); //!< Set the mechanism timeout interval
  
  bool isValid(const int &); //!< Tests to see if a position is valid

  // Mechanism state methods (enabled|disabled|move pending)

  bool isEnabled() const; //!< Is the mechanism enabled or disabled?
  void enable();          //!< Enable the mechanism
  void disable();         //!< Disable the mechanism

  bool isPending() const; //!< Is a move pending?
  void setPending(const bool &);  //!< Set the pending move flag

  // Error handling

  QString errorString() const;  //!< Return the last error message string
  void setErrorStr(const QString &);  //!< Set the error string
  void clearError();      //!< Clears the error state flags/strings
  
  // Mechanism ID Table Methods

  QStringList IDList() const;    //!< Return the entire ID table
  QString ID(const int &);       //!< Return the ID of a given position
  void setID(const int &, const QString &); //!< Set the position ID
  void setIDList(QStringList *);      //!< Set the IDs by String List
  void clearIDs();               //!< Clear the ID table
  bool loadIDList(const QString &); //!< Load IDs from a mechanism table file

signals:
  void posChanged(); //!< Signal emitted when the current position changes
  void idsChanged(); //!< Signal emitted when the ID table has changed

  void error();      //!< Signal emitted on errors (errStr contains what happened)
  
private:
  QString mechName;       //!< Name of the mechanism (e.g., descriptive name)
  QString cmdStr;         //!< Command verb for this mechanism
  int minPos;             //!< Minimum mechanism position
  int maxPos;             //!< Maximum mechanism position
  int numPos;             //!< Number of discrete positions
  int curPos;             //!< Current mechanism position
  int commTimeOut;        //!< Mechanism communications timeout interval in seconds
  QStringList *idStrList; //!< List of discrete position IDs (e.g., filter names)
  QString errStr;         //!< Last error string
  bool movePending;       //!< True if a requested move is pending, false if mechanism is idle
  bool enabled;           //!< True if mechanisms is enabled, false if disabled
};

#endif
