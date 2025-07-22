#ifndef FILECONFIG_H
#define FILECONFIG_H

#include <QWidget>
#include <QHostAddress>
#include <QFile>

#include "mods.h" // Parameters of the MODS User Interface

// Qt widgets

class QString;
class QTextEdit;
class QLineEdit;
class QLabel;
class QPushButton;
class QDateTime;
class QTimer;

// Custom widgets

class TextEntry;
class TextDisplay;
class ValueDisplay;
class ActionButton;
class ISISClient;

// FileConfig Class definition

class FileConfig : public QWidget
{
  Q_OBJECT

public:
  FileConfig(const int &myMODS, QWidget *parent = 0);

  // Public methods

  QString error() const;  //!< Return the last error string
  void enable();  //!< disable the widget
  void disable(); //!< enable the widget

public slots:
  void update();    //!< Refresh the form and query for current filenames
  void parse(const QString &, const QString &, const int &, 
	     const QHash<QString,QString> &); //!< parse command replies

signals:
  void cmdReady(const QString &, const QString &);  //!< command ready to send to ISIS 
  void cmdWait(const QString &, const QString &, const int &); //!< command ready, send to ISIS and wait up to a timeout interval
  void logMessage(const QString &);  //!< Signal with a message to log

private slots:  
  void getDateTag();     //!< Get the current UTC date tag
  void setRootName(const QString &);    //!< Set the rootname
  void setFileNames();   //!< send the filenames to the ICs
  void setBlueNum(const QString &); //!< set the file counter for the Blue CCD
  void setRedNum(const QString &);  //!< set the file counter for the Red CCD

private:
  int modsID;           //!< MODS Instrument ID (MODS1 or MODS2)

  bool haveBlueName;    //!< A full blue filename exists
  QString bluePrefix;   //!< Blue CCD file prefix (e.g., "m1b")
  QString blueRootName; //!< Blue CCD file rootname (usually CCYYMMDD)
  QString blueFileNum;  //!< Blue CCD file number string (e.g., 0001)
  int blueCounter;      //!< Blue CCD file counter (1..9999)
  
  bool haveRedName;     //!< A full red filename exists
  QString redPrefix;    //!< Red CCD file prefix (e.g., "m1r")
  QString redRootName;  //!< Red CCD file rootname (usually CCYYMMDD)
  QString redFileNum;   //!< Red CCD file number string (e.g., 0001)
  int redCounter;       //!< Red CCD file counter (1..9999)

  QString rootName;     //!< File rootname, the CCYYMMDD UTC date

  QString errorStr;     //!< Error strings for error conditions

  // Entry and Display widgets and Action Buttons, 

  TextEntry *rootNameEntry;      //!< root name entry box

  ValueDisplay *blueRootDisplay; //!< Blue CCD root name display widget
  TextEntry *blueNumEntry;       //!< Blue CCD file number entry box

  ValueDisplay *redRootDisplay;  //!< Red CCD root name display widget
  TextEntry *redNumEntry;        //!< Red CCD file number entry box

  ActionButton *getDateButton;   //!< Get current UTC time button
  ActionButton *refreshButton;   //!< Refresh the filename form button
  ActionButton *applyButton;     //!< Apply filename button

};

#endif
