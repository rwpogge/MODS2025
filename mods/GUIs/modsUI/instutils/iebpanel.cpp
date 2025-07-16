#include <QtWidgets>
#include <QtNetwork>
#include <math.h>

#include "iebpanel.h"

#include "powerswitch.h"
#include "powerbutton.h"
#include "toggleswitch.h"
#include "lightbutton.h"
#include "textdisplay.h"
#include "actionbutton.h"

#include "isisclient.h"

IEBPanel::IEBPanel(const int &myMODS, const QString &myIEB, QWidget *parent)
  : QWidget(parent)
{
  modsID = myMODS;
  modsNum = modsID + 1;
  iebID = myIEB;
 
  // Load the IEB configuration file

  mlynxList = new QStringList;
  tempSensorList = new QStringList;

  QString cfgFile = QString("%1/Config/MODS%2/ieb_%3.tab").arg(MODS_BASE_PATH).arg(modsNum).arg(iebID);
  loadConfig(cfgFile);

  // Drive Power button - a soft-off switch since it only turns
  // of the drive power but leaves interfaces powered

  drivePower = new PowerSwitch(tr("Drive Power"),QString("IEB %1 MPOWER").arg(iebID),
			       64, false, this);
  connect(drivePower,SIGNAL(cmdReady(const QString &)),this,SLOT(setDrivePower(const QString &)));

  // Update Button

  updateButton = new ActionButton(tr("Update"),this);
  connect(updateButton,SIGNAL(clicked()),this,SLOT(update()));
  
  // Check Box to lock/unlock the power controls.  We lock these
  // controls by default because power cycling MicroLYNX controllers
  // while under AC power is strenous - there is high in-rush current
  // (up to 40-50A) that can fuse the relay if done too often.  Even
  // when locked, the buttons display the power state.

  lockPower = true;
  lockPowerCheck = new QCheckBox(tr("Lock\nPower"),this);
  if (lockPower)
    lockPowerCheck->setCheckState(Qt::Checked);
  else
    lockPowerCheck->setCheckState(Qt::Unchecked);
  connect(lockPowerCheck,SIGNAL(stateChanged(int)),this,SLOT(setPowerLock()));

  // Power Readouts

  QFormLayout *powerReadoutLayout = new QFormLayout;
  powerReadoutLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
  powerReadoutLayout->setFormAlignment(Qt::AlignCenter);  
  powerReadoutLayout->setLabelAlignment(Qt::AlignRight);
  powerReadoutLayout->setVerticalSpacing(0);

  iebStatus = false;
  iebStatusDisplay = new TextDisplay("","",8,this);
  iebStatusDisplay->setNormalColors(Qt::black,Qt::green);
  iebStatusDisplay->setFaultColors(Qt::black,Qt::red); 
  iebStatusDisplay->setChangeColors(Qt::black,QColor(255,165,0,255));
  iebStatusDisplay->setAlignment(Qt::AlignCenter);
  iebStatusDisplay->setNormal();
  iebStatusDisplay->clear();
  powerReadoutLayout->addRow(tr("Status:"),iebStatusDisplay);

  driveVoltage = 0.0;
  driveVoltageDisplay = new TextDisplay("",tr("VDC"),5,this);
  driveVoltageDisplay->setNormalColors(Qt::black,Qt::green);
  driveVoltageDisplay->setFaultColors(Qt::black,Qt::red);
  driveVoltageDisplay->setChangeColors(Qt::black,QColor(255,165,0,255));
  driveVoltageDisplay->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  driveVoltageDisplay->setNormal();
  driveVoltageDisplay->clear();
  powerReadoutLayout->addRow(tr("Drive Voltage:"),driveVoltageDisplay);

  driveCurrent = 0.0;
  driveCurrentDisplay = new TextDisplay("",tr("Amps"),5,this);
  driveCurrentDisplay->setNormalColors(Qt::black,Qt::green);
  driveCurrentDisplay->setFaultColors(Qt::black,Qt::red);
  driveCurrentDisplay->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  driveCurrentDisplay->setChangeColors(Qt::black,QColor(255,165,0,255));
  driveCurrentDisplay->setNormal();
  driveCurrentDisplay->clear();
  powerReadoutLayout->addRow(tr("Drive Current:"),driveCurrentDisplay);

  wagoVoltage = 0.0;
  wagoVoltageDisplay = new TextDisplay("",tr("VDC"),5,this);
  wagoVoltageDisplay->setNormalColors(Qt::black,Qt::green);
  wagoVoltageDisplay->setFaultColors(Qt::black,Qt::red);
  wagoVoltageDisplay->setChangeColors(Qt::black,QColor(255,165,0,255));
  wagoVoltageDisplay->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  wagoVoltageDisplay->setNormal();
  wagoVoltageDisplay->clear();
  powerReadoutLayout->addRow(tr("WAGO Voltage:"),wagoVoltageDisplay);

  // Temperature Readouts

  QFormLayout *tempReadoutLayout = new QFormLayout;
  tempReadoutLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
  tempReadoutLayout->setFormAlignment(Qt::AlignCenter);  
  tempReadoutLayout->setLabelAlignment(Qt::AlignRight);
  tempReadoutLayout->setVerticalSpacing(0);

  for (int i=0; i<4; i++) {
    temp[i] = 0.0;
    tempDisplay[i] = new TextDisplay("",tr("C"),5,this);
    tempDisplay[i]->setNormalColors(Qt::black,Qt::green);
    tempDisplay[i]->setFaultColors(Qt::black,Qt::red);
    tempDisplay[i]->setChangeColors(Qt::black,QColor(255,165,0,255));
    tempDisplay[i]->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
    tempDisplay[i]->clear();
    tempReadoutLayout->addRow(tr("%1:").arg(tempSensorList->at(i)),tempDisplay[i]);
  }

  // Individual MicroLynx toggle switches
  // Each are in a row of 8 buttons

  // First Row: Odd-numbered controllers MLC1..15

  QHBoxLayout *mlcomLayout1 = new QHBoxLayout;
  mlcomLayout1->setContentsMargins(0,0,0,0);

  int maxWid = 0;
  int bwid = 0;
  for (int i=0;i<16;i+=2) {
    mlSwitch[i] = new ToggleSwitch(mlynxList->at(i),
				   QString("IEB %1 MLC %2").arg(iebID).arg(i+1),
				   "ON","OFF",this);
    connect(mlSwitch[i],SIGNAL(cmdReady(const QString &)),
	    this,SLOT(setMLynxPower(const QString &)));
    mlcomLayout1->addWidget(mlSwitch[i]);
    bwid = (int)(mlSwitch[i]->minimumSizeHint().width());
    if (bwid > maxWid) maxWid = bwid;
    mlSwitch[i]->setFont(QFont(MODS_BASE_FONT,MODS_BASE_FONTSIZE-2,QFont::Normal));
  }
  mlcomLayout1->addStretch();
  mlcomLayout1->setSpacing(2);

  // Second Row: Even-numbered controllers MLC2..16

  QHBoxLayout *mlcomLayout2 = new QHBoxLayout;
  mlcomLayout2->setContentsMargins(0,0,0,0);
  
  maxWid = 0;
  for (int i=1;i<16;i+=2) {
    mlSwitch[i] = new ToggleSwitch(mlynxList->at(i),
				   QString("IEB %1 MLC %2").arg(iebID).arg(i+1),
				   "ON","OFF",this);
    connect(mlSwitch[i],SIGNAL(cmdReady(const QString &)),
	    this,SLOT(setMLynxPower(const QString &)));
    mlcomLayout2->addWidget(mlSwitch[i]);
    int bwid = (int)(mlSwitch[i]->minimumSizeHint().width());
    if (bwid > maxWid) maxWid = bwid;
    mlSwitch[i]->setFont(QFont(MODS_BASE_FONT,MODS_BASE_FONTSIZE-2,QFont::Normal));
  } 
  mlcomLayout2->addStretch();
  mlcomLayout2->setSpacing(2);

  // Now homogenize the toggleswitch sizes

  for (int i=0;i<16;i++)
    mlSwitch[i]->setMinimumWidth(maxWid);

  // Stack them in a vertical layout box

  QVBoxLayout *mlcomLayout = new QVBoxLayout;
  mlcomLayout->addLayout(mlcomLayout1);
  mlcomLayout->addLayout(mlcomLayout2);
  mlcomLayout->setContentsMargins(3,1,3,1);
  mlcomLayout->setSpacing(0);

  // And wrap in a QGroupBox

  QGroupBox *mlGroupBox = new QGroupBox(tr("MicroLYNX Controller Power"));
  mlGroupBox->setLayout(mlcomLayout);

  // Organize the various elements

  QVBoxLayout *buttonLayout = new QVBoxLayout;
  buttonLayout->addWidget(updateButton,0,Qt::AlignHCenter);
  buttonLayout->addWidget(lockPowerCheck,0,Qt::AlignHCenter);

  QHBoxLayout *powerLayout = new QHBoxLayout;
  powerLayout->addWidget(drivePower,0,Qt::AlignCenter);
  powerLayout->addStretch();
  powerLayout->addLayout(powerReadoutLayout);
  powerLayout->addStretch();
  powerLayout->addLayout(tempReadoutLayout);
  powerLayout->addStretch();
  powerLayout->addLayout(buttonLayout);
  powerLayout->setContentsMargins(1,1,1,1);

  QVBoxLayout *controlLayout = new QVBoxLayout;
  controlLayout->addLayout(powerLayout);
  controlLayout->addWidget(mlGroupBox);

  // The main group box 

  QString gbTitle;
  if (iebID.compare("B",Qt::CaseInsensitive)==0)
    gbTitle = tr("MODS%1 Blue Channel IEB").arg(modsNum);
  else
    gbTitle = tr("MODS%1 Red Channel IEB").arg(modsNum);

  QGroupBox *mainGroupBox = new QGroupBox(gbTitle);

  mainGroupBox->setContentsMargins(3,15,3,3);
  mainGroupBox->setLayout(controlLayout);

  // ... and build the final layout.

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(mainGroupBox);
  mainLayout->setContentsMargins(0,0,0,0);
  setLayout(mainLayout);

  // Anything else?

}

