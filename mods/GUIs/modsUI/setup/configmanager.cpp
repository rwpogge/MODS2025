#include <QtWidgets>
#include <QtNetwork>
#include <math.h>

#include "configmanager.h"
#include "instconfig.h"
#include "fileconfig.h"

#include "textentry.h"
#include "texthistentry.h"
#include "textdisplay.h"
#include "valuedisplay.h"
#include "lightbutton.h"
#include "actionbutton.h"
#include "led.h"

#include "commlog.h"

#include "isisclient.h"

ConfigManager::ConfigManager(const QString &myID, 
			     const bool &useM1, const bool &useM2, 
			     const int &myPort, QWidget *parent)
  : QWidget(parent)
{
  clientID = myID;
  clientPort = myPort;
  useMODS1 = useM1;
  useMODS2 = useM2;

  // Basic initializations

  queue = false;   // disable command dispatch queuing by default
  dispatchCadence = MODS_QUEUE_CADENCE;  // default dispatch queue cadence in msec
  cmdHost.clear(); // clear the command host hash

  // Default values.  If not "None", these are LBT prototypes

  observerStr = "None";
  partnerStr  = "NOPARTNER"; 
  propIDStr   = "NOTSPECIFIED";  
  piNameStr   = "NOTSPECIFIED"; 
  supportStr  = "None"; 
  telopsStr   = "None";  
  commentStr  = "None"; 
  
  //----------------------------------------------------------------
  //
  // Build the Instrument Setup Control Panel GUI
  //

  /*

  // MODS1 and MODS2 setup panels.  We setup both panels even if both
  // MODS are not enabled, and then disable the widget.  This allows
  // us to enable it later.

  modsConfig[MODS1] = new InstConfig(MODS1,this);
  connect(modsConfig[MODS1],SIGNAL(cmdReady(const QString &,const QString &)),
	  this,SLOT(sendToISIS(const QString &,const QString &)));
  connect(modsConfig[MODS1],SIGNAL(cmdWait(const QString &,const QString &,const int &)),
	  this,SLOT(sendAndWait(const QString &,const QString &,const int &)));
  connect(modsConfig[MODS1],SIGNAL(logMessage(const QString &)),
	  this,SLOT(showMessage(const QString &)));
  
  if (!useMODS1) modsConfig[MODS1]->disable();

  modsConfig[MODS2] = new InstConfig(MODS2,this);
  connect(modsConfig[MODS2],SIGNAL(cmdReady(const QString &,const QString &)),
	  this,SLOT(sendToISIS(const QString &,const QString &)));
  connect(modsConfig[MODS2],SIGNAL(cmdWait(const QString &,const QString &,const int &)),
	  this,SLOT(sendAndWait(const QString &,const QString &,const int &)));
  connect(modsConfig[MODS2],SIGNAL(logMessage(const QString &)),
	  this,SLOT(showMessage(const QString &)));

  if (!useMODS2) modsConfig[MODS2]->disable();

  // Make sure the setup panels are the same width and height.

  int cpw = qMax((int)(modsConfig[MODS1]->minimumSizeHint().width()),
		 (int)(modsConfig[MODS2]->minimumSizeHint().width()));

  int cph = qMax((int)(modsConfig[MODS1]->minimumSizeHint().height()),
		 (int)(modsConfig[MODS2]->minimumSizeHint().height()));

  modsConfig[MODS1]->setMinimumWidth(cpw);
  modsConfig[MODS1]->setMinimumHeight(cph);
  modsConfig[MODS2]->setMinimumWidth(cpw);
  modsConfig[MODS2]->setMinimumHeight(cph);

  */

  // Observer/Project Information panel

  QGroupBox *obsGroupBox = new QGroupBox(tr("Observer/Project Information"));
  obsGroupBox->setFont(QFont("Helvetica",appFontSize+2,QFont::Normal));
  obsGroupBox->setAlignment(Qt::AlignHCenter);

  QFormLayout *obsFormLayout = new QFormLayout;
  obsFormLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
  obsFormLayout->setFormAlignment(Qt::AlignCenter);  
  obsFormLayout->setLabelAlignment(Qt::AlignRight);

  // Observer name(s)

  observerEntry = new TextEntry("","",0,this);
  observerEntry->setEntryFocusPolicy(Qt::StrongFocus);
  observerEntry->setCommit(false);
  connect(observerEntry,SIGNAL(textReady(const QString &)),
	  this,SLOT(setObserver(const QString &)));
  obsFormLayout->addRow(tr("Observer(s):"),observerEntry);

  // Partner name (default, scripts may change locally)

  partnerEntry = new TextEntry("","",0,this);
  partnerEntry->setEntryFocusPolicy(Qt::StrongFocus);
  partnerEntry->setCommit(false);
  connect(partnerEntry,SIGNAL(textReady(const QString &)),
	  this,SLOT(setPartner(const QString &)));
  obsFormLayout->addRow(tr("Partner(s):"),partnerEntry);

  // Proposal ID (default, scripts may change locally)

  propIDEntry = new TextEntry("","",0,this);
  propIDEntry->setEntryFocusPolicy(Qt::StrongFocus);
  propIDEntry->setCommit(false);
  connect(propIDEntry,SIGNAL(textReady(const QString &)),
	  this,SLOT(setPropID(const QString &)));
  obsFormLayout->addRow(tr("PropID:"),propIDEntry);

  // PI Name (default, scripts may change locally)

  piNameEntry = new TextEntry("","",0,this);
  piNameEntry->setEntryFocusPolicy(Qt::StrongFocus);
  piNameEntry->setCommit(false);
  connect(piNameEntry,SIGNAL(textReady(const QString &)),
	  this,SLOT(setPIName(const QString &)));
  obsFormLayout->addRow(tr("PI Name:"),piNameEntry);

  // Support Scientist(s)

  supportEntry = new TextEntry("","",0,this);
  supportEntry->setEntryFocusPolicy(Qt::StrongFocus);
  supportEntry->setCommit(false);
  connect(supportEntry,SIGNAL(textReady(const QString &)),
	  this,SLOT(setSupport(const QString &)));
  obsFormLayout->addRow(tr("Support:"),supportEntry);

  // Telescope Operator(s)

  telopsEntry = new TextEntry("","",0,this);
  telopsEntry->setEntryFocusPolicy(Qt::StrongFocus);
  telopsEntry->setCommit(false);
  connect(telopsEntry,SIGNAL(textReady(const QString &)),
	  this,SLOT(setTelops(const QString &)));
  obsFormLayout->addRow(tr("TelOps:"),telopsEntry);

  // General Comment

  commentEntry = new TextEntry("","",0,this);
  commentEntry->setEntryFocusPolicy(Qt::StrongFocus);
  commentEntry->setCommit(false);
  connect(commentEntry,SIGNAL(textReady(const QString &)),
	  this,SLOT(setComment(const QString &)));
  obsFormLayout->addRow(tr("Comment:"),commentEntry);

  // Clear and Save buttons

  QHBoxLayout *obsButtonLayout = new QHBoxLayout;

  clearButton = new ActionButton(tr("Clear"),this);
  clearButton->setFont(QFont("Helvetica",appFontSize,QFont::Normal));
  connect(clearButton,SIGNAL(clicked()),this,SLOT(clearForm()));

  applyButton = new ActionButton(tr("Apply"),this);
  applyButton->setFont(QFont("Helvetica",appFontSize,QFont::Normal));
  connect(applyButton,SIGNAL(clicked()),this,SLOT(sendConfig()));

  saveButton = new ActionButton(tr("Save"),this);
  saveButton->setFont(QFont("Helvetica",appFontSize,QFont::Normal));
  connect(saveButton,SIGNAL(clicked()),this,SLOT(saveConfig()));

  reloadButton = new ActionButton(tr("Reload"),this);
  reloadButton->setFont(QFont("Helvetica",appFontSize,QFont::Normal));
  connect(reloadButton,SIGNAL(clicked()),this,SLOT(loadConfig()));

  obsButtonLayout->addWidget(clearButton);
  obsButtonLayout->addStretch();
  obsButtonLayout->addWidget(reloadButton);
  obsButtonLayout->addStretch();
  obsButtonLayout->addWidget(applyButton);
  obsButtonLayout->addStretch();
  obsButtonLayout->addWidget(saveButton);

  // Observer/Project form main layout

  QVBoxLayout *obsLayout = new QVBoxLayout;
  obsLayout->addLayout(obsFormLayout);
  obsLayout->addLayout(obsButtonLayout);
  //obsLayout->addStretch();

  obsGroupBox->setLayout(obsLayout);

  // FITS Image Filename entry forms for each instrument

  modsFiles[MODS1] = new FileConfig(MODS1,this);
  connect(modsFiles[MODS1],SIGNAL(cmdReady(const QString &,const QString &)),
	  this,SLOT(sendToISIS(const QString &,const QString &)));
  connect(modsFiles[MODS1],SIGNAL(cmdWait(const QString &,const QString &,const int &)),
	  this,SLOT(sendAndWait(const QString &,const QString &,const int &)));
  connect(modsFiles[MODS1],SIGNAL(logMessage(const QString &)),
	  this,SLOT(showMessage(const QString &)));
  
  if (!useMODS1) modsFiles[MODS1]->disable();

  modsFiles[MODS2] = new FileConfig(MODS2,this);
  connect(modsFiles[MODS2],SIGNAL(cmdReady(const QString &,const QString &)),
	  this,SLOT(sendToISIS(const QString &,const QString &)));
  connect(modsFiles[MODS2],SIGNAL(cmdWait(const QString &,const QString &,const int &)),
	  this,SLOT(sendAndWait(const QString &,const QString &,const int &)));
  connect(modsFiles[MODS2],SIGNAL(logMessage(const QString &)),
	  this,SLOT(showMessage(const QString &)));

  if (!useMODS2) modsFiles[MODS2]->disable();

  // Create a group box and stack the widgets vertically

  QGroupBox *fileGroupBox = new QGroupBox(tr("FITS Image File Names"));
  fileGroupBox->setFont(QFont("Helvetica",appFontSize+2,QFont::Normal));
  fileGroupBox->setAlignment(Qt::AlignHCenter);

  QVBoxLayout *fileLayout = new QVBoxLayout;
  fileLayout->addWidget(modsFiles[MODS1]);
  fileLayout->addWidget(modsFiles[MODS2]);
  //fileLayout->addStretch();
  fileGroupBox->setLayout(fileLayout);

  // Status display, nestled between an update button and a comm log button

  refreshButton = new ActionButton(tr("Refresh"),this);
  connect(refreshButton,SIGNAL(clicked()),this,SLOT(update()));

  statusDisplay = new TextDisplay("","",0,this);
  statusDisplay->setTimer(MODS_STATUSMSG_TIME); // status display message persistence

  // Communications log (modeless dialog), and the view/hide button
  
  commLog = new CommLog(tr("MODS Configuration Manager Log"),640,256,this);
  connect(commLog,SIGNAL(logHidden()),this,SLOT(setLogButton()));

  commLogButton = new ActionButton(tr("View Log"));
  connect(commLogButton,SIGNAL(clicked()),this,SLOT(toggleCommLog()));

  QHBoxLayout *buttonLayout = new QHBoxLayout;
  buttonLayout->addWidget(refreshButton);
  buttonLayout->addWidget(statusDisplay);
  buttonLayout->addWidget(commLogButton);

  // The masthead has the OSU and LBT logos flanking the page title, centered

  QLabel *mainLabel = new QLabel(tr("MODS Instrument Setup"));
  mainLabel->setFont(QFont("Helvetica",(2*appFontSize),QFont::Normal));
  QLabel *osuLogo = new QLabel();
  osuLogo->setPixmap(QPixmap(":/images/OSUlogo.png"));
  QLabel *lbtLogo = new QLabel();
  lbtLogo->setPixmap(QPixmap(":/images/LBTlogo.png"));
  
  QGridLayout *mastheadLayout = new QGridLayout;
  mastheadLayout->addWidget(osuLogo,0,0,Qt::AlignLeft);
  mastheadLayout->addWidget(mainLabel,0,1,Qt::AlignCenter);
  mastheadLayout->addWidget(lbtLogo,0,2,Qt::AlignRight);

  // Layout the widgets that make up the setup panel into a 2x2 grid

  /*
  QGridLayout *mainGridLayout = new QGridLayout();
  mainGridLayout->addWidget(modsConfig[MODS1],0,0);
  mainGridLayout->addWidget(modsConfig[MODS2],0,1);
  mainGridLayout->addWidget(obsGroupBox,1,0);
  mainGridLayout->addWidget(fileGroupBox,1,1);
  */

  QHBoxLayout *obsFileLayout = new QHBoxLayout;
  obsFileLayout->addWidget(obsGroupBox);
  obsFileLayout->addWidget(fileGroupBox);

  // Assemble the main layout

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addLayout(mastheadLayout);
  //mainLayout->addLayout(mainGridLayout);
  mainLayout->addLayout(obsFileLayout);
  mainLayout->addStretch();
  mainLayout->addLayout(buttonLayout); 
  mainLayout->setContentsMargins(0,0,0,0);
  setLayout(mainLayout);

  //----------------------------------------------------------------

  // The various communications and timing services

  // Create the ISIS client socket but don't initialize it yet. The
  // ISIS server ID, host, and port are sensible defaults defined at
  // compile-time.  These can be changed later with the setISISInfo()
  // method.

  isis = new ISISClient(MODS_ISIS_SERVER_ID,MODS_ISIS_SERVER_HOST,
			MODS_ISIS_SERVER_PORT,clientID,myPort,this);
  connect(isis,SIGNAL(dataReady()),this,SLOT(parseISIS()));
  connect(isis,SIGNAL(logMessage(const QString &)),this,
	  SLOT(passMessage(const QString &)));

  // Anything else for the constructor to do?

}

