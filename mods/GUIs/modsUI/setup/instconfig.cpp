#include <QtWidgets>
#include <QtNetwork>
#include <math.h>

#include "instconfig.h"
#include "actionbutton.h"
#include "led.h"

#include "isisclient.h"  // needed for msgType codes

InstConfig::InstConfig(const int &myMODS, QWidget *parent)
  : QWidget(parent)
{
  modsID = myMODS;
  if (modsSide[modsID].compare("left",Qt::CaseInsensitive)==0 ||
      modsSide[modsID].compare("SX",Qt::CaseInsensitive)==0) {
    lbtSide = "left";
    focalStation = "Left Direct Gregorian";
  }
  else {
    lbtSide = "right";
    focalStation = "Right Direct Gregorian";
  }

  // Startup, Reset, Sleep, Stow, and Shutdown Buttons

  startButton = new ActionButton(tr("Startup"),this);
  connect(startButton,SIGNAL(clicked()),this,SLOT(doStartup()));
  int sbw = (int)(startButton->minimumSizeHint().width());

  resetButton = new ActionButton(tr("Reset"),this);
  connect(resetButton,SIGNAL(clicked()),this,SLOT(doReset()));
  sbw = qMax((int)(resetButton->minimumSizeHint().width()),sbw);

  sleepButton = new ActionButton(tr("Sleep"),this);
  connect(sleepButton,SIGNAL(clicked()),this,SLOT(doSleep()));
  sbw = qMax((int)(sleepButton->minimumSizeHint().width()),sbw);

  stowButton = new ActionButton(tr("Stow"),this);
  connect(stowButton,SIGNAL(clicked()),this,SLOT(doStow()));
  sbw = qMax((int)(stowButton->minimumSizeHint().width()),sbw);

  shutdownButton = new ActionButton(tr("Shutdown"),this);
  connect(shutdownButton,SIGNAL(clicked()),this,SLOT(doShutdown()));
  sbw = qMax((int)(shutdownButton->minimumSizeHint().width()),sbw);

  // Make all the buttons the same width

  startButton->setMinimumWidth(2*sbw);
  resetButton->setMinimumWidth(2*sbw);
  sleepButton->setMinimumWidth(2*sbw);
  stowButton->setMinimumWidth(2*sbw);
  shutdownButton->setMinimumWidth(2*sbw);

  // QRadioButtons to select cold or warm start

  warmStartButton = new QRadioButton(tr("Warm Start"),this);
  warmStartButton->setFont(QFont("Helvetica",appFontSize,QFont::Normal));
  connect(warmStartButton,SIGNAL(clicked()),this,SLOT(setWarmStart()));

  coldStartButton = new QRadioButton(tr("Cold Start"),this);
  coldStartButton->setFont(QFont("Helvetica",appFontSize,QFont::Normal));
  connect(coldStartButton,SIGNAL(clicked()),this,SLOT(setColdStart()));

  warmStart = true;
  warmStartButton->setChecked(true);

  QHBoxLayout *radioLayout = new QHBoxLayout;
  radioLayout->addWidget(warmStartButton,0,Qt::AlignHCenter);
  radioLayout->addWidget(coldStartButton,0,Qt::AlignHCenter);

  // Build the group box with the instrument Startup/Reset/... buttons

  QGroupBox *instGroupBox = new QGroupBox(tr("Startup && Shutdown"));
  instGroupBox->setAlignment(Qt::AlignHCenter);

  QVBoxLayout *instLayout = new QVBoxLayout;
  instLayout->addWidget(startButton,0,Qt::AlignHCenter);
  instLayout->addLayout(radioLayout);
  instLayout->addWidget(resetButton,0,Qt::AlignHCenter);
  instLayout->addWidget(sleepButton,0,Qt::AlignHCenter);
  instLayout->addWidget(stowButton,0,Qt::AlignHCenter);
  instLayout->addWidget(shutdownButton,0,Qt::AlignHCenter);
  instGroupBox->setLayout(instLayout);

  // Slit and Filter Load dialog buttons

  slitmaskButton = new ActionButton(tr("Slit Masks..."),this);
  connect(slitmaskButton,SIGNAL(clicked()),this,SLOT(launchSlitLoad()));

  bfilterButton = new ActionButton(tr("Blue Filters..."),this);
  connect(bfilterButton,SIGNAL(clicked()),this,SLOT(launchBFiltLoad()));

  rfilterButton = new ActionButton(tr("Red Filters..."),this);
  connect(rfilterButton,SIGNAL(clicked()),this,SLOT(launchRFiltLoad()));

  slitmaskButton->setMinimumWidth(2*sbw);
  bfilterButton->setMinimumWidth(2*sbw);
  rfilterButton->setMinimumWidth(2*sbw);

  // Pack these into the loadGroupBox

  QGroupBox *loadGroupBox = new QGroupBox(tr("Masks && Filters"));
  loadGroupBox->setAlignment(Qt::AlignHCenter);

  QVBoxLayout *loadButtonLayout = new QVBoxLayout;
  loadButtonLayout->addWidget(slitmaskButton,0,Qt::AlignHCenter);
  loadButtonLayout->addWidget(bfilterButton,0,Qt::AlignHCenter);
  loadButtonLayout->addWidget(rfilterButton,0,Qt::AlignHCenter);
  loadGroupBox->setLayout(loadButtonLayout);

  // Finally, the LBT focal station authorization status indicator LED
  // and the update/query button

  authLight = new LED(this);
  authLight->setOff();
  QLabel *authLabel = new QLabel(tr("Authorized"));

  QHBoxLayout *ledLayout = new QHBoxLayout;
  ledLayout->addStretch();
  ledLayout->addWidget(authLight);
  ledLayout->addWidget(authLabel);
  ledLayout->addStretch();

  tcsButton = new ActionButton(tr("Update"));
  connect(tcsButton,SIGNAL(clicked()),this,SLOT(checkLBTAuth()));
  tcsButton->setMinimumWidth(2*sbw);
  isAuthorized = false;

  QVBoxLayout *tcsLayout = new QVBoxLayout;
  tcsLayout->addLayout(ledLayout);
  tcsLayout->addWidget(tcsButton,0,Qt::AlignHCenter);

  QGroupBox *tcsGroupBox = new QGroupBox(tr("LBT Interface"));
  tcsGroupBox->setAlignment(Qt::AlignHCenter);
  tcsGroupBox->setLayout(tcsLayout);

  QVBoxLayout *loadLayout = new QVBoxLayout;
  loadLayout->addWidget(loadGroupBox);
  loadLayout->addStretch();
  loadLayout->addWidget(tcsGroupBox);

  // Assemble the main QGroupBox

  QGroupBox *mainGroupBox = new QGroupBox(QString("MODS%1 (%2)").arg(modsID+1).arg(focalStation));
  mainGroupBox->setAlignment(Qt::AlignHCenter);
  mainGroupBox->setFont(QFont("Helvetica",appFontSize+2,QFont::Normal));

  QHBoxLayout *horizLayout = new QHBoxLayout;
  horizLayout->addWidget(instGroupBox);
  horizLayout->addLayout(loadLayout);

  QVBoxLayout *vertLayout = new QVBoxLayout;
  vertLayout->addLayout(horizLayout);

  mainGroupBox->setLayout(vertLayout);

  //----------------------------------------------------------------

  // Main widget layout

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(mainGroupBox);
  mainLayout->setContentsMargins(0,0,0,0);
  setLayout(mainLayout);

}

