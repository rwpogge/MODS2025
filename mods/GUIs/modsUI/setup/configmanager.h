#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

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

// Custom widgets

class CommLog;
class TextEntry;
class TextHistEntry;
class TextDisplay;
class ValueDisplay;
class ActionButton;
class ISISClient;
class InstConfig;
class FileConfig;

// ConfigManager Class definition

class ConfigManager : public QWidget
{
  Q_OBJECT

public:
  ConfigManager(const QString &myID, const bool &useM1, const bool &useM2,
		const int &myPort=0, QWidget *parent = 0);

  // Public methods

  void addStatus(const QString &, const QColor &, const bool &); //!< add a status message
  void setID(const QString &);  //!< set the IMPv2 client node ID for this application
  void setPort(const int &);    //!< set the port for this IMPv2 client node
  void setISISInfo(const QString &,const QString &, const int &); //!< set the ISIS server info
  void startISISClient();  //!< start the ISIS (IMPv) client service
  void cmdQueue(const bool &);  //!< If true, enables queued dispatching, false is immediate dispatch
  void setDispatchCadence(const int &); //!< Set the dispatch queue cadence, if enabled
  void enableMODS(const int &, const bool &); //!< Enable/Disable a MODSn setup panel
  QString id();                 //!< return our IMPv2 client node ID string

public slots:
  void update();  //!< Update the setup control panel
  void sendToISIS(const QString &, const QString &);  //!< send a message to an ISIS client or server
  void sendAndWait(const QString &, const QString &, const int &); //!< send a message and wait for a reply 
  void parseShared(const QString &); //!< Parse an ISIS status message shared by a sibling
  void setParameter(const QString &, const QString &); //!< Set the named parameter on the instrument hosts
  void setObserver(const QString &); //!< Set the observer name(s)
  void setPartner(const QString &);  //!< Set the partner ID
  void setPropID(const QString &);   //!< Set the proposal ID
  void setPIName(const QString &);   //!< Set the PI Name(s)
  void setSupport(const QString &);  //!< Set the support scientist name(s)
  void setTelops(const QString &);   //!< Set the telescope operator name(s)
  void setComment(const QString &);  //!< Set the general comment string
  void saveConfig();                 //!< Save the configuration information info to the .modsrc file
  void sendConfig();                 //!< Send the observer configuration info
  void loadConfig();                 //!< Load the .modsrc runtime configuration file
  void clearForm();                  //!< Clear the setup form
  
signals:
  void logMessage(const QString &);  //!< Send a message to the parent's logging mechanism
  void shareMsg(const QString &);    //!< Share an ISIS status message with any siblings

private slots:
  void doCommand(const QString &, QString *);  //!< Execute a command
  void parseISIS();        //!< Parse messages from the ISIS client socket
  void toggleCommLog();    //!< Toggle the communications log display on/off
  void setLogButton();     //!< Change the communications log toggle button
  void passMessage(const QString &);  //!< Pass along a logMessage() signal from a child
  void showMessage(const QString &);  //!< Show a logMessage() signal from a child

private:
  bool useMODS1;     //!< MODS1 enabled?
  bool useMODS2;     //!< MODS2 enabled?
  QString sideMODS1; //!< MODS1 side on LBT (left or right)
  QString sideMODS2; //!< MODS2 side on LBT (left or right)

  // UDP socket communications parameters for the instconfig's IMPv2
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
  
  // Communication Processing Flags
  
  bool remoteCmd;     //!< Did the command original remotely (true) or locally (false)?
  QString remoteHost; //!< Who did the message originate from?
  QHash<QString,QString> cmdHost;  //!< Hash table of remote command hosts

  // Generic Error String

  QString errorStr;    //!< Error condition string

  // MODS1 and MODS2 instrument-level config control panels

  InstConfig *modsConfig[2];  //!< MODS1 & MODS2 instrument configuration panels
  FileConfig *modsFiles[2];   //!< MODS1 & MODS2 filename configuration panels

  // Observer/Project Information entry widgets
  
  TextEntry *observerEntry;   //!< Observer(s) entry widget
  TextEntry *partnerEntry;    //!< LBT Project partner entry widget
  TextEntry *propIDEntry;     //!< Project ID entry widget
  TextEntry *piNameEntry;     //!< PI name entry widget
  TextEntry *supportEntry;    //!< Support Scientist(s) entry widget
  TextEntry *telopsEntry;     //!< Telescope Operator(s) entry widget
  TextEntry *commentEntry;    //!< Comment entry widget
  ActionButton *saveButton;   //!< save observer config button
  ActionButton *applyButton;  //!< apply observer config button
  ActionButton *clearButton;  //!< clear the entry form
  ActionButton *reloadButton; //!< reload the runtime config file

  QString observerStr;        //!< Observer Name(s)
  QString partnerStr;         //!< Partner Name(s)
  QString propIDStr;          //!< Proposal ID
  QString piNameStr;          //!< PI Name
  QString supportStr;         //!< Support scientist name(s)
  QString telopsStr;          //!< Telescope Operator name(s)
  QString commentStr;         //!< General comment string

  // Other Buttons and Displays

  ActionButton *refreshButton; //!< setup panel refresh button
  ActionButton *helpButton;    //!< setup panel help button
  TextDisplay  *statusDisplay; //!< Panel command/control status display widget
  CommLog      *commLog;       //!< Communications log widget
  ActionButton *commLogButton; //!< Show/hide the communications log
  
};

#endif
