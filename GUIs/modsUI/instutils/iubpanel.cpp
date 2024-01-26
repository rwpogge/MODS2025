#include <QtGui>
#include <QtNetwork>
#include <math.h>

#include "iubpanel.h"
#include "powerswitch.h"
#include "powerbreaker.h"
#include "actionbutton.h"
#include "powerbutton.h"
#include "textdisplay.h"
#include "valuedisplay.h"

#include "isisclient.h"

IUBPanel::IUBPanel(const int &myMODS, QWidget *parent)
  : QWidget(parent)
{
  // Initialize parameters and state variables
  
  modsID = myMODS;
  panelName = QString("MODS%1").arg(modsID+1);
  powerOn = false;
  sleepMode = false;

  // int pwrSize = 128;
  int pwrSize = 96;

  // Power buttons are arrayed in a 1x7 grid

  QGridLayout *powerGridLayout = new QGridLayout;
  powerGridLayout->setHorizontalSpacing(0);

  int iCol = 0;

  llbPower = new PowerBreaker("Lamp/Laser","UTIL LLB",pwrSize,this);
  connect(llbPower,SIGNAL(cmdReady(const QString &)),
	  this,SLOT(sendIECommand(const QString &)));
  powerGridLayout->addWidget(llbPower,0,iCol,Qt::AlignHCenter);

  iCol++;
  agwGuidePower = new PowerBreaker("AGw Guide","UTIL AGC",pwrSize,this);
  connect(agwGuidePower,SIGNAL(cmdReady(const QString &)),
	  this,SLOT(sendIECommand(const QString &)));
  powerGridLayout->addWidget(agwGuidePower,0,iCol,Qt::AlignHCenter);

  iCol++;
  agwWFSPower = new PowerBreaker("AGw WFS","UTIL WFS",pwrSize,this);
  connect(agwWFSPower,SIGNAL(cmdReady(const QString &)),
	  this,SLOT(sendIECommand(const QString &)));
  powerGridLayout->addWidget(agwWFSPower,0,iCol,Qt::AlignHCenter);

  iCol++;
  blueIEBPower = new PowerBreaker("Blue IEB","UTIL IEB_B",pwrSize,this);
  connect(blueIEBPower,SIGNAL(cmdReady(const QString &)),
	  this,SLOT(sendIECommand(const QString &)));
  powerGridLayout->addWidget(blueIEBPower,0,iCol,Qt::AlignHCenter);

  iCol++;
  redIEBPower = new PowerBreaker("Red IEB","UTIL IEB_R",pwrSize,this);
  connect(redIEBPower,SIGNAL(cmdReady(const QString &)),
	  this,SLOT(sendIECommand(const QString &)));
  powerGridLayout->addWidget(redIEBPower,0,iCol,Qt::AlignHCenter);

  iCol++;
  blueHEBPower = new PowerBreaker("Blue HEB","UTIL HEB_B",pwrSize,this);
  connect(blueHEBPower,SIGNAL(cmdReady(const QString &)),
	  this,SLOT(sendIECommand(const QString &)));
  powerGridLayout->addWidget(blueHEBPower,0,iCol,Qt::AlignHCenter);

  iCol++;
  redHEBPower = new PowerBreaker("Red HEB","UTIL HEB_R",pwrSize,this);
  connect(redHEBPower,SIGNAL(cmdReady(const QString &)),
	  this,SLOT(sendIECommand(const QString &)));
  powerGridLayout->addWidget(redHEBPower,0,iCol,Qt::AlignHCenter);

  iCol++;
  lockPower = true;
  lockPowerCheck = new QCheckBox(tr("Lock\nPower"),this);
  if (lockPower)
    lockPowerCheck->setCheckState(Qt::Checked);
  else
    lockPowerCheck->setCheckState(Qt::Unchecked);
  connect(lockPowerCheck,SIGNAL(stateChanged(int)),this,SLOT(setPowerLock()));
  powerGridLayout->addWidget(lockPowerCheck,0,iCol,Qt::AlignHCenter);

  QGroupBox *powerGroupBox = new QGroupBox(tr("%1 Power").arg(panelName));
  powerGroupBox->setLayout(powerGridLayout);

  // Glycol Supply & Return Monitor

  QFormLayout *gsLayout = new QFormLayout;
  gsLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
  gsLayout->setFormAlignment(Qt::AlignCenter);  
  gsLayout->setLabelAlignment(Qt::AlignRight);
  gsLayout->setVerticalSpacing(0);

  gsPDisplay = new TextDisplay("","psi",5,this);
  gsPDisplay->setNormalColors(Qt::black,Qt::green);
  gsPDisplay->setFaultColors(Qt::black,Qt::red);
  gsPDisplay->setChangeColors(Qt::black,QColor(255,165,0,255));
  gsPDisplay->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  gsPDisplay->clear();
  gsLayout->addRow(tr("Pres"),gsPDisplay);

  gsTDisplay = new TextDisplay("","C",5,this);
  gsTDisplay->setNormalColors(Qt::black,Qt::green);
  gsTDisplay->setFaultColors(Qt::black,Qt::red);
  gsTDisplay->setChangeColors(Qt::black,QColor(255,165,0,255));
  gsTDisplay->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  gsTDisplay->clear();
  gsLayout->addRow(tr("Temp"),gsTDisplay);

  QGroupBox *gsGroupBox = new QGroupBox(tr("Glycol Supply"));
  gsGroupBox->setLayout(gsLayout);
  
  // Glycol Return P and T

  QFormLayout *grLayout = new QFormLayout;
  grLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
  grLayout->setFormAlignment(Qt::AlignCenter);  
  grLayout->setLabelAlignment(Qt::AlignRight);
  grLayout->setVerticalSpacing(0);

  grPDisplay = new TextDisplay("","psi",5,this);
  grPDisplay->setNormalColors(Qt::black,Qt::green);
  grPDisplay->setFaultColors(Qt::black,Qt::red);
  grPDisplay->setChangeColors(Qt::black,QColor(255,165,0,255));
  grPDisplay->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  grPDisplay->clear();
  grLayout->addRow(tr("Pres"),grPDisplay);

  grTDisplay = new TextDisplay("","C",5,this);
  grTDisplay->setNormalColors(Qt::black,Qt::green);
  grTDisplay->setFaultColors(Qt::black,Qt::red);
  grTDisplay->setChangeColors(Qt::black,QColor(255,165,0,255));
  grTDisplay->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  grTDisplay->clear();
  grLayout->addRow(tr("Temp"),grTDisplay);

  QGroupBox *grGroupBox = new QGroupBox(tr("Glycol Return"));
  grGroupBox->setLayout(grLayout);
  
  // Temperature Sensors

  // IUB Air and AGw Heat Sink

  QFormLayout *iubTempLayout = new QFormLayout;
  iubTempLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
  iubTempLayout->setFormAlignment(Qt::AlignCenter);  
  iubTempLayout->setLabelAlignment(Qt::AlignRight);
  iubTempLayout->setVerticalSpacing(0);

  iubTDisplay = new TextDisplay("","C",5,this);
  iubTDisplay->setNormalColors(Qt::black,Qt::green);
  iubTDisplay->setFaultColors(Qt::black,Qt::red);
  iubTDisplay->setChangeColors(Qt::black,QColor(255,165,0,255));
  iubTDisplay->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  iubTDisplay->clear();
  iubTempLayout->addRow(tr("Air"),iubTDisplay);

  agwTDisplay = new TextDisplay("","C",5,this);
  agwTDisplay->setNormalColors(Qt::black,Qt::green);
  agwTDisplay->setFaultColors(Qt::black,Qt::red);
  agwTDisplay->setChangeColors(Qt::black,QColor(255,165,0,255));
  agwTDisplay->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  agwTDisplay->clear();
  iubTempLayout->addRow(tr("Glycol"),agwTDisplay);

  QGroupBox *iubTempGroupBox = new QGroupBox(tr("IUB"));
  iubTempGroupBox->setLayout(iubTempLayout);

  // Blue IEB Air and Glycol

  QFormLayout *iebBTempLayout = new QFormLayout;
  iebBTempLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
  iebBTempLayout->setFormAlignment(Qt::AlignCenter);  
  iebBTempLayout->setLabelAlignment(Qt::AlignRight);
  iebBTempLayout->setVerticalSpacing(0);

  iebBTDisplay = new TextDisplay("","C",5,this);
  iebBTDisplay->setNormalColors(Qt::black,Qt::green);
  iebBTDisplay->setFaultColors(Qt::black,Qt::red);
  iebBTDisplay->setChangeColors(Qt::black,QColor(255,165,0,255));
  iebBTDisplay->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  iebBTDisplay->clear();
  iebBTempLayout->addRow(tr("Air"),iebBTDisplay);

  iebBGDisplay = new TextDisplay("","C",5,this);
  iebBGDisplay->setNormalColors(Qt::black,Qt::green);
  iebBGDisplay->setFaultColors(Qt::black,Qt::red);
  iebBGDisplay->setChangeColors(Qt::black,QColor(255,165,0,255));
  iebBGDisplay->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  iebBGDisplay->clear();
  iebBTempLayout->addRow(tr("Glycol"),iebBGDisplay);

  QGroupBox *iebBTempGroupBox = new QGroupBox(tr("Blue IEB"));
  iebBTempGroupBox->setLayout(iebBTempLayout);

  // Red IEB Air and Glycol

  QFormLayout *iebRTempLayout = new QFormLayout;
  iebRTempLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
  iebRTempLayout->setFormAlignment(Qt::AlignCenter);  
  iebRTempLayout->setLabelAlignment(Qt::AlignRight);
  iebRTempLayout->setVerticalSpacing(0);

  iebRTDisplay = new TextDisplay("","C",5,this);
  iebRTDisplay->setNormalColors(Qt::black,Qt::green);
  iebRTDisplay->setFaultColors(Qt::black,Qt::red);
  iebRTDisplay->setChangeColors(Qt::black,QColor(255,165,0,255));
  iebRTDisplay->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  iebRTDisplay->clear();
  iebRTempLayout->addRow(tr("Air"),iebRTDisplay);

  iebRGDisplay = new TextDisplay("","C",5,this);
  iebRGDisplay->setNormalColors(Qt::black,Qt::green);
  iebRGDisplay->setFaultColors(Qt::black,Qt::red);
  iebRGDisplay->setChangeColors(Qt::black,QColor(255,165,0,255));
  iebRGDisplay->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  iebRGDisplay->clear();
  iebRTempLayout->addRow(tr("Glycol"),iebRGDisplay);

  QGroupBox *iebRTempGroupBox = new QGroupBox(tr("Red IEB"));
  iebRTempGroupBox->setLayout(iebRTempLayout);

  // Blue CCD HEB Air and Glycol

  QFormLayout *hebBTempLayout = new QFormLayout;
  hebBTempLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
  hebBTempLayout->setFormAlignment(Qt::AlignCenter);  
  hebBTempLayout->setLabelAlignment(Qt::AlignRight);
  hebBTempLayout->setVerticalSpacing(0);

  hebBTDisplay = new TextDisplay("","C",5,this);
  hebBTDisplay->setNormalColors(Qt::black,Qt::green);
  hebBTDisplay->setFaultColors(Qt::black,Qt::red);
  hebBTDisplay->setChangeColors(Qt::black,QColor(255,165,0,255));
  hebBTDisplay->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  hebBTDisplay->clear();
  hebBTempLayout->addRow(tr("Air"),hebBTDisplay);

  hebBGDisplay = new TextDisplay("","C",5,this);
  hebBGDisplay->setNormalColors(Qt::black,Qt::green);
  hebBGDisplay->setFaultColors(Qt::black,Qt::red);
  hebBGDisplay->setChangeColors(Qt::black,QColor(255,165,0,255));
  hebBGDisplay->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  hebBGDisplay->clear();
  hebBTempLayout->addRow(tr("Glycol"),hebBGDisplay);

  QGroupBox *hebBTempGroupBox = new QGroupBox(tr("Blue HEB"));
  hebBTempGroupBox->setLayout(hebBTempLayout);

  // Red CCD HEB Air and Glycol

  QFormLayout *hebRTempLayout = new QFormLayout;
  hebRTempLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
  hebRTempLayout->setFormAlignment(Qt::AlignCenter);  
  hebRTempLayout->setLabelAlignment(Qt::AlignRight);
  hebRTempLayout->setVerticalSpacing(0);

  hebRTDisplay = new TextDisplay("","C",5,this);
  hebRTDisplay->setNormalColors(Qt::black,Qt::green);
  hebRTDisplay->setFaultColors(Qt::black,Qt::red);
  hebRTDisplay->setChangeColors(Qt::black,QColor(255,165,0,255));
  hebRTDisplay->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  hebRTDisplay->clear();
  hebRTempLayout->addRow(tr("Air"),hebRTDisplay);

  hebRGDisplay = new TextDisplay("","C",5,this);
  hebRGDisplay->setNormalColors(Qt::black,Qt::green);
  hebRGDisplay->setFaultColors(Qt::black,Qt::red);
  hebRGDisplay->setChangeColors(Qt::black,QColor(255,165,0,255));
  hebRGDisplay->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  hebRGDisplay->clear();
  hebRTempLayout->addRow(tr("Glycol"),hebRGDisplay);

  QGroupBox *hebRTempGroupBox = new QGroupBox(tr("Red HEB"));
  hebRTempGroupBox->setLayout(hebRTempLayout);

  // Lay them out side-by-side

  QHBoxLayout *envLayout = new QHBoxLayout;
  envLayout->addWidget(gsGroupBox);
  envLayout->addWidget(grGroupBox);
  envLayout->addWidget(iubTempGroupBox);
  envLayout->addWidget(iebBTempGroupBox);
  envLayout->addWidget(iebRTempGroupBox);
  envLayout->addWidget(hebBTempGroupBox);
  envLayout->addWidget(hebRTempGroupBox);

  // Ambient Air temperature readout, update time, and update button

  ambTDisplay = new TextDisplay("Ambient:","C",5,this);
  ambTDisplay->setNormalColors(Qt::black,Qt::green);
  ambTDisplay->setFaultColors(Qt::black,Qt::red);
  ambTDisplay->setChangeColors(Qt::black,QColor(255,165,0,255));
  ambTDisplay->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  ambTDisplay->clear();

  updateDisplay = new ValueDisplay("","UTC",this);

  updateButton = new ActionButton(tr("Update"),this);
  connect(updateButton,SIGNAL(clicked()),this,SLOT(update()));

  QHBoxLayout *ambTempLayout = new QHBoxLayout;
  ambTempLayout->addWidget(ambTDisplay);
  ambTempLayout->addStretch();
  ambTempLayout->addWidget(updateDisplay);
  ambTempLayout->addStretch();
  ambTempLayout->addWidget(updateButton);

  // Organize the contents

  QVBoxLayout *iubLayout = new QVBoxLayout;
  iubLayout->addWidget(powerGroupBox);
  iubLayout->addLayout(envLayout);
  iubLayout->addLayout(ambTempLayout);

  // Put it into the a group box...

  QGroupBox *mainGroupBox = new QGroupBox(tr("%1 Instrument Utility Box").arg(panelName));
  mainGroupBox->setAlignment(Qt::AlignHCenter);
  mainGroupBox->setContentsMargins(3,15,3,3);
  mainGroupBox->setLayout(iubLayout);
  
  // ... and build the final layout.

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(mainGroupBox);
  mainLayout->setContentsMargins(0,0,0,0);
  setLayout(mainLayout);

  // Other stuff?

  gsPDisplay->setNormal();
  gsTDisplay->setNormal();
  grPDisplay->setNormal();
  grTDisplay->setNormal();
  ambTDisplay->setNormal();
  iubTDisplay->setNormal();
  agwTDisplay->setNormal();
  iebBTDisplay->setNormal();
  iebBGDisplay->setNormal();
  iebRTDisplay->setNormal();
  iebRGDisplay->setNormal();
  hebBTDisplay->setNormal();
  hebBGDisplay->setNormal();
  hebRTDisplay->setNormal();
  hebRGDisplay->setNormal();
}