// Public Methods

// Update the control panel

void InstConfig::update()
{
  emit cmdReady(modsTCHost[modsID],"AUTHORIZED");
  return;
}

// Return the most recent error string

QString InstConfig::error() const
{
  return errorStr;
}

// Public Slots

//---------------------------------------------------------------------------
// 
// parse - process replies from an IMPv2 client
//
// Parse messages, picking off those keyword=value pairs we care about
// and ignorning the rest
//
// This slot is meant to be invoked by the parent that operates the
// ISISClient instance.
//

void InstConfig::parse(const QString &cmdStr, 
		       const int &msgType, 
		       const QHash<QString,QString> &keyDict)
{
  // TODO: Unused cmdDone flag. Commented out on 2-19-24
  // bool cmdDone = false;   // Flag: message is a command completion
  bool cmdFault = false;  // Flag: command completed with a fault

  QString msgStr = cmdStr;
  msgStr = msgStr.simplified(); // TODO: Evaluate this line. Why is it here? 

  if (msgStr.isEmpty()) return; // Empty messages are ignored
  
  // TODO: Unused cmdDone flag. Commented out on 2-19-24
  // Our response depends on the type of message we received. 
  switch (msgType) {
  case ISISClient::ERROR:
    // cmdDone = true;
    cmdFault = true;
    break;

  case ISISClient::WARNING:
    // cmdDone = true;
    cmdFault = false;
    break;

  case ISISClient::FATAL:
    // cmdDone = true;
    cmdFault = true;
    break;

  case ISISClient::DONE:
    // cmdDone = true;
    cmdFault = false;
    break;

  case ISISClient::STATUS:
    // cmdDone = false;
    cmdFault = false;
    break;

  default:
    // Nothing to be done here, return (we don't handle REQ: or EXEC: requests)
    return;
    break;
  }

  // The first word in the message body is the command that
  // generated this response.

  QString cmdWord = msgStr.section(" ",0,0);

  // Were we passed any keyword=value pairs to interpret?

  int numKeys = keyDict.size();

  if (numKeys > 0) { // use Qt's built-in Java-style iterator to traverse the hash table
    QHashIterator<QString,QString> i(keyDict);
    while (i.hasNext()) {
      i.next();
      QString keyStr = i.key();
      QString valStr = i.value();

      // Some keywords depend on our focal station

      // Is this MODS instance authorized at its focal station?

      if (keyStr.compare("TELAUTH",Qt::CaseInsensitive)==0) {
	if (valStr.compare("T",Qt::CaseInsensitive)==0)
	  setLBTAuth(true);
	else if (valStr.compare("F",Qt::CaseInsensitive)==0)
	  setLBTAuth(false);
	if (cmdFault) authLight->setRed();
      }

      if (valStr.compare("BUSY",Qt::CaseInsensitive)==0) cmdFault = false;

    }
  }

  // Process fault conditions here, once we figure out what we need
  // to process

  /*
  if (cmdFault) {
    qDebug() << QString("Got a fault on MODS%1 setup panel, command %2").arg(modsID+1).arg(cmdWord);
  }
  */

  // Other things to do?  Do them here...

}


