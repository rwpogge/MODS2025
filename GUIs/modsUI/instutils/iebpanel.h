#ifndef IEBPANEL_H
#define IEBPANEL_H

#include <QWidget>
#include <QHostAddress>

#include "mods.h" // Parameters of the MODS User Interface

// Qt widgets

class QString;
//class QStringList;
class QCheckBox;

// Custom widgets

class PowerSwitch;
class PowerButton;
class ToggleSwitch;
class LightButton;
class ActionButton;
class TextDisplay;

class ISISClient;

// IEBPanel Class definition - used to operate Instrument Electronics
// Box (IEB) internal management functions

class IEBPanel : public QWidget
{
  Q_OBJECT

public:
  IEBPanel(const int &myMODS, const QString &myIEB, QWidget *parent = 0);

  void setMLIDs(QStringList *);     //!< Set the MicroLYNX mechanism IDs
  void loadConfig(const QString &); //!< Load and parse the IEB configuration file

public slots:
  void parse(const QString &, const int &, const QHash<QString,QString> &); //!< parse command replies
  void update();        //!< Update the IEB control panel data
  void setPowerLock();  //!< Lock/Unlock IEB power controls

signals:
  void cmdReady(const QString &, const QString &);  //!< Command ready to execute
  void cmdWait(const QString &, const QString &, const int &); //!< Command ready to execute with wait/timeout
  void statusMsg(const QString &, const QColor &); //!< Status message for the parent status display

private slots:
  void sendCommand(const QString &); //!< Send a command
  void sendCmdWait(const QString &, const int &); //!< Send a command with wait/timeout 
  void setMLynxPower(const QString &); //!< Set a MicroLYNX Controller power state on/off
  void setDrivePower(const QString &); //!< Set the IEB box motor drive power state on/off

private:
  int modsID;                  //!< Which MODS is this panel attached to (MODS1 or MODS2)
  int modsNum;                 //!< MODS number (1 or 2) - convenience parameter
  QString iebID;               //!< Which IEB is this panel controlling (B or R)
  QStringList *mlynxList;      //!< List of MicroLYNX controller mechanism assignments
  QStringList *tempSensorList; //!< List of Temperature readout assignments

  // Power Switches

  PowerSwitch *drivePower;    //!< IEB MicroLYNX Stepper Motor Controller Drive Power

  // MicroLYNX controller power toggle switches

  ToggleSwitch *mlSwitch[16];     //!< MicroLYNX controller power toggle switches
  bool mlSwitchOn[16];            //!< MicroLYNX Power State, true=on, false=off/unknown

  // Action Buttons

  ActionButton *updateButton;   //!< Update IEB status
  QCheckBox *lockPowerCheck;    //!< Lock/unlock the IEB power control buttons
  bool lockPower;               //!< Are IEB power control buttons locked?

  // IEB Box Readouts

  bool iebStatus;                      //!< Is the IEB online (true) or offline (false)?
  TextDisplay *iebStatusDisplay;       //!< IEB status (online or offline)

  double driveVoltage;                 //!< 65V motor-drive power supply voltage [VDC]
  TextDisplay *driveVoltageDisplay;    //!< 65V motor-drive power supply voltage display

  double driveCurrent;                 //!< 65V motor-drive power supply current [Amps]
  TextDisplay *driveCurrentDisplay;    //!< Motor-drive power supply current display

  double wagoVoltage;                  //!< 24V WAGO power supply voltage [VDC]
  TextDisplay *wagoVoltageDisplay;     //!< 24V WAGO power supply voltage display

  double temp[4];                      //!< Temperature readout (4 max per IEB)
  TextDisplay *tempDisplay[4];         //!< Temperature readout display widget

};

#endif
