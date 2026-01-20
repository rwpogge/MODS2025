#include <QtWidgets>
#include <QtNetwork>
#include <QStyleFactory>
#include <math.h>

#include "modschannel.h"

#include "bistatemechanism.h"
#include "indexedmechanism.h"
#include "linearmechanism.h"

#include "bistatecontrol.h"
#include "indexedselector.h"
#include "linearcontrol.h"
#include "toggleswitch.h"

#include "doubleentry.h"
#include "textentry.h"
#include "intentry.h"
#include "selectentry.h"
#include "texthistentry.h"
#include "numberentry.h"
#include "textdisplay.h"
#include "valuedisplay.h"
#include "lightbutton.h"
#include "actionbutton.h"
#include "led.h"

#include "isisclient.h" // needed for msgType codes

MODSChannel::MODSChannel(const int &myMODS, const int &myChannel, QWidget *parent)
  : QWidget(parent)
{
  modsID = myMODS;

  // Which channel is this?

  switch (myChannel) {
  case MODSChannel::Blue:
    channelID = "B";
    channelName = "Blue";
    icHostID = modsBCHost[modsID];
    dmHostID = modsBDHost[modsID];
    break;

  case MODSChannel::Red:
    channelID = "R";
    channelName = "Red";
    icHostID = modsRCHost[modsID];
    dmHostID = modsRDHost[modsID];
    break;
    
  default:
    channelID = "X";
    channelName = "Unknown";
    break;
  }

  // Internal Tables

  grtiltTable[0] = new QStringList;
  gratingTilt[0] = 0;
  grtiltTable[1] = new QStringList;
  gratingTilt[1] = 0;

  // Channel Mechanism Layout

  QFormLayout *chFormLayout = new QFormLayout;
  chFormLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
  chFormLayout->setFormAlignment(Qt::AlignCenter);  
  chFormLayout->setLabelAlignment(Qt::AlignRight);

  // Instrument Mode Setup: 4 buttons: Imaging Grating Prism + Clear & Commit

  // Preset instrument modes

  configTable.clear(); 
  reqConfig.clear();

  imagingButton = new ActionButton(tr("Imaging"),this);
  connect(imagingButton,SIGNAL(clicked()),this,SLOT(reqImagingMode()));
  int mbw = (int)(imagingButton->minimumSizeHint().width());

  gratingButton = new ActionButton(tr("Grating"),this);
  connect(gratingButton,SIGNAL(clicked()),this,SLOT(reqGratingMode()));
  mbw = qMax((int)(gratingButton->minimumSizeHint().width()),mbw);

  prismButton = new ActionButton(tr("Prism"),this);
  connect(prismButton,SIGNAL(clicked()),this,SLOT(reqPrismMode()));
  mbw = qMax((int)(prismButton->minimumSizeHint().width()),mbw);

  // Clear Button - Clear any uncommitted configuration requests

  clearButton = new ActionButton(tr("Clear"),this);
  connect(clearButton,SIGNAL(clicked()),this,SLOT(clearConfig()));
  mbw = qMax((int)(clearButton->minimumSizeHint().width()),mbw);

  // Commit Button - Commit the selected configuration

  commitButton = new ActionButton(tr("Commit"),this);
  connect(commitButton,SIGNAL(clicked()),this,SLOT(commitConfig()));
  mbw = qMax((int)(commitButton->minimumSizeHint().width()),mbw);

  imagingButton->setMinimumWidth(mbw);
  gratingButton->setMinimumWidth(mbw);
  prismButton->setMinimumWidth(mbw);
  commitButton->setMinimumWidth(mbw);
  clearButton->setMinimumWidth(mbw);

  QHBoxLayout *configLayout = new QHBoxLayout;
  configLayout->addWidget(imagingButton,0,Qt::AlignHCenter);
  configLayout->addWidget(gratingButton,0,Qt::AlignHCenter);
  configLayout->addWidget(prismButton,0,Qt::AlignHCenter);
  configLayout->addStretch();
  configLayout->addWidget(commitButton,0,Qt::AlignHCenter);
  configLayout->addStretch();
  configLayout->addWidget(clearButton,0,Qt::AlignHCenter);

  chFormLayout->addRow(tr("Mode:"),configLayout);

  // Grating Select and Tilt (CenLam) widgets

  // Grating Select Mechanism 

  grating = new IndexedMechanism(tr("%1 Grating Select").arg(channelName),
				 QString("%1GRATING").arg(channelID),
				 MODS_GRATING_MIN,
				 MODS_GRATING_MAX,30);

  gratingDisplay = new TextDisplay("Grating ID:","",6,this);
  gratingDisplay->setNormalColors(palette().color(QPalette::Window),Qt::blue);

  grtiltDisplay = new TextDisplay("CenLam:","",6,this);
  grtiltDisplay->setNormalColors(palette().color(QPalette::Window),Qt::blue);

  // Grating Tilt Mechanism

  grtilt = new LinearMechanism(tr("%1 Grating Tilt").arg(channelName),
			       QString("%1GRTILT").arg(channelID),
			       MODS_GRTILT_MIN,
			       MODS_GRTILT_MAX,2,120);
  // Filter Wheel

  filter = new IndexedMechanism(tr("%1 Camera Filter Wheel").arg(channelName),
				QString("%1FILTER").arg(channelID),
				MODS_CAMFILT_MIN,
				MODS_CAMFILT_MAX,
				30);
  filterControl = new IndexedSelector(filter,"",this);
  connect(filterControl,SIGNAL(cmdReady(const QString &)),
	  this,SLOT(sendIECommand(const QString &)));

  QHBoxLayout *gratingLayout = new QHBoxLayout;
  gratingLayout->addWidget(filterControl);
  gratingLayout->addStretch();
  gratingLayout->addWidget(gratingDisplay);
  gratingLayout->addStretch();
  gratingLayout->addWidget(grtiltDisplay);

  chFormLayout->addRow(tr("Filter:"),gratingLayout);

  // Camera Focus and Collimator TTFA/B/C Readouts

  camfoc = new LinearMechanism(tr("%1 Camera Focus").arg(channelName),
			       QString("%1CAMFOC").arg(channelID),
			       MODS_CAMFOC_MIN,
			       MODS_CAMFOC_MAX,0,30); 

  camfocDisplay = new TextDisplay("","um",6,this);
  camfocDisplay->setNormalColors(palette().color(QPalette::Window),Qt::blue);

  // Collimator TTFA/B/C readouts (display only), but keep the mechanisms
  // around as we need these for active control features

  colttfa = new LinearMechanism(tr("%1 Collimator TTFA").arg(channelID),
				QString("%1COLTTFA").arg(channelID),
				MODS_COLTTF_MIN,
				MODS_COLTTF_MAX,0,60);
  colttfb = new LinearMechanism(tr("%1 Collimator TTFB").arg(channelID),
				QString("%1COLTTFB").arg(channelID),
				MODS_COLTTF_MIN,
				MODS_COLTTF_MAX,0,60);
  colttfc = new LinearMechanism(tr("%1 Collimator TTFC").arg(channelID),
				QString("%1COLTTFC").arg(channelID),
				MODS_COLTTF_MIN,
				MODS_COLTTF_MAX,0,60);

  // The value displays

  ttfaDisplay = new TextDisplay("TTF A:","",6,this);
  ttfaDisplay->setNormalColors(palette().color(QPalette::Window),Qt::blue);
  ttfbDisplay = new TextDisplay("B:","",6,this);
  ttfbDisplay->setNormalColors(palette().color(QPalette::Window),Qt::blue);
  ttfcDisplay = new TextDisplay("C:","um",6,this);
  ttfcDisplay->setNormalColors(palette().color(QPalette::Window),Qt::blue);
 
  QHBoxLayout *focusLayout = new QHBoxLayout;
  focusLayout->addWidget(camfocDisplay);
  focusLayout->addStretch();
  focusLayout->addWidget(ttfaDisplay);
  focusLayout->addWidget(ttfbDisplay);
  focusLayout->addWidget(ttfcDisplay);
  chFormLayout->addRow(tr("Focus:"),focusLayout);

  // Assembly the channel configuration QGroupBox

  QGroupBox *configGroupBox = new QGroupBox(tr("Configuration"));

  configGroupBox->setLayout(chFormLayout);

  //----------------------------------------------------------------
  //
  // Exposure Control
  //

  QGroupBox *expGroupBox = new QGroupBox(tr("Exposure Control"));

  QFormLayout *expFormLayout = new QFormLayout;
  expFormLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
  expFormLayout->setFormAlignment(Qt::AlignCenter);  
  expFormLayout->setLabelAlignment(Qt::AlignRight);

  // Object name entry widgets all the way across the top row

  objNameEntry = new TextEntry("","",0,this);
  connect(objNameEntry,SIGNAL(dataReady(const QString &)),
	  this,SLOT(setObject(const QString &)));

  expFormLayout->addRow(tr("Name:"),objNameEntry);

  // Exposure time, number of exposures, and exposure type entry widgets

  expTimeEntry = new NumberEntry("ExpTime",tr("sec"),6,0.0,
				 MODS_MAX_EXPTIME,1,this);
  connect(expTimeEntry,SIGNAL(dataReady(const double &)),
	  this,SLOT(setExpTime(const double &)));
  
  numExpEntry = new NumberEntry("# Images:","",4,0,MODS_MAX_NUMEXP,1,this);
  connect(numExpEntry,SIGNAL(intReady(const int &)),
	  this,SLOT(setNumExp(const int &)));

  imgTypeEntry = new SelectEntry("","",this); // label is on the form layout
  imgTypeEntry->addItem("Object","OBJECT");
  imgTypeEntry->addItem("Bias","BIAS");
  imgTypeEntry->addItem("Flat","FLAT");
  imgTypeEntry->addItem("Comp","COMP");
  imgTypeEntry->addItem("Std","STD");
  imgTypeEntry->addItem("Sky","SKY");
  imgTypeEntry->addItem("Dark","DARK");
  connect(imgTypeEntry,SIGNAL(dataReady(const QString &)),
	  this,SLOT(setImgType(const QString &)));

  QHBoxLayout *expLayout = new QHBoxLayout;
  expLayout->addWidget(imgTypeEntry);
  expLayout->addStretch();
  expLayout->addWidget(expTimeEntry);
  expLayout->addStretch();
  expLayout->addWidget(numExpEntry);

  expFormLayout->addRow(tr("Type:"),expLayout);

  // CCD Readout Controls: Binning, Speed and ROI entry widgets

  QHBoxLayout *binLayout = new QHBoxLayout;

  // Implement X and Y binning as an indexed mechanism class

  xCCDBin = new IndexedMechanism(tr("%1 CCD X-axis Binning Factor").arg(channelName),
				 "XBIN",0,1,10);
  xBinEntry = new IndexedSelector(xCCDBin,"X:",this);
  xCCDBin->setID(0,"1");
  xCCDBin->setID(1,"2");
  //xCCDBin->setID(2,"4");
  //xCCDBin->setID(3,"8");
  connect(xBinEntry,SIGNAL(cmdReady(const QString &)),
	  this,SLOT(ccdXBin(const QString &)));
  binLayout->addWidget(xBinEntry);

  yCCDBin = new IndexedMechanism(tr("%1 CCD Y-axis Binning Factor").arg(channelName),
				 "YBIN",0,1,10);
  yBinEntry = new IndexedSelector(yCCDBin,"Y:",this);
  yCCDBin->setID(0,"1");
  yCCDBin->setID(1,"2");
  //yCCDBin->setID(2,"4");
  //yCCDBin->setID(3,"8");
  connect(yBinEntry,SIGNAL(cmdReady(const QString &)),
	  this,SLOT(ccdYBin(const QString &)));
  binLayout->addWidget(yBinEntry);
  binLayout->addStretch();

  // Region-of-Interest Readout (eventually get these from the
  // ccdroi.tab file)

  roiEntry = new SelectEntry("CCD Readout:","",this);
  roiEntry->addItem(MODS_CCD_FORMAT,"ROI OFF");
  connect(roiEntry,SIGNAL(dataReady(const QString &)),
	  this,SLOT(ccdROI(const QString &)));
  binLayout->addWidget(roiEntry);
  binLayout->addStretch();

  expFormLayout->addRow(tr("Binning"),binLayout);

  // GO/Abort and Pause/Resume Buttons - a little trickery here, we
  // set up the button minimum size hints with the largest labels
  // they'll have, then set the state at the end of the constructor.

  goButton = new ActionButton(tr("Abort"),this);
  connect(goButton, SIGNAL(clicked()), this, SLOT(doExposure()));
  goButton->setFont(QFont("Helvetica",18,QFont::Normal));

  pauseButton = new ActionButton(tr("Resume"),this);
  connect(pauseButton, SIGNAL(clicked()),
          this, SLOT(pauseExposure()));
  pauseButton->setFont(QFont("Helvetica",14,QFont::Normal));

  // Exposure STOP button

  stopButton = new ActionButton(tr("Stop"),this);
  connect(stopButton, SIGNAL(clicked()),
	  this, SLOT(stopExposure()));
  stopButton->setFont(QFont("Helvetica",14,QFont::Normal));

  // Make the GO button square and the Pause and Stop buttons
  // the same width as the GO button, but variable height.

  int gbw = (int)(pauseButton->minimumSizeHint().width());
  int gbh = (int)(pauseButton->minimumSizeHint().height());
  int gbs = qMax(gbw,gbh);
  goButton->setMinimumWidth(gbs);
  goButton->setMinimumHeight(gbs);
  pauseButton->setMinimumWidth(gbs);
  stopButton->setMinimumWidth(gbs);
  int pbh = (int)(pauseButton->minimumSizeHint().height());
  pauseButton->setMinimumHeight(pbh);
  stopButton->setMinimumHeight(pbh);
  
  // CCD Shutter and Exposure/Readout Progress Bars

  QFormLayout *progressLayout = new QFormLayout;
  progressLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
  progressLayout->setFormAlignment(Qt::AlignCenter);  
  progressLayout->setLabelAlignment(Qt::AlignRight);

  shutter = new BiStateMechanism(tr("%1 Camera Shutter").arg(channelName),
				 "SHUTTER","OPEN","CLOSE",10);
  shutterDisplay = new ValueDisplay("","",this);
  shutterDisplay->setText("Closed");
  shutterOpen = false;
  expNumDisplay = new ValueDisplay("Image:","",this);
  expNumDisplay->clear();
  expNumDisplay->setTextColor(Qt::blue);
  
  QHBoxLayout *shutterLayout = new QHBoxLayout;
  shutterLayout->addWidget(shutterDisplay);
  shutterLayout->addSpacing(5);
  shutterLayout->addWidget(expNumDisplay);
  
  progressLayout->addRow(tr("Shutter:"),shutterLayout);

  expProgress = new QProgressBar(this);
  expProgress->setFormat("%vs");
  expProgress->setStyle(QStyleFactory::create("Fusion"));
  expProgress->setMaximumHeight((int)(0.9*expProgress->minimumSizeHint().height()));
  expProgress->reset();
  progressLayout->addRow(tr("Exposure:"),expProgress);
  
  readProgress = new QProgressBar(this);
  readProgress->setFormat("%p%");
  readProgress->setStyle(QStyleFactory::create("Fusion"));
  readProgress->setMaximumHeight((int)(0.9*readProgress->minimumSizeHint().height()));
  readProgress->reset();
  readProgress->setRange(0,100);
  progressLayout->addRow(tr("Readout:"),readProgress);

  QVBoxLayout *pauseStopLayout = new QVBoxLayout;
  pauseStopLayout->addStretch();
  pauseStopLayout->addWidget(pauseButton);
  pauseStopLayout->addStretch();
  pauseStopLayout->addWidget(stopButton);
  pauseStopLayout->addStretch();

  QHBoxLayout *expButtonLayout = new QHBoxLayout;
  expButtonLayout->addWidget(goButton);
  expButtonLayout->addLayout(pauseStopLayout);
  expButtonLayout->addLayout(progressLayout);

  // IMCS Status, Enable/Disable CheckBox, and Lock-On Button

  imcsLoopDisplay = new ValueDisplay("IMCS:","",this);
  imcsLoopDisplay->setText("Idle");
  imcsLoopDisplay->setToolTip(tr("Indicates whether the IMCS error-correction loop is active or idle"));
  imcsLoopClosed = false;
  useIMCS = false;

  useIMCSCheck = new QCheckBox(tr("Use IMCS"),this);
  imcsEnabled = true;
  if (imcsEnabled)
    useIMCSCheck->setCheckState(Qt::Checked);
  else
    useIMCSCheck->setCheckState(Qt::Unchecked);
  connect(useIMCSCheck,SIGNAL(stateChanged(int)),this,SLOT(setUseIMCS()));
  useIMCSCheck->setToolTip(tr("Checking this will enable using the IMCS during exposures"));

  imcsLockButton = new ActionButton(tr("Lock-On"),this);
  connect(imcsLockButton, SIGNAL(clicked()), this, SLOT(doIMCSLock()));
  imcsLockButton->setToolTip(tr("Instructs the IMCS to center-up and lock-on the reference laser"));

  // During transition from no-lock to lock we change the button state to HiLight, which turns
  // the button SAE/ECE Amber in color.

  imcsLockButton->setHighLightColors(palette().color(QPalette::Window),QColor(255,126,0,255));

  QHBoxLayout *imcsLayout = new QHBoxLayout;
  imcsLayout->addWidget(imcsLoopDisplay);
  imcsLayout->addStretch();
  imcsLayout->addWidget(useIMCSCheck);
  imcsLayout->addStretch();
  imcsLayout->addWidget(imcsLockButton);

  // Next and Last File displays

  nextFileDisp = new TextDisplay(tr("NextFile:"),"",0,this);
  lastFileDisp = new TextDisplay(tr("LastFile:"),"",0,this);

  QHBoxLayout *fileLayout = new QHBoxLayout;
  fileLayout->addWidget(nextFileDisp);
  fileLayout->addWidget(lastFileDisp);

  // Final element: exposure status text display

  acqStatus = new TextDisplay("","",0,this);
  
  // Now assemble them into the Exposure Control QGroupBox

  QVBoxLayout *expControlLayout = new QVBoxLayout;
  expControlLayout->addLayout(expFormLayout);
  expControlLayout->addLayout(fileLayout);
  expControlLayout->addLayout(expButtonLayout);
  expControlLayout->addWidget(acqStatus);
  expControlLayout->addLayout(imcsLayout);

  expGroupBox->setLayout(expControlLayout);

  //------------------------------------------------
  //
  // Channel Group Box
  //

  QGroupBox *mainGroupBox = new QGroupBox(tr("%1 Channel").arg(channelName));
  mainGroupBox->setAlignment(Qt::AlignHCenter);

  QVBoxLayout *controlLayout = new QVBoxLayout;
  controlLayout->addWidget(configGroupBox);
  controlLayout->addWidget(expGroupBox);
  controlLayout->addStretch();
  mainGroupBox->setLayout(controlLayout);
  mainGroupBox->setContentsMargins(3,10,3,3);

  // A little highlighting based on the channel color.  Just enough
  // for a visual cue, but not so much that it gets up in your face
  // and makes your eyes hurt at 3am...

  QPalette *mainPalette = new QPalette;
  *mainPalette = mainGroupBox->palette();
  switch (myChannel) {
  case MODSChannel::Blue:
    mainPalette->setColor(QPalette::Mid,Qt::blue);
    mainGroupBox->setPalette(*mainPalette);
    break;

  case MODSChannel::Red:
    mainPalette->setColor(QPalette::Mid,Qt::red);
    mainGroupBox->setPalette(*mainPalette);
    break;
  }

  // Main Widget Layout

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(mainGroupBox);
  mainLayout->setContentsMargins(0,0,0,0);
  setLayout(mainLayout);

  // Various sensible startup defaults

  objName = "";
  imgType = "OBJECT";
  expTime = 0.0;
  numExp = 1;
  numExpEntry->setValue(numExp);
  xBin = 1;
  yBin = 1;

  // Acquisition Flags

  kExp = 1;
  imgAbort = false;
  imgAcquire = false;
  imgPause = false;
  expState = MODSChannel::expIdle;
  expStatusStr = tr("Idle");
  acqStatus->clear();
  goButton->setText(tr("GO"));
  goButton->setNormal();
  pauseButton->setText(tr("Pause"));
  pauseButton->disable();
  stopButton->disable();
  imcsLockButton->enable();
  useIMCS = false;
  imcsMinExpTime = MODS_IMCS_MINEXP;
  imcsLockCount = 0;

  // Setup the exposure timer and connect its timeout() signal to the
  // expUpdate() slot

  expTimer = new QTimer(this);
  connect(expTimer,SIGNAL(timeout()),this,SLOT(expUpdate()));

  // Setup the IMCS timer and connect its timeout() signal to the
  // queryIMCSTimer() slot.

  imcsTimer = new QTimer(this);
  connect(imcsTimer,SIGNAL(timeout()),this,SLOT(queryIMCSLock()));
  imcsLockPending = false;

}

