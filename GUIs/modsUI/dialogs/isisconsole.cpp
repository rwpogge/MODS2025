#include <QtWidgets>

#include "isisconsole.h"
#include "texthistentry.h"
#include "isisclient.h"
#include "mods.h"

// A widget to show the provide a low-level ISIS command client with a
// command history entry, traffic display with color coding on message
// types, and optional time tagging of communications and an option to
// designate a specific remote host to simplify use when talking
// primarily to a single IMPv2 host.

ISISConsole::ISISConsole(const QString &myTitle, const QString &myID, const int &myPort, 
			 const QString &isID, const QString &isHost, const int &isPort, 
			 QWidget *parent) 
  : QWidget(parent)
{
  
  // Get our ID and port assignments

  clientID = myID;
  clientPort = myPort;

  // Set the ISIS info using the data passed by the constructor.  We
  // can redefine these during runtime using the setISISInfo() method.

  isisID = isID;
  isisHost = isHost;
  isisPort = isPort;

  // Title for this instance

  QLabel *consoleTitle = new QLabel(myTitle,this);
  consoleTitle->setFont(QFont("Helvetica",(int)(1.5*appFontSize),QFont::Normal));

  // Big text widget, readonly, to show the comm traffic. Use default
  // line wrapping like a regular xterm screen.

  commDisplay = new QTextEdit(this);
  commDisplay->setReadOnly(true);

  // Command Line Interface widget

  QLabel *commandLabel = new QLabel(tr("Command:"));

  commandEntry  = new TextHistEntry("","",0,256,this);
  connect (commandEntry,SIGNAL(dataReady(const QString &)),this,SLOT(doCommand()));
  commandEntry->clearHistory();

  QHBoxLayout *commandLayout = new QHBoxLayout;
  commandLayout->addWidget(commandLabel);
  commandLayout->addWidget(commandEntry);

  // A specific remote host entry box

  QLabel *remHostLabel = new QLabel(tr("Remote Host:"));

  remHostEntry = new QLineEdit(this);
  connect(remHostEntry,SIGNAL(editingFinished()),
	  this,SLOT(getRemHost()));
  remHostEntry->setMaxLength(8);  // 8 chars max
  int hmx = (int)(0.8*remHostEntry->minimumSizeHint().height());
  remHostEntry->setMaximumHeight(hmx);
  int mxw = (int)(0.5*8*remHostEntry->minimumSizeHint().width());
  remHostEntry->setMaximumWidth(mxw);
  remHostEntry->setToolTip(tr("Enter a host ID here to send\nmessages to a specific remote host w/o typing >XX"));

  // Check box - Time Tag communications?

  timeTagCheck = new QCheckBox(tr("&Time Tag"),this);
  timeTag = true;
  timeTagCheck->setCheckState(Qt::Checked);
  connect(timeTagCheck,SIGNAL(stateChanged(int)),this,SLOT(setTimeTag()));
  timeTagCheck->setToolTip(tr("Checking this prepends the date/time\nof receipt to the message in the console display."));

  // Wrap lines in the commDisplay widget?

  wrapLines = true;
  lineWrapCheck = new QCheckBox(tr("&Wrap Lines"),this);
  if (wrapLines) {
    lineWrapCheck->setCheckState(Qt::Checked); 
    commDisplay->setLineWrapMode(QTextEdit::WidgetWidth);
  }
  else {
    commDisplay->setLineWrapMode(QTextEdit::NoWrap);
    lineWrapCheck->setCheckState(Qt::Unchecked);
  }
  connect(lineWrapCheck,SIGNAL(stateChanged(int)),this,SLOT(setLineWrap()));
  lineWrapCheck->setToolTip(tr("Checking this will enable wrapping long lines\nof text inside the display window."));

  // Clear comm button

  clearButton = new QPushButton(tr("Clear"),this);
  connect(clearButton,SIGNAL(clicked()),this,SLOT(clearDisplay()));
  clearButton->setToolTip(tr("Clear the console display window."));

  // Layout for the remote host entry, time-tag and line-wrap check
  // boxes and clear-screen button.

  QHBoxLayout *optionLayout = new QHBoxLayout;
  optionLayout->addWidget(remHostLabel);
  optionLayout->addWidget(remHostEntry);
  optionLayout->addWidget(timeTagCheck);
  optionLayout->addStretch();
  optionLayout->addWidget(lineWrapCheck);
  optionLayout->addWidget(clearButton);

  // Vertical layout with the comm viewer and command/option widgets over/under

  QVBoxLayout *commLayout = new QVBoxLayout;
  commLayout->addWidget(commDisplay);
  commLayout->addLayout(commandLayout);
  commLayout->addLayout(optionLayout);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(consoleTitle,0,Qt::AlignHCenter);
  mainLayout->addLayout(commLayout);
  // mainLayout->setContentsMargins(0,0,0,0);
  setLayout(mainLayout);  
  
  // Start the ISIS client service

  isis = new ISISClient(isisID,isisHost,isisPort,clientID,clientPort,this);
  connect(isis,SIGNAL(dataReady()),this,SLOT(parseISIS()));
  if (!isis->init()) {
    QString cmdErr = tr("Could not initialize ISIS client socket!"
			"\nReason:\n%1\n Is another client running on this port?\n"
			"ISISConsole aborting with fatal errors.").arg(isis->errorString());
    QMessageBox::critical(this,tr("Network Socket Error"),cmdErr); 
    exit(1);
  }
  append("STATUS: Started ISIS client service");

  // Success, ping the ISIS server

  sendToISIS(isisID,"PING");

}

