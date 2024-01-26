#ifndef INSTMONITOR_H
#define INSTMONITOR_H

#include <QWidget>
#include <QHostAddress>
#include <QFile>
#include <QHash>

#include "mods.h" // Parameters of the MODS User Interface

// Qt widgets

class QString;
class QUdpSocket;
class QTabWidget;

// Custom widgets

class CommViewer;
class ActionButton;
class TextDisplay;
class ISISClient;
class EnvPanel;
class CommLog;

// InstMonitor Class definition

class InstMonitor : public QWidget
{
  Q_OBJECT

public:
  InstMonitor(const QString &myID, const bool &useM1, const bool &useM2,
	      const int &myPort=0, QWidget *parent = 0);

  void setISISInfo(const QString &,const QString &, const int &); //!< set the ISIS server info
  QString id();                 //!< return our IMPv2 client node ID string
  void setID(const QString &);  //!< set the IMPv2 client node ID for this application
  void setPort(const int &);    //!< set the port for this IMPv2 client node
  void startISISClient();       //!< start the ISIS (IMPv) client service
  void cmdQueue(const bool &);  //!< If true, enables queued dispatching, false is immediate dispatch
  void setDispatchCadence(const int &); //!< Set the dispatch queue cadence, if enabled

public slots:
  void sendToISIS(const QString &, const QString &);  //!< send a message to an ISIS client or server
  void sendAndWait(const QString &, const QString &, const int &); //!< send message and wait for reply 
  void update();    //!< Update the instrument monitor state
  void cancelAll(); //!< Cancel/clear all alert conditions
  void addMessage(const QString &); //!< Add a message to the communications viewer
  void addStatus(const QString &, const QColor &); //!< Add a message to the status display
  void parseShared(const QString &);  //!< Parse an ISIS status message shared by a sibling

signals:
  void logMessage(const QString &);  //!< Send a message to the parent's logging mechanism
  void shareMsg(const QString &);    //!< Share a ISIS status message with any siblings
  void alert(const QString &);       //!< Monitor alert signal - QString contains alert message
  void cancelAlerts();               //!< Cancel/clear instrument sensor monitor alerts

private slots:
  void parseISIS();      //!< Parse messages from the ISIS client socket
  void passMessage(const QString &); //!< Pass along a logMessage() signal from a child
  void passAlert(const QString &);   //!< Pass along an alert() signal from a child
  void passCancel();     //!< Pass along a cancelAlerts() signal from a child
  void toggleAlertLog(); //!< Toggle the alert log display on/off
  void setAlertButton(); //!< Change the alert log toggle button
  void logAlert(const QString &, const QColor &); //!< Append a message to the alert log

private:
  ISISClient *isis;     //!< IMPv2 client instance
  QString clientName;   //!< Long name of the client
  QString clientID;     //!< Client ID (IMPv2 protocol)
  int clientPort;       //!< Client UDP socket port for bind()
  QString isisID;       //!< ISIS server ID (IMPv2 protocol)
  QString isisHost;     //!< ISIS server host
  int isisPort;         //!< ISIS server port on isisHost
  bool queue;           //!< Is command dispatch queuing enabled?
  int  dispatchCadence; //!< Command dispatch queue cadence in milliseconds

  // Instrument Configuration 

  bool useMODS1;        //!< MODS1 Enabled
  bool useMODS2;        //!< MODS2 Enabled

  // The instrument monitoring panels are layered in a QTabWidget

  QTabWidget *tabWidget;    //!< Tab Widget to organize the instrument monitors
  
  // Communications monitor

  CommViewer *commMonitor;  //!< Instrument communications log viewer

  // Instrument environmental monitor panels (coming soon).  We use CommViewers for placeholders

  EnvPanel *mods1Env;  //!< MODS1 environmental monitor panel
  EnvPanel *mods2Env;  //!< MODS2 environmental monitor panel

  // Alert Log widget (pop-up, modeless dialog box)

  CommLog      *alertLog;        //!< Alert log widget
  ActionButton *alertLogButton;  //!< Show/Hide the alert log

  // Update and Help buttons

  ActionButton *updateButton;  //!< instrument monitor update button
  ActionButton *cancelButton;  //!< instrument alert cancel/clear button

  // Status display

  TextDisplay *statusDisplay; //!< Command status display

};

#endif
