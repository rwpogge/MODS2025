#ifndef INSTCONFIG_H
#define INSTCONFIG_H

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
class QProgressBar;
class QRadioButton;

// Custom widgets

class TextEntry;
class TextHistEntry;
class TextDisplay;
class ValueDisplay;
class ActionButton;
class LED;
class ISISClient;

// InstConfig Class definition

class InstConfig : public QWidget
{
  Q_OBJECT

public:
  InstConfig(const int &myMODS, QWidget *parent = 0);

  // Public methods

  void update();    //!< Update the control panel & any subpanels
  QString error() const;  //!< Return the last error string
  void enable();  //!< disable the widget
  void disable(); //!< enable the widget

public slots:
  void parse(const QString &, const int &, const QHash<QString,QString> &); //!< parse command replies

signals:
  void cmdReady(const QString &, const QString &);  //!< command ready to send to ISIS 
  void cmdWait(const QString &, const QString &, const int &); //!< command ready, send to ISIS and wait up to a timeout interval
  void logMessage(const QString &);  //!< Signal with a message to log

private slots:  
  void doStartup();    //!< Perform instrument startup
  void doReset();      //!< Perform an instrument reset
  void doSleep();      //!< Put the instrument to sleep
  void doStow();       //!< Stow instrument mechanisms
  void doShutdown();   //!< Shutdown (power down) the instrument
  void checkLBTAuth(); //!< Check the telescope authorization state of the instrument
  void setWarmStart(); //!< Set warm-start mode for startup
  void setColdStart(); //!< Set cold-start mode for startup
  void launchSlitLoad();  //!< Launch the slit mask load dialog
  void launchRFiltLoad(); //!< Launch the red filter wheel load dialog
  void launchBFiltLoad(); //!< Launch the blue filter wheel load dialog
  void setLBTAuth(const bool &); //!< Set the LBT authorization state  (authorized|not)

private:
  int modsID;           //!< MODS Instrument ID (MODS1 or MODS2)
  QString lbtSide;      //!< LBT Side (left or right)
  QString focalStation; //!< MODS Focal Station Name
  QString errorStr;     //!< Error strings for error conditions

  // Action Buttons, Radio Buttons, and indicators

  ActionButton *startButton;     //!< Instrument startup button
  QRadioButton *warmStartButton; //!< warm-start mode radio button
  QRadioButton *coldStartButton; //!< cold-start mode radio button

  ActionButton *resetButton;     //!< Instrument reset button
  ActionButton *sleepButton;     //!< Instrument sleep button
  ActionButton *stowButton;      //!< Instrument stow button
  ActionButton *shutdownButton;  //!< Instrument shutdown button
  ActionButton *slitmaskButton;  //!< Slit Mask load dialog button
  ActionButton *bfilterButton;   //!< Blue Filter Load dialog button
  ActionButton *rfilterButton;   //!< Red Filter Load dialog button

  ActionButton *tcsButton;       //!< Connect/Disconnect LBT button (authorize/deauthorize)
  LED *authLight;                //!< LED indicating the instrument IIF authorization state
 
  // State variables and indicators

  bool warmStart; 
  bool isAuthorized;  //!< Is this MODS instances authorized on the telescope?
  
};

#endif
