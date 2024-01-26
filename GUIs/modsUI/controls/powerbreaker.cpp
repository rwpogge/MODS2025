#include <QtGui>

#include "powerbreaker.h"
#include "powerbutton.h"
#include "led.h"

// PowerBreaker control constructor

PowerBreaker::PowerBreaker(const QString &myLabel, const QString &myCmd,
			   const int &mySize, QWidget *parent) 
  : QWidget(parent)
{
  labelStr = myLabel;
  cmdStr = myCmd;
  fullCmdStr = cmdStr;

  // Power switch label

  QLabel *widgetLabel = new QLabel(labelStr);

  // Circuit Breaker indicator

  breakerLED = new LED(this);
  breakerLED->setOff();

  // Put the label and breaker indicator LED into a horizontal layout

  QHBoxLayout *labelLayout = new QHBoxLayout;
  labelLayout->addStretch();
  labelLayout->addWidget(widgetLabel);
  labelLayout->addSpacing(5);
  labelLayout->addWidget(breakerLED);
  labelLayout->addStretch();
  labelLayout->setContentsMargins(0,0,0,0);
  
  // PowerButton widget - this is always a hard off switch

  button = new PowerButton(mySize,true,this);
  connect(button,SIGNAL(clicked()),this,SLOT(setPower()));
  button->setOff();
  breaker = PowerBreaker::BreakerUnknown;
  enabled = true;

  // Widget Layout

  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(button,0,Qt::AlignHCenter);
  layout->addLayout(labelLayout);
  layout->addStretch();
  layout->setSpacing(0);

  layout->setContentsMargins(2,2,2,2);
  setLayout(layout);

}

// Set the power switch state
//
//   On:       Button lit green
//   Off:      Button black 
//   Sleep:    Button lit amber
//   Fault:    Button lit red
//   Changing: Button lit yellow
//   Unknown:  Button off
//  
// Note that the circuit breaker state is set separately in
// setBreaker()
//

void PowerBreaker::setState(const int &myState)
{

  switch (myState) {
  case PowerBreaker::PowerOff:
    button->setOff();
    if (!labelStr.isEmpty())
      setStatusTip(tr("%1 power is Off").arg(labelStr));
    power = myState;
    break;

  case PowerBreaker::PowerOn:
    button->setOn();
    if (!labelStr.isEmpty())
      setStatusTip(tr("%1 power is On").arg(labelStr));
    power = myState;
    break;

  case PowerBreaker::SleepPower:
    button->setSleep();
    if (!labelStr.isEmpty())
      setStatusTip(tr("%1 in Sleep Mode - click to Wake").arg(labelStr));
    power = myState;
    break;

  case PowerBreaker::PowerChanging:
    button->setChanging();
    if (!labelStr.isEmpty())
      setStatusTip(tr("%1 power state Changing - waiting for completion...").arg(labelStr));
    power = myState;
    break;

  case PowerBreaker::PowerFault:
    button->setFault(); 
    if (!labelStr.isEmpty())
      setStatusTip(tr("%1 power Fault Detected - See logs for information.").arg(labelStr));
    power = myState;
    break;

  case PowerBreaker::PowerUnknown:
  default:
    button->setOff();
    if (!labelStr.isEmpty())
      setStatusTip(tr("%1 Power State Unknown - Click button to query").arg(labelStr));
    power = PowerBreaker::PowerUnknown;
    break;
  }

}

// Is this control widget enabled?

bool PowerBreaker::isEnabled() const
{
  return enabled;
}

// Enable the control widget

void PowerBreaker::enable()
{
  enabled = true;
  button->enable();
  setState(power);  // restore the state
}

// Disable the control widget

void PowerBreaker::disable()
{
  enabled = false;
  button->disable();
}