// Private Slots

// Set warm-start mode, invoked by the warmStart QRadioButton

void InstConfig::setWarmStart()
{
  warmStart = true;
}

// Set cold-start mode, invoked by the coldStart QRadioButton

void InstConfig::setColdStart()
{
  warmStart = false;
}

// Execute startup - we first put up a dialog to confirm this is
// really what they want to do (and its consequences).  The warmStart
// flag is used to tell which startup mode to use.

void InstConfig::doStartup()
{
  QString titleStr;
  QString msgStr;
  QString cmdStr;
  if (warmStart) {
    cmdStr = QString("STARTUP WARM");
    titleStr = tr("MODS%1 Warm Startup").arg(modsID+1);
    msgStr = tr("You have requested a warm start (restart) of MODS%1\n\n"
		"This will take approximately X minutes to complete"
		" and may change some of the current instrument configuration.\n"
		"\nAre you sure this is what you want to do?\n"
		).arg(modsID+1);
  }
  else {
    cmdStr = QString("STARTUP COLD");
    titleStr = tr("MODS%1 Cold Startup").arg(modsID+1);
    msgStr = tr("You have requested a cold start of MODS%1\n\n"
		"This will take approximately XX minutes to complete"
		" and will reset a number of instrument mechanisms,"
		" significantly changing the instrument configuration.\n"
		"\nAre you sure this is what you want to do?\n"
		).arg(modsID+1);
  }
  int ret = QMessageBox::warning(this,titleStr,msgStr,
				 (QMessageBox::Cancel | QMessageBox::Yes));
  switch (ret) {
  case QMessageBox::Yes:
    emit cmdReady(modsIEHost[modsID],cmdStr); // send the startup to the IE host
    emit cmdReady(modsBCHost[modsID],"ABORT"); // these make sure the ICs are not stuck in movie mode
    emit cmdReady(modsRCHost[modsID],"ABORT"); 
    emit cmdReady(modsTCHost[modsID],"TCINIT"); // tweak the TCS agent
    emit logMessage(tr("%1 Requested...").arg(titleStr));
    break;

  default:
    break;
  }

}

