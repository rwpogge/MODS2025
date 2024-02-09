#include <QtWidgets>
#include <QtNetwork>
#include <math.h>

#include "utilmanager.h"
#include "instutils.h"
#include "utilpanel.h"
#include "actionbutton.h"
#include "textdisplay.h"
#include "isisclient.h"

UtilManager::UtilManager(const QString &myID, const bool &useM1, const bool &useM2,
			 const int &myPort, QWidget *parent)
  : QWidget(parent)
{

  if (myID.isEmpty())
    clientID = "UM";
  else
    clientID = myID;

  clientPort = myPort;

  useMODS1 = useM1;
  useMODS2 = useM2;

  // Basic initializations

  queue = false;  // All commands are sent immediately
  dispatchCadence = MODS_QUEUE_CADENCE;  // Default dispatch queue cadence

  //----------------------------------------------------------------
  //
  // Build the utility management widget
  //

  tabWidget = new QTabWidget;
  tabWidget->setTabShape(QTabWidget::Triangular);

  // Create a utility control panel and connect its signals

  utilityPanel = new UtilPanel(this);
  connect(utilityPanel,SIGNAL(cmdReady(const QString &, const QString &)),
	  this,SLOT(sendToISIS(const QString &, const QString &)));
  connect(utilityPanel,SIGNAL(cmdWait(const QString &, const QString &, const int &)),
	  this,SLOT(sendAndWait(const QString &, const QString &, const int &)));
  connect(utilityPanel,SIGNAL(statusMsg(const QString &, const QColor &)),
	  this,SLOT(addStatus(const QString &, const QColor &)));
  tabWidget->addTab(utilityPanel,tr("Instrument Utilties"));
  utilityPanel->enableMODS1(useMODS1);
  utilityPanel->enableMODS2(useMODS2);

  // Create a MODS1 box-level utilities control panel and connect its signals

  if (useMODS1) {
    mods1Utils = new InstUtils(MODS1,this);
    connect(mods1Utils,SIGNAL(cmdReady(const QString &, const QString &)),
	    this,SLOT(sendToISIS(const QString &, const QString &)));
    connect(mods1Utils,SIGNAL(cmdWait(const QString &, const QString &, const int &)),
	    this,SLOT(sendAndWait(const QString &, const QString &, const int &)));
    connect(mods1Utils,SIGNAL(statusMsg(const QString &, const QColor &)),
	    this,SLOT(addStatus(const QString &, const QColor &)));
    tabWidget->addTab(mods1Utils,tr("MODS1 Electronics Boxes"));
  }

  // Create a MODS2 box-level utilities control panel and connect its signals

  if (useMODS2) {
    mods2Utils = new InstUtils(MODS2,this);
    connect(mods2Utils,SIGNAL(cmdReady(const QString &, const QString &)),
	    this,SLOT(sendToISIS(const QString &, const QString &)));
    connect(mods2Utils,SIGNAL(cmdWait(const QString &, const QString &, const int &)),
	    this,SLOT(sendAndWait(const QString &, const QString &, const int &)));
    connect(mods2Utils,SIGNAL(statusMsg(const QString &, const QColor &)),
	    this,SLOT(addStatus(const QString &, const QColor &)));
    tabWidget->addTab(mods2Utils,tr("MODS2 Electronics Boxes"));
  }

  // Status display, nestled between an update button and a help button

  updateButton = new ActionButton(tr("Refresh"),this);
  connect(updateButton,SIGNAL(clicked()),this,SLOT(update()));

  helpButton = new ActionButton(tr("Help"),this);
  connect(helpButton,SIGNAL(clicked()),this,SLOT(help()));

  statusDisplay = new TextDisplay("","",0,this);
  statusDisplay->setTimer(MODS_STATUSMSG_TIME); // status display message persistence

  QHBoxLayout *buttonLayout = new QHBoxLayout;
  buttonLayout->addWidget(updateButton);
  buttonLayout->addWidget(statusDisplay);
  buttonLayout->addWidget(helpButton);

  // Assemble the main layout

  QVBoxLayout *mainLayout = new QVBoxLayout;
  QLabel *mainLabel = new QLabel(tr("MODS Instrument Utilities Manager"));
  mainLabel->setFont(QFont("Helvetica",(int)(1.5*appFontSize),QFont::Normal));

  mainLayout->addWidget(mainLabel,0,Qt::AlignHCenter);
  mainLayout->addStretch();
  mainLayout->addWidget(tabWidget);
  mainLayout->addStretch();
  mainLayout->addLayout(buttonLayout);
  // mainLayout->setContentsMargins(0,0,0,0);
  mainLayout->setStretchFactor(tabWidget,1);
  setLayout(mainLayout);

  //----------------------------------------------------------------

  // Create the ISIS client socket but don't initialize it yet The
  // ISIS server ID, host, and port are sensible defaults.  These can
  // be changed later with the setISISInfo() method.

  isis = new ISISClient(MODS_ISIS_SERVER_ID,MODS_ISIS_SERVER_HOST,
			MODS_ISIS_SERVER_PORT,clientID,myPort,this);
  connect(isis,SIGNAL(dataReady()),this,SLOT(parseISIS()));
  connect(isis,SIGNAL(logMessage(const QString &)),this,SLOT(passMessage(const QString &)));

}