// Append a communications string to the comm log viewer with the
// specified color and note the date/time it was logged

void ISISConsole::append(const QString &txt)
{
  if (txt.isEmpty()) return;

  // Work with a copy so we can clean it a little...

  QString commStr = txt;
  commStr.remove('\r');  // strip off any terminator characters

  // Put a timetag on the msgText string we use for display purposes,
  // if enabled.

  QString msgText;
  if (timeTag) {
    QDateTime dateTime = QDateTime::currentDateTime().toUTC();
    msgText.append(dateTime.toString("[yyyy-MM-ddThh:mm:ss.zzz] "));
  }
  msgText.append(commStr);

  // Color code the entry by the presence of certain keywords

  if (commStr.contains("ERROR:",Qt::CaseInsensitive))
    commDisplay->setTextColor(Qt::red);
  else if (commStr.contains("WARNING:",Qt::CaseInsensitive))
    commDisplay->setTextColor(QColor(255,165,0,255));  // orange for warning
  else if (commStr.contains("FATAL:",Qt::CaseInsensitive) ||
	   commStr.contains("CRITCAL:",Qt::CaseInsensitive))
    commDisplay->setTextColor(Qt::magenta);
  else if (commStr.contains("STATUS:",Qt::CaseInsensitive))
    commDisplay->setTextColor(Qt::blue);
  else if (commStr.contains("EXEC:",Qt::CaseInsensitive))
    commDisplay->setTextColor(Qt::darkYellow);
  else if (commStr.contains("DONE:",Qt::CaseInsensitive))
    commDisplay->setTextColor(Qt::blue);
  else
    commDisplay->setTextColor(Qt::black);

  // And put it into the communications traffic display, setting the
  // cursor so that the latest entry is visible in the slider box

  commDisplay->append(msgText);
  commDisplay->setTextColor(Qt::black); 
  commDisplay->moveCursor(QTextCursor::End);
  commDisplay->moveCursor(QTextCursor::StartOfLine);
}

// setTimeTag() - set the time-tag flag (slot invoked by the
// timeTagCheck checkbox widget)

void ISISConsole::setTimeTag()
{
  if (timeTagCheck->isChecked())
    timeTag = true;
  else
    timeTag = false;
}

// setLineWrap() - enable/disable line wrapping in the commDisplay
// widget.  This slot is usually connectd to the lineWrapCheck
// checkbox widget.

