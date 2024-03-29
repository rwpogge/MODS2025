#ifndef ENVPANEL_H
#define ENVPANEL_H

#include <QWidget>
#include <QHostAddress>

#include "mods.h" // Parameters of the MODS User Interface

// Qt widgets

class QString;
class QLabel;
class QMessageBox;

// Custom widgets

class ISISClient;
class ActionButton;
class TextDisplay;
class ValueDisplay;

// EnvPanel Class definition - MODS instrument environmental sensor monitor panel

class EnvPanel : public QWidget
{
  Q_OBJECT

public:
  EnvPanel(const int &myMODS,  QWidget *parent = 0);
  void enable();  //!< Enable the instrument sensor monitor panel
  void disable(); //!< Disable the instrument sensor monitor panel
  void logging(const bool &); //!< enable/disable logging
  bool setAlarm(const int &);    //!< Set an alarm condition - return true if send message, false if throttle message
  void clearAlarm(const int &);  //!< Clear an alarm condition
  void clearAll();               //!< Clear all alarm conditions

  // Enum with the instrument sensor list - make sure you count and update NUMSENSORS correctly!

  enum instSensor {
    AmbientTemp,    //!< Outside Ambient Air Temperature Sensor (deg C)
    IUB_AirTemp,    //!< IUB Inside Air Temperature Sensor (deg C)
    IUB_AGwTemp,    //!< IUB AGw Heat Sink Temperature Sensor (deg C)
    GSPres,         //!< Glycol Supply in-line Pressure Sensor (psi-g)
    GSTemp,         //!< Glycol Supply in-line Temperature Sensor (deg C)
    GRPres,         //!< Glycol Return in-line Pressure Sensor (psi-g)
    GRTemp,         //!< Glycol Return in-line Temperature Sensor (deg C)
    DGPres,         //!< Glycol differential pressure (Return-Supply in psi-g)  
    DGTemp,         //!< Glycol differential temperature (Return-Supply in deg C)
    IEB_B_AirTemp,  //!< Blue IEB Inside Air Temperature Sensor (deg C)
    IEB_B_GRTemp,   //!< Blue IEB Glycol Return Temperature Sensor (deg C)
    IEB_R_AirTemp,  //!< Red IEB Inside Air Temperature Sensor (deg C)
    IEB_R_GRTemp,   //!< Red IEB Glycol Return Temperature Sensor (deg C)
    TopAirTemp,     //!< Instrument Top Inside Air Temperature Sensor (deg C)
    BotAirTemp,     //!< Instrument Bottom Inside Air Temperature Sensor (deg C)
    DTInstAir,      //!< Instrument Air temperature difference (Top-Bottom, deg C)
    TopTrussTemp,   //!< Instrument Top Collimator Truss Temperature Sensor (deg C)
    BotTrussTemp,   //!< Instrument Bottom Collimator Truss Temperature Sensor (deg C)
    DTTruss,        //!< Instrument Collimator Truss temperature difference (Top-Bottom, deg C)
    HEB_B_AirTemp,  //!< Blue HEB Inside Air Temperature Sensor (deg C)
    HEB_B_HSTemp,   //!< Blue HEB Heat Sink Temperature Sensor (deg C)
    HEB_B_DTAir,    //!< Blue HEB Air temperature difference (HS-Air, deg C)
    HEB_B_GSTemp,   //!< Blue HEB Glycol Supply Temperature Sensor (deg C)
    HEB_B_GRTemp,   //!< Blue HEB Glycol Return Temperature Sensor (deg C)
    HEB_B_DTGlycol, //!< Blue HEB Glycol temperature difference (Return-Supply, deg C)
    HEB_R_AirTemp,  //!< Red HEB Inside Air Temperature Sensor (deg C)
    HEB_R_HSTemp,   //!< Red HEB Heat Sink Temperature Sensor (deg C)
    HEB_R_DTAir,    //!< Blue HEB Air temperature difference (HS-Air, deg C)
    HEB_R_GSTemp,   //!< Red HEB Glycol Supply Temperature Sensor (deg C)
    HEB_R_GRTemp,   //!< Red HEB Glycol Return Temperature Sensor (deg C)
    HEB_R_DTGlycol, //!< Blue HEB Glycol temperature difference (Return-Supply, deg C)
    Blue_DewPres,   //!< Blue CCD Dewar Pressure Gauge (torr)
    Blue_CCDTemp,   //!< Blue CCD Detector Mount Temperature Sensor (deg C)
    Blue_DewTemp,   //!< Blue CCD Dewar Tank Temperature Sensor (deg C)
    Blue_TEDPower,  //!< Blue HEB ThermoElectricDevice Power (On/Off)
    Blue_Sequencer, //!< Blue CCD controller sequencer state (online/offline)
    Red_DewPres,    //!< Red CCD Dewar Pressure Gauge (torr)
    Red_CCDTemp,    //!< Red CCD Detector Mount Temperature Sensor (deg C)
    Red_DewTemp,    //!< Red CCD Dewar Tank Temperature Sensor (deg C)
    Red_TEDPower,   //!< Red HEB ThermoElectricDevice Power (On/Off)
    Red_Sequencer,  //!< Red CCD controller sequencer state (online/offline)
    IEB_B_ACPower,  //!< Blue IEB AC power state (On/Off/Fault)
    IEB_R_ACPower,  //!< Red IEB AC Power state (On/Off/Fault)
    HEB_B_ACPower,  //!< Blue HEB AC Power state (On/Off/Fault)
    HEB_R_ACPower,  //!< Red HEB AC Power state (On/Off/Fault)
    LLB_ACPower,    //!< Lamp/Laser Box AC Power state (On/Off/Fault)
    GCam_ACPower,   //!< AGw Guide Camera AC Power state (On/Off/Fault)
    WFS_ACPower     //!< AGw WFS Camera AC Power state (On/Off/Fault)
  };

#define NUMSENSORS 48 //!< Number of sensors and indicators


public slots:
  void parse(const QString &, const QString &, const int &, const QHash<QString,QString> &); //!< parse command replies
  void update();    //!< Update environmental sensor data

signals:
  void cmdReady(const QString &, const QString &);  //!< command ready to send to ISIS
  void cmdWait(const QString &, const QString &, const int &); //!< command with wait ready
  void statusMsg(const QString &, const QColor &);  //!< Emit a status message
  void alert(const QString &); //!< Emit an alert message
  void cancelAlerts();         //!< Cancel alerts ("all clear")
  void alertMsg(const QString &, const QColor &);  //!< Emit an alert message

private slots:
  void sendCmdWait(const QString &, const QString &, const int &); //!< send a command and wait or timeout

private:
  int modsID;          //!< MODS instrument ID (MODS1 or MODS2)
  QString panelName;   //!< Name of the environmental sensor monitor panel

  // Environmental Data, Displays, and Alarm flags

  TextDisplay *sensorDisplay[NUMSENSORS]; //!< Sensor data display widgets
  double sensorData[NUMSENSORS];          //!< Sensor data 
  bool sensorAlarm[NUMSENSORS];           //!< Sensor alarm flags
  int numAlarmRepeats[NUMSENSORS];        //!< Number times alarms have repeated in sequence

  ValueDisplay *updateDisplay; //!< Update time (UTC)
  ActionButton *updateButton;  //!< Button to update all environmental data

};

#endif
