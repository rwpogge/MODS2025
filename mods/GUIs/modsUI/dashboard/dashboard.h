#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <QWidget>
#include <QHostAddress>
#include <QFile>

#include "mods.h" // Parameters of the MODS User Interface

// Qt widgets

class QString;
class QTextEdit;
class QLineEdit;
class QLabel;
class QPushButton;
class QUdpSocket;
class QDateTime;
class QCheckBox;
class QTimer;

// Custom widgets

class BiStateMechanism;
class IndexedMechanism;
class LinearMechanism;

class BiStateControl;
class IndexedSelector;
class LinearControl;
class ToggleSwitch;
class FocalPlane;
class TargetPanel;
class MODSChannel;

class CommLog;
class TextEntry;
class IntEntry;
class DoubleEntry;
class SelectEntry;
class TextHistEntry;
class TextDisplay;
class ValueDisplay;
class NumberEntry;
class LightButton;
class ActionButton;
class LED;
class ISISClient;

// DashBoard Class definition

class DashBoard : public QWidget
{
  Q_OBJECT

public:
  DashBoard(const int &myMODS, const int &myPort=0, QWidget *parent = 0);

  void addStatus(const QString &, const QColor &, const bool &); //!< add a status message
  void setID(const QString &);  //!< set the IMPv2 client node ID for this application
  void setPort(const int &);    //!< set the port for this IMPv2 client node
  void setISISInfo(const QString &,const QString &, const int &); //!< set the ISIS server info
  void startISISClient();  //!< start the ISIS (IMPv) client service
  void cmdQueue(const bool &);  //!< If true, enables queued dispatching, false is immediate dispatch
  void setDispatchCadence(const int &); //!< Set the dispatch queue cadence in msec, if enabled
  void enableHeartBeat(const bool &);  //!< If true, enables IMPv2 heartbeat, false disables it
  void setHeartBeatTime(const int &);  //!< Set the IMPv2 heartbeat time in seconds
  void enableEnvMonitor(const bool &); //!< If true, enables the environmental status monitor, false disables
  void setEnvQueryTime(const int &);   //!< Set the environmental status query interval in seconds
  void loadTable(const QString &, const int &, QStringList *, QStringList *); //!< Load a device table
  void reloadTables();  //!< Reload all device and dashboard configuration tables
  void setMaxLampTime(const int &);  //!< Set the maximum lamp-on time in seconds
  QString id(); //!< return our IMPv2 client node ID string
  bool isNumber(const QString &);  //!< is the string a number?
  bool isIEKeyword(const QString &); //!< is the string one of the value IE command-line keywords?

public slots:
  void update();   //!< Update the dashboard control panel entries
  void abort();    //!< Panic abort all dashboard functions
  void sendToISIS(const QString &, const QString &);  //!< send a message to an ISIS client or server
  void sendAndWait(const QString &, const QString &, const int &); //!< send a message and wait 
  void parseShared(const QString &);  //!< Parse an ISIS status message shared by a sibling
  void execCommand(const QString &);  //!< Execute an instrument command requested by a sibling
  void csqCommand(const QString &);   //!< Execute a command sequencer (CSQ) instance command

signals:
  void logMessage(const QString &);   //!< Send a message to the parent's logging mechanism
  void shareMsg(const QString &);     //!< Share an ISIS status message with any siblings
  void csqMessage(const QString &);   //!< Command Sequencer (CSQ) message to pass

private slots:
  void getCommand();       //!< Get a command from the command entry widget
  void doCommand(const QString &, QString *); //!< Interpret and execute a command
  void parseISIS();        //!< Parse messages from the ISIS client socket
  void toggleCommLog();    //!< Toggle the communications log display on/off (show/hide)
  void setLogButton();     //!< Change the communications log button
  void toggleErrLog();     //!< Toggle the error/warning log display on/off (show/hide)
  void setErrButton();     //!< Change the error/warning log button
  void homeAGw();          //!< Home the AGw stage
  void launchAGwDialog();  //!< Launch the AGw stage control panel
  void sendIECommand(const QString &); //!< Execute an IE command.
  void passMessage(const QString &);  //!< Pass along a logMessage() signal from a child
  void showMessage(const QString &);  //!< Show a logMessage() signal from a child
  void loadTiltTable(const QString &); //!< Load a grating tilt preset hash table
  void envUpdate();  //!< Send an enviromental status (estatus) update query
  void imcsState(const QString &,const bool &); //!< Handle IMCS lock-on signals
  void setDichroic(const QString &);   //!< Request the dichroic mode for the instrument configuration
  void setBlueConfig(const QString &); //!< Request the blue channel configurtion
  void setRedConfig(const QString &);  //!< Request the red channel configurtion
  void commitConfig();                 //!< Commit the selected instrument configuration request
  void clearConfig();                  //!< Clear an uncommitted instrument configuration request
  
private:
  int modsID;      //!< MODS instrument ID (MODS1 or MODS2)
  int modsNum;     //!< MODS number (1 or 2) - convenience parameter