// Send a command to the IE host via ISIS.  This emits the cmdReady() signal
// to whoever is listening for commands to pass along.

void MODSChannel::sendIECommand(const QString &cmd)
{
  if (cmd.isEmpty()) return;

  // Seems silly, but in future versions we will need a working
  // command string (cf. focalplane.cpp).

  QString cmdStr = cmd;

  // We have something to send, emit the cmdReady() signal

  emit cmdReady(modsIEHost[modsID],cmdStr);

}

// Send a command to the IC host via ISIS.  This emits the cmdReady() signal
// to whoever is listening for commands to pass along.

void MODSChannel::sendICCommand(const QString &cmd)
{
  if (cmd.isEmpty()) return;

  // Seems silly, but in future versions we will need a working
  // command string (cf. focalplane.cpp).

  QString cmdStr = cmd;

  // We have something to send, emit the cmdReady() signal

  emit cmdReady(icHostID,cmdStr);

}

// Send a command and wait for reply.  If timeout>0 we wait and block,
// if timeout<0 it queues the request, and if timeout=0 it sends it immediately

void MODSChannel::sendCmdWait(const QString &hostID, const QString &cmdStr,
			      const int &timeout)
{
  if (timeout == 0)
    emit cmdReady(hostID,cmdStr);
  else
    emit cmdWait(hostID,cmdStr,timeout);

}

//---------------------------------------------------------------------------
// 
// parse - process replies from an IMPv2 client 
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
// remoteCmd flag and execute it using the sendIECommand() method.
//

