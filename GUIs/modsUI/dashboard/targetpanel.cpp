#include <QtWidgets>
#include <QtNetwork>
#include <math.h>

#include "targetpanel.h"

#include "catviewer.h"
#include "selectentry.h"
#include "textentry.h"
#include "numberentry.h"
#include "actionbutton.h"

#include "isisclient.h"  // needed for msgType codes


TargetPanel::TargetPanel(const int &myMODS, QWidget *parent)
  : QWidget(parent)
{

  modsID = myMODS;

  QString titleStr;
  if (modsSide[modsID].compare("left",Qt::CaseInsensitive)==0) 
    titleStr = tr("Telescope Preset - Left Side");
  else
    titleStr = tr("Telescope Preset - Right Side");

  //----------------------------------------------------------------
  //
  // Build the Target Entry Panel GUI
  //

  // Target, Guide Star, and Rotator in a form layout

  QFormLayout *presetFormLayout = new QFormLayout;
  presetFormLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
  presetFormLayout->setFormAlignment(Qt::AlignCenter);  
  presetFormLayout->setLabelAlignment(Qt::AlignRight);

  // Target Entry

  targNameEntry = new TextEntry("","",0,this);
  connect(targNameEntry,SIGNAL(textReady(const QString &)),
          this,SLOT(setTargName(const QString &)));

  targRAEntry = new TextEntry("RA","",0,this);
  connect(targRAEntry,SIGNAL(textReady(const QString &)),
          this,SLOT(setTargRA(const QString &)));

  targDecEntry = new TextEntry("Dec","",0,this);
  connect(targDecEntry,SIGNAL(textReady(const QString &)),
          this,SLOT(setTargDec(const QString &)));

  // Guide Star Entry

  guiNameEntry = new TextEntry("","",0,this);
  connect(guiNameEntry,SIGNAL(textReady(const QString &)),
          this,SLOT(setGuiName(const QString &)));

  guiRAEntry = new TextEntry("RA","",0,this);
  connect(guiRAEntry,SIGNAL(textReady(const QString &)),
          this,SLOT(setGuiRA(const QString &)));

  guiDecEntry = new TextEntry("Dec","",0,this);
  connect(guiDecEntry,SIGNAL(textReady(const QString &)),
          this,SLOT(setGuiDec(const QString &)));

  // Instrument Rotator and preset button

  rotPAEntry = new NumberEntry("","deg",6,
			       MODS_MIN_ROTPA,MODS_MAX_ROTPA,
			       2,this);
  connect(rotPAEntry,SIGNAL(dataReady(const double &)),
	  this,SLOT(setRotPA(const double &)));

  rotModeEntry = new SelectEntry("","",this);
  rotModeEntry->addItem("Position","POSITION");
  rotModeEntry->addItem("Parallactic","PARALLACTIC");
  // rotModeEntry->addItem("Idle","IDLE");
  // rotModeEntry->addItem("Native","NATIVE"); // not yet implemented at LBT
  connect(rotModeEntry,SIGNAL(dataReady(const QString &)),
          this,SLOT(setRotMode(const QString &)));
  rotMode = "POSITION";
  rotModeEntry->setCurrentIndex(0);

  presetModeEntry = new SelectEntry("Mode:","",this);
  presetModeEntry->addItem("Acquire","ACQUIRE");
  presetModeEntry->addItem("Track","TRACK");
  presetModeEntry->addItem("Guide","GUIDE");
  presetModeEntry->addItem("Active","ACTIVE");
  // presetModeEntry->addItem("Static","STATIC"); // not yet implemented at LBT
  connect(presetModeEntry,SIGNAL(dataReady(const QString &)),
          this,SLOT(setPresetMode(const QString &)));
  preMode = "ACQUIRE";
  presetModeEntry->setCurrentIndex(0);

  // Catalog viewer dialog

  catDialog = new CatalogViewer(tr("Target and Guide Star Catalog"),this);
  connect(catDialog,SIGNAL(newTarget(const QString &)),
	  this,SLOT(setCatalog(const QString &)));

  // Buttons: 

  presetButton = new ActionButton(tr("Send Preset"),this);
  connect(presetButton, SIGNAL(clicked()),
          this, SLOT(sendPreset()));
  int hMax = (int)(0.9*presetButton->minimumSizeHint().height());
  presetButton->setMaximumHeight(hMax);

  clrPresetButton = new ActionButton(tr("Clear"));
  connect(clrPresetButton, SIGNAL(clicked()), this, SLOT(clearPreset()));
  clrPresetButton->setMaximumHeight(hMax);
  int mbw = (int)(clrPresetButton->minimumSizeHint().width());

  catalogButton = new ActionButton(tr("Catalog"),this);
  connect(catalogButton, SIGNAL(clicked()), this, SLOT(launchCatalogDialog()));
  catalogButton->setMaximumHeight(hMax);
  mbw = qMax((int)(catalogButton->minimumSizeHint().width()),mbw);

  clrPresetButton->setMinimumWidth(mbw);
  catalogButton->setMinimumWidth(mbw);

  // Telescope Preset Layout
  
  QHBoxLayout *targLayout = new QHBoxLayout;
  targLayout->addWidget(targNameEntry);
  targLayout->addWidget(targRAEntry);
  targLayout->addWidget(targDecEntry);
  targLayout->addWidget(catalogButton);
  presetFormLayout->addRow(tr("Target:"),targLayout);

  QHBoxLayout *guiLayout = new QHBoxLayout;
  guiLayout->addWidget(guiNameEntry);
  guiLayout->addWidget(guiRAEntry);
  guiLayout->addWidget(guiDecEntry);
  guiLayout->addWidget(clrPresetButton);
  presetFormLayout->addRow(tr("GuideStar:"),guiLayout);

  QHBoxLayout *rotPreLayout = new QHBoxLayout;
  rotPreLayout->addWidget(rotPAEntry);
  rotPreLayout->addWidget(rotModeEntry);
  rotPreLayout->addStretch();
  rotPreLayout->addWidget(presetModeEntry);
  rotPreLayout->addStretch();
  rotPreLayout->addWidget(presetButton);
  presetFormLayout->addRow(tr("Rotator:"),rotPreLayout);

  QVBoxLayout *presetLayout = new QVBoxLayout;
  presetLayout->addLayout(presetFormLayout);

  QGroupBox *presetGroupBox = new QGroupBox(titleStr);
  presetGroupBox->setContentsMargins(0,14,0,0);
  presetGroupBox->setLayout(presetLayout);

  // Offset Pointing Control Panel

  QFormLayout *offsetFormLayout = new QFormLayout;
  offsetFormLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
  offsetFormLayout->setFormAlignment(Qt::AlignCenter);  
  offsetFormLayout->setLabelAlignment(Qt::AlignRight);

  offCoordsEntry = new SelectEntry("","",this);
  offCoordsEntry->addItem("DETXY","DETXY");
  offCoordsEntry->addItem("RADec","RADEC");
  connect(offCoordsEntry,SIGNAL(dataReady(const QString &)),
          this,SLOT(setOffCoords(const QString &)));

  offModeEntry = new SelectEntry("MoveType:","",this);
  offModeEntry->addItem("Relative","REL");
  offModeEntry->addItem("Absolute","ABS");
  connect(offModeEntry,SIGNAL(dataReady(const QString &)),
          this,SLOT(setOffMode(const QString &)));
  offMode = "REL";
  offModeEntry->setCurrentIndex(0);
  // offModeEntry->setMaxSelectorWidth(2);

  // Offset Panel Buttons

  clrOffsetButton = new ActionButton(tr("Clear"),this);
  connect(clrOffsetButton, SIGNAL(clicked()), this, SLOT(clearOffset()));
  clrOffsetButton->setMaximumHeight(hMax);
  mbw = (int)(clrOffsetButton->minimumSizeHint().width());
  
  absorbButton = new ActionButton(tr("Absorb"),this);
  connect(absorbButton, SIGNAL(clicked()), this, SLOT(absorbOffset()));
  absorbButton->setMaximumHeight(hMax);
  mbw = qMax((int)(absorbButton->minimumSizeHint().width()),mbw);
 
  offsetButton = new ActionButton(tr("Send Offset"),this);
  connect(offsetButton, SIGNAL(clicked()), this, SLOT(sendOffset()));
  offsetButton->setMaximumHeight(hMax);

  clrOffsetButton->setMinimumWidth(mbw);
  absorbButton->setMinimumWidth(mbw);

  // Offset Setup Layout

  QHBoxLayout *offSetupLayout = new QHBoxLayout;
  offSetupLayout->addWidget(offCoordsEntry);
  offSetupLayout->addStretch();
  offSetupLayout->addWidget(offModeEntry);
  offSetupLayout->addStretch();
  offSetupLayout->addWidget(clrOffsetButton);
  offSetupLayout->addStretch();
  offSetupLayout->addWidget(absorbButton);
  offsetFormLayout->addRow(tr("CoordSys:"),offSetupLayout);

  // Offset Entry

  offRAEntry = new NumberEntry("RA","\"",6,0,0,2,this);
  connect(offRAEntry,SIGNAL(dataReady(const double &)),
	  this,SLOT(setOffX(const double &)));
  
  offDecEntry = new NumberEntry("Dec","\"",6,0,0,2,this);
  connect(offDecEntry,SIGNAL(dataReady(const double &)),
	  this,SLOT(setOffY(const double &)));
  
  offPAEntry = new NumberEntry("PA","deg",6,0,0,2,this);
  connect(offPAEntry,SIGNAL(dataReady(const double &)),
	  this,SLOT(setOffPA(const double &)));

  QHBoxLayout *offCoordsLayout = new QHBoxLayout;
  offCoordsLayout->addWidget(offRAEntry);
  offCoordsLayout->addWidget(offDecEntry);
  offCoordsLayout->addWidget(offPAEntry);
  offCoordsLayout->addStretch();
  offCoordsLayout->addWidget(offsetButton);
  offsetFormLayout->addRow(tr("Offset:"),offCoordsLayout);

  // Offset layout

  QVBoxLayout *offsetLayout = new QVBoxLayout;
  offsetLayout->addLayout(offsetFormLayout);

  QGroupBox *offsetGroupBox = new QGroupBox(tr("Offset Pointing"));
  offsetGroupBox->setContentsMargins(0,14,0,0);
  offsetGroupBox->setLayout(offsetLayout);

  //----------------------------------------------------------------

  // Main widget layout

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(presetGroupBox);
  mainLayout->addWidget(offsetGroupBox);
  mainLayout->setContentsMargins(0,0,0,0);
  setLayout(mainLayout);

  // Basic initializations

  setOffCoords("DETXY");
  offCoordsEntry->setCurrentIndex(0);
  setOffset(0.0,0.0,0.0,"DETXY","REL");
  setRotator(0.0,"POSITION");
  presetPending = false;
  offsetPending = false;
  presetButton->setNormal();
  offsetButton->setNormal();
}

