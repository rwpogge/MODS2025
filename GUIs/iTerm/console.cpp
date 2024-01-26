#include <QtGui>

#include "iterm.h"
#include "console.h"
#include "texthistentry.h"
#include "isisclient.h"

// A widget to show the provide a low-level ISIS command client with a
// command history entry, traffic display with color coding on message
// types, and optional time tagging of communications and an option to
// designate a specific remote host to simplify use when talking
// primarily to a single IMPv2 host.

ITermConsole::ITermConsole(const QString &myCFG, QWidget *parent)
  : QWidget(parent)
{
  
  // Load the runtime configuration file that tells us what to do.  If
  // empty, use the compiled-in defaults

  cfgFile = myCFG;
  loadConfig(cfgFile);

  // Start building the main window...

  setWindowTitle(title);

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
  if (!remHost.isEmpty()) remHostEntry->setText(remHost);

  // Check box - Time Tag communications?

  timeTagCheck = new QCheckBox(tr("&Time Tag"),this);
  if (timeTag)
    timeTagCheck->setCheckState(Qt::Checked);
  else
    timeTagCheck->setCheckState(Qt::Unchecked);
  connect(timeTagCheck,SIGNAL(stateChanged(int)),this,SLOT(setTimeTag()));
  timeTagCheck->setToolTip(tr("Checking this prepends the date/time\nof receipt to messages in the console display."));

  // Check box - Wrap long lines in the commDisplay widget?

  lineWrapCheck = new QCheckBox(tr("&Wrap Lines"),this);
  if (wrapLines) {
    lineWrapCheck->setCheckState(Qt::Checked);
    commDisplay->setLineWrapMode(QTextEdit::WidgetWidth);
  } 
  else {
    lineWrapCheck->setCheckState(Qt::Unchecked);
    commDisplay->setLineWrapMode(QTextEdit::NoWrap);
  }
  connect(lineWrapCheck,SIGNAL(stateChanged(int)),this,SLOT(setLineWrap()));
  lineWrapCheck->setToolTip(tr("Checking this will cause long lines of text\nto be wrapped in the console display."));

  // Clear comm button

  clearButton = new QPushButton(tr("Clear"),this);
  connect(clearButton,SIGNAL(clicked()),this,SLOT(clearDisplay()));
  clearButton->setToolTip(tr("Clear the console display window."));

  quitButton = new QPushButton(tr("Quit"),this);
  connect(quitButton,SIGNAL(clicked()),this,SLOT(quit()));

  // Layout for the remote host entry, timetag check and clear screen options

  QHBoxLayout *optionLayout = new QHBoxLayout;
  optionLayout->addWidget(remHostLabel);
  optionLayout->addWidget(remHostEntry);
  optionLayout->addWidget(timeTagCheck);
  optionLayout->addStretch();
  optionLayout->addWidget(lineWrapCheck);
  optionLayout->addWidget(clearButton);
  optionLayout->addWidget(quitButton);

  // Vertical layout with the comm viewer and command/option widgets over/under

  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(commDisplay);
  layout->addLayout(commandLayout);
  layout->addLayout(optionLayout);

  setLayout(layout);  
  
  // Start the ISIS client service

  isis = new ISISClient(isisID,isisHost,isisPort,clientID,clientPort,this);
  connect(isis,SIGNAL(dataReady()),this,SLOT(parseISIS()));
  if (!isis->init()) {
    QString cmdErr = tr("Could not initialize ISIS client socket!"
			"\nReason:\n%1\n Is another client running on this port?\n"
			"ITermConsole aborting with fatal errors.").arg(isis->errorString());
    QMessageBox::critical(this,tr("Network Socket Error"),cmdErr); 
    exit(1);
  }
  append("STATUS: Started ISIS client service");

  // Success, ping the ISIS server

  sendToISIS(isisID,"PING");

}

// Append a communications string to the comm log viewer with the
// specified color and note the date/time it was logged

