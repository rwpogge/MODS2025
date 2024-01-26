#include <QtGui>
#include <QtNetwork>
#include <math.h>

#include "focalplane.h"

#include "bistatemechanism.h"
#include "indexedmechanism.h"
#include "linearmechanism.h"

#include "bistatecontrol.h"
#include "indexedselector.h"
#include "linearcontrol.h"
#include "linearsetting.h"
#include "toggleswitch.h"
#include "textdisplay.h"

#include "selectentry.h"
#include "numberentry.h"
#include "lightbutton.h"
#include "actionbutton.h"

#include "isisclient.h"  // needed for msgType codes

FocalPlane::FocalPlane(const int &myMODS, QWidget *parent)
  : QWidget(parent)
{
  modsID = myMODS;

  //----------------------------------------------------------------
  //
  // Build the Focal Plane Control Panel GUI
  //

  // Hatch and Calibration tower controls in a grid layout

  QFormLayout *fpTopFormLayout = new QFormLayout;
  fpTopFormLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
  fpTopFormLayout->setFormAlignment(Qt::AlignCenter);  
  fpTopFormLayout->setLabelAlignment(Qt::AlignRight);

  // Dark Hatch

  hatch = new BiStateMechanism(tr("Instrument Dark Hatch"),"HATCH","OPEN","CLOSE",30);
  hatchControl = new BiStateControl(hatch,"","Open","Closed",this);
  connect(hatchControl,SIGNAL(cmdReady(const QString &)),
	  this,SLOT(sendCommand(const QString &)));
  fpTopFormLayout->addRow(tr("Hatch:"),hatchControl);

  // Calibration Tower

  calib = new BiStateMechanism(tr("Calibration Tower"),"CALIB","IN","OUT",40);
  calibControl = new BiStateControl(calib,"","In","Out",this);
  connect(calibControl,SIGNAL(cmdReady(const QString &)),
	  this,SLOT(sendCommand(const QString &)));
  fpTopFormLayout->addRow(tr("Calib:"),calibControl);

  // Top-End configuration commands: obsmode, calmode, and gprobe restore checkbox

  QHBoxLayout *modeLayout = new QHBoxLayout;

  QLabel *modeLabel = new QLabel(tr("Mode:"));
  modeLayout->addWidget(modeLabel);

  calModeButton = new ActionButton(tr("Calibration"),this);
  connect(calModeButton,SIGNAL(clicked()),this,SLOT(setCalMode()));
  calModeButton->setToolTip(tr("Configure the instrument for internal calibration data acqusition"));
  modeLayout->addWidget(calModeButton);

  obsModeButton = new ActionButton(tr("Observing"),this);
  connect(obsModeButton,SIGNAL(clicked()),this,SLOT(setObsMode()));
  obsModeButton->setToolTip(tr("Configure the instrument for on-sky observing"));
  modeLayout->addWidget(obsModeButton);

  int mbw = qMax((int)(calModeButton->minimumSizeHint().width()),
		 (int)(obsModeButton->minimumSizeHint().width())); // make the same width
  calModeButton->setMinimumWidth(mbw);
  obsModeButton->setMinimumWidth(mbw);

  gpRestoreCheck = new QCheckBox(tr("Restore AGw Probe"),this);
  gpRestore = false;
  if (gpRestore)
    gpRestoreCheck->setCheckState(Qt::Checked);
  else
    gpRestoreCheck->setCheckState(Qt::Unchecked);
  connect(gpRestoreCheck,SIGNAL(stateChanged(int)),this,SLOT(setRestoreGP()));
  gpRestoreCheck->setToolTip(tr("Checking this will restore the Guide Probe XY on switching back to OBSMODE"));
  modeLayout->addWidget(gpRestoreCheck);
  modeLayout->addStretch();

  // AGw Stage Guide Probe X, Y and Focus displays, filter select, and Stage Home buttons

  QHBoxLayout *gprobeLayout = new QHBoxLayout;

  double gpxMin = MODS_GPX_MIN/LBT_DGF_SCALE;   // Guide Probe Xmin in arcsec
  double gpxMax = MODS_GPX_MAX/LBT_DGF_SCALE;   // Guide Probe Xmax in arcsec
  gprobeX = new LinearMechanism(tr("Guide Probe X Position"),"GPROBE",
				gpxMin,gpxMax,1,60);
  gpXDisplay = new TextDisplay("AGw X:","",8,this);
  gpXDisplay->setNormalColors(palette().color(QPalette::Window),Qt::blue);
  gprobeLayout->addWidget(gpXDisplay);
  gprobeLayout->addStretch();

  double gpyMin = MODS_GPY_MIN/LBT_DGF_SCALE;   // Guide Probe Xmin in arcsec
  double gpyMax = MODS_GPY_MAX/LBT_DGF_SCALE;   // Guide Probe Xmax in arcsec
  gprobeY = new LinearMechanism(tr("Guide Probe Y Position"),"GPROBE",
				gpyMin,gpyMax,1,60);
  gpYDisplay = new TextDisplay("Y:","",8,this);
  gpYDisplay->setNormalColors(palette().color(QPalette::Window),Qt::blue);
  gprobeLayout->addWidget(gpYDisplay);
  gprobeLayout->addStretch();

  gpFocus = new LinearMechanism(tr("Guide Probe Focus"),"GPFOCUS",
				MODS_GPFOC_MIN,MODS_GPFOC_MAX,3,30);
  gpFocDisplay = new TextDisplay("Foc:","",6,this);
  gpFocDisplay->setNormalColors(palette().color(QPalette::Window),Qt::blue);
  gprobeLayout->addWidget(gpFocDisplay);
  gprobeLayout->addStretch();

  // Guide Camera filter wheel selector

  agwFilter = new IndexedMechanism(tr("AGw Filter Wheel"),"AGWFILT",
				   MODS_AGWFILT_MIN,
				   MODS_AGWFILT_MAX,30);
  agwFiltControl = new IndexedSelector(agwFilter,"Filter:",this);
  connect(agwFiltControl,SIGNAL(cmdReady(const QString &)),
          this,SLOT(sendCommand(const QString &)));
  gprobeLayout->addWidget(agwFiltControl);
  gprobeLayout->addStretch();

  // AGw Stage home button

  agwHomeButton = new ActionButton("Home",this);
  connect(agwHomeButton,SIGNAL(clicked()),this,SLOT(homeGuideProbe()));
  gprobeLayout->addWidget(agwHomeButton);

  // Calibration Lamp Toggle Switches are in a QGroupBox with their
  // controls

  // First Row of Lamps are the 5 PenRay emission-line calibration
  // lamps

  QHBoxLayout *lampLayout1 = new QHBoxLayout;
  lampLayout1->setContentsMargins(0,0,0,0);

  neLamp = new ToggleSwitch(tr("Ne"),"LAMP NE","ON","OFF",this);
  connect(neLamp,SIGNAL(cmdReady(const QString &)),
	  this,SLOT(setLamp(const QString &)));
  lampLayout1->addWidget(neLamp);

  hgLamp = new ToggleSwitch(tr("Hg"),"LAMP HG","ON","OFF",this);
  connect(hgLamp,SIGNAL(cmdReady(const QString &)),
	  this,SLOT(setLamp(const QString &)));
  lampLayout1->addWidget(hgLamp);

  arLamp = new ToggleSwitch(tr("Ar"),"LAMP AR","ON","OFF",this);
  connect(arLamp,SIGNAL(cmdReady(const QString &)),
	  this,SLOT(setLamp(const QString &)));
  lampLayout1->addWidget(arLamp);

  xeLamp = new ToggleSwitch(tr("Xe"),"LAMP XE","ON","OFF",this);
  connect(xeLamp,SIGNAL(cmdReady(const QString &)),
	  this,SLOT(setLamp(const QString &)));
  lampLayout1->addWidget(xeLamp);

  krLamp = new ToggleSwitch(tr("Kr"),"LAMP KR","ON","OFF",this);
  connect(krLamp,SIGNAL(cmdReady(const QString &)),
	  this,SLOT(setLamp(const QString &)));
  lampLayout1->addWidget(krLamp);

  lampLayout1->setSpacing(2);

  int bws = (int)(hgLamp->minimumSizeHint().width());
  neLamp->setMinimumWidth(bws);
  hgLamp->setMinimumWidth(bws);
  arLamp->setMinimumWidth(bws);
  xeLamp->setMinimumWidth(bws);
  krLamp->setMinimumWidth(bws);

  // Second Row of Lamps is the flat-field lamps, including the
  // intensity setting widget for the variable-intensity flat-field
  // lamp.

  QHBoxLayout *lampLayout2 = new QHBoxLayout;
  lampLayout2->setContentsMargins(0,0,0,0);

  qthLamp1 = new ToggleSwitch(tr("QTH1"),"LAMP QTH1","ON","OFF",this);
  connect(qthLamp1,SIGNAL(cmdReady(const QString &)),
	  this,SLOT(setLamp(const QString &)));
  lampLayout2->addWidget(qthLamp1);
  
  qthLamp2 = new ToggleSwitch(tr("QTH2"),"LAMP QTH2","ON","OFF",this);
  connect(qthLamp2,SIGNAL(cmdReady(const QString &)),
	  this,SLOT(setLamp(const QString &)));
  lampLayout2->addWidget(qthLamp2);

  vflatLamp = new ToggleSwitch(tr("VFlat"),"LAMP VFLAT","ON","OFF",this);
  connect(vflatLamp,SIGNAL(cmdReady(const QString &)),
	  this,SLOT(setVFlatLamp(const QString &)));
  lampLayout2->addWidget(vflatLamp);
  
  vflatInt = 0.0;
  vflatIsOn = false;
  vflatSetting = new NumberEntry("","",4,MODS_VFLAT_MIN,MODS_VFLAT_MAX,1,this);
  connect(vflatSetting,SIGNAL(dataReady(const double &)),
	  this,SLOT(setVFlatInt(const double &)));
  lampLayout2->addStretch();
  lampLayout2->addWidget(vflatSetting);
  lampLayout2->addStretch();

  lampLayout2->setSpacing(2);

  // Create the 2 rows of lamp buttons

  QVBoxLayout *lampButtonLayout = new QVBoxLayout;
  lampButtonLayout->addLayout(lampLayout1);
  lampButtonLayout->addLayout(lampLayout2);
  lampButtonLayout->setSpacing(1);

  // Button Layout is scrunched and fixed size

  lampButtonLayout->setSpacing(1);
  lampButtonLayout->setSizeConstraint(QLayout::SetFixedSize);

  // The Flat-field lamp button sizes have same width

  int bwc = (int)(vflatLamp->minimumSizeHint().width());
  qthLamp1->setMinimumWidth(bwc);
  qthLamp2->setMinimumWidth(bwc);
  vflatLamp->setMinimumWidth(bwc);

  // An all-off button for the lamps.  This has square aspect ratio

  lampOffButton = new ActionButton(tr("OFF"),this);
  connect(lampOffButton,SIGNAL(clicked()),this,SLOT(setLampsOff()));
  int obw = (int)(lampOffButton->minimumSizeHint().width());
  lampOffButton->setMinimumHeight(obw);

  // Horizontal Box for the lamp buttons and the power button

  QHBoxLayout *lampLayout = new QHBoxLayout;
  lampLayout->addLayout(lampButtonLayout);
  lampLayout->addWidget(lampOffButton);

  // Put it into the a group box

  QGroupBox *lampGroupBox = new QGroupBox(tr("Calibration Lamps"));
  lampGroupBox->setContentsMargins(0,14,0,0);
  lampGroupBox->setLayout(lampLayout);

  // Now put it all together into the Focal Plane QGroupBox

  QGroupBox *mainGroupBox = new QGroupBox(tr("Calibration and AGw Unit"));
  //  mainGroupBox->setContentsMargins(3,10,3,3);
  //mainGroupBox->setContentsMargins(0,14,0,0);

  QHBoxLayout *hcLayout = new QHBoxLayout;
  hcLayout->addLayout(fpTopFormLayout);
  hcLayout->addWidget(lampGroupBox,0,Qt::AlignHCenter);

  QVBoxLayout *fpLayout = new QVBoxLayout;
  fpLayout->addLayout(modeLayout);
  fpLayout->addLayout(hcLayout);
  fpLayout->addLayout(gprobeLayout);

  mainGroupBox->setLayout(fpLayout);

  //----------------------------------------------------------------

  // Main widget layout

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(mainGroupBox);
  mainLayout->setContentsMargins(0,0,0,0);
  setLayout(mainLayout);

  // Setup the lamp timer, but don't actually start it and connect its
  // timeout signal to the "all lamps off" slot

  lampTimer = new QTimer(this);
  connect(lampTimer,SIGNAL(timeout()),this,SLOT(setLampsOff()));
  maxLampOnTime = MODS_MAX_LAMPTIME;  // default max-on time

  // Various state flags we set false initially.  The first instrument status
  // calls will set these

  hatchOpen = false;
  calibStow = false;

}