//---------------------------------------------------------------------------
// 
// parseISIS - parse an IMPv2 message into keywords
//
// This slot is connected to the dataReady() signal of an ISISClient
// instance.  It assumes, correctly, that the ISISClient has already
// parsed the message into components.  We check validity then proceed
// to process those messages of relevance to us.
//

void UtilManager::parseISIS()
{
  // Ignore invalid messages passed to us

  if (!isis->isValid()) return;

  // Add to the status display, color coding by message type

  switch (isis->type()) {
  case ISISClient::ERROR:
    addStatus(isis->text().section(" ",1,-1),Qt::red);
    break;

  case ISISClient::WARNING:
    addStatus(isis->text().section(" ",1,-1),QColor(255,191,0,255));
    break;

  case ISISClient::FATAL:
    addStatus(isis->text().section(" ",1,-1),Qt::magenta);
    break;

  case ISISClient::DONE:
    addStatus(isis->text().section(" ",1,-1),Qt::blue);
    break;

  case ISISClient::STATUS:
    addStatus(isis->text().section(" ",1,-1),Qt::blue);
    break;
 
  case ISISClient::EXEC:
  case ISISClient::REQ:
    break;

  default:
    return;
    break;
  }

  // We have a valid message, see if we need to do anything with it

  QString msgStr = isis->body();
  msgStr = msgStr.simplified(); // TODO: Evaluate this line. Why is it here? 

  if (msgStr.isEmpty()) return; // Empty messages are ignored 

  // The first word in the message body is the command that
  // generated this response.

  QString cmdWord = isis->body().section(" ",0,0);

  // Did we get PINGed?  PONG back (immediate dispatch)

  if (cmdWord.compare("PING",Qt::CaseInsensitive)==0) {
    sendAndWait(isis->sender(),"PONG",0);
    return;
  }
  else if (cmdWord.compare("PONG",Qt::CaseInsensitive)==0) {
    return;
  }

  // Create a hash with the keyword=value pairs

  QHash<QString,QString> keyDict; // keyword dictionary
  isis->getKeys(msgStr,&keyDict);

  // Pass it along for processing by the various control panels that
  // make up this widget

  if (isis->sender().compare(modsIEHost[MODS1],Qt::CaseInsensitive)==0) {
    utilityPanel->parse(MODS1,isis->sender(),isis->body(),isis->type(),keyDict);
    if (useMODS1) mods1Utils->parse(isis->body(),isis->type(),keyDict);
  }
  else if (isis->sender().compare(modsBCHost[MODS1],Qt::CaseInsensitive)==0 ||
	   isis->sender().compare(modsRCHost[MODS1],Qt::CaseInsensitive)==0) {
    utilityPanel->parse(MODS1,isis->sender(),isis->body(),isis->type(),keyDict);
  }
  else if (isis->sender().compare(modsIEHost[MODS2],Qt::CaseInsensitive)==0) {
    utilityPanel->parse(MODS2,isis->sender(),isis->body(),isis->type(),keyDict);
    if (useMODS2) mods2Utils->parse(isis->body(),isis->type(),keyDict);
  }
  else if (isis->sender().compare(modsBCHost[MODS2],Qt::CaseInsensitive)==0 ||
	   isis->sender().compare(modsRCHost[MODS2],Qt::CaseInsensitive)==0) {
    utilityPanel->parse(MODS2,isis->sender(),isis->body(),isis->type(),keyDict);
  }

  // Share the message with the parent

  emit shareMsg(isis->text());

}

// Update the control panel

void UtilManager::update()
{
  utilityPanel->update();
  if (useMODS1) mods1Utils->update();
  if (useMODS2) mods2Utils->update();
}

// ISIS Client Socket communications bits - most are convenience functions

void UtilManager::setID(const QString &id)
{
  clientID = id;
  isis->setClient(clientID,clientPort);
}