void ISISConsole::setLineWrap()
{
  if (lineWrapCheck->isChecked()) {
    wrapLines = true;
    commDisplay->setLineWrapMode(QTextEdit::WidgetWidth);
  }
  else {
    wrapLines = false;
    commDisplay->setLineWrapMode(QTextEdit::NoWrap);
  }
}

//---------------------------------------------------------------------------
//
// doCommand() slot - invoked by the command entry widget
//

void ISISConsole::doCommand()
{
  QString cmdStr;

  cmdStr = commandEntry->value();
  commandEntry->clear();

  cmdStr = cmdStr.trimmed();  // Clean up whitespace & terminators
  if (cmdStr.isEmpty()) return;

  // Does the command start with >?  If so, decompose the command
  // string into the destination host and message body and send to the
  // ISIS server.  Otherwise, send the command string as-is to the
  // remote host, or complain if no remote host has been specified.

  if (cmdStr.startsWith(">")) {
    QString msgStr = cmdStr;
    msgStr.remove(">");
    QString dest = msgStr.section(" ",0,0).toUpper();
    QString body = msgStr.section(" ",1,-1);
    sendToISIS(dest,body);
  }
  else {
    if (remHost.isEmpty()) {
      append("ERROR: Commands must be preceded by \'>HostID\' if no remote host is specified");
      return;
    }
    sendToISIS(remHost,cmdStr);
  }

}

//
// Send a message to an ISIS client or server
//
// Wrapper for the isis->send() method, so we can do logging and other
// fun stuff
//

void ISISConsole::sendToISIS(const QString &toAddr, const QString &msgStr)
{
  if (msgStr.isEmpty()) return;
  if (toAddr.isEmpty()) return;

  // Show it to us on the comm viewer...

  QString fullMsg = QString("%1>%2 %3").arg(clientID,toAddr,msgStr);
  append(fullMsg);   

  // ... and emit a signal to whoever else logs IMPv2 traffic

  emit logMessage(fullMsg);

  // Have the ISIS client instance send it...

  isis->send(toAddr,msgStr);

}

// Get a specific remote host ID from the remHostEntry widget

void ISISConsole::getRemHost()
{
  remHost = remHostEntry->text().toUpper();
  remHostEntry->setText(remHost);
}

// Clear the command display

void ISISConsole::clearDisplay() 
{
  commDisplay->clear();
}

// Parse incoming messages from the ISIS client

void ISISConsole::parseISIS()
{

  // Show it on the console and emit logMessage() to whoever
  // else is logging IMPv2 comm traffic

  append(isis->text());
  emit logMessage(isis->text());

  // Ignore invalid messages (aka "junk strings")

  if (!isis->isValid()) return;

  // We got a valid message, see if we need to do anything with it

  QString msgStr = isis->body();
  msgStr = msgStr.simplified(); // TODO: Evaluate this line. Why is it here? 

  if (msgStr.isEmpty()) return; // empty messages ignored

  // This tool is a dumb terminal and only responds to a very limited
  // set of commands, starting with PING

  QString cmdWord = msgStr.section(" ",0,0);
  QString remHost = isis->sender();

  // We got PINGed, PONG back

  if (cmdWord.compare("PING",Qt::CaseInsensitive)==0) {
    sendToISIS(remHost,"PONG");
    return;
  }

  // Further processing depends on the message type

  switch (isis->type()) {
  case ISISClient::ERROR:
  case ISISClient::WARNING:
  case ISISClient::FATAL:
  case ISISClient::STATUS:
  case ISISClient::DONE:
    emit shareMsg(isis->text());  // share the return message with the parent
    break;
  }

}

// Set the console's host ID

void ISISConsole::setID(const QString &newID)
{
  clientID = newID;
}

// Set the console's ISIS server ID, Host, and Port info, and pass it
// along to the ISIS client instance.

void ISISConsole::setISISInfo(const QString &isID, const QString &isHost, const int &isPort)
{
  isisID = isID;
  isisHost = isHost;
  isisPort = isPort;
  isis->setServer(isisID,isisHost,isisPort);
}
