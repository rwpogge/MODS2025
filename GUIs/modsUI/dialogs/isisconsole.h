#ifndef ISISCONSOLE_H
#define ISISCONSOLE_H

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

class ISISConsole : public QWidget
{
  Q_OBJECT

public:
  ISISConsole(const QString &myTitle, const QString &myID, const int &myPort, 
	      const QString &isID, const QString &isHost,
	      const int &isPort, QWidget *parent=0);

public slots:
  void setID(const QString &); //!< Set the console's IMPv2 node name
  void setISISInfo(const QString &, const QString &, const int &); //!< Set the ISIS server info

private slots:
  void doCommand();    //!< Get a command from the command widget and process it
  void setTimeTag();   //!< Enable/Disable message time tagging
  void setLineWrap();  //!< Enable/Disable line wrapping
  void getRemHost();   //!< Get the fixed remote host ID from the remHostEntry widget
  void parseISIS();    //!< Parse and display communications from the ISIS client socket
  void clearDisplay(); //!< Clear the communications display
  void append(const QString &);  //!< Append a message string to the comm viewer
  void sendToISIS(const QString &, const QString &); //!< send a command via the ISIS server

signals:
  void logMessage(const QString &);  //!< Send a message to the parent's logging mechanism
  void shareMsg(const QString &);    //!< Share a message with the parent

private:
  TextHistEntry *commandEntry; //!< Command entry widget with history
  QLineEdit *remHostEntry;     //!< Widget to enter a fixed remote host ID
  QTextEdit *commDisplay;      //!< QTextEdit for the communications viewer
  QPushButton *clearButton;    //!< button to clear the terminal window
  QCheckBox   *timeTagCheck;   //!< Checkbox to enable/disable time tagging
  bool timeTag;                //!< Messages in the commDisplay tagged with the UTC date/time
  QCheckBox   *lineWrapCheck;  //!< Checkbox to enable/disable line wrapping
  bool wrapLines;              //!< Wrap lines in the commDisplay

  ISISClient *isis;  //!< ISIS Client for this console
  QString remHost;   //!< Fixed remote host ID to use (blank=none)
  QString clientID;  //!< Client ID (IMPv2 protocol)
  int clientPort;    //!< Client UDP socket port for bind()
  QString isisID;    //!< ISIS server ID (IMPv2 protocol)
  QString isisHost;  //!< ISIS server host
  int isisPort;      //!< ISIS server port on isisHost  QString isisID;
};

#endif
