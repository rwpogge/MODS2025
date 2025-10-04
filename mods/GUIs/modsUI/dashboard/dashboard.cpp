#include <QtWidgets>
#include <QtNetwork>
#include <math.h>

#include "dashboard.h"

#include "bistatemechanism.h"
#include "indexedmechanism.h"
#include "linearmechanism.h"

#include "bistatecontrol.h"
#include "indexedselector.h"
#include "linearcontrol.h"
#include "toggleswitch.h"
#include "focalplane.h"
#include "targetpanel.h"
#include "modschannel.h"

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

#include "commlog.h"
#include "warmup.h"

#include "isisclient.h"

DashBoard::DashBoard(const int &myMODS, const int &myPort, QWidget *parent)
  : QWidget(parent)
{
  modsID = myMODS;
  modsNum = modsID+1;

  // Construct the full client name and IMPv2 node name from the
  // instrument number (modsID) and our focal station (modsSide[modsID]).

  if (modsNum > 0) {
    clientName = QString("MODS%1").arg(modsNum);
    clientID = QString("MC%1").arg(modsNum);
  }
  else {
    clientName = "MODS";
    clientID = "MC";
  }

  // We also need to know our UDP socket port, note that clientPort=0
  // means "any free port".

  clientPort = myPort;
  
  // Basic initializations

  remoteCmd = false;
  queue = false;   // disable command dispatch queuing by default
  dispatchCadence = MODS_QUEUE_CADENCE;  // default dispatch queue cadence in msec
  cmdHost.clear(); // clear the command host hash table
  dualCmd.clear(); // clear the implicit dual command hash table

  //----------------------------------------------------------------
  //
  // Build the Instrument Dash Board GUI
  //

  //----------------------------------------------------------------
  //
  // Focal Plane Suite Control Panel
  //
  
  fpControlPanel = new FocalPlane(modsID,this);
  connect(fpControlPanel,SIGNAL(cmdReady(const QString &,const QString &)),
	  this,SLOT(sendToISIS(const QString &,const QString &)));
  connect(fpControlPanel,SIGNAL(cmdWait(const QString &,const QString &,const int &)),
	  this,SLOT(sendAndWait(const QString &,const QString &,const int &)));
  connect(fpControlPanel,SIGNAL(logMessage(const QString &)),
	  this,SLOT(showMessage(const QString &)));
  
  // AGw Filter Table 

  agwFilterName = new QStringList;
  agwFilterInfo = new QStringList;
  loadTable("agwfilt",MODS_AGWFILT_MAX,agwFilterName,agwFilterInfo);
  fpControlPanel->setAGwFiltName(agwFilterName);
  
  int fpw = (int)(fpControlPanel->minimumSizeHint().width());
  
  //----------------------------------------------------------------
  //
  // Telescope target entry panel
  //
   
  targPanel = new TargetPanel(modsID,this);
  connect(targPanel,SIGNAL(cmdReady(const QString &,const QString &)),
	  this,SLOT(sendToISIS(const QString &,const QString &)));
  connect(targPanel,SIGNAL(cmdWait(const QString &,const QString &,const int &)),
	  this,SLOT(sendAndWait(const QString &,const QString &,const int &)));
  connect(targPanel,SIGNAL(logMessage(const QString &)),
	  this,SLOT(showMessage(const QString &)));

  // int tpw = (int)(targPanel->minimumSizeHint().width());

  //----------------------------------------------------------------
  //
  // Red and Blue Channel Control Panels
  //

  // Create a Blue Channel control panel and connect its signals
  
  blueCP = new MODSChannel(modsID,MODSChannel::Blue,this);
  connect(blueCP,SIGNAL(cmdReady(const QString &,const QString &)),
	  this,SLOT(sendToISIS(const QString &,const QString &)));
  connect(blueCP,SIGNAL(cmdWait(const QString &,const QString &,const int &)),
	  this,SLOT(sendAndWait(const QString &,const QString &,const int &)));
  connect(blueCP,SIGNAL(logMessage(const QString &)),
	  this,SLOT(showMessage(const QString &)));
  connect(blueCP,SIGNAL(doCommand(const QString &)),
	  this,SLOT(execCommand(const QString &)));
  connect(blueCP,SIGNAL(imcsLocked(const QString &,const bool &)),
	  this,SLOT(imcsState(const QString &,const bool &)));

  // Blue Grating Turret Assignments
  
  bGratingName = new QStringList;
  bGratingInfo = new QStringList;
  loadTable("bgrating",MODS_GRATING_MAX,bGratingName,bGratingInfo);
  blueCP->setGratingName(bGratingName);

  // Blue Grating Preset Tilt Tables (bgrtilt1 and bgrtilt2).  We have
  // to load these separately for the dashboard (for the command
  // interface) and in the channel GUIs separately (they are used
  // differently in both places).

  loadTiltTable("bgrtilt1");
  loadTiltTable("bgrtilt2");
  blueCP->loadTiltTable(QString("%1/Config/MODS%2/bgrtilt1.tab").arg(MODS_BASE_PATH).arg(modsNum),1);
  blueCP->loadTiltTable(QString("%1/Config/MODS%2/bgrtilt2.tab").arg(MODS_BASE_PATH).arg(modsNum),2);

  // Blue Channel Filter Assignments

  bFilterName = new QStringList;
  bFilterInfo = new QStringList;
  loadTable("bfilter",MODS_CAMFILT_MAX,bFilterName,bFilterInfo);
  blueCP->setFilterName(bFilterName);

  // Create a Red Channel control panel and connect its signals

  redCP = new MODSChannel(modsID,MODSChannel::Red,this);
  connect(redCP,SIGNAL(cmdReady(const QString &,const QString &)),
	  this,SLOT(sendToISIS(const QString &,const QString &)));
  connect(redCP,SIGNAL(cmdWait(const QString &,const QString &,const int &)),
	  this,SLOT(sendAndWait(const QString &,const QString &,const int &)));
  connect(redCP,SIGNAL(logMessage(const QString &)),
	  this,SLOT(showMessage(const QString &)));
  connect(redCP,SIGNAL(doCommand(const QString &)),
	  this,SLOT(execCommand(const QString &)));
  connect(redCP,SIGNAL(imcsLocked(const QString &,const bool &)),
	  this,SLOT(imcsState(const QString &,const bool &)));

  // Make sure the red and blue channel control panels are the same width and
  // height.

  int cpw = qMax(fpw,
		 qMax((int)(blueCP->minimumSizeHint().width()),
		      (int)(redCP->minimumSizeHint().width())));

  int cph = qMax((int)(blueCP->minimumSizeHint().height()),
		 (int)(redCP->minimumSizeHint().height()));

  blueCP->setMinimumWidth(cpw);
  blueCP->setMinimumHeight(cph);
  redCP->setMinimumWidth(cpw);
  redCP->setMinimumHeight(cph);

  // Red Channel Grating Turret Assignments

  rGratingName = new QStringList;
  rGratingInfo = new QStringList;
  loadTable("rgrating",MODS_GRATING_MAX,rGratingName,rGratingInfo);
  redCP->setGratingName(rGratingName);

  // Red Grating Preset Tilt Tables (rgrtilt1 and rgrtilt2).  Same
  // procedure as with the Blue Gratings.

  loadTiltTable("rgrtilt1");
  loadTiltTable("rgrtilt2");
  redCP->loadTiltTable(QString("%1/Config/MODS%2/rgrtilt1.tab").arg(MODS_BASE_PATH).arg(modsNum),1);
  redCP->loadTiltTable(QString("%1/Config/MODS%2/rgrtilt2.tab").arg(MODS_BASE_PATH).arg(modsNum),2);

  // Red Channel Filter Assignments

  rFilterName = new QStringList;
  rFilterInfo = new QStringList;
  loadTable("rfilter",MODS_CAMFILT_MAX,rFilterName,rFilterInfo);
  redCP->setFilterName(rFilterName);
  
  // CCD ROI modes - we use the same set for both channels

  blueCP->loadROITable(QString("%1/Config/MODS%2/ccdroi.tab").arg(MODS_BASE_PATH).arg(modsNum));
   redCP->loadROITable(QString("%1/Config/MODS%2/ccdroi.tab").arg(MODS_BASE_PATH).arg(modsNum));

  // Blue and Red Channel Preset Configurations

  blueCP->loadConfigTable(QString("%1/Config/MODS%2/bconfig.tab").arg(MODS_BASE_PATH).arg(modsNum));
  redCP->loadConfigTable(QString("%1/Config/MODS%2/rconfig.tab").arg(MODS_BASE_PATH).arg(modsNum));

  //----------------------------------------------------------------
  //
  // Command Line Interface widget and status display
  //

  commandEntry  = new TextHistEntry("","",0,256,this);
  connect (commandEntry,SIGNAL(dataReady(const QString &)),this,SLOT(getCommand()));
  commandEntry->clearHistory();

  // System status display

  statusDisplay = new TextDisplay("","",0,this);
  statusDisplay->setTimer(MODS_STATUSMSG_TIME); // status display message persistence

  // Command group box layout

  QGroupBox *cmdGroupBox = new QGroupBox;
  QFormLayout *cmdLayout = new QFormLayout;
  cmdLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
  cmdLayout->setFormAlignment(Qt::AlignHCenter|Qt::AlignTop);
  cmdLayout->setLabelAlignment(Qt::AlignRight);
  cmdLayout->addRow(tr("Command:"),commandEntry);
  cmdLayout->addRow(tr("Status:"),statusDisplay);
  cmdGroupBox->setLayout(cmdLayout);

  //----------------------------------------------------------------

  // The Instrument Configuration selections that go between the
  // focal plane+telescope and the channel control panels

  // Slit Mask Selection 

  slitmask = new IndexedMechanism(tr("Slit Mask"),"SLITMASK",
                                  MODS_SLITMASK_MIN,
                                  MODS_SLITMASK_MAX,30);
  slitControl = new IndexedSelector(slitmask,"Slit Mask:",this);
  connect(slitControl,SIGNAL(cmdReady(const QString &)),
	  this,SLOT(sendIECommand(const QString &)));

  slitmaskName = new QStringList;
  slitmaskInfo = new QStringList;
  loadTable("slitmask",MODS_SLITMASK_MAX,slitmaskName,slitmaskInfo);
  slitmask->setIDList(slitmaskName);

  dichroicName = new QStringList;
  dichroicInfo = new QStringList;
  loadTable("dichroic",MODS_DICHROIC_MAX,dichroicName,dichroicInfo);

  // Slit Mask insert/retract (minsert) mechanism

  minsert = new BiStateMechanism(tr("Slit Mask Insert/Retract"),"MINSERT","IN","STOW",30);
  minsertControl = new BiStateControl(minsert,"","In","Out",this);
  connect(minsertControl,SIGNAL(cmdReady(const QString &)),
	  this,SLOT(sendIECommand(const QString &)));

  // Instrument Mode Selection

  beamConfigEntry = new SelectEntry("Dichroic:","",this);
  beamConfigEntry->scaleHeight(0.9);
  beamConfigEntry->addItem("Dual","DUAL");
  beamConfigEntry->addItem("Blue-Only","BLUE");
  beamConfigEntry->addItem("Red-Only","RED");
  connect(beamConfigEntry,SIGNAL(dataReady(const QString &)),this,SLOT(setDichroic(const QString &)));

  blueConfigEntry = new SelectEntry(" Blue Channel:","",this);
  blueConfigEntry->scaleHeight(0.9);
  blueConfigEntry->addItem("Imaging","IMAGING");
  blueConfigEntry->addItem("Grating","GRATING");
  blueConfigEntry->addItem("Prism","PRISM");
  //blueConfigEntry->addItem("Hi-Res Grating","HIRES");
  connect(blueConfigEntry,SIGNAL(dataReady(const QString &)),this,SLOT(setBlueConfig(const QString &)));

  redConfigEntry = new SelectEntry(" Red Channel:","",this);
  redConfigEntry->scaleHeight(0.9);
  redConfigEntry->addItem("Imaging","IMAGING");
  redConfigEntry->addItem("Grating","GRATING");
  redConfigEntry->addItem("Prism","PRISM");
  //redConfigEntry->addItem("Hi-Res Grating","HIRES");
  connect(redConfigEntry,SIGNAL(dataReady(const QString &)),this,SLOT(setRedConfig(const QString &)));

  // Clear Button - Clear any uncommitted configuration requests

  clearButton = new ActionButton(tr("Clear"),this);
  connect(clearButton,SIGNAL(clicked()),this,SLOT(clearConfig()));
  int mbw = (int)(clearButton->minimumSizeHint().width());
  clearButton->setToolTip(tr("Clears the uncommitted instrument configuration request"));

  // Commit Button - Commit the selected configuration

  commitButton = new ActionButton(tr("Commit"),this);
  connect(commitButton,SIGNAL(clicked()),this,SLOT(commitConfig()));
  mbw = qMax((int)(commitButton->minimumSizeHint().width()),mbw);
  commitButton->setToolTip(tr("Commits the requested instrument configuration to execution"));

  clearButton->setMinimumWidth(mbw);
  commitButton->setMinimumWidth(mbw);

  QGroupBox *maskGroupBox = new QGroupBox(tr("Slit Mask"));
  QHBoxLayout *maskLayout = new QHBoxLayout;
  maskLayout->addWidget(slitControl);
  maskLayout->addWidget(minsertControl);
  maskGroupBox->setLayout(maskLayout);

  QGroupBox *configGroupBox = new QGroupBox(tr("Instrument Configuration"));
  QHBoxLayout *configLayout = new QHBoxLayout;
  configLayout->addWidget(beamConfigEntry);
  configLayout->addSpacing(10);
  configLayout->addWidget(blueConfigEntry);
  configLayout->addSpacing(10);
  configLayout->addWidget(redConfigEntry);
  configLayout->addStretch();
  configLayout->addWidget(commitButton);
  configLayout->addStretch();
  configLayout->addWidget(clearButton);
  configGroupBox->setLayout(configLayout);

  // Give these a dark green color (midway between blue and red)

  QPalette *configPalette = new QPalette;
  *configPalette = configGroupBox->palette();
  configPalette->setColor(QPalette::Mid,Qt::darkGreen);
  configGroupBox->setPalette(*configPalette);
  maskGroupBox->setPalette(*configPalette);

  // Gather together into a single horizontal layout

  QHBoxLayout *maskInstLayout = new QHBoxLayout();
  maskInstLayout->addWidget(maskGroupBox);
  maskInstLayout->addWidget(configGroupBox);

  // Some general buttons to go across the bottom

  // Communications log (modeless CommLog widget) with its view/hide button
  
  commLog = new CommLog(tr("%1 Session Log").arg(clientName),640,256,this);
  connect(commLog,SIGNAL(logHidden()),this,SLOT(setLogButton()));

  commLogButton = new ActionButton(tr("View Comm"));
  connect(commLogButton,SIGNAL(clicked()),this,SLOT(toggleCommLog()));

  // Error/Warning log (modeless CommLog widget) with its view/hide button

  errLog = new CommLog(tr("%1 Error/Warning Log").arg(clientName),640,256,this);
  connect(errLog,SIGNAL(logHidden()),this,SLOT(setErrButton()));
  errLog->seconds(); // we only show timestamps to second precision

  errLogButton = new ActionButton(tr("View Faults"));
  connect(errLogButton,SIGNAL(clicked()),this,SLOT(toggleErrLog()));

  // Control Panel Update button

  updateButton = new ActionButton(tr("Update"),this);
  connect(updateButton,SIGNAL(clicked()),this,SLOT(update()));
  updateButton->setToolTip(tr("Update Control Panel Information"));

  // Control Panel Abort (Panic) button

  /*
  abortButton = new ActionButton(tr("ABORT"),this);
  connect(abortButton,SIGNAL(clicked()),this,SLOT(abort()));
  abortButton->setToolTip(tr("MODS%1 Emergency Abort").arg(modsNum));
  */
  // abortButton->setFont(QFont("Helvetica",18,QFont::Normal));
  // abortButton->setColor(Qt::yellow);

  //----------------------------------------------------------------

  // Layout the control panels on a 2x2 grid, with the command
  // groupbox layout spanning the bottom 2 columns.

  QGridLayout *mainGridLayout = new QGridLayout();
  mainGridLayout->addWidget(fpControlPanel,0,0);
  mainGridLayout->addWidget(targPanel,0,1);
  mainGridLayout->addLayout(maskInstLayout,1,0,1,2);
  mainGridLayout->addWidget(blueCP,2,0);
  mainGridLayout->addWidget(redCP,2,1);
  mainGridLayout->addWidget(cmdGroupBox,3,0,1,2);

  // Box with the update and log view/hide buttons

  QHBoxLayout *buttonLayout = new QHBoxLayout;
  buttonLayout->addWidget(updateButton);
  buttonLayout->addStretch();
  /*
  buttonLayout->addWidget(abortButton);
  buttonLayout->addStretch();
  */
  buttonLayout->addWidget(errLogButton);
  buttonLayout->addWidget(commLogButton);

  // Assemble the main layout

  QVBoxLayout *mainLayout = new QVBoxLayout;
  QLabel *mainLabel = new QLabel(QString("%1 Dashboard").arg(clientName));
  mainLabel->setFont(QFont("Helvetica",(int)(1.5*appFontSize),QFont::Normal));
  mainLayout->addWidget(mainLabel,0,Qt::AlignHCenter);
  //mainLayout->addStretch();
  //mainLayout->addLayout(configLayout);
  //mainLayout->addStretch();
  mainLayout->addLayout(mainGridLayout);
  mainLayout->addStretch();
  mainLayout->addLayout(buttonLayout);
  mainLayout->setContentsMargins(0,0,0,0);
  // mainLayout->setSizeConstraint(QLayout::SetFixedSize);
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
  connect(isis,SIGNAL(logMessage(const QString &)),this,SLOT(passMessage(const QString &)));

  // Dual-Channel Image Acquisition ("DGO") state flags

  isDGO = false;
  blueGoDone = true;
  redGoDone = true;

  // Dual-Channel IMCS state flags and parameters

  isDIMCS = false;
  blueIMCSDone = true;
  blueIMCSLock = false;
  redIMCSDone = true;
  redIMCSLock = false;
  imcsLockHost = "local";

  // Instrument configuration (INSTCONFIG) state flags

  isInstConfig = false;
  isSingleConfig = false;
  numConfigPending = 0;
  numConfigErrors  = 0;

  // Default maximum calibration lamp-on time in seconds (can be set later from the runtime config)

  maxLampTime = MODS_MAX_LAMPTIME;

  // Setup the environmental status monitor query timer and connect
  // its slot, but do not start it yet

  envTimer = new QTimer(this);
  connect(envTimer,SIGNAL(timeout()),this,SLOT(envUpdate()));

  // Anything else for the constructor to do?

}


