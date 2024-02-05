#ifndef CONSOLE_H
#define CONSOLE_H

#include <QWidget>

class QString;
class QTextEdit;
class QLabel;
class QPushButton;
class QCheckBox;
class QDateTime;
class QLineEdit;

// custom classes

class ISISClient;
class TextHistEntry;

class ITermConsole : public QWidget
{
  Q_OBJECT

public:
  ITermConsole(const QString &myCFG, QWidget *parent=0);
  void loadConfig(const QString &); //!< load and parse the runtime config file

public slots:
  void setID(const QString &); //!< Set the console's IMPv2 node name
  void setISISInfo(const QString &, const QString &, const int &); //!< Set the ISIS server info

private slots:
  void doCommand();    //!< Get a command from the command widget and process it
  void setTimeTag();   //!< Enable/Disable time tagging
  void setLineWrap();  //!< Enable/Disable display line wrapping
  void getRemHost();   //!< Get the fixed remote host ID from the remHostEntry widget
  void parseISIS();    //!< Parse and display communications from the ISIS client socket
  void clearDisplay(); //!< Clear the communications display
  void append(const QString &);  //!< Append a message string to the comm viewer
  void sendToISIS(const QString &, const QString &); //!< send a command via the ISIS server
  void quit();         //!< Terminate the ITerm session

signals:
  void logMessage(const QString &);  //!< Emit message traffic to whoever might be logging

private:
  TextHistEntry *commandEntry; //!< Command entry widget with history
  QLineEdit *remHostEntry;     //!< Widget to enter a fixed remote host ID
  QTextEdit *commDisplay;      //!< QTextEdit for the communications viewer
  QPushButton *clearButton;    //!< button to clear the terminal window
  QPushButton *quitButton;     //!< button to quit the ITerm session
  QCheckBox   *timeTagCheck;   //!< Checkbox to enable/disable message time-tagging
  bool timeTag;                //!< if true, tag entries with date/time
  bool useUTC;                 //!< if true, timetags are UTC, otherwise local time
  QCheckBox   *lineWrapCheck;  //!< Checkbox to enable/disable line wrapping in the display
  bool wrapLines;              //!< if true, wrap lines inside the commDisplay widget

  ISISClient *isis;  	       //!< ISIS Client instance for this console
  QString remHost;   	       //!< Fixed remote host ID to use (blank=none)
  QString title;     	       //!< Window Title (name of the control panel)
  QString clientID;  	       //!< Client ID (IMPv2 protocol)
  int clientPort;    	       //!< Client UDP socket port for bind()
  QString isisID;    	       //!< ISIS Server ID
  QString isisHost;  	       //!< ISIS Server Host IP address
  int isisPort;      	       //!< ISIS server IP port number
  QString cfgFile;   	       //!< Runtime configuration file
};

#endif