// Execute an instrument reset - this can be very time-consuming to
// recover from if done in error, so we warn the user of the
// consequences and demand confirmation before proceeding.
//
// The "reset" IE state all mechanisms stopped, and if needed moved to
// a safe "home" or "nominal" positions.
//
// The ICs are reset by aborting any exposures in progress, then
// setting full-frame readout, 1x1 binning, 0s exposure.
//
// The instrument is also disconnected from the LBT TCS/IIF interface
// and must be reconnected.

void InstConfig::doReset()
{
  QString titleStr = tr("MODS%1 Reset").arg(modsID+1);
  QString msgStr = tr("You have requested a full-instrument reset of MODS%1\n\n"
		      "This will take approximately XX minutes to complete"
		      " and will significantly change the current instrument configuration,"
		      " requiring several minutes to recover if this is not your intention.\n"
		      "\nAre you really sure this is what you want to do?\n"
		      ).arg(modsID+1);
  int ret = QMessageBox::warning(this,titleStr,msgStr,
				 (QMessageBox::Cancel | QMessageBox::Yes));
  switch (ret) {
  case QMessageBox::Yes:
    emit cmdReady(modsIEHost[modsID],"RESET ALL"); // send the reset to the IE host
    // These commands return the ICs to a known starting state
    emit cmdReady(modsBCHost[modsID],"ABORT");   
    emit cmdReady(modsBCHost[modsID],"CCDBIN 1 1");  // 1x1 binning
    emit cmdReady(modsBCHost[modsID],"ROI OFF");     // full-frame readout
    emit cmdReady(modsBCHost[modsID],"EXPTIME 0");
    emit cmdReady(modsRCHost[modsID],"ABORT");   
    emit cmdReady(modsRCHost[modsID],"CCDBIN 1 1");  // 1x1 binning
    emit cmdReady(modsRCHost[modsID],"ROI OFF");     // full-frame readout
    emit cmdReady(modsRCHost[modsID],"EXPTIME 0");
    emit logMessage(tr("%1 Requested...").arg(titleStr));
    break;

  default:
    break;
  }
}

// Put the instrument to sleep.  Waking requires a full startup.

void InstConfig::doSleep()
{
  QString titleStr = tr("MODS%1 Sleep (Low-Power Standby)").arg(modsID+1);
  QString msgStr = tr("You are about to put MODS%1 to sleep (low-power standby mode)\n\n"
		      "This will take approximately X minutes to complete"
		      " and will require a full restart before you can use it again.\n"
		      "\nAre you really sure this is what you want to do?\n"
		      ).arg(modsID+1);
  int ret = QMessageBox::warning(this,titleStr,msgStr,
				 (QMessageBox::Cancel | QMessageBox::Yes));
  switch (ret) {
  case QMessageBox::Yes:
    // Send the SLEEP command to the IE host
    emit cmdReady(modsIEHost[modsID],"SLEEP");
    // Put each of the ICs into movie mode with 0s exposure, 8x8 binning
    emit cmdReady(modsBCHost[modsID],"EXPTIME 0");
    // emit cmdReady(modsBCHost[modsID],"CCDBIN 8 8");
    // emit cmdReady(modsBCHost[modsID],"MOVIE");
    emit cmdReady(modsRCHost[modsID],"EXPTIME 0");
    // emit cmdReady(modsRCHost[modsID],"CCDBIN 8 8");
    // emit cmdReady(modsRCHost[modsID],"MOVIE");
    emit logMessage(tr("%1 Requested...").arg(titleStr));
    break;

  default:
    break;
  }

}