void MODSChannel::parse(const QString &cmdStr, 
			const int &msgType, 
			const QHash<QString,QString> &keyDict)
{
  // TODO: Unused cmdWarn flag. Commented out on 2-19-24
  bool cmdDone = false;   // Flag: message is a command completion
  bool cmdFault = false;  // Flag: command completed with a fault
  // bool cmdWarn = false;   // Flag: command generated a warning
  bool ok;

  QString msgStr = cmdStr;
  msgStr = msgStr.simplified(); // TODO: Evaluate this line. Why is it here? 

  if (msgStr.isEmpty()) return; // Empty messages are ignored
  
  // TODO: Unused cmdWarn flag. Commented out on 2-19-24
  // Our response depends on the type of message we received. 

  switch (msgType) {
  case ISISClient::ERROR:
    cmdDone = true;
    cmdFault = true;
    // cmdWarn = false;
    break;

  case ISISClient::WARNING:
    cmdDone = false;
    cmdFault = false;
    // cmdWarn = true;
    break;

  case ISISClient::FATAL:
    cmdDone = true;
    cmdFault = true;
    // cmdWarn = false;
    break;

  case ISISClient::DONE:
    cmdDone = true;
    cmdFault = false;
    // cmdWarn = false;
    break;

  case ISISClient::STATUS:
    cmdDone = false;
    cmdFault = false;
    // cmdWarn = false;
    break;

  default:
    // Nothing to be done here, return (we don't handle REQ: or EXEC: requests - yet?)
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

      // Grating Select State is complicated by the need to enable the
      // grtiltEntry menu if the position is tiltable
	
      if (keyStr.compare(grating->command(),Qt::CaseInsensitive)==0) {
	//gratingControl->setISISState(msgType);
	int ipos = valStr.toInt(&ok,10);
	if (ok) {
	  grating->setPos(ipos);  // change the grating menu select entry
	  gratingDisplay->setText(grating->ID(ipos),Qt::blue);
	  gratingDisplay->setNormal();
	  // Enable the tilt menu if we are at a turret position with
	  // an active tilt mechanism (currently grating 2 or 4)
	  switch(ipos) {
	  case 1:
	    grtiltDisplay->setNormal();
	    grtiltDisplay->clear();
	    imagingButton->highLight();
	    gratingButton->setNormal();
	    prismButton->setNormal();
	    commitButton->setNormal();
	    break;

	  case 2:
	    grtilt->setPos(gratingTilt[0]);
	    grtilt->setCommand(QString("%1GRTILT1").arg(channelID));
	    grtiltDisplay->setNormal();
	    grtiltDisplay->clear();
	    setCurrentTilt(gratingTilt[0],1); // last value we know
	    imagingButton->setNormal();
	    gratingButton->highLight();
	    prismButton->setNormal();
	    commitButton->setNormal();
	    break;

	  case 3:
	    grtiltDisplay->setNormal();
	    grtiltDisplay->clear();
	    imagingButton->setNormal();
	    gratingButton->setNormal();
	    prismButton->highLight();
	    commitButton->setNormal();
	    break;

	  case 4:
	    grtilt->setPos(gratingTilt[1]);
	    grtilt->setCommand(QString("%1GRTILT2").arg(channelID));
	    grtiltDisplay->setNormal();
	    grtiltDisplay->clear();
	    setCurrentTilt(gratingTilt[1],2); // last value we know
	    imagingButton->setNormal();
	    gratingButton->setNormal();
	    prismButton->setNormal();
	    commitButton->setNormal();
	    break;
	    
	  default:
	    grtiltDisplay->setNormal();
	    grtiltDisplay->clear();
	    break;
	  }
	}
	else {
	  if (valStr.compare("BUSY",Qt::CaseInsensitive)!=0) {
	    grating->setErrorStr(msgStr);
	    gratingDisplay->setFault();
	  }
	}
      }

      // Grating Tilt States

      // GRTILT1 = tilt mechanism at grating position 2
      
      else if (keyStr.compare(QString("%1GRTILT1").arg(channelID),Qt::CaseInsensitive)==0) {
	int ipos = valStr.toInt(&ok,10);
	if (ok) {
	  gratingTilt[0] = ipos;
	  if (grating->pos() == 2) {
	    grtilt->setPos(ipos);
	    grtiltDisplay->setNormal();
	    grtiltDisplay->clear();
	    gratingTilt[0]=ipos;
	    setCurrentTilt(ipos,1);
	  }
	}
	else {
	  if (grating->pos() == 2 && valStr.compare("BUSY",Qt::CaseInsensitive)!=0)
	    grtiltDisplay->setFault();
	}
      }
	  
      // GRTILT2 = tilt mechanism at grating position 4
      
      else if (keyStr.compare(QString("%1GRTILT2").arg(channelID),Qt::CaseInsensitive)==0) {
	int ipos = valStr.toInt(&ok,10);
	if (ok) {
	  gratingTilt[1] = ipos;
	  if (grating->pos() == 4) {
	    grtilt->setPos(ipos);
	    grtiltDisplay->setNormal();
	    grtiltDisplay->clear();
	    gratingTilt[1]=ipos;
	    setCurrentTilt(ipos,2);
	  }
	}
	else {
	  if (grating->pos() == 4 && valStr.compare("BUSY",Qt::CaseInsensitive)!=0)
	    grtiltDisplay->setFault();
	}
      }
	  
      // Camera Filter Wheel

      else if (keyStr.compare(filter->command(),Qt::CaseInsensitive)==0) {
	filterControl->setISISState(msgType);
	int ipos = valStr.toInt(&ok,10);
	if (ok)
	  filter->setPos(ipos);
	else
	  if(valStr.compare("BUSY",Qt::CaseInsensitive)!=0)
	    filterControl->setFault();
      }
      
      // Camera Focus

      else if (keyStr.compare(camfoc->command(),Qt::CaseInsensitive)==0) {
	double pos = valStr.toDouble(&ok);
	if (ok) {
	  camfoc->setPos((int)(pos));
	  int ipos = (int)(pos);
	  camfocDisplay->setText(QString("%1").arg(ipos),Qt::blue);
	  camfocDisplay->setNormal();
	}
	else
	  if(valStr.compare("BUSY",Qt::CaseInsensitive)!=0)
	    camfocDisplay->setFault();
      }

      // Collimator TTF actuators

      else if (keyStr.compare(colttfa->command(),Qt::CaseInsensitive)==0) {
	double pos = valStr.toDouble(&ok);
	if (ok) {
	  colttfa->setPos((int)(pos));
	  ttfaDisplay->setText(valStr,Qt::blue);
	  ttfaDisplay->setNormal();
	}
	else
	  if (valStr.compare("BUSY",Qt::CaseInsensitive)!=0)
	    ttfaDisplay->setFault();
      }

      else if (keyStr.compare(colttfb->command(),Qt::CaseInsensitive)==0) {
	double pos = valStr.toDouble(&ok);
	if (ok) {
	  colttfb->setPos((int)(pos));
	  ttfbDisplay->setText(valStr,Qt::blue);
	  ttfbDisplay->setNormal();
	}
	else
	  if(valStr.compare("BUSY",Qt::CaseInsensitive)!=0)
	    ttfbDisplay->setFault();
      }

      else if (keyStr.compare(colttfc->command(),Qt::CaseInsensitive)==0) {
	double pos = valStr.toDouble(&ok);
	if (ok) {
	  colttfc->setPos((int)(pos));
	  ttfcDisplay->setText(valStr,Qt::blue);
	  ttfcDisplay->setNormal();
	}
	else
	  if(valStr.compare("BUSY",Qt::CaseInsensitive)!=0)
	    ttfcDisplay->setFault();
      }

      // Detector System Keywords

      // Exposure Time, a couple of different variants

      else if (keyStr.compare("EXPTIME",Qt::CaseInsensitive)==0 || 
	       keyStr.compare("EXPOSURETIME",Qt::CaseInsensitive)==0 ||
	       keyStr.compare("EXP",Qt::CaseInsensitive)==0) {
	expTime = valStr.toDouble(&ok);
	expTimeEntry->setValue(expTime);
	expTimeEntry->setNormal();
      }

      // This is a real kludge - do better ...

      else if (keyStr.compare("IMAGETYPE",Qt::CaseInsensitive)==0 ||
	       keyStr.compare("IMTYP",Qt::CaseInsensitive)==0) {
	imgType = valStr;
	if (imgType.compare("OBJECT",Qt::CaseInsensitive)==0)
	  imgTypeEntry->setCurrentIndex(0);
	else if (imgType.compare("BIAS",Qt::CaseInsensitive)==0)
	  imgTypeEntry->setCurrentIndex(1);
	else if (imgType.compare("FLAT",Qt::CaseInsensitive)==0)
	  imgTypeEntry->setCurrentIndex(2);
	else if (imgType.compare("COMP",Qt::CaseInsensitive)==0)
	  imgTypeEntry->setCurrentIndex(3);
	else if (imgType.compare("STD",Qt::CaseInsensitive)==0)
	  imgTypeEntry->setCurrentIndex(4);
	else if (imgType.compare("SKY",Qt::CaseInsensitive)==0)
	  imgTypeEntry->setCurrentIndex(5);
	else if (imgType.compare("DARK",Qt::CaseInsensitive)==0)
	  imgTypeEntry->setCurrentIndex(6);

	if (cmdFault)
	  imgTypeEntry->setFault();
	else
	  imgTypeEntry->setNormal();
      }

      // Object (aka Target) Name - a couple of variants are possible here

      else if (keyStr.compare("OBJECTNAME",Qt::CaseInsensitive)==0 ||
	       keyStr.compare("OBJ",Qt::CaseInsensitive)==0) {
	objName = valStr;
	objNameEntry->setText(objName);
	if (cmdFault)
	  objNameEntry->setFault();
	else
	  objNameEntry->setNormal();
      }
      
      // X-axis on-chip binning factor

      else if (keyStr.compare("XBIN",Qt::CaseInsensitive)==0 ||
	       keyStr.compare("DATAXBIN",Qt::CaseInsensitive)==0) {
	int nxb = valStr.toInt(&ok,10);
	if (ok) {
	  int ixb = (int)(ceil(log10((double)(nxb))/log10(2.0)));
	  xCCDBin->setPos(ixb);
	  xBin = nxb;
	  xBinEntry->setNormal();
	}
	else {
	  xBinEntry->setFault();
	}
      }
      
      // Y-axis on-chip binning factor

      else if (keyStr.compare("YBIN",Qt::CaseInsensitive)==0 ||
	       keyStr.compare("DATAYBIN",Qt::CaseInsensitive)==0) {
	int nyb = valStr.toInt(&ok,10);
	if (ok) {
	  int iyb = (int)(ceil(log10((double)(nyb))/log10(2.0)));
	  yCCDBin->setPos(iyb);
	  yBin = nyb;
	  yBinEntry->setNormal();
	}
	else {
	  yBinEntry->setFault();
	}
      }

      // Parse the ROI, and see if we can ID it as one of the preset
      // values, otherwise we indicate it is a "custom" readout mode

      else if (keyStr.compare("ROI",Qt::CaseInsensitive)==0) {
	QString testCmd = QString("ROI ON %1 %2 %3 %4").arg(valStr.section(",",0,0)).arg(valStr.section(",",1,1)).arg(valStr.section(",",2,2)).arg(valStr.section(",",3,3));
	int roiIndex = roiEntry->findData(testCmd);
	if (roiIndex > -1)
	  roiEntry->setCurrentIndex(roiIndex);
	else {
	  if (valStr.compare(QString("1,%1,1,%2").arg(MODS_CCD_XSIZE).arg(MODS_CCD_YSIZE),
			     Qt::CaseInsensitive)==0)
	    roiEntry->setCurrentIndex(0);  // full frame
	  else
	    roiEntry->setCurrentIndex(roiEntry->count()-1);
	}
	if (cmdFault)
	  roiEntry->setFault();
	else
	  roiEntry->setNormal();
      }

      // Filename of the next image

      else if (keyStr.compare("FILENAME",Qt::CaseInsensitive)==0) {
	QFileInfo foo(valStr);
	fileName = foo.fileName();
	nextFileDisp->setText(fileName,Qt::blue);
      }

      else if (keyStr.compare("NEXTFILE",Qt::CaseInsensitive)==0) {
	QFileInfo foo(valStr);
	fileName = foo.fileName();
	nextFileDisp->setText(fileName,Qt::blue);
      }

      // Filename of the *last* image, w/o path or .fits extension

      else if (keyStr.compare("LASTFILE",Qt::CaseInsensitive)==0) {
	QFileInfo foo(valStr);
	lastFile = foo.completeBaseName();
	lastFileDisp->setText(lastFile,Qt::blue);
      }

      // Raw Image Directory Path

      else if (keyStr.compare("PATH",Qt::CaseInsensitive)==0) {
	imgPath = valStr;
	// and then show them somewhere...
      }

      // CCD Controller Mode

      else if (keyStr.compare("CCDMODE",Qt::CaseInsensitive)==0) {
	if (valStr.compare("IDLE",Qt::CaseInsensitive)==0) {
	  expState = MODSChannel::expIdle;
	  expStatusStr = tr("Idle");
	  imgAcquire = false;
	  imgAbort = false;
	  imgPause = false;
	  enableCCDControls(true);
	  kExp = 1;
	}
      }
	    
      // Shutter State

      else if (keyStr.compare("SHUTTER",Qt::CaseInsensitive)==0) {
	int shs = valStr.toInt(&ok,10);
	if (shs == 0) {
	  shutterDisplay->setText(tr("Closed"));
	  shutterDisplay->setTextColor(Qt::blue);
	  shutterOpen = false;
	}
	else if (shs == 1) {
	  shutterDisplay->setText(tr("OPEN"));
	  shutterDisplay->setTextColor(Qt::red);
	  shutterOpen = true;
	}	  
      }

      // IMCS State flags

      // Is the IMCS loop open or closed

      else if (keyStr.compare(QString("IMCS_%1").arg(channelID),Qt::CaseInsensitive)==0 ||
	       keyStr.compare(QString("%1IMCS").arg(channelID),Qt::CaseInsensitive)==0) {
	if (valStr.compare("OPEN",Qt::CaseInsensitive)==0) {
	  imcsLoopClosed = false;
	  imcsLoopDisplay->setText("Idle");
	  imcsLoopDisplay->setTextColor(Qt::blue);
	}
	else {
	  imcsLoopClosed = true;
	  if (imcsLockPending) {
	    imcsLoopDisplay->setNormal();
	    if (imcsOnTarget) {
	      imcsLoopDisplay->setText("ACTIVE - Locked On");
	      imcsLoopDisplay->setTextColor(Qt::darkGreen);
	    }
	    else {
	      imcsLoopDisplay->setText("ACTIVE - Lock Requested");
	      imcsLoopDisplay->setChange();
	    }
	  }
	  else {
	    imcsLoopDisplay->setText("ACTIVE");
	    imcsLoopDisplay->setTextColor(Qt::darkGreen);
	  }
	}
      }

      // Is laser spot centered on the quad cell (to within tolerances)?

      else if (keyStr.compare(QString("TARGET_%1").arg(channelID),Qt::CaseInsensitive)==0) {
	if (valStr.compare("ON",Qt::CaseInsensitive)==0) {
	  imcsOnTarget = true;
	  if (imcsLoopClosed) {
	    imcsLoopDisplay->setText("ACTIVE - Locked On");
	    imcsLoopDisplay->setNormal();
	    imcsLoopDisplay->setTextColor(Qt::darkGreen);
	    imcsLockButton->highLight();
	  }
	  else {
	    imcsLockButton->setNormal();
	  }
	}
	else {
	  imcsOnTarget = false;
	  if (imcsLockPending) {
	    imcsLockButton->setChange();
	    if (imcsLoopClosed) {
	      imcsLoopDisplay->setText("ACTIVE - Lock Pending");
	      imcsLoopDisplay->setChange();
	    }
	  }
	  else {
	    imcsLockButton->setNormal();
	    if (imcsLoopClosed) {
	      imcsLoopDisplay->setText("ACTIVE");
	      imcsLoopDisplay->setTextColor(Qt::darkGreen);
	    }
	  }
	}
      }

      else if (keyStr.compare(QString("CLGAIN_%1").arg(channelID),Qt::CaseInsensitive)==0) {
	double gain = valStr.toInt(&ok);
	if (ok) imcsGain = gain;
      }

      else if (keyStr.compare(QString("QCMIN_%1").arg(channelID),Qt::CaseInsensitive)==0) {
	double qcmin = valStr.toInt(&ok);
	if (ok) imcsThresh = qcmin;
      }

      else if (keyStr.compare(QString("QCSAMP_%1").arg(channelID),Qt::CaseInsensitive)==0) {
	int qcsamp = valStr.toInt(&ok,10);
	if (ok) { 
	  imcsSampTime = qcsamp;
	  imcsCycleTime = imcsSampTime*imcsNumAverage;
	}
      }

      else if (keyStr.compare(QString("QCNAVG_%1").arg(channelID),Qt::CaseInsensitive)==0 ||
	       keyStr.compare(QString("QCNAVER_%1").arg(channelID),Qt::CaseInsensitive)==0) {
	int qcnavg = valStr.toInt(&ok,10);
	if (ok) {
	  imcsNumAverage = qcnavg;
	  imcsCycleTime = imcsSampTime*imcsNumAverage;
	}
      }
	  
      // EXPSTATE gives the exposure status during GO, SNAP, and related commands

      else if (keyStr.compare("EXPSTATUS",Qt::CaseInsensitive)==0) {
	if (cmdWord.compare("GO",Qt::CaseInsensitive)==0 ||
	    cmdWord.compare("MGO",Qt::CaseInsensitive)==0 ||
	    cmdWord.compare("SNAP",Qt::CaseInsensitive)==0) {
	  if (valStr.compare("INITIALIZING",Qt::CaseInsensitive)==0) {
	    if (expState != MODSChannel::expInit) {
	      expState = MODSChannel::expInit;
	      expProgress->reset();
	      expProgress->setFormat("%vs");
	      expProgress->setValue((int)(expTime));
	      readProgress->reset();
	      pauseButton->disable();
	      stopButton->disable();
	      if (numExp > 1)
		expStatusStr = tr("Initializing Exposure %1 of %2...").arg(kExp).arg(numExp);
	      else
		expStatusStr = tr("Initializing CCD...");

	      acqStatus->setText(expStatusStr,Qt::blue);
	      expNumDisplay->setText(QString("%1 of %2").arg(kExp).arg(numExp));
	      imgAcquire = true;
	      imgPause = false;
	      imgAbort = false;
	      enableCCDControls(false);
	    }
	  }
	  else if (valStr.compare("ERASE",Qt::CaseInsensitive)==0) {
	    expState = MODSChannel::expErase;
	    expStatusStr = tr("Erasing the CCD...");
	    acqStatus->setText(expStatusStr,Qt::blue);
	  }
	  else if (valStr.compare("INTEGRATING",Qt::CaseInsensitive)==0) {
	    if (expState != MODSChannel::expIntegrating) {
	      // If we are paused, this means we are resuming the exposure
	      if (expState == MODSChannel::expPause) {
		if (numExp > 1)
		  expStatusStr = tr("Resuming Acquisition of Image %1 of %2...").arg(kExp).arg(numExp);
		else
		  expStatusStr = tr("Resuming Image Acquisition...");
		acqStatus->setText(expStatusStr,Qt::blue);
	      }
	      // otherwise, we are starting from the beginning
	      else {
		enableCCDControls(false);
		expStart();
		pauseButton->enable();
		stopButton->enable();
		if (numExp > 1)
		  expStatusStr = tr("Acquiring Image %1 of %2...").arg(kExp).arg(numExp);
		else
		  expStatusStr = tr("Acquiring Image...");
		acqStatus->setText(expStatusStr,Qt::blue);
	      }
	      expState = MODSChannel::expIntegrating;
	      imgAcquire = true;
	      imgPause = false;
	      imgAbort = false;	
	    }
	  }
	  else if (valStr.compare("READOUT",Qt::CaseInsensitive)==0) {
	    if (expState != MODSChannel::expReadout) {
	      expState = MODSChannel::expReadout;	
	      imgPause = false;
	      expTimer->stop();
	      expProgress->setValue(0);
	      readProgress->reset();
	      readProgress->setRange(0,100);
	      readProgress->setValue(0);
	      pctReadout = 0;
	      goButton->disable();
	      pauseButton->disable();
	      stopButton->disable();
	      expStatusStr = tr("Reading Out...");
	      acqStatus->setText(expStatusStr,Qt::blue);
	      // IMCS controls go here...
	    }
	  }
	  else if (valStr.compare("PAUSED",Qt::CaseInsensitive)==0) {
	    if (expState != MODSChannel::expPause) {
	      expState = MODSChannel::expPause;
	      imgPause = true;
	      // Pause our internal exposure timer
	      expTimer->stop();
	      // Change the Pause button into a Resume button
	      pauseButton->enable();
	      pauseButton->setText(tr("Resume"));
	      pauseButton->setChange();
	      stopButton->enable();
	      // Notify the user that the pause has begun
	      if (numExp > 1)
		expStatusStr = tr("Exposure %1 of %2 Paused...").arg(kExp).arg(numExp);
	      else
		expStatusStr = tr("Exposure Paused...");
	      acqStatus->setText(expStatusStr,QColor(255,165,0,255));
	    }
	  }
	  else if (valStr.compare("WRITING",Qt::CaseInsensitive)==0) {
	    if (expState != MODSChannel::expWrite) {
	      expState = MODSChannel::expWrite;
	      readProgress->setValue(100);
	      imgPause = false;
	      expStatusStr = tr("Writing Image...");
	      acqStatus->setText(expStatusStr,Qt::blue);
	    }
	  }
	  else if (valStr.compare("DONE",Qt::CaseInsensitive)==0) {
	    if (expState != MODSChannel::expDone) {
	      expState = MODSChannel::expDone;
	      imgPause = false;
	      imgAbort = false;
	      expProgress->reset();
	      readProgress->reset();
	      goButton->enable();
	      pauseButton->disable();
	      stopButton->disable();
	      if (numExp > 1) 
		expStatusStr = tr("Exposure Done, cleaning up...");
	      else {
		expStatusStr = tr("Exposure Done, cleaning up...");
		expNumDisplay->clear();
	      }
	      acqStatus->setText(expStatusStr,Qt::blue);
	      if (useExpBell) qApp->beep();
	    }
	  }
	  // others?
	}
      }

      // PCTREAD = % of the CCD readout in progress

      else if (keyStr.compare("PCTREAD",Qt::CaseInsensitive)==0) {
	int pctReadout = valStr.toInt(&ok,10);
	if (expState == MODSChannel::expReadout)
	  readProgress->setValue(pctReadout);
      }

      // Unrecognized keywords get ignored.

      else {
	// Nothing to see here, folks, move along please...
      }

      // If the keyword value is BUSY, reset the cmdFault flag

      if (valStr.compare("BUSY",Qt::CaseInsensitive)==0) cmdFault = false;

    }
  }

  // If commands require additional handling, especially for faults, do it here...

  if (cmdFault || cmdDone) {
    if (cmdWord.compare(camfoc->command(),Qt::CaseInsensitive)==0) {
      if (msgType == ISISClient::ERROR || msgType == ISISClient::FATAL)
	camfocDisplay->setFault();
      else
	camfocDisplay->setNormal();
    }
    else if (cmdWord.compare(grating->command(),Qt::CaseInsensitive)==0) {
      if (msgType == ISISClient::ERROR || msgType == ISISClient::FATAL)
	gratingDisplay->setFault();
      else
	gratingDisplay->setNormal();
    }
    else if (cmdWord.compare(filter->command(),Qt::CaseInsensitive)==0)
      filterControl->setISISState(msgType);
    else if (cmdWord.compare(grtilt->command(),Qt::CaseInsensitive)==0) {
      if (msgType == ISISClient::ERROR || msgType == ISISClient::FATAL)
	grtiltDisplay->setFault();
      else
	grtiltDisplay->setNormal();
    }
    else if (cmdWord.compare("ROI",Qt::CaseInsensitive)==0) {
      if (msgType == ISISClient::ERROR) 
	roiEntry->setFault();
      else
	roiEntry->setNormal();
    }
    else if (cmdWord.compare("XBIN",Qt::CaseInsensitive)==0) {
      if (msgType == ISISClient::ERROR) 
	xBinEntry->setFault();
      else
	xBinEntry->setNormal();
    }
    else if (cmdWord.compare("YBIN",Qt::CaseInsensitive)==0) {
      if (msgType == ISISClient::ERROR) 
	yBinEntry->setFault();
      else
	yBinEntry->setNormal();
    }
    else if (cmdWord.compare("CCDBIN",Qt::CaseInsensitive)==0) {
      if (msgType == ISISClient::ERROR) {
	xBinEntry->setFault();
	yBinEntry->setFault();
      }
      else {
	xBinEntry->setNormal();
	yBinEntry->setNormal();
      }
    }
    
  }

  // The xCOLFOC command is a combination of 3 commands (xCOLTTFA,B,C) and
  // so requires some special handling

  if (cmdWord.compare(QString("%1COLFOC").arg(channelID),Qt::CaseInsensitive)==0) {
    if (cmdFault) {
      ttfaDisplay->setFault();
      ttfbDisplay->setFault();
      ttfcDisplay->setFault();
    }
    else if (cmdDone) {
      ttfaDisplay->setNormal();
      ttfbDisplay->setNormal();
      ttfcDisplay->setNormal();
    }
  }

  // Exposure-control handling related to completion, warnings, and other errors

  if (cmdWord.compare("GO",Qt::CaseInsensitive)==0 ||
      cmdWord.compare("MGO",Qt::CaseInsensitive)==0 ||
      cmdWord.compare("SNAP",Qt::CaseInsensitive)==0) {
    if (cmdDone) { // acquisition command is complete
      if (imgAbort) { // servicing an abort request
	if (numExp > 1)
	  expStatusStr = tr("%1 Exposure Sequence aborted during image %2 of %3").arg(channelName).arg(kExp).arg(numExp);
	else
	  expStatusStr = tr("%1 Exposure Aborted").arg(channelName);

	emit logMessage(expStatusStr);
	acqStatus->clear();
	imgAcquire = false;
	goButton->setText(tr("GO"));
	goButton->setNormal();
	imgPause = false;	
	pauseButton->setText(tr("Pause"));
	pauseButton->disable();
	stopButton->disable();
	expNumDisplay->clear();
	imgAbort = false;
	enableCCDControls(true);
	kExp = 1;
	if (useExpBell) qApp->beep();
      }
      else if (imgPause) { // servicing a pause request...
	if (numExp > 1)
	  expStatusStr = tr("%1 Exposure Sequence paused during image %2 of %3"
			    " at %4 sec of %5"
			    ).arg(channelName).arg(kExp).arg(numExp).arg(expElapsed).arg((int)(expTime));
	else
	  expStatusStr = tr("%1 Exposure Paused at %2 sec of %3"
			    ).arg(channelName).arg(expElapsed).arg((int)(expTime));

	acqStatus->setText(expStatusStr,Qt::blue);
      }
      else { // the exposure is really done
	if (numExp > 1)
	  expStatusStr = tr("%1 Exposure %2 of %3 completed...").arg(channelName).arg(kExp).arg(numExp);
	else
	  expStatusStr = tr("%1 Exposure completed...").arg(channelName);

	acqStatus->setText(expStatusStr,Qt::blue);

	// Update the next file and last file

	sendCmdWait(icHostID,"nextfile",MODS_QUEUE_REQUEST);
	sendCmdWait(icHostID,"lastfile",MODS_QUEUE_REQUEST);

	// What do we do next, is this it or is there another image to
	// acquire?  Do the test for kExp >= numExp, as there are some
	// abort-service race conditions that can cause kExp to "jump
	// the fence", leading to a runaway exposure sequence.

	if (kExp >= numExp) {
	  if (numExp > 1) 
	    expStatusStr = tr("%1 Exposure Sequence Completed.").arg(channelName);
	  else
	    expStatusStr = tr("%1 Exposure Completed.").arg(channelName);
	  emit logMessage(expStatusStr);
	  acqStatus->clear();
	  imgAbort = false;
	  imgAcquire = false;
	  goButton->setText(tr("GO"));
	  goButton->setNormal();
	  imgPause = false;
	  pauseButton->setText(tr("Pause"));
	  pauseButton->disable();
	  stopButton->disable();
	  kExp = 1;
	  expProgress->reset();
	  readProgress->reset();
	  expNumDisplay->clear();
	  enableCCDControls(true);
	  if (numExp > 1)
	    if (useExpBell) qApp->beep();
	  expState = MODSChannel::expIdle;
	  expStatusStr = tr("Idle");
	}
	else {
	  imgAcquire = false;  // we are not yet acquiring the next image...
	  kExp++;        // increment the exposure counter
	  doExposure();  // start the next exposure
	}
      }
    }
    else { 
      // other stuff, warnings, errors, etc.
    }

    // ...
  }


}
  