//---------------------------------------------------------------------------
//
// doCommand() - execute a command
//
// Executes a command, either internally or passing it to the ISIS
// server to route to a specific ISIS client host.
//
// Note that any GUI widgets that need updating will be updated by
// subsequent reply-receipt parsing from the host that executes the
// requested command.
//

void ConfigManager::doCommand(const QString &cmdStr, QString *replyStr)
{
  QString msgStr;
  QString cmdKey;

  if (cmdStr.isEmpty()) return;

  replyStr->clear();

  // If it starts with >, it is a raw direct command for an IMPv2
  // node from the local client.  Send and return.  

  if (cmdStr.startsWith(">")) {
    msgStr = cmdStr;
    msgStr.remove(">");
    QString dest = msgStr.section(" ",0,0).toUpper();
    QString body = msgStr.section(" ",1,-1);
    sendToISIS(dest,body);
    remoteCmd = false;
    return;
  }

  // Command to execute, split into command verb and arguments

  QString cmdWord = cmdStr.section(" ",0,0);
  QString cmdArgs = cmdStr.section(" ",1,-1);

  //----------------------------------------------------------------
  // 
  // Command Tree
  //

  // PING: We got PINGed, PONG back (enqueue)

  if (cmdWord.compare("PING",Qt::CaseInsensitive)==0) {
    if (remoteCmd)
      sendAndWait(remoteHost,"PONG",queue);
    remoteCmd = false;
    return;
  }

  // PONG: We got PONGed by an ISIS node, just swallow it.

  else if (cmdWord.compare("PONG",Qt::CaseInsensitive)==0) {
    remoteCmd = false;
    return;
  }

  // Here we implement interactive commands as needed.  Only a few
  // at present, not likely to be more

  // Save the config for restarts

  else if (cmdStr.compare("saveconfig",Qt::CaseInsensitive)==0) {
    saveConfig();
    *replyStr = QString("DONE: %1 observer/project startup configuration saved").arg(cmdStr);
    remoteCmd = false;
    return;
  }    

  // Unknown command word, complain and reset the remoteCmd.
  
  else {
    *replyStr = QString("ERROR: Unknown/Unimplemented setup manager command %1").arg(cmdWord);
    if (!remoteCmd)
      addStatus(*replyStr,Qt::red,false);
    remoteCmd = false;
  }
  
}