// sendCommand() - Send a command to the ISIS system. 
//
// Emit the cmdReady() signal to whoever is listening for commands to
// pass along.  Most commands are just passed along as-is, but a few
// require additional actions, usually following the base command.
// Note that the decision to enqueue commands is decided by the
// parent, not here.
//

void FocalPlane::sendCommand(const QString &cmd)
{
  if (cmd.isEmpty()) return;

  // See what command we've been sent

  QString cmdStr = cmd;
  QString cmdWord = cmdStr.section(" ",0,0);

  // CALIB has additional requirements.  If "CALIB OUT", turn off lamps

  if (cmdWord.compare("CALIB",Qt::CaseInsensitive)==0) {
    emit cmdReady(modsIEHost[modsID],cmdStr);
    if (cmdStr.section(" ",1,-1).compare("OUT",Qt::CaseInsensitive)==0) setLampsOff();
  }

  // Everything else is sent as-is

  else
    emit cmdReady(modsIEHost[modsID],cmdStr);

}

// sendCmdWait() - Send a command with a timeout wait interval if
// timeout>0, queue on the command sequencer if timeout<0, or just
// send-and-forget if timeout=0.

void FocalPlane::sendCmdWait(const QString &cmdStr, const int &timeout)
{
  if (timeout == 0)
    emit cmdReady(modsIEHost[modsID],cmdStr);
  else
    emit cmdWait(modsIEHost[modsID],cmdStr,timeout);
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

void FocalPlane::parse(const QString &cmdStr, 
		       const int &msgType, 
		       const QHash<QString,QString> &keyDict)
{
  bool cmdDone = false;   // Flag: message is a command completion
  bool cmdFault = false;  // Flag: command completed with a fault
  bool ok;

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

      // Instrument Dark Hatch state
      // (include known mis-spelling)

      if (keyStr.compare("HATCH",Qt::CaseInsensitive)==0) {
	hatchControl->setISISState(msgType);
	if (valStr.compare("OPEN",Qt::CaseInsensitive)==0) {
	  hatch->setState(0);
	  hatchOpen = true;
	}
	else if (valStr.compare("CLOSED",Qt::CaseInsensitive)==0 ||
		 valStr.compare("CLOSE",Qt::CaseInsensitive)==0) {
	  hatch->setState(1);
	  hatchOpen = false;
	}
	else {
	  if (valStr.compare("BUSY",Qt::CaseInsensitive)!=0) {
	    hatch->setErrorStr(msgStr);
	    hatchControl->setFault();
	  }
	}
      }

      // Calibration Tower position

      else if (keyStr.compare("CALIB",Qt::CaseInsensitive)==0) {
	calibControl->setISISState(msgType);
	if (valStr.compare("IN",Qt::CaseInsensitive)==0) {
	  calib->setState(0);
	  calibStow = false;
	}
	else if (valStr.compare("OUT",Qt::CaseInsensitive)==0) {
	  calib->setState(1);
	  calibStow = true;
	}
	else {
	  if (valStr.compare("BUSY",Qt::CaseInsensitive)!=0) {
	    calib->setErrorStr(msgStr);
	    calibControl->setFault();
	  }
	}
      }

      // Calibration Lamps - lets us process the CALLAMPS keyword

      else if (keyStr.compare("CALLAMPS",Qt::CaseInsensitive)==0 ||
	       keyStr.compare("CALLAMP",Qt::CaseInsensitive)==0) {
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
	  qthLamp1->setOn();
	else
	  qthLamp1->setOff();
	if (valStr.contains("FLAT2",Qt::CaseInsensitive) ||
	    valStr.contains("QTH2",Qt::CaseInsensitive))
	  qthLamp2->setOn();
	else
	  qthLamp2->setOff();
	if (valStr.contains("VFLAT",Qt::CaseInsensitive)) {
	  vflatLamp->setOn();
	  vflatIsOn = true;
	}
	else {
	  vflatLamp->setOff();
	  vflatIsOn = false;
	}

	if (valStr.compare("NONE",Qt::CaseInsensitive)==0 ||
	    valStr.compare("OFF",Qt::CaseInsensitive)==0) {
	  neLamp->setOff();
	  hgLamp->setOff();
	  arLamp->setOff();
	  xeLamp->setOff();
	  krLamp->setOff();
	  qthLamp1->setOff();
	  qthLamp2->setOff();
	  vflatLamp->setOff();
	}
      }
	
      // Neon Lamp

      else if (keyStr.compare("NE",Qt::CaseInsensitive)==0) { 
	if (cmdWord.compare("LAMP",Qt::CaseInsensitive)==0) {
	  if (valStr.compare("ON",Qt::CaseInsensitive)==0)
	    neLamp->setOn();
	  else if (valStr.compare("OFF",Qt::CaseInsensitive)==0)
	    neLamp->setOff();
	  else
	    neLamp->setFault();
	}
      }
	    
      // Mercury Lamp
      
      else if (keyStr.compare("HG",Qt::CaseInsensitive)==0) { 
	if (cmdWord.compare("LAMP",Qt::CaseInsensitive)==0) {
	  if (valStr.compare("ON",Qt::CaseInsensitive)==0)
	    hgLamp->setOn();
	  else if (valStr.compare("OFF",Qt::CaseInsensitive)==0)
	    hgLamp->setOff();
	  else
	    hgLamp->setFault();
	}
      }
      
      // Argon Lamp

      else if (keyStr.compare("AR",Qt::CaseInsensitive)==0) { 
	if (cmdWord.compare("LAMP",Qt::CaseInsensitive)==0) {
	  if (valStr.compare("ON",Qt::CaseInsensitive)==0)
	    arLamp->setOn();
	  else if (valStr.compare("OFF",Qt::CaseInsensitive)==0)
	    arLamp->setOff();
	  else
	    arLamp->setFault();
	}
      }
      
      // Xenon Lamp

      else if (keyStr.compare("XE",Qt::CaseInsensitive)==0) { 
	if (cmdWord.compare("LAMP",Qt::CaseInsensitive)==0) {
	  if (valStr.compare("ON",Qt::CaseInsensitive)==0)
	    xeLamp->setOn();
	  else if (valStr.compare("OFF",Qt::CaseInsensitive)==0)
	    xeLamp->setOff();
	  else
	    xeLamp->setFault();
	}
      }
	    
      // Krypton Lamp

      else if (keyStr.compare("KR",Qt::CaseInsensitive)==0) { 
	if (cmdWord.compare("LAMP",Qt::CaseInsensitive)==0) {
	  if (valStr.compare("ON",Qt::CaseInsensitive)==0)
	    krLamp->setOn();
	  else if (valStr.compare("OFF",Qt::CaseInsensitive)==0)
	    krLamp->setOff();
	  else
	    krLamp->setFault();
	}
      }
	    
      // QTH Lamp 1
      
      else if (keyStr.compare("FLAT1",Qt::CaseInsensitive)==0 ||
	       keyStr.compare("QTH1",Qt::CaseInsensitive)==0) { 
	if (cmdWord.compare("LAMP",Qt::CaseInsensitive)==0) {
	  if (valStr.compare("ON",Qt::CaseInsensitive)==0)
	    qthLamp1->setOn();
	  else if (valStr.compare("OFF",Qt::CaseInsensitive)==0)
	    qthLamp1->setOff();
	  else
	    qthLamp1->setFault();
	}
      }
	    
      // QTH Lamp 2
      
      else if (keyStr.compare("FLAT2",Qt::CaseInsensitive)==0 ||
	       keyStr.compare("QTH2",Qt::CaseInsensitive)==0) { 
	if (cmdWord.compare("LAMP",Qt::CaseInsensitive)==0) {
	  if (valStr.compare("ON",Qt::CaseInsensitive)==0)
	    qthLamp2->setOn();
	  else if (valStr.compare("OFF",Qt::CaseInsensitive)==0)
	    qthLamp2->setOff();
	  else
	    qthLamp2->setFault();
	}
      }
	    
      // Variable-intensity flat (VFLAT) lamp intensity level

      else if (keyStr.compare("VFLAT",Qt::CaseInsensitive)==0) { 
	double dtmp = valStr.toDouble(&ok);
	if (ok && (dtmp >= MODS_VFLAT_MIN && dtmp <= MODS_VFLAT_MAX)) {
	  vflatInt = dtmp;
	  vflatSetting->setValue(vflatInt);
	  vflatSetting->setNormal();
	}
	else
	  vflatSetting->setFault();
      }

      // AGw Filter Wheel

      else if (keyStr.compare("AGWFILT",Qt::CaseInsensitive)==0) {
	agwFiltControl->setISISState(msgType);
	int iPos = valStr.toInt(&ok,10);
	if (ok) agwFilter->setPos(iPos);
      }

      // AGw Guide Probe X stage position 
      
      else if (keyStr.compare("AGWXGP",Qt::CaseInsensitive)==0) {
	double pos = valStr.toDouble(&ok);
	if (ok) {
	  gprobeX->setPos(pos);
	  gpXDisplay->setText(valStr,Qt::blue);
	  gpXDisplay->setNormal();
	  agwHomeButton->setNormal();
	}
	else
	  if (valStr.compare("BUSY",Qt::CaseInsensitive)!=0)
	    gpXDisplay->setFault();
      }
      
      // AGw Guide Probe Y stage position 
      
      else if (keyStr.compare("AGWYGP",Qt::CaseInsensitive)==0) {
	double pos = valStr.toDouble(&ok);
	if (ok) {
	  gprobeY->setPos(pos);
	  gpYDisplay->setText(valStr,Qt::blue);
	  gpYDisplay->setNormal();
	  agwHomeButton->setNormal();
	}
	else
	  if (valStr.compare("BUSY",Qt::CaseInsensitive)!=0)
	    gpYDisplay->setFault();
      }

      // AGw Stage Guide Probe Focus 

      else if (keyStr.compare("AGWFOCUS",Qt::CaseInsensitive)==0) {
	double pos = valStr.toDouble(&ok);
	if (ok) {
	  gpFocus->setPos(pos);
	  gpFocDisplay->setText(valStr,Qt::blue);
	  gpFocDisplay->setNormal();
	}
	else
	  if (valStr.compare("BUSY",Qt::CaseInsensitive)!=0)
	    gpFocDisplay->setFault();
      }

      // Instrument Mode: CAL or OBS

      else if (keyStr.compare("INSTMODE",Qt::CaseInsensitive)==0) {
	if (valStr.compare("CAL",Qt::CaseInsensitive)==0) {
	  calModeButton->highLight();
	  obsModeButton->setNormal();
	}
	else if (valStr.compare("OBS",Qt::CaseInsensitive)==0) {
	  obsModeButton->highLight();
	  calModeButton->setNormal();
	}
      }

      else {
	// unrecognized keyword, move along...
      }

      // If the keyword value is BUSY, reset the cmdFault flag

      if (valStr.compare("BUSY",Qt::CaseInsensitive)==0) cmdFault = false;

      // Check the hatch and calibration state, and set calmode or obsmode button
      // states as needed

      if (hatchOpen && calibStow) {
	obsModeButton->highLight();
	calModeButton->setNormal();
      }
      else if (!hatchOpen && !calibStow) {
	calModeButton->highLight();
	obsModeButton->setNormal();
      }

    }
  }

  // Process fault conditions
  
  if (cmdFault) {
    if (cmdWord.compare(hatch->command(),Qt::CaseInsensitive)==0) {
      hatch->setErrorStr(msgStr);
      hatchControl->setFault();
    }
    else if (cmdWord.compare(calib->command(),Qt::CaseInsensitive)==0) {
      calib->setErrorStr(msgStr);
      calibControl->setFault();
    }
    else if (cmdWord.compare("GPROBE",Qt::CaseInsensitive)==0) {
      gpXDisplay->setFault();
      gpYDisplay->setFault();
    }
    else if (cmdWord.compare("GPFOCUS",Qt::CaseInsensitive)==0) {
      gpFocDisplay->setFault();
    }
    else if (cmdWord.compare(agwFilter->command(),Qt::CaseInsensitive)==0) {
      agwFiltControl->setFault();
    }
  }

  // Other things to do?  Do them here...

}

