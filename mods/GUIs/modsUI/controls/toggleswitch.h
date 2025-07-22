#ifndef TOGGLESWITCH_H
#define TOGGLESWITCH_H

// Qt Widgets

#include <QWidget>

class QString;

// Custom Widgets

class LightButton;

// ToggleSwitch control Class definition

class ToggleSwitch : public QWidget
{
  Q_OBJECT

public:
  ToggleSwitch(const QString &labelStr, const QString &cmd,
	       const QString &onStr, const QString &offStr,
	       QWidget *parent=0);

  // Basic Methods

  QString command() const;       //!< Command to execute (verb+argument)
  void setState(const int &);    //!< Set the toggle switch state (see states enum)
  void setOn();                  //!< Set the toggle switch On state
  void setOff();                 //!< Set the toggle switch Off state
  void setFault();               //!< Set the toggle switch Fault state
  void setChange();              //!< Set the toggle switch Changing state
  void setUnknown();             //!< Set the toggle switch Unknown state
  void setText(const QString &); //!< Set the switch text

  // State flags

  enum states {
    On,       //!< ToggleSwitch is ON (LED green)
    Off,      //!< ToggleSwitch is OFF (LED off)
    Changing, //!< ToggleSwitch is changing state (LED yellow)
    Fault,    //!< A fault occurred while changing state (LED red)
    Unknown   //!< We don't know the toggle switch state (e.g., on startup)
  };

  // Enable/disable the toggle switch widget

  bool isEnabled() const; //!< Is the toggle switch widget enabled?
  void enable();          //!< Enable the toggle switch widget
  void disable();         //!< Disable the toggle switch widget

signals:
  void cmdReady(const QString &); //!< Emit this signal when a command is ready for execution
  void switchOn();    //!< emit this signal when the switch has been turned on
  void switchOff();   //!< emit this signal when the switch has been turned off

private slots:
  void setSwitch();       //!< Toggle Switch has been presssed...

private:
  QString cmdStr;         //!< Command verb string to bind to the toggle switch
  QString fullCmdStr;     //!< Full command string (verb+argument) to emit
  int state;              //!< State of the toggle switch (see states enum)
  LightButton *button;    //!< Toggle switch light button
  QString onArg;          //!< Command argument for "Switch ON"
  QString offArg;         //!< Command argument for "Switch OFF"

  bool enabled;           //!< Toggle switche enabled (true) or disabled (false)
};

#endif