//---------------------------------------------------------------------------
// 
// parseISIS() - parse an IMPv2 message into keywords
//
// Parse messages, picking off those keyword=value pairs we care about
// and ignorning the rest
//
// This slot is connected to the dataReady() signal of an ISISClient
// instance.  It assumes, correctly, that the ISISClient has already
// parsed the message into components.  We check validity then proceed
// to process those messages of relevance to us.
//
// If we have received a command directive (REQ: or EXEC:), set the
// remoteCmd flag and execute it using the doCommand() method.
//

void ConfigManager::parseISIS()
{
  // TODO: Unused cmdFault flag. Commented out on 2-19-24
  bool execCmd = false;   // Flag: message is a command execution request
  bool cmdDone = false;   // Flag: message is a command completion
  // bool cmdFault = false;  // Flag: command completed with a fault

  QString typeStr;

  remoteCmd = false;

  // Log invalid messages for diagnostic purposes, but otherwise
  // ignore them

  if (!isis->isValid()) {
    commLog->append(isis->text(),Qt::red);
    return;
  }

  // We have a valid message, see if we need to do anything with it

  QString msgStr = isis->body();
  msgStr = msgStr.simplified(); // TODO: Evaluate this line. Why is it here? 

  if (msgStr.isEmpty()) { // Empty messages are ignored but logged
    commLog->append(isis->text(),Qt::blue);
    return;  
  }

  // Our response depends on the type of message we received, and who
  // sent it.  We also color-code the display of the response version
  // by message type

  remoteHost = isis->sender();

  // TODO: Unused cmdFault flag. Commented out on 2-19-24
  switch (isis->type()) {
  case ISISClient::ERROR:
    addStatus(QString("%1> %2").arg(remoteHost).arg(msgStr),Qt::red,true);
    typeStr = "ERROR:";
    cmdDone = true;
    // cmdFault = true;
    break;

  case ISISClient::WARNING:
    addStatus(QString("%1> %2").arg(remoteHost).arg(msgStr),QColor(255,165,0,255),true);
    typeStr = "WARNING:";
    cmdDone = false;
    // cmdFault = false;
    break;

  case ISISClient::FATAL:
    addStatus(QString("%1> %2").arg(remoteHost).arg(msgStr),Qt::magenta,true);
    typeStr = "FATAL:";
    cmdDone = true;
    // cmdFault = true;
    break;

  case ISISClient::DONE:
    addStatus(QString("%1> %2").arg(remoteHost).arg(msgStr),Qt::blue,true);
    typeStr = "DONE:";
    cmdDone = true;
    // cmdFault = false;
    break;

  case ISISClient::STATUS:
    addStatus(QString("%1> %2").arg(remoteHost).arg(msgStr),Qt::blue,true);
    typeStr = "STATUS:";
    cmdDone = false;
    // cmdFault = false;
    break;

  case ISISClient::EXEC:
  case ISISClient::REQ:
    commLog->append(QString("%1> %2").arg(remoteHost).arg(msgStr),Qt::blue);
    execCmd = true;
    break;

  default:
    commLog->append(QString("Uninterpretable ISIS message: %1").arg(isis->text()),
		    Qt::red);
    return;
    break;
  }

  // Did we receive a command to execute (REQ: or EXEC:)?

  if (execCmd) {
    remoteCmd = true;

    // Hand the command off to doCommand() to attempt to execute the
    // command.  doCommand() will handle any validation

    QString *replyStr = new QString("");
    doCommand(msgStr,replyStr); 

    // Return the response to the sender (if we have something to send
    // them yet)

    if (!replyStr->isEmpty()) 
      sendToISIS(remoteHost,*replyStr);

  }

  // We have received either a command status, command done, or
  // command done w/fault message.

  else {
    // The first word in the message body is the command that
    // generated this response.

    QString cmdWord = isis->body().section(" ",0,0);

    // Create a hash with the keyword=value pairs

    QHash<QString,QString> keyDict; // keyword dictionary
    int numKeys = isis->getKeys(msgStr,&keyDict);

    // Pass it along for processing by the various control panels that
    // make up this widget


    if (useMODS1) {
      if (remoteHost.compare(modsIEHost[MODS1],Qt::CaseInsensitive)==0 ||
	  remoteHost.compare(modsTCHost[MODS1],Qt::CaseInsensitive)==0) {
	//modsConfig[MODS1]->parse(isis->body(),isis->type(),keyDict);
      }
      else if (remoteHost.compare(modsBCHost[MODS1],Qt::CaseInsensitive)==0 ||
	       remoteHost.compare(modsRCHost[MODS1],Qt::CaseInsensitive)==0) {
	//modsConfig[MODS1]->parse(isis->body(),isis->type(),keyDict);
	modsFiles[MODS1]->parse(remoteHost,isis->body(),isis->type(),keyDict);
      }
    }

    if (useMODS2) {
      if (remoteHost.compare(modsIEHost[MODS2],Qt::CaseInsensitive)==0 ||
	  remoteHost.compare(modsTCHost[MODS2],Qt::CaseInsensitive)==0) {
	//modsConfig[MODS2]->parse(isis->body(),isis->type(),keyDict);
      }
      else if (remoteHost.compare(modsBCHost[MODS2],Qt::CaseInsensitive)==0 ||
	       remoteHost.compare(modsRCHost[MODS2],Qt::CaseInsensitive)==0) {
	//modsConfig[MODS2]->parse(isis->body(),isis->type(),keyDict);
	modsFiles[MODS2]->parse(remoteHost,isis->body(),isis->type(),keyDict);
      }
    }

    // Share the message with the parent

    emit shareMsg(isis->text());

    // Now, build the command host key and see if this originated with
    // a previous remote host command handled by us.  If the command
    // transaction is complete, remove the item from the cmdHost hash.

    QString cmdKey = QString("%1-%2").arg(remoteHost).arg(cmdWord.toUpper());

    if (cmdHost.contains(cmdKey)) {
      QString replyHost = cmdHost.value(cmdKey);
      if (replyHost.compare("local",Qt::CaseInsensitive)!=0)
	sendToISIS(replyHost,QString("%1 %2").arg(typeStr).arg(isis->body()));
      if (cmdDone) cmdHost.remove(cmdKey);
    }

    // Anything else we might mine from this?

    if (numKeys > 0) { // use Qt's built-in Java-style iterator
      QHashIterator<QString,QString> i(keyDict);
      while (i.hasNext()) {
        i.next();
	QString keyStr = i.key();
	QString valStr = i.value();

	// Observer name(s)

	if (keyStr.compare("OBSERVER",Qt::CaseInsensitive)==0) {
	  observerStr = valStr;
	  observerEntry->setText(observerStr);
	}

	// Partner name(s)

	else if (keyStr.compare("PARTNER",Qt::CaseInsensitive)==0) {
	  partnerStr = valStr;
	  partnerEntry->setText(partnerStr);
	}

	// Proposal ID

	else if (keyStr.compare("PROPID",Qt::CaseInsensitive)==0) {
	  propIDStr = valStr;
	  propIDEntry->setText(propIDStr);
	}

	// Support scientist name(s)

	else if (keyStr.compare("SUPPORT",Qt::CaseInsensitive)==0) {
	  supportStr = valStr;
	  supportEntry->setText(supportStr);
	}

	// Telescope Operator name(s)

	else if (keyStr.compare("TELOPS",Qt::CaseInsensitive)==0) {
	  telopsStr = valStr;
	  telopsEntry->setText(telopsStr);
	}

	// PI/CoI name(s)

	else if (keyStr.compare("PI_NAME",Qt::CaseInsensitive)==0) {
	  piNameStr = valStr;
	  piNameEntry->setText(piNameStr);
	}

	// User Header Comment 

	else if (keyStr.compare("COMMENT",Qt::CaseInsensitive)==0) {
	  commentStr = valStr;
	  commentEntry->setText(commentStr);
	}

	else {
	  // unrecognized keyword, move along...
	}
      }
    }

    // Some commands require additional handling, especially for faults

  }

}