// Some special methods

// Handle calibration lamp commands

void FocalPlane::setLamp(const QString &cmd)
{
  if (cmd.isEmpty()) return;

  // See what command we've been sent

  QString cmdWord = cmd.section(" ",0,0);
  QStringList wordList = cmd.section(" ",1,-1).split(" ");

  if (cmdWord.compare("lamp",Qt::CaseInsensitive)==0) {
    if (wordList.count() == 1 && (wordList.at(0).compare("OFF",Qt::CaseInsensitive)==0)) {
      setLampsOff();
      return;
    }
    else if (wordList.count() > 1 && cmd.contains("ON",Qt::CaseInsensitive))
      startLampTimer();

    emit cmdReady(modsIEHost[modsID],cmd);
  }
}

// Turn off all calibration lamps and stop the lamp timer

void FocalPlane::setLampsOff()
{
  emit cmdReady(modsIEHost[modsID],"LAMP OFF");
  lampTimer->stop();
}

// Update the focal plane control panel

void FocalPlane::update()
{
  // Reset the control states (free stuck controls)

  neLamp->setUnknown();
  hgLamp->setUnknown();
  arLamp->setUnknown();
  xeLamp->setUnknown();
  krLamp->setUnknown();
  qthLamp1->setUnknown();
  qthLamp2->setUnknown();
  vflatLamp->setUnknown();
  hatchControl->setUnknown();
  calibControl->setUnknown();
  gpXDisplay->clear();
  gpYDisplay->clear();
  gpFocDisplay->clear();
  agwFiltControl->setUnknown();
  agwHomeButton->setNormal();
  calModeButton->setNormal();
  obsModeButton->setNormal();
  gpRestore = false;
  gpRestoreCheck->setCheckState(Qt::Unchecked);

  // Send the command requests to get the current status of all the
  // mechanisms under our control, especially those that might get
  // set "by hand" during work inside the instrument.
 
  sendCmdWait("hatch",MODS_QUEUE_REQUEST);
  sendCmdWait("calib",MODS_QUEUE_REQUEST);
  sendCmdWait("agw",MODS_QUEUE_REQUEST);
  sendCmdWait("lamp",MODS_QUEUE_REQUEST);

}