// Send a command to the IE Server. This emits the cmdReady() signal
// to whoever is listening for commands to pass along.
//
// For this instance, if lockPower is true, we block sending the
// command as the panel is locked out and only send a query, otherwise
// permit the command to execute.

void IUBPanel::sendIECommand(const QString &cmd)
{
  if (cmd.isEmpty()) return;

  // Seems silly, but in future versions we will need a working
  // command string...

  QString cmdStr = cmd;

  // We have something to send, emit the cmdReady() signal If we have
  // power locked, only send a status query.  This resets the button,
  // and updates the rest.

  if (lockPower) {
    emit statusMsg(tr("MODS%1 Utility Box Power Controls are LOCKED").arg(modsID+1),Qt::red);
    QString titleStr = tr("MODS%1 Utility Box Power Controls are LOCKED").arg(modsID+1);
    QString msgStr = tr("The MODS%1 Utility Box Power Controls are currently locked out.\n\n"
			"This is a safety feature to prevent you from accidentally"
			" changing the power state of critical systems in an operating"
			" instrument that could result in loss of data or many minutes"
			" of lost time recovering instrument function.\n"
			"\nUnlock the power controls at right if this is really what you"
			" want to do.\n").arg(modsID+1);
    QMessageBox::information(this,titleStr,msgStr,QMessageBox::Ok);    
    cmdStr = "UTIL";
  }
  emit cmdReady(modsIEHost[modsID],cmdStr);

}

