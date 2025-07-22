#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Qt classes 

#include <QMainWindow>
#include <QHash>

class QAction;
class QMenu;
class QListWidget;
class QListWidgetItem;

// Custom classes

class DashBoard;
class CommViewer;
class UtilManager;
class ConfigManager;
class ISISConsole;
class InstMonitor;

// The main window class definition

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(const QString &myCFG="");
  void loadConfig(const QString &);  //!< load and parse the configuration file

protected:
  void closeEvent(QCloseEvent *event);

public slots:
  void showStatus(const QString &);  //!< show info on the status bar

private slots:
  void about();
  void commands(); 
  void houseAlert(const QString &);  //!< Housekeeping System Alert notification
  void cancelHouseAlert();           //!< Cancel active housekeeping system alerts

private:
  void createActions();
  void createMenus();
  void createStatusBar();
  void readSettings();
  void writeSettings();
  void createIcons();
  void busyWait(const int &); 

  bool areYouSure();

  // The central widget - a MODS control panel

  QString cfgFile;           //!< Session runtime config file
  QString title;             //!< Session window title (NAME keyword in config file)

  // DashBoard *mods;
  ConfigManager *modsConfig;  //!< Instrument configuration console
  // CommViewer  *houseKeeper;   //!< Instrument Housekeeping Console (logs, housekeeping etc.)
  InstMonitor *houseKeeper;   //!< Instrument Housekeeping Console (logs, housekeeping etc.)
  UtilManager *utilsManager;  //!< Instrument Power and Utilities Manager
  DashBoard   *mods1;         //!< MODS1 dashboard (master control panel)
  DashBoard   *mods2;         //!< MODS2 dashboard (master control panel)
  ISISConsole *iTerm;         //!< ISIS command console (aka iTerm)

  // Sidebar with icon buttons to bring various subwindows forward

  QListWidget *contentsWidget;
  QListWidgetItem *configButton;  //!< Configurator window button
  QListWidgetItem *mods1Button;   //!< MODS1 dashboard window button
  QListWidgetItem *mods2Button;   //!< MODS2 dashboard window button
  QListWidgetItem *houseButton;   //!< Housekeeping monitor window button
  QListWidgetItem *utilsButton;   //!< Utilities Manager window button
  QListWidgetItem *iTermButton;   //!< Engineering iTerm window button
  
  // menus
  
  QMenu   *sessionMenu;
  QMenu   *helpMenu;
  
  // actions
  
  QAction *exitAction;
  QAction *aboutAction;
  QAction *aboutQtAction;
  QAction *cmdHelpAction;

  // Control Panel configuration parameters, read from the mods.ini file

  QString isisID;     //!< ISIS server ID
  QString isisHost;   //!< ISIS server Host IP address
  int isisPort;       //!< ISIS server IP port number
  int basePort;       //!< Base IP port number for this control panel
  QString configPath; //!< Instrument Configuration root directory path

  // Instrument configuration info [i=MODS<i+1>]

  bool useMODS[2];           //!< MODS[1/2] instrument enabled
  QString modsConfigPath[2]; //!< MODS[1/2] instrument-specific configuration file path

  // Other client Host IDs we need

  bool useHKM;       //!< Use the Instrument Housekeeping Manager
  QString hkmHost;   //!< Instrument Housekeeping Monitor host ID

  bool useUM;        //!< Use the Utilties Manager 
  QString umHost;    //!< Instrument Utility Manager host ID

  bool useITerm;     //!< Use the iTerm panel
  QString itHost;    //!< ISIS Terminal session host ID

  QString cmHost;    //!< Instrument configuration management control panel host ID

  bool useSupport;   //!< Enable/Disable Engineering/Support Functions

  bool useTCS;       //!< Use the TCS interface

  // Runtime configuration parameters

  bool useDispatcher;  //!< Enable/disable implicit message dispatch queuing
  int dispatchCadence; //!< Message dispatcher queue cadence in milliseconds

  bool useHeartBeat;   //!< Enable/disable ISIS client heartbeat
  int  heartBeatTime;  //!< ISIS client heartbeat time in seconds

  bool useEnvMonitor;  //!< Enable/disable the environmental status monitor
  int  envQueryTime;   //!< Enviromental status query time in seconds

  int maxLampTime;     //!< Maximum time calibration lamps may be lit in seconds

  // User-specific Runtime configuration parameters, read from the
  // user's .modsrc file for restoration of user parameters on
  // restarts

  QString observers;  //!< Observer name string

};

#endif
