#include <QtWidgets>
#include <QtNetwork>
#include <math.h>

#include "instmonitor.h"
#include "actionbutton.h"
#include "textdisplay.h"
#include "isisclient.h"
#include "commviewer.h"
#include "envpanel.h"
#include "commlog.h"

InstMonitor::InstMonitor(const QString &myID, const bool &useM1, const bool &useM2,
			 const int &myPort, QWidget *parent)
  : QWidget(parent)
{

  if (myID.isEmpty())
    clientID = "HK";
  else
    clientID = myID;

  clientPort = myPort;

  useMODS1 = useM1;
  useMODS2 = useM2;

  // Basic initializations

  queue = true;  // All commands are queued for dispatch
  dispatchCadence = MODS_QUEUE_CADENCE;  // Default dispatch queue cadence

  //----------------------------------------------------------------
  //
  // Build the instrument monitor main panel
  //

  tabWidget = new QTabWidget;
  tabWidget->setTabShape(QTabWidget::Triangular);

  // Create a MODS1 environmental monitor console

  if (useMODS1) {
    mods1Env = new EnvPanel(MODS1,this);
    connect(mods1Env,SIGNAL(cmdReady(const QString &, const QString &)),
	    this,SLOT(sendToISIS(const QString &, const QString &)));
    connect(mods1Env,SIGNAL(cmdWait(const QString &, const QString &, const int &)),
	    this,SLOT(sendAndWait(const QString &, const QString &, const int &)));
    connect(mods1Env,SIGNAL(statusMsg(const QString &, const QColor &)),
	    this,SLOT(addStatus(const QString &, const QColor &)));
    connect(mods1Env,SIGNAL(alert(const QString &)),this,SLOT(passAlert(const QString &)));
    connect(mods1Env,SIGNAL(cancelAlerts()),this,SLOT(passCancel()));
    connect(mods1Env,SIGNAL(alertMsg(const QString &, const QColor &)),
	    this,SLOT(logAlert(const QString &, const QColor &)));
    tabWidget->addTab(mods1Env,tr("MODS1 Instrument Sensors"));
  }

  // Create a MODS2 environmental monitor console

  if (useMODS2) {
    mods2Env = new EnvPanel(MODS2,this);
    connect(mods2Env,SIGNAL(cmdReady(const QString &, const QString &)),
	    this,SLOT(sendToISIS(const QString &, const QString &)));
    connect(mods2Env,SIGNAL(cmdWait(const QString &, const QString &, const int &)),
	    this,SLOT(sendAndWait(const QString &, const QString &, const int &)));
    connect(mods2Env,SIGNAL(statusMsg(const QString &, const QColor &)),
	    this,SLOT(addStatus(const QString &, const QColor &)));
    connect(mods2Env,SIGNAL(alert(const QString &)),this,SLOT(passAlert(const QString &)));
    connect(mods2Env,SIGNAL(cancelAlerts()),this,SLOT(passCancel()));
    connect(mods2Env,SIGNAL(alertMsg(const QString &, const QColor &)),
	    this,SLOT(logAlert(const QString &, const QColor &)));
    tabWidget->addTab(mods2Env,tr("MODS2 Instrument Sensors"));
  }

  // Create a CommViewer and put it in the last tab

  commMonitor = new CommViewer(tr("Communications Log"),this);
  commMonitor->showHeartBeats(false);
  tabWidget->addTab(commMonitor,tr("Communications Log"));

  // Status display, nestled between an update button and alert log buttons

  updateButton = new ActionButton(tr("Update"),this);
  connect(updateButton,SIGNAL(clicked()),this,SLOT(update()));
  updateButton->setToolTip(tr("Update instrument sensor data"));

  statusDisplay = new TextDisplay("","",0,this);
  statusDisplay->setTimer(MODS_STATUSMSG_TIME); // status display message persistence

  // Alert Log implemented as a modeless dialog using the CommLog object

  alertLog = new CommLog(tr("MODS Alert Log"),800,400,this);
  connect(alertLog,SIGNAL(logHidden()),this,SLOT(setAlertButton()));
  alertLog->seconds();  // show time in log to the nearest second

  alertLogButton = new ActionButton(tr("View Alerts"));
  connect(alertLogButton,SIGNAL(clicked()),this,SLOT(toggleAlertLog()));
  alertLogButton->setToolTip(tr("View the Instrument Sensor Alert log"));

  /*
  cancelButton = new ActionButton(tr("Clear Alerts"),this);
  connect(cancelButton,SIGNAL(clicked()),this,SLOT(cancelAll()));
  cancelButton->setToolTip(tr("Clear/Cancel Instrument Sensor Alerts"));
  */
  // Layout the buttons across the bottom as follows
  // [Update] [status Display Box] [View Alerts] [Clear Alerts]

  QHBoxLayout *buttonLayout = new QHBoxLayout;
  buttonLayout->addWidget(updateButton);
  buttonLayout->addWidget(statusDisplay);
  buttonLayout->addWidget(alertLogButton);
  // buttonLayout->addWidget(cancelButton);

  // Assemble the main layout

  QVBoxLayout *mainLayout = new QVBoxLayout;
  QLabel *mainLabel = new QLabel(tr("Instrument Housekeeping Monitor"));
  mainLabel->setFont(QFont("Helvetica",(int)(1.5*appFontSize),QFont::Normal));

  mainLayout->addWidget(mainLabel,0,Qt::AlignHCenter);
  mainLayout->addStretch();
  mainLayout->addWidget(tabWidget);
  mainLayout->addStretch();
  mainLayout->addLayout(buttonLayout);
  mainLayout->setStretchFactor(tabWidget,1); // make sure it fills the space
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

void InstMonitor::parseISIS()
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
  QString remHost = isis->sender();

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

  // Pass it along for processing by the various monitor panels that
  // make up this widget

  if (useMODS1) {
    if (remHost.compare(modsIEHost[MODS1],Qt::CaseInsensitive)==0 ||
	remHost.compare(modsIMHost[MODS1],Qt::CaseInsensitive)==0 ||
	remHost.compare(modsBCHost[MODS1],Qt::CaseInsensitive)==0 ||
	remHost.compare(modsRCHost[MODS1],Qt::CaseInsensitive)==0)
      mods1Env->parse(remHost,isis->body(),isis->type(),keyDict);
  }

  if (useMODS2) {
    if (remHost.compare(modsIEHost[MODS2],Qt::CaseInsensitive)==0 ||
	remHost.compare(modsIMHost[MODS2],Qt::CaseInsensitive)==0 ||
	remHost.compare(modsBCHost[MODS2],Qt::CaseInsensitive)==0 ||
	remHost.compare(modsRCHost[MODS2],Qt::CaseInsensitive)==0)
      mods2Env->parse(remHost,isis->body(),isis->type(),keyDict);
  }

  // Display it ourselves...

  addMessage(isis->text());

  // Share the message with the parent

  emit shareMsg(isis->text());
}

