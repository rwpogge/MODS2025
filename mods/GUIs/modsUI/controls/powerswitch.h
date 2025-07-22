#ifndef POWERSWITCH_H
#define POWERSWITCH_H

// Qt Widgets

#include <QWidget>

class QString;

// Custom Widgets

class PowerButton;

// PowerSwitch control Class definition

class PowerSwitch : public QWidget
{
  Q_OBJECT

public:
  PowerSwitch(const QString &myLabel, const QString &myCmd,
	      const int &mySize=128, const bool &hard=true, 
	      QWidget *parent=0);

  // Basic Methods

  QString command() const;    //!< Command to execute (verb+argument)
  void setState(const int &); //!< Set the power switch state (see states enum)
  void setOn();               //!< Set the power switch On 
  void setOff();              //!< Set the power switch Off
  void setSleep();            //!< Set the power switch to Sleep
  void setChanging();         //!< Set the power switch to the Changing state
  void setFault();            //!< Set the power switch to FAULT
  void setUnknown();          //!< Set the power switch state to UNKNOWN
  bool isOn();                //!< Return true if on, false if off
  bool isSleep();             //!< Return true if in standby mode, false otherwise

  // Power Switch flags - graphics follow the IEEE 1621 standard

  enum states {
    On,       //!< PowerSwitch is ON (green with bright green highlights)
    Off,      //!< PowerSwitch is OFF (black with no highlights)
    Sleep,    //!< PowerSwitch is in SLEEP mode (amber with subtle amber highlights)
    Fault,    //!< PowerSwitch FAULT detected (red with bright red highlights)
    Changing, //!< PowerSwitch is CHANGING state (yellow with bright yellow highlights)
    Unknown   //!< PowerSwitch state is UNKNOWN (looks Off)
  };

  // Enable/disable the power switch widget

  bool isEnabled() const; //!< Is the power switch widget enabled?
  void enable();          //!< Enable the power switch widget
  void disable();         //!< Disable the power switch widget

signals:
  void cmdReady(const QString &); //!< Emit this signal when a command is ready for execution

private slots:
  void setPower();       //!< PowerButton has been presssed...

private:
  QString labelStr;       //!< Power Button label (appears below)
  QString cmdStr;         //!< Command verb string to bind to the power switch
  QString fullCmdStr;     //!< Full command string (verb+argument) to emit
  int state;              //!< State of the power switch (see states enum)
  PowerButton *button;    //!< Power Button
  bool enabled;           //!< Power switch enabled (true) or disabled (false)

};

#endif