QString UtilManager::id() 
{
  return clientID;
}

void UtilManager::setPort(const int &port)
{
  clientPort = port;
  isis->setClient(clientID,clientPort);
}

// Set the ISIS server ID, host, and port for this client

void UtilManager::setISISInfo(const QString &id, const QString &host, 
			       const int &port)
{
  isisID = id;
  isisPort = port;
  isisHost = host;
  isis->setServer(isisID,isisHost,isisPort);
}

// Start the ISIS client UDP socket service

void UtilManager::startISISClient()
{
  if (!isis->init()) {
    QString cmdErr = tr("Cannot initialize ISIS client socket!\n"
			"Reason:\n %1\n Is another client running on this port?\n"
			"%2 aborting with fatal errors.").arg(isis->errorString()).arg(MODS_SESSION_NAME);
    QMessageBox::critical(this,tr("Network Socket Error"),cmdErr); 
    exit(1);
  }

  // Set the isis client's message dispatcher cadence in milliseconds

  isis->setDispatchCadence(dispatchCadence);

  // Success, ping the ISIS server (explicit immediate dispatch)

  sendAndWait(isisID,"PING",0);

}

// Send a message to an ISIS client or server
//
// Wrapper for the isis->send() method, so we can do logging and other
// fun stuff like dispatch queuing.

void UtilManager::sendToISIS(const QString &toAddr, const QString &msgStr)
{
  if (msgStr.isEmpty()) return;
  if (toAddr.isEmpty()) return;
  QString fullMsg = QString("%1>%2 %3").arg(clientID,toAddr,msgStr);
  if (queue)
    isis->queue(toAddr,msgStr);
  else
    isis->send(toAddr,msgStr);
}

// Generalized Send and Wait. 
//   If timeout=0 send immediately.  
//   If timeout>0 send and then block and wait up to timeout for a reply.
//   If timeout<0 queue it to the isis client's message dispatcher
//      (non-blocking threaded message queue).

void UtilManager::sendAndWait(const QString &toAddr, const QString &msgStr, 
			       const int &timeout)
{
  if (msgStr.isEmpty()) return;
  if (toAddr.isEmpty()) return;

  QString fullMsg = QString("%1>%2 %3").arg(clientID,toAddr,msgStr);

  if (timeout == 0)
    isis->send(toAddr,msgStr);
  else if (timeout > 0)
    isis->sendAndWait(toAddr,msgStr,timeout);
  else if (timeout < 0) 
    isis->queue(toAddr,msgStr);
}

// Pass messages from child objects to our parent or siblings.

void UtilManager::passMessage(const QString &msgStr)
{
  emit logMessage(msgStr);
}

// Launch the help dialog

void UtilManager::help()
{
  addStatus("UtilManager::help() not yet implemented...",Qt::red);
}

// Add a message to the status display with color coding

void UtilManager::addStatus(const QString &text, const QColor &color)
{
  if (text.isEmpty()) return;
  statusDisplay->setText(text,color);
}

// Enable/Disable queued message dispatch for most commands (changes
// the behavior of the sendToISIS() method).

void UtilManager::cmdQueue(const bool &flag)
{
  queue = flag;
}

// Set the dispatch queue cadence in milliseconds

void UtilManager::setDispatchCadence(const int &twait)
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

void UtilManager::parseShared(const QString &msgStr)
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
  int msgType = isis->type();

  // We have something to interpret, pass it along to the various
  // widgets that need updated information

  if (remHost.compare(modsIEHost[MODS1],Qt::CaseInsensitive)==0) {
    utilityPanel->parse(MODS1,remHost,msgBody,msgType,keyDict);
    if (useMODS1) mods1Utils->parse(msgBody,msgType,keyDict);
  }
  else if (remHost.compare(modsBCHost[MODS1],Qt::CaseInsensitive)==0 ||
	   remHost.compare(modsRCHost[MODS1],Qt::CaseInsensitive)==0) {
    utilityPanel->parse(MODS1,remHost,msgBody,msgType,keyDict);
  }
  else if (remHost.compare(modsIEHost[MODS2],Qt::CaseInsensitive)==0) {
    utilityPanel->parse(MODS2,remHost,msgBody,msgType,keyDict);
    if (useMODS2) mods2Utils->parse(msgBody,msgType,keyDict);
  }
  else if (remHost.compare(modsBCHost[MODS2],Qt::CaseInsensitive)==0 ||
	   remHost.compare(modsRCHost[MODS2],Qt::CaseInsensitive)==0) {
    utilityPanel->parse(MODS2,remHost,msgBody,msgType,keyDict);
  }
  
}