// Send a command to a particular host and wait for reply up to a
// timeout interval if timeout>0 otherwise queue to the command
// sequencer if timeout<0, or just send and forget if timeout=0

void IUBPanel::sendCmdWait(const QString &hostStr, const QString &cmdStr, 
			   const int &timeout)
{
  if (hostStr.isEmpty() || cmdStr.isEmpty()) return;
  
  if (timeout == 0)
    emit cmdReady(hostStr,cmdStr);
  else
    emit cmdWait(hostStr,cmdStr,timeout);
}

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

void IUBPanel::parse(const QString &remHost, 
		     const QString &cmdStr,
		     const int &msgType, 
		     const QHash<QString,QString> &keyDict)
{
  bool cmdDone = false;   // Flag: message is a command completion
  bool cmdFault = false;  // Flag: command completed with a fault
  bool ok;
  bool fromIE = false; 
  bool isRed = false;

  QString msgStr = cmdStr;
  msgStr.simplified();

  if (msgStr.isEmpty()) return; // Empty messages are ignored
  
  // Our response depends on the type of message we received. 

  switch (msgType) {
  case ISISClient::ERROR:
    cmdDone = true;
    cmdFault = true;
    break;

  case ISISClient::WARNING:
    cmdDone = true;
    cmdFault = false;
    break;

  case ISISClient::FATAL:
    cmdDone = true;
    cmdFault = true;
    break;

  case ISISClient::DONE:
    cmdDone = true;
    cmdFault = false;
    break;

  case ISISClient::STATUS:
    cmdDone = false;
    cmdFault = false;
    break;

  default:
    // Nothing to be done here, return (we don't handle REQ: or EXEC: requests)
    return;
    break;
  }

  // The first word in the message body is the command that generated
  // this response.  The conservative implementation of this parser
  // demands that the command word by "UTIL", otherwise we ignore it
  // so that we don't confuse keywords.

  QString cmdWord = msgStr.section(" ",0,0);
  if (cmdWord.compare("UTIL",Qt::CaseInsensitive)!=0 &&
      cmdWord.compare("ESTATUS",Qt::CaseInsensitive)!=0) return;

  // Were we passed any keyword=value pairs to interpret?

  int numKeys = keyDict.size();
  if (numKeys < 1) return;

  // Who did this come from?

  if (remHost.compare(modsIEHost[modsID],Qt::CaseInsensitive)==0) {
    fromIE = true;
    isRed = false;
  }
  else if (remHost.compare(modsBCHost[modsID],Qt::CaseInsensitive)==0) {
    fromIE = false;
    isRed = false;
  }
  else if (remHost.compare(modsRCHost[modsID],Qt::CaseInsensitive)==0) {
    fromIE = false;
    isRed = true;
  }
  else {
    return; // nobody we care to talk to...
  }

  // We have keys to interpret, do it.  Use the built-in Java-style
  // iterator to traverse the keyword dictionary hash.

  QHashIterator<QString,QString> i(keyDict);
  while (i.hasNext()) {
    i.next();
    QString keyStr = i.key();
    QString valStr = i.value();
    
    int numUpdated = 0;
      
    // Is the message from our IE server?

    if (fromIE) { 
      // IEB_B power state
    
      if (keyStr.compare("IEB_B",Qt::CaseInsensitive)==0) {
	if (valStr.compare("ON",Qt::CaseInsensitive)==0)
	  blueIEBPower->setOn();
	else if (valStr.compare("OFF",Qt::CaseInsensitive)==0)
	  blueIEBPower->setOff();
	else
	  blueIEBPower->setFault();
	numUpdated++;
      }
      
      else if (keyStr.compare("IEB_B_BRK",Qt::CaseInsensitive)==0) {
	if (valStr.compare("OK",Qt::CaseInsensitive)==0)
	  blueIEBPower->setBreakerOK();
	else if (valStr.compare("FAULT",Qt::CaseInsensitive)==0)
	  blueIEBPower->setBreakerFault();
	else
	  blueIEBPower->setBreakerUnknown();
	numUpdated++;
      }
      
      // IEB_R power state
      
      else if (keyStr.compare("IEB_R",Qt::CaseInsensitive)==0) {
	if (valStr.compare("ON",Qt::CaseInsensitive)==0)
	  redIEBPower->setOn();
	else if (valStr.compare("OFF",Qt::CaseInsensitive)==0)
	  redIEBPower->setOff();
	else
	  redIEBPower->setFault();
	numUpdated++;
      }
      
      else if (keyStr.compare("IEB_R_BRK",Qt::CaseInsensitive)==0) {
	if (valStr.compare("OK",Qt::CaseInsensitive)==0)
	  redIEBPower->setBreakerOK();
	else if (valStr.compare("FAULT",Qt::CaseInsensitive)==0)
	  redIEBPower->setBreakerFault();
	else
	  redIEBPower->setBreakerUnknown();
	numUpdated++;
      }
      
      // Blue HEB power state
      
      else if (keyStr.compare("HEB_B",Qt::CaseInsensitive)==0) {
	if (valStr.compare("ON",Qt::CaseInsensitive)==0)
	  blueHEBPower->setOn();
	else if (valStr.compare("OFF",Qt::CaseInsensitive)==0)
	  blueHEBPower->setOff();
	else
	  blueHEBPower->setFault();
	numUpdated++;
      }
      
      else if (keyStr.compare("HEB_B_BRK",Qt::CaseInsensitive)==0) {
	if (valStr.compare("OK",Qt::CaseInsensitive)==0)
	  blueHEBPower->setBreakerOK();
	else if (valStr.compare("FAULT",Qt::CaseInsensitive)==0)
	  blueHEBPower->setBreakerFault();
	else
	  blueHEBPower->setBreakerUnknown();
	numUpdated++;
      }
      
      // Red HEB power state
      
      else if (keyStr.compare("HEB_R",Qt::CaseInsensitive)==0) {
	if (valStr.compare("ON",Qt::CaseInsensitive)==0)
	  redHEBPower->setOn();
	else if (valStr.compare("OFF",Qt::CaseInsensitive)==0)
	  redHEBPower->setOff();
	else
	  redHEBPower->setFault();
	numUpdated++;
      }
      
      else if (keyStr.compare("HEB_R_BRK",Qt::CaseInsensitive)==0) {
	if (valStr.compare("OK",Qt::CaseInsensitive)==0)
	  redHEBPower->setBreakerOK();
	else if (valStr.compare("FAULT",Qt::CaseInsensitive)==0)
	  redHEBPower->setBreakerFault();
	else
	  redHEBPower->setBreakerUnknown();
	numUpdated++;
      }
      
      // AGW Guide Camera power state
      
      else if (keyStr.compare("AGC",Qt::CaseInsensitive)==0) {
	if (valStr.compare("ON",Qt::CaseInsensitive)==0)
	  agwGuidePower->setOn();
	else if (valStr.compare("OFF",Qt::CaseInsensitive)==0)
	  agwGuidePower->setOff();
	else
	  agwGuidePower->setFault();
	numUpdated++;
      }
      
      else if (keyStr.compare("AGC_BRK",Qt::CaseInsensitive)==0) {
	if (valStr.compare("OK",Qt::CaseInsensitive)==0)
	  agwGuidePower->setBreakerOK();
	else if (valStr.compare("FAULT",Qt::CaseInsensitive)==0)
	  agwGuidePower->setBreakerFault();
	else
	  agwGuidePower->setBreakerUnknown();
	numUpdated++;
      }
      
      // AGW WFS Camera power state
      
      else if (keyStr.compare("WFS",Qt::CaseInsensitive)==0) {
	if (valStr.compare("ON",Qt::CaseInsensitive)==0)
	  agwWFSPower->setOn();
	else if (valStr.compare("OFF",Qt::CaseInsensitive)==0)
	  agwWFSPower->setOff();
	else
	  agwWFSPower->setFault();
	numUpdated++;
      }
      
      else if (keyStr.compare("WFS_BRK",Qt::CaseInsensitive)==0) {
	if (valStr.compare("OK",Qt::CaseInsensitive)==0)
	  agwWFSPower->setBreakerOK();
	else if (valStr.compare("FAULT",Qt::CaseInsensitive)==0)
	  agwWFSPower->setBreakerFault();
	else
	  agwWFSPower->setBreakerUnknown();
	numUpdated++;
      }
      
      // Lamp/Laser Box power state
      
      else if (keyStr.compare("LLB",Qt::CaseInsensitive)==0) {
	if (valStr.compare("ON",Qt::CaseInsensitive)==0)
	  llbPower->setOn();
	else if (valStr.compare("OFF",Qt::CaseInsensitive)==0)
	  llbPower->setOff();
	else
	  llbPower->setFault();
	numUpdated++;
      }
      
      else if (keyStr.compare("LLB_BRK",Qt::CaseInsensitive)==0) {
	if (valStr.compare("OK",Qt::CaseInsensitive)==0)
	  llbPower->setBreakerOK();
	else if (valStr.compare("FAULT",Qt::CaseInsensitive)==0)
	  llbPower->setBreakerFault();
	else
	  llbPower->setBreakerUnknown();
	numUpdated++;
      }
      
      // Pressure and Temperature Sensor Readouts
      
      // Glycol Supply in-line pressure sensor
      
      else if (keyStr.compare("GSPRES",Qt::CaseInsensitive)==0) {
	double dtmp = valStr.toDouble(&ok);
	if (ok) {
	  gsPressure = dtmp;
	  gsPDisplay->setText(QString::number(gsPressure,'f',1),Qt::green);
	  gsPDisplay->setNormal();
	}
	else 
	  gsPDisplay->clear();
	numUpdated++;
      }
      
      // Main Glycol Supply in-line temperature sensor
      
      else if (keyStr.compare("GSTEMP",Qt::CaseInsensitive)==0) {
	double dtmp = valStr.toDouble(&ok);
	if (ok && dtmp < 200.0) {
	  gsTemp = dtmp;
	  gsTDisplay->setText(QString::number(gsTemp,'f',1),Qt::green);
	  gsTDisplay->setNormal();
	}
	else 
	  gsTDisplay->clear();
	numUpdated++;
      }
      
      // Main Glycol Return in-line pressure sensor
      
      else if (keyStr.compare("GRPRES",Qt::CaseInsensitive)==0) {
	double dtmp = valStr.toDouble(&ok);
	if (ok) {
	  grPressure = dtmp;
	  grPDisplay->setText(QString::number(grPressure,'f',1),Qt::green);
	  grPDisplay->setNormal();
	}
	else 
	  grPDisplay->clear();
	numUpdated++;
      }
      
      // Main Glycol Return in-line temperature sensor
      
      else if (keyStr.compare("GRTEMP",Qt::CaseInsensitive)==0) {
	double dtmp = valStr.toDouble(&ok);
	if (ok && dtmp < 200.0) {
	  grTemp = dtmp;
	  grTDisplay->setText(QString::number(grTemp,'f',1),Qt::green);
	  grTDisplay->setNormal();
	}
	else 
	  grTDisplay->clear();
	numUpdated++;
      }
      
      // Outside-Instrument ambient air temperature near the IUB
      
      else if (keyStr.compare("AMBTEMP",Qt::CaseInsensitive)==0) {
	double dtmp = valStr.toDouble(&ok);
	if (ok && dtmp < 200.0) {
	  ambTemp = dtmp;
	  ambTDisplay->setText(QString::number(ambTemp,'f',1),Qt::green);
	  ambTDisplay->setNormal();
	}
	else 
	  ambTDisplay->clear();
	numUpdated++;
      }
      
      // Air temperature inside the IUB
      
      else if (keyStr.compare("IUBTAIR",Qt::CaseInsensitive)==0) {
	double dtmp = valStr.toDouble(&ok);
	if (ok && dtmp < 200.0) {
	  iubTemp = dtmp;
	  iubTDisplay->setText(QString::number(iubTemp,'f',1),Qt::green);
	  iubTDisplay->setNormal();
	}
	else 
	  iubTDisplay->clear();
	numUpdated++;
      }
      
      // Temperature of the AGw controller heat sink
      
      else if (keyStr.compare("AGHSTEMP",Qt::CaseInsensitive)==0) {
	double dtmp = valStr.toDouble(&ok);
	if (ok && dtmp < 200.0) {
	  agwTemp = dtmp;
	  agwTDisplay->setText(QString::number(agwTemp,'f',1),Qt::green);
	  agwTDisplay->setNormal();
	}
	else 
	  agwTDisplay->clear();
	numUpdated++;
      }
      
      // Blue IEB Temperature Sensors
      
      else if (keyStr.compare("IEBTEMPB",Qt::CaseInsensitive)==0) {
	double dtmp = valStr.toDouble(&ok);
	if (ok && dtmp < 200.0) {
	  iebBTemp = dtmp;
	  iebBTDisplay->setText(QString::number(iebBTemp,'f',1),Qt::green);
	  iebBTDisplay->setNormal();
	}
	else 
	  iebBTDisplay->clear();
	numUpdated++;
      }
      
      else if (keyStr.compare("IEBGRT_B",Qt::CaseInsensitive)==0) {
	double dtmp = valStr.toDouble(&ok);
	if (ok && dtmp < 200.0) {
	  iebBGTemp = dtmp;
	  iebBGDisplay->setText(QString::number(iebBGTemp,'f',1),Qt::green);
	  iebBGDisplay->setNormal();
	}
	else 
	  iebBGDisplay->clear();
	numUpdated++;
      }
      
      // Red IEB Temperature Sensors
      
      else if (keyStr.compare("IEBTEMPR",Qt::CaseInsensitive)==0) {
	double dtmp = valStr.toDouble(&ok);
	if (ok && dtmp < 200.0) {
	  iebRTemp = dtmp;
	  iebRTDisplay->setText(QString::number(iebRTemp,'f',1),Qt::green);
	  iebRTDisplay->setNormal();
	}
	else 
	  iebRTDisplay->clear();
	numUpdated++;
      }
      
      else if (keyStr.compare("IEBGRT_R",Qt::CaseInsensitive)==0) {
	double dtmp = valStr.toDouble(&ok);
	if (ok && dtmp < 200.0) {
	  iebRGTemp = dtmp;
	  iebRGDisplay->setText(QString::number(iebRGTemp,'f',1),Qt::green);
	  iebRGDisplay->setNormal();
	}
	else 
	  iebRGDisplay->clear();
	numUpdated++;
      }
      
      else {
	// unrecognized keyword, move along...
      }
      
    }
    else {
      // Message is from one of the HEB servers
      
      // HEB glycol return temperature

      if (keyStr.compare("CTEMPOUT",Qt::CaseInsensitive)==0) {
	double dtmp = valStr.toDouble(&ok);
	if (isRed) {
	  if (ok && dtmp < 200.0) {
	    hebRGDisplay->setText(QString::number(dtmp,'f',1),Qt::green);
	    hebRGDisplay->setNormal();
	  }
	  else 
	    hebRGDisplay->clear();
	}
	else {
	  if (ok && dtmp < 200.0) {
	    hebBGDisplay->setText(QString::number(dtmp,'f',1),Qt::green);
	    hebBGDisplay->setNormal();
	  }
	  else 
	    hebBGDisplay->clear();
	}
	numUpdated++;
      }
      
      // HEB inside Air temperature
      
      else if (keyStr.compare("HEBTEMP",Qt::CaseInsensitive)==0) {
	double dtmp = valStr.toDouble(&ok);
	if (isRed) {
	  if (ok && dtmp < 200.0) {
	    hebRTDisplay->setText(QString::number(dtmp,'f',1),Qt::green);
	    hebRTDisplay->setNormal();
	  }
	  else 
	    hebRTDisplay->clear();
	}
	else {
	  if (ok && dtmp < 200.0) {
	    hebBTDisplay->setText(QString::number(dtmp,'f',1),Qt::green);
	    hebBTDisplay->setNormal();
	  }
	  else 
	    hebBTDisplay->clear();
	}
	numUpdated++;
      }
      
      else {
	// unrecognized keyword, move along...
      }

    }

    // Did we update any of the panel?  If so, mark the time

    if (numUpdated>0) {
      QDateTime dateTime = QDateTime::currentDateTime().toUTC();
      updateDisplay->setText(dateTime.toString("HH:mm:ss"));
    }

  } // Bottom of the hash iterator
   
}