// The Power Switch has been pressed.  The action depends on its
// current state 
//
//   If Off      -> Send the "On" command
//   If On       -> Send the "Off" command
//   If Sleep    -> Send the "On" command to wake
//   If Changing -> Do nothing (ignore switch press)
//   If Fault    -> Fault, attempt to turn off
//   If Unknown  -> Unknown, query state
//
// Once the command string is created, emit the cmdReady() signal and
// pass the cmdStr in it.

void PowerBreaker::setPower()
{
  switch (power) {
  case PowerBreaker::PowerOff:
    fullCmdStr = QString("%1 ON").arg(cmdStr); // send the "ON" command
    setState(PowerBreaker::PowerChanging);
    emit cmdReady(fullCmdStr);
    break;

  case PowerBreaker::SleepPower:
    fullCmdStr = QString("%1 ON").arg(cmdStr); // send the "ON" command
    setState(PowerBreaker::PowerChanging);
    emit cmdReady(fullCmdStr);
    break;

  case PowerBreaker::PowerOn:
    fullCmdStr = QString("%1 OFF").arg(cmdStr); // send the "OFF" command
    setState(PowerBreaker::PowerChanging);
    emit cmdReady(fullCmdStr);
    break;

  case PowerBreaker::PowerChanging:
    // Do nothing if we are changing
    break;

  case PowerBreaker::PowerFault:
    fullCmdStr = QString("%1 RESET").arg(cmdStr);  // attempt to RESET
    setState(PowerBreaker::PowerChanging);
    emit cmdReady(fullCmdStr);
    break;

  case PowerBreaker::PowerUnknown:
    fullCmdStr = cmdStr; // send a query (no arguments)
    setState(PowerBreaker::PowerChanging);
    emit cmdReady(fullCmdStr);
    break;

  default:
    // Unknown state, so play it safe and do nothing
    break;

  }
}

// Return the current full command string

QString PowerBreaker::command() const
{
  return fullCmdStr;
}

// Set the power switch state ON (convenience function)

void PowerBreaker::setOn()
{
  setState(PowerBreaker::PowerOn);
}

// Set the power switch state OFF (convenience function)

void PowerBreaker::setOff()
{
  setState(PowerBreaker::PowerOff);
}

// Set the power switch FAULT state (convenience function)

void PowerBreaker::setFault()
{
  setState(PowerBreaker::PowerFault);
}

// Set the power switch CHANGING state (convenience function)

void PowerBreaker::setChanging()
{
  setState(PowerBreaker::PowerChanging);
}

// Set the power switch to SLEEP mode (convenience function)

void PowerBreaker::setSleep()
{
  setState(PowerBreaker::SleepPower);
}

// Set the power switch UNKNOWN state (convenience function)

void PowerBreaker::setUnknown()
{
  setState(PowerBreaker::PowerUnknown);
}

// Is the power on or off?

bool PowerBreaker::isOn()
{
  if (power == PowerBreaker::PowerOn)
    return true;
  else
    return false;
}

// Set the circuit breaker state to OK

void PowerBreaker::setBreakerOK()
{
  breaker=PowerBreaker::BreakerOK;
  breakerLED->setGreen();
}

// Set the circuit breaker state to FAULT

void PowerBreaker::setBreakerFault()
{
  breaker=PowerBreaker::BreakerFault;
  breakerLED->setRed();
}

// Set the circuit breaker state to UNKNOWN

void PowerBreaker::setBreakerUnknown()
{
  breaker=PowerBreaker::BreakerUnknown;
  breakerLED->setOff();
}

// Report the breaker state

bool PowerBreaker::breakerOK() const
{
  switch (breaker) {
  case PowerBreaker::BreakerOK:
    return true;
    break;

  case PowerBreaker::BreakerFault:
    return false;
    break;

  case PowerBreaker::BreakerUnknown:   // technically correct...
  default:
    return true;
    break;
  }
}

// Reset the power and breaker state (convenience function)

void PowerBreaker::reset()
{
  setUnknown();        // power switch state
  setBreakerUnknown(); // circuit breaker state
}
