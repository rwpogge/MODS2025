#include <QtWidgets>
#include <QSplashScreen>

#include "unistd.h"
#include "mainwindow.h"
#include "dashboard.h"
#include "commviewer.h"
#include "utilmanager.h"
#include "configmanager.h"
#include "isisconsole.h"
#include "instmonitor.h"

MainWindow::MainWindow(const QString &myCFG)
{

  QSplashScreen *splash = new QSplashScreen;
  splash->setPixmap(QPixmap(":/images/MODS1_OHIO.png"));
  splash->setFont(QFont("Helvetica",14,QFont::Normal));
  splash->show();
  Qt::Alignment topCenter = Qt::AlignHCenter | Qt::AlignTop;

  // Load the runtime config file that tells us what to do.  If empty, we use
  // the compiled-in defaults

  cfgFile = myCFG;
  splash->showMessage(QObject::tr("Loading runtime configuration file %1...").arg(cfgFile),
                      topCenter,Qt::white);
  loadConfig(cfgFile);

  // Start building the main window...

  setWindowTitle(title);

  splash->showMessage(QObject::tr("Setting up the MODS control panel..."),
                      topCenter,Qt::white);

  // Create an instrument monitor, this includes the communcations
  // logger (in and out of this application), and an instrument
  // environmental monitor panel.

  if (useHKM) {
    splash->showMessage(QObject::tr("Creating the Housekeeping Console..."),topCenter,Qt::white);
    int hkPort = basePort+5;
    houseKeeper = new InstMonitor(hkmHost,useMODS[MODS1],useMODS[MODS2],hkPort,this);
    houseKeeper->setISISInfo(isisID,isisHost,isisPort);
    houseKeeper->startISISClient();
    houseKeeper->cmdQueue(useDispatcher);
    houseKeeper->setDispatchCadence(dispatchCadence);
    connect(houseKeeper,SIGNAL(alert(const QString &)),this,SLOT(houseAlert(const QString &)));
    connect(houseKeeper,SIGNAL(cancelAlerts()),this,SLOT(cancelHouseAlert()));
  }

  // Create an ISIS engineering command console instance, and connect
  // its logMessage() signal to the houseKeeper (if used)

  if (useITerm) {
    splash->showMessage(QObject::tr("Creating the Engineering Console..."),topCenter,Qt::white);
    int itPort = basePort;
    iTerm = new ISISConsole("MODS Engineering Command Console",
			    itHost,itPort,isisID,isisHost,isisPort,this);
    if (useHKM) connect(iTerm,SIGNAL(logMessage(const QString &)),
			houseKeeper,SLOT(addMessage(const QString &)));
  }

  // Create the MODSn DashBoard instances for this session, and
  // connect their logMessage() signals to the houseKeeper (if used)

  // MODS1 Dashboard

  int mods1Port = basePort+1;

  if (useMODS[MODS1]) {
    splash->showMessage(QObject::tr("Creating the MODS1 Dashboard..."),topCenter,Qt::white);
    mods1 = new DashBoard(MODS1,mods1Port,this);
    if (useHKM) connect(mods1,SIGNAL(logMessage(const QString &)),
			houseKeeper,SLOT(addMessage(const QString &)));
    mods1->setISISInfo(isisID,isisHost,isisPort);
    mods1->startISISClient();
    mods1->cmdQueue(useDispatcher);
    mods1->setDispatchCadence(dispatchCadence);
    mods1->setHeartBeatTime(heartBeatTime);
    mods1->enableHeartBeat(useHeartBeat);
    mods1->setEnvQueryTime(envQueryTime);
    mods1->enableEnvMonitor(useEnvMonitor);
    mods1->setMaxLampTime(maxLampTime);
  }

  // MODS2 Dashboard, if enabled

  int mods2Port = basePort+2;

  if (useMODS[MODS2]) {
    splash->showMessage(QObject::tr("Creating the MODS2 Dashboard..."),topCenter,Qt::white);
    mods2 = new DashBoard(MODS2,mods2Port,this);
    if (useHKM) connect(mods2,SIGNAL(logMessage(const QString &)),
			houseKeeper,SLOT(addMessage(const QString &)));
    mods2->setISISInfo(isisID,isisHost,isisPort);
    mods2->startISISClient();
    mods2->cmdQueue(useDispatcher);
    mods2->setDispatchCadence(dispatchCadence);
    mods2->setHeartBeatTime(heartBeatTime);
    mods2->enableHeartBeat(useHeartBeat);
    mods2->setEnvQueryTime(envQueryTime);
    mods2->enableEnvMonitor(useEnvMonitor);
    mods2->setMaxLampTime(maxLampTime);
  }
  
  // Utilities Manager Window instance, if enabled

  if (useUM) {
    int umPort = basePort+3;
    splash->showMessage(QObject::tr("Creating the Utilities Manager..."),topCenter,Qt::white);
    utilsManager = new UtilManager(umHost,useMODS[MODS1],useMODS[MODS2],umPort,this);
    if (useHKM) connect(utilsManager,SIGNAL(logMessage(const QString &)),
			houseKeeper,SLOT(addMessage(const QString &)));

    utilsManager->setISISInfo(isisID,isisHost,isisPort);
    utilsManager->startISISClient();
    utilsManager->cmdQueue(useDispatcher);
    utilsManager->setDispatchCadence(dispatchCadence);
  }

  // MODS Instrument Configuration (session aka "setup") manager

  splash->showMessage(QObject::tr("Creating the Configuration Manager..."),topCenter,Qt::white);

  int cmPort = basePort+4;
  modsConfig = new ConfigManager(cmHost,useMODS[MODS1],useMODS[MODS2],cmPort,this);
  if (useHKM) connect(modsConfig,SIGNAL(logMessage(const QString &)),
		      houseKeeper,SLOT(addMessage(const QString &)));

  modsConfig->setISISInfo(isisID,isisHost,isisPort);
  modsConfig->startISISClient();
  modsConfig->cmdQueue(useDispatcher);
  modsConfig->setDispatchCadence(dispatchCadence);

  splash->showMessage(QObject::tr("Loading the (re)start configuration..."),topCenter,Qt::white);

  modsConfig->update();  // Load config and query TCS auth status

  // A QListWidget is used for the selection menu that will appear
  // along the lefthand side of the control panel

  splash->showMessage(QObject::tr("Creating the Main Control Panel..."),topCenter,Qt::white);

  contentsWidget = new QListWidget;
  contentsWidget->setViewMode(QListView::IconMode);
  contentsWidget->setIconSize(QSize(96,96));
  contentsWidget->setMovement(QListView::Static);
  contentsWidget->setMaximumWidth(138);
  contentsWidget->setMinimumWidth(138);
  contentsWidget->setSpacing(12);

  createIcons();

  // A stacked layout is used to organize the control panels

  QStackedWidget *pageWidget = new QStackedWidget;
  pageWidget->addWidget(modsConfig);
  if (useMODS[MODS1]) pageWidget->addWidget(mods1);
  if (useMODS[MODS2]) pageWidget->addWidget(mods2);
  if (useHKM) pageWidget->addWidget(houseKeeper);
  if (useUM) pageWidget->addWidget(utilsManager);
  if (useITerm) pageWidget->addWidget(iTerm);
  connect(contentsWidget,SIGNAL(currentRowChanged(int)),
	  pageWidget,SLOT(setCurrentIndex(int)));
  contentsWidget->setCurrentRow(0);

  QHBoxLayout *cpLayout = new QHBoxLayout;
  cpLayout->addWidget(contentsWidget);
  cpLayout->addWidget(pageWidget,1);

  // Various startup bits we should accomplish

  QGroupBox *pagesBox = new QGroupBox("");
  pagesBox->setLayout(cpLayout);

  // Now setup the central widget, actions, and menus

  splash->showMessage(QObject::tr("Creating the Main Widget..."),topCenter,Qt::white);

  setCentralWidget(pagesBox);

  createActions();
  createMenus();
  createStatusBar();

  // Setup shared message connections among sibling ISIS clients

  splash->showMessage(QObject::tr("Connecting message-sharing channels..."),
		      topCenter,Qt::white);

  if (useITerm && useUM) 
    connect(iTerm,SIGNAL(shareMsg(const QString &)),
	    utilsManager,SLOT(parseShared(const QString &)));

  if (useITerm) 
    connect(iTerm,SIGNAL(shareMsg(const QString &)),
	    modsConfig,SLOT(parseShared(const QString &)));
  
  if (useMODS[MODS1]) {
    connect(modsConfig,SIGNAL(shareMsg(const QString &)),
	    mods1,SLOT(parseShared(const QString &)));
    connect(mods1,SIGNAL(shareMsg(const QString &)),
	    modsConfig,SLOT(parseShared(const QString &)));
    if (useITerm) connect(iTerm,SIGNAL(shareMsg(const QString &)),
			  mods1,SLOT(parseShared(const QString &)));
    if (useHKM) connect(mods1,SIGNAL(shareMsg(const QString &)),
			houseKeeper,SLOT(parseShared(const QString &)));
    if (useUM) {
      connect(utilsManager,SIGNAL(shareMsg(const QString &)),
	      mods1,SLOT(parseShared(const QString &)));
      connect(mods1,SIGNAL(shareMsg(const QString &)),
	      utilsManager,SLOT(parseShared(const QString &)));
    }
  }

  if (useMODS[MODS2]) {
    connect(modsConfig,SIGNAL(shareMsg(const QString &)),
	    mods2,SLOT(parseShared(const QString &)));
    connect(mods2,SIGNAL(shareMsg(const QString &)),
	    modsConfig,SLOT(parseShared(const QString &)));
    if (useITerm) connect(iTerm,SIGNAL(shareMsg(const QString &)),
			  mods2,SLOT(parseShared(const QString &)));
    if (useHKM) connect(mods2,SIGNAL(shareMsg(const QString &)),
			houseKeeper,SLOT(parseShared(const QString &)));
    if (useUM) {
      connect(utilsManager,SIGNAL(shareMsg(const QString &)),
	      mods2,SLOT(parseShared(const QString &)));
      connect(mods2,SIGNAL(shareMsg(const QString &)),
	      utilsManager,SLOT(parseShared(const QString &)));
    }
  }

  // A busy wait to let things catch up without message pileup during
  // startup.

  busyWait(5);

  // Update various control panels - pace ourselves with busy waits.

  if (useUM) {
    splash->showMessage(QObject::tr("Querying Instrument Utility Status..."),topCenter,Qt::white);
    utilsManager->update();
    busyWait(5);
  }

  if (useMODS[MODS1]) {
    splash->showMessage(QObject::tr("Querying MODS1 Instrument Status..."),topCenter,Qt::white);
    mods1->update();
    busyWait(5);
  }

  if (useMODS[MODS2]) {
    splash->showMessage(QObject::tr("Querying MODS2 Instrument Status..."),
			topCenter,Qt::white);
    mods2->update();
    busyWait(5);
  }

  if (useHKM) {
    splash->showMessage(QObject::tr("Querying Instrument Sensors..."),
			topCenter,Qt::white);
    houseKeeper->update();
    busyWait(5);
  }

  // Shutdown the splash screen, we're ready to go...

  splash->close();
  delete splash;

}