//---------------------------------------------------------------------------
//
// getCommand() slot - invoked by the command entry widget
//

void DashBoard::getCommand()
{
  QString cmdStr;

  remoteCmd = false;  // Command originated locally...
  remoteHost = "local";

  cmdStr = commandEntry->value();
  commandEntry->clear();

  cmdStr = cmdStr.trimmed();  // Clean up whitespace & terminators
  if (cmdStr.isEmpty()) return;

  if (!cmdStr.startsWith(">")) {
    QString msgStr = "Command: ";
    msgStr.append(cmdStr);
    commLog->append(msgStr,Qt::black); 
  }

  QString *repStr = new QString("");
  doCommand(cmdStr,repStr);
  
}

//---------------------------------------------------------------------------
//
// execCommand() slot - Execute a command passed to us from a sibling or child
//

void DashBoard::execCommand(const QString &cmdStr)
{
  if (cmdStr.isEmpty()) return;

  remoteCmd = false;
  remoteHost = "local";

  QString command = cmdStr.simplified();  // strip off junk and extraneous whitespace

  if (!command.startsWith(">")) {
    QString msgStr = "Command: ";
    msgStr.append(cmdStr);
    commLog->append(msgStr,Qt::black); 
  }

  QString *repStr = new QString("");
  doCommand(cmdStr,repStr);

  // we should probably do something if repStr is not empty...
}

//---------------------------------------------------------------------------
//
// csqCommand() slot - Execute a command passed to us from a command
//                     sequencer (CSQ) instance
//