// Pass the command string out to the parent control panel 

void IEBPanel::sendCommand(const QString &cmd)
{
  if (cmd.isEmpty()) return;
  emit statusMsg(cmd,Qt::blue);
  emit cmdReady(modsIEHost[modsID],cmd);
}

// Pass the command string out to the parent control panel and wait
// for completion or timeout

void IEBPanel::sendCmdWait(const QString &cmd, const int &timeout)
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
// Note that many IEB-specific keywords come qualified with a _#
// suffix that indicates the IEB ID that it corresponds to, thus
// MLC1_B is MicroLynx Controller #1 in the blue channel IEB, VDRIVE_R
// is the stepper motor drive power supply voltage in the red channel
// IEB, etc.  This disambiguates keywords shared among multiple
// instances of an IEB in the system.
//


void IEBPanel::parse(const QString &cmdStr, 
		     const int &msgType, 
		     const QHash<QString,QString> &keyDict)
{
  // TODO: Unused flags. Commented out on 2-19-24
  // bool cmdDone = false;   // Flag: message is a command completion
  // bool cmdFault = false;  // Flag: command completed with a fault
  QString iebIndex;       // IEB index (B or R) for keyword disambiguation
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

      // Look for the IEB index, denoted by _# at the end of the
      // keyword.  We only need to do something with this keyword if
      // it is for our IEB instance.

      if (keyStr.contains("_",Qt::CaseInsensitive)) {
	iebIndex = keyStr.section("_",1,1);

	if (iebIndex.compare(iebID,Qt::CaseInsensitive)==0) {

	  // MLCn power states
	  
	  if (keyStr.contains("MLC",Qt::CaseInsensitive)) {
	    QString mlcStr = keyStr.section("_",0,0);
	    int mlNum = mlcStr.section("MLC",1,-1).toInt(&ok,10);
	    if (mlNum > 0 && mlNum <= 16) {
	      if (valStr.compare("ON",Qt::CaseInsensitive)==0) {
		mlSwitch[mlNum-1]->setOn();
		mlSwitchOn[mlNum-1] = true;
	      }
	      else if (valStr.compare("OFF",Qt::CaseInsensitive)==0) {
		mlSwitch[mlNum-1]->setOff();
		mlSwitchOn[mlNum-1] = false;
	      }
	      else
		mlSwitch[mlNum-1]->setFault();
	    }
	  }

	  // IEB Main Power - special case if off
	  //
	  // Treat UNKNOWN as an alias for OFF and blank displays
	  //

	  else if (keyStr.contains("IEB_",Qt::CaseInsensitive)) {
	    if (valStr.compare("OFF",Qt::CaseInsensitive)==0 ||
		valStr.compare("UNKNOWN",Qt::CaseInsensitive)==0) {
	      drivePower->setOff();
	      for (int i=0;i<16;i++) {
		mlSwitch[i]->setOff();
		mlSwitchOn[i] = false;
	      }
	      iebStatus = false;
	      iebStatusDisplay->setText(tr("OFFLINE"),Qt::black);
	      iebStatusDisplay->setFault();
	      driveVoltageDisplay->clear();
	      driveVoltageDisplay->setNormal();
	      driveCurrentDisplay->clear();
	      driveCurrentDisplay->setNormal();
	      wagoVoltageDisplay->clear();
	      wagoVoltageDisplay->setNormal();
	      for (int i=0;i<4;i++) {
		tempDisplay[i]->clear();
		tempDisplay[i]->setNormal();
	      }
	    }
	    else if (valStr.compare("ON",Qt::CaseInsensitive)==0) {
	      iebStatus = true;
	      iebStatusDisplay->setText(tr("ONLINE"),Qt::green);
	      iebStatusDisplay->setNormal();
	    }
	  }

	  // MPOWER_# = Motor Controller Power Supply.  If Off, also
 	  //            turn off MLC switches and clear the voltage
	  //            and current displays

	  else if (keyStr.contains("MPOWER_",Qt::CaseInsensitive)) {
	    if (valStr.compare("ON",Qt::CaseInsensitive)==0)
	      drivePower->setOn();
	    else if (valStr.compare("OFF",Qt::CaseInsensitive)==0) {
	      drivePower->setOff();
	      for (int i=0;i<16;i++) {
		mlSwitch[i]->setOff();
		mlSwitchOn[i] = false;
	      }
	      driveVoltageDisplay->clear();
	      driveCurrentDisplay->clear();
	    }
	    else
	      drivePower->setFault();
	  }

	  // Voltage/Current readouts

	  else if (keyStr.contains("VDRIVE_",Qt::CaseInsensitive)) {
	    double dtmp = valStr.toDouble(&ok);
	    if (ok) {
	      driveVoltage = dtmp;
	      driveVoltageDisplay->setText(QString::number(driveVoltage,'f',1),Qt::green);
	      driveVoltageDisplay->setNormal();
	    }
	    else {
	      driveVoltageDisplay->clear();
	    }
	  }

	  else if (keyStr.contains("IDRIVE_",Qt::CaseInsensitive)) {
	    double dtmp = valStr.toDouble(&ok);
	    if (ok) {
	      driveCurrent = dtmp;
	      driveCurrentDisplay->setText(QString::number(driveCurrent,'f',2),Qt::green);
	      driveCurrentDisplay->setNormal();
	    }
	    else {
	      driveCurrentDisplay->clear();
	    }
	  }

	  else if (keyStr.contains("VCONTROL_",Qt::CaseInsensitive)) {
	    double dtmp = valStr.toDouble(&ok);
	    if (ok) {
	      wagoVoltage = dtmp;
	      wagoVoltageDisplay->setText(QString::number(wagoVoltage,'f',1),Qt::green);
	      wagoVoltageDisplay->setNormal();
	    }
	    else {
	      wagoVoltageDisplay->clear();
	    }
	  }

	  /*
	  // TEMPn_ Temperature readouts.  Readings over 100C mean no RTD connected...

	  else if (keyStr.contains("TEMP",Qt::CaseInsensitive)) {
	    QString trStr = keyStr.section("_",0,0);
	    int trNum = trStr.section("TEMP",1,-1).toInt(&ok,10);
	    if (trNum > 0 && trNum <= 4) {
	      double dtmp = valStr.toDouble(&ok);
	      if (ok && dtmp < 200.0) {
		temp[trNum-1] = dtmp;
		tempDisplay[trNum-1]->setText(QString::number(temp[trNum-1],'f',1),Qt::green);
		tempDisplay[trNum-1]->setNormal();
	      }
	      else {
		tempDisplay[trNum-1]->clear();
	      }
	    }
	  }
	  */

	  // IEBGRT_x = IEB Gylcol Return Temp (temp index 1)

	  else if (keyStr.contains("IEBGRT",Qt::CaseInsensitive)) {
	    double dtmp = valStr.toDouble(&ok);
	    if (ok && dtmp < 200.0) {
	      temp[1] = dtmp;
	      tempDisplay[1]->setText(QString::number(temp[1],'f',1),Qt::green);
	      tempDisplay[1]->setNormal();
	    }
	    else {
	      tempDisplay[1]->clear();
	    }
	  }

	  else {
	    // unrecognized keyword, move along...
	  }
	}
      } // bottom of _<channel> indexed keyword parsing

      // Keywords that are not indexed with _<channel>

      if (iebID.compare("B",Qt::CaseInsensitive)==0) { // Blue channel IEB
	if (keyStr.compare("IEBTEMPB",Qt::CaseInsensitive)==0) {
	  double dtmp = valStr.toDouble(&ok);
	  if (ok && dtmp < 200.0) {
	    temp[0] = dtmp;
	    tempDisplay[0]->setText(QString::number(temp[0],'f',1),Qt::green);
	    tempDisplay[0]->setNormal();
	  }
	  else {
	    tempDisplay[0]->clear();
	  }
	}
	else if (keyStr.compare("TCOLLTOP",Qt::CaseInsensitive)==0) {
	  double dtmp = valStr.toDouble(&ok);
	  if (ok && dtmp < 200.0) {
	    temp[2] = dtmp;
	    tempDisplay[2]->setText(QString::number(temp[2],'f',1),Qt::green);
	    tempDisplay[2]->setNormal();
	  }
	  else {
	    tempDisplay[2]->clear();
	  }
	}
	else if (keyStr.compare("TCOLLBOT",Qt::CaseInsensitive)==0) {
	  double dtmp = valStr.toDouble(&ok);
	  if (ok && dtmp < 200.0) {
	    temp[3] = dtmp;
	    tempDisplay[3]->setText(QString::number(temp[3],'f',1),Qt::green);
	    tempDisplay[3]->setNormal();
	  }
	  else {
	    tempDisplay[3]->clear();
	  }
	}
      }

      else if (iebID.compare("R",Qt::CaseInsensitive)==0) { // Red channel IEB
	if (keyStr.compare("IEBTEMPR",Qt::CaseInsensitive)==0) {
	  double dtmp = valStr.toDouble(&ok);
	  if (ok && dtmp < 200.0) {
	    temp[0] = dtmp;
	    tempDisplay[0]->setText(QString::number(temp[0],'f',1),Qt::green);
	    tempDisplay[0]->setNormal();
	  }
	  else {
	    tempDisplay[0]->clear();
	  }
	}
	else if (keyStr.compare("TAIRTOP",Qt::CaseInsensitive)==0) {
	  double dtmp = valStr.toDouble(&ok);
	  if (ok && dtmp < 200.0) {
	    temp[2] = dtmp;
	    tempDisplay[2]->setText(QString::number(temp[2],'f',1),Qt::green);
	    tempDisplay[2]->setNormal();
	  }
	  else {
	    tempDisplay[2]->clear();
	  }
	}
	else if (keyStr.compare("TAIRBOT",Qt::CaseInsensitive)==0) {
	  double dtmp = valStr.toDouble(&ok);
	  if (ok && dtmp < 200.0) {
	    temp[3] = dtmp;
	    tempDisplay[3]->setText(QString::number(temp[3],'f',1),Qt::green);
	    tempDisplay[3]->setNormal();
	  }
	  else {
	    tempDisplay[3]->clear();
	  }
	}
      }

    } // bottom of hash iterator loop
  } // bottom of keyword parsing

  // If commands require additional handling, especially for faults,
  // do it here...

}