// update() - Update the control panel

void ConfigManager::update()
{
  if (useMODS1) {
    //modsConfig[MODS1]->update();
    modsFiles[MODS1]->update();
  }
  if (useMODS2) {
    //modsConfig[MODS2]->update();
    modsFiles[MODS2]->update();
  }
  remoteCmd = false;

  // Re-read the startup config file (or try to) and set the
  // observer/project defaults accordingly

  loadConfig();
}

// toggleCommLog() - Hide/View the communications log

void ConfigManager::toggleCommLog()
{
  if (commLog->isHidden())
    commLog->show();
  else
    commLog->hide();
  commLog->activateWindow();
  setLogButton();
}

// setLogButton() - Set the communications log button label based on
// the visibility of the comm log

void ConfigManager::setLogButton()
{
  if (commLog->isHidden())
    commLogButton->setText(tr("View Log"));
  else
    commLogButton->setText(tr("Hide Log"));
}

// addStatus() - Add an entry to the command status display with
// color, and also put info into the communications log

void ConfigManager::addStatus(const QString &text, const QColor &color, const bool &logIt)
{
  if (text.isEmpty()) return;
  statusDisplay->setText(text,color);
  if (logIt)
    commLog->append(text,color);
}

//---------------------------------------------------------------------------
//
// ISIS Client Socket communications bits - most are convenience functions
//