// Special Methods

// Launch the Collimator Mirror Control Panel dialog

void MODSChannel::launchCollimatorDialog()
{
  emit logMessage(tr("%1 Collimator Mirror Control Panel not yet implemented").arg(channelName));
}

// Update (refresh) the control panel

void MODSChannel::update()
{
  // Reset the control states (free stuck controls)

  gratingDisplay->clear();
  grtiltDisplay->clear();
  filterControl->setUnknown();
  camfocDisplay->clear();
  ttfaDisplay->clear();
  ttfbDisplay->clear();
  ttfcDisplay->clear();
  
  // Do not refresh the exposure controls if an exposure is in
  // progress

  if (!imgAcquire) {
    imgAcquire = false;
    imgPause = false;
    imgAbort = false;

    enableCCDControls(true);  // enable the CCD setup controls

    // Fix the exposure controls

    goButton->setNormal();
    goButton->setText(tr("GO"));
    goButton->enable();
    pauseButton->setNormal();
    pauseButton->setText(tr("Pause"));
    pauseButton->disable();
    stopButton->disable();
    shutterOpen = false;
    shutterDisplay->setText(tr("Closed"));
    shutterDisplay->setTextColor(Qt::blue);
    imcsLoopDisplay->clear();
    objName = "";
    imgType = "OBJECT";
    expTime = 0.0;
    numExp = 1;
    kExp = 1;
    numExpEntry->setValue(numExp);
    fileName = "";
    nextFileDisp->clear();
    lastFile = "";
    lastFileDisp->clear();
    expProgress->reset();
    readProgress->reset();
    acqStatus->clear();
  }

  // Send status commands to the CCD control and data transfer hosts.

  sendCmdWait(icHostID,"status",MODS_QUEUE_REQUEST);
  sendCmdWait(icHostID,"nextfile",MODS_QUEUE_REQUEST);
  sendCmdWait(icHostID,"lastfile",MODS_QUEUE_REQUEST);
  // sendCmdWait(dmHostID,"+swap",MODS_QUEUE_REQUEST);

  // And to the main mechanism groups in this channel.  Single
  // queries are needed because ISTATUS can be stale...

  emit doCommand(QString("%1GRATING").arg(channelID));
  emit doCommand(QString("%1GRTILT1").arg(channelID));
  emit doCommand(QString("%1FILTER").arg(channelID));
  emit doCommand(QString("%1CAMFOC").arg(channelID));
  emit doCommand(QString("%1COLFOC").arg(channelID));
  emit doCommand(QString("%1IMCS").arg(channelID));
  // emit doCommand(QString("%1GRTILT2").arg(channelID)); // unused mechanism causes timeouts
  
}