// Update the control panel

void IEBPanel::update()
{
  // Reset readouts and controls

  iebStatus = false;
  iebStatusDisplay->clear();
  iebStatusDisplay->setChange();
  drivePower->setUnknown();
  for (int i=0;i<16;i++) {
    mlSwitch[i]->setUnknown();
    mlSwitchOn[i] = false;
  }
  driveVoltageDisplay->clear();
  driveCurrentDisplay->clear();
  wagoVoltageDisplay->clear();
  for (int i=0;i<4;i++) {
    tempDisplay[i]->clear();
    tempDisplay[i]->setNormal();
  }

  // Send an "IEB n" general status query

  sendCmdWait(QString("IEB %1 STATUS").arg(iebID),MODS_QUEUE_REQUEST);
	      
}

// Set the MicroLYNX controller mechanism assignments using a QStringList

void IEBPanel::setMLIDs(QStringList *newList)
{
  mlynxList->clear();
  for (int i=0;i<15;i++) {
    if (newList->at(i).compare("NONE",Qt::CaseInsensitive)==0)
      mlynxList->append(QString("MLC%1").arg(i));
    else
      mlynxList->append(newList->at(i));
    mlSwitch[i]->setText(mlynxList->at(i));
  }
}

// Load and parse an IEB configuration fe

