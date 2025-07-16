#include <QtWidgets>
#include <QtNetwork>
#include <math.h>

#include "llbpanel.h"

#include "powerswitch.h"
#include "powerbutton.h"
#include "toggleswitch.h"
#include "lightbutton.h"
#include "textdisplay.h"
#include "numberentry.h"
#include "actionbutton.h"

#include "isisclient.h"

LLBPanel::LLBPanel(const int &myMODS, QWidget *parent)
  : QWidget(parent)
{
  modsID = myMODS;
  
  // Load the configuration file

  QString cfgFile = QString("%1/Config/MODS%2/llb.tab").arg(MODS_BASE_PATH).arg(modsID+1);
  loadConfig(cfgFile);

  // IR IMCS Laser Controls and Readouts

  irLaserSwitch = new PowerSwitch(tr("AC Power"),"IRLASER",64,true,this);
  connect(irLaserSwitch,SIGNAL(cmdReady(const QString &)),
	  this,SLOT(sendCommand(const QString &)));

  // IR Laser controls are laid out on a grid

  QGridLayout *irPowerLayout = new QGridLayout;
  irPowerLayout->setVerticalSpacing(0);

  irBeamSwitch = new ToggleSwitch(tr("Enable"),"IRLASER","ENABLE","DISABLE",this);
  connect(irBeamSwitch,SIGNAL(cmdReady(const QString &)),
	  this,SLOT(enableIRLaser(const QString &)));
  irPowerLayout->addWidget(irBeamSwitch,0,0);

  irBeamPower = 0.0;
  irBeamDisplay = new TextDisplay("Power:","mW",4,this);
  irBeamDisplay->setNormalColors(Qt::black,Qt::green);
  irBeamDisplay->setFaultColors(Qt::black,Qt::red);
  irBeamDisplay->setChangeColors(Qt::black,QColor(255,165,0,255));
  irBeamDisplay->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  irBeamDisplay->clear();
  irPowerLayout->addWidget(irBeamDisplay,0,1);

  irBeamSet = 0.0;
  irBeamSetting = new NumberEntry(" Set:","mW",4,MODS_IR_MINPOWER,MODS_IR_MAXPOWER,1,this);
  connect(irBeamSetting,SIGNAL(dataReady(const double &)),
	  this,SLOT(setIRBeamPower(const double &)));
  irPowerLayout->addWidget(irBeamSetting,0,2);

  // IR Laser reset button

  irResetButton = new ActionButton(tr("Reset"),this);
  irResetButton->setMinimumWidth((int)(irBeamSwitch->minimumSizeHint().width()));
  connect(irResetButton,SIGNAL(clicked()),this,SLOT(resetIRLaser()));
  irPowerLayout->addWidget(irResetButton,1,0);

  // IR Laser Preset Checkbox

  irPresetCheck = new QCheckBox(tr("Use Preset Power (%1mW)").arg(irBeamPreset),this);
  if (useIRPreset)
    irPresetCheck->setCheckState(Qt::Checked);
  else
    irPresetCheck->setCheckState(Qt::Unchecked);
  connect(irPresetCheck,SIGNAL(stateChanged(int)),this,SLOT(setIRPreset()));
  irPresetCheck->setToolTip(tr("Checking this will enable/disable using the IR preset power of %1mW whenever the laser is powered on or enabled.").arg(irBeamPreset));
  irPowerLayout->addWidget(irPresetCheck,1,1,1,2);
  
  // IR Laser update button

  irUpdateButton = new ActionButton(tr("Update"),this);
  irUpdateButton->setMinimumWidth((int)(irBeamSwitch->minimumSizeHint().width()));
  connect(irUpdateButton,SIGNAL(clicked()),this,SLOT(updateIRLaser()));
  irPowerLayout->addWidget(irUpdateButton,2,0);

  // IR Laser Head Operating and Set-Point Temperature Displays

  irTempSet = 0.0;
  irTempSetDisp = new TextDisplay(tr("Temp Set:"),"C",4,this);
  irTempSetDisp->setNormalColors(Qt::black,Qt::green);
  irTempSetDisp->setFaultColors(Qt::black,Qt::red);
  irTempSetDisp->setChangeColors(Qt::black,QColor(255,165,0,255));
  irTempSetDisp->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  irTempSetDisp->clear();
  irPowerLayout->addWidget(irTempSetDisp,2,1);

  irLaserTemp = 0.0;
  irTempActDisp = new TextDisplay(tr(" Actual:"),"C",4,this);
  irTempActDisp->setNormalColors(Qt::black,Qt::green);
  irTempActDisp->setFaultColors(Qt::black,Qt::red);
  irTempActDisp->setChangeColors(Qt::black,QColor(255,165,0,255));
  irTempActDisp->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  irTempActDisp->clear();
  irPowerLayout->addWidget(irTempActDisp,2,2);

  // Layout the controls horizontally

  QVBoxLayout *irLayout = new QVBoxLayout;
  irLayout->addLayout(irPowerLayout);
  irLayout->addStretch();

  QHBoxLayout *irLaserLayout = new QHBoxLayout;
  irLaserLayout->addWidget(irLaserSwitch);
  irLaserLayout->addLayout(irLayout);
  // irLaserLayout->addStretch();

  // Wrap this in a QGroupBox layout

  QGroupBox *irGroupBox = new QGroupBox(tr("IMCS IR Laser"));
  irGroupBox->setLayout(irLaserLayout);

  // Visible Laser controls and readouts

  visLaserSwitch = new PowerSwitch(tr("AC Power"),"VISLASER",64,true,this);
  connect(visLaserSwitch,SIGNAL(cmdReady(const QString &)),
	  this,SLOT(sendCommand(const QString &)));

  // Visible Laser controls are laid out on a grid

  QGridLayout *visLayout = new QGridLayout;

  visBeamSwitch = new ToggleSwitch(tr("Enable"),"VISLASER","ENABLE","DISABLE",this);
  connect(visBeamSwitch,SIGNAL(cmdReady(const QString &)),
	  this,SLOT(enableVisLaser(const QString &)));
  visLayout->addWidget(visBeamSwitch,0,0);

  // Power Readout and Setting Widget

  visBeamPower = 0.0;
  visBeamDisplay = new TextDisplay(" Power:","mW",4,this);
  visBeamDisplay->setNormalColors(Qt::black,Qt::green);
  visBeamDisplay->setFaultColors(Qt::black,Qt::red);
  visBeamDisplay->setChangeColors(Qt::black,QColor(255,165,0,255));
  visBeamDisplay->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  visBeamDisplay->clear();
  visLayout->addWidget(visBeamDisplay,0,1);

  visBeamSet = 0.0;
  visBeamSetting = new NumberEntry(" Set:","mW",4,MODS_VIS_MINPOWER,MODS_VIS_MAXPOWER,1,this);
  connect(visBeamSetting,SIGNAL(dataReady(const double &)),
	  this,SLOT(setVisBeamPower(const double &)));
  visLayout->addWidget(visBeamSetting,0,2);

  // Reset button

  visResetButton = new ActionButton(tr("Reset"),this);
  visResetButton->setMinimumWidth((int)(visBeamSwitch->minimumSizeHint().width()));
  connect(visResetButton,SIGNAL(clicked()),this,SLOT(resetVisLaser()));
  visLayout->addWidget(visResetButton,1,0);

  // Visible Laser Preset Checkbox

  visPresetCheck = new QCheckBox(tr("Use Preset Power (%1mW)").arg(visBeamPreset),this);
  if (useVisPreset)
    visPresetCheck->setCheckState(Qt::Checked);
  else
    visPresetCheck->setCheckState(Qt::Unchecked);
  connect(visPresetCheck,SIGNAL(stateChanged(int)),this,SLOT(setVisPreset()));
  visPresetCheck->setToolTip(tr("Checking this will enable/disable using the preset power of %1mW whenever the laser is powered on or enabled.").arg(visBeamPreset));
  visLayout->addWidget(visPresetCheck,1,1,1,2);
  
  // Visible Laser update button

  visUpdateButton = new ActionButton(tr("Update"),this);
  visUpdateButton->setMinimumWidth((int)(visBeamSwitch->minimumSizeHint().width()));
  connect(visUpdateButton,SIGNAL(clicked()),this,SLOT(updateVisLaser()));
  visLayout->addWidget(visUpdateButton,2,0);

  // Layout the controls horizontally

  QVBoxLayout *visPowerLayout = new QVBoxLayout;
  visPowerLayout->setSpacing(1);
  visPowerLayout->addLayout(visLayout);
  visPowerLayout->addStretch();

  QHBoxLayout *visLaserLayout = new QHBoxLayout;
  visLaserLayout->addWidget(visLaserSwitch);
  visLaserLayout->addLayout(visPowerLayout);

  // Wrap this in a QGroupBox layout

  QGroupBox *visGroupBox = new QGroupBox(tr("Visible-Light Laser"));
  visGroupBox->setLayout(visLaserLayout);

  // Calibration Lamp Controls - one long row of buttons

  QHBoxLayout *lampLayout = new QHBoxLayout;
  lampLayout->setContentsMargins(1,1,1,1);
  lampLayout->setSpacing(2);

  neLamp = new ToggleSwitch(tr("Ne"),"LAMP Ne","ON","OFF",this);
  connect(neLamp,SIGNAL(cmdReady(const QString &)),
	  this,SLOT(sendCommand(const QString &)));
  lampLayout->addWidget(neLamp);
  int lbw = 0;
  int bws = (int)(neLamp->minimumSizeHint().width());
  if (bws > lbw) lbw = bws;
  neLamp->setFont(QFont(MODS_BASE_FONT,MODS_BASE_FONTSIZE-2,QFont::Normal));

  hgLamp = new ToggleSwitch(tr("Hg"),"LAMP Hg","ON","OFF",this);
  connect(hgLamp,SIGNAL(cmdReady(const QString &)),
	  this,SLOT(sendCommand(const QString &)));
  lampLayout->addWidget(hgLamp);
  bws = (int)(hgLamp->minimumSizeHint().width());
  if (bws > lbw) lbw = bws;
  hgLamp->setFont(QFont(MODS_BASE_FONT,MODS_BASE_FONTSIZE-2,QFont::Normal));

  arLamp = new ToggleSwitch(tr("Ar"),"LAMP Ar","ON","OFF",this);
  connect(arLamp,SIGNAL(cmdReady(const QString &)),
	  this,SLOT(sendCommand(const QString &)));
  lampLayout->addWidget(arLamp);
  bws = (int)(arLamp->minimumSizeHint().width());
  if (bws > lbw) lbw = bws;
  arLamp->setFont(QFont(MODS_BASE_FONT,MODS_BASE_FONTSIZE-2,QFont::Normal));

  xeLamp = new ToggleSwitch(tr("Xe"),"LAMP Xe","ON","OFF",this);
  connect(xeLamp,SIGNAL(cmdReady(const QString &)),
	  this,SLOT(sendCommand(const QString &)));
  lampLayout->addWidget(xeLamp);
  bws = (int)(xeLamp->minimumSizeHint().width());
  if (bws > lbw) lbw = bws;
  xeLamp->setFont(QFont(MODS_BASE_FONT,MODS_BASE_FONTSIZE-2,QFont::Normal));

  krLamp = new ToggleSwitch(tr("Kr"),"LAMP Kr","ON","OFF",this);
  connect(krLamp,SIGNAL(cmdReady(const QString &)),
	  this,SLOT(sendCommand(const QString &)));
  lampLayout->addWidget(krLamp);
  bws = (int)(krLamp->minimumSizeHint().width());
  if (bws > lbw) lbw = bws;
  krLamp->setFont(QFont(MODS_BASE_FONT,MODS_BASE_FONTSIZE-2,QFont::Normal));

  // QTH lamps 1 and 2

  for (int i=0;i<3;i++) {
    int nQTH = i+1;
    if (i<2) 
       qthLamp[i] = new ToggleSwitch(tr("QTH%1").arg(nQTH),
   				     QString("LAMP QTH%1").arg(nQTH),
				     "ON","OFF",this);
    else
       qthLamp[i] = new ToggleSwitch(tr("VFLAT"),
   				     QString("LAMP VFLAT"),
				     "ON","OFF",this);
       
    connect(qthLamp[i],SIGNAL(cmdReady(const QString &)),
	    this,SLOT(sendCommand(const QString &)));
    lampLayout->addWidget(qthLamp[i]);
    bws = (int)(qthLamp[i]->minimumSizeHint().width());
    if (bws > lbw) lbw = bws;
    qthLamp[i]->setFont(QFont(MODS_BASE_FONT,MODS_BASE_FONTSIZE-2,QFont::Normal));
  }

  qthSupply = new ToggleSwitch(tr("QTH 6V"),"LAMP QTH6V","ON","OFF",this);
  connect(qthSupply,SIGNAL(cmdReady(const QString &)),
	  this,SLOT(sendCommand(const QString &)));
  lampLayout->addWidget(qthSupply);
  bws = (int)(qthSupply->minimumSizeHint().width());
  if (bws > lbw) lbw = bws;
  qthSupply->setFont(QFont(MODS_BASE_FONT,MODS_BASE_FONTSIZE-2,QFont::Normal));

  allOff = new ActionButton(tr("OFF"),this);
  connect(allOff,SIGNAL(clicked()),this,SLOT(allLampsOff()));
  lampLayout->addWidget(allOff);
  bws = (int)(allOff->minimumSizeHint().width());
  if (bws > lbw) lbw = bws;
  allOff->setFont(QFont(MODS_BASE_FONT,MODS_BASE_FONTSIZE-2,QFont::Normal));

  // Equalize the calibration lamp button widths

  neLamp->setMinimumWidth(lbw);
  hgLamp->setMinimumWidth(lbw);
  arLamp->setMinimumWidth(lbw);
  xeLamp->setMinimumWidth(lbw);
  krLamp->setMinimumWidth(lbw);
  for (int i=0;i<3;i++) qthLamp[i]->setMinimumWidth(lbw);
  qthSupply->setMinimumWidth(lbw);
  allOff->setMinimumWidth(lbw);

  // Wrap in a QGroupBox

  QGroupBox *lampGroupBox = new QGroupBox(tr("Calibration Lamps"));
  lampGroupBox->setLayout(lampLayout);

  // Put it all together

  QHBoxLayout *laserLayout = new QHBoxLayout;
  laserLayout->addWidget(irGroupBox);
  laserLayout->addWidget(visGroupBox);

  QVBoxLayout *controlLayout = new QVBoxLayout;
  controlLayout->addLayout(laserLayout);
  controlLayout->addWidget(lampGroupBox);

  // Wrap in the main group box

  QGroupBox *mainGroupBox = new QGroupBox(tr("MODS%1 Lamp/Laser Box").arg(modsID+1));
  mainGroupBox->setContentsMargins(3,10,3,3);
  mainGroupBox->setLayout(controlLayout);

  // ... and build the final layout.

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(mainGroupBox);
  mainLayout->setContentsMargins(0,0,0,0);
  setLayout(mainLayout);

  // Other stuff?

}