// Reset the exposure control state

void MODSChannel::resetExp()
{
  // Reset the various exposure state flags

  imgAcquire = false;
  imgPause = false;
  imgAbort = false;

  enableCCDControls(true);  // enable the CCD setup controls

  // Fix the exposure controls

  goButton->setNormal();
  goButton->setText(tr("GO"));
  goButton->enable();
  pauseButton->setNormal();
  pauseButton->setText(tr("Pause"));
  pauseButton->disable();
  stopButton->disable();
  shutterOpen = false;
  shutterDisplay->setText(tr("Closed"));
  shutterDisplay->setTextColor(Qt::blue);
  objName = "";
  imgType = "OBJECT";
  expTime = 0.0;
  setNumExp(1);
  kExp = 1;
  fileName = "";
  nextFileDisp->clear();
  lastFile = "";
  lastFileDisp->clear();
  expProgress->reset();
  readProgress->reset();
  acqStatus->clear();

  // Stop the timer and reset the progress bars

  expTimer->stop();
  expProgress->reset();
  readProgress->reset();

  // Make sure the IMCS control loop is stopped if active

  if (useIMCS) {
    if (imcsLoopClosed) sendIECommand(QString("%1IMCS OPEN").arg(channelID));
    useIMCS = false;
  }
  else
    sendIECommand(QString("%1IMCS").arg(channelID));
  
  // Send status commands to the IC and data transfer hosts, waiting between queries

  sendCmdWait(icHostID,"status",MODS_QUEUE_REQUEST);
  sendCmdWait(icHostID,"nextfile",MODS_QUEUE_REQUEST);
  sendCmdWait(icHostID,"lastfile",MODS_QUEUE_REQUEST);
  // sendCmdWait(dmHostID,"+swap",MODS_QUEUE_REQUEST);
}

// Get the MODS Channel ID prefix (B or R)

QString MODSChannel::channel()
{
  return channelID;
}

// Exposure Control methods

// Set the object name using the current image type.  We
// make sure the string follows FITS format restrictions
// (e.g., 68 chars max).

void MODSChannel::setObject(const QString &obj)
{
  // FITS requires character strings must be 68 characters maximum

  objName = obj.left(68);

  // IMPv2 reserves single quotes and () for its own purposes,
  // remove or modifiy as required to conform to FITS 3.0

  objName.remove("'");
  objName.replace("(","[");
  objName.replace(")","]");

  // No "=" for the Archon/azcam server (you really don't want to know)

  objName.replace("="," ");
  
  // If the objName has been changed, replace it in the display

  if (objName != obj)
    objNameEntry->setText(objName);

  // Now create the command string and send it to our channel's IC host

  QString cmdStr = QString("%1 %2").arg(imgType,objName);
  objNameEntry->setChange();
  sendICCommand(cmdStr);
}

// Set the image type using the current object name (blank does a query)

void MODSChannel::setImgType(const QString &imt)
{
  imgType = imt;
  QString cmdStr = QString("%1 %2").arg(imgType,objName);
  imgTypeEntry->setChange();
  sendICCommand(cmdStr);
  setNumExp(1); // reset the exposure counter to 1 (safety)
}

// Set the unit exposure time in seconds.  Only send if it has changed.

void MODSChannel::setExpTime(const double &et)
{
  if (et != expTime) {
    expTime = et;
    QString cmdStr = QString("EXP %1").arg(QString::number(expTime,'f',1));
    expTimeEntry->setChange();
    sendICCommand(cmdStr);
  }
  else 
    expTimeEntry->setNormal();
}

// Set the number of exposures to acquire with the next GO

void MODSChannel::setNumExp(const int &nx)
{
  numExp = nx;
  numExpEntry->setNormal();
  numExpEntry->setValue(numExp);
}

// Return the number of exposures to acquire next

int MODSChannel::getNumExp()
{
  return numExp;
}

// Set the CCD on-axis binning factors for X and Y (convenience function)

void MODSChannel::setCCDBinning(const int &ixb,const int &iyb)
{
  setCCDXBin(ixb);
  setCCDYBin(iyb);
}

// Set the CCD X-axis on-chip binning factor
//
// For the Archon controllers, only 1 and 2 are supported [rwp/osu 2026 Jan 19]

void MODSChannel::ccdXBin(const QString &bfc)
{
  bool ok;
  
  QString xbarg = bfc.section(" ",1,-1);
  int nb = xbarg.toInt(&ok,10);
  int xbf = (int)(pow(2,nb));
  QString cmdStr = QString("xbin %1").arg(xbf);
  xBinEntry->setChange();
  sendICCommand(cmdStr);
}

void MODSChannel::setCCDXBin(const int &ixb)
{
  if (ixb == 1 || ixb == 2) {
    xBin = ixb;
    QString cmdStr = QString("xbin %1").arg(QString::number(ixb));
    xBinEntry->setChange();
    sendICCommand(cmdStr);
  }
}

// Set the CCD Y-axis on-chip binning factor
//
// For the Archon controllers, only 1 and 2 are supported [rwp/osu 2026 Jan 19]

void MODSChannel::ccdYBin(const QString &bfc)
{
  bool ok;
  QString ybarg = bfc.section(" ",1,-1);
  int nb = ybarg.toInt(&ok,10);
  int ybf = (int)(pow(2,nb));
  QString cmdStr = QString("ybin %1").arg(ybf);
  yBinEntry->setChange();
  sendICCommand(cmdStr);
}

void MODSChannel::setCCDYBin(const int &iyb)
{
  if (iyb == 1 || iyb == 2) { 
    yBin = iyb;
    QString cmdStr = QString("ybin %1").arg(QString::number(iyb));
    yBinEntry->setChange();
    sendICCommand(cmdStr);
  }
}

// Set the subframe region of interest - we are passed the command to do the job...

void MODSChannel::ccdROI(const QString &cmdStr)
{
  if (cmdStr.compare("CUSTOM",Qt::CaseInsensitive)==0) {
    emit logMessage(tr("%1 channel Custom ROI setup not yet enabled").arg(channelName));
    roiEntry->setCurrentIndex(0);
  }
  else {
    roiEntry->setChange();
    sendICCommand(cmdStr);
  }
}

// Set the CCD readout speed, either fast or slow

void MODSChannel::ccdSpeed(const QString &cmdStr)
{
  emit logMessage(tr("%1 command %2 not enabled").arg(channelName).arg(cmdStr));
}

// Exposure GO/Abort - this slot is triggered when we hit the GO/Abort
// button.  The dirty work is done in the public methods execGo() and
// execAbort()

void MODSChannel::doExposure() 
{
  // If an exposure is in progress, abort, otherwise try to go

  if (imgAcquire)
    execAbort();
  else
    execGo();
}

// Stop an exposure in progress when the Stop button is pressed.  The
// dirty work is done by execStop()

void MODSChannel::stopExposure() 
{
  execStop();
}

// Exposure Pause/Resume

