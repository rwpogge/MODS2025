#include <QtGui>

#include "powerswitch.h"
#include "powerbutton.h"

// PowerSwitch control constructor

PowerSwitch::PowerSwitch(const QString &myLabel, const QString &myCmd,
			 const int &mySize, const bool &hard, 
			 QWidget *parent) 
  : QWidget(parent)
{
  labelStr = myLabel;
  cmdStr = myCmd;

  // PowerButton widget is the active element

  button = new PowerButton(mySize,hard,this);
  connect(button,SIGNAL(clicked()),this,SLOT(setPower()));

  // If we have a label, it goes below

  QLabel *buttonLabel = new QLabel(labelStr);

  // Set the initial state to be unknown but enabled.
  
  setState(PowerSwitch::Unknown);
  enabled = true;
  fullCmdStr = cmdStr;  // safe default: command w/o arguments is a status query

  // Widget Layout

  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(button,0,Qt::AlignHCenter);
  if (!labelStr.isEmpty()) {
    layout->addWidget(buttonLabel,0,Qt::AlignHCenter);
    layout->addStretch();
    layout->setSpacing(0);
  }
  setLayout(layout);
  
  // Some other bits to make it look good

  layout->setContentsMargins(2,2,2,2);

}

// Set the power switch state
//
//   On:       Button lit green
//   Off:      Button black 
//   Sleep:    Button lit amber
//   Changing: Button lit yellow
//   Fault:    Button lit red
//   Unknown:  Button off
//  

void PowerSwitch::setState(const int &myState)
{

  switch (myState) {
  case PowerSwitch::Off:
    button->setOff();
    if (!labelStr.isEmpty())
      setStatusTip(tr("%1 power is Off").arg(labelStr));
    state = myState;
    break;

  case PowerSwitch::On:
    button->setOn();
    if (!labelStr.isEmpty())
      setStatusTip(tr("%1 power is ON").arg(labelStr));
    state = myState;
    break;

  case PowerSwitch::Sleep:
    button->setSleep();
    if (!labelStr.isEmpty())
      setStatusTip(tr("%1 in Sleep Mode - click to Wake").arg(labelStr));
    state = myState;
    break;

  case PowerSwitch::Changing:
    button->setChanging();
    if (!labelStr.isEmpty())
      setStatusTip(tr("%1 power state Changing - waiting for completion...").arg(labelStr));
    state = myState;
    break;

  case PowerSwitch::Fault:
    button->setFault(); 
    if (!labelStr.isEmpty())
      setStatusTip(tr("%1 power Fault Detected - See logs for information.").arg(labelStr));
    state = myState;
    break;

  case PowerSwitch::Unknown:
  default:
    button->setOff();
    if (!labelStr.isEmpty())
      setStatusTip(tr("%1 power state Unknown - Click button to query").arg(labelStr));
    state = PowerSwitch::Unknown;
    break;
  }

}

// Is this control widget enabled?

bool PowerSwitch::isEnabled() const
{
  return enabled;
}

// Enable the control widget

void PowerSwitch::enable()
{
  enabled = true;
  button->enable();
  setState(state);  // restore the state
}

// Disable the control widget

void PowerSwitch::disable()
{
  enabled = false;
  button->disable();
}

// The Power Switch has been pressed.  The action depends on its
// current state 
//
//   If Off      -> Send the "On" command
//   If On       -> Send the "Off" command
//   If Sleep  -> Send the "On" command
//   If Changing -> Do nothing (ignore switch press)
//   If Fault    -> Fault, attempt to turn off
//   If Unknown  -> Unknown, query state
//
// Once the command string is created, emit the cmdReady() signal and
// pass the cmdStr in it.

void PowerSwitch::setPower()
{
  switch (state) {
  case PowerSwitch::Off:
    fullCmdStr = QString("%1 ON").arg(cmdStr); // send the "ON" command
    setState(PowerSwitch::Changing);
    emit cmdReady(fullCmdStr);
    break;

  case PowerSwitch::Sleep:
    fullCmdStr = QString("%1 ON").arg(cmdStr); // send the "ON" command
    setState(PowerSwitch::Changing);
    emit cmdReady(fullCmdStr);
    break;

  case PowerSwitch::On:
    fullCmdStr = QString("%1 OFF").arg(cmdStr); // send the "OFF" command
    setState(PowerSwitch::Changing);
    emit cmdReady(fullCmdStr);
    break;

  case PowerSwitch::Changing:
    // Do nothing if we are changing
    break;

  case PowerSwitch::Fault:
    fullCmdStr = QString("%1 RESET").arg(cmdStr);  // attempt to RESET
    setState(PowerSwitch::Changing);
    emit cmdReady(fullCmdStr);
    break;

  case PowerSwitch::Unknown:
    fullCmdStr = cmdStr; // send a query (no arguments)
    setState(PowerSwitch::Changing);
    emit cmdReady(fullCmdStr);
    break;

  default:
    // Unknown state, so play it safe and do nothing
    break;

  }
}

// Return the current full command string

QString PowerSwitch::command() const
{
  return fullCmdStr;
}

// Set the power switch state ON (convenience function)

void PowerSwitch::setOn()
{
  setState(PowerSwitch::On);
}

// Set the power switch state OFF (convenience function)

void PowerSwitch::setOff()
{
  setState(PowerSwitch::Off);
}

// Set the power switch FAULT state (convenience function)

void PowerSwitch::setFault()
{
  setState(PowerSwitch::Fault);
}

// Set the power switch CHANGING state (convenience function)

void PowerSwitch::setChanging()
{
  setState(PowerSwitch::Changing);
}

// Set the power switch to SLEEP mode (convenience function)

void PowerSwitch::setSleep()
{
  setState(PowerSwitch::Sleep);
}

// Set the power switch UNKNOWN state (convenience function)

void PowerSwitch::setUnknown()
{
  setState(PowerSwitch::Unknown);
}

// Is the power on or off?

bool PowerSwitch::isOn()
{
  if (state == PowerSwitch::On)
    return true;
  else
    return false;
}

		      
