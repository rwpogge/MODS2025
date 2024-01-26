#ifndef UTILMANAGER_H
#define UTILMANAGER_H

#include <QWidget>
#include <QHostAddress>
#include <QFile>

#include "mods.h" // Parameters of the MODS User Interface

// Qt widgets

class QString;
class QUdpSocket;
class QTabWidget;

// Custom widgets

class UtilPanel;
class InstUtils;
class ActionButton;
class TextDisplay;
class ISISClient;

// UtilManager Class definition

class UtilManager : public QWidget
{
  Q_OBJECT

public:
  UtilManager(const QString &myID, const bool &useM1, const bool &useM2,
	      const int &myPort=0, QWidget *parent = 0);

  void setID(const QString &);  //!< set the IMPv2 client node ID for this application
  void setPort(const int &);    //!< set the port for this IMPv2 client node
  void setISISInfo(const QString &,const QString &, const int &); //!< set the ISIS server info
  QString id();            //!< return our IMPv2 client node ID string
  void startISISClient();  //!< start the ISIS (IMPv) client service
  void cmdQueue(const bool &);  //!< If true, enables queued dispatching, false is immediate dispatch
  void setDispatchCadence(const int &); //!< Set the dispatch queue cadence, if enabled

public slots:
  void sendToISIS(const QString &, const QString &);  //!< send a message to an ISIS client or server
  void sendAndWait(const QString &, const QString &, const int &); //!< send a message and wait for a reply 
  void update();  //!< Update the utility manager state
  void addStatus(const QString &, const QColor &); //!< Add a message to the status display
  void parseShared(const QString &);  //!< Parse an ISIS status message shared by a sibling

signals:
  void logMessage(const QString &);  //!< Send a message to the parent's logging mechanism
  void shareMsg(const QString &);    //!< Share a ISIS status message with any siblings
  
private slots:
  void parseISIS();  //!< Parse messages from the ISIS client socket
  void passMessage(const QString &);  //!< Pass along a logMessage() signal from a child
  void help();  //!< launch the utility manager help dialog

private:

  // UDP socket communications parameters for the utility manager's
  // IMPv2 client instance

  ISISClient *isis;     //!< IMPv2 client instance
  QString clientName;   //!< Long name of the client
  QString clientID;     //!< Client ID (IMPv2 protocol)
  int clientPort;       //!< Client UDP socket port for bind()
  QString isisID;       //!< ISIS server ID (IMPv2 protocol)
  QString isisHost;     //!< ISIS server host
  int isisPort;         //!< ISIS server port on isisHost
  bool useMODS1;        //!< MODS1 Enabled
  bool useMODS2;        //!< MODS2 Enabled
  bool queue;           //!< Is command dispatch queuing enabled?
  int  dispatchCadence; //!< Command dispatch queue cadence in milliseconds

  // The instrument utility control panels are layered in a QTabWidget

  QTabWidget *tabWidget;    //!< main tab widget for the utility management panels
  
  // Instrument Utility Control Panel

  UtilPanel *utilityPanel; //!< Main instrument utility control panel (MODS1 and MODS2)

  // Box-Level utility sub-panels for each instrument

  InstUtils *mods1Utils;  //!< MODS1 box-level utilities control panel
  InstUtils *mods2Utils;  //!< MODS2 box-level utilities control panel

  // Update and Help buttons

  ActionButton *updateButton;  //!< utility manager update button
  ActionButton *helpButton;    //!< utility manager help button

  // Status display

  TextDisplay *statusDisplay; //!< Command status display

};

#endif