void MODSChannel::pauseExposure() 
{
  if (!imgAcquire) return;  // only do something if acquiring an image...
  QString cmdStr;
  QString msgStr;
  if (imgPause) {
    cmdStr = "RESUME";
    pauseButton->setText(tr("Pause"));
    pauseButton->setNormal();
    imgPause = false;
    if (numExp > 1) 
      msgStr = tr("Resuming %1 CCD Exposure %2 of %3...").arg(channelName).arg(kExp).arg(numExp);
    else
      msgStr = tr("Resuming %1 CCD Exposure...").arg(channelName);

    // Make sure we show the correct resumed shutter state

    shutterDisplay->setText(tr("OPEN"));
    shutterDisplay->setTextColor(Qt::red);
    shutterOpen = true;

    // Resume the internal exposure timer, 1-second update
    expTimer->start(1000);
  }
  else {
    cmdStr = "PAUSE";
    pauseButton->setText(tr("Resume"));
    pauseButton->setChange();
    imgPause = true;
    if (numExp > 1) 
      msgStr = tr("Pausing %1 CCD Exposure %2 of %3...").arg(channelName).arg(kExp).arg(numExp);
    else
      msgStr = tr("Pausing %1 CCD Exposure...").arg(channelName);
    expTimer->stop();
    if (useIMCS) {
      sendIECommand(QString("%1IMCS OPEN").arg(channelID)); // Open the IMCS loop
      imcsLoopDisplay->setChange();
    }
    shutterDisplay->setText(tr("Closed"));
    shutterDisplay->setTextColor(Qt::blue);
    shutterOpen = false;
  }
  acqStatus->setText(msgStr,Qt::blue);
  emit logMessage(tr("STATUS: %1").arg(msgStr));
  
  sendICCommand(cmdStr);
}

// Start the exposure countdown progress monitor

void MODSChannel::expStart()
{
  expTimer->stop();  // just in case...
  expElapsed = 1;    // elapsed expTime starts at 1 second for the controller delay
  expRemaining = (int)(expTime);  // time remaining

  // Reset the progress bar and start the exposure timer with 1s ticks
  // If the exposure time is <1 second, we treat it like a 0s bias frame

  expProgress->reset();
  if (expTime >= 1) {
    expProgress->setRange(0,(int)(expTime));
    expProgress->setValue((int)(expTime));
    expProgress->setFormat(QString("%vs of %1s remaining").arg((int)(expTime)));
    expTimer->start(1000);  // 1-second update time
  }

  // We also decide if we are going to use the IMCS at this time (it does not
  // start until the shutter opens)

  if (imcsEnabled && expTime >= imcsMinExpTime)
    useIMCS = true;
  else
    useIMCS = false;

}

// Current exposure countdown progress (SLOT connected to the
// expTimer's timeout() signal)

void MODSChannel::expUpdate()
{
  expElapsed++;  // Increment the elapsed exposure time counter

  // Are we done?  If so, stop the countdown timer

  if (expElapsed > (int)(expTime)) { 
    expTimer->stop();  
    expProgress->setValue(0);
    if (imcsEnabled && imcsLoopClosed) {
      sendIECommand(QString("%1IMCS OPEN").arg(channelID));
      imcsLoopDisplay->setChange();
    }
  }

  // We have time remaining, update the exposure progress

  else {
    expRemaining = (int)(expTime) - expElapsed;
    expProgress->setValue(expRemaining);

    // We do IMCS control here since we must key on the current
    // exposure time.  Only use the IMCS if the shutter is open and the IMCS enabled.

    if (shutterOpen && useIMCS) {
      if (imcsLoopClosed) {
	// Open the IMCS correction loop 2 seconds before the shutter closes
	if (expRemaining <= 2) { 
	  sendIECommand(QString("%1IMCS OPEN").arg(channelID));
	  imcsLoopDisplay->setChange();
	}
      }
      else {
	// Close the loop after the first 1 second of exposure has elapsed
	if (expElapsed > 1 && expRemaining > 2) {
	  sendIECommand(QString("%1IMCS CLOSE").arg(channelID));
	  imcsLoopDisplay->setChange();
	}
      }
    }

  }
}

// Set the Camera Filter Wheel ID list

void MODSChannel::setFilterName(QStringList *newList)
{
  filter->setIDList(newList);
}

// Get the Camera Filter Wheel ID list

QStringList MODSChannel::filterName()
{
  return filter->IDList();
}

// Set the Grating ID list

void MODSChannel::setGratingName(QStringList *newList)
{
  grating->setIDList(newList);
}

// Get the ID list

QStringList MODSChannel::gratingName()
{
  return grating->IDList();
}

// Execute an exposure GO

bool MODSChannel::execGo()
{
  QString msgStr;

  errorStr.clear();
  if (imgAcquire) {  // we're already acquiring data...
    errorStr = QString("Exposure sequence already in progress");
    return false;
  }

  imgAcquire = true;
  imgAbort = false;
  imgPause = false;
  enableCCDControls(false);
  expProgress->reset();
  if (expTime >= 1) 
    expProgress->setRange(0,(int)(expTime));
  readProgress->reset();
  readProgress->setRange(0,100);
  goButton->setText(tr("Abort"));
  goButton->setNormal();
  pauseButton->enable();
  stopButton->enable();
  if (numExp > 1) 
    msgStr = tr("Starting %1 CCD Exposure %2 of %3...").arg(channelName).arg(kExp).arg(numExp);
  else
    msgStr = tr("Starting %1 CCD Exposure...").arg(channelName);

  acqStatus->setText(msgStr,Qt::blue);
  sendCmdWait(icHostID,"GO",MODS_QUEUE_REQUEST); // queued dispatch, breaks up race conditions
  return true;

}

// Execute an exposure ABORT and discard it.  To stop an exposure prematurely
// and save the image, see execStop()

void MODSChannel::execAbort()
{
  QString msgStr;

  if (imgAcquire) {
    imgAbort = true;
    imgAcquire = false;
    imgPause = false;
    if (useIMCS && imcsLoopClosed) {
      sendIECommand(QString("%1IMCS OPEN").arg(channelID));
      imcsLoopDisplay->setChange();
    }
    useIMCS = false;
    enableCCDControls(true);
    expProgress->reset();
    readProgress->reset();
    goButton->enable();
    goButton->setText(tr("GO"));
    goButton->setTextColor(Qt::red); // Red means we're aborting...
    pauseButton->disable();
    stopButton->disable();
    if (numExp > 1) {
      msgStr = tr("Aborting %1 CCD Exposure Sequence during image %2 of %3..."
		  ).arg(channelName).arg(kExp).arg(numExp);
      // This trick, setting the exposure kounter (kExp) to numExp,
      // will let the upstream exposure state machine think it is done
      // with the sequence and exit gracefully.  This is mostly to
      // help with aborts originating from scripts so that the script
      // can terminate gracefully.
      kExp = numExp;
    }
    else
      msgStr = tr("Aborting %1 CCD Exposure...").arg(channelName);
  
    expTimer->stop();  // stop the countdown timer
    acqStatus->setText(msgStr,Qt::red);
  
    emit logMessage(tr("STATUS: %1").arg(msgStr));
  }
  sendICCommand("ABORT");  // send even if not in an acquisition, this is benign

}

// Stop an exposure in progress and readout/write the image. 

void MODSChannel::execStop()
{
  QString msgStr;
  if (imgAcquire) {
    if (useIMCS && imcsLoopClosed) {
      sendIECommand(QString("%1IMCS OPEN").arg(channelID));  // open the IMCS loop
      imcsLoopDisplay->setChange();
    }
    sendICCommand("STOP");
    if (numExp > 1) {
      msgStr = tr("Stopping %1 CCD Exposure Sequence after reading out image %2 of %3..."
		  ).arg(channelName).arg(kExp).arg(numExp);
      kExp = numExp;
    }
    else
      msgStr = tr("Stopping %1 CCD Exposure and reading out...").arg(channelName);
  
    expTimer->stop();  // stop the countdown timer
    acqStatus->setText(msgStr,Qt::red);
    emit logMessage(tr("STATUS: %1").arg(msgStr));
    imgAcquire = false;
    imgPause = false;
    imgAbort = false;
    pauseButton->disable();
    stopButton->disable();
  }
}

// Load CCD readout region-of-interest coordinates from an ROI table
// and rebuild the menu items in the Readout pulldown menu (roiEntry)

void MODSChannel::loadROITable(const QString &fileName)
{
  if (fileName.isEmpty()) return;

  // Open the ROI table

  QFile inFile(fileName);
  if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QString errMsg = tr("Could not open the %1-channel CCD ROI mode table %2!\n"
			"Reason:\n %3\nSystem defaults used instead."
			).arg(channelName).arg(fileName).arg(inFile.errorString());
    QMessageBox::warning(this,tr("CCD ROI Table Load Error"),errMsg); 
    return;
  }

  // Clear the entry widget menu, add the first entry

  roiEntry->clear();
  roiEntry->addItem(MODS_CCD_FORMAT,"ROI OFF");

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
      if (!line.startsWith("#")) { // skip comment lines
        if (line.contains("#")) // excise any inline comments
          itemStr = line.section('#',0,0);
        else
          itemStr = line;

	// We have a line, pull out the info we need.  We do not attempt
	// to validate the entries, probably we should but the rules have
	// not yet converged...

	if (!itemStr.isEmpty()) {
	  QString roiName = itemStr.section(' ',0,0);
	  if (!roiName.isEmpty()) {
	    QString roiSpec = itemStr.section(' ',1,-1);
	    if (!roiSpec.isEmpty()) {
	      QString roiCmd = QString("ROI ON %1").arg(roiSpec.simplified());
	      roiEntry->addItem(roiName,roiCmd);
	    }
	  }
	}
      }
    }
  } while (!line.isNull());
  inFile.close();

}

//
// Set the CCD Region-of-Interest either by name or by full spec
//
// Syntax:
//   by-name:  roi 3Kx3K
//   by-spec:  roi llx urx lly ury
//             roi off
//             roi    
//
// Returns true if it the ROI request was valid and the ROI command
// was queued to the CCD controller, false on invalid requests.
//

bool MODSChannel::setCCDROI(const QString &roiStr)
{
  errorStr.clear();
  if (roiStr.isEmpty()) return false;

  QStringList wordList = roiStr.split(" ");  // split the command string into words

  // Sanity check

  if (wordList.at(0).compare("ROI",Qt::CaseInsensitive)!=0) {
    errorStr = QString("unrecognized ROI option '%1'").arg(wordList.at(0));
    return false;
  }

  // If no command arguments, or if the first argument is ON or OFF, 
  // these are spec-type ROI commands, send them as-is and let the IC
  // host do the validation and dirty work

  if (wordList.size() == 1) {
    sendICCommand(roiStr);
    return true;
  }

  // Separate, just in case we need to do something different in the future

  if (wordList.at(1).compare("ON" ,Qt::CaseInsensitive)==0 ||
      wordList.at(1).compare("OFF",Qt::CaseInsensitive)==0) {
    sendICCommand(roiStr);
    return true;
  }

  // ROI FULL is an alias for ROI OFF to return the device to full-frame readout

  if (wordList.at(1).compare("FULL",Qt::CaseInsensitive)==0) {
    sendICCommand("ROI OFF");
    return true;
  }

  // We appear to have a by-name argument.  Look at the roiEntry table
  // and see if it is there with the findText method.  If the requested
  // ROI name is in the list, send the command and set the change color
  // on the widget, otherwise an error.

  int roiIndex = roiEntry->findText(wordList.at(1));
  if (roiIndex > -1) {
    QString roiCmd = roiEntry->itemData(roiIndex);
    roiEntry->setCurrentIndex(roiIndex);
    roiEntry->setChange();
    sendICCommand(roiCmd);
    return true;
  }
  else {
    errorStr = QString("Undefined CCD ROI mode '%1' requested").arg(wordList.at(1));
    return false;
  }
}

//
// Return the most recent error string
//

QString MODSChannel::error()
{
  return errorStr;
}

//
// Set the grating tilt (slot connected to the grtiltEntry widget)
//
// The menu entry consists of a string in the following format
// 
//  gratNum grTilt dA dB dC
//
// gratNum = the grating tilt actuator number, 1 or 2
// grTilt = the grating tilt value to apply
// dA dB dC = the collimator TTF actuator offsets to apply
//