// setID() - set our IMPv2 client host ID

void ConfigManager::setID(const QString &id)
{
  clientID = id;
  isis->setClient(clientID,clientPort);
}

// id() - return our IMPv2 client host ID

QString ConfigManager::id() 
{
  return clientID;
}

// setPort() - set our client socket Port number

void ConfigManager::setPort(const int &port)
{
  clientPort = port;
  isis->setClient(clientID,clientPort);
}

// setISISInfo() - Set the ISIS server ID, host, and port for this client

void ConfigManager::setISISInfo(const QString &id, const QString &host, 
			    const int &port)
{
  isisID = id;
  isisPort = port;
  isisHost = host;
  isis->setServer(isisID,isisHost,isisPort);
}

// startISISClient() - Start the ISIS client UDP socket service

void ConfigManager::startISISClient()
{
  addStatus("Starting ISIS client service...",Qt::blue,true);

  if (!isis->init()) {
    QString cmdErr = tr("Cannot initialize ISIS client socket!\n"
			"Reason:\n %1\n Is another client running on this port?\n"
			"%2 aborting with fatal errors.").arg(isis->errorString()).arg(MODS_SESSION_NAME);
    QMessageBox::critical(this,tr("Network Socket Error"),cmdErr); 
    exit(1);
  }

  // Set the isis message dispatcher queue cadence in milliseconds

  isis->setDispatchCadence(dispatchCadence);

  // Success, ping the ISIS server (explicit immediate dispatch)

  sendAndWait(isisID,"PING",MODS_IMMEDIATE_REQUEST);

}

//
// sendToISIS() - Send a message to an ISIS client or server
//
// Wrapper for the isis->send() method, so we can do logging and other
// fun stuff like dispatch queuing.
//

void ConfigManager::sendToISIS(const QString &toAddr, const QString &msgStr)
{
  if (msgStr.isEmpty()) return;
  if (toAddr.isEmpty()) return;

  QString fullMsg = QString("%1>%2 %3").arg(clientID,toAddr,msgStr);

  if (msgStr.contains("ERROR:",Qt::CaseInsensitive))
    commLog->append(fullMsg,Qt::red);
  else if (msgStr.contains("WARNING:",Qt::CaseInsensitive))
    commLog->append(fullMsg,QColor(255,165,0,255));
  else if (msgStr.contains("FATAL:",Qt::CaseInsensitive))
    commLog->append(fullMsg,Qt::magenta);
  else 
    commLog->append(fullMsg,Qt::blue);

  if (queue)
    isis->queue(toAddr,msgStr);
  else
    isis->send(toAddr,msgStr);
}