// Close main window - ask if they want to save the runtime config

void MainWindow::closeEvent(QCloseEvent *event)
{
  if (areYouSure()) {
    modsConfig->saveConfig();  // Save the current .modsrc runtime configuration
    writeSettings();
    event->accept();
    qApp->quit(); // make sure everybody dies...
  } else {
    event->ignore();
  }
}

void MainWindow::about()
{
  QMessageBox::about(this, tr("About the MODS Control Panel"),
  tr("MODS Control Panel version %1\n"
     "Author:\n"
     "  R. Pogge (pogge@astronomy.ohio-state.edu)\n"
     "  The Ohio State University, Dept. of Astronomy\n"
     "Version Date: %2\n"
     "Compiled with Qt%3").arg(MODS_REV_NUMBER).arg(MODS_REV_DATE).arg(QT_VERSION_STR));
}

void MainWindow::commands()
{
  QMessageBox::information(this, tr("MODS Interactive Commands"),
  tr("MODS Interactive Command Summary:\n"
     "  coming soon...\n"
     "See the MODS website\n %1\n"
     "or the LBTO MODS wiki\n %2\n"
     "for more information.").arg(MODS_URL_WEB).arg(MODS_URL_WIKI));
}

// We have received an alert from the housekeeping system.  Turn the
// housekeeping system icon red to alert the user, and do something
// with the alert message text (TBD)