// Update the data in the instrument monitor 

void InstMonitor::update()
{
  if (useMODS1) mods1Env->update();
  if (useMODS2) mods2Env->update();
  return;
}

// ISIS Client Socket communications bits - most are convenience functions

void InstMonitor::setID(const QString &id)
{
  clientID = id;
  isis->setClient(clientID,clientPort);
}

QString InstMonitor::id() 
{
  return clientID;
}

void InstMonitor::setPort(const int &port)
{
  clientPort = port;
  isis->setClient(clientID,clientPort);
}

// Set the ISIS server ID, host, and port for this client

void InstMonitor::setISISInfo(const QString &id, const QString &host, const int &port)
{
  isisID = id;
  isisPort = port;
  isisHost = host;
  isis->setServer(isisID,isisHost,isisPort);
}

// Start the ISIS client UDP socket service

void InstMonitor::startISISClient()
{
  if (!isis->init()) {
    QString cmdErr = tr("Cannot initialize the Instrument Monitor ISIS client socket!\n"
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

void InstMonitor::sendToISIS(const QString &toAddr, const QString &msgStr)
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

void InstMonitor::sendAndWait(const QString &toAddr, const QString &msgStr, 
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

void InstMonitor::passMessage(const QString &msgStr)
{
  emit logMessage(msgStr);
}

// Add a message to the status display with color coding

void InstMonitor::addStatus(const QString &text, const QColor &color)
{
  if (text.isEmpty()) return;
  statusDisplay->setText(text,color);
}

// Enable/Disable queued message dispatch for most commands (changes
// the behavior of the sendToISIS() method).

void InstMonitor::cmdQueue(const bool &flag)
{
  queue = flag;
}

// Set the dispatch queue cadence in milliseconds

void InstMonitor::setDispatchCadence(const int &twait)
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

void InstMonitor::parseShared(const QString &msgStr)
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

  if (useMODS1) {
    if (remHost.compare(modsIEHost[MODS1],Qt::CaseInsensitive)==0 ||
	remHost.compare(modsIMHost[MODS1],Qt::CaseInsensitive)==0)
      mods1Env->parse(remHost,isis->body(),msgType,keyDict);
    else if (remHost.compare(modsBCHost[MODS1],Qt::CaseInsensitive)==0 ||
	     remHost.compare(modsRCHost[MODS1],Qt::CaseInsensitive)==0)
      mods1Env->parse(remHost,isis->body(),msgType,keyDict);
  }

  if (useMODS2) {
    if (remHost.compare(modsIEHost[MODS2],Qt::CaseInsensitive)==0 ||
	remHost.compare(modsIMHost[MODS2],Qt::CaseInsensitive)==0)
      mods2Env->parse(remHost,isis->body(),msgType,keyDict);
    else if (remHost.compare(modsBCHost[MODS2],Qt::CaseInsensitive)==0 ||
	     remHost.compare(modsRCHost[MODS2],Qt::CaseInsensitive)==0)
      mods2Env->parse(remHost,isis->body(),msgType,keyDict);
  }

  // Display it ourselves...
  //addMessage(isis->text());

}

// Add a message to the communications log (public slot)

void InstMonitor::addMessage(const QString &msgStr)
{
  if (msgStr.isEmpty()) return;

  // Filter message strings for routine/mundane messages
  // to reduce clutter

  if (msgStr.contains("estatus",Qt::CaseInsensitive) ||
      msgStr.contains("pctread",Qt::CaseInsensitive)) return;

  // Show it.
  commMonitor->addMessage(msgStr);
}

// setAlertButton() - Set the alert log button label based on
// the visibility of the Alert Log dialog box

void InstMonitor::setAlertButton()
{
  if (alertLog->isHidden()) {
    alertLogButton->setText(tr("View Alerts"));
    alertLogButton->setToolTip(tr("View the Instrument Sensor Alert log"));
  }
  else {
    alertLogButton->setText(tr("Hide Alerts"));
    alertLogButton->setToolTip(tr("Hide the Instrument Sensor Alert log"));
  }
}

// toggleAlertLog() - Hide/View the Alert Log modeless dialog box

void InstMonitor::toggleAlertLog()
{
  if (alertLog->isHidden())
    alertLog->show();
  else
    alertLog->hide();
  alertLog->activateWindow();
  setAlertButton();
}

// The user has cancelled all active alerts by hand.  Not to say they
// won't recur - this is a way to clear out delinquent alerts.

void InstMonitor::cancelAll()
{
  // Make a notation in the alert log
  alertLog->append(tr("*** Alert(s) cleared/canceled by user"),Qt::blue);
  // Instruct each env subpanel to clear all alerts
  if (useMODS1) mods1Env->clearAll();
  if (useMODS2) mods2Env->clearAll();
  // emit the cancelAlerts() signal
  emit cancelAlerts();
  // and then trigger an update()
  update();
}

// Append a message to the alert log (e.g., if an alert/alarm condition
// is cleared).

void InstMonitor::logAlert(const QString &msgStr, const QColor &color)
{
  alertLog->append(msgStr,color);
}

// Pass alerts from child objects to our parent or siblings.
// We also make sure the alert is put into the AlertLog (the
// widget takes care of color coding) and make the log
// visible if not already up.

void InstMonitor::passAlert(const QString &msgStr)
{
  emit alert(msgStr);
  alertLog->append(msgStr,Qt::red);
  alertLog->show();
  alertLog->activateWindow();
  setAlertButton();
}

// Pass alert cancellations from child object to our parent or siblings.

void InstMonitor::passCancel()
{
  emit cancelAlerts();
}