// Stow instrument mechanisms.  

void InstConfig::doStow()
{
  QString titleStr = tr("MODS%1 Stow Mechanisms").arg(modsID+1);
  QString msgStr = tr("You are about to stow (home) all MODS%1 mechanisms\n\n"
		      "This will take approximately X minutes to complete"
		      " and will significantly change the current instrument configuration,"
		      " requiring several minutes to restore if this is not your intention.\n"
		      "\nAre you really sure this is what you want to do?\n"
		      ).arg(modsID+1);
  int ret = QMessageBox::warning(this,titleStr,msgStr,
				 (QMessageBox::Cancel | QMessageBox::Yes));
  switch (ret) {
  case QMessageBox::Yes:
    // Send the STOW command to the IE host
    emit cmdReady(modsIEHost[modsID],"STOW");
    emit logMessage(tr("%1 Requested...").arg(titleStr));
    break;

  default:
    break;
  }

}

// Shutdown the instrument.  This means a full power off to all systems, including
// detectors.  Restarting can be very time-consuming.

void InstConfig::doShutdown()
{
  QString titleStr = tr("MODS%1 Shutdown").arg(modsID+1);
  QString msgStr = tr("You have requested a full shutdown of MODS%1\n\n"
		      "This will take approximately X minutes to complete, and it will"
		      " stow (home) all mechanisms and then turn off power to all"
		      " instrument systems (mechanisms, detectors, guiders, and utilities).\n"
		      "\nA full power-up and restart of the instrument taking several minutes"
		      " will be required before can use it again.\n"
		      "\nAre you really sure this is what you want to do?\n"
		      ).arg(modsID+1);
  int ret = QMessageBox::warning(this,titleStr,msgStr,
				 (QMessageBox::Cancel | QMessageBox::Yes));
  switch (ret) {
  case QMessageBox::Yes:
    // Send the SHUTDOWN command to the IE host
    emit cmdReady(modsIEHost[modsID],"SHUTDOWN");
    emit logMessage(tr("%1 Requested...").arg(titleStr));
    break;

  default:
    break;
  }

}

// Launch the various load dialogs

void InstConfig::launchSlitLoad()
{
  logMessage(tr("MODS%1 slit mask load/scan dialog not yet implemented").arg(modsID+1));
}

void InstConfig::launchBFiltLoad()
{
  logMessage(tr("MODS%1 blue filter wheel load dialog not yet implemented").arg(modsID+1));
}

void InstConfig::launchRFiltLoad()
{
  logMessage(tr("MODS%1 red filter wheel load dialog not yet implemented").arg(modsID+1));
}

// Enable/Disable the widget

void InstConfig::enable()
{
  setEnabled(true);
}

void InstConfig::disable()
{
  setEnabled(false);
}

// Query the LBT telescope authorization state of the instrument.
// Actual authorization as the "beam-receiving instrument" is done by
// the telescope operator.  This lets us verify if we are the
// authorized instrument.  Without authorization, we can only issue
// status queries and active command requests (preset, offset, etc.)
// will be denied.

void InstConfig::checkLBTAuth()
{
  QString cmdStr;
  cmdStr = "AUTHORIZED";
  emit cmdReady(modsTCHost[modsID],cmdStr);
}

// Set the LBT authorization state - propagates the LBT connection
// status to all widgets that care, usually invoked by the parse()
// method.

void InstConfig::setLBTAuth(const bool &connectState)
{
  if (connectState) {
    isAuthorized = true;
    authLight->setGreen();
  }
  else {
    isAuthorized = false;
    authLight->setOff();
  }
}
