#ifndef IUBPANEL_H
#define IUBPANEL_H

#include <QWidget>
#include <QHostAddress>

#include "mods.h" // Parameters of the MODS User Interface

// Qt widgets

class QString;
class QLabel;
class QMessageBox;
class QCheckBox;

// Custom widgets

class PowerSwitch;
class PowerBreaker;
class PowerButton;
class ISISClient;
class ActionButton;
class TextDisplay;
class ValueDisplay;

// IUBPanel Class definition - used to operate the MODS AGW/Utility Box

class IUBPanel : public QWidget
{
  Q_OBJECT

public:
  IUBPanel(const int &myMODS,  QWidget *parent = 0);

  bool isSleeping();                //!< Report if the instrument is in low-power sleep mode
  void enable();                    //!< Enable the IUB control panel
  void disable();                   //!< Disable the IUB control panel

public slots:
  void parse(const QString &, const QString &, const int &, const QHash<QString,QString> &); //!< parse command replies
  void update();       //!< Refresh the IUB control panel
  void allOff();       //!< Turn off all instrument AC power 
  void sleep();        //!< Put the instrument to sleep 
  void wake();         //!< Wake up a sleeping instrument
  void setPowerLock(); //!< Lock/Unlock the IUB power controls

signals:
  void cmdReady(const QString &, const QString &);  //!< command ready to send to ISIS
  void cmdWait(const QString &, const QString &, const int &); //!< command with wait ready
  void statusMsg(const QString &, const QColor &);  //!< Emit a status message

private slots:
  void sendIECommand(const QString &); //!< Send a command
  void sendCmdWait(const QString &, const QString &, const int &); //!< send a command and wait or timeout

private:
  int modsID;          //!< MODS instrument ID (MODS1 or MODS2)
  QString panelName;   //!< Name of the IUB panel (e.g. MODS1 Instrument Utility Box)
  int numOn;           //!< Number of switched power outlets currently turned on
  bool sleepMode;      //!< Is the instrument in sleep mode (true) or not (false)
  bool powerOn;        //!< At least one power outlet is on (true) or all are off (false)

  // Power Switches

  PowerBreaker *blueIEBPower;   //!< Blue Instrument Electronics Box main power switch
  PowerBreaker *redIEBPower;    //!< Red Instrument Electronics Box main power switch
  PowerBreaker *blueHEBPower;   //!< Blue CCD Head Electronics Box main power switch
  PowerBreaker *redHEBPower;    //!< Red CCD Head Electronics Box main power switch
  PowerBreaker *agwGuidePower;  //!< AGW Guide Camera main power switch
  PowerBreaker *agwWFSPower;    //!< AGW Off-Axis Wavefront Sensor Camera main power switch
  PowerBreaker *llbPower;       //!< Lamp/Laser Box main power switch

  QCheckBox *lockPowerCheck;    //!< Lock/unlock the IUB Power control buttons
  bool lockPower;               //!< Power controls are locked out (true) or unlocked (false)

  double gsPressure;            //!< Glycol sypply pressure (psi-g)
  TextDisplay *gsPDisplay;      //!< Glycol sypply pressure display
  double gsTemp;                //!< Glycol supply temperature (deg C)
  TextDisplay *gsTDisplay;      //!< Glycol supply temperature display

  double grPressure;            //!< Glycol return pressure (psi-g)
  TextDisplay *grPDisplay;      //!< Glycol return pressure display
  double grTemp;                //!< Glycol return temperature (deg C)
  TextDisplay *grTDisplay;      //!< Glycol return temperature display

  double iubTemp;               //!< IUB box air temperature (deg C)
  TextDisplay *iubTDisplay;     //!< IUB box air temperature display

  double agwTemp;               //!< AGw Controller heat sink temperature (deg C)
  TextDisplay *agwTDisplay;     //!< AGw Controller heat sink temperature display

  double iebBTemp;              //!< Blue IEB Air Temperture (deg C)
  TextDisplay *iebBTDisplay;    //!< Blue IEB Air Temp display
  double iebBGTemp;             //!< Blue IEB Glycol Temperature (deg C)
  TextDisplay *iebBGDisplay;    //!< Blue IEB Glycol Temp display

  double iebRTemp;              //!< Red IEB Air Temperture (deg C)
  TextDisplay *iebRTDisplay;    //!< Red IEB Air Temp display
  double iebRGTemp;             //!< Red IEB Glycol Temperature (deg C)
  TextDisplay *iebRGDisplay;    //!< Red IEB Glycol Temp display
 
  double hebBTemp;              //!< Blue Camera HEB Air Temperature (deg C)
  TextDisplay *hebBTDisplay;    //!< Blue Camera HEB Temp display
  double hebBGTemp;             //!< Blue HEB Glycol Return Temperature (deg C)
  TextDisplay *hebBGDisplay;    //!< Blue Camera HEB Glycol Return Temp display

  double hebRTemp;              //!< Red Camera HEB Air Temperature (deg C)
  TextDisplay *hebRTDisplay;    //!< Red Camera HEB Temp display
  double hebRGTemp;             //!< Red HEB Glycol Return Temperature (deg C)
  TextDisplay *hebRGDisplay;    //!< Red Camera HEB Glycol Return Temp display

  double ambTemp;               //!< Ambient air temperature (deg C)
  TextDisplay *ambTDisplay;     //!< Ambient air temperature display

  ValueDisplay *updateDisplay;  //!< Update time of the displays (UTC hh:mm:ss)

  ActionButton *updateButton;   //!< Button to update the widget

};

#endif