void DashBoard::csqCommand(const QString &cmdStr)
{
  if (cmdStr.isEmpty()) return;

  remoteCmd = false;
  remoteHost = "local-csq";

  QString command = cmdStr.simplified();  // strip off junk and extraneous whitespace

  if (!command.startsWith(">")) {
    QString msgStr = "Command: ";
    msgStr.append(cmdStr);
    commLog->append(msgStr,Qt::black); 
  }

  QString *repStr = new QString("");
  doCommand(cmdStr,repStr);

  if (!repStr->isEmpty())
    emit csqMessage(*repStr);
 
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
// If the command is local (entered via commandEntry), we don't
// worry about routing replies.  If from a remote client (e.g.,
// one of various command scripting agents), we route the
// reply accordingly using the cmdHost hash table.
//

void DashBoard::doCommand(const QString &cmdStr, QString *replyStr)
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

  // Here we implement interactive commands...

  // Global Abort with no arguments

  else if (cmdStr.compare("abort",Qt::CaseInsensitive)==0) {

    // Store the remote host associated with this command in the hash for
    // each remote host.

    /*
    // Abort to the IE Host - note: disabled, as IE has no global abort
    // only mechanism-specific abort directives are implemented

    cmdKey = QString("%1-%2").arg(modsIEHost[modsID]).arg(cmdStr.toUpper());
    cmdHost.insert(cmdKey,remoteHost);
    sendIECommand(cmdStr);
    */


    /*
    // Abort the Blue CCD controller

    cmdKey = QString("%1-%2").arg(modsBCHost[modsID]).arg(cmdStr.toUpper());
    cmdHost.insert(cmdKey,remoteHost);
    sendAndWait(modsBCHost[modsID],cmdStr,MODS_IMMEDIATE_REQUEST);

    // Abort the Red CCD controller

    cmdKey = QString("%1-%2").arg(modsRCHost[modsID]).arg(cmdStr.toUpper());
    cmdHost.insert(cmdKey,remoteHost);
    sendAndWait(modsRCHost[modsID],cmdStr,MODS_IMMEDIATE_REQUEST);
    */

    // Abort the red and blue channels (if no exposure is in progress
    // on a channel this is benign.

    blueCP->execAbort();
    redCP->execAbort();

    // Clear any state flags

    isDGO = false;
    blueGoDone = true;
    redGoDone = true;

    // And let the requesting party know the abort has been requested,
    // but because the actual abort service is done by asynchronous
    // agents, they have to wait until those agents are done before
    // proceeding, and not all agents actually respond.

    *replyStr = QString("DONE: ABORT requested - wait for abort servicing to complete.");
    if (!remoteCmd) addStatus(*replyStr,Qt::darkGreen,false);
  }    

  // Raw IE commands with no Blue/Red channel prefix

  else if (cmdWord.compare("hatch"     ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("calib"     ,Qt::CaseInsensitive)==0 || 
	   cmdWord.compare("agw"       ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("agwx"      ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("agwy"      ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("agwfoc"    ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("agwfilt"   ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("gprobe"    ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("gpoffset"  ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("gpfocus"   ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("slitmask"  ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("mselect"   ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("minsert"   ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("readmask"  ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("dichroic"  ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("lamp"      ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("irlaser"   ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("vislaser"  ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("ieb"       ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("heb"       ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("llb"       ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("util"      ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("istatus"   ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("estatus"   ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("pstatus"   ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("startup"   ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("obsmode"   ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("calmode"   ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("stow"      ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("shutdown"  ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("sleep"     ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("wake"      ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("saveconfig",Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("home"      ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("reset"     ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("abort"     ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("panic"     ,Qt::CaseInsensitive)==0) {
    
    QStringList wordList = cmdStr.split(" ");

    // Store the remote host associated with this command in the hash

    cmdKey = QString("%1-%2").arg(modsIEHost[modsID]).arg(cmdWord.toUpper());
    cmdHost.insert(cmdKey,remoteHost);

    // Special handling for some commands, others are passed as-is to
    // the IE Host.  

    // Dichroic, AGw Filter, and Slitmask can be selected by number or
    // by name (case-insensitive exact match)

    if (cmdWord.compare("dichroic",Qt::CaseInsensitive)==0 || 
	cmdWord.compare("agwfilt" ,Qt::CaseInsensitive)==0 ||
	cmdWord.compare("slitmask",Qt::CaseInsensitive)==0 || 
	cmdWord.compare("mselect" ,Qt::CaseInsensitive)==0) {
      int iPos;

      if (cmdWord.compare("mselect",Qt::CaseInsensitive)==0) {
	QString slitCmd = cmdStr;
	slitCmd.replace("mselect","slitmask");
	iPos = posByName(slitCmd);
      }
      else
	iPos = posByName(cmdStr);
      if (iPos < 0) {
	*replyStr = QString("ERROR: %1").arg(errorStr);
	cmdHost.remove(cmdKey);
	if (!remoteCmd)
	  addStatus(*replyStr,Qt::red,false);
	remoteCmd = false;
	return;
      }
      else if (iPos == 0) 
	sendIECommand(cmdStr);  // send as-is
      else
	sendIECommand(QString("%1 %2").arg(cmdWord).arg(iPos));

      if (cmdWord.compare("slitmask",Qt::CaseInsensitive)==0 ||
	  cmdWord.compare("mselect",Qt::CaseInsensitive)==0) {
	slitControl->setChange();
	minsertControl->setChange();
      }
      else if (cmdWord.compare("dichroic",Qt::CaseInsensitive)==0) 
	beamConfigEntry->setChange();
    }

    else if (cmdWord.compare("minsert",Qt::CaseInsensitive)==0) {
      minsertControl->setChange();
      sendIECommand(cmdStr);
    }

    // Calibration lamp commands are handled through the focalplane
    // control panel so that we may properly trigger the lamp timer
    // and light the lamp power-indicator "LED".

    else if (cmdWord.compare("lamp",Qt::CaseInsensitive)==0)
      fpControlPanel->setLamp(cmdStr);

    // Mask is used to insert/retract a mask - slitmask selects by name

    else if (cmdWord.compare("mask",Qt::CaseInsensitive)==0) 
      qDebug() << "mask command not yet implemented";


    // OBSMODE and CALMODE should change the state of the dashboard to show
    // which mechanisms are in play

    else if (cmdWord.compare("calmode",Qt::CaseInsensitive)==0 ||
	     cmdWord.compare("obsmode",Qt::CaseInsensitive)==0) {
      fpControlPanel->setChange(cmdWord);
      sendIECommand(cmdStr);
    }
    
    // Otherwise no special handling required, send as-is

    else 
      sendIECommand(cmdStr);

    if (wordList.count() > 1) 
      fpControlPanel->setChange(cmdWord);
  }

  // Raw IE commands with Blue/Red channel prefixes

  else if ((cmdWord.startsWith("B",Qt::CaseInsensitive) ||
	    cmdWord.startsWith("R",Qt::CaseInsensitive)) && (
	   cmdWord.contains("colttfa",Qt::CaseInsensitive) ||
	   cmdWord.contains("colttfb",Qt::CaseInsensitive) ||
	   cmdWord.contains("colttfc",Qt::CaseInsensitive) ||
	   cmdWord.contains("colfoc" ,Qt::CaseInsensitive) ||
	   cmdWord.contains("grating",Qt::CaseInsensitive) ||
 	   cmdWord.contains("grtilt1",Qt::CaseInsensitive) ||
 	   cmdWord.contains("grtilt2",Qt::CaseInsensitive) ||
	   cmdWord.contains("camfoc" ,Qt::CaseInsensitive) ||
	   cmdWord.contains("filter" ,Qt::CaseInsensitive) ||
	   cmdWord.contains("imcs"   ,Qt::CaseInsensitive))) {

    bool isRed = false;
    if (cmdWord.startsWith("R",Qt::CaseInsensitive)) isRed = true;

    // Store the remote host associated with this command in the hash

    cmdKey = QString("%1-%2").arg(modsIEHost[modsID]).arg(cmdWord.toUpper());
    cmdHost.insert(cmdKey,remoteHost);
    QStringList wordList = cmdStr.split(" ");

    //
    // Some commands require special handling.  
    //
    // * Filters and Gratings may be selected by number or name, the
    //   latter via a case-insensitive exact match from the relevant
    //   device table.
    //

    if (cmdWord.compare("bfilter",Qt::CaseInsensitive)==0 ||
	cmdWord.compare("rfilter",Qt::CaseInsensitive)==0 ||
	cmdWord.compare("bgrating",Qt::CaseInsensitive)==0 ||
	cmdWord.compare("rgrating",Qt::CaseInsensitive)==0) {
      int iPos = posByName(cmdStr);
      if (iPos < 0) {
	*replyStr = QString("ERROR: %1").arg(errorStr);
	cmdHost.remove(cmdKey);
	if (!remoteCmd)
	  addStatus(*replyStr,Qt::red,false);
	remoteCmd = false;
	return;
      }
      else if (iPos == 0) 
	sendIECommand(cmdStr);  // send as-is
      else
	sendIECommand(QString("%1 %2").arg(cmdWord).arg(iPos));

      if (wordList.count() > 1) {
	if (isRed)
	  redCP->setChange(cmdWord);
	else
	  blueCP->setChange(cmdWord);
      }
      return;
    }

    // * Grating tilt may be by number or preset tilt name.  For
    //   by-name selection we force to upper case, as was done when
    //   filling the table, and require an exact match.

    else if (cmdWord.compare("bgrtilt1",Qt::CaseInsensitive)==0 ||
	     cmdWord.compare("bgrtilt2",Qt::CaseInsensitive)==0 ||
	     cmdWord.compare("rgrtilt1",Qt::CaseInsensitive)==0 ||
	     cmdWord.compare("rgrtilt2",Qt::CaseInsensitive)==0) {
      int iTilt = tiltByName(cmdStr);
      if (iTilt < 0) {
	*replyStr = QString("ERROR: %1").arg(errorStr);
	cmdHost.remove(cmdKey);
	if (!remoteCmd)
	  addStatus(*replyStr,Qt::red,false);
	remoteCmd = false;
	return;
      }
      else if (iTilt == 0) 
	sendIECommand(cmdStr);  // send as-is
      else
	sendIECommand(QString("%1 %2").arg(cmdWord).arg(iTilt));

      if (cmdWord.startsWith("r",Qt::CaseInsensitive))
	redCP->setChange(cmdWord);
      else
	blueCP->setChange(cmdWord);

      return;
    }

    // No special handling required, send the command as-is

    else 
      sendIECommand(cmdStr);

    if (wordList.count() > 1) {
      if (isRed)
	redCP->setChange(cmdWord);
      else
	blueCP->setChange(cmdWord);
    }

  }

  // Blue or Red Channel commands for their respective channels are
  // handled here.  We treat ABORT and GO differently, otherwise most
  // other channel commands are sent as-is, we let the respective
  // hosts handle validation as required.

  else if (cmdWord.compare("RED",Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("BLUE",Qt::CaseInsensitive)==0) {

    QString destHost;
    QString dmHost;
    bool isRed = false;
    QString chPrefix;

    if (cmdWord.compare("RED",Qt::CaseInsensitive)==0) {
      destHost = modsRCHost[modsID];
      dmHost = modsRDHost[modsID];
      isRed = true;
      chPrefix = "r";
    }
    else {
      destHost = modsBCHost[modsID];
      dmHost = modsBDHost[modsID];
      isRed = false;
      chPrefix = "b";
    }

    QString chanCmd = cmdStr.section(" ",1,-1);  // full command string
    QString chanWord = cmdStr.section(" ",1,1);  // just the command word
    QString chanArgs = cmdStr.section(" ",2,-1); // just the arguments

    // We alias a small number of channel mechanism commands here,
    // e.g., "red filter" = "rfilter", etc., so extra logic is
    // required to sort this all out

    // Filter or Grating can be set by number or name

    if (chanWord.compare("filter" ,Qt::CaseInsensitive)==0 ||
	chanWord.compare("grating",Qt::CaseInsensitive)==0) {

      chanCmd = QString("%1%2").arg(chPrefix).arg(cmdStr.section(" ",1,-1));
      QString mechWord = QString("%1%2").arg(chPrefix).arg(cmdStr.section(" ",1,1));
      cmdKey = QString("%1-%2").arg(modsIEHost[modsID]).arg(mechWord.toUpper());
      cmdHost.insert(cmdKey,remoteHost);

      int iPos = posByName(chanCmd);
      if (iPos < 0) {
	*replyStr = QString("ERROR: %1 %2 - %3").arg(cmdWord).arg(chanWord).arg(errorStr);
	cmdHost.remove(cmdKey);
	if (!remoteCmd)
	  addStatus(*replyStr,Qt::red,false);
	remoteCmd = false;
	return;
      }
      else if (iPos == 0) 
	sendIECommand(chanCmd);
      else
	sendIECommand(QString("%1 %2").arg(mechWord).arg(iPos));

      if (isRed)
	redCP->setChange(mechWord);
      else
	blueCP->setChange(mechWord);
      return;
    }

    // Grating Tilt can be set by number or name

    else if (chanWord.compare("grtilt1" ,Qt::CaseInsensitive)==0 ||
	     chanWord.compare("grtilt2",Qt::CaseInsensitive)==0) {

      chanCmd = QString("%1%2").arg(chPrefix).arg(cmdStr.section(" ",1,-1));
      QString mechWord = QString("%1%2").arg(chPrefix).arg(cmdStr.section(" ",1,1));
      cmdKey = QString("%1-%2").arg(modsIEHost[modsID]).arg(mechWord.toUpper());
      cmdHost.insert(cmdKey,remoteHost);

      int iTilt = tiltByName(chanCmd);
      if (iTilt < 0) {
	*replyStr = QString("ERROR: %1 %2 - %3").arg(cmdWord).arg(chanWord).arg(errorStr);
	cmdHost.remove(cmdKey);
	if (!remoteCmd)
	  addStatus(*replyStr,Qt::red,false);
	remoteCmd = false;
	return;
      }
      else if (iTilt == 0) 
	sendIECommand(chanCmd);  // send as-is
      else
	sendIECommand(QString("%1 %2").arg(mechWord).arg(iTilt));

      if (isRed)
	redCP->setChange(mechWord);
      else
	blueCP->setChange(mechWord);
      return;
    }

    // CENLAM or CENWAVE (aliases) - Set grating tilt by central wavelength.
    // This is a prototype that is messy as it assumes grtilt1 (the only tiltalble
    // grating we have).  We'll fix this eventually

    else if (chanWord.compare("cenlam" ,Qt::CaseInsensitive)==0 ||
	     chanWord.compare("cenwave",Qt::CaseInsensitive)==0) {

      QString argStr = cmdStr.section(" ",2,-1);
      if (argStr.isEmpty()) {
	*replyStr = QString("ERROR: %1 %2 missing command line argument"
			    ).arg(cmdWord).arg(chanWord);
	if (!remoteCmd) addStatus(*replyStr,Qt::red,false);
	remoteCmd = false;
	return;
      }

      // Make sure A is part of the name, just in case...

      if (!argStr.contains("A",Qt::CaseInsensitive)) argStr.append("A");

      // Build the grating tilt command.

      chanCmd = QString("%1GRTILT1 %2").arg(chPrefix).arg(argStr);
      QString mechWord = QString("%1GRTILT1").arg(chPrefix);
      cmdKey = QString("%1-%2").arg(modsIEHost[modsID]).arg(mechWord.toUpper());
      cmdHost.insert(cmdKey,remoteHost);

      int iTilt = tiltByName(chanCmd);
      if (iTilt < 0) {
	*replyStr = QString("ERROR: %1 %2 - %3").arg(cmdWord).arg(chanWord).arg(errorStr);
	cmdHost.remove(cmdKey);
	if (!remoteCmd)
	  addStatus(*replyStr,Qt::red,false);
	remoteCmd = false;
	return;
      }
      else if (iTilt == 0) 
	sendIECommand(chanCmd);  // send as-is
      else
	sendIECommand(QString("%1 %2").arg(mechWord).arg(iTilt));

      if (isRed)
	redCP->setChange(mechWord);
      else
	blueCP->setChange(mechWord);
      return;
    }

    // Camera focus and imcs are sent as-is after prefixing with r|b

    else if (chanWord.compare("camfoc" ,Qt::CaseInsensitive)==0 ||
	     chanWord.compare("imcs"   ,Qt::CaseInsensitive)==0) {
      destHost = modsIEHost[modsID];
      chanCmd = QString("%1%2").arg(chPrefix).arg(cmdStr.section(" ",1,-1));
      chanWord = QString("%1%2").arg(chPrefix).arg(cmdStr.section(" ",1,1));
      cmdKey = QString("%1-%2").arg(destHost).arg(chanWord.toUpper());

      sendIECommand(chanCmd);
      if (isRed)
	redCP->setChange(chanWord);
      else
	blueCP->setChange(chanWord);
      return;
    }
    // put IGPOWER and ARCHON here, go to IE as "HEB <chan> <cmd>"

    // CONFIG is a pseudo-local command

    else if (chanWord.compare("config",Qt::CaseInsensitive)==0)
      cmdKey = "LOCAL-CONFIG";

    else 
      cmdKey = QString("%1-%2").arg(destHost).arg(chanWord.toUpper());

    cmdHost.insert(cmdKey,remoteHost);

    // ABORTS are dispatched immediately, by way of the channel control
    // panel execAbort() method

    if (chanWord.compare("ABORT",Qt::CaseInsensitive)==0) {
      if (isRed)
	redCP->execAbort();
      else
	blueCP->execAbort();
      if (isDGO) {
	isDGO = false;
	blueGoDone = true;
	redGoDone = true;
      }
    }

    // Pause/Resume attempts to pause/resume an exposure.  Use the
    // mechanisms in the channel control panels to implement this.

    else if (chanWord.compare("PAUSE",Qt::CaseInsensitive)==0 ||
	chanWord.compare("RESUME",Qt::CaseInsensitive)==0) {
      if (isRed)
	redCP->pauseExposure();
      else
	blueCP->pauseExposure();
    }

    // Stop will stop an exposure in progress and read it out (abort with save)

    else if (chanWord.compare("STOP",Qt::CaseInsensitive)==0) {
      if (isRed)
	redCP->execStop();
      else
	blueCP->execStop();
    }

    // RESET is a local command, it resets and clears the exposure
    // control panel

    else if (chanWord.compare("RESET",Qt::CaseInsensitive)==0) {
      if (isRed) {
	redCP->resetExp();
	*replyStr = QString("DONE: %1 RESET - Red Channel exposure control panel reset"
			    ).arg(cmdWord);
      }
      else {
	blueCP->resetExp();
	*replyStr = QString("DONE: %1 RESET - Blue Channel exposure control panel reset"
			    ).arg(cmdWord);
      }
    }

    // EXPDONE is a local command, it forces the named channel to
    // perform the actions taken during normal exposure completion -
    // used to unstick the control panel when the exposure hangs
    // because the "DONE: GO ..." message was not received (e.g., comm
    // drops).  Less extreme than RESET above.

    else if (chanWord.compare("EXPDONE",Qt::CaseInsensitive)==0) {
      if (isRed) {
	redCP->forceExpDone();
	*replyStr = QString("DONE: %1 EXPDONE - Attempting to force red-channel exposure completion servicing"
			    ).arg(cmdWord);
      }
      else {
	blueCP->forceExpDone();
	*replyStr = QString("DONE: %1 EXPDONE - Attempting to force blue-channel exposure completion servicing"
			    ).arg(cmdWord);
      }
    }

    /*
    // LASTFILE is now on the CCD host not DM host after the 2025 Archon update [rwp/osu]

    // The LASTFILE command goes out to the IC host for the specified channel

    else if (chanWord.compare("LASTFILE",Qt::CaseInsensitive)==0) {
      sendToISIS(dmHost,chanCmd);
	
    }
    */
    /*
    // FITSFLUSH is retired after the 2025 Archon update [rwp/osu]

    // The FITSFLUSH command fires a +SWAP command to the data-manager host for the
    // specified channel

    else if (chanWord.compare("FITSFLUSH",Qt::CaseInsensitive)==0) {
      sendToISIS(dmHost,"+SWAP");
      *replyStr = QString("DONE: %1 FITSFLUSH - pending FITS transfer request completed").arg(cmdWord);
      if (!remoteCmd) addStatus(*replyStr,Qt::darkGreen,false);
      cmdHost.remove(cmdKey);
      return;
    }
    */
    
    // IMCSLOCK - request IMCS lock-on for this channel

    else if (chanWord.compare("IMCSLOCK",Qt::CaseInsensitive)==0) {

      isDIMCS = false; // single-channel lock request
      imcsLockHost = remoteHost;

      QStringList argList = chanCmd.split(" ");
      int waitTime;
      if (argList.size() == 1) { // use the default time
	waitTime = MODS_IMCS_LOCKTIME;
      }
      else if (argList.size() == 2) {
	bool isOK;
	waitTime = argList.at(1).toInt(&isOK,10);
	if (!isOK || waitTime < MODS_IMCS_MINWAIT) {
	  *replyStr = QString("ERROR: %1 IMCSLOCK invalid IMCS wait-for-lock time '%2'"
			      " - Usage: %1 IMCSLOCK [wait]").arg(cmdWord).arg(argList.at(1));
	  if (!remoteCmd) addStatus(*replyStr,Qt::red,false);
	  cmdHost.remove(cmdKey);
	  return;
	}
      }
      else {
	*replyStr = QString("ERROR: %1 IMCSLOCK invalid arguments, "
			    "Usage: %1 IMCSLOCK [wait]").arg(cmdWord);
	if (!remoteCmd) addStatus(*replyStr,Qt::red,false);
	cmdHost.remove(cmdKey);
	return;
      }

      // We have a wait time, see if we can execute the IMCS lock-on request

      if (isRed) {
	if (redCP->acquiring()) {
	  *replyStr = QString("ERROR: %1 IMCSLOCK disallowed during exposures").arg(cmdWord);
	  if (!remoteCmd) addStatus(*replyStr,Qt::red,false);
	  cmdHost.remove(cmdKey);
	  return;
	}
	else
	  redCP->imcsLockOn(waitTime);
      }
      else {
	if (blueCP->acquiring()) {
	  *replyStr = QString("ERROR: %1 IMCSLOCK disallowed during exposures").arg(cmdWord);
	  if (!remoteCmd) addStatus(*replyStr,Qt::red,false);
	  cmdHost.remove(cmdKey);
	  return;
	}
	else
	  blueCP->imcsLockOn(waitTime);
      }
      cmdHost.remove(cmdKey);
      return;
    }

    // IMCSABORT or IMCSCANCEL will cancel a pending IMCS lock-on request

    else if (chanWord.compare("IMCSABORT",Qt::CaseInsensitive)==0 ||
	     chanWord.compare("IMCSCANCEL",Qt::CaseInsensitive)==0) {
      if (isRed) {
	if (redCP->acquiring()) {
	  *replyStr = QString("ERROR: %1 %2 disallowed during exposures").arg(cmdWord).arg(chanWord);
	  cmdHost.remove(cmdKey);
	  return;
	}
	else 
	  redCP->imcsCancelLock();
      }
      else {
	if (blueCP->acquiring()) {
	  *replyStr = QString("ERROR: %1 %2 disallowed during exposures").arg(cmdWord).arg(chanWord);
	  cmdHost.remove(cmdKey);
	  return;
	}
	else
	  blueCP->imcsCancelLock();
      }
      *replyStr = QString("DONE: %1 %2 sent").arg(cmdWord).arg(chanWord);
      cmdHost.remove(cmdKey);
      return;
    }
    
    // The CONFIG command is used to set one of the preset channel
    // configurations defined in the bconfig.tab and rconfig.tab files
    // for this instrument instance.

    else if (chanWord.compare("CONFIG",Qt::CaseInsensitive)==0) {
      QString cfgStr = chanCmd.section(" ",1,1);
      if (cfgStr.isEmpty()) {
	  *replyStr = QString("ERROR: %1 CONFIG syntax error - Usage: %2 CONFIG configID").arg(cmdWord).arg(cmdWord);
	  if (!remoteCmd) addStatus(*replyStr,Qt::red,false);
	  cmdHost.remove(cmdKey);
	  return;
      }

      // If we are in dual mode, make sure we invoke the dual version of the configuration

      if (instMode[modsID].compare("dual",Qt::CaseInsensitive)==0 && 
	  !cfgStr.startsWith("D",Qt::CaseInsensitive)) 
	cfgStr.prepend("D");

      // Validate the configuration requested and then execute it

      if (isRed) { // Red Channel
	if (!redCP->validConfig(cfgStr)) {
	  *replyStr = QString("ERROR: %1 CONFIG undefined red-channel preset %2").arg(cmdWord).arg(cfgStr);
	  if (!remoteCmd) addStatus(*replyStr,Qt::red,false);
	  cmdHost.remove(cmdKey);
	  return;
	}
	numConfigPending = redCP->setConfig(cfgStr);
	if (numConfigPending>0) {
	  isInstConfig = true;
	  isSingleConfig = true;
	  numConfigErrors = 0;
	  addStatus(QString("STATUS: %1 CONFIG initiated").arg(cmdWord),Qt::blue,false);
	}
	else {
	  numConfigPending = 0;
	  *replyStr = QString("ERROR: %1 CONFIG could not execute red-channel preset %2 - %3"
			      ).arg(cmdWord).arg(cfgStr).arg(redCP->error());
	  if (!remoteCmd) addStatus(*replyStr,Qt::red,false);
	  cmdHost.remove(cmdKey);
	  return;
	}
      }
      else { // Blue Channel
	if (!blueCP->validConfig(cfgStr)) {
	  *replyStr = QString("ERROR: %1 CONFIG undefined blue-channel preset %2").arg(cmdWord).arg(cfgStr);
	  if (!remoteCmd) addStatus(*replyStr,Qt::red,false);
	  cmdHost.remove(cmdKey);
	  return;
	}
	numConfigPending = blueCP->setConfig(cfgStr);
	if (numConfigPending>0) {
	  isInstConfig = true;
	  isSingleConfig = true;
	  numConfigErrors = 0;
	  addStatus(QString("STATUS: %1 CONFIG initiated").arg(cmdWord),Qt::blue,false);
	}
	else {
	  numConfigPending = 0;
	  *replyStr = QString("ERROR: %1 CONFIG could not execute blue-channel preset %2 - %3"
			      ).arg(cmdWord).arg(cfgStr).arg(blueCP->error());
	  if (!remoteCmd) addStatus(*replyStr,Qt::red,false);
	  cmdHost.remove(cmdKey);
	  return;
	}
      }
      return;
    }

    // ROI commands can be by name or spec.  Let the channel widget handle it

    else if (chanWord.compare("ROI",Qt::CaseInsensitive)==0) {
      if (isRed)
	if (redCP->setCCDROI(chanCmd)) 
	  return;
	else
	  *replyStr = QString("ERROR: %1 ROI %2").arg(cmdWord).arg(redCP->error());
      else
	if (blueCP->setCCDROI(chanCmd)) 
	  return;
	else
	  *replyStr = QString("ERROR: %1 ROI %2").arg(cmdWord).arg(blueCP->error());
      
      // if we got here, we had a fault...
      
      cmdHost.remove(cmdKey);
      if (!remoteCmd)
	addStatus(*replyStr,Qt::red,false);
      remoteCmd = false;
      return;
    }

    // GO [n] - start one or more exposures
    //
    // Set the channel control panel's numExp value, and then execute
    // the GO via the channel control panel's execGo() method.  If an
    // exposure is already in progress, we flag it as an error and
    // close the transaction.
    // 
    // If "GO" is sent without arguments, we use the current value of
    // numExp, otherwise we set numExp on the channel if within the
    // valid range.

    else if (chanWord.compare("GO",Qt::CaseInsensitive)==0) {
      QStringList goList = chanCmd.split(" ");
      int numGo;
      bool goOK;
      if (goList.size() > 1) {
	bool ok;
	numGo = goList.at(1).toInt(&ok,10);
	if (ok && (numGo > 0 && numGo <= MODS_MAX_NUMEXP)) {
	  if (isRed)
	    redCP->setNumExp(numGo);
	  else
	    blueCP->setNumExp(numGo);
	}
	else {
	  *replyStr = QString("ERROR: %1 GO Invalid number of exposures %2, must be 1..%3"
			      ).arg(cmdWord).arg(goList.at(1)).arg(MODS_MAX_NUMEXP);
	  cmdHost.remove(cmdKey);
	  if (!remoteCmd)
	    addStatus(*replyStr,Qt::red,false);
	  remoteCmd = false;
	  return;
	}
      }

      // We have a valid number of exposures to acquire, start the
      // acquisition via the channel control panel's execGo() method.

      if (isRed)
	goOK = redCP->execGo();
      else
	goOK = blueCP->execGo();

      // Was the GO initiation successful?  If not, squawk.

      if (!goOK) {
	*replyStr = QString("ERROR: %1 GO exposure already in progress").arg(cmdWord);
	cmdHost.remove(cmdKey);
	if (!remoteCmd)
	  addStatus(*replyStr,Qt::red,false);
	remoteCmd = false;
	return;
      }

      *replyStr = QString("STATUS: %1 initiated").arg(cmdStr);

    }

    // NUMEXP - set the number of exposures to acquire.  Used by DGO.
    // This is a rare instance of a truly internal command that
    // doesn't use cmdHost since we do it all here.
    //
    // Aliases: NUMEXP, NIMGS, NIMAGES

    else if (chanWord.compare("NUMEXP",Qt::CaseInsensitive)==0 ||
	     chanWord.compare("NIMGS",Qt::CaseInsensitive)==0 ||
	     chanWord.compare("NIMAGES",Qt::CaseInsensitive)==0) {
      QStringList goList = chanCmd.split(" ");
      int numGo;
      // No arguments: this is a query
      if (goList.size() == 1) {
	if (isRed)
	  numGo = redCP->getNumExp();
	else
	  numGo = blueCP->getNumExp();
	*replyStr = QString("DONE: %1 %2 NIMGS=%3").arg(cmdWord).arg(chanWord).arg(numGo);
	if (!remoteCmd) addStatus(*replyStr,Qt::darkGreen,false);
      }
      // This is a set - lock out during acquisition!
      else {
	bool ok;
	numGo = goList.at(1).toInt(&ok,10);
	if (ok && (numGo > 0 && numGo <= MODS_MAX_NUMEXP)) {
	  if (isRed) {
	    if (redCP->acquiring()) {
	      *replyStr = QString("ERROR: %1 %2 cannot change during image acquisition"
				  ).arg(cmdWord).arg(chanWord);
	      if (!remoteCmd) addStatus(*replyStr,Qt::red,false);
	      cmdHost.remove(cmdKey);
	      return;
	    }
	    else 
	      redCP->setNumExp(numGo);
	  }
	  else {
	    if (blueCP->acquiring()) {
	      *replyStr = QString("ERROR: %1 %2 cannot change during image acquisition"
				  ).arg(cmdWord).arg(chanWord);
	      if (!remoteCmd) addStatus(*replyStr,Qt::red,false);
	      cmdHost.remove(cmdKey);
	      return;
	    }
	    else 
	      blueCP->setNumExp(numGo);
	  }
	  *replyStr = QString("DONE: %1 %2 NIMGS=%3").arg(cmdWord).arg(chanWord).arg(numGo);
	  if (!remoteCmd) addStatus(*replyStr,Qt::darkGreen,false);
	}
	else {
	  *replyStr = QString("ERROR: %1 %2 Invalid number of exposures %3, must be 1..%4"
			      ).arg(cmdWord).arg(chanWord).arg(numGo).arg(MODS_MAX_NUMEXP);
	  if (!remoteCmd) addStatus(*replyStr,Qt::red,false);
	}
      }
      cmdHost.remove(cmdKey);
      return;
    }

    // Enable/Disable/Query use of the IMCS on the channel

    else if (chanWord.compare("USEIMCS",Qt::CaseInsensitive)==0) {
      QStringList argList = chanCmd.split(" ");
      bool useIMCS;

      if (argList.size() == 1) { // just a query...
	if (isRed)
	  useIMCS = redCP->usingIMCS();
	else
	  useIMCS = blueCP->usingIMCS();
	if (useIMCS)
	  *replyStr = QString("DONE: %1 IMCS=ENABLED").arg(cmdWord);
	else
	  *replyStr = QString("DONE: %1 IMCS=DISABLED").arg(cmdWord);
	if (!remoteCmd) addStatus(*replyStr,Qt::darkGreen,false);
      }
      else {
	if (argList.size() == 2) {
	  if (argList.at(1).compare("Y",Qt::CaseInsensitive)==0 ||
	      argList.at(1).compare("YES",Qt::CaseInsensitive)==0)
	    useIMCS = true;
	  else if (argList.at(1).compare("N",Qt::CaseInsensitive)==0 ||
		   argList.at(1).compare("NO",Qt::CaseInsensitive)==0)
	    useIMCS = false;
	  else {
	    *replyStr = QString("ERROR: %1 unrecognized argument '%2'"
				" - Usage: %1 USEIMCS [Y|N]").arg(cmdWord).arg(argList.at(1));
	    if (!remoteCmd) addStatus(*replyStr,Qt::red,false);
	    cmdHost.remove(cmdKey);
	    return;
	  }
	  if (isRed)
	    redCP->setIMCS(useIMCS);
	  else
	    blueCP->setIMCS(useIMCS);
	  if (useIMCS)
	    *replyStr = QString("DONE: %1 IMCS=ENABLED").arg(cmdWord);
	  else
	    *replyStr = QString("DONE: %1 IMCS=DISABLED").arg(cmdWord);
	  if (!remoteCmd) addStatus(*replyStr,Qt::darkGreen,false);
	}
	else {
	  *replyStr = QString("ERROR: %1 invalid arguments, "
			      "Usage: %1 USEIMCS [Y|N]").arg(cmdWord);
	  if (!remoteCmd) addStatus(*replyStr,Qt::red,false);
	}
      }
      cmdHost.remove(cmdKey);
      return;
    }

    // It is some other channel command, send as-is to the host that handles
    // those commands.  The channel host will validate it...

    else
      sendToISIS(destHost,chanCmd);

  }

  // Implicit dual-channel commands.  These are a subset of the CCD
  // controller commands that can set parameters on both channels
  // without the Blue/Red qualifier

  else if (cmdWord.compare("exptime" ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("exp"     ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("object"  ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("flat"    ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("bias"    ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("zero"    ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("dark"    ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("comp"    ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("sky"     ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("std"     ,Qt::CaseInsensitive)==0 ||
 	   cmdWord.compare("nextfile",Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("lastfile",Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("observer",Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("partner" ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("propid"  ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("pi_name" ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("support" ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("telops"  ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("comment" ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("ccdbin"  ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("xbin"    ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("ybin"    ,Qt::CaseInsensitive)==0) {

    // Set the flag in the dualCmd hash for this command

    dualCmd.insert(cmdWord.toUpper(),true);

    cmdKey = QString("%1-%2").arg(modsBCHost[modsID]).arg(cmdWord.toUpper());
    cmdHost.insert(cmdKey,remoteHost);

    cmdKey = QString("%1-%2").arg(modsRCHost[modsID]).arg(cmdWord.toUpper());
    cmdHost.insert(cmdKey,remoteHost);

    // Some occasional special handling where command arguments might
    // exceed 40 characters which violates the maximum string size allowed
    // by the FITS standard.  If cmdArgs exceeds a length of 40 characters,
    // truncate it.

    if (cmdWord.compare("pi_name" ,Qt::CaseInsensitive)==0 ||
	cmdWord.compare("partner" ,Qt::CaseInsensitive)==0 ||
	cmdWord.compare("observer",Qt::CaseInsensitive)==0 ||
	cmdWord.compare("support" ,Qt::CaseInsensitive)==0 ||
	cmdWord.compare("telops"  ,Qt::CaseInsensitive)==0 ||
	cmdWord.compare("object"  ,Qt::CaseInsensitive)==0 ||
	cmdWord.compare("flat"    ,Qt::CaseInsensitive)==0 ||
	cmdWord.compare("bias"    ,Qt::CaseInsensitive)==0 ||
	cmdWord.compare("zero"    ,Qt::CaseInsensitive)==0 ||
	cmdWord.compare("dark"    ,Qt::CaseInsensitive)==0 ||
	cmdWord.compare("comp"    ,Qt::CaseInsensitive)==0 ||
	cmdWord.compare("sky"     ,Qt::CaseInsensitive)==0 ||
	cmdWord.compare("std"     ,Qt::CaseInsensitive)==0 ||
	cmdWord.compare("comment" ,Qt::CaseInsensitive)==0) {

      // Only use up to the left-most 39 characters in cmdArgs
      
      QString cmdTemp = QString("%1 %2").arg(cmdWord).arg(cmdArgs.left(39));
      sendToISIS(modsBCHost[modsID],cmdTemp);
      sendToISIS(modsRCHost[modsID],cmdTemp);
    }
    else { // send cmdStr as-is
      sendToISIS(modsBCHost[modsID],cmdStr);
      sendToISIS(modsRCHost[modsID],cmdStr);
    }

  }

  // Telescope commands.  We only enable a few of the lbttcs agent
  // aliases possible to avoid ambiguity
  //
  // Added bc (BinocularControl) command for the IIF [rwp/osu - 2015 Aug 30]

  else if (cmdWord.compare("iifinit"   ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("iifclose"  ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("tcstatus"  ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("telpos"    ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("gettime"   ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("authorized",Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("clearstars",Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("objname"   ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("objcoords" ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("guiname"   ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("guicoords" ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("rotator"   ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("refwave"   ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("remove"    ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("bc"        ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("preset"    ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("skyoffset" ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("offset"    ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("slitoffset",Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("offsetxy"  ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("rotoffset" ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("offsetpointing",Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("updatepointing",Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("movefocus" ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("stepfocus" ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("rotready"  ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("rotpos"    ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("logevent"  ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("setparameter",Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("getparameter",Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("pauseguiding",Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("gpause"    ,Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("resumeguiding",Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("gresume"   ,Qt::CaseInsensitive)==0) {

    QStringList wordList = cmdStr.split(" ");

    // Store the remote host associated with this command in the hash

    cmdKey = QString("%1-%2").arg(modsTCHost[modsID]).arg(cmdWord.toUpper());
    cmdHost.insert(cmdKey,remoteHost);

    sendToISIS(modsTCHost[modsID],cmdStr);

    // Also pass it along to the target panel to update as needed

    targPanel->cmdParse(cmdStr);

  }

  // Implicit dual-channel commands for the data managers - retired 2025 with Archons

  /*
  else if (cmdWord.compare("LASTFILE",Qt::CaseInsensitive)==0) {

    // Set the flag in the dualCmd hash for this command

    dualCmd.insert(cmdWord.toUpper(),true);

    // Send it to the blue data-manager host...

    cmdKey = QString("%1-%2").arg(modsBDHost[modsID]).arg(cmdWord.toUpper());
    cmdHost.insert(cmdKey,remoteHost);
    sendToISIS(modsBDHost[modsID],cmdStr);

    // ... and the red data-manager host

    cmdKey = QString("%1-%2").arg(modsRDHost[modsID]).arg(cmdWord.toUpper());
    cmdHost.insert(cmdKey,remoteHost);
    sendToISIS(modsRDHost[modsID],cmdStr);

  }
  */
  
  // Implicit dual FITSFLUSH - fire +SWAP at each data-manager host - retired 2025 with Archons

  
  /*
  else if (cmdWord.compare("FITSFLUSH",Qt::CaseInsensitive)==0) {
    sendToISIS(modsBDHost[modsID],"+SWAP");
    sendToISIS(modsRDHost[modsID],"+SWAP");
    *replyStr = "DONE: FITSFLUSH - pending FITS transfer request completed";
    if (!remoteCmd) addStatus(*replyStr,Qt::darkGreen,false);
    cmdHost.remove(cmdKey);
    return;
  }
  */
  
  // Implicit dual USEIMCS directive

  else if (cmdWord.compare("USEIMCS",Qt::CaseInsensitive)==0) {
    if (cmdArgs.isEmpty()) { // just a query...
      QString redIMCS, blueIMCS;
      if (blueCP->usingIMCS())
	blueIMCS = "BLUEIMCS=ENABLED";
      else
	blueIMCS = "BLUEIMCS=DISABLED";
      if (redCP->usingIMCS())
	redIMCS = "REDIMCS=ENABLED";
      else
	redIMCS = "REDIMCS=DISABLED";
      *replyStr = QString("DONE: %1 %2 %3").arg(cmdWord).arg(blueIMCS).arg(redIMCS);
      if (!remoteCmd) addStatus(*replyStr,Qt::darkGreen,false);
      cmdHost.remove(cmdKey);
      return;
    }

    if (cmdArgs.compare("Y",Qt::CaseInsensitive)==0 ||
	cmdArgs.compare("YES",Qt::CaseInsensitive)==0) {
      blueCP->setIMCS(true);
      redCP->setIMCS(true);
      *replyStr = QString("DONE: %1 BLUEIMCS=ENABLED REDIMCS=ENABLED").arg(cmdWord);
      if (!remoteCmd) addStatus(*replyStr,Qt::darkGreen,false);
    }
    else if (cmdArgs.compare("N",Qt::CaseInsensitive)==0 ||
	     cmdArgs.compare("NO",Qt::CaseInsensitive)==0) {
      blueCP->setIMCS(false);
      redCP->setIMCS(false);
      *replyStr = QString("DONE: %1 BLUEIMCS=DISABLED REDIMCS=DISABLED").arg(cmdWord);
      if (!remoteCmd) addStatus(*replyStr,Qt::darkGreen,false);
    }
    else {
      *replyStr = QString("ERROR: %1 unrecognized argument '%2'"
			  " - Usage: %1 [Y|N]").arg(cmdWord).arg(cmdArgs);
      if (!remoteCmd) addStatus(*replyStr,Qt::red,false);
    }
    cmdHost.remove(cmdKey);
    return;
  }

  // Implicit dual IMCSLOCK command

  else if (cmdWord.compare("IMCSLOCK",Qt::CaseInsensitive)==0) {
    if (redCP->acquiring() || blueCP->acquiring()) {
      *replyStr = QString("ERROR: IMCSLOCK disallowed during exposures");
      if (!remoteCmd) addStatus(*replyStr,Qt::red,false);
      cmdHost.remove(cmdKey);
      return;
    }

    // What lock-on wait time is requested?

    int waitTime;

    if (cmdArgs.isEmpty()) 
      waitTime = MODS_IMCS_LOCKTIME;
    else {
      bool isOK;
      waitTime = cmdArgs.toInt(&isOK,10);
      if (!isOK) {
	*replyStr = QString("ERROR: IMCSLOCK invalid argument '%1' - Usage: IMCSLOCK [wait]").arg(cmdArgs);
	if (!remoteCmd) addStatus(*replyStr,Qt::red,false);
	cmdHost.remove(cmdKey);
	return;
      }
    }      

    // Check the instrument mode and only launch on the active channel(s)

    if (instMode[modsID].compare("dual",Qt::CaseInsensitive)==0) { // dual mode
      isDIMCS = true;
      blueIMCSLock = false;
      blueIMCSDone = false;
      redIMCSLock = false;
      redIMCSDone = false;
      redCP->imcsLockOn(waitTime);
      blueCP->imcsLockOn(waitTime);
    }
    else if (instMode[modsID].compare("red",Qt::CaseInsensitive)==0) {
      isDIMCS = false;
      redIMCSLock = false;
      redIMCSDone = false;
      redCP->imcsLockOn(waitTime);
    }
    else if (instMode[modsID].compare("blue",Qt::CaseInsensitive)==0) {
      isDIMCS = false;
      blueIMCSLock = false;
      blueIMCSDone = false;
      blueCP->imcsLockOn(waitTime);
    }
    imcsLockHost = remoteHost;

  }

  // Implicit dual IMCSABORT/IMCSCANCEL command

  else if (cmdWord.compare("IMCSABORT",Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("IMCSCANCEL",Qt::CaseInsensitive)==0) {
    if (redCP->acquiring() || blueCP->acquiring()) {
      *replyStr = QString("ERROR: %1 disallowed during exposures").arg(cmdWord);
      if (!remoteCmd) addStatus(*replyStr,Qt::red,false);
      cmdHost.remove(cmdKey);
      return;
    }
    redCP->imcsCancelLock();
    blueCP->imcsCancelLock();
    *replyStr = QString("DONE: %1 sent").arg(cmdWord);
    if (!remoteCmd) addStatus(*replyStr,Qt::darkGreen,false);
    cmdHost.remove(cmdKey);
    return;
  }

  // INSTCONFIG - high-level instrument configuration directive

  else if (cmdWord.compare("INSTCONFIG",Qt::CaseInsensitive)==0) {
    cmdKey = QString("LOCAL-%1").arg(cmdWord.toUpper());
    cmdHost.insert(cmdKey,remoteHost);

    if (isInstConfig) {
      *replyStr = QString("ERROR: %1 instrument configuration already in progress - command disallowed").arg(cmdWord);
      if (!remoteCmd) addStatus(*replyStr,Qt::red,false);
      cmdHost.remove(cmdKey);
      return;
    }
    if (cmdArgs.isEmpty()) {
      *replyStr = QString("ERROR: %1 no arguments given"
			  " - Usage: INSTCONFIG [dual|blue|red] [config | [blueConfig redConfig]]").arg(cmdWord);
      if (!remoteCmd) addStatus(*replyStr,Qt::red,false);
      cmdHost.remove(cmdKey);
      return;
    }
    QStringList wordList = cmdArgs.split(" ");
    int numArgs = wordList.count();
    if (numArgs < 2 || numArgs > 3) {
      *replyStr = QString("ERROR: %1 wrong number of arguments given"
			  " - Usage: INSTCONFIG [dual|blue|red] [config | [blueConfig redConfig]]").arg(cmdWord);
      if (!remoteCmd) addStatus(*replyStr,Qt::red,false);
      cmdHost.remove(cmdKey);
      return;
    }
    
    // First argument is the configuration mode: dual, red, or blue

    bool isRed;
    bool isDual;
    QString modeID = wordList.at(0);

    if (modeID.compare("DUAL",Qt::CaseInsensitive)==0) {
      isDual = true;
      isRed = false;
    }
    else if (modeID.compare("RED",Qt::CaseInsensitive)==0) {
      isRed = true;
      isDual = false;
    }
    else if (modeID.compare("BLUE",Qt::CaseInsensitive)==0) {
      isRed = false;
      isDual = false;
    }
    else {
      *replyStr = QString("ERROR: %1 Invalid instrument dichroic selection '%2'"
			  " - Usage: INSTCONFIG [dual|blue|red] [config | [blueConfig redConfig]]"
			  ).arg(cmdWord).arg(modeID);
      if (!remoteCmd) addStatus(*replyStr,Qt::red,false);
      cmdHost.remove(cmdKey);
      remoteCmd = false;
      return;
    }

    // Validate the name against our internal table as well (we need a
    // numeric iPos for the dichroic command)

    int iPos = posByName(QString("dichroic %1").arg(modeID));
    if (iPos < 0) {
      *replyStr = QString("ERROR: %1 invalid instrument dichroic mode '%2'"
			  " - Usage: INSTCONFIG [dual|blue|red] [config | [blueConfig redConfig]]"
			  ).arg(cmdWord).arg(modeID);
      if (!remoteCmd) addStatus(*replyStr,Qt::red,false);
      cmdHost.remove(cmdKey);
      remoteCmd = false;
      return;
    }
    
    // Now extract the requested channel configuration(s) and test for validity
    
    QString blueConfigID;
    QString redConfigID;
    bool blueIsValid = false;
    bool redIsValid = false;

    if (wordList.count() == 2) { // same config for both channels
      if (isDual) {  // prepend D in dual mode
	blueConfigID = QString("D%1").arg(wordList.at(1));
	redConfigID = blueConfigID;
      }
      else {
	if (isRed)
	  redConfigID = wordList.at(1);
	else
	  blueConfigID = wordList.at(1);
      }
    }
    else {
      if (isDual) {  // prepend D in dual mode
	blueConfigID = QString("D%1").arg(wordList.at(1));
	redConfigID = QString("D%1").arg(wordList.at(2));
      }
      else {
	blueConfigID = wordList.at(1);
	redConfigID  = wordList.at(2);
      }
    }
    
    if (isDual) { // validate dual-channel configuration(s)
      blueIsValid = blueCP->validConfig(blueConfigID); 
      redIsValid = redCP->validConfig(redConfigID);
      if (!blueIsValid) {
	if (!redIsValid) {
	  *replyStr = QString("ERROR: %1 invalid channel configs blueCfg=%2 redCfg=%3"
			      " - Usage: INSTCONFIG [dual|blue|red] [config | [blueConfig redConfig]]"
			      ).arg(cmdWord).arg(blueConfigID).arg(redConfigID);
	  if (!remoteCmd) addStatus(*replyStr,Qt::red,false);
	  cmdHost.remove(cmdKey);
	  remoteCmd = false;
	  return;
	}
	else {
	  *replyStr = QString("ERROR: %1 invalid blue-channel configuration '%2'"
			      " - Usage: INSTCONFIG [dual|blue|red] [config | [blueConfig redConfig]]"
			      ).arg(cmdWord).arg(blueConfigID);
	  if (!remoteCmd) addStatus(*replyStr,Qt::red,false);
	  cmdHost.remove(cmdKey);
	  remoteCmd = false;
	  return;
	}
      }
    }
    else { // validate single-channel configuration requested
      if (isRed) {
	redIsValid = redCP->validConfig(redConfigID); 
	if (!redIsValid) {
	  *replyStr = QString("ERROR: %1 invalid red-channel configuration '%2'"
			      " - Usage: INSTCONFIG [dual|blue|red] [config | [blueConfig redConfig]]"
			      ).arg(cmdWord).arg(redConfigID);
	  if (!remoteCmd) addStatus(*replyStr,Qt::red,false);
	  cmdHost.remove(cmdKey);
	  remoteCmd = false;
	  return;
	}
      }
      else {
	blueIsValid = blueCP->validConfig(blueConfigID); 
	if (!blueIsValid) {
	  *replyStr = QString("ERROR: %1 invalid blue-channel configuration '%2'"
			      " - Usage: INSTCONFIG [dual|blue|red] [config | [blueConfig redConfig]]"
			      ).arg(cmdWord).arg(blueConfigID);
	  if (!remoteCmd) addStatus(*replyStr,Qt::red,false);
	  cmdHost.remove(cmdKey);
	  remoteCmd = false;
	  return;
	}
      }
    }

    // We have a mode, now execute it (or try)

    // Reset the state flags

    isInstConfig = true;
    isSingleConfig = false;
    numConfigPending = 0;
    numConfigErrors = 0;

    // Set the dichroic

    sendIECommand(QString("dichroic %1").arg(iPos));
    beamConfigEntry->setChange();
    numConfigPending++;

    // Set the channel(s) as required

    if (isDual) { // Start blue and red configuration
      numConfigPending += blueCP->setConfig(blueConfigID);
      numConfigPending += redCP->setConfig(redConfigID);
    }
    else {
      if (isRed) 
	numConfigPending += redCP->setConfig(redConfigID);
      else 
	numConfigPending += blueCP->setConfig(blueConfigID);
    }
    addStatus(QString("STATUS: %1 initiated").arg(cmdStr),Qt::blue,false);
  }

  // Set the preset mode on the telescope targeting control panel

  else if (cmdWord.compare("PRESETMODE",Qt::CaseInsensitive)==0) {
    if (cmdArgs.compare("acquire",Qt::CaseInsensitive)==0 ||
	cmdArgs.compare("track",Qt::CaseInsensitive)==0 ||
	cmdArgs.compare("guide",Qt::CaseInsensitive)==0 ||
	cmdArgs.compare("active",Qt::CaseInsensitive)==0) {
      targPanel->setPreset(cmdArgs);
      *replyStr = QString("DONE: PRESETMODE set presetMode=%1").arg(cmdArgs);
      if (!remoteCmd) addStatus(*replyStr,Qt::darkGreen,false);
    }
    else {
      *replyStr = QString("ERROR: PRESETMODE invalid preset mode '%1'").arg(cmdArgs);
      if (!remoteCmd) addStatus(*replyStr,Qt::red,false);
    }
  }

  // Implicit Dual NUMEXP/NIMAGES/NIMGS - lock out during acquisition

  else if (cmdWord.compare("NUMEXP",Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("NIMAGES",Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("NIMGS",Qt::CaseInsensitive)==0) {
    if (cmdArgs.isEmpty()) { // query
      *replyStr = QString("DONE: %1 NUMEXP_B=%2 NUMEXP_R=%3"
			  ).arg(cmdWord).arg(blueCP->getNumExp()).arg(redCP->getNumExp());
      if (!remoteCmd) addStatus(*replyStr,Qt::darkGreen,false);
    }
    else {
      bool ok;
      int numGo = cmdArgs.toInt(&ok,10);
      if (redCP->acquiring() || blueCP->acquiring()) {
	*replyStr = QString("ERROR: %1 Cannot change this parameter during image acquisition"
			    ).arg(cmdWord);
	if (!remoteCmd) addStatus(*replyStr,Qt::red,false);
      }
      else {
	if (ok && (numGo > 0 && numGo <= MODS_MAX_NUMEXP)) {
	  blueCP->setNumExp(numGo);
	  redCP->setNumExp(numGo);
	  *replyStr = QString("DONE: %1 NUMEXP_B=%2 NUMEXP_R=%3"
			      ).arg(cmdWord).arg(numGo).arg(numGo);
	  if (!remoteCmd) addStatus(*replyStr,Qt::darkGreen,false);
	}
	else {
	  *replyStr = QString("ERROR: %1 Invalid number of exposures %2, must be 1..%3"
			      ).arg(cmdWord).arg(numGo).arg(MODS_MAX_NUMEXP);
	  if (!remoteCmd) addStatus(*replyStr,Qt::red,false);
	}
      }
    }
  }

  // Experimental: implicit dual pause/resume/stop 

  else if (cmdWord.compare("PAUSE",Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("RESUME",Qt::CaseInsensitive)==0) {
    blueCP->pauseExposure();
    redCP->pauseExposure();
  }

  else if (cmdWord.compare("STOP",Qt::CaseInsensitive)==0) {
    blueCP->execStop();
    redCP->execStop();
  }

  // DGO - Dual-Channel GO.  Fires an execGo at each in whatever
  // configuration is setup (numExp, etc.), and sets the status flags

  else if (cmdWord.compare("DGO",Qt::CaseInsensitive)==0) {
    bool redOK;
    bool blueOK;

    // 2011-05-27: Some sick voodoo - if we reverse the order of
    // launch to be red then blue, and in modschannel.cpp enqueue the
    // GO command so that they are dispatched at the usual command
    // cadence, it seems to break up the race condition with ISTATUS
    // that results in timeout error on current IC (v5.5)

    redOK = redCP->execGo();
    if (!redOK)
      addStatus("ERROR: DGO cannot start exposures on the red channel"
		" - exposure already in progress",Qt::red,false);
    else {
      cmdKey = QString("%1-GO").arg(modsRCHost[modsID]);
      cmdHost.insert(cmdKey,remoteHost);
      addStatus("Started red channel exposure sequence",Qt::blue,false);
      redGoDone = false;
    }

    blueOK = blueCP->execGo();
    if (!blueOK)
      addStatus("ERROR: DGO cannot start exposures on the blue channel"
		" - exposure already in progress",Qt::red,false);
    else {
      cmdKey = QString("%1-GO").arg(modsBCHost[modsID]);
      cmdHost.insert(cmdKey,remoteHost);
      addStatus("Started blue channel exposure sequence",Qt::blue,false);
      blueGoDone = false;
    }
    
    // Now process the outcomes...
    
    if (blueOK) {
      if (redOK)
	*replyStr = QString("STATUS: DGO exposures started on blue and red channels");
      else
	*replyStr = QString("STATUS: DGO exposure started on blue channel,"
			    " but not on red, see error messages for the red-channel status");
    }
    else {
      if (redOK) 
	*replyStr = QString("STATUS: DGO exposure started on red channel,"
			    " but not on blue, see error messages for the blue-channel status");
      else {
	*replyStr = QString("ERROR: DGO cannot start exposures"
			    " - both channels already exposing");
	if (!remoteCmd)
	  addStatus(*replyStr,Qt::red,false);
	remoteCmd = false;
	return;
      }
    }
    isDGO = true;  // set the DGO in-progress state flag
    if (!remoteCmd) addStatus(*replyStr,Qt::blue,false);
  }

  // GO - implicit GO without channel qualifier.  Looks at the
  //      instMode[] flag to determine what action to take (DGO, 
  //      RED GO, or BLUE GO).

  else if (cmdWord.compare("GO",Qt::CaseInsensitive)==0) {
    bool useBlue;
    bool useRed;

    useRed = false;
    useBlue = false;

    // Check the instrument mode and only launch on the active channel(s)

    if (instMode[modsID].compare("dual",Qt::CaseInsensitive)==0) { // dual mode
      if (blueCP->acquiring() || redCP->acquiring()) {
	*replyStr = QString("ERROR: GO cannot start exposures - one or both channels already exposing");
	if (!remoteCmd)
	  addStatus(*replyStr,Qt::red,false);
	remoteCmd = false;
	return;
      }
      useRed = true;
      useBlue = true;
    }
    else if (instMode[modsID].compare("red",Qt::CaseInsensitive)==0) {
      if (redCP->acquiring()) {
	*replyStr = QString("ERROR: GO cannot start a red exposure - red channel already exposing");
	if (!remoteCmd)
	  addStatus(*replyStr,Qt::red,false);
	remoteCmd = false;
	return;
      }
      useRed = true;
      useBlue = false;
    }
    else if (instMode[modsID].compare("blue",Qt::CaseInsensitive)==0) {
      if (blueCP->acquiring()) {
	*replyStr = QString("ERROR: GO cannot start a blue exposure - blue channel already exposing");
	if (!remoteCmd)
	  addStatus(*replyStr,Qt::red,false);
	remoteCmd = false;
	return;
      }
      useRed = false;
      useBlue = true;
    }

    // Launch the GO command(s)

    if (useBlue) {
      blueCP->execGo();
      cmdKey = QString("%1-GO").arg(modsBCHost[modsID]);
      cmdHost.insert(cmdKey,remoteHost);
      addStatus("Started blue channel exposure sequence",Qt::blue,false);
      blueGoDone = false;
    }

    if (useRed) {
      redCP->execGo();
      cmdKey = QString("%1-GO").arg(modsRCHost[modsID]);
      cmdHost.insert(cmdKey,remoteHost);
      addStatus("Started red channel exposure sequence",Qt::blue,false);
      redGoDone = false;
    }

    // Set the exposure handling flags

    if (useBlue && useRed) {
      isDGO = true;  
      redGoDone = false;
      blueGoDone = false;
      *replyStr = QString("STATUS: GO exposures started on the blue and red channels");
    }
    else {
      isDGO = false;
      if (useBlue) {
	blueGoDone = false;
	redGoDone = true;
	*replyStr = QString("STATUS: GO exposures started on the blue channel");
      }
      else {
	redGoDone = false;
	blueGoDone = true;
	*replyStr = QString("STATUS: GO exposures started on the red channel");
      }
    }
      
    if (!remoteCmd) addStatus(*replyStr,Qt::blue,false);

  }

  //---------------------------------------------------------------------------
  // 
  // Commands for the dashboard proper 
  //

  // UPDATE - Update the Control Panel (alias: REFRESH)
  //
  // Arguments:
  //   ALL = update the entire control panel
  //    FP = update focal-plane suite
  //   RED = update red channel 
  //  BLUE = update blue channel
  //  INSTCONFIG = clear pending InstConfig flag
  //
  // If given with no arguments, return an error with a syntax usage
  // message.  We require an explicit statement of what is to be
  // refreshed/reset.
  //

  else if (cmdWord.compare("UPDATE",Qt::CaseInsensitive)==0 ||
	   cmdWord.compare("REFRESH",Qt::CaseInsensitive)==0) {
    if (cmdArgs.compare("FP",Qt::CaseInsensitive)==0) {
      fpControlPanel->update();
      *replyStr = QString("DONE: %1 MODS%2 Dashboard Focal Plane"
			  " Display Updated").arg(cmdWord).arg(modsNum);
    }
    else if (cmdArgs.compare("Blue",Qt::CaseInsensitive)==0) {
      blueCP->update();
      *replyStr = QString("DONE: %1 MODS%2 Dashboard Blue Channel"
			  " Display Updated").arg(cmdWord).arg(modsNum);
    }
    else if (cmdArgs.compare("Red",Qt::CaseInsensitive)==0) {
      redCP->update();
      *replyStr = QString("DONE: %1 MODS%2 Dashboard Red Channel"
			  " Display Updated").arg(cmdWord).arg(modsNum);
    }
    else if (cmdArgs.compare("ALL",Qt::CaseInsensitive)==0) {
      update();
      *replyStr = QString("DONE: %1 MODS%2 Control Panel Updated").arg(cmdWord).arg(modsNum);
    }
    else if (cmdArgs.compare("INSTCONFIG",Qt::CaseInsensitive)==0) {
      isInstConfig = false;
      *replyStr = QString("DONE: %1 MODS%2 InstConfig flag cleared").arg(cmdWord).arg(modsNum);
    }
    else {
      *replyStr = QString("ERROR: %1 Unknown argument %2 - Usage: %1 [all|fp|blue|red|instconfig]").arg(cmdWord).arg(cmdArgs);
      addStatus(*replyStr,Qt::red,false);
    }
    if (!remoteCmd) addStatus(*replyStr,Qt::darkGreen,false);
  }

  // CANCEL - Cancels any pending multi-channel requests and clears
  //          their status bits.  A way to cleanup after certain types
  //          of failed/stuck requests.
  // Arguments:
  //    IMCSLOCK - cancels a pending imcslock
  //    INSTCONFIG - cancels a pending instconfig

  else if (cmdWord.compare("CANCEL",Qt::CaseInsensitive)==0) {
    if (cmdArgs.compare("INSTCONFIG",Qt::CaseInsensitive)==0) {
      isInstConfig = false;
      *replyStr = QString("DONE: %1 MODS%2 INSTCONFIG Canceled").arg(cmdWord).arg(modsNum);
    }
    else if (cmdArgs.compare("IMCS",Qt::CaseInsensitive)==0 ||
	     cmdArgs.compare("IMCSLOCK",Qt::CaseInsensitive)==0) {
      redCP->imcsCancelLock();
      blueCP->imcsCancelLock();
      *replyStr = QString("DONE: %1 MODS%2 IMCSLOCK Canceled").arg(cmdWord).arg(modsNum);
    }
    if (!remoteCmd) addStatus(*replyStr,Qt::darkGreen,false);
  }

  // RELOAD - Reload all of the device and configuration tables for this
  //          dashboard instance

  else if (cmdWord.compare("RELOAD",Qt::CaseInsensitive)==0) {
    reloadTables();
    *replyStr = QString("DONE: RELOAD MODS%1 device and configuration tables reloaded").arg(modsNum);
    if (!remoteCmd) addStatus(*replyStr,Qt::darkGreen,false);

  }

  // BELL - Enable/Disable the End-of-Exposure Bell

  else if (cmdWord.compare("BELL",Qt::CaseInsensitive)==0) {
    if (cmdArgs.compare("ON",Qt::CaseInsensitive)==0) {
      useExpBell = true;
      *replyStr = QString("DONE: BELL End-of-Exposure Bell ENABLED");
    }
    else if (cmdArgs.compare("OFF",Qt::CaseInsensitive)==0) {
      useExpBell = false;
      *replyStr = QString("DONE: BELL End-of-Exposure Bell DISABLED");
    }
    else {
      if (useExpBell)
	*replyStr = QString("DONE: BELL End-of-Exposure Bell is ENABLED");
      else
	*replyStr = QString("DONE: BELL End-of-Exposure Bell is DISSBLED");
    }
    if (!remoteCmd) addStatus(*replyStr,Qt::darkGreen,false);
  }

  // BEEP - ring the bell...

  else if (cmdWord.compare("BEEP",Qt::CaseInsensitive)==0) {
    qApp->beep();
    *replyStr = QString("DONE: BEEP rang the console bell");
    if (!remoteCmd) addStatus(*replyStr,Qt::darkGreen,false);
  }

  // OBSINIT - clear various flags at the start of a new observation. This
  //           is designed primarily for use with scripts, and serves to
  //           undo anything non-standard the previous observation might have
  //           set.

  else if (cmdWord.compare("OBSINIT",Qt::CaseInsensitive)==0) {

    // Disallow during active image acquisition

    if (redCP->acquiring() || blueCP->acquiring()) {
      *replyStr = QString("ERROR: OBSINIT disallowed during active image acquisition");
      if (!remoteCmd) addStatus(*replyStr,Qt::red,false);
      cmdHost.remove(cmdKey);
      return;
    }
    
    QString newObjName;
    if (cmdArgs.isEmpty())
      newObjName = "None";
    else
      newObjName = cmdArgs;

    // Reset the exposure control states of the two channels

    redCP->resetExp();
    blueCP->resetExp();

    // Set the image type to "OBJECT", the startup default, and
    // 1-second exposure time.

    redCP->setImgType("OBJECT");
    redCP->setObject(newObjName);
    redCP->setExpTime(1.0);
    blueCP->setImgType("OBJECT");
    blueCP->setObject(newObjName);
    blueCP->setExpTime(1.0);

    // Reset binning to 1x1, the startup default, but leave ROI alone.

    redCP->setCCDBinning(1,1);
    blueCP->setCCDBinning(1,1);

    // Enable IMCS - just in case someone unset it in engineering work

    redCP->setIMCS(true);
    blueCP->setIMCS(true);

    // Reset the maximum calibration lamp time to the default set at runtime

    if (maxLampTime < 0)
      fpControlPanel->setMaxLampTime(MODS_MAX_LAMPTIME);
    else
      fpControlPanel->setMaxLampTime(maxLampTime);

    // Reset the telescope preset and offset control panels

    targPanel->reset();

    // Make sure calibration lamps are off

    fpControlPanel->setLamp("OFF");
    
    // Clear delinquent entires from the command queue and clear any
    // unresolved instconfig requests.

    isInstConfig = false;

    // All done

    *replyStr = QString("DONE: OBSINIT completed");
    if (!remoteCmd) addStatus(*replyStr,Qt::darkGreen,false);

  }

  // Query or Change the calibration lamp maximum on time

  else if (cmdWord.compare("LAMPTIME",Qt::CaseInsensitive)==0) {
    if (cmdArgs.isEmpty()) {
      int lampTime = fpControlPanel->maxLampTime();
      *replyStr = QString("DONE: LAMPTIME MaxLampTime=%1 sec").arg(lampTime);
    }
    else {
      bool isOK;
      int lampTime = cmdArgs.toInt(&isOK,10);
      if (isOK) {
	if (lampTime > 0) {
	  fpControlPanel->setMaxLampTime(lampTime);
	  *replyStr = QString("DONE: LAMPTIME MaxLampTime=%1 sec").arg(lampTime);
	}
	else {
	  fpControlPanel->setMaxLampTime(0);
	  *replyStr = QString("DONE: LAMPTIME MaxLampTime=0 lamp timing disabled");
	}
      }
    }
    if (!remoteCmd) addStatus(*replyStr,Qt::darkGreen,false);
  }

  // DEBUG - various debugging options

  else if (cmdWord.compare("DEBUG",Qt::CaseInsensitive)==0) {
    if (cmdArgs.compare("CMDCLEAR",Qt::CaseInsensitive)==0) {
      cmdHost.clear();
      *replyStr = QString("DONE: DEBUG cmdHost hash table cleared");
    }
    else if (cmdArgs.compare("CMDHOST",Qt::CaseInsensitive)==0) {
      int numHosts = cmdHost.size();
      int numDual = dualCmd.size();
      if (numHosts == 0) {
	if (numDual == 0) 
	  *replyStr = QString("DONE: DEBUG cmdHost and dualCmd have no active items");
	else
	  *replyStr = QString("DONE: DEBUG numHosts=0 numDual=%1").arg(numDual);
      }
      else {
	*replyStr = QString("DONE: DEBUG numHosts=%1 numDual=%2 active item(s)"
			    ).arg(numHosts).arg(numDual);
	QHashIterator<QString,QString> i(cmdHost);
	while (i.hasNext()) {
	  i.next();
	  qDebug() << i.key() << ":" << i.value();
	}	
      }
    }
    if (!remoteCmd) addStatus(*replyStr,Qt::darkGreen,false);
  }

  // Unknown command word, complain and reset the remoteCmd.
  
  else {
    *replyStr = QString("ERROR: Unknown/Unimplemented command %1").arg(cmdWord);
    if (!remoteCmd)
      addStatus(*replyStr,Qt::red,false);
    remoteCmd = false;
  }
  
}

// sendIECommand() - Send a command to the IE Host

void DashBoard::sendIECommand(const QString &cmd)
{
  if (cmd.isEmpty()) return;

  QString cmdStr = cmd;

  // We have something to send, do it.  Only ABORT and PANIC are
  // always dispatched immediately, overriding any dispatch queuing

  if (cmdStr.contains("ABORT",Qt::CaseInsensitive))
    sendAndWait(modsIEHost[modsID],cmdStr,MODS_IMMEDIATE_REQUEST);
  else if (cmdStr.contains("PANIC",Qt::CaseInsensitive))
    sendAndWait(modsIEHost[modsID],cmdStr,MODS_IMMEDIATE_REQUEST);
  else
    sendToISIS(modsIEHost[modsID],cmdStr);

  // A little extra handling for commands that are linked to multiple
  // widgets

  // Slitmask changes also affect the insert/retract buttons

  if (cmdStr.contains("slitmask",Qt::CaseInsensitive))
    minsertControl->setChange();

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

void DashBoard::parseISIS()
{
  bool execCmd = false;   // Flag: message is a command execution request
  bool cmdDone = false;   // Flag: message is a command completion
  bool cmdFault = false;  // Flag: command completed with a fault
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

  switch (isis->type()) {
  case ISISClient::ERROR:
    addStatus(QString("%1> ERROR: %2").arg(remoteHost).arg(msgStr),Qt::red,true);
    typeStr = "ERROR:";
    cmdDone = true;
    cmdFault = true;
    break;

  case ISISClient::WARNING:
    addStatus(QString("%1> WARNING: %2").arg(remoteHost).arg(msgStr),QColor(255,165,0,255),true);
    typeStr = "WARNING:";
    cmdDone = false;
    cmdFault = false;
    break;

  case ISISClient::FATAL:
    addStatus(QString("%1> FATAL: %2").arg(remoteHost).arg(msgStr),Qt::magenta,true);
    typeStr = "FATAL:";
    cmdDone = true;
    cmdFault = true;
    break;

  case ISISClient::DONE:
    addStatus(QString("%1> %2").arg(remoteHost).arg(msgStr),Qt::darkGreen,true);
    typeStr = "DONE:";
    cmdDone = true;
    cmdFault = false;
    break;

  case ISISClient::STATUS:
    addStatus(QString("%1> %2").arg(remoteHost).arg(msgStr),Qt::blue,true);
    typeStr = "STATUS:";
    cmdDone = false;
    cmdFault = false;
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
  // command fault message.

  else {
    // The first word in the message body is the command that
    // generated this response.

    QString cmdWord = isis->body().section(" ",0,0);

    // Create a hash with the keyword=value pairs

    QHash<QString,QString> keyDict; // keyword dictionary
    int numKeys = isis->getKeys(msgStr,&keyDict);

    // Pass it along for processing by the various control panels that
    // make up this widget

    if (remoteHost.compare(modsIEHost[modsID],Qt::CaseInsensitive)==0) {
      fpControlPanel->parse(isis->body(),isis->type(),keyDict);
      blueCP->parse(isis->body(),isis->type(),keyDict);
      redCP->parse(isis->body(),isis->type(),keyDict);
    }
    else if (remoteHost.compare(modsBCHost[modsID],Qt::CaseInsensitive)==0 ||
	     remoteHost.compare(modsBDHost[modsID],Qt::CaseInsensitive)==0) {
      blueCP->parse(isis->body(),isis->type(),keyDict);
    }
    else if (remoteHost.compare(modsRCHost[modsID],Qt::CaseInsensitive)==0 ||
	     remoteHost.compare(modsRDHost[modsID],Qt::CaseInsensitive)==0) {
      redCP->parse(isis->body(),isis->type(),keyDict);
    }
    else if (remoteHost.compare(modsTCHost[modsID],Qt::CaseInsensitive)==0) {
      targPanel->parse(isis->body(),isis->type(),keyDict);
    }

    // Share the message with the parent

    emit shareMsg(isis->text());

    // Special handling is required if this is an exposure command as
    // we need to check the image acquisition state and/or instrument
    // configuration state so as not to prematurely indicate a command
    // is "done" when executing a multi-image sequence.

    if (cmdWord.compare("GO",  Qt::CaseInsensitive)==0 ||
	cmdWord.compare("SNAP",Qt::CaseInsensitive)==0 ||
	cmdWord.compare("MGO", Qt::CaseInsensitive)==0) {
      if (isDGO && cmdDone) { // dual-channel acquisition, even more twisted logic...
	if (remoteHost.compare(modsBCHost[modsID],Qt::CaseInsensitive)==0) { // Blue CCD controller host
	  if (blueCP->acquiring()) {
	    typeStr = "STATUS:";
	    msgStr = QString("%1 %2").arg(cmdWord).arg(blueCP->expStatus());
	    cmdDone = false;
	  }
	  else {
	    blueGoDone = true;
	    if (redCP->acquiring()) {
	      typeStr = "STATUS:";
	      msgStr = QString("%1 Blue Channel Exposures Done - Red Channel still acquiring...").arg(cmdWord);
	      cmdDone = false;
	    }
	    else {
	      redGoDone = true;
	      isDGO = false;
	    }
	  }
	}
	else if (remoteHost.compare(modsRCHost[modsID],Qt::CaseInsensitive)==0) { // Red CCD controller host
	  if (redCP->acquiring()) {
	    typeStr = "STATUS:";
	    msgStr = QString("%1 %2").arg(cmdWord).arg(redCP->expStatus());
	    cmdDone = false;
	  }
	  else {
	    redGoDone = true;
	    if (blueCP->acquiring()) {
	      typeStr = "STATUS:";
	      msgStr = QString("%1 Red Channel Exposures Done - Blue Channel still acquiring...").arg(cmdWord);
	      cmdDone = false;
	    }
	    else {
	      blueGoDone = true;
	      isDGO = false;
	    }
	  }
	}
      }
      else { // single-channel GO
	if (remoteHost.compare(modsBCHost[modsID],Qt::CaseInsensitive)==0) { // Blue CCD controller host
	  if (blueCP->acquiring() && cmdDone) {
	    typeStr = "STATUS:";
	    msgStr = QString("%1 %2").arg(cmdWord).arg(blueCP->expStatus());
	    cmdDone = false;
	  }
	}
	else if (remoteHost.compare(modsRCHost[modsID],Qt::CaseInsensitive)==0) { // Red CCD controller host
	  if (redCP->acquiring() && cmdDone) {
	    typeStr = "STATUS:";
	    msgStr = QString("%1 %2").arg(cmdWord).arg(redCP->expStatus());
	    cmdDone = false;
	  }
	}
      }
    }

    // Instrument configuration command in progress?

    if (isInstConfig) {
      bool cfgCmd;
      cfgCmd = false;

      // Blue-channel mechanism config commands

      if (cmdWord.compare("BFILTER",Qt::CaseInsensitive)==0 ||
	  cmdWord.compare("BGRATING",Qt::CaseInsensitive)==0 ||
	  cmdWord.compare("BCOLFOC",Qt::CaseInsensitive)==0 ||
	  cmdWord.compare("BCAMFOC",Qt::CaseInsensitive)==0) {
	cfgCmd = true;
	numConfigPending--;
	if (cmdFault) numConfigErrors++;
      }

      // Red-channel mechanism config commands

      if (cmdWord.compare("RFILTER",Qt::CaseInsensitive)==0 ||
	  cmdWord.compare("RGRATING",Qt::CaseInsensitive)==0 ||
	  cmdWord.compare("RCOLFOC",Qt::CaseInsensitive)==0 ||
	  cmdWord.compare("RCAMFOC",Qt::CaseInsensitive)==0) {
	cfgCmd = true;
	numConfigPending--;
	if (cmdFault) numConfigErrors++;
      }

      // Blue Channel CCD Controller commands

      if (remoteHost.compare(modsBCHost[modsID],Qt::CaseInsensitive)==0) { // Blue CCD controller host
	if (cmdWord.compare("ROI",Qt::CaseInsensitive)==0 ||
	    cmdWord.compare("CCDBIN",Qt::CaseInsensitive)==0 ||
	    cmdWord.compare("OBJECT",Qt::CaseInsensitive)==0) {
	  cfgCmd = true;
	  numConfigPending--;
	  if (cmdFault) numConfigErrors++;
	}
      }

      // Red Channel CCD Controller commands

      if (remoteHost.compare(modsRCHost[modsID],Qt::CaseInsensitive)==0) { // Red CCD controller host
	if (cmdWord.compare("ROI",Qt::CaseInsensitive)==0 ||
	    cmdWord.compare("CCDBIN",Qt::CaseInsensitive)==0 ||
	    cmdWord.compare("OBJECT",Qt::CaseInsensitive)==0) {
	  cfgCmd = true;
	  numConfigPending--;
	  if (cmdFault) numConfigErrors++;
	}
      }

      // The dichroic is set as part of the instConfig command

      if (cmdWord.compare("DICHROIC",Qt::CaseInsensitive)==0) {
	cfgCmd = true;
	numConfigPending--;
	if (cmdFault) numConfigErrors++;
      }

      // This *is* a pending configuration message, process it

      if (cfgCmd) {
	// Remove the configuration command key
	QString cfgKey = QString("%1-%2").arg(remoteHost).arg(cmdWord.toUpper());
	cmdHost.remove(cfgKey);

	// Setup the info needed to route the reply back to whoever sent INSTCONFIG

	remoteHost = "LOCAL";
	if (isSingleConfig)
	  cmdWord = "CONFIG";
	else
	  cmdWord = "INSTCONFIG";

	// See if we still have pending configuration commands...

	if (numConfigPending > 0) { // we still have pending commands, send a status reply if faults
	                            // otherwise throttle to reduce noise and queue delay
	  typeStr = "STATUS:";
	  if (cmdFault)
	    msgStr = QString("WARNING: ***FAULT*** - %1").arg(isis->body());
	  else
	    msgStr.clear();
	  // msgStr = QString("%1 %2").arg(cmdWord).arg(isis->body());

	  cmdDone = false;
	  cmdFault = false;
	}
	else { // Config complete, finish transaction and reset state flags
	  if (numConfigErrors > 0) {
	    cmdFault = true;
	    msgStr = QString("%1 completed with %2 faults").arg(cmdWord).arg(numConfigErrors);
	  }
	  else
	    msgStr = QString("%1 completed").arg(cmdWord);
	  cmdDone = true;
	  isInstConfig = false;
	  numConfigPending = 0;
	  numConfigErrors = 0;
	}
      }

    } // End of INSTCONFIG parsing

    // Was this an implicit dual command response?  Check the dualCmd hash
    // If we have an entry and we received a DONE:, change it to STATUS:
    // and remove the key so that the next pass sends DONE:

    if (dualCmd.contains(cmdWord.toUpper())) {
      if (cmdDone) {
	typeStr = "STATUS:";
	dualCmd.remove(cmdWord.toUpper());
      }
    }

    // Now, build the command host key and see if this originated with
    // a previous remote host command handled by us.  If the command
    // transaction is complete, remove the item from the cmdHost hash.

    QString cmdKey = QString("%1-%2").arg(remoteHost).arg(cmdWord.toUpper());

    if (cmdHost.contains(cmdKey)) {
      QString replyHost = cmdHost.value(cmdKey);

      // First Case - the command originated with a remote ISIS client
      // instance.  Pass the message to the originating client via the
      // ISIS server.  Insert the BLUE or RED channel qualifer as
      // needed.  ***CHECK*** msgStr <-> isis->body(), and don't send 
      // anything if msgStr is empty.

      if (replyHost.compare("local",Qt::CaseInsensitive)!=0 && !msgStr.isEmpty()) {
	if (remoteHost.compare(modsBCHost[modsID],Qt::CaseInsensitive)==0) {
	  sendToISIS(replyHost,QString("%1 BLUE %2").arg(typeStr).arg(msgStr));
	}
	else if (remoteHost.compare(modsRCHost[modsID],Qt::CaseInsensitive)==0) {
	  sendToISIS(replyHost,QString("%1 RED %2").arg(typeStr).arg(msgStr));
	}
	else 
	  sendToISIS(replyHost,QString("%1 %2").arg(typeStr).arg(msgStr));
      }

      // Second Case - the command originated with a local command
      // sequencer instance.  Emit the signal to pass the message back
      // to the command sequencer for handling.

      else if (replyHost.compare("local-csq",Qt::CaseInsensitive)==0) {
	if (remoteHost.compare(modsBCHost[modsID],Qt::CaseInsensitive)==0) 
	  emit csqMessage(QString("%1 BLUE %2").arg(typeStr).arg(isis->body()));
	else if (remoteHost.compare(modsRCHost[modsID],Qt::CaseInsensitive)==0) 
	  emit csqMessage(QString("%1 RED %2").arg(typeStr).arg(isis->body()));
	else 
	  emit csqMessage(QString("%1 %2").arg(typeStr).arg(isis->body()));
      }

      if (cmdDone) cmdHost.remove(cmdKey);
    }

    // Anything else we might mine from this?

    if (numKeys > 0) { // use Qt's built-in Java-style iterator
      QHashIterator<QString,QString> i(keyDict);
      while (i.hasNext()) {
        i.next();
	QString keyStr = i.key();
	QString valStr = i.value();
	bool ok;

	if (keyStr.compare("DICHROIC",Qt::CaseInsensitive)==0) {
	  int iPos = valStr.toInt(&ok,10);
	  int ind;
	  if (ok) {
	    switch (iPos) {
	    case 1:
	      ind = beamConfigEntry->findData("RED");
	      blueConfigEntry->disable();
	      redConfigEntry->enable();
	      instMode[modsID] = "RED";
	      break;

	    case 2:
	      ind = beamConfigEntry->findData("DUAL");
	      blueConfigEntry->enable();
	      redConfigEntry->enable();
	      instMode[modsID] = "DUAL";
	      break;
	      
	    case 3:
	      ind = beamConfigEntry->findData("BLUE");
	      blueConfigEntry->enable();
	      redConfigEntry->disable();
	      instMode[modsID] = "BLUE";
	      break;
	    }
	    if (ind>=0) {
	      beamConfigEntry->setCurrentIndex(ind);
	      curBeamConfig = ind;
	      if (cmdFault)
		beamConfigEntry->setFault();
	      else
		beamConfigEntry->setNormal();
	    }
	  }
	}
	else if (keyStr.compare("BGRATING",Qt::CaseInsensitive)==0) {
	  int iPos = valStr.toInt(&ok,10);
	  int ind;
	  if (ok) {
	    switch (iPos) {
	    case 1:
	      ind = blueConfigEntry->findText("Imaging");
	      break;

	    case 2:
	      ind = blueConfigEntry->findText("Grating");
	      break;
	      
	    case 3:
	      ind = blueConfigEntry->findText("Prism");
	      break;
	    }
	    if (ind>=0) {
	      blueConfigEntry->setCurrentIndex(ind);
	      curBlueConfig = ind;
	    }
	  }
	}
	else if (keyStr.compare("RGRATING",Qt::CaseInsensitive)==0) {
	  int iPos = valStr.toInt(&ok,10);
	  int ind;
	  if (ok) {
	    switch (iPos) {
	    case 1:
	      ind = redConfigEntry->findText("Imaging");	
	      break;

	    case 2:
	      ind = redConfigEntry->findText("Grating");
	      break;
	      
	    case 3:
	      ind = redConfigEntry->findText("Prism");
	      break;
	    }
	    if (ind>=0) {
	      redConfigEntry->setCurrentIndex(ind);
	      curRedConfig = ind;
	    }
	  }
	}

	// Slit Mask Select - treat SLITMASK=BRACE as "unknown" for now

	else if (keyStr.compare("SLITMASK",Qt::CaseInsensitive)==0 ||
		 keyStr.compare("MSELECT",Qt::CaseInsensitive)==0) {
	  slitControl->setISISState(isis->type());
	  int iPos = valStr.toInt(&ok,10);
	  if (ok)
	    slitmask->setPos(iPos);
	  else {
	    if (valStr.compare("BRACE",Qt::CaseInsensitive)==0) {
	      slitControl->setUnknown();
	      minsertControl->setUnknown();
	    }
	    else if (valStr.compare("BUSY",Qt::CaseInsensitive)!=0) {
	      slitmask->setErrorStr(msgStr);
	      slitControl->setFault();
	    }
	  }
	}

	// Mask insert/retract - MASKPOS=EMPTY means the mask grabber
	// is in the stow (aka OUT) position but there is no mask there

	else if (keyStr.compare("MASKPOS",Qt::CaseInsensitive)==0) {
	  minsertControl->setISISState(isis->type());
	  if (valStr.compare("IN",Qt::CaseInsensitive)==0)
	    minsert->setState(0);
	  else if (valStr.compare("STOW",Qt::CaseInsensitive)==0)
	    minsert->setState(1);
	  else if (valStr.compare("EMPTY",Qt::CaseInsensitive)==0)
	    minsertControl->setUnknown();
	  else if (valStr.compare("BUSY",Qt::CaseInsensitive)!=0) {
	    minsert->setErrorStr(msgStr);
	    minsertControl->setFault();
	  }
	}

	// MINSERT=xxx contains fault and other info

	else if (keyStr.compare("MINSERT",Qt::CaseInsensitive)==0) {
	  minsertControl->setISISState(isis->type());
	  if (valStr.compare("PWRFLR",Qt::CaseInsensitive)==0) {
	    minsert->setErrorStr(msgStr);
	    minsertControl->setFault();
	  }
	}

	// Anything else?
	
      }
    }
    
    // Some commands require additional handling, especially for faults
    
  }

}

// Some special methods

// homeAGw() - Home the AGw stage

void DashBoard::homeAGw()
{
  QString cmdStr = "AGW HOME";
  sendIECommand(cmdStr);
}

// launchAGwDialog() - Launch the AGw control panel dialog 

void DashBoard::launchAGwDialog()
{
  addStatus("AGW Control Panel Dialog not yet implemented",Qt::red,false);
}

// update() - Update he dashboard display
//
// Performs the following actions
//   * Queries instrument mechanism and detector system status
//   * Updates the focal-plane and channel subpanels
//   * Checks certain critical mechanism groups
//   * Resets state flags
// Should return the control panel to a known, idle state ready
// to go, or as a fresh start for fault recovery.
//

void DashBoard::update()
{
  // Request status from the relevant remote hosts

  //isis->queue(modsIEHost[modsID],"istatus");  // done below individually
  isis->send(modsBCHost[modsID],"status");
  isis->send(modsRCHost[modsID],"status");

  // Update the individual instrument subpanels

  fpControlPanel->update();
  blueCP->update();
  redCP->update();

  // Some mechanisms we query status directly

  isis->queue(modsIEHost[modsID],"dichroic");
  isis->queue(modsIEHost[modsID],"slitmask");

  // Reset state flags to clear pending command states

  // remoteCmd = false;  // this one is problematic
  isInstConfig = false;

}

// toggleCommLog() - Hide/View the communications log

void DashBoard::toggleCommLog()
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

void DashBoard::setLogButton()
{
  if (commLog->isHidden())
    commLogButton->setText(tr("View Comm"));
  else
    commLogButton->setText(tr("Hide Comm"));
}

// toggleErrLog() - Hide/Show the error/warning

void DashBoard::toggleErrLog()
{
  if (errLog->isHidden())
    errLog->show();
  else
    errLog->hide();
  errLog->activateWindow();
  setErrButton();
}

// setErrButton() - Set the error/warning log button label based on
// the visibility of the comm log

void DashBoard::setErrButton()
{
  if (errLog->isHidden())
    errLogButton->setText(tr("View Faults"));
  else
    errLogButton->setText(tr("Hide Faults"));
}

//
// addStatus() - Add an entry to the command status display with
//               color, and also put info into the communications log.
//
// We filter automated command replies to reduce noise (e.g., estatus queries)
//
// If an error, warning, or fatal fault condition, also pass it to the
// errLog widget for display

void DashBoard::addStatus(const QString &text, const QColor &color, const bool &logIt)
{
  if (text.isEmpty()) return;

  // Filter out routine messages that would just add noise to the status
  // display or comm log.  Current we filter the following high-traffic
  // status messages:
  //    estatus queries
  //    detector readout progress (pctread)
  // These help prevent accidentally burying error or warning message
  // especially during reaodut or other times of high message traffic.

  if (text.contains("estatus",Qt::CaseInsensitive) ||
      text.contains("pctread",Qt::CaseInsensitive)) return;
  
  // Display it in the status widget, and log in the comm log if requested

  statusDisplay->setText(text,color);
  if (logIt)
    commLog->append(text,color);

  // Display errors and warnings in the error/warning log

  if (text.contains("ERROR:",Qt::CaseInsensitive))
    errLog->append(text,Qt::red);
  else if (text.contains("WARNING:",Qt::CaseInsensitive))
    errLog->append(text,QColor(255,165,0,255));
  else if (text.contains("FATAL:",Qt::CaseInsensitive))
    errLog->append(text,Qt::magenta);
}

//---------------------------------------------------------------------------
//
// ISIS Client Socket communications bits - most are convenience functions
//

// setID() - set our IMPv2 client host ID

void DashBoard::setID(const QString &id)
{
  clientID = id;
  isis->setClient(clientID,clientPort);
}

// id() - return our IMPv2 client host ID

QString DashBoard::id() 
{
  return clientID;
}

// setPort() - set our client socket Port number

void DashBoard::setPort(const int &port)
{
  clientPort = port;
  isis->setClient(clientID,clientPort);
}

// setISISInfo() - Set the ISIS server ID, host, and port for this client

void DashBoard::setISISInfo(const QString &id, const QString &host, 
			    const int &port)
{
  isisID = id;
  isisPort = port;
  isisHost = host;
  isis->setServer(isisID,isisHost,isisPort);
}

// startISISClient() - Start the ISIS client UDP socket service

void DashBoard::startISISClient()
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

void DashBoard::sendToISIS(const QString &toAddr, const QString &msgStr)
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

void DashBoard::sendAndWait(const QString &toAddr, const QString &msgStr, 
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

void DashBoard::passMessage(const QString &msgStr)
{
  emit logMessage(msgStr);
}

// showMessage() - Show messages from child objects in the status
// display widget.  Display but do not log.

void DashBoard::showMessage(const QString &msgStr)
{
  if (msgStr.contains("ABORT",Qt::CaseInsensitive) ||
      msgStr.contains("PANIC",Qt::CaseInsensitive))
    addStatus(msgStr,Qt::red,false);
  else
    addStatus(msgStr,Qt::blue,false); 
}

// cmdQueue() - Enable/Disable queued message dispatch for most
// commands (changes the behavior of the sendToISIS() method).

void DashBoard::cmdQueue(const bool &flag)
{
  queue = flag;
}

// setDispatchCadence() - Set the dispatch queue cadence in
// milliseconds

void DashBoard::setDispatchCadence(const int &twait)
{
  if (twait > 0) {
    dispatchCadence = twait;
    isis->setDispatchCadence(dispatchCadence);
  }
}

// setMaxLampTime() - Set the maximum time any calibration lamp may be
// left one in seconds. <=0 means disable lamp timing.

void DashBoard::setMaxLampTime(const int &maxTime)
{
  maxLampTime = 0;
  if (maxTime > 0) {
    fpControlPanel->setMaxLampTime(maxTime);
    maxLampTime = maxTime;
  }
  else
    fpControlPanel->setMaxLampTime(0);
}


// loadTable() - Load a device table
//
// The format of a device table is
//
//  posNum posID [dFoc] posInfo
//
// posNum = position number, 1..Npos
// posID = string ID name (e.g., r_sdss)
// dFoc = optional focus offset value (int)
// posInfo = multi-word string describing posID
//
// The dFoc field is only used on the bfilter, rfilter, and dichroic
// devices.
//

void DashBoard::loadTable(const QString &device, 
			  const int &tabSize, 
			  QStringList *nameTab,
			  QStringList *infoTab)
{
  if (device.isEmpty()) return;
  if (tabSize < 0) return;

  bool ok;

  QString tabFile = QString("%1/Config/MODS%2/%3.tab").arg(MODS_BASE_PATH).arg(modsNum).arg(device);

  // Open the device table

  QFile inFile(tabFile);
  if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QString errMsg = tr("Could not open the MODS%1 device table %2!\n"
			"Reason:\n %3\nSystem defaults loaded instead.").arg(modsNum).arg(tabFile).arg(inFile.errorString());
    QMessageBox::warning(this,tr("Device Table Load Error"),errMsg); 
    return;
  }

  // Clear the device name and info tables and load the defaults, just
  // in case

  nameTab->clear();
  infoTab->clear();
  for (int i=0;i<tabSize;i++) {
    nameTab->append("Empty");
    infoTab->append("Position Empty");
  }
  
  // Read in the table line-by-line, parsing as we go.  # denotes
  // comments (either whole lines or inline comments), and we skip
  // spaces.  For this version, we only pay attention to the posNum,
  // posID, and posInfo fields.  Later versions will extract the dFoc
  // field, for now we ignore it.

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

	if (!itemStr.isEmpty()) {
	  int itemNum = itemStr.section(' ',0,0).toInt(&ok,10);
	  if (ok && (itemNum > 0 && itemNum <= tabSize)) {
	    QString itemName = itemStr.section(' ',1,1);  // position Name field
	    if (!itemName.isEmpty())
	      nameTab->replace(itemNum-1,itemName.simplified());
	    QString itemInfo; // position Info field
	    if (device.compare("rfilter",Qt::CaseInsensitive)==0 ||
		device.compare("bfilter",Qt::CaseInsensitive)==0 ||
		device.compare("dichroic",Qt::CaseInsensitive)==0)
	      itemInfo = itemStr.section(' ',3,-1);
	    else
	      itemInfo = itemStr.section(' ',2,-1);
	    if (!itemInfo.isEmpty())
	      infoTab->replace(itemNum-1,itemInfo.simplified());
	  }
	}
      }
    }
  } while (!line.isNull());
  inFile.close();
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

void DashBoard::parseShared(const QString &msgStr)
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

  if (remHost.compare(modsIEHost[modsID],Qt::CaseInsensitive)==0) {
    fpControlPanel->parse(msgBody,msgType,keyDict);
    blueCP->parse(msgBody,msgType,keyDict);
    redCP->parse(msgBody,msgType,keyDict);
  }
  else if (remHost.compare(modsBCHost[modsID],Qt::CaseInsensitive)==0 ||
	   remHost.compare(modsBDHost[modsID],Qt::CaseInsensitive)==0) {
    blueCP->parse(msgBody,msgType,keyDict);
  }
  else if (remHost.compare(modsRCHost[modsID],Qt::CaseInsensitive)==0 ||
	   remHost.compare(modsRDHost[modsID],Qt::CaseInsensitive)==0) {
    redCP->parse(msgBody,msgType,keyDict);
  }
  else if (remHost.compare(modsTCHost[modsID],Qt::CaseInsensitive)==0) {
    targPanel->parse(msgBody,msgType,keyDict);
  }
}

//
// Get a device position number by name
//
// Returns the device position corresponding the to argument in the
// second position (e.g., rfilter r_sdss).  
//
// * If the string position ID argument does not match anything in the
//   device table, it returns -1, and puts its gripe in the errorStr.
//
// * If the argument is a number (e.g., rfilter 2) return that 
//   number.
//
// * If the argument is reserved keyword (ABORT, RESET, etc.), or if
//   no arguments are given, return 0 (send as-is)
//

int DashBoard::posByName(const QString &cmdStr)
{
  bool ok;
  int iPos;

  if (cmdStr.isEmpty()) return 0;

  // How many arguments are there in the command string?

  QStringList wordList = cmdStr.split(" ");
  if (wordList.size()==1) return 0;  // just a query, send as-is

  // We have something to interpret, but first weed out reserved
  // keywords like ABORT, RESET, et al. in the 2nd argument.

  if (isIEKeyword(wordList.at(1))) return 0;

  // We have either a number or string to interpret
  
  iPos = wordList.at(1).toInt(&ok,10); // is the command argument numerical?
  if (ok) 
    return iPos;

  // It is not a number or a reserved word, is it a device table entry?

  QString devList;

  // Set up the reg-exp search: case-insensitive exact match (no wildcards)

  QRegularExpression rx(wordList.at(1), QRegularExpression::CaseInsensitiveOption);

  // Make sure this device has a device table...

  if (wordList.at(0).compare("dichroic",Qt::CaseInsensitive)==0) {
    iPos = dichroicName->indexOf(rx,0)+1;
    devList = dichroicName->join(",");
  }
  else if (wordList.at(0).compare("bfilter",Qt::CaseInsensitive)==0) {
    iPos = bFilterName->indexOf(rx,0)+1;
    devList = bFilterName->join(",");
  }
  else if (wordList.at(0).compare("rfilter",Qt::CaseInsensitive)==0) {
    iPos = rFilterName->indexOf(rx,0)+1;
    devList = rFilterName->join(",");
  }
  else if (wordList.at(0).compare("bgrating",Qt::CaseInsensitive)==0) {
    iPos = bGratingName->indexOf(rx,0)+1;
    devList = bGratingName->join(",");
  }
  else if (wordList.at(0).compare("rgrating",Qt::CaseInsensitive)==0) {
    iPos = rGratingName->indexOf(rx,0)+1;
    devList = rGratingName->join(",");
  }
  else if (wordList.at(0).compare("slitmask",Qt::CaseInsensitive)==0) {
    iPos = slitmaskName->indexOf(rx,0)+1;
    devList = slitmaskName->join(",");
  }
  else if (wordList.at(0).compare("agwfilt",Qt::CaseInsensitive)==0) {
    iPos = agwFilterName->indexOf(rx,0)+1;
    devList = agwFilterName->join(",");
  }
  else {
    errorStr = QString("%1 does not have a device table").arg(wordList.at(0));
    return -1;
  }

  // We got a valid position, send it

  if (iPos > 0)
    return iPos;

  // We did not get a valid position, create an error message and return
  // the error condition

  errorStr = QString("%1 Unknown position ID '%2', must be one of [%3]"
		     ).arg(wordList.at(0)).arg(wordList.at(1)).arg(devList);
  return -1;
}

// Set a grating tilt position by name

int DashBoard::tiltByName(const QString &cmdStr)
{
  int iGrat = 0;
  int iTilt;
  bool ok;

  if (cmdStr.isEmpty()) return 0;

  // How many arguments are there in the command string?

  QStringList wordList = cmdStr.split(" ");
  if (wordList.size()==1) return 0;  // just a query, send as-is

  // We have something to interpret, but first weed out reserved
  // keywords like ABORT, RESET, et al. in the 2nd argument.

  if (isIEKeyword(wordList.at(1))) return 0; // send as-is

  // We have either a number or string to interpret
  
  QString cmdWord = wordList.at(0);
  if (cmdWord.endsWith("2")) iGrat = 1;
  if (cmdWord.startsWith("r",Qt::CaseInsensitive)) {
    if (rGRTiltTable[iGrat].contains(wordList.at(1).toUpper()))
      iTilt = rGRTiltTable[iGrat].value(wordList.at(1).toUpper());
    else {
      if (isNumber(wordList.at(1))) {
	iTilt = wordList.at(1).toInt(&ok,10);
	if (ok) return iTilt;
      }
      errorStr = QString("%1 does not recognize preset value '%2'").arg(cmdWord).arg(wordList.at(1));
      return -1;
    }
  }
  else {
    if (bGRTiltTable[iGrat].contains(wordList.at(1).toUpper()))
      iTilt = bGRTiltTable[iGrat].value(wordList.at(1).toUpper());
    else {
      if (isNumber(wordList.at(1))) {
	iTilt = wordList.at(1).toInt(&ok,10);
	if (ok) return iTilt;
      }
      errorStr = QString("%1 does not recognize preset value '%2'").arg(cmdWord).arg(wordList.at(1));
      return -1;
    }
  }
  return iTilt;
}

// Load a grating tilt preset hash table.

void DashBoard::loadTiltTable(const QString &tiltDev) 
{
  if (tiltDev.isEmpty()) return;
  bool ok;

  // Clear the tilt table.  This lets us block invalid tiltDevs

  if (tiltDev.compare("BGRTILT1",Qt::CaseInsensitive)==0)
    bGRTiltTable[0].clear();
  else if (tiltDev.compare("BGRTILT2",Qt::CaseInsensitive)==0)
    bGRTiltTable[1].clear();
  else if (tiltDev.compare("RGRTILT1",Qt::CaseInsensitive)==0)
    rGRTiltTable[0].clear();
  else if (tiltDev.compare("RGRTILT2",Qt::CaseInsensitive)==0)
    rGRTiltTable[1].clear();
  else
    return;  // not a recognized device, do nothing

  // Open the tilt table 

  QString tiltFile = QString("%1/Config/MODS%2/%3.tab").arg(MODS_BASE_PATH).arg(modsNum).arg(tiltDev);

  QFile inFile(tiltFile);
  if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QString errMsg = tr("Could not open the MODS%1 grating tilt preset table %2!\n"
			"Reason:\n %3\nNo presets will be available."
			).arg(modsNum).arg(tiltFile).arg(inFile.errorString());
    QMessageBox::warning(this,tr("Grating Tilt Preset Table Load Error"),errMsg); 
    return;
  }

  // Read in the table line-by-line, parsing as we go.  # denotes
  // comments (either whole lines or inline comments), and we skip
  // spaces.  We use the comment in most of these files to provide
  // more detailed information on the tilt setting.  Note that
  // the first setting is also designated the Primary setting.

  QString tiltStr;
  QString line;
  int numEntries = 0;
  QTextStream in(&inFile);
  do {
    line = in.readLine();
    if (!line.isEmpty()) {  // skip blanks
      line = line.simplified();
      if (!line.startsWith("#")) { // skip comments
        if (line.contains("#")) // remove any inline comments
          tiltStr = line.section('#',0,0);
        else
          tiltStr = line;

	if (!tiltStr.isEmpty()) {
	  QString tiltID = tiltStr.section(' ',0,0);
	  if (!tiltID.isEmpty()) {
	    int tiltVal = tiltStr.section(' ',1,1).toInt(&ok,10);
	    if (ok) {
	      if (tiltDev.compare("RGRTILT1",Qt::CaseInsensitive)==0) {
		rGRTiltTable[0].insert(tiltID.toUpper(),tiltVal);
		if (numEntries == 0) rGRTiltTable[0].insert("PRIMARY",tiltVal);
	      }
	      else if (tiltDev.compare("RGRTILT2",Qt::CaseInsensitive)==0) {
		rGRTiltTable[1].insert(tiltID.toUpper(),tiltVal);
		if (numEntries == 0) rGRTiltTable[1].insert("PRIMARY",tiltVal);
	      }
	      else if (tiltDev.compare("BGRTILT1",Qt::CaseInsensitive)==0) {
		bGRTiltTable[0].insert(tiltID.toUpper(),tiltVal);
		if (numEntries == 0) bGRTiltTable[0].insert("PRIMARY",tiltVal);
	      }
	      else if (tiltDev.compare("BGRTILT2",Qt::CaseInsensitive)==0) {
		bGRTiltTable[1].insert(tiltID.toUpper(),tiltVal);
		if (numEntries == 0) bGRTiltTable[1].insert("PRIMARY",tiltVal);
	      }
	      numEntries++;
	    }
	  }
	}
      }
    }
  } while (!line.isNull());
  inFile.close();

}

// Is the string a number?

bool DashBoard::isNumber(const QString &testStr)
{
  bool ok;
  testStr.toDouble(&ok);
  return ok;
}

// Is the string one of the reserved IE command-line keywords?  We
// require an exact but case-insensitive match.  Note that we
// explicitly block the "M#" raw MicroLYNX directives, as these are
// way too dangerous to allow in scripts or on the general command
// line.

bool DashBoard::isIEKeyword(const QString &testStr)
{
  if (testStr.compare("STEP",Qt::CaseInsensitive)==0)
    return true;
  else if (testStr.compare("ABORT",Qt::CaseInsensitive)==0)
    return true;
  else if (testStr.compare("HOME",Qt::CaseInsensitive)==0)
    return true;
  else if (testStr.compare("RESET",Qt::CaseInsensitive)==0)
    return true;
  else if (testStr.compare("HARDRESET",Qt::CaseInsensitive)==0)
    return true;
  else if (testStr.compare("SOFTRESET",Qt::CaseInsensitive)==0)
    return true;
  else if (testStr.compare("FINDPOS",Qt::CaseInsensitive)==0)
    return true;
  else if (testStr.compare("RDBITS",Qt::CaseInsensitive)==0)
    return true;
  else if (testStr.compare("NOMINAL",Qt::CaseInsensitive)==0)
    return true;
  else if (testStr.compare("LOAD",Qt::CaseInsensitive)==0)
    return true;

  // These are for the SLITMASK command 

  else if (testStr.compare("IN",Qt::CaseInsensitive)==0)
    return true;
  else if (testStr.compare("OUT",Qt::CaseInsensitive)==0)
    return true;
  else
    return false;
}

// ISIS heartbeat functions

// Enable ISIS heartbeat

void DashBoard::enableHeartBeat(const bool &doHB)
{
  heartBeat = doHB;
  isis->enableHeartBeat(doHB);
}

// Set the ISIS heartbeat time interval in seconds

void DashBoard::setHeartBeatTime(const int &hbTime)
{
  if (hbTime > 0) 
    heartBeatTime = hbTime;
  else
    heartBeatTime = 0;
  isis->setHeartBeatTime(heartBeatTime);
}

// Reload all of the dashboard's device and configuration tables

void DashBoard::reloadTables()
{
  // Common Focal Plane control panel tables

  loadTable("agwfilt",MODS_AGWFILT_MAX,agwFilterName,agwFilterInfo);
  fpControlPanel->setAGwFiltName(agwFilterName);  

  // Slit Mask select widget 

  loadTable("slitmask",MODS_SLITMASK_MAX,slitmaskName,slitmaskInfo);
  slitmask->setIDList(slitmaskName);

  // Blue Channel control panel tables

  loadTable("bgrating",MODS_GRATING_MAX,bGratingName,bGratingInfo);
  blueCP->setGratingName(bGratingName);
  loadTiltTable("bgrtilt1");
  loadTiltTable("bgrtilt2");
  blueCP->loadTiltTable(QString("%1/Config/MODS%2/bgrtilt1.tab").arg(MODS_BASE_PATH).arg(modsNum),1);
  blueCP->loadTiltTable(QString("%1/Config/MODS%2/bgrtilt2.tab").arg(MODS_BASE_PATH).arg(modsNum),2);
  loadTable("bfilter",MODS_CAMFILT_MAX,bFilterName,bFilterInfo);
  blueCP->setFilterName(bFilterName);

  // Red Channel control panel tables

  loadTable("rgrating",MODS_GRATING_MAX,rGratingName,rGratingInfo);
  redCP->setGratingName(rGratingName);
  loadTiltTable("rgrtilt1");
  loadTiltTable("rgrtilt2");
  redCP->loadTiltTable(QString("%1/Config/MODS%2/rgrtilt1.tab").arg(MODS_BASE_PATH).arg(modsNum),1);
  redCP->loadTiltTable(QString("%1/Config/MODS%2/rgrtilt2.tab").arg(MODS_BASE_PATH).arg(modsNum),2);
  loadTable("rfilter",MODS_CAMFILT_MAX,rFilterName,rFilterInfo);
  redCP->setFilterName(rFilterName);

  // CCD ROI modes

  blueCP->loadROITable(QString("%1/Config/MODS%2/ccdroi.tab").arg(MODS_BASE_PATH).arg(modsNum));
   redCP->loadROITable(QString("%1/Config/MODS%2/ccdroi.tab").arg(MODS_BASE_PATH).arg(modsNum));

  // Blue and Red Channel Preset Configurations

  blueCP->loadConfigTable(QString("%1/Config/MODS%2/bconfig.tab").arg(MODS_BASE_PATH).arg(modsNum));
   redCP->loadConfigTable(QString("%1/Config/MODS%2/rconfig.tab").arg(MODS_BASE_PATH).arg(modsNum));

}

// Abort all operations on this dashboard

void DashBoard::abort()
{
  QMessageBox msgBox;
  msgBox.setText(tr("MODS%1 Full Instrument Abort Requested").arg(modsNum));
  msgBox.setInformativeText(tr("Are you sure you want to do this?"));
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::Yes);
  msgBox.setIcon(QMessageBox::Warning);
  int ret = msgBox.exec();
  switch (ret) {
  case QMessageBox::Yes:
    emit logMessage(QString("MODS%1 Full Instrument Abort Requested").arg(modsNum));
    redCP->execAbort();
    blueCP->execAbort();
    sendIECommand("ABORT ALL");
    break;
    
  default:
    break;
  }
}

// Enable/disable the environmental status monitor

void DashBoard::enableEnvMonitor(const bool &doESM)
{

  // If true, start the query timer if envQueryTime > 0

  if (doESM) {  
    envMonitor = true;
    if (envQueryTime > 0) {
      envTimer->stop();  // stop any delinquent timers
      envTimer->start(1000*envQueryTime);
    }
  }
  else { // stop the timer
    envMonitor = false;
    envTimer->stop();
  }    
}

// Set the environmental status monitor query time interval in seconds
// If the env status timer is running, set the new time now.  If the
// query time is set to 0, stop the env status timer.

void DashBoard::setEnvQueryTime(const int &esTime)
{
  if (esTime > 0) {
    envQueryTime = esTime;
    if (envMonitor) { // restart the timer only if running...
      envTimer->stop();
      envTimer->start(1000*esTime);
    }
  }
  else {
    envQueryTime = 0;
    if (envMonitor) envTimer->stop(); // stop the timer if running
  }
}

// Send an enviromental status (estatus) update query - triggered by a
// QTimer instance.  Performed by queing estatus queries the IE and
// Red/Blue IC hosts. 

void DashBoard::envUpdate()
{
  //isis->queue(modsIEHost[modsID],"estatus");
  isis->send(modsIMHost[modsID],"estatus");
  isis->send(modsBCHost[modsID],"estatus");
  isis->send(modsRCHost[modsID],"estatus");
}

// This slot is used to handle imcsLocked() signals

void DashBoard::imcsState(const QString &channel, const bool &isLocked)
{
  QString imcsMsg;
  bool isRed;

  // Which channel sent the isLocked() signal?

  if (channel.compare("red",Qt::CaseInsensitive)==0)
    isRed = true;
  else
    isRed = false;

  // What we do with the signal depends on how the original request was sent

  if (!isDIMCS) { // signal is in response to a single-channel lock-on request
    if (isRed) {
      redIMCSLock = isLocked;
      if (isLocked)
	imcsMsg = QString("DONE: RED IMCSLOCK Red-Channel IMCS locked-on RedIMCS=LockedOn");
      else
	imcsMsg = QString("DONE: RED IMCSLOCK Red-Channel IMCS failed to lock-on RedIMCS=Fail");
    }
    else {
      blueIMCSLock = isLocked;
      if (isLocked)
	imcsMsg = QString("DONE: BLUE IMCSLOCK Blue-Channel IMCS locked-on BlueIMCS=LockedOn");
      else
	imcsMsg = QString("DONE: BLUE IMCSLOCK Blue-Channel IMCS failed to lock-on BlueIMCS=Fail");
    }

  }

  // Dual-Channel Request was issued, this is one of the pending responses

  else {

    if (isRed) { // Red IMCS Lock-On Request Done
      redIMCSDone = true;
      redIMCSLock = isLocked;
      if (blueIMCSDone) {
	if (redIMCSLock && blueIMCSLock) { // both channels locked-on
	  imcsMsg = QString("DONE: IMCSLOCK IMCS Locked-On RedIMCS=LockedOn BlueIMCS=LockedOn");
	}
	else {
	  if (redIMCSLock)
	    imcsMsg = QString("ERROR: IMCSLOCK RedIMCS=LockedOn BlueIMCS=Fail");
	  else {
	    if (blueIMCSLock) 
	      imcsMsg = QString("ERROR: IMCSLOCK RedIMCS=Fail BlueIMCS=LockedOn");
	    else
	      imcsMsg = QString("ERROR: IMCSLOCK RedIMCS=Fail BlueIMCS=Fail");
	  }
	}
	// We are done, clean up
	isDIMCS = false;
	blueIMCSDone = true;
	redIMCSDone = true;
      }
      else { // Red is done, but blue is still pending.  Only report success, fail will report at the end.
	if (redIMCSLock)
	  imcsMsg = QString("STATUS: IMCSLOCK Red IMCS locked-on, Blue Pending RedIMCS=LockedOn BlueIMCS=Pending");
	else
	  imcsMsg.clear();
      }
    }
    else {  // Blue IMCS Lock-On Request Done
      blueIMCSDone = true;
      blueIMCSLock = isLocked;
      if (redIMCSDone) {
	if (redIMCSLock && blueIMCSLock) { // both channels locked
	  imcsMsg = QString("DONE: IMCSLOCK IMCS Locked-On RedIMCS=LockedOn BlueIMCS=LockedOn");
	}
	else {
	  if (redIMCSLock)
	    imcsMsg = QString("ERROR: IMCSLOCK RedIMCS=LockedOn BlueIMCS=Fail");
	  else {
	    if (blueIMCSLock) 
	      imcsMsg = QString("ERROR: IMCSLOCK RedIMCS=Fail BlueIMCS=LockedOn");
	    else
	      imcsMsg = QString("ERROR: IMCSLOCK RedIMCS=Fail BlueIMCS=Fail");
	  }
	}
	// We are done, clean up
	isDIMCS = false;
	blueIMCSDone = true;
	redIMCSDone = true;
      }
      else { // Blue is done, but red is still pending.  Only report success, fail will report at the end.
	if (blueIMCSLock)
	  imcsMsg = QString("STATUS: IMCSLOCK Blue IMCS locked-on, Red Pending BlueIMCS=LockedOn RedIMCS=Pending");
	else
	  imcsMsg.clear();
      }
    }      
  }

  // Send out a message (or display locally) 

  if (!imcsLockHost.isEmpty() && !imcsMsg.isEmpty()) {
    if (imcsLockHost.compare("local",Qt::CaseInsensitive)==0) 
      showMessage(imcsMsg);
    else
      sendToISIS(imcsLockHost,imcsMsg);
  }      
  
}

// Setup the beam-selector (dichroic/red flat/none) request

void DashBoard::setDichroic(const QString &newDich)
{
  reqDichroic = newDich;
  if (reqDichroic.compare("dual",Qt::CaseInsensitive)==0) {
    blueConfigEntry->enable();
    redConfigEntry->enable();
  }
  else if (reqDichroic.compare("blue",Qt::CaseInsensitive)==0) {
    blueConfigEntry->enable();
    redConfigEntry->disable();
  }
  else if (reqDichroic.compare("red",Qt::CaseInsensitive)==0) {
    blueConfigEntry->disable();
    redConfigEntry->enable();
  }
  commitButton->setChange();
}

// Blue channel configuration request

void DashBoard::setBlueConfig(const QString &cfgStr)
{
  reqBlueConfig = cfgStr;
  commitButton->setChange();
}

// Red channel configuration request

void DashBoard::setRedConfig(const QString &cfgStr)
{
  reqRedConfig = cfgStr;
  commitButton->setChange();
}

// Clear an uncommitted instrument configuration request

void DashBoard::clearConfig()
{
  reqDichroic.clear();
  reqBlueConfig.clear();
  reqRedConfig.clear();
  beamConfigEntry->setCurrentIndex(curBeamConfig);
  blueConfigEntry->setCurrentIndex(curBlueConfig);
  redConfigEntry->setCurrentIndex(curRedConfig);
  clearButton->setNormal();
  commitButton->setNormal();
  beamConfigEntry->setNormal();
  switch (curBeamConfig) {
  case 0:
    blueConfigEntry->enable();
    redConfigEntry->enable();
    break;
    
  case 1:
    blueConfigEntry->enable();
    redConfigEntry->disable();
    break;
    
  case 2:
    blueConfigEntry->disable();
    redConfigEntry->enable();
    break;
  }
}

// Commit a requested instrument configuration change request

void DashBoard::commitConfig()
{
  QString cmdStr;
  reqDichroic = beamConfigEntry->itemData(beamConfigEntry->currentIndex());
  if (reqDichroic.isEmpty()) return;

  if (reqDichroic.compare("DUAL",Qt::CaseInsensitive)==0) { // dual-channel config
    reqBlueConfig = blueConfigEntry->itemData(blueConfigEntry->currentIndex());
    reqRedConfig  = redConfigEntry->itemData(redConfigEntry->currentIndex());
    if (reqBlueConfig.isEmpty() || reqRedConfig.isEmpty()) return;
    if (reqBlueConfig == reqRedConfig) 
      cmdStr = QString("INSTCONFIG %1 %2").arg(reqDichroic).arg(reqBlueConfig);
    else
      cmdStr = QString("INSTCONFIG %1 %2 %3").arg(reqDichroic).arg(reqBlueConfig).arg(reqRedConfig);
  }
  else if (reqDichroic.compare("BLUE",Qt::CaseInsensitive)==0) {
    reqBlueConfig = blueConfigEntry->itemData(blueConfigEntry->currentIndex());
    if (reqBlueConfig.isEmpty()) return;
    cmdStr = QString("INSTCONFIG BLUE %1").arg(reqBlueConfig);
  }
  else if (reqDichroic.compare("RED",Qt::CaseInsensitive)==0) {
    reqRedConfig = redConfigEntry->itemData(redConfigEntry->currentIndex());
    if (reqRedConfig.isEmpty()) return;
    cmdStr = QString("INSTCONFIG RED %1").arg(reqRedConfig);
  }

  // Execute the configuration as a local INSTCONFIG command

  remoteCmd = false;  
  remoteHost = "local";
  commitButton->setNormal();
  clearButton->setNormal();
  QString *repStr = new QString("");
  doCommand(cmdStr,repStr);

}