// Update the control panel

void IUBPanel::update()
{
  // Reset the control states (free stuck controls)

  blueIEBPower->reset();
  redIEBPower->reset();
  blueHEBPower->reset();
  redHEBPower->reset();
  agwGuidePower->reset();
  agwWFSPower->reset();
  llbPower->reset();

  // Clear pressure and temperature readouts

  gsPDisplay->clear();
  gsTDisplay->clear();
  grPDisplay->clear();
  grTDisplay->clear();
  ambTDisplay->clear();
  iubTDisplay->clear();
  agwTDisplay->clear();

  // Queue the IUB and general environmental status queries

  emit statusMsg(tr("Updating %1 utility status...").arg(panelName),Qt::blue);
  sendCmdWait(modsIEHost[modsID],"UTIL STATUS",MODS_QUEUE_REQUEST);
  sendCmdWait(modsIEHost[modsID],"ESTATUS",MODS_QUEUE_REQUEST);
  sendCmdWait(modsBCHost[modsID],"ESTATUS",MODS_QUEUE_REQUEST);
  sendCmdWait(modsRCHost[modsID],"ESTATUS",MODS_QUEUE_REQUEST);

}

// Turn off all switched power outlets.  Implemented as a public slot
// to provide a backdoor emergency power-off function.  Despite the
// "emergency" designation, it actually powers down boxes in sequence