//
// sendCommand() - Send a command to the ISIS system. 
//
// Emit the cmdReady() signal to whoever is listening for commands to
// pass along.  Most commands are just passed along as-is, but a few
// require additional actions, usually following the base command.
// Note that the decision to enqueue commands is decided by the
// parent, not here.
//

void TargetPanel::sendCommand(const QString &cmd)
{
  if (cmd.isEmpty()) return;

  // See what command we've been sent, this allows for
  // additional processing at need in the future

  QString cmdStr = cmd;
  QString cmdWord = cmdStr.section(" ",0,0);

  emit cmdReady(modsTCHost[modsID],cmdStr);

}

// sendCmdWait() - Send a command with a timeout wait interval if
// timeout>0, queue on the command sequencer if timeout<0, or just
// send-and-forget if timeout=0.

void TargetPanel::sendCmdWait(const QString &cmdStr, const int &timeout)
{
  if (timeout == 0)
    emit cmdReady(modsTCHost[modsID],cmdStr);
  else
    emit cmdWait(modsTCHost[modsID],cmdStr,timeout);
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

void TargetPanel::parse(const QString &cmdStr, 
			const int &msgType, 
			const QHash<QString,QString> &keyDict)
{
  bool cmdDone = false;   // Flag: message is a command completion
  bool cmdFault = false;  // Flag: command completed with a fault
  //  bool ok;

  QString msgStr = cmdStr;
  msgStr = msgStr.simplified(); // TODO: Evaluate this line. Why is it here? 

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

      // Target object coordinates

      if (keyStr.compare("OBJNAME",Qt::CaseInsensitive)==0) {
	targName = valStr;
	targNameEntry->setText(targName);
      }

      // Guide object coordinates

      else if (keyStr.compare("GUINAME",Qt::CaseInsensitive)==0) {
	guiName = valStr;
	guiNameEntry->setText(guiName);
      }

      // Authorization variables, so we know who is authorized where

      else if (keyStr.compare("LEFTINST",Qt::CaseInsensitive)==0) {
	leftInstID = valStr;
      }

      else if (keyStr.compare("LEFTFOC",Qt::CaseInsensitive)==0) {
	leftFocStation = valStr;
      }

      else if (keyStr.compare("RIGHTINST",Qt::CaseInsensitive)==0) {
	rightInstID = valStr;
      }

      else if (keyStr.compare("RIGHTFOC",Qt::CaseInsensitive)==0) {
	rightFocStation = valStr;
      }

    }
  }

  // Process command completion and/or fault conditions on selected commands

  if (cmdWord.compare("PRESET",Qt::CaseInsensitive)==0) {
    if (cmdDone) {
      presetPending = false;
      if (cmdFault)
	presetButton->setFault();
      else
	presetButton->setNormal();

      // Last post-preset step: where is the guide probe?

      emit cmdReady(modsIEHost[modsID],"gprobe");

    }
  }
  else if (cmdWord.contains("OFFSET",Qt::CaseInsensitive)) {
    if (cmdDone) {
      offsetPending = false;
      if (cmdFault)
	offsetButton->setFault();
      else
	offsetButton->setNormal();

      // Last post-offset step: where is the guide probe?

      emit cmdReady(modsIEHost[modsID],"gprobe");

    }
  }

  // Other things to do?  Do them here...

}