void IEBPanel::loadConfig(const QString &cfgFile)
{
  bool ok;

  // Set the runtime default values

  mlynxList->clear();
  for (int i=0;i<16;i++)
    mlynxList->append(QString("MLC%1").arg(i+1));

  tempSensorList->clear();
  for (int i=0;i<4;i++)
    tempSensorList->insert(i,QString("Temp %1").arg(i+1));

  if (cfgFile.isEmpty()) return;  // null config file means "use defaults"

  QFile inFile(cfgFile);
  if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QString errMsg = tr("Could not open the MODS%1 IEB-%2 configuration file %3!\n"
			"Reason:\n %4\nSystem defaults loaded instead.").arg(modsNum).arg(iebID).arg(cfgFile).arg(inFile.errorString());
    QMessageBox::warning(this,tr("IEB Configuration File Load Error"),errMsg); 
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

	  // Temperature Sensor Assignment
	  //
	  // Syntax: TEMP n String Temp
	  //         n=1..4, use "none" to use the default setting

	  if (keyWord.compare("TEMP",Qt::CaseInsensitive)==0) {
	    int tempNum = itemStr.section(' ',1,1).toInt(&ok,10);
	    if (ok && (tempNum > 0 && tempNum <= 4)) {
	      if (itemStr.section(' ',2,-1).compare("NONE",Qt::CaseInsensitive)!=0)
		tempSensorList->insert(tempNum-1,itemStr.section(' ',2,-1));
	      else
		tempSensorList->insert(tempNum-1,QString("Temp %1").arg(tempNum));
	    }
	  }

	  // MicroLYNX Controller Assignment

	  else if (keyWord.compare("MLC",Qt::CaseInsensitive)==0) {
	    int mlcNum = itemStr.section(' ',1,1).toInt(&ok,10);
	    if (ok && (mlcNum > 0 && mlcNum <= 16)) {
	      if (itemStr.section(' ',2,-1).compare("NONE",Qt::CaseInsensitive)!=0)
		mlynxList->insert(mlcNum-1,itemStr.section(' ',2,-1));
	      else
		mlynxList->insert(mlcNum-1,QString("MLC%1").arg(mlcNum));
	    }
	  }
	}
      }
    }
  } while (!line.isNull());
  inFile.close();

}