void IUBPanel::allOff()
{
  emit statusMsg(tr("Turning OFF %1 AC power...").arg(panelName),Qt::blue);
  sendCmdWait(modsIEHost[modsID],"UTIL IEB_B OFF",MODS_QUEUE_REQUEST);  // Turn on IEB Blue
  blueIEBPower->setChanging();
  sendCmdWait(modsIEHost[modsID],"UTIL IEB_R OFF",MODS_QUEUE_REQUEST);  // Turn on IEB Red
  redIEBPower->setChanging();
  sendCmdWait(modsIEHost[modsID],"UTIL HEB_B OFF",MODS_QUEUE_REQUEST);  // Turn on the Blue HEB
  blueHEBPower->setChanging();
  sendCmdWait(modsIEHost[modsID],"UTIL HEB_R OFF",MODS_QUEUE_REQUEST);  // Turn on the Red HEB
  redHEBPower->setChanging(); 
  sendCmdWait(modsIEHost[modsID],"UTIL LLB OFF",MODS_QUEUE_REQUEST);  // Turn the lamp/laser box
  llbPower->setChanging();
  sendCmdWait(modsIEHost[modsID],"UTIL AGC OFF",MODS_QUEUE_REQUEST);  // Turn on the AGW Guide Camera (AGC)
  agwGuidePower->setChanging();
  sendCmdWait(modsIEHost[modsID],"UTIL WFS OFF",MODS_QUEUE_REQUEST);  // Turn on the AGW WFS Camera
  agwWFSPower->setChanging();
}