// Parse TCS commands and extract display info as needed. 
//
// Normally TCS commands are sent direct to the lbttcs agent, but we
// often need to extract information from the command arguments to
// update the widget display.  This public slot serves that purpose.

void TargetPanel::cmdParse(const QString &cmdStr)
{
  bool ok;
  double dtmp;

  if (cmdStr.isEmpty()) return;

  // Pick apart the command string into components
  
  QStringList argList = cmdStr.simplified().split(" ");

  // We only extract info from a limited subset of possible TCS commands

  QString cmdWord = argList.at(0);

  // These directives do not require arguments

  // ClearStars - clear target, guide, and offset coordinates

  if (cmdWord.compare("clearstars",Qt::CaseInsensitive)==0) {
    clearPreset();
    clearOffset();
  }

  // These directives require arguments

  if (argList.count() < 2) return;

  if (cmdWord.compare("objname",Qt::CaseInsensitive)==0) {
    targName = argList.at(1);
    targNameEntry->setText(targName);
  }

  else if (cmdWord.compare("guiname",Qt::CaseInsensitive)==0) {
    guiName = argList.at(1);
    guiNameEntry->setText(guiName);
  }

  else if (cmdWord.compare("objcoords",Qt::CaseInsensitive)==0) {
    if (argList.count() > 2) {
      targRA = argList.at(1).simplified().replace(" ",":"); // remove whitespace and insert : if needed
      targRAEntry->setText(targRA);
      targDec = argList.at(2).simplified().replace(" ",":"); // remove whitespace and insert : if needed
      targDecEntry->setText(targDec);
      targPMRA = 0.0;
      targPMDec = 0.0;
      targEpoch = 2000.0;
      if (argList.count() == 6) {
	dtmp = argList.at(3).toDouble(&ok);
	if (ok) targPMRA = dtmp;
	dtmp = argList.at(4).toDouble(&ok);
	if (ok) targPMDec = dtmp;
	dtmp = argList.at(5).toDouble(&ok); 
	if (ok) targEpoch = dtmp;
      }
    }
  }

  else if (cmdWord.compare("guicoords",Qt::CaseInsensitive)==0) {
    if (argList.count() > 2) {
      guiRA = argList.at(1).simplified().replace(" ",":"); // remove whitespace and insert : if needed
      guiRAEntry->setText(guiRA);
      guiDec = argList.at(2).simplified().replace(" ",":"); // remove whitespace and insert : if needed
      guiDecEntry->setText(guiDec);
      guiPMRA = 0.0;
      guiPMDec = 0.0;
      guiEpoch = 2000.0;
      guiMag = 0.0;
      guiColor = 0.0;
      if (argList.count() > 5 ) {
	dtmp = argList.at(3).toDouble(&ok);
	if (ok) guiPMRA = dtmp;
	dtmp = argList.at(4).toDouble(&ok);
	if (ok) guiPMDec = dtmp;
	dtmp = argList.at(5).toDouble(&ok); 
	if (ok) guiEpoch = dtmp;
	if (argList.count() == 8) {
	  dtmp = argList.at(6).toDouble(&ok); 
	  if (ok) guiMag = dtmp;
	  dtmp = argList.at(6).toDouble(&ok); 
	  if (ok) guiColor = dtmp;
	}
      }
    }
  }

  else if (cmdWord.compare("rotator",Qt::CaseInsensitive)==0) {
    if (argList.count() == 3) {
      dtmp = argList.at(1).toDouble(&ok);
      if (!ok) return;
      setRotator(dtmp,argList.at(2).toUpper());
    }
  }

  // offset dRA dDec [abs|rel]  Alias: skyoffset

  else if (cmdWord.compare("skyoffset",Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("offset",Qt::CaseInsensitive)==0) {
    if (argList.count() == 4) {
      dtmp = argList.at(1).toDouble(&ok); 
      if (ok) dX = dtmp;
      dtmp = argList.at(2).toDouble(&ok); 
      if (ok) dY = dtmp;
      setOffset(dX,dY,0.0,"RADEC",argList.at(3).toUpper());
      offsetPending = true;
      offsetButton->setChange();
    }
  }

  // offsetxy dX dY [abs|rel]  Alias: slitoffset

  else if (cmdWord.compare("slitoffset",Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("offsetxy",Qt::CaseInsensitive)==0) {
    if (argList.count() == 4) {
      dtmp = argList.at(1).toDouble(&ok); 
      if (ok) dX = dtmp;
      dtmp = argList.at(2).toDouble(&ok); 
      if (ok) dY = dtmp;
      setOffset(dX,dY,dPA,"DETXY",argList.at(3).toUpper());
      offsetPending = true;
      offsetButton->setChange();
    }
  }

  // rotoffset dPA 

  else if (cmdWord.compare("rotoffset",Qt::CaseInsensitive)==0) {
    if (argList.count() == 2) {
      dtmp = argList.at(1).toDouble(&ok); 
      if (ok) dPA = dtmp;
      setOffset(0.0,0.0,dPA,"DETXY","REL");
      offsetPending = true;
      offsetButton->setChange();
    }
  }

  // offsetPointing dPA dRA dDec coordSys offMode
  // test was 6, changed to  6 or 7

  else if (cmdWord.compare("offsetpointing",Qt::CaseInsensitive)==0) {
    if (argList.count() == 6 || argList.count() == 7) {
      dtmp = argList.at(1).toDouble(&ok); 
      if (ok) dPA = dtmp;
      dtmp = argList.at(2).toDouble(&ok); 
      if (ok) dX = dtmp;
      dtmp = argList.at(3).toDouble(&ok); 
      if (ok) dY = dtmp;
      setOffset(dX,dY,dPA,argList.at(4).toUpper(),argList.at(5).toUpper());
      offsetPending = true;
      offsetButton->setChange();
    }
  }

  // Send a preset

  else if (cmdWord.compare("preset",Qt::CaseInsensitive)==0) {
    if (argList.count() == 2) setPreset(argList.at(1).toUpper());
    presetPending = true;
    presetButton->setChange();
  }

  // Set the preset mode, but don't send the preset 

  else if (cmdWord.compare("presetmode",Qt::CaseInsensitive)==0) {
    if (argList.count() == 2) 
      setPreset(argList.at(1).toUpper());
  }

}


// Some special methods

// Launch the Target Catalog dialog

void TargetPanel::launchCatalogDialog()
{
  if (catDialog->isHidden())
    catDialog->show();
  else
    catDialog->hide();
  catDialog->activateWindow();
}

// Set target/guide-star entries from a catalog entry.  This uses the
// addresses for the 17-column format. Can you say "kludge" boys and
// girls? I knew you could.

void TargetPanel::setCatalog(const QString &catLine)
{
  if (catLine.isEmpty()) return;
  QStringList bits = catLine.split("|");
  if (bits.size() == MODS_CATALOG_NCOLS) {
    // Target Name, RA, Dec, pmRA, pmDec, and Epoch (equinox is J2000 always)
    setTarget(bits.at(0),bits.at(1),bits.at(2),bits.at(3),bits.at(4),bits.at(6));
    // Observation rotator PA
    bool ok;
    double dtmp = bits.at(7).toDouble(&ok);
    if (ok) setRotatorPA(dtmp);
    // GS Name, RA, Dec, pmRA, pmDec, Epoch, mag, and color (equinox also J2000)
    setGuideStar(bits.at(8),bits.at(9),bits.at(10),bits.at(11),bits.at(12),
		 bits.at(14),bits.at(15),bits.at(16));
  }
}

// Reset the telescope preset and offset control panels and associated state
// variables.  Uses the internal private methods as needed.

void TargetPanel::reset()
{
  clearPreset();
  clearOffset();
}

// Parameter entry methods and slots

// Set the preset type

void TargetPanel::setPresetMode(const QString &ptype)
{
  preMode = ptype;
}

// Set the name of the target

void TargetPanel::setTargName(const QString &reqTargName)
{
  targName = reqTargName;
  targNameEntry->setNormal();
}

// Set the RA of the target

void TargetPanel::setTargRA(const QString &reqTargRA)
{
  targRA = reqTargRA;
  targRAEntry->setNormal();
}

// Set the Dec of the target

void TargetPanel::setTargDec(const QString &reqTargDec)
{
  targDec = reqTargDec;
  targDecEntry->setNormal();
}

// Set the name of the guide star

void TargetPanel::setGuiName(const QString &reqGuiName)
{
  guiName = reqGuiName;
  guiNameEntry->setNormal();
}

// Set the RA of the guide star

void TargetPanel::setGuiRA(const QString &reqGuiRA)
{
  guiRA = reqGuiRA;
  guiRAEntry->setNormal();
}

// Set the Dec of the guide star

void TargetPanel::setGuiDec(const QString &reqGuiDec)
{
  guiDec = reqGuiDec;
  guiDecEntry->setNormal();
}

// Set the rotator mode

void TargetPanel::setRotMode(const QString &reqRotMode)
{
  rotMode = reqRotMode;
}

// Set the rotator PA requested in mode

void TargetPanel::setRotPA(const double &reqRotPA)
{
  rotPA = reqRotPA;
  rotPAEntry->setNormal();
}

// Set the X/RA offset

void TargetPanel::setOffX(const double &reqDRA)
{
  dX = reqDRA;
  offRAEntry->setNormal();
}

// Set the Y/Dec offset

void TargetPanel::setOffY(const double &reqDDec)
{
  dY = reqDDec;
  offDecEntry->setNormal();
}

// Set the Rotator PA offset

void TargetPanel::setOffPA(const double &reqDPA)
{
  dPA = reqDPA;
  offPAEntry->setNormal();
}

// Set the Offset type

void TargetPanel::setOffMode(const QString &reqOffMode)
{
  offMode = reqOffMode;
}

// Set the Offset coordinate system

void TargetPanel::setOffCoords(const QString &reqCoords)
{
  offCoords = reqCoords;
  if (offCoords.compare("RADEC")==0) {
    offRAEntry->setLabelText(QString("%1%2").arg(QChar(0x0394)).arg(QChar(0x03B1)));
    offDecEntry->setLabelText(QString("%1%2").arg(QChar(0x0394)).arg(QChar(0x03B4)));
  }
  else if (offCoords.compare("DETXY")==0) {
    offRAEntry->setLabelText(QString("%1X").arg(QChar(0x0394)));
    offDecEntry->setLabelText(QString("%1Y").arg(QChar(0x0394)));
  }
}

// Action Button slots

// Execute a preset.  This sends commands to the lbttcs agent in its
// command syntax, which then takes care of executing the preset and
// doing all the blocking and error checking for us.

void TargetPanel::sendPreset()
{
  QString cmdStr;
  presetPending = false;
  presetButton->setNormal();

  // Send the target name and coords

  if (targName.isEmpty())
    cmdStr = QString("OBJNAME None");
  else
    cmdStr = QString("OBJNAME %1").arg(targName);
  sendCommand(cmdStr);

  if (targRA.isEmpty() || targDec.isEmpty()) {
    // cannot send a preset without target coordinates
    return;
  }
  else {
    cmdStr = QString("OBJCOORDS %1 %2 %3 %4 %5").arg(targRA).arg(targDec).arg(targPMRA).arg(targPMDec).arg(targEpoch);
    sendCommand(cmdStr);
  }

  // Send the guide star name and coords

  if (guiName.isEmpty())
    cmdStr = QString("GUINAME None");
  else
    cmdStr = QString("GUINAME %1").arg(guiName);
  sendCommand(cmdStr);

  if (guiRA.isEmpty() || guiDec.isEmpty()) {
    // cannot send a preset without guide star coordinates??
    return;
  }
  else {
    cmdStr = QString("GUICOORDS %1 %2 %3 %4 %5 %6").arg(guiRA).arg(guiDec).arg(guiPMRA).arg(guiPMDec).arg(guiEpoch).arg(guiMag);
    sendCommand(cmdStr);
  }

  // Set the rotator request

  cmdStr = QString("ROTATOR %1 %2").arg(rotPA).arg(rotMode);
  sendCommand(cmdStr);

  // Execute the preset

  cmdStr = QString("PRESET %1 %2").arg(preMode).arg(modsSide[modsID]);
  sendCommand(cmdStr);
  presetPending = true;
  presetButton->setChange();

  emit logMessage(tr("%1 Preset Sent...").arg(preMode));

}

// Execute an offset.  The command syntax is that of the lbttcs agent.

void TargetPanel::sendOffset()
{
  QString cmdWord;
  offsetPending = false;
  offsetButton->setNormal();
  if (offCoords.compare("RADEC")==0)
    cmdWord = "OFFSET";
  else if (offCoords.compare("DETXY")==0)
    cmdWord = "OFFSETXY";
  else
    return;

  // This sends an offsetpointing with the dPA, dX, dY, coords, mode, and side as arguments

  QString cmdStr = QString("offsetpointing %1 %2 %3 %4 %5 %6").arg(dPA).arg(dX).arg(dY).arg(offCoords).arg(offMode).arg(modsSide[modsID]);

  //  QString cmdStr = QString("%1 %2 %3 %4 %5 %6").arg(cmdWord).arg(dX).arg(dY).arg(dPA).arg(offMode).arg(modsSide[modsID]);
  // QString cmdStr = QString("%1 %2 %3 %4 %5").arg(cmdWord).arg(dX).arg(dY).arg(offMode).arg(modsSide[modsID]);
  sendCommand(cmdStr);
  offsetPending = true;
  offsetButton->setChange();

  emit logMessage(tr("%1 Offset Sent...").arg(offCoords));

}

// Public methods that provide external access to the entries

// Set/get the target ID and coordinates.  Allow for the case of the
// catalog containing spaces instead of : as the separator in
// sexagesimal coordinates.

void TargetPanel::setTarget(const QString &tID, const QString &tRA, 
			    const QString &tDec, const QString &pmRA,
			    const QString &pmDec, const QString &epoch)
{
  double dtmp;
  bool ok;

  targName = tID.simplified();
  targNameEntry->setText(targName);
  targRA = tRA.simplified().replace(" ",":"); // remove whitespace and insert : if needed
  targRAEntry->setText(targRA);
  targDec = tDec.simplified().replace(" ",":"); // remove whitespace and insert : if needed
  targDecEntry->setText(targDec);
  targPMRA = 0.0;
  targPMDec = 0.0;
  targEpoch = 2000.0;
  dtmp = pmRA.toDouble(&ok);
  if (ok) targPMRA = dtmp;
  dtmp = pmDec.toDouble(&ok);
  if (ok) targPMDec = dtmp;
  dtmp = epoch.toDouble(&ok); 
  if (ok) targEpoch = dtmp;
}

void TargetPanel::setTargetName(const QString &tID)
{
  targName = tID;
  targNameEntry->setText(targName);
}

QString TargetPanel::targetName()
{
  return targName;
}

void TargetPanel::setTargetRA(const QString &tRA)
{
  targRA = tRA;
  targRAEntry->setText(targRA);
}

QString TargetPanel::targetRA()
{
  return targRA;
}

void TargetPanel::setTargetDec(const QString &tDec)
{
  targDec = tDec;
  targDecEntry->setText(targDec);
}

QString TargetPanel::targetDec()
{
  return targDec;
}

// Set the target proper motion (RA,Dec) in mas/yr and the epoch in decimal years

void TargetPanel::setTargetPM(const double &pmRA, const double &pmDec, const double &epoch)
{
  targPMRA = pmRA;
  targPMDec = pmDec;
  targEpoch = epoch;
}

// Set/get the guide star ID and coordinates.  Convert spaces into colons in
// sexagesimal coordinates.

void TargetPanel::setGuideStar(const QString &gsID, const QString &gsRA,
			       const QString &gsDec, const QString &pmRA,
			       const QString &pmDec, const QString &epoch,
			       const QString &mag, const QString &color)
{
  double dtmp;
  bool ok;

  guiName = gsID.simplified();
  guiNameEntry->setText(guiName);
  guiRA = gsRA.simplified().replace(" ",":"); // remove whitespace and insert : if needed
  guiRAEntry->setText(guiRA);
  guiDec = gsDec.simplified().replace(" ",":"); // remove whitespace and insert : if needed
  guiDecEntry->setText(guiDec);
  guiPMRA = 0.0;
  guiPMDec = 0.0;
  guiEpoch = 2000.0;
  guiMag = 0.0;
  guiColor = 0.0;

  dtmp = pmRA.toDouble(&ok);
  if (ok) guiPMRA = dtmp;
  dtmp = pmDec.toDouble(&ok);
  if (ok) guiPMDec = dtmp;
  dtmp = epoch.toDouble(&ok);
  if (ok) guiEpoch = dtmp;
  dtmp = mag.toDouble(&ok);
  if (ok) guiMag = dtmp;
  dtmp = color.toDouble(&ok);
  if (ok) guiColor = dtmp;
}

void TargetPanel::setGuideName(const QString &tID)
{
  guiName = tID;
  guiNameEntry->setText(guiName);
}

QString TargetPanel::guideName()
{
  return guiName;
}

void TargetPanel::setGuideRA(const QString &tRA)
{
  guiRA = tRA;
  guiRAEntry->setText(guiRA);
}

QString TargetPanel::guideRA()
{
  return guiRA;
}

void TargetPanel::setGuideDec(const QString &tDec)
{
  guiDec = tDec;
  guiDecEntry->setText(guiDec);
}

QString TargetPanel::guideDec()
{
  return guiDec;
}

// Set the guide star proper motion (RA,Dec) in mas/yr and the epoch in decimal years

void TargetPanel::setGuidePM(const double &pmRA, const double &pmDec, const double &epoch)
{
  guiPMRA = pmRA;
  guiPMDec = pmDec;
  guiEpoch = epoch;
}

// Set the guide star photometry info (magnitude and color)

void TargetPanel::setGuideMag(const double &mag, const double &color)
{
  guiMag = mag;
  guiColor = color;
}

// Set the instrument rotator parameters

void TargetPanel::setRotator(const double &rPA, const QString &rMode)
{
  rotPA = rPA;
  rotPAEntry->setValue(rotPA);
  rotPAEntry->setNormal();
  rotMode = rMode;
  int ind = rotModeEntry->findText(rotMode);
  if (ind >= 0) 
    rotModeEntry->setCurrentIndex(ind);
}

void TargetPanel::setRotatorPA(const double &rPA)
{
  rotPA = rPA;
  rotPAEntry->setValue(rotPA);
  rotPAEntry->setNormal();
}

double TargetPanel::rotatorPA()
{
  return rotPA;
}

void TargetPanel::setRotatorMode(const QString &rMode)
{
  rotMode = rMode;
  int ind = rotModeEntry->findText(rotMode);
  if (ind >= 0) 
    rotModeEntry->setCurrentIndex(ind);
  else {
    rotModeEntry->setCurrentIndex(0);
    rotMode = "POSITION";
  }
}

QString TargetPanel::rotatorMode()
{
  return rotMode;
}

// Set/Get the preset type to execute

void TargetPanel::setPreset(const QString &reqPM)
{
  preMode = reqPM;
  int ind = presetModeEntry->findText(preMode);
  if (ind >= 0) 
    presetModeEntry->setCurrentIndex(ind);
}

QString TargetPanel::presetMode()
{
  return preMode;
}

// Set/get the offset parameters

void TargetPanel::setOffset(const double &rRA, const double &rDec, 
			    const double &rRot, const QString &rCoords, 
			    const QString &rMode)
{
  //int ind;
  dX = rRA;
  offRAEntry->setValue(dX);
  dY = rDec;
  offDecEntry->setValue(dY);
  dPA = rRot;
  offPAEntry->setValue(dPA);
  setOffsetMode(rMode);
  setOffsetCoords(rCoords);
}

void TargetPanel::setOffsetRA(const double &rRA)
{
  dX = rRA;
  offRAEntry->setValue(dX);
}

void TargetPanel::setOffsetDec(const double &rDec)
{
  dY = rDec;
  offDecEntry->setValue(dY);
}

void TargetPanel::setOffsetPA(const double &rPA)
{
  dPA = rPA;
  offPAEntry->setValue(dPA);
}

void TargetPanel::setOffsetMode(const QString &rMode)
{
  setOffMode(rMode);
  int ind = offModeEntry->findText(offMode);
  if (ind >= 0) 
    offModeEntry->setCurrentIndex(ind);
}

void TargetPanel::setOffsetCoords(const QString &rCoords)
{
  setOffCoords(rCoords);
  int ind = offCoordsEntry->findText(offCoords);
  if (ind >= 0) 
    offCoordsEntry->setCurrentIndex(ind);
}

// Clear the entries on the target form, and tell the lbttcs agent to
// clear its pointing preset entries.

void TargetPanel::clearPreset()
{
  // Clear the target info

  targName.clear();
  targNameEntry->clear();
  targRA.clear();
  targRAEntry->clear();
  targDec.clear();
  targDecEntry->clear();
  targPMRA = 0.0;
  targPMDec = 0.0;
  targEpoch = 2000.0;
  targEquinox = "J2000";

  // Clear the guide star info

  guiName.clear();
  guiNameEntry->clear();
  guiRA.clear();
  guiRAEntry->clear();
  guiDec.clear();
  guiDecEntry->clear();
  guiPMRA = 0.0;
  guiPMDec = 0.0;
  guiMag = 0.0;
  guiColor = 0.0;
  guiEquinox = "J2000";
  guiEpoch = 2000.0;

  // Clear the Rotator Setup

  setRotator(0.0,"POSITION");

  // Send the ClearStars command

  QString cmdStr = QString("ClearStars");
  sendCommand(cmdStr);

  emit logMessage(tr("ClearStars Sent..."));  

  // Clear any delinquent pending preset states

  presetPending = false;
  presetButton->setNormal();

  return;
} 

// Clear the offset data and clear any delinquent pending offset states

void TargetPanel::clearOffset()
{
  setOffset(0.0,0.0,0.0,"DETXY","REL");
  offsetPending = false;
  offsetButton->setNormal();
}

// Send the UpdatePointing directive - we absorb DETXY offsets since
// RADEC absoption should be rare but still accessible via the direct
// command option

void TargetPanel::absorbOffset()
{
  QString cmdStr = QString("UpdatePointing DETXY %1").arg(modsSide[modsID]);
  sendCommand(cmdStr);
  
  emit logMessage(tr("UpdatePointingReference[%1] Sent...").arg(modsSide[modsID]));  
  return;
}