// Pass the command string out to the parent control panel 
// (they know who should get it)

void LLBPanel::sendCommand(const QString &cmd)
{
  if (cmd.isEmpty()) return;
  emit statusMsg(cmd,Qt::blue);
  emit cmdReady(modsIEHost[modsID],cmd);
}

// Pass the command string out to the parent control panel and wait
// for completion or timeout (they know who should get it)

void LLBPanel::sendCmdWait(const QString &cmd, const int &timeout)
{
  if (cmd.isEmpty()) return;
  if (timeout==0)
    emit cmdReady(modsIEHost[modsID],cmd);
  else
    emit cmdWait(modsIEHost[modsID],cmd,timeout);
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

void LLBPanel::parse(const QString &cmdStr, 
		     const int &msgType, 
		     const QHash<QString,QString> &keyDict)
{
  // TODO: Unused flags. Commented out on 2-19-24
  // bool cmdDone = false;   // Flag: message is a command completion
  // bool cmdFault = false;  // Flag: command completed with a fault
  bool ok;

  QString msgStr = cmdStr;
  msgStr = msgStr.simplified(); // TODO: Evaluate this line. Why is it here? 

  if (msgStr.isEmpty()) return; // Empty messages are ignored
  
  // TODO: Unused flags. Commented out on 2-19-24
  // Our response depends on the type of message we received. 
  switch (msgType) {
  case ISISClient::ERROR:
    // cmdDone = true;
    // cmdFault = true;
    break;

  case ISISClient::WARNING:
    // cmdDone = true;
    // cmdFault = false;
    break;

  case ISISClient::FATAL:
    // cmdDone = true;
    // cmdFault = true;
    break;

  case ISISClient::DONE:
    // cmdDone = true;
    // cmdFault = false;
    break;

  case ISISClient::STATUS:
    // cmdDone = false;
    // cmdFault = false;
    break;

  default:
    // Nothing to be done here, return (we don't handle REQ: or EXEC: requests)
    return;
    break;
  }

  // The first word in the message body is the command that generated
  // this response.

  QString cmdWord = msgStr.section(" ",0,0);

  // Were we passed any keyword=value pairs to interpret?

  int numKeys = keyDict.size();

  if (numKeys > 0) { // use Qt's built-in Java-style iterator to traverse the hash table
    QHashIterator<QString,QString> i(keyDict);
    while (i.hasNext()) {
      i.next();
      QString keyStr = i.key();
      QString valStr = i.value();

      // Special Case if LLB=OFF, means everything is off...

      if (keyStr.compare("LLB",Qt::CaseInsensitive)==0) {
	if (valStr.compare("OFF",Qt::CaseInsensitive)==0 ||
	    valStr.compare("PWROFF",Qt::CaseInsensitive)==0) {
	  irLaserSwitch->setOff();
	  irBeamSwitch->setOff();
	  irBeamDisplay->clear();
	  irBeamDisplay->setNormal();
	  irTempActDisp->clear();
	  irTempActDisp->setNormal();
	  visLaserSwitch->setOff();
	  visBeamDisplay->clear();
	  visBeamDisplay->setNormal();
	  neLamp->setOff();
	  hgLamp->setOff();
	  arLamp->setOff();
	  xeLamp->setOff();
	  krLamp->setOff();
	  for (int i=0;i<3;i++) 
	    qthLamp[i]->setOff();
	  qthSupply->setOff();
	}
      }

      // IR Laser AC Main Power
      
      else if (keyStr.compare("IRLASER",Qt::CaseInsensitive)==0) {
	if (valStr.compare("ON",Qt::CaseInsensitive)==0)
	  irLaserSwitch->setOn();
	else if (valStr.compare("OFF",Qt::CaseInsensitive)==0)
	  irLaserSwitch->setOff();
	else
	  irLaserSwitch->setFault();
      }
      
      // IR Laser Beam Enable/Disable
      
      else if (keyStr.compare("IRBEAM",Qt::CaseInsensitive)==0) {
	if (valStr.compare("ENABLED",Qt::CaseInsensitive)==0)
	  irBeamSwitch->setOn();
	else if (valStr.compare("DISABLED",Qt::CaseInsensitive)==0)
	  irBeamSwitch->setOff();
	else
	  irBeamSwitch->setFault();
      }
      
      // IR Power output and setpoint
      
      else if (keyStr.compare("IRPOUT",Qt::CaseInsensitive)==0) {
	double dtmp = valStr.toDouble(&ok);
	if (ok) {
	  if (dtmp >= MODS_IR_MINPOWER && dtmp <= MODS_IR_MAXPOWER) {
	    irBeamPower = dtmp;
	    irBeamDisplay->setText(QString::number(irBeamPower,'f',2),Qt::green);
	    irBeamDisplay->setNormal();
	  }
	  else if (dtmp > MODS_IR_MAXPOWER) {
	    irBeamPower = MODS_IR_MAXPOWER;
	    irBeamDisplay->setText(QString::number(irBeamPower,'f',2),Qt::green);
	    irBeamDisplay->setFault();
	  }
	  else 
	    irBeamDisplay->clear();
	}
      }
      
      else if (keyStr.compare("IRPSET",Qt::CaseInsensitive)==0) {
	double dtmp = valStr.toDouble(&ok);
	if (ok && (dtmp >= MODS_IR_MINPOWER && dtmp <= MODS_IR_MAXPOWER)) {
	  irBeamSet = dtmp;
	  irBeamSetting->setValue(irBeamSet);
	  irBeamSetting->setNormal();
	}
	else
	  irBeamSetting->setFault();
      }
      
      // IR Laser Operating Temperature and Temperature Set Point
      
      else if (keyStr.compare("IRTEMP",Qt::CaseInsensitive)==0) {
	double dtmp = valStr.toDouble(&ok);
	if (ok && (dtmp >= MODS_IR_MINTEMP && dtmp <= MODS_IR_MAXTEMP)) {
	  irLaserTemp = dtmp;
	  irTempActDisp->setText(QString::number(irLaserTemp,'f',1),Qt::green);
	  irTempActDisp->setNormal();
	}
	else {
	  irTempActDisp->clear();
	  irTempActDisp->setFault();
	}
      }
      
      else if (keyStr.compare("IRTSET",Qt::CaseInsensitive)==0) {
	double dtmp = valStr.toDouble(&ok);
	if (ok && (dtmp >= MODS_IR_MINTEMP && dtmp <= MODS_IR_MAXTEMP)) {
	  irTempSet = dtmp;
	  irTempSetDisp->setText(QString::number(irTempSet,'f',1),Qt::green);
	  irTempSetDisp->setNormal();
	}
	else {
	  irTempSetDisp->clear();
	  irTempSetDisp->setFault();
	}
      }
      
      // Visible Laser AC Main Power

       else if (keyStr.compare("VISLASER",Qt::CaseInsensitive)==0) {
	if (valStr.compare("ON",Qt::CaseInsensitive)==0)
	  visLaserSwitch->setOn();
	else if (valStr.compare("OFF",Qt::CaseInsensitive)==0)
	  visLaserSwitch->setOff();
	else
	  visLaserSwitch->setFault();
      }

      // Visible Laser Beam Enable/Disable

      else if (keyStr.compare("VISBEAM",Qt::CaseInsensitive)==0) {
	if (valStr.compare("ENABLED",Qt::CaseInsensitive)==0)
	  visBeamSwitch->setOn();
	else if (valStr.compare("DISABLED",Qt::CaseInsensitive)==0)
	  visBeamSwitch->setOff();
	else
	  visBeamSwitch->setFault();
      }

      // VIS Power output and setpoint

      else if (keyStr.compare("VISPOUT",Qt::CaseInsensitive)==0) {
	double dtmp = valStr.toDouble(&ok);
	if (ok) {
	  if (dtmp >= MODS_VIS_MINPOWER && dtmp <= MODS_VIS_MAXPOWER) {
	    visBeamPower = dtmp;
	    visBeamDisplay->setText(QString::number(visBeamPower,'f',2),Qt::green);
	    visBeamDisplay->setNormal();
	  }
	  else if (dtmp > MODS_VIS_MAXPOWER) {
	    visBeamPower = MODS_VIS_MAXPOWER;
	    visBeamDisplay->setText(QString::number(visBeamPower,'f',2),Qt::green);
	    visBeamDisplay->setFault();
	  }
	  else 
	    visBeamDisplay->clear();
	}
      }

      else if (keyStr.compare("VISPSET",Qt::CaseInsensitive)==0) {
	double dtmp = valStr.toDouble(&ok);
	if (ok && (dtmp >= MODS_VIS_MINPOWER && dtmp <= MODS_VIS_MAXPOWER)) {
	  visBeamSet = dtmp;
	  visBeamSetting->setValue(visBeamSet);
	  visBeamSetting->setNormal();
	}
	else
	  visBeamSetting->setFault();
      }

      // Calibration Lamps - lets us process the CALLAMPS keyword

      else if (keyStr.compare("CALLAMPS",Qt::CaseInsensitive)==0) {
	if (valStr.contains("NE",Qt::CaseInsensitive)) 
	  neLamp->setOn();
	else 
	  neLamp->setOff();
	if (valStr.contains("HG",Qt::CaseInsensitive))
	  hgLamp->setOn();
	else
	  hgLamp->setOff();
	if (valStr.contains("AR",Qt::CaseInsensitive))
	  arLamp->setOn();
	else
	  arLamp->setOff();
	if (valStr.contains("XE",Qt::CaseInsensitive))
	  xeLamp->setOn();
	else
	  xeLamp->setOff();
	if (valStr.contains("KR",Qt::CaseInsensitive))
	  krLamp->setOn();
	else
	  krLamp->setOff();
	if (valStr.contains("FLAT1",Qt::CaseInsensitive) ||
            valStr.contains("QTH1",Qt::CaseInsensitive))
	  qthLamp[0]->setOn();
	else
	  qthLamp[0]->setOff();
	if (valStr.contains("FLAT2",Qt::CaseInsensitive) ||
            valStr.contains("QTH2",Qt::CaseInsensitive))
	  qthLamp[1]->setOn();
	else
	  qthLamp[1]->setOff();
	if (valStr.contains("VFLAT",Qt::CaseInsensitive))
	  qthLamp[2]->setOn();
	else
	  qthLamp[2]->setOff();
	if (valStr.contains("QTH6V",Qt::CaseInsensitive))
	  qthSupply->setOn();
	else
	  qthSupply->setOff();
	if (valStr.compare("NONE",Qt::CaseInsensitive)==0 ||
	    valStr.compare("OFF",Qt::CaseInsensitive)==0) {
	  neLamp->setOff();
	  hgLamp->setOff();
	  arLamp->setOff();
	  xeLamp->setOff();
	  krLamp->setOff();
	  for (int i=0;i<2;i++)
	    qthLamp[i]->setOff();
	  qthSupply->setOff();
	}	
      }

      else if (keyStr.compare("QTH6V",Qt::CaseInsensitive)==0) {
	if (valStr.compare("ON",Qt::CaseInsensitive)==0) 
	  qthSupply->setOn();
	else
	  qthSupply->setOff();
      }

      else {
	// unrecognized keyword, move along...
      }
    }
  }

  // If commands require additional handling, especially for faults,
  // do it here...

}

// Update the control panel

void LLBPanel::update()
{
  // Reset readouts and controls

  irLaserSwitch->setUnknown();
  irBeamSwitch->setUnknown();
  irBeamDisplay->clear();
  irBeamDisplay->setNormal();
  irBeamSetting->clear();
  irTempActDisp->clear();
  irTempActDisp->setNormal();
  irTempSetDisp->clear();
  irTempSetDisp->setNormal();

  visLaserSwitch->setUnknown();
  visBeamSwitch->setUnknown();
  visBeamDisplay->clear();
  visBeamDisplay->setNormal();
  visBeamSetting->clear();

  neLamp->setUnknown();
  hgLamp->setUnknown();
  arLamp->setUnknown();
  xeLamp->setUnknown();
  krLamp->setUnknown();
  for (int i=0;i<3;i++)
    qthLamp[i]->setUnknown();
  qthSupply->setUnknown();

  // Request updated status info

  // sendCmdWait("LLB STATUS",MODS_QUEUE_REQUEST);

  // Aliases until LLB STATUS enabled

  sendCmdWait("LAMP",MODS_QUEUE_REQUEST);
  sendCmdWait("VISLASER",MODS_QUEUE_REQUEST);
  sendCmdWait("IRLASER",MODS_QUEUE_REQUEST);
	      
}

// Turn off all calibration lamps

void LLBPanel::allLampsOff()
{
  sendCommand("LAMP OFF");
}

// Set the IR Laser Head Temperature set point

void LLBPanel::setIRTemp(const double &newTemp)
{
  QString cmdStr = QString("IRLASER TEMP %1").arg(newTemp);
  sendCommand(cmdStr);
}

// Set the VIS Laser Beam Power

void LLBPanel::setVisBeamPower(const double &newPower)
{
  QString cmdStr = QString("VISLASER POWER %1").arg(newPower);
  sendCommand(cmdStr);
}

// Set the IR Laser Beam Power

void LLBPanel::setIRBeamPower(const double &newPower)
{
  QString cmdStr = QString("IRLASER POWER %1").arg(newPower);
  sendCommand(cmdStr);
}

// Reset the IR Laser System

void LLBPanel::resetIRLaser()
{
  sendCommand("IRLASER RESET");
}

// Reset the Visible Laser System

void LLBPanel::resetVisLaser()
{
  sendCommand("VISLASER RESET");
}

// Update IR Laser Status

void LLBPanel::updateIRLaser()
{
  sendCommand("IRLASER");
}

// Update Visible Laser Status

void LLBPanel::updateVisLaser()
{
  sendCommand("VISLASER");
}

// Enable the IR Laser Beam

void LLBPanel::enableIRLaser(const QString &cmd)
{
  if (cmd.isEmpty()) return;
  QString cmdStr = cmd;
  if (cmdStr.contains("ENABLE",Qt::CaseInsensitive)) {
    if (useIRPreset) cmdStr.append(QString(" %1").arg(irBeamPreset));
  }
  sendCommand(cmdStr);
  // Start a one-shot timer that 7 seconds from now will request update status
  QTimer::singleShot(7000,this,SLOT(updateIRLaser()));
  irBeamDisplay->setChange();
}

// Enable the Visible Laser Beam

void LLBPanel::enableVisLaser(const QString &cmd)
{
  if (cmd.isEmpty()) return;
  QString cmdStr = cmd;
  if (cmdStr.contains("ENABLE",Qt::CaseInsensitive)) {
    if (useVisPreset) cmdStr.append(QString(" %1").arg(visBeamPreset));
  }
  sendCommand(cmdStr);
  // Start a one-shot timer that 7 seconds from now will request update status
  QTimer::singleShot(7000,this,SLOT(updateVisLaser()));
  visBeamDisplay->setChange();
}

// Load and parse an IEB configuration fe

void LLBPanel::loadConfig(const QString &cfgFile)
{
  bool ok;

  // Set the runtime default values

  irBeamPreset  = MODS_IR_PRESET;
  useIRPreset = false;
  visBeamPreset = MODS_VIS_PRESET;
  useVisPreset = false;

  if (cfgFile.isEmpty()) return;  // null config file means "use defaults"

  QFile inFile(cfgFile);
  if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QString errMsg = tr("Could not open the MODS%1 LLB configuration file %2!\n"
			"Reason:\n %3\nSystem defaults loaded instead.").arg(modsID+1).arg(cfgFile).arg(inFile.errorString());
    QMessageBox::warning(this,tr("LLB Configuration File Load Error"),errMsg); 
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

	// We have a keyword, parse it.  On any errors, use the default

	if (!itemStr.isEmpty()) {
	  QString keyWord = itemStr.section(' ',0,0);
	  
	  // IR Laser Preset Power in mW

	  if (keyWord.compare("IRPRESET",Qt::CaseInsensitive)==0) {
	    double power = itemStr.section(' ',1,1).toDouble(&ok);
	    if (ok && (power >= MODS_IR_MINPOWER && power <= MODS_IR_MAXPOWER)) {
	      irBeamPreset = power;
	      useIRPreset = true;
	    }
	    else {
	      irBeamPreset = MODS_IR_PRESET;
	      useIRPreset = false;
	    }
	  }

	  // Visible Laser Preset Power in mW

	  else if (keyWord.compare("VISPRESET",Qt::CaseInsensitive)==0) {
	    double power = itemStr.section(' ',1,1).toDouble(&ok);
	    if (ok && (power >= MODS_VIS_MINPOWER && power <= MODS_VIS_MAXPOWER)) {
	      visBeamPreset = power;
	      useVisPreset = true;
	    }
	    else {
	      visBeamPreset = MODS_VIS_PRESET;
	      useVisPreset = false;
	    }
	  }

	}
      }
    }
  } while (!line.isNull());
  inFile.close();

}

// Enable/Disable the IR Laser Preset Power Function via the checkbox

void LLBPanel::setIRPreset()
{
  if (irPresetCheck->isChecked())
    useIRPreset = true;
  else
    useIRPreset = false;
}

// Enable/Disable the Visible Laser Preset Power Function via the checkbox

void LLBPanel::setVisPreset()
{
  if (visPresetCheck->isChecked())
    useVisPreset = true;
  else
    useVisPreset = false;
}