//
// Put the instrument to sleep
//
// Low-power sleep mode is defined as
//   IEBs have the drive power turned off, but interface and thermal management on
//   HEBs powered up for CCD power management and dewar thermal-vac monitors
//   LLB off
//   AGW Guide and WFS cameras off
//
// Eventually this configuration should be loaded from a file, but
// in all likelihood this is so stable it can be safely (if somewhat
// clumsily) hardcoded
//

void IUBPanel::sleep()
{
  emit statusMsg(tr("Putting %1 to Sleep...").arg(panelName),Qt::blue);
  sendCmdWait(modsIEHost[modsID],"IEB B MPOWER OFF",MODS_QUEUE_REQUEST);  // Turn off IEB_B drive & MicroLYNX power
  blueIEBPower->setSleep();             // to put IEB_B into sleep mode
      
  sendCmdWait(modsIEHost[modsID],"IEB R MPOWER OFF",MODS_QUEUE_REQUEST);  // Turn off IEB_R drive & MicroLYNX power
  redIEBPower->setSleep();             // to put IEB_R into sleep mode
      
  blueHEBPower->setSleep();      // Set the HEB switches to sleep, even though this mode
  redHEBPower->setSleep();       // is currently indistinguishable from "on"
    
  // Turn off the LLB, Guider, and WFS outlets off one at a time.

  sendCmdWait(modsIEHost[modsID],"UTIL LLB OFF",MODS_QUEUE_REQUEST);
  llbPower->setSleep();
  sendCmdWait(modsIEHost[modsID],"UTIL AGC OFF",MODS_QUEUE_REQUEST);
  agwGuidePower->setSleep();
  sendCmdWait(modsIEHost[modsID],"UTIL WFS OFF",MODS_QUEUE_REQUEST);
  agwWFSPower->setSleep();
    
  emit statusMsg(tr("Done: %1 going to sleep, press the main power button to wake").arg(panelName),Qt::blue);
}