// Lock/Unlock the IEB power control buttons.
//
// When unlocking, warn them that cycling power on elements of the
// IEBs is a strenous activity and should only be undertaken under the
// direct supervision of support or OSU engineering team personnel.
// Make them *want* to do it.
//
// When locking, say nothing and just do it
//

void IEBPanel::setPowerLock()
{
  if (lockPowerCheck->isChecked()) {
    lockPower = true;
    emit statusMsg(tr("MODS%1 IEB %2 Power Controls are now LOCKED").arg(modsID+1).arg(iebID),Qt::blue);
  }
  else {
    QString titleStr = tr("Unlock MODS%1 IEB %2 Power Controls").arg(modsID+1).arg(iebID);
    QString msgStr = tr("You are attempting to unlock the power controls for the"
			" MODS%1 IEB %2.\n\n"
			"This will allow you to power cycle some or all of the instrument mechanism"
			" motor controllers operated by this IEB unit, some of which if powered off"
			" could require many minutes after power up to restore full instrument function.\n"
			"\nAre you really sure you want to do this?\n").arg(modsID+1).arg(iebID);
    int ret = QMessageBox::warning(this,titleStr,msgStr,
				   (QMessageBox::Cancel | QMessageBox::Yes));
    switch (ret) {
    case QMessageBox::Yes:
      lockPower = false;
      emit statusMsg(tr("MODS%1 IEB %2 Power Controls are now UNLOCKED.").arg(modsID+1).arg(iebID),
		     Qt::blue);
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

// Set the MicroLYNX controller state at the device level.  This
// checks the lockPower state. If the power controls are locked, it
// gripes with a pop-up box and then resets the state of the
// indicators (clearing any change state set when the button was
// pressed).

void IEBPanel::setMLynxPower(const QString &cmd)
{
  if (cmd.isEmpty()) return;

  // IF the IEB power controls are locked, gripe in status and reset
  // state

  if (lockPower) { 
    emit statusMsg(tr("MODS%1 IEB %2 MicroLYNX Controller Power Controls are LOCKED").arg(modsID+1).arg(iebID),
		   Qt::red);
    QString titleStr = tr("MODS%1 IEB %2 Power Controls are LOCKED").arg(modsID+1).arg(iebID);
    QString msgStr = tr("The MODS%1 IEB %2 Power Controls are currently locked out.\n\n"
                        "This is a safety feature to prevent you from accidentally turning off"
                        " an individual MicroLYNX stepper motor controller inside this IEB unit."
                        " Accidental power cycling of mechanisms in an active instrument could"
			" result in loss of data and cost many minutes of lost time recovering"
			" full instrument function.\n"
                        "\nUnlock the power controls at right if this is really what you"
                        " want to do.\n").arg(modsID+1).arg(iebID);
    QMessageBox::information(this,titleStr,msgStr,QMessageBox::Ok);    
    for (int i=0;i<16;i++) {
      if (mlSwitchOn[i])
	mlSwitch[i]->setOn();
      else
	mlSwitch[i]->setOff();
    }
  }

  // We're unlocked, do it

  else {
    emit statusMsg(cmd,Qt::blue);
    emit cmdReady(modsIEHost[modsID],cmd);
  }

}

// Set the AC Power On/Off state of the IEB Motor Drive Power Supply
// We pay attention to the state of the power lock for this.  If it
// is locked, just send a status query.

void IEBPanel::setDrivePower(const QString &cmd)
{
  if (cmd.isEmpty()) return;

  QString cmdStr = cmd; // working copy

  if (lockPower) { 
    emit statusMsg(tr("MODS%1 IEB %2 Motor Drive Power Control is LOCKED").arg(modsID+1).arg(iebID),
		   Qt::red);
    QString titleStr = tr("MODS%1 IEB %2 Power Controls are LOCKED").arg(modsID+1).arg(iebID);
    QString msgStr = tr("The MODS%1 IEB %2 Power Controls are currently locked out.\n\n"
                        "This is a safety feature to prevent you from accidentally turning off all of"
                        " the stepper motor drive power to the mechanisms controlled by this"
                        " IEB unit.  Accidental power cycling of an active instrument could"
			" result in loss of data and cost many minutes of lost time recovering"
			" full instrument function.\n"
                        "\nUnlock the power controls at right if this is really what you"
                        " want to do.\n").arg(modsID+1).arg(iebID);
    QMessageBox::information(this,titleStr,msgStr,QMessageBox::Ok);    
    cmdStr = QString("ieb %1 status").arg(iebID);
  }
  emit statusMsg(cmdStr,Qt::blue);
  emit cmdReady(modsIEHost[modsID],cmdStr);

}