  // UDP socket communications parameters for the dashboard's IMPv2
  // client instance

  ISISClient *isis;       //!< IMPv2 client socket
  QString clientName;     //!< Long name of the client
  QString clientID;       //!< Client ID (IMPv2 protocol)
  int clientPort;         //!< Client UDP socket port for bind()
  QString isisID;         //!< ISIS server ID (IMPv2 protocol)
  QString isisHost;       //!< ISIS server host
  int isisPort;           //!< ISIS server port on isisHost
  bool queue;             //!< Is command dispatch queuing enabled?
  int  dispatchCadence;   //!< Command dispatch queue cadence in milliseconds
  bool heartBeat;         //!< Is the ISIS heartbeat message enabled?
  int  heartBeatTime;     //!< ISIS heartbeat time in seconds

  // Communication Processing Flags
  
  bool remoteCmd;     //!< Did the command original remotely (true) or locally (false)?
  QString remoteHost; //!< Who did the message originate from?
  QHash<QString,QString> cmdHost;  //!< Hash table of remote command hosts
  QHash<QString,bool> dualCmd;     //!< Implicit dual command hash

  // Configuration file path

  QString configPath;  //!< full qualified path to the configuration file directory

  // Generic Error String

  QString errorStr;    //!< Error condition string

  // Focal Plane Suite mechanisms and controls

  FocalPlane  *fpControlPanel;   //!< Focal Plane control panel

  // Target Entry Panel

  TargetPanel *targPanel;   //!< Target and guide star entry panel

  // The Blue and Red Channel control panels

  MODSChannel *blueCP;  //!< Blue channel mechanism and exposure control panel
  MODSChannel *redCP;   //!< Red channel mechanism and exposure control panel
  
  // Mechanism Name and Info tables

  QStringList *agwFilterName; //!< AGw Filter Wheel Name List
  QStringList *agwFilterInfo; //!< AGw Filter Info List

  QStringList *slitmaskName;  //!< Slit Mask Name List
  QStringList *slitmaskInfo;  //!< Slit Mask Info List

  QStringList *dichroicName;  //!< Dichroic Drum Name List
  QStringList *dichroicInfo;  //!< Dichroic Drum Info List

  QStringList *bGratingName;  //!< Blue Channel Grating Turret Name List
  QStringList *bGratingInfo;  //!< Blue Grating Turret Info List

  QStringList *bFilterName;   //!< Blue Channel Camera Filter Name List
  QStringList *bFilterInfo;   //!< Blue Camera Filter Info List

  QStringList *rGratingName;  //!< Red Channel Grating Turret Name List
  QStringList *rGratingInfo;  //!< RedGrating Turret Info List

  QStringList *rFilterName;   //!< Red Channel Camera Filter Name List
  QStringList *rFilterInfo;   //!< Red Camera Filter Info List