void MainWindow::houseAlert(const QString &msgStr)
{
  if (useHKM) {
    houseButton->setIcon(QIcon(":/images/housekeeping_alert.png"));
    houseButton->setToolTip(msgStr);
  }
}

// Cancel a housekeeping system alert.  Revert to the normal
// housekeeping icon.

void MainWindow::cancelHouseAlert()
{
  if (useHKM) {
    houseButton->setIcon(QIcon(":/images/housekeeping.png"));
    houseButton->setToolTip(tr("Open the Housekeeping Monitor Panel"));
  }
}

void MainWindow::createActions()
{
  exitAction = new QAction(tr("E&xit"), this);
  exitAction->setShortcut(tr("Ctrl+Q"));
  connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

  aboutAction = new QAction(tr("&About"), this);
  connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
  
  cmdHelpAction = new QAction(tr("&Commands"), this);
  connect(cmdHelpAction, SIGNAL(triggered()), this, SLOT(commands()));
  
  aboutQtAction = new QAction(tr("About &Qt"), this);
  connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
  
}

void MainWindow::createMenus()
{
  menuBar()->setFont(QFont("Helvetica",10,QFont::Normal));
  sessionMenu = menuBar()->addMenu(tr("&Session"));
  sessionMenu->addSeparator();
  sessionMenu->addAction(exitAction);

  menuBar()->addSeparator();

  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(aboutAction);
  helpMenu->addAction(cmdHelpAction);
  helpMenu->addAction(aboutQtAction);
}