// sendAndWait() - Generalized Send and Wait. 
//   If timeout=0 send immediately.  
//   If timeout>0 send and then block and wait up to timeout for a reply.
//   If timeout<0 queue it to the isis client's message dispatcher

void ConfigManager::sendAndWait(const QString &toAddr, const QString &msgStr, 
			    const int &timeout)
{
  if (msgStr.isEmpty()) return;
  if (toAddr.isEmpty()) return;

  QString fullMsg = QString("%1>%2 %3").arg(clientID,toAddr,msgStr);

  if (msgStr.contains("ERROR:",Qt::CaseInsensitive))
    commLog->append(fullMsg,Qt::red);
  else if (msgStr.contains("WARNING:",Qt::CaseInsensitive))
    commLog->append(fullMsg,QColor(255,165,0,255));
  else if (msgStr.contains("FATAL:",Qt::CaseInsensitive))
    commLog->append(fullMsg,Qt::magenta);
  else 
    commLog->append(fullMsg,Qt::blue);

  // Send, Send-and-Wait, or Queue as required.

  if (timeout == 0)
    isis->send(toAddr,msgStr);
  else if (timeout > 0)
    isis->sendAndWait(toAddr,msgStr,timeout);
  else if (timeout < 0)
    isis->queue(toAddr,msgStr);

}

// passMessage() - Pass messages from child objects to our parent or
// siblings.

void ConfigManager::passMessage(const QString &msgStr)
{
  emit logMessage(msgStr);
}

// showMessage() - Show messages from child objects in the status
// display widget.  Display but do not log.

void ConfigManager::showMessage(const QString &msgStr)
{
  if (msgStr.contains("ABORT",Qt::CaseInsensitive) ||
      msgStr.contains("PANIC",Qt::CaseInsensitive))
    addStatus(msgStr,Qt::red,false);
  else
    addStatus(msgStr,Qt::blue,false); 
}

// cmdQueue() - Enable/Disable queued message dispatch for most
// commands (changes the behavior of the sendToISIS() method).

void ConfigManager::cmdQueue(const bool &flag)
{
  queue = flag;
}

// setDispatchCadence() - Set the dispatch queue cadence in
// milliseconds

void ConfigManager::setDispatchCadence(const int &twait)
{
  if (twait > 0) {
    dispatchCadence = twait;
    isis->setDispatchCadence(dispatchCadence);
  }
}

//
// parseShared() - parse a shared message from a sibling
//
// This slot is usually connected to the shareMsg() signal from
// sibling ISIS clients.  This is used to share status information
// with siblings, so if the message is blank or there are no
// keyword=value pairs to interpret and use to update our widget, we
// don't need to go any further.
//

void ConfigManager::parseShared(const QString &msgStr)
{
  if (msgStr.isEmpty()) return;

  // Use our ISIS parser to do the basic parsing into components.  Go
  // no further if it fails to recognize it as a valid IMPv2 protocol
  // message.

  if (!isis->parse(msgStr)) return;
  if (!isis->isValid()) return;

  // Extract the body, the keys, and the host name

  QString msgBody = isis->body();
  msgBody = msgBody.simplified(); // TODO: Evaluate this line. Why is it here? 
  if (msgBody.isEmpty()) return; // nothing to do...

  QHash<QString,QString> keyDict; // keyword dictionary
  int numKeys = isis->getKeys(msgStr,&keyDict);
  if (numKeys < 1) return;  // No keywords to interpret, nothing to do...

  QString remHost = isis->sender();
  // int msgType = isis->type();
  
  // We have something to interpret, pass it along to the various
  // widgets that need updated information

  if (useMODS1) {
    if (remHost.compare(modsIEHost[MODS1],Qt::CaseInsensitive)==0 ||
	remHost.compare(modsTCHost[MODS1],Qt::CaseInsensitive)==0) {
      //modsConfig[MODS1]->parse(isis->body(),isis->type(),keyDict);
    }
    else if (remHost.compare(modsBCHost[MODS1],Qt::CaseInsensitive)==0 ||
	     remHost.compare(modsRCHost[MODS1],Qt::CaseInsensitive)==0) {
      //modsConfig[MODS1]->parse(isis->body(),isis->type(),keyDict);
      modsFiles[MODS1]->parse(remHost,isis->body(),isis->type(),keyDict);
    }
  }

  if (useMODS2) {
    if (remHost.compare(modsIEHost[MODS2],Qt::CaseInsensitive)==0 ||
	remHost.compare(modsTCHost[MODS2],Qt::CaseInsensitive)==0) {
      //modsConfig[MODS2]->parse(isis->body(),isis->type(),keyDict);
    }
    else if (remHost.compare(modsBCHost[MODS2],Qt::CaseInsensitive)==0 ||
	     remHost.compare(modsRCHost[MODS2],Qt::CaseInsensitive)==0) {
      //modsConfig[MODS2]->parse(isis->body(),isis->type(),keyDict);
      modsFiles[MODS2]->parse(remHost,isis->body(),isis->type(),keyDict);
    }
  }

  // Now mine it for information needed by the Observer/Project Info panel

  QHashIterator<QString,QString> i(keyDict);
  while (i.hasNext()) {
    i.next();
    QString keyStr = i.key();
    QString valStr = i.value();

    // Observer name(s)

    if (keyStr.compare("OBSERVER",Qt::CaseInsensitive)==0) {
      observerStr = valStr;
      observerEntry->setText(observerStr);
    }

    // Partner name(s)

    else if (keyStr.compare("PARTNER",Qt::CaseInsensitive)==0) {
      partnerStr = valStr;
      partnerEntry->setText(partnerStr);
    }

    // Proposal ID

    else if (keyStr.compare("PROPID",Qt::CaseInsensitive)==0) {
      propIDStr = valStr;
      propIDEntry->setText(propIDStr);
    }

    // Support scientist name(s)
    
    else if (keyStr.compare("SUPPORT",Qt::CaseInsensitive)==0) {
      supportStr = valStr;
      supportEntry->setText(supportStr);
    }

    // Telescope Operator name(s)

    else if (keyStr.compare("TELOPS",Qt::CaseInsensitive)==0) {
      telopsStr = valStr;
      telopsEntry->setText(telopsStr);
    }

    // PI/CoI name(s)

    else if (keyStr.compare("PI_NAME",Qt::CaseInsensitive)==0) {
      piNameStr = valStr;
      piNameEntry->setText(piNameStr);
    }

    // User Header Comment 

    else if (keyStr.compare("COMMENT",Qt::CaseInsensitive)==0) {
      commentStr = valStr;
      commentEntry->setText(commentStr);
    }

    else {
      // unrecognized keyword, move along...
    }
  }

}

