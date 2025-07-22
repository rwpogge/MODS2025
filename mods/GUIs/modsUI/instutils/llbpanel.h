#ifndef LLBPANEL_H
#define LLBPANEL_H

#include <QWidget>
#include <QHostAddress>

#include "mods.h" // Parameters of the MODS User Interface

// Qt widgets

class QString;
class QCheckBox;

// Custom widgets

class PowerSwitch;
class PowerButton;
class ToggleSwitch;
class LightButton;
class ActionButton;
class TextDisplay;
class NumberEntry;

class ISISClient;

// LLBPanel Class definition - used to operate Lamp/Laser Box (LLB)
// internal management functions

class LLBPanel : public QWidget
{
  Q_OBJECT

public:
  LLBPanel(const int &myMODS, QWidget *parent = 0);
  void loadConfig(const QString &); //!< Load and parse the LLB configuration file

public slots:
  void parse(const QString &, const int &, const QHash<QString,QString> &); //!< parse command replies
  void update(); //!< Update the LLB control panel data
  void allLampsOff();  //!< Turn off all lamps

signals:
  void cmdReady(const QString &,const QString &);   //!< Command ready to execute
  void cmdWait(const QString &, const QString &, const int &); //!< Command ready to execute with wait/timeout
  void statusMsg(const QString &, const QColor &); //!< Status message for the parent status display

private slots:
  void sendCommand(const QString &); //!< Send a command
  void sendCmdWait(const QString &, const int &);  //!< Send a command with wait/timeout
  void setIRBeamPower(const double &); //!< Set the IR Laser Beam Power
  void setIRTemp(const double &);  //!< Set the IR laser head temperature
  void setVisBeamPower(const double &); //!< Set the Visible Laser Beam Power
  void resetIRLaser();   //!< Reset the IR laser
  void resetVisLaser();  //!< Reset the visible laser
  void updateIRLaser();  //!< Update the IR laser status
  void updateVisLaser(); //!< Update the visible laser status
  void enableIRLaser(const QString &);  //!< Enable the IR laser beam
  void enableVisLaser(const QString &); //!< Enable the visible laser beam
  void setIRPreset();  //!< Enable/Disable using the IR Laser power preset
  void setVisPreset(); //!< Enable/Disable using the Visible Laser power preset

private:
  int modsID;         //!< MODS instrument ID (MODS1 or MODS2) for this control panel

  // IR IMCS Laser Controls and Readouts

  PowerSwitch *irLaserSwitch;   //!< IR Laser AC power switch
  ToggleSwitch *irBeamSwitch;   //!< IR Laser beam enable/disable switch

  TextDisplay *irBeamDisplay;   //!< IR Laser Beam output power display
  double irBeamPower;           //!< IR Laser Beam output power [mW]

  QCheckBox *irPresetCheck;     //!< Checkbox to enable/disable IR laser preset power function
  bool useIRPreset;             //!< Use the IR Laser power preset?
  double irBeamPreset;          //!< IR Laser Beam output power preset [mW]

  NumberEntry *irBeamSetting;   //!< IR Laser Beam output power setting entry widget
  double irBeamSet;             //!< IR Laser Beam output power setpoint [mW]

  TextDisplay *irTempActDisp;   //!< IR Laser Head actual temperature display
  double irLaserTemp;           //!< IR Laser Head actual temperature [deg C]

  TextDisplay *irTempSetDisp;   //!< IR Laser Head temperature set-point display
  double irTempSet;             //!< IR Laser Head temperature set-point [deg C]
 
  ActionButton *irResetButton;  //!< Reset the IR laser system
  ActionButton *irUpdateButton; //!< Update IR laser system status

  // Visible-light alignment Laser Controls and Readout

  PowerSwitch *visLaserSwitch;   //!< Visible Laser AC power switch
  ToggleSwitch *visBeamSwitch;   //!< Visible Laser beam enable/disable switch

  TextDisplay *visBeamDisplay;   //!< Visible Laser Beam output power display
  double visBeamPower;           //!< Visible Laser Beam output power out [mW]

  QCheckBox *visPresetCheck;     //!< Checkbox to enable/disable Visible laser preset power function
  bool useVisPreset;             //!< Use the Visible Laser power preset?
  double visBeamPreset;          //!< Visible Laser Beam output power preset [mW]

  NumberEntry *visBeamSetting;   //!< Visible Laser Beam output power setting entry widget
  double visBeamSet;             //!< Visible Laser Beam output power setpoint [mW]

  ActionButton *visResetButton;  //!< Reset the visible laser system
  ActionButton *visUpdateButton; //!< Update visible laser system status

  // Calibration Lamp controls

  ToggleSwitch *neLamp;     //!< Neon PenRay calibration lamp power switch
  ToggleSwitch *hgLamp;     //!< Mercury/Argon PenRay calibration lamp power switch
  ToggleSwitch *arLamp;     //!< Argon PenRay calibration lamp power switch
  ToggleSwitch *xeLamp;     //!< Xenon PenRay calibration lamp power switch
  ToggleSwitch *krLamp;     //!< Krypton PenRay calibration lamp power switch
  ToggleSwitch *qthLamp[3]; //!< QTH continum lamps (0..2) power switches
  ToggleSwitch *qthSupply;  //!< QTH lamp 6V power supply switch
  ActionButton *allOff;     //!< Turn off all lamps

};

#endif