// Home the guide probe

void FocalPlane::homeGuideProbe()
{
  sendCommand("GPROBE HOME");
  gpXDisplay->setChange();
  gpYDisplay->setChange();
}

// Set the AGw guide/acquire camera filter ID list

void FocalPlane::setAGwFiltName(QStringList *dList)
{
  agwFilter->setIDList(dList);
}

// Get the AGw guide/acquire camera filter ID list

QStringList FocalPlane::agwFiltName()
{
  return agwFilter->IDList();
}

// Set the named mechanism widget to the "changing" state.  This is
// used by the parent dashboard to indicate when a device command is
// being executed via the remote channel, providing visual feedback on
// the dashboard GUI.  Currently we show change on slow mechanisms
// like the calib tower, hatch, and AGw stage.


void FocalPlane::setChange(const QString &device)
{
  if (device.isEmpty()) return;
  if (device.compare(hatch->command(),Qt::CaseInsensitive)==0)
    hatchControl->setChange();
  else if (device.compare(calib->command(),Qt::CaseInsensitive)==0)
    calibControl->setChange();
  else if (device.compare(agwFilter->command(),Qt::CaseInsensitive)==0)
    agwFiltControl->setChange();
  else if (device.compare("gprobe",Qt::CaseInsensitive)==0) {
    gpXDisplay->setChange();
    gpYDisplay->setChange();
  }
  else if (device.compare("calmode",Qt::CaseInsensitive)==0) {
    calModeButton->setChange();
    hatchControl->setChange();
    calibControl->setChange();
    gpXDisplay->setChange();
    gpYDisplay->setChange();
  }
  else if (device.compare("obsmode",Qt::CaseInsensitive)==0) {
    obsModeButton->setChange();
    hatchControl->setChange();
    calibControl->setChange();
    gpXDisplay->setChange();
    gpYDisplay->setChange();
  }
  return;
}