// Wake up the instrument, either out of full OFF or sleep mode
//
// Full-on mode (awake) is defined as
//   IEBs on with drives powered up
//   HEBs powered up 
//   LLB on, but no lamps on
//   AGW Guide and WFS cameras on and ready
//
// Eventually this configuration should be loaded from a file, but
// in all likelihood this is so stable it can be safely (if somewhat
// clumsily) hardcoded.  What we do depends on the power and sleep state
//

void IUBPanel::wake()
{
  if (!powerOn) { // full instrument start from full off
    emit statusMsg(tr("Turning on %1 AC power...").arg(panelName),Qt::blue);
    sleepMode = false;
    sendCmdWait(modsIEHost[modsID],"UTIL IEB_B ON",MODS_QUEUE_REQUEST);  // Turn on IEB Blue
    blueIEBPower->setChanging();
    sendCmdWait(modsIEHost[modsID],"UTIL IEB_R ON",MODS_QUEUE_REQUEST);  // Turn on IEB Red
    redIEBPower->setChanging();
    sendCmdWait(modsIEHost[modsID],"UTIL HEB_B ON",MODS_QUEUE_REQUEST);  // Turn on the Blue HEB
    blueHEBPower->setChanging();
    sendCmdWait(modsIEHost[modsID],"UTIL HEB_R ON",MODS_QUEUE_REQUEST);  // Turn on the Red HEB
    redHEBPower->setChanging(); 
    sendCmdWait(modsIEHost[modsID],"UTIL LLB ON",MODS_QUEUE_REQUEST);  // Turn the lamp/laser box
    llbPower->setChanging();
    sendCmdWait(modsIEHost[modsID],"UTIL AGC ON",MODS_QUEUE_REQUEST);  // Turn on the AGW Guide Camera (AGC)
    agwGuidePower->setChanging();
    sendCmdWait(modsIEHost[modsID],"UTIL WFS ON",MODS_QUEUE_REQUEST);  // Turn on the AGW WFS Camera
    agwWFSPower->setChanging();
  }
  else { 
    emit statusMsg(tr("Waking %1 from Sleep Mode...").arg(panelName),Qt::blue);

    sendCmdWait(modsIEHost[modsID],"IEB B MPOWER ON",MODS_QUEUE_REQUEST);  // Turn on IEB_B drive & MicroLYNX power
    blueIEBPower->setChanging();
    sendCmdWait(modsIEHost[modsID],"IEB R MPOWER ON",MODS_QUEUE_REQUEST);  // Turn on  IEB_R drive & MicroLYNX power
    redIEBPower->setChanging();

    // Turn on the HEBs, LLB, and AGW cameras in sequence

    sendCmdWait(modsIEHost[modsID],"UTIL HEB_B ON",MODS_QUEUE_REQUEST);
    blueHEBPower->setChanging();

    sendCmdWait(modsIEHost[modsID],"UTIL HEB_R ON",MODS_QUEUE_REQUEST);
    redHEBPower->setChanging();

    sendCmdWait(modsIEHost[modsID],"UTIL LLB ON",MODS_QUEUE_REQUEST);
    llbPower->setChanging();

    sendCmdWait(modsIEHost[modsID],"UTIL AGC ON",MODS_QUEUE_REQUEST);
    agwGuidePower->setChanging();

    sendCmdWait(modsIEHost[modsID],"UTIL WFS ON",MODS_QUEUE_REQUEST);
    agwWFSPower->setChanging();

    sleepMode = false;
  }

}