// Set the parameter keyword to the given value on all relevant and
// available instrument hosts.  We don't do any validation, as if we
// get to this point, we should be past other validation layers

void ConfigManager::setParameter(const QString &keyStr, const QString &valStr)
{
  if (keyStr.isEmpty()) return;
  if (valStr.isEmpty()) return;
  QString cmdStr = QString("%1 %2").arg(keyStr).arg(valStr);
  if (useMODS1) {
    sendToISIS(modsBCHost[MODS1],cmdStr);
    sendToISIS(modsRCHost[MODS1],cmdStr);
  }
  if (useMODS2) {
    sendToISIS(modsBCHost[MODS2],cmdStr);
    sendToISIS(modsRCHost[MODS2],cmdStr);
  }
}

// Set the observer name(s)

void ConfigManager::setObserver(const QString &str)
{
  if (str.isEmpty()) {
    observerStr.clear();
    observerEntry->clear();
    setParameter("OBSERVER","None");
  }
  else if (str.compare(observerStr)!=0) {
    observerStr = str;
    observerEntry->setText(str);
    setParameter("OBSERVER",str);
  }
}

// Set the parter name(s)

void ConfigManager::setPartner(const QString &str)
{
  if (str.isEmpty()) {
    partnerStr.clear();
    partnerEntry->clear();
    setParameter("PARTNER","NOPARTNER");
  }
  else if (str.compare(partnerStr)!=0) {
    partnerStr = str;
    partnerEntry->setText(str);
    setParameter("PARTNER",str);
  }
}

// Set the proposal ID string

void ConfigManager::setPropID(const QString &str)
{
  if (str.isEmpty()) {
    propIDStr.clear();
    propIDEntry->clear();
    setParameter("PROPID","NOTSPECIFIED");
  }
  else if (str.compare(propIDStr)!=0) {
    propIDStr = str;
    propIDEntry->setText(str);
    setParameter("PROPID",str);
  }
}

// Set the PI Name(s)

void ConfigManager::setPIName(const QString &str)
{
  if (str.isEmpty()) {
    piNameStr.clear();
    piNameEntry->clear();
    setParameter("PI_NAME","NOTSPECIFIED");
  }
  else if (str.compare(piNameStr)!=0) {
    piNameStr = str;
    piNameEntry->setText(str);
    setParameter("PI_NAME",str);
  }
}

// Set the Support Scientist name(s)

void ConfigManager::setSupport(const QString &str)
{
  if (str.isEmpty()) {
    supportStr.clear();
    supportEntry->clear();
    setParameter("SUPPORT","None");
  }
  else if (str.compare(supportStr)!=0) {
    supportStr = str;
    supportEntry->setText(str);
    setParameter("SUPPORT",str);
  }
}

// Set the Telescope Operator name(s)

void ConfigManager::setTelops(const QString &str)
{
  if (str.isEmpty()) {
    telopsStr.clear();
    telopsEntry->clear();
    setParameter("TELOPS","None");
  }
  else if (str.compare(telopsStr)!=0) {
    telopsStr = str;
    telopsEntry->setText(str);
    setParameter("TELOPS",str);
  }
}

// Set the generic Comment string

void ConfigManager::setComment(const QString &str)
{
  if (str.isEmpty()) {
    commentStr.clear();
    commentEntry->clear();
    setParameter("comment","None");
  }
  else if (str.compare(commentStr)!=0) {
    commentStr = str;
    commentEntry->setText(str);
    setParameter("comment",str);
  }
}

// loadConfig() - Load a runtime configuration file.

