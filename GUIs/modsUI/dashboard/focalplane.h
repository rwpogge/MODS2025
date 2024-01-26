#ifndef FOCALPLANE_H
#define FOCALPLANE_H

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
class QTimer;
class QCheckBox;

// Custom widgets

class BiStateMechanism;
class IndexedMechanism;
class LinearMechanism;

class BiStateControl;
class IndexedSelector;
class LinearControl;
class LinearSetting;
class ToggleSwitch;

class SelectEntry;
class NumberEntry;
class LightButton;
class ActionButton;
class TextDisplay;

class ISISClient;

// FocalPlane Class definition

class FocalPlane : public QWidget
{
    Q_OBJECT

public:
  FocalPlane(const int &myMODS, QWidget *parent = 0);
  void update();                       //!< Update the control panel
  void setAGwFiltName(QStringList *);  //!< Set the AGw filter names
  QStringList agwFiltName();           //!< Get the AGw filter names
  void setChange(const QString &);     //!< Set the change state of a mechanism widget 
  void setMaxLampTime(const int &);    //!< Set the maximum lamp-on time
  int maxLampTime();                   //!< Get the maximum lamp-on time

public slots:
  void parse(const QString &, const int &, const QHash<QString,QString> &); //!< parse command replies
  void homeGuideProbe(); //!< Home the AGw stage
  void setLamp(const QString &);  //!< Set a calibration lamp

signals:
  void cmdReady(const QString &, const QString &);  //!< command ready to send to ISIS
  void cmdWait(const QString &, const QString &, const int &); //!< command ready, send to ISIS and wait up to a timeout interval
  void logMessage(const QString &);                 //!< a message for the log

private slots:
  void sendCommand(const QString &); //!< Send a command w/preprocessing as needed
  void sendCmdWait(const QString &, const int &); // Send command and wait
  void setLampsOff();      //!< Turn off all lamps
  void setVFlatLamp(const QString &);  //!< Toggle the variable-intensity flat-field lamp on/off
  void setVFlatInt(const double &);    //!< Set the variable-intensity lamp flat-field power
  void startLampTimer();    //!< Start the calibration lamp timer
  void setObsMode();        //!< Put the instrument in sky-observing mode
  void setCalMode();        //!< Put the instrument in internal calibration mode
  void setRestoreGP();      //!< Enable/Disable restoring GProbe XY on returning to OBSMODE

private:
  int modsID;    //!< MODS instrument ID (MODS1 or MODS2)

  // Focal Plane Suite mechanisms and controls

  BiStateMechanism *hatch;        //!< Instrument dark hatch
  BiStateControl   *hatchControl; //!< BiState (Open/Close) control widget

  BiStateMechanism *calib;         //!< Calibration tower
  BiStateControl   *calibControl;  //!< BiState (In/Out) control widget

  ToggleSwitch     *neLamp;        //!< Neon calibration lamp toggle switch
  ToggleSwitch     *hgLamp;        //!< Mercury calibration lamp toggle switch
  ToggleSwitch     *arLamp;        //!< Argon calibration lamp toggle switch
  ToggleSwitch     *xeLamp;        //!< Xenon calibration lamp toggle switch
  ToggleSwitch     *krLamp;        //!< Krypton calibration lamp toggle switch
  ToggleSwitch     *qthLamp1;      //!< QTH calibration lamp 1 toggle switch
  ToggleSwitch     *qthLamp2;      //!< QTH calibration lamp 2 toggle switch
  ToggleSwitch     *vflatLamp;     //!< Variable-intensity lamp toggle switch
  NumberEntry      *vflatSetting;  //!< Variable-intensity lamp setting entry widget
  double vflatInt;                 //!< Variable-intensity lamp set point [V]
  bool vflatIsOn;                  //!< Is the variable-intensity lamp on or off?
  ActionButton     *lampOffButton; //!< Calibration lamp master Off button

  LinearMechanism  *gprobeX;        //!< AGw Stage guide probe X position mechanism
  TextDisplay      *gpXDisplay;     //!< AGw Stage guide probe X position display

  LinearMechanism  *gprobeY;        //!< AGw Stage guide probe Y position mechanism
  TextDisplay      *gpYDisplay;     //!< AGw Stage guide probe Y position display

  LinearMechanism  *gpFocus;        //!< AGw Stage guide probe focus adjustment
  TextDisplay      *gpFocDisplay;   //!< AGw Stage guide probe focus display

  IndexedMechanism *agwFilter;      //!< AGw Guide Camera Filter selection mechanism
  IndexedSelector  *agwFiltControl; //!< AGw Guide Camera filter selector

  ActionButton     *agwHomeButton;  //!< AGw home button

  // Focal Plane Configuration Buttons

  ActionButton     *obsModeButton;   //!< Observing Mode Button
  ActionButton     *calModeButton;   //!< Calibration Mode Button
  QCheckBox        *gpRestoreCheck;  //!< CheckBox to enable/disable gprobe position restore on OBSMODE
  bool gpRestore;                    //!< restore GProbe to last saved position on switching to OBSMODE?
  bool hatchOpen;                    //!< Is the instrument dark hatch open?
  bool calibStow;                    //!< Is the calibration tower stowed?

  // Lamp timer

  QTimer *lampTimer;  //!< Calibration lamp timer
  int maxLampOnTime;  //!< Maximum lamp-on time in seconds
    
};

#endif