void MODSChannel::setGratingTilt(const QString &grConfig)
{
  if (grConfig.isEmpty()) return;

  bool ok;
  int grNum = grConfig.section(" ",0,0).toInt(&ok,10);
  if (!ok) return;
  int grTilt = grConfig.section(" ",1,1).toInt(&ok,10);
  if (!ok) return;
  int dA = grConfig.section(" ",2,2).toInt(&ok,10);
  if (!ok) return;
  int dB = grConfig.section(" ",3,3).toInt(&ok,10);
  if (!ok) return;
  int dC = grConfig.section(" ",4,4).toInt(&ok,10);
  if (!ok) return;

  int iGrat = 0;
  if (grNum == 1)
    iGrat = 2;
  else if (grNum == 2)
    iGrat = 4;

  int ttfA = nominalTTFA[iGrat-1];
  int ttfB = nominalTTFB[iGrat-1];
  int ttfC = nominalTTFC[iGrat-1];

  QString tiltStr;
  QString ttfStr;
  if (grTilt == 0) {
    tiltStr= QString("%1GRTILT%2 RESET").arg(channelID).arg(grNum);
    ttfStr = QString("%1COLFOC %2 %3 %4").arg(channelID).arg(ttfA).arg(ttfB).arg(ttfC);
  }
  else {
    tiltStr = QString("%1GRTILT%2 %3").arg(channelID).arg(grNum).arg(grTilt);
    ttfA += dA;
    ttfB += dB;
    ttfC += dC;
    ttfStr = QString("%1COLFOC %2 %3 %4").arg(channelID).arg(ttfA).arg(ttfB).arg(ttfC);
  }

  // Queue the grating tilt and collimator TTF offset commands

  grtiltDisplay->setChange();
  ttfaDisplay->setChange();
  ttfbDisplay->setChange();
  ttfcDisplay->setChange();

  if (!tiltStr.isEmpty()) emit doCommand(tiltStr);
  if (!ttfStr.isEmpty()) emit doCommand(ttfStr);
}

// Load a grating preset tilt table from a file.  Grating presets
// consist of a QStringList with 5 entries per string: the preset
// name, the corresponding grating tilt and the nominal collimator
// TTFA/B/C offsets that ensures that the IMCS spot is nearly centered
// on the bypass grating.

void MODSChannel::loadTiltTable(const QString &fileName, const int &gratNum)
{
  if (fileName.isEmpty()) return;
  if (gratNum < 1 || gratNum > 2) return;

  // Open the grating tilt table

  QFile inFile(fileName);
  if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QString errMsg = tr("Could not open the %1-channel grating preset tilt table %2!\n"
			"Reason:\n %3\nSystem defaults used instead."
			).arg(channelName).arg(fileName).arg(inFile.errorString());
    QMessageBox::warning(this,tr("Grating Preset Tilt Table Load Error"),errMsg); 
    return;
  }

  // Clear the grating tilt table

  grtiltTable[gratNum-1]->clear();

  // Read in the table line-by-line, parsing as we go.  # denotes
  // comments (either whole lines or inline comments), and we skip
  // spaces.  The grtiltTable contains strings separated by a space,
  // coding the ID in the first word, the grating tilt value in the
  // second, and the collimator TTF actuator offsets in positions 3
  // thru 5.  Example: "7750A 20000 -14 -9 -6"

  QString itemStr;
  QString line;

  QTextStream in(&inFile);
  do {
    line = in.readLine();
    if (!line.isEmpty()) {  // skip blanks
      line = line.simplified();
      if (!line.startsWith("#")) { // skip comment lines
        if (line.contains("#")) // excise any inline comments
          itemStr = line.section('#',0,0);
        else
          itemStr = line;
	if (!itemStr.isEmpty())
	  grtiltTable[gratNum-1]->append(itemStr);
      }
    }
  } while (!line.isNull());
  inFile.close();

}

//
// Build the grating preset tilt menu for the given tilt position
// The menu datum is 5 numbers: gratNum grTilt dA dB dC
//

void MODSChannel::loadTiltMenu(const int &gratNum)
{
  if (gratNum < 1 || gratNum > 2) return;
}

// Set the current index of the grating tilt menu to the entry
// corresponding to the current grtilt value

void MODSChannel::setCurrentTilt(const int &ipos, const int &gratNum)
{
  if (gratNum < 1 || gratNum > 2) return;

  // Which grtilt menu entry corresponds to ipos?  We must be within 2 units

  if (ipos == 0) { // home position
    grtiltDisplay->setText("Home",Qt::blue);
    grtiltDisplay->setNormal();
  }
  else {  // iterate over the grating table
    bool inTable = false;
    for (int i=0;i<grtiltTable[gratNum-1]->count();i++) {
      QString tiltVal = grtiltTable[gratNum-1]->at(i);
      bool valOK;
      int tiltPos = tiltVal.section(" ",1,1).toInt(&valOK,10);
      if (valOK) {
	if ((ipos >= tiltPos-3) && (ipos <= tiltPos+3)) {
	  grtiltDisplay->setNormal();
	  grtiltDisplay->setText(tiltVal.section(" ",0,1),Qt::blue);
	  inTable = true;
	}
      }
    } // end of table iteration
    if (!inTable) { // Not a preset value, just put the number in
      grtiltDisplay->setNormal();
      grtiltDisplay->setText(QString("*%1").arg(ipos),Qt::blue);
    }
  }
}

// Set the named mechanism widget to the "changing" state.  This is
// used by the parent dashboard to indicate when a device command is
// being executed via the remote channel, providing visual feedback on
// the dashboard GUI.  Currently we show change on slow mechanisms
// like the grating, grating tilt, filter, and camera focus.  

void MODSChannel::setChange(const QString &device)
{
  if (device.isEmpty()) return;
  if (device.compare(filter->command(),Qt::CaseInsensitive)==0)
    filterControl->setChange();
  else if (device.compare(grating->command(),Qt::CaseInsensitive)==0)
    gratingDisplay->setChange();
  else if (device.compare(grtilt->command(),Qt::CaseInsensitive)==0)
    grtiltDisplay->setChange();
  else if (device.compare(camfoc->command(),Qt::CaseInsensitive)==0)
    camfocDisplay->setChange();
  else if (device.compare(colttfa->command(),Qt::CaseInsensitive)==0)
    ttfaDisplay->setChange();
  else if (device.compare(colttfb->command(),Qt::CaseInsensitive)==0)
    ttfbDisplay->setChange();
  else if (device.compare(colttfc->command(),Qt::CaseInsensitive)==0)
    ttfcDisplay->setChange();
  else if (device.compare(QString("%1COLFOC").arg(channelID),
			  Qt::CaseInsensitive)==0) {
    ttfaDisplay->setChange();
    ttfbDisplay->setChange();
    ttfcDisplay->setChange();
  }
  return;
}

// Quick-Configuration Setup Methods

//
// loadConfigTable() - load channel preset configuration table
//
// Load the channel preset configuration table.  This table defines
// the actions taken by the Imaging, Grating, Prism, and Acquire
// quick-config buttons (implemented as the setXXXMode() methods), and
// implements custom configurations available as command-line options
// (or maybe someday via a combobox menu, who knows).  A preset
// configuration puts the instrument channel into a sensible default
// configuration for that use mode.

void MODSChannel::loadConfigTable(const QString &fileName)
{
  bool ok;
  if (fileName.isEmpty()) return;

  // Open the configuration table.  If none is present, disable the
  // presets

  QFile inFile(fileName);
  if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QString errMsg = tr("Could not open the %1-channel preset configuration table %2!\n"
			"Reason:\n %3\nNo presets available for this channel."
			).arg(channelName).arg(fileName).arg(inFile.errorString());
    QMessageBox::warning(this,tr("Preset Configuration Table Load Error"),errMsg); 
    
    // Clear the config table and disable the quick-config buttons
    configTable.clear();
    imagingButton->disable();
    gratingButton->disable();
    prismButton->disable();

    return;
  }

  // Read in the table line-by-line, parsing as we go.  # denotes
  // comments (either whole lines or inline comments) and we skip
  // spaces.  

  configTable.clear();  // Clear the configuration hash table

  QString itemStr;
  QString line;
  QTextStream in(&inFile);
  do {
    line = in.readLine();
    if (!line.isEmpty()) {  // skip blanks
      line = line.simplified();
      if (!line.startsWith("#")) { // skip comment lines
        if (line.contains("#")) // excise any inline comments
          itemStr = line.section('#',0,0);
        else
          itemStr = line;

	// We have a configuration definition, extract the parameters
	// needed to build the command table for that configuration.
	// We'll figure out how to do more sophisticated parameter
	// validation later.

	int iGrat = 0; // grating turret position
	if (!itemStr.isEmpty()) {
	  QString cfgName = itemStr.section(' ',0,0);
	  if (!cfgName.isEmpty()) {
	    QString cfgSpec = itemStr.section(' ',1,-1).simplified();
	    QStringList cmdList;

	    // Get the grating turret assignment if one of the
	    // reserved presets While it seems dangerous to hardwire
	    // this, this mechanism really is hardwired!

	    if (cfgName.compare("Imaging",Qt::CaseInsensitive)==0)
	      iGrat = 1;
	    else if (cfgName.compare("Grating",Qt::CaseInsensitive)==0 ||
		     cfgName.compare("LoRes",Qt::CaseInsensitive)==0)
	      iGrat = 2;
	    else if (cfgName.compare("Prism",Qt::CaseInsensitive)==0)
	      iGrat = 3;
	    else
	      iGrat = 0;  // means not a reserved preset
	      
	    if (!cfgSpec.isEmpty()) {
	      cmdList.clear();

	      // The default grating turret setting or Any if no default (just query)

	      if (cfgSpec.section(' ',0,0).compare("Any",Qt::CaseInsensitive)==0) 
		cmdList.append(QString("%1GRATING").arg(channelID));
	      else
		cmdList.append(QString("%1GRATING %2").arg(channelID).arg(cfgSpec.section(' ',0,0)));


	      // The default filter or Any if no default

	      if (cfgSpec.section(' ',1,1).compare("Any",Qt::CaseInsensitive)==0) 
		cmdList.append(QString("%1FILTER").arg(channelID));
	      else
		cmdList.append(QString("%1FILTER %2").arg(channelID).arg(cfgSpec.section(' ',1,1)));

	      // Nominal camera focus value

	      cmdList.append(QString("%1CAMFOC %2").arg(channelID).arg(cfgSpec.section(' ',2,2)));

	      // The nominal collimator TTF setting for each disperser
	      // must be setup as a command *and* stored in the
	      // relevant data arrays if this is one of the reserved
	      // configurations (Imaging, Grating, etc.)

	      cmdList.append(QString("%1COLFOC %2").arg(channelID).arg(cfgSpec.section(' ',3,5)));
	      if (iGrat > 0) {
		nominalTTFA[iGrat-1] = cfgSpec.section(" ",3,3).toInt(&ok,10);
		nominalTTFB[iGrat-1] = cfgSpec.section(" ",4,4).toInt(&ok,10);
		nominalTTFC[iGrat-1] = cfgSpec.section(" ",5,5).toInt(&ok,10);
	      }

	      // CCD readout Region-of-Interest (ROI)

	      if (cfgSpec.section(' ',6,6).compare("Any",Qt::CaseInsensitive)!=0){
		      if (cfgSpec.section(' ',6,6).compare("Full",Qt::CaseInsensitive)==0) 
		        cmdList.append(QString("%1 ROI OFF").arg(channelName));
		      else
		        cmdList.append(QString("%1 ROI %2").arg(channelName).arg(cfgSpec.section(' ',6,6)));
	      }
        
	      // The CCD binning factor - must be defined

	      cmdList.append(QString("%1 CCDBIN %2").arg(channelName).arg(cfgSpec.section(' ',7,8)));

	      // Set the image type to OBJECT when changing configurations

	      cmdList.append(QString("%1 OBJECT").arg(channelName));
	      
	      // Create the hash table entry for this mode

	      configTable.insert(cfgName.toUpper(),cmdList);

	    }
	  }
	}
      }
    }
  } while (!line.isNull());
  inFile.close();
}

// Set a preset configuration by name - makes presets available to
// parent widgets.  Names are forced to upper case.  Returns -1 if
// the requested configuration does not exist or the number
// of pending commands executed if successful.  We use the global
// instMode[modsID] to determine whether or not to use the single-
// or dual-channel configuration entry