  // The preset grating tilt tables are hash tables

  QHash<QString,int> rGRTiltTable[2];  //!< Hash table of red grating tilt presets
  QHash<QString,int> bGRTiltTable[2];  //!< Hash table of red grating tilt presets

  // Interactive Command and Status widgets
    
  TextHistEntry *commandEntry; //!< Interactive command line entry widget
  TextDisplay *statusDisplay;  //!< Command status display widget

  // Session Communications Log widget (pop-up. modeless dialog box)

  CommLog      *commLog;       //!< Communications log widget
  ActionButton *commLogButton; //!< Show/hide the communications log

  // Session Error/Warning Log widget (pop-up, modeless dialog)

  CommLog      *errLog;       //!< Error/warning log widget
  ActionButton *errLogButton; //!< Show/hide the error log

  // Other buttons of use

  ActionButton *updateButton;  //!< control panel refresh button
  ActionButton *abortButton;   //!< dashboard-wide panic abort button

  // Dual-Channel Image Acquisition ("DGO") state variables

  bool isDGO;      //!< Are we in dual-acquisition mode?
  bool blueGoDone; //!< Blue channel data acquisition is done
  bool redGoDone;  //!< Red channel data acquisition is done

  // Slit Mask Selection and Insert/Retract 

  IndexedMechanism *slitmask;        //!< Slit Mask select/insert mechanism
  IndexedSelector  *slitControl;     //!< Slit Mask Select Indexed control widget

  BiStateMechanism *minsert;         //!< Slit Mask insert/retract
  BiStateControl   *minsertControl;  //!< Slit Mask insert/retract control widget

  // Instrument Configuration (INSTCONFIG) controls and state variables

  SelectEntry  *beamConfigEntry; //!< Select the beam-selector configuration (dual/red/blue)
  QString reqDichroic;           //!< Requested dichroic config
  int curBeamConfig;             //!< Current dichroic config
  SelectEntry  *blueConfigEntry; //!< Select the blue-channel configuration
  QString reqBlueConfig;         //!< Requested blue channel configuration
  int curBlueConfig;             //!< Current blue channel configuration
  SelectEntry  *redConfigEntry;  //!< Select the red-channel configuratin
  QString reqRedConfig;          //!< Requested red channel configuration
  int curRedConfig;              //!< Current red channel configuration
  ActionButton *commitButton;    //!< Commit a configuration selection
  ActionButton *clearButton;     //!< Clear an uncommitted configuration selection

  bool isInstConfig;     //!< Are we executing an instrument configuration?
  bool isSingleConfig;   //!< Is this a single-channel CONFIG command?
  int numConfigPending;  //!< Number of configuration commands pending
  int numConfigErrors;   //!< Number of configuration errors

  // Dual-Channel IMCS state variables

  bool isDIMCS;  //!< Are we in dual-IMCS lock-on wait?
  bool blueIMCSDone;  //!< Blue-channel IMCS lock-on request done
  bool redIMCSDone;   //!< Red-channel ICMS lock-on request done
  bool blueIMCSLock;  //!< Is the blue-channel IMCS locked on?
  bool redIMCSLock;   //!< Is the red-channel IMCS locked on?
  QString imcsLockHost;  //!< HostID requesting IMCS lock-on

  // Enviromental Status Monitoring

  bool envMonitor;    //!< Is the enviromental status monitor enabled
  int  envQueryTime;  //!< Environmental status monitor query time in seconds
  QTimer *envTimer;   //!< Environmental status monitor query timer object

  // Calibration Lamp state parameters

  int maxLampTime;    //!< Default maximum calibration lamp-on time in seconds

  // Some private methods

  int posByName(const QString &);   //!< Return the mechanism position number corresponding to a position ID table entry
  int tiltByName(const QString &);  //!< Return the grating tilt corresponding to a named tilt

};

#endif