// Create a status bar with a 5-second timeout

void MainWindow::createStatusBar()
{
  statusBar()->showMessage(tr("Ready"),5000);
  statusBar()->setFont(QFont("Helvetica",10,QFont::Normal));
}

void MainWindow::readSettings()
{
  QSettings settings("MODSControlPanel", "mods");
  QPoint pos = settings.value("pos").toPoint();
  QSize size = settings.value("size").toSize();
  resize(size);
  move(pos);
}

void MainWindow::writeSettings()
{
  QSettings settings("MODSControlPanel", "mods");
  settings.setValue("pos", pos());
  settings.setValue("size", size());
}

// Ask the user if they want to save the current configuration

bool MainWindow::areYouSure()
{
  QMessageBox::StandardButton ret;
  ret = QMessageBox::question(this, tr("MODS Control Panel"),
			      tr("Are you sure you want to quit?"),
			      QMessageBox::Yes | 
			      QMessageBox::No);
  if (ret == QMessageBox::No)
    return false;
  
  return true;
}

// Create the icons for the contentsWidget

void MainWindow::createIcons()
{
  configButton = new QListWidgetItem(contentsWidget);
  configButton->setIcon(QIcon(":/images/config.png"));
  configButton->setText(tr("Setup"));
  configButton->setTextAlignment(Qt::AlignHCenter);
  configButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

  if (useMODS[MODS1]) {
    mods1Button = new QListWidgetItem(contentsWidget);
    mods1Button->setIcon(QIcon(":/images/MODS.png"));
    mods1Button->setText(tr("MODS1"));
    mods1Button->setTextAlignment(Qt::AlignHCenter);
    mods1Button->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
  }

  if (useMODS[MODS2]) {
    mods2Button = new QListWidgetItem(contentsWidget);
    mods2Button->setIcon(QIcon(":/images/MODS.png"));
    mods2Button->setText(tr("MODS2"));
    mods2Button->setTextAlignment(Qt::AlignHCenter);
    mods2Button->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
  }

  if (useHKM) {
    houseButton = new QListWidgetItem(contentsWidget);
    houseButton->setIcon(QIcon(":/images/housekeeping.png"));
    houseButton->setText(tr("Housekeeping"));
    houseButton->setTextAlignment(Qt::AlignHCenter);
    houseButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
  }

  if (useUM) {
    utilsButton = new QListWidgetItem(contentsWidget);
    utilsButton->setIcon(QIcon(":/images/utilsmgr.png"));
    utilsButton->setText(tr("Utilities"));
    utilsButton->setTextAlignment(Qt::AlignHCenter);
    utilsButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
  }

  if (useITerm) {
    iTermButton = new QListWidgetItem(contentsWidget);
    iTermButton->setIcon(QIcon(":/images/terminal.png"));
    iTermButton->setText(tr("ITerm"));
    iTermButton->setTextAlignment(Qt::AlignHCenter);
    iTermButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
  }
}