// Toggle the variable-intensity flat-field lamp on/off with the specified
// power

void FocalPlane::setVFlatLamp(const QString &setStr)
{
  QString cmdStr;
  if (setStr.contains("ON",Qt::CaseInsensitive)) {
    if (vflatInt <= 0.0) 
      cmdStr = "LAMP VFLAT OFF";
    else {
      cmdStr = QString("LAMP VFLAT %1 ON").arg(vflatInt);
      startLampTimer();
    }
  }
  else {
    cmdStr = "LAMP VFLAT OFF";
  }
  sendCommand(cmdStr);
}

// Set the intensity of the variable-intensity flat-field
// lamp If the lamp is already on, send new setting, otherwise just
// note for future power on.

void FocalPlane::setVFlatInt(const double &newInt)
{
  if (newInt < MODS_VFLAT_MIN || newInt > MODS_VFLAT_MAX) return;
  vflatInt = newInt;
  if (vflatIsOn)
    sendCommand(QString("LAMP VFLAT %1").arg(vflatInt));
  vflatSetting->setNormal();
}

// Set the maximum lamp-on time in seconds.  Setting it to 0 or -1
// disables the lamp timer.  If the lamp timer is running and it is
// set to disable also stop the timer.

void FocalPlane::setMaxLampTime(const int &newTime)
{
  if (newTime <=0) {
    maxLampOnTime = 0;
    lampTimer->stop();
  }
  else
    maxLampOnTime = newTime;
}