// Report if the instrument is sleeping

bool IUBPanel::isSleeping()
{
  return sleepMode;
}

// Enable the utility box control panel

void IUBPanel::enable()
{
  setDisabled(false);
}

// Disable the utility box control panel

void IUBPanel::disable()
{
  setDisabled(true);
}

// Lock/Unlock the IUB Control Panel Power Buttons

void IUBPanel::setPowerLock()
{
  if (lockPowerCheck->isChecked()) {
    lockPower = true;
    emit statusMsg(tr("MODS%1 IUB Power Controls are now LOCKED").arg(modsID+1),Qt::blue);
  }
  else {
    QString titleStr = tr("Unlock MODS%1 Utility Box Power Controls").arg(modsID+1);
    QString msgStr = tr("You are attempting to unlock the MODS%1 Utility Box Power Controls.\n\n"
			"This will allow you to power cycle key instrument systems,"
			" which if accidentally powered off could result in loss of data"
			" and loss of many minutes restoring full instrument function.\n"
			"\nAre you really sure you want to do this?\n").arg(modsID+1);
    int ret = QMessageBox::warning(this,titleStr,msgStr,
				   (QMessageBox::Cancel | QMessageBox::Yes));
    switch (ret) {
    case QMessageBox::Yes:
      lockPower = false;
      emit statusMsg(tr("MODS%1 IUB Power Controls are now UNLOCKED.").arg(modsID+1),Qt::blue);
      break;
      
    default:
      lockPower = true;
      break;
    }
  }
  if (lockPower)
    lockPowerCheck->setCheckState(Qt::Checked);
  else
    lockPowerCheck->setCheckState(Qt::Unchecked);

}
