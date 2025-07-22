#ifndef POWERBREAKER_H
#define POWERBREAKER_H

// Qt Widgets

#include <QWidget>

class QString;

// Custom Widgets

class PowerButton;
class LED;

// PowerBreaker control Class definition 

class PowerBreaker : public QWidget
{
  Q_OBJECT

public:
  PowerBreaker(const QString &myLabel, const QString &myCmd,
	       const int &mySize=128, QWidget *parent=0);

  // Public Methods for this subclass

  QString command() const;    //!< Command to execute (verb+argument)
  void setState(const int &); //!< Set the power switch state (see states enum)
  void setOn();               //!< Set the power switch ON 
  void setOff();              //!< Set the power switch OFF
  void setSleep();            //!< Set the power switch to SLEEP
  void setFault();            //!< Set the power switch to FAULT
  void setChanging();         //!< Set the power switch to the CHANGING state
  void setUnknown();          //!< Set the power switch state to UNKNOWN
  bool isOn();                //!< Return true if on, false if off
  bool isSleep();             //!< Return true if in standby mode, false otherwise

  void setBreakerOK();        //!< Set the circuit breaker state to OK
  void setBreakerFault();     //!< Set the circuit breaker state to Fault
  void setBreakerUnknown();   //!< Set the circuit breaker state to Unknown
  bool breakerOK() const;     //!< Is the power breaker state OK?

  void reset();               //!< Global reset of power switch and breaker state

  // Power Switch States

  enum powerState {
    PowerOn,       //!< PowerSwitch is ON (green with bright green highlights)
    PowerOff,      //!< PowerSwitch is OFF (black with no highlights)
    SleepPower,    //!< PowerSwitch is in SLEEP mode (amber with subtle amber highlights)
    PowerFault,    //!< PowerSwitch FAULT detected (red with bright red highlights)
    PowerChanging, //!< PowerSwitch is CHANGING state (yellow with bright yellow highlights)
    PowerUnknown   //!< PowerSwitch state is UNKNOWN (looks Off)
  };

  // Circuit Breaker States

  enum breakerState {
    BreakerOK,     //!< Circuit Breaker OK - voltage detected on the output side 
    BreakerFault,  //!< Circuit Breaker FAULT - no voltage detected on the output side
    BreakerUnknown //!< Circuit Breaker state UNKNOWN (no voltage on the input side)
  };

  // Enable/Disable the power breaker widget

  bool isEnabled() const; //!< Is the power switch widget enabled?
  void enable();          //!< Enable the power switch widget
  void disable();         //!< Disable the power switch widget

signals:
  void cmdReady(const QString &); //!< Command is ready for execution

private slots:
  void setPower();       //!< PowerButton has been presssed...

private:
  QString labelStr;    //!< Power Button label (appears below)
  QString cmdStr;      //!< Command verb string to bind to the power switch
  QString fullCmdStr;  //!< Full command string (verb+argument) to emit
  int power;           //!< State of the power switch (see states enum)
  PowerButton *button; //!< Power Button
  LED *breakerLED;     //!< Circuit breaker state indicator LED
  int breaker;         //!< Current circuit breaker state (see Breaker enum)

  bool enabled;        //!< Widget is enabled (true) or disabled (false)

};

#endif