// Report the maximum lamp-on time in seconds

int FocalPlane::maxLampTime()
{
  return maxLampOnTime;
}

// Start the lamp timer (or restart if running)

void FocalPlane::startLampTimer()
{
  lampTimer->stop(); // just in case...
  if (maxLampOnTime <= 0) return;
  lampTimer->start(1000*maxLampOnTime);
}

// Set Observing Mode

void FocalPlane::setObsMode()
{
  QString cmdStr;
  if (gpRestore)
    cmdStr = "OBSMODE RESTORE";
  else
    cmdStr = "OBSMODE";

  // Set changing state indicators
  obsModeButton->setChange();
  calModeButton->setNormal();
  hatchControl->setChange();
  calibControl->setChange();
  gpXDisplay->setChange();
  gpYDisplay->setChange();

  // Send it...

  sendCommand(cmdStr);
  
}

// Set Calibration Mode

void FocalPlane::setCalMode()
{
  QString cmdStr;
  cmdStr = "CALMODE";

  // Set changing state indicators
  obsModeButton->setNormal();
  calModeButton->setChange();
  hatchControl->setChange();
  calibControl->setChange();
  gpXDisplay->setChange();
  gpYDisplay->setChange();

  // Send it...

  sendCommand(cmdStr);
  
}

// Set the OBSMODE restore state

void FocalPlane::setRestoreGP() 
{
  if (gpRestoreCheck->isChecked())
    gpRestore = true;
  else
    gpRestore = false;
}