// Public slot used to pass information to the status bar from child
// widgets.  Messages persist for 5 seconds (5000msec).  If a null or
// empty string is passed we clear the status bar.

void MainWindow::showStatus(const QString &statusStr)
{
  if (statusStr.isEmpty())
    statusBar()->clearMessage();
  else
    statusBar()->showMessage(statusStr,5000);
}

//
// Open and parse the contents of the runtime configuration file
//

void MainWindow::loadConfig(const QString &cfgFile)
{
  bool ok;

  // Set the runtime defaults set at compile time

  title    = MODS_SESSION_NAME;
  isisID   = MODS_ISIS_SERVER_ID;
  isisHost = MODS_ISIS_SERVER_HOST;
  isisPort = MODS_ISIS_SERVER_PORT;
  basePort = MODS_BASEPORT;
  useMODS[MODS1]=false;
  useMODS[MODS2]=false;
  for (int i=0;i<2;i++) {
    modsIEHost[i] = MODS_IE_HOST;
    modsBCHost[i] = MODS_BCCD_HOST;
    modsRCHost[i] = MODS_RCCD_HOST;
    modsBDHost[i] = MODS_BDM_HOST;
    modsRDHost[i] = MODS_RDM_HOST;   
    modsTCHost[i] = MODS_IIF_HOST;
    modsIMHost[i] = MODS_ENV_HOST;
  }
  modsConfigPath[MODS1] = QString("%1/Config/MODS1").arg(MODS_BASE_PATH);
  modsConfigPath[MODS2] = QString("%1/Config/MODS2").arg(MODS_BASE_PATH);
  modsSide[MODS1]=MODS1_SIDE;
  modsSide[MODS2]=MODS2_SIDE;
  useHKM   = MODS_USE_HKM;
  hkmHost  = MODS_HKM_HOST;
  useUM    = MODS_USE_UM;
  umHost   = MODS_UTILS_ID;
  useITerm = MODS_USE_ITERM;
  itHost   = MODS_ITERM_ID;
  cmHost   = MODS_CONFIG_ID;
  useSupport = false;      // disable engineering support functions by default
  useDispatcher = false;   // disable implicit message dispatching by default
  useHeartBeat = false;    // disable ISIS client heartbeat by default
  useEnvMonitor = false;   // disable the environmental status monitor by default
  dispatchCadence = MODS_QUEUE_CADENCE;  // default dispatcher queue cadence (milliseconds)
  heartBeatTime = MODS_HEARTBEAT_TIME;   // default heartbeat time (seconds)
  envQueryTime = MODS_ESTATUS_TIME;      // default environmental status query time (seconds)
  maxLampTime = MODS_MAX_LAMPTIME;       // default maximum time calibration lamps may be lit (seconds)

  if (cfgFile.isEmpty()) return; // null config file means "use defaults"

  // Create a QFile instance for the config file and open readonly

  QFile inFile(cfgFile);
  if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QString errMsg = tr("Could not open configuration file %1!\n"
			"Reason:\n %2\nSystem default settings loaded instead.").arg(cfgFile).arg(inFile.errorString());
    QMessageBox::warning(this,tr("Configuration File Load Error"),errMsg); 
    return;
  }

  // We have a config file, read it.

  // Read in the table line-by-line, parsing as we go.  # denotes
  // comments (either whole lines or inline comments), and we skip
  // spaces.

  QString itemStr;
  QString line;
  int modsID = -1;

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
	  if (keyWord.compare("ISISID",Qt::CaseInsensitive)==0) {
	    isisID = itemStr.section(' ',1,1);
	    if (isisID.isEmpty()) isisID = MODS_ISIS_SERVER_ID;
	  }
	  else if (keyWord.compare("ISISHOST",Qt::CaseInsensitive)==0) {
	    isisHost = itemStr.section(' ',1,1);
	    if (isisID.isEmpty()) isisID = MODS_ISIS_SERVER_HOST;
	  }
	  else if (keyWord.compare("ISISPORT",Qt::CaseInsensitive)==0) {
	    isisPort = itemStr.section(' ',1,1).toInt(&ok,10);
	    if (!ok || isisPort<=0) isisPort=MODS_ISIS_SERVER_PORT;
	  }
	  else if (keyWord.compare("BASEPORT",Qt::CaseInsensitive)==0) {
	    basePort = itemStr.section(' ',1,1).toInt(&ok,10);
	    if (!ok || basePort<=0) basePort=MODS_BASEPORT;
	  }
	  else if (keyWord.compare("NAME",Qt::CaseInsensitive)==0) {
	    title = itemStr.section(' ',1,-1);
	    if (title.isEmpty()) title = MODS_SESSION_NAME;
	  }
	  else if (keyWord.compare("MODS1:",Qt::CaseInsensitive)==0) {
	    modsID = MODS1;
	  }
	  else if (keyWord.compare("MODS2:",Qt::CaseInsensitive)==0) {
	    modsID = MODS2;
	  }
	  else if (keyWord.compare("USE",Qt::CaseInsensitive)==0) {
	    QString useWhat = itemStr.section(' ',1,1);
	    if (useWhat.compare("MODS1",Qt::CaseInsensitive)==0) 
	      useMODS[MODS1] = true;
	    else if (useWhat.compare("MODS2",Qt::CaseInsensitive)==0) 
	      useMODS[MODS2] = true;
	    else if (useWhat.compare("HKM",Qt::CaseInsensitive)==0) 
	      useHKM = true;
	    else if (useWhat.compare("TCS",Qt::CaseInsensitive)==0) 
	      useTCS = true;
	    else if (useWhat.compare("UTILS",Qt::CaseInsensitive)==0) 
	      useUM = true;
	    else if (useWhat.compare("ITERM",Qt::CaseInsensitive)==0) 
	      useITerm = true;
	    else if (useWhat.compare("SUPPORT",Qt::CaseInsensitive)==0) 
	      useSupport = true;
	    else if (useWhat.compare("DISPATCHER",Qt::CaseInsensitive)==0) 
	      useDispatcher = true;
	    else if (useWhat.compare("EXPBELL",Qt::CaseInsensitive)==0)
	      useExpBell = true;
	    else if (useWhat.compare("BELL",Qt::CaseInsensitive)==0)
	      useExpBell = true;
	    else if (useWhat.compare("HEARTBEAT",Qt::CaseInsensitive)==0) 
	      useHeartBeat = true;
	    else if (useWhat.compare("ENVMONITOR",Qt::CaseInsensitive)==0) 
	      useEnvMonitor = true;
	  }
	  else if (keyWord.compare("CADENCE",Qt::CaseInsensitive)==0) {
	    dispatchCadence = itemStr.section(' ',1,1).toInt(&ok,10);
	    if (!ok || dispatchCadence<=0) dispatchCadence = MODS_QUEUE_CADENCE;
	  }
	  else if (keyWord.compare("HEARTBEAT",Qt::CaseInsensitive)==0) {
	    heartBeatTime = itemStr.section(' ',1,1).toInt(&ok,10);
	    if (!ok || heartBeatTime<=0) heartBeatTime = MODS_HEARTBEAT_TIME;
	  }
	  else if (keyWord.compare("ENVMONITOR",Qt::CaseInsensitive)==0) {
	    envQueryTime = itemStr.section(' ',1,1).toInt(&ok,10);
	    if (!ok || envQueryTime<=0) envQueryTime = MODS_ESTATUS_TIME;
	  }
	  else if (keyWord.compare("MAXLAMPTIME",Qt::CaseInsensitive)==0) {
	    maxLampTime = itemStr.section(' ',1,1).toInt(&ok,10);
	    if (!ok) 
	      maxLampTime = MODS_MAX_LAMPTIME;
	    else
	      if (maxLampTime <=0) maxLampTime = 0;
	  }
	  else if (keyWord.compare("ITERM",Qt::CaseInsensitive)==0) {
	    itHost = itemStr.section(' ',1,1);
	    if (itHost.isEmpty()) itHost = MODS_ITERM_ID;
	  }
	  else if (keyWord.compare("HOUSEKEEPING",Qt::CaseInsensitive)==0) {
	    hkmHost = itemStr.section(' ',1,1);
	    if (hkmHost.isEmpty()) hkmHost = MODS_HKM_HOST;
	  }
	  else if (keyWord.compare("UTILITIES",Qt::CaseInsensitive)==0) {
	    umHost = itemStr.section(' ',1,1);
	    if (umHost.isEmpty()) umHost = MODS_UTILS_ID;
	  }

	  // Instrument-specific keywords, modsID=0,1 required

	  else if (keyWord.compare("FOCUS",Qt::CaseInsensitive)==0 ||
		   keyWord.compare("SIDE",Qt::CaseInsensitive)==0) {
	    if (modsID>-1) {
	      QString sideStr = itemStr.section(' ',1,1);
	      if (sideStr.compare("SX",Qt::CaseInsensitive)==0 ||
		  sideStr.compare("left",Qt::CaseInsensitive)==0) 
		modsSide[modsID] = "left";
	      else if (sideStr.compare("DX",Qt::CaseInsensitive)==0 ||
		       sideStr.compare("right",Qt::CaseInsensitive)==0) 
		modsSide[modsID] = "right";
	    }
	  }
	  else if (keyWord.compare("IEHOST",Qt::CaseInsensitive)==0) {
	    if (modsID>-1)
	      modsIEHost[modsID] = itemStr.section(' ',1,1);
	  }
	  else if (keyWord.compare("BLUEHOST",Qt::CaseInsensitive)==0) {
	    if (modsID>-1)
	      modsBCHost[modsID] = itemStr.section(' ',1,1);
	  }
	  else if (keyWord.compare("REDHOST",Qt::CaseInsensitive)==0) {
	    if (modsID>-1)
	      modsRCHost[modsID] = itemStr.section(' ',1,1);
	  }
	  else if (keyWord.compare("BDMHOST",Qt::CaseInsensitive)==0) {
	    if (modsID>-1) 
	      modsBDHost[modsID] = itemStr.section(' ',1,1);
	  }
	  else if (keyWord.compare("RDMHOST",Qt::CaseInsensitive)==0) {
	    if (modsID>-1) 
	      modsRDHost[modsID] = itemStr.section(' ',1,1);
	  }
	  else if (keyWord.compare("TCSHOST",Qt::CaseInsensitive)==0) {
	    if (modsID>-1) 
	      modsTCHost[modsID] = itemStr.section(' ',1,1);
	  }
	  else if (keyWord.compare("ENVHOST",Qt::CaseInsensitive)==0) {
	    if (modsID>-1) 
	      modsIMHost[modsID] = itemStr.section(' ',1,1);
	  }
	  else if (keyWord.compare("ConfigPath",Qt::CaseInsensitive)==0) {
	    if (modsID>-1) 
	      modsConfigPath[modsID] = itemStr.section(' ',1,1);
	  }
	}
      }
    }
  } while (!line.isNull());
  inFile.close();

}

//
// Busy Wait - sleep for MODS_BUSYTICK_TIME microseconds, then
// process pending events (the busy part of the wait)
//

void MainWindow::busyWait(const int &numTicks)
{
  for (int i=0;i<numTicks;i++) {  // ensures we don't have message pileup
    usleep(MODS_BUSYTICK_TIME);
    qApp->processEvents();
  }
}  