void ITermConsole::append(const QString &txt)
{
  if (txt.isEmpty()) return;

  // Work with a copy so we can clean it a little...

  QString commStr = txt;
  commStr.remove('\r');  // strip off any terminator characters

  // Put a timetag on the msgText string we use for display purposes,
  // if enabled.

  QString msgText;
  if (timeTag) {
    QDateTime dateTime = QDateTime::currentDateTime();
    if (useUTC)
      dateTime = dateTime.toUTC();

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

// set the time-tag flag

void ITermConsole::setTimeTag()
{
  if (timeTagCheck->isChecked())
    timeTag = true;
  else
    timeTag = false;
}

// setLineWrap() - enable/disable line wrapping

// setLineWrap() - enable/disable line wrapping in the commDisplay
// widget.  This slot is usually connectd to the lineWrapCheck
// checkbox widget.

void ITermConsole::setLineWrap()
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

void ITermConsole::doCommand()
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

void ITermConsole::sendToISIS(const QString &toAddr, const QString &msgStr)
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

void ITermConsole::getRemHost()
{
  remHost = remHostEntry->text().toUpper();
  remHostEntry->setText(remHost);
}

// Clear the command display

void ITermConsole::clearDisplay() 
{
  commDisplay->clear();
}

// Parse incoming messages from the ISIS client

void ITermConsole::parseISIS()
{

  // Show it on the console and emit logMessage() to whoever
  // else is logging IMPv2 comm traffic

  append(isis->text());
  emit logMessage(isis->text());

  // Ignore invalid messages (aka "junk strings")

  if (!isis->isValid()) return;

  // We got a valid message, see if we need to do anything with it

  QString msgStr = isis->body();
  msgStr.simplified();

  if (msgStr.isEmpty()) return; // empty messages ignored

  // This tool is a dumb terminal and only responds to a very limited
  // set of commands, starting with PING

  QString cmdWord = msgStr.section(" ",0,0);
  QString remHost = isis->sender();

  // We got PINGed, PONG back

  if (cmdWord.compare("PING",Qt::CaseInsensitive)==0)
    sendToISIS(remHost,"PONG");

}

// Set the console's host ID

void ITermConsole::setID(const QString &newID)
{
  clientID = newID;
}

// Set the console's ISIS server ID, Host, and Port info, and pass it
// along to the ISIS client instance.

void ITermConsole::setISISInfo(const QString &isID, const QString &isHost, 
			       const int &isPort)
{
  isisID = isID;
  isisHost = isHost;
  isisPort = isPort;
  isis->setServer(isisID,isisHost,isisPort);
}

// Open and parse the contents of the runtime configuration file

void ITermConsole::loadConfig(const QString &cfg)
{
  bool ok;

  // Set the runtime defaults set at compile time

  title      = ITERM_SESSION_NAME;
  isisID     = ISIS_SERVER_ID;
  isisHost   = ISIS_SERVER_HOST;
  isisPort   = ISIS_SERVER_PORT;
  clientPort = ITERM_PORT;
  clientID   = ITERM_ID;
  remHost.clear();
  timeTag = false;
  useUTC = false;

  if (cfg.isEmpty()) return; // null config file means "use defaults"

  // Create a QFile instance for the config file and open readonly

  QFile inFile(cfg);
  if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QString errMsg = tr("Could not open configuration file %1!\n"
			"Reason:\n %2\nSystem default settings loaded instead.").arg(cfg).arg(inFile.errorString());
    QMessageBox::warning(this,tr("Configuration File Load Error"),errMsg); 
    return;
  }

  // We have a config file, read it.

  // Read in the table line-by-line, parsing as we go.  # denotes
  // comments (either whole lines or inline comments), and we skip
  // spaces.

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

	// We have a keyword, parse it.  On any errors, use the
	// default and ignore unknown keywords.

	if (!itemStr.isEmpty()) {
	  QString keyWord = itemStr.section(' ',0,0);
	  if (keyWord.compare("ISISID",Qt::CaseInsensitive)==0) {
	    isisID = itemStr.section(' ',1,1);
	    if (isisID.isEmpty()) isisID = ISIS_SERVER_ID;
	  }
	  else if (keyWord.compare("ISISHOST",Qt::CaseInsensitive)==0) {
	    isisHost = itemStr.section(' ',1,1);
	    if (isisID.isEmpty()) isisID = ISIS_SERVER_HOST;
	  }
	  else if (keyWord.compare("ISISPORT",Qt::CaseInsensitive)==0) {
	    isisPort = itemStr.section(' ',1,1).toInt(&ok,10);
	    if (!ok || isisPort<=0) isisPort=ISIS_SERVER_PORT;
	  }
	  else if (keyWord.compare("NAME",Qt::CaseInsensitive)==0) {
	    title = itemStr.section(' ',1,-1);
	    if (title.isEmpty()) title = ITERM_SESSION_NAME;
	  }
	  else if (keyWord.compare("ID",Qt::CaseInsensitive)==0) {
	    clientID = itemStr.section(' ',1,1);
	    if (clientID.isEmpty()) clientID = ITERM_ID;
	  }
	  else if (keyWord.compare("PORT",Qt::CaseInsensitive)==0) {
	    clientPort = itemStr.section(' ',1,1).toInt(&ok,10);
	    if (!ok || clientPort<=0) clientPort=ITERM_PORT;
	  }
	  else if (keyWord.compare("REMHOST",Qt::CaseInsensitive)==0) {
	    remHost = itemStr.section(' ',1,-1).toUpper();
	    if (remHost.isEmpty()) remHost.clear();
	  }
	  else if (keyWord.compare("TIMETAG",Qt::CaseInsensitive)==0) {
	    timeTag = true;
	  }
	  else if (keyWord.compare("NOTIMETAG",Qt::CaseInsensitive)==0) {
	    timeTag = false;
	  }
	  else if (keyWord.compare("LocalTime",Qt::CaseInsensitive)==0) {
	    useUTC = false;
	  }
	  else if (keyWord.compare("UTCTime",Qt::CaseInsensitive)==0) {
	    useUTC = true;
	  }
	  else if (keyWord.compare("WRAP",Qt::CaseInsensitive)==0) {
	    wrapLines = true;
	  }
	  else if (keyWord.compare("NOWRAP",Qt::CaseInsensitive)==0) {
	    wrapLines = false;
	  }
	}
      }
    }
  } while (!line.isNull());
  inFile.close();
}

// Quit the ITerm Session

void ITermConsole::quit()
{
  QMessageBox::StandardButton ret;
  ret = QMessageBox::question(this, title,
			      tr("Are you sure you want to quit?"),
			      QMessageBox::Yes | 
			      QMessageBox::No);
  if (ret == QMessageBox::No)
    return;
  
  qApp->quit();
}