int MODSChannel::setConfig(const QString &cfgName)
{
  errorStr.clear();
  QString cfgStr;

  // Check the configuration name for validity

  if (cfgName.isEmpty()) {
    errorStr = QString("Usage: %1 CONFIG configID").arg(channelName);
    return -1;
  }

  // Are we in single-channel or dual mode?

  cfgStr = cfgName.toUpper();

  // Validate it

  if (!configTable.contains(cfgStr)) {
    errorStr = QString("Unknown %1 channel preset configuration '%2'").arg(channelName).arg(cfgName);
    return -1;
  }

  // Extract the configuration name

  QStringList cmdList = configTable.value(cfgStr);
  if (cmdList.count()==0) {
    errorStr = QString("%1 channel configuration %2 is empty!").arg(channelName).arg(cfgName);
    return -1;
  }
  emit logMessage(QString("Setting %1 channel to %2 mode").arg(channelName).arg(cfgName));
  for (int i=0;i<cmdList.count();i++)
    emit doCommand(cmdList.at(i));
  return cmdList.count();
}

// Is this a valid channel configuration?

bool MODSChannel::validConfig(const QString &cfgName)
{
  if (cfgName.isEmpty()) return false;
  if (configTable.contains(cfgName.toUpper())) return true;
  return false;
}


// Setup a Direct Imaging Mode request for this channel using the
// presets loaded from the channel configuration definition table
// (xconfig.tab).  This puts this channel into a sensible default
// configuration for acquiring images.

void MODSChannel::reqImagingMode()
{
  if (instMode[modsID].compare("dual",Qt::CaseInsensitive)==0)
    reqConfig = "DIMAGING";
  else
    reqConfig = "IMAGING";
  
  // Set the "commit" button to the change state to indicate that the
  // requested configuration change is ready to commit

  imagingButton->setChange();
  commitButton->setChange();
}

// Setup Grating Spectroscopy Mode in this channel using the presets
// loaded from the channel configuration definition table
// (xconfig.tab).  This puts this channel into a sensible default
// configuration for acquiring spectra with the low-resolution
// diffration grating

void MODSChannel::reqGratingMode()
{
  if (instMode[modsID].compare("dual",Qt::CaseInsensitive)==0)
    reqConfig = "DGRATING";
  else
    reqConfig = "GRATING";

  // Set the "commit" button to the change state to indicate that the
  // requested configuration change is ready to commit

  commitButton->setChange();
  gratingButton->setChange();
}

// Setup Prism Spectroscopy Mode in this channel using the presets
// loaded from the channel configuration definition table
// (xconfig.tab).  This puts this channel into a sensible default
// configuration for acquiring spectra with the double-pass prism

void MODSChannel::reqPrismMode()
{
  if (instMode[modsID].compare("dual",Qt::CaseInsensitive)==0)
    reqConfig = "DPRISM";
  else
    reqConfig = "PRISM";

  // Set the "commit" button to the change state to indicate that the
  // requested configuration change is ready to commit

  commitButton->setChange();
  prismButton->setChange();
}

// Commit a requested configuration

void MODSChannel::commitConfig()
{
  if (reqConfig.isEmpty()) return;
  
  // Prevent changing the configuratin during an exposure

  if (imgAcquire || imgPause) {
    emit logMessage(tr("You cannot change the channel configuration during image acquisition"));
    return;
  }

  // Do it..

  if (setConfig(reqConfig) < 0)
    emit logMessage(errorStr);
  else
    setNumExp(1);
  reqConfig.clear();
  commitButton->setNormal();
}

// Clear a requested but uncommitted configuration change

void MODSChannel::clearConfig()
{
  commitButton->setNormal();
  imagingButton->setNormal();
  gratingButton->setNormal();
  prismButton->setNormal();
  switch (grating->pos()) {
  case 1:
    imagingButton->highLight();
    break;

  case 2:
    gratingButton->highLight();
    break;

  case 3:
    prismButton->highLight();
    break;

  }
  reqConfig.clear();
}

//
// Enable/Disable the CCD setup controls
//
// The CCD setup controls should be disabled during acquisition and
// readout.  This convenience function encapsulates that operation for
// disableing and re-enabling the controls.
//

void MODSChannel::enableCCDControls(const bool &enable)
{
  if (enable) {
    objNameEntry->enable();
    imgTypeEntry->enable();
    expTimeEntry->enable();
    numExpEntry->enable();
    xBinEntry->enable();
    yBinEntry->enable();
    roiEntry->enable();
    imcsLockButton->enable();
  }
  else {
    objNameEntry->disable();
    imgTypeEntry->disable();
    expTimeEntry->disable();
    numExpEntry->disable();
    xBinEntry->disable();
    yBinEntry->disable();
    roiEntry->disable();
    imcsLockButton->disable();
  }
}

// Report the image acquisition state 

bool MODSChannel::acquiring()
{
  return imgAcquire;
}

// Report the current exposure status string

QString MODSChannel::expStatus()
{
  return expStatusStr;
}

// Enable/Disable using the IMCS via the checkbox

void MODSChannel::setUseIMCS()
{
  if (useIMCSCheck->isChecked()) {
    imcsEnabled = true;
    if (expTime >= imcsMinExpTime) useIMCS = true;
  }
  else {
    imcsEnabled = false;
    if (useIMCS) {
      sendIECommand(QString("%1IMCS OPEN").arg(channelID));
      imcsLoopDisplay->setChange();
      useIMCS = false;
    }
  }
}

// Method to enable/disable using the IMCS.  Also sets the checkbox

void MODSChannel::setIMCS(const bool &enable)
{
  if (enable) {
    imcsEnabled = true;
    useIMCSCheck->setCheckState(Qt::Checked);
    if (expTime >= imcsMinExpTime) useIMCS = true;
  }
  else {
    imcsEnabled = false;
    useIMCSCheck->setCheckState(Qt::Unchecked);
    if (useIMCS) {
      sendIECommand(QString("%1IMCS OPEN").arg(channelID));
      imcsLoopDisplay->setChange();
      useIMCS = false;
    }
  }
}

// Report if the IMCS is enabled/disabled

bool MODSChannel::usingIMCS()
{
  return imcsEnabled;
}

// Set the minimum exposure time threshold for IMCS use

void MODSChannel::setIMCSExpTime(const double &minExp)
{
  if (minExp <= 0.0) return;
  imcsMinExpTime = minExp;
}

// IMCS Lock-On functions

// Initiate an IMCS Lock-On Request

void MODSChannel::imcsLockOn(const int &wait)
{
  if (wait < MODS_IMCS_MINWAIT) return;

  // Who initiated the lock request, and for how long to wait for lock?
  
  imcsWaitForLock = wait;

  // Queue the first IMCS query, open the shutter, and close the IMCS loop

  sendIECommand(QString("%1IMCS").arg(channelID));
  sendICCommand("SHOPEN -1");
  sendIECommand(QString("%1IMCS CLOSE").arg(channelID));

  // (re)Start the IMCS timer for one imcsCycleTime

  imcsTimer->stop();
  if (imcsCycleTime <= 0) imcsCycleTime = MODS_IMCS_CYCLETIME;  
  imcsTimer->start(imcsCycleTime);

  // Set the IMCS state flags

  imcsLockPending = true;
  imcsOnTarget = false;   
  imcsTimeLeft = imcsWaitForLock*1000; // milliseconds
  imcsLockCount = 0;

  // Indicate the request by changing the lock button state

  imcsLockButton->setChange();
  imcsLoopDisplay->setText("ACTIVE - Lock Requested");
  imcsLoopDisplay->setChange();

}

// Cancel a pending IMCS Lock-On request

void MODSChannel::imcsCancelLock()
{
  imcsTimer->stop();
  if (imcsLockPending) {
    sendIECommand(QString("%1IMCS OPEN").arg(channelID));
    sendICCommand("SHCLOSE");
    imcsLoopDisplay->setText("Idle - Lock-On Canceled");
  }
  imcsLockPending = false;
  imcsTimeLeft = 0;
  imcsOnTarget = false;
  emit imcsLocked(channelName,false);
  imcsLockButton->setNormal();
  imcsLockCount = 0;
}  

// IMCS Lock Query Slot - should be connected to the imcsTimer timeout() signal

void MODSChannel::queryIMCSLock()
{
  imcsTimer->stop(); // stop the timer

  // IMCS has locked-on since the last cycle query, queue opening the loop
  // and closing the shutter

  if (imcsOnTarget) {
    imcsLockCount++;
    if (imcsLockCount >= MODS_IMCS_NUMLOCK) {
      sendIECommand(QString("%1IMCS OPEN").arg(channelID));
      sendICCommand("SHCLOSE");
      imcsLockPending = false;
      emit imcsLocked(channelName,true);
      imcsLockButton->setNormal();
      imcsLockCount = 0;
      imcsLoopDisplay->setText("Idle - Lock-On Done");
      return;
    }
  }

  // We are not yet locked on. If timeout, emit and error and cancel the lock request

  imcsTimeLeft -= imcsCycleTime;
  if (imcsTimeLeft <= 0) { // timeout!
    sendIECommand(QString("%1IMCS OPEN").arg(channelID));
    sendICCommand("SHCLOSE");
    imcsLockPending = false;
    imcsLockButton->setFault();
    emit imcsLocked(channelName,false);
    imcsLockCount = 0;
    imcsLoopDisplay->setText(QString("Fault - No Lock after %1 sec").arg(imcsWaitForLock));
    imcsLoopDisplay->setTextColor(Qt::red);
    return;
  }

  // We have neither locked-on nor timed-out, query the IMCS again and
  // wait another correction cycle for a lock

  imcsLockPending = true;
  sendIECommand(QString("%1IMCS").arg(channelID));
  if (imcsCycleTime <= 0) imcsCycleTime = MODS_IMCS_CYCLETIME;
  imcsTimer->start(imcsCycleTime);

}

// Initiate IMCS Lock-On from the IMCSLock Button (Private Slot)

void MODSChannel::doIMCSLock()
{
  if (imcsLockPending) return;  // IMCS lock-on already in progress
  if (imgAcquire || imgPause) { // disallow lock-on request during exposures
    return;
  }
  imcsLockOn(MODS_IMCS_LOCKTIME);
}

// Force exposure completion.  
//
// This is a somewhat brute-force way to re-enter and resume/complete
// an exposure sequence that is hung up by a communications glitch
// that prevented the IC Host from sending the "DONE: GO ..."
// exposure-completion message.  Performs the same function as when an
// exposure completes normally inside the master message parser method
// (MODSChannel::parse()).
//

void MODSChannel::forceExpDone()
{
  if (numExp > 1)
    expStatusStr = tr("%1 Exposure %2 of %3 completed...").arg(channelName).arg(kExp).arg(numExp);
  else
    expStatusStr = tr("%1 Exposure completed...").arg(channelName);

  acqStatus->setText(expStatusStr,Qt::blue);

  // Update nextfile and lastfile

  sendCmdWait(icHostID,"nextfile",MODS_QUEUE_REQUEST);
  sendCmdWait(icHostID,"lastfile",MODS_QUEUE_REQUEST);
  
  // What do we do next, is this it or is there another image to
  // acquire?  We must test kExp>=numExp otherwise there are some rare
  // abort-service race conditions that cause the exposure counter
  // (kExp) to "jump the fence" and lead to a runaway exposure sequence.

  if (kExp >= numExp) {
    if (numExp > 1) 
      expStatusStr = tr("%1 Exposure Sequence Completed.").arg(channelName);
    else
      expStatusStr = tr("%1 Exposure Completed.").arg(channelName);
    emit logMessage(expStatusStr);
    acqStatus->clear();
    imgAbort = false;
    imgAcquire = false;
    goButton->setText(tr("GO"));
    goButton->setNormal();
    imgPause = false;
    pauseButton->setText(tr("Pause"));
    pauseButton->disable();
    stopButton->disable();
    kExp = 1;
    expProgress->reset();
    readProgress->reset();
    expNumDisplay->clear();
    enableCCDControls(true);
    if (numExp > 1)
      if (useExpBell) qApp->beep();
    expState = MODSChannel::expIdle;
    expStatusStr = tr("Idle");
  }
  else {
    imgAcquire = false;  // we are not yet acquiring the next image...
    kExp++;        // increment the exposure counter
    doExposure();  // start the next exposure
  }
}