void ConfigManager::loadConfig()
{
  QFile inFile;
  QDir dir = QDir::home();  // user's home directory

  // Start by looking for .modsrc in the user's home directory.  If none,
  // look in the default configuration path for modsrc.def, our default
  // runtime config template.  If that fails, complain and set sensible
  // defaults.

  QString rcFile = QString("%1/.modsrc").arg(dir.homePath());
  inFile.setFileName(rcFile);
  if (!inFile.exists() || !inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    inFile.close();
    rcFile = QString("%1/Config/modsrc.def").arg(MODS_BASE_PATH);
    inFile.setFileName(rcFile);
    if (!inFile.exists()) {
      QString errMsg = tr("Could not open the default MODS user runtime configuration file %1!\n"
			  "Reason:\n %2\nStarting with a blank configuration\n"
			  "Remember to Save after setting observer/project info.\n"
			  ).arg(rcFile).arg(inFile.errorString());
      QMessageBox::warning(this,tr("MODS User Configuration File Load Error"),errMsg); 
      clearForm();
      return;
    }
  }

  // Read in the table line-by-line, parsing as we go.  # denotes
  // comments (either whole lines or inline comments), and we skip
  // blank lines.

  QString itemStr;
  QString line;

  QTextStream in(&inFile);
  do {
    line = in.readLine();
    if (!line.isEmpty()) {  // skip blank lines
      line = line.simplified();
      if (!line.startsWith("#")) { // skip full-line comments
        if (line.contains("#"))    // strip off inline comments
          itemStr = line.section('#',0,0);
        else
          itemStr = line;

	if (!itemStr.isEmpty()) {
	  QString key = itemStr.section(' ',0,0);
	  QString val = itemStr.section(' ',1,-1);

	  // Process the keywords

	  if (key.compare("OBSERVER",Qt::CaseInsensitive)==0)
	    setObserver(val);
	  else if (key.compare("PARTNER",Qt::CaseInsensitive)==0)
	    setPartner(val);
	  else if (key.compare("PROPID",Qt::CaseInsensitive)==0)
	    setPropID(val);
	  else if (key.compare("PI_NAME",Qt::CaseInsensitive)==0)
	    setPIName(val);
	  else if (key.compare("SUPPORT",Qt::CaseInsensitive)==0)
	    setSupport(val);
	  else if (key.compare("TELOPS",Qt::CaseInsensitive)==0)
	    setTelops(val);
	  else if (key.compare("COMMENT",Qt::CaseInsensitive)==0)
	    setComment(val);
	  // ignore unrecognzed keywords,
	}
      }
    }
  } while (!line.isNull());
  inFile.close();
}

// Save a setup runtime file - name is .modsrc in the user's home
// directory We make this a public slot so that the mainwindow parent
// widget can invoke it to save the configuration on exiting the
// session.

void ConfigManager::saveConfig()
{
  QFile outFile;
  QDir dir = QDir::home();  // user's home directory

  QString rcFile = QString("%1/.modsrc").arg(dir.homePath());
  outFile.setFileName(rcFile);
  if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QString errMsg = tr("Could not save the MODS user configuration file %1 for writing!\n"
			"Reason:\n %2\n").arg(rcFile).arg(outFile.errorString());
    QMessageBox::warning(this,tr("MODS User Configuration File Save Error"),errMsg); 
    return;
  }

  // Create the runtime config file so that on the next restart will
  // begin with the same observer info (and anything else we put here
  // in the future).

  QDateTime dateTime = QDateTime::currentDateTime().toUTC();
  QTextStream out(&outFile);
  out << "#\n";
  out << "# MODS Runtime User Configuration File\n";
  out << QString("# Created: UTC %1\n").arg(dateTime.toString(Qt::ISODate));
  out << "#\n";
  out << QString("Observer %1\n").arg(observerStr);
  out << QString("Partner  %1\n").arg(partnerStr);
  out << QString("PropID   %1\n").arg(propIDStr);
  out << QString("PI_Name  %1\n").arg(piNameStr);
  out << QString("Support  %1\n").arg(supportStr);
  out << QString("TelOps   %1\n").arg(telopsStr);
  out << QString("Comment  %1\n").arg(commentStr);
  outFile.close();

}

// Clear the setup form

void ConfigManager::clearForm()
{
  observerStr.clear();
  observerEntry->clear();
  partnerStr.clear();
  partnerEntry->clear();
  propIDStr.clear();
  propIDEntry->clear();
  piNameStr.clear();
  piNameEntry->clear();
  supportStr.clear();
  supportEntry->clear();
  telopsStr.clear();
  telopsEntry->clear();
  commentStr.clear();
  commentEntry->clear();
}

// Send the parameters in the observer/project information form to the
// relevant IC hosts, and save the configuration.

void ConfigManager::sendConfig()
{
  setParameter("OBSERVER",observerStr);
  setParameter("PARTNER",partnerStr);
  setParameter("PROPID",propIDStr);
  setParameter("PI_NAME",piNameStr);
  setParameter("SUPPORT",supportStr);
  setParameter("TELOPS",telopsStr);
  setParameter("COMMENT",commentStr);
  setParameter("SAVECONFIG","null");

  // now save it

  saveConfig();
}

// Enable/disable a MODSn setup panel

void ConfigManager::enableMODS(const int &modsNum, const bool &enable)
{
  if (modsNum < 0 || modsNum > 2) return;
  if (modsNum == 0) { // special - enable both
    useMODS1 = enable;
    if (useMODS1) {
      //modsConfig[MODS1]->enable();
      modsFiles[MODS1]->enable();
    }
    else {
      //modsConfig[MODS1]->disable();
      modsFiles[MODS1]->disable();
    }

    useMODS2 = enable;
    if (useMODS2) {
      //modsConfig[MODS2]->enable();
      modsFiles[MODS2]->enable();
    }
    else {
      //modsConfig[MODS2]->disable();
      modsFiles[MODS1]->disable();
    }
  }
  else if (modsNum == 1) {
    useMODS1 = enable;
    if (useMODS1) {
      //modsConfig[MODS1]->enable();
      modsFiles[MODS1]->enable();
    }
    else {
      //modsConfig[MODS1]->disable();
      modsFiles[MODS1]->disable();
    }
  }
  else if (modsNum == 2) {
    useMODS2 = enable;
    if (useMODS2) {
      //modsConfig[MODS2]->enable();
      modsFiles[MODS2]->enable();
    }
    else {
      //modsConfig[MODS2]->disable();
      modsFiles[MODS2]->disable();
    }
  }
}
