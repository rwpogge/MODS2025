#include <QtGui>

#include "toggleswitch.h"
#include "lightbutton.h"

// ToggleSwitch control constructor

ToggleSwitch::ToggleSwitch(const QString &labelStr, const QString &cmd,
			   const QString &onStr, const QString &offStr,
			   QWidget *parent) 
  : QWidget(parent)
{
  cmdStr = cmd;
  onArg = onStr;
  offArg = offStr;

  // Widget is a single LED LightButton widget

  button = new LightButton(labelStr,this);
  connect(button,SIGNAL(clicked()),this,SLOT(setSwitch()));
  
  // Set the initial state to be unknown but enabled.
  
  setState(ToggleSwitch::Unknown);
  enabled = true;
  fullCmdStr = cmdStr;  // safe default: command w/o arguments is a status query

  // Widget Layout

  QHBoxLayout *layout = new QHBoxLayout;
  layout->addWidget(button);
  layout->setContentsMargins(1,1,1,1);
  setLayout(layout);

  // Some other bits to make it look good


}

// Set the toggle switch state
//
//   On:       LED lit green
//   Off:      LED off
//   Changing: LED lit yellow
//   Fault:    LED lit red
//   Unknown:  LED off
//  

void ToggleSwitch::setState(const int &stat)
{

  switch (stat) {
  case ToggleSwitch::Off:
    button->setOff();
    button->setTextColor(Qt::black);
    setStatusTip(tr("%1 is %2").arg(cmdStr,offArg));
    state = stat;
    break;

  case ToggleSwitch::On:
    button->setGreen();
    button->setTextColor(Qt::black);
    setStatusTip(tr("%1 is %2").arg(cmdStr,onArg));
    state = stat;
    break;

  case ToggleSwitch::Changing:
    button->setYellow();
    button->setTextColor(Qt::black);
    setStatusTip(tr("%1 is Changing - Waiting for completion...").arg(cmdStr));
    state = stat;
    break;

  case ToggleSwitch::Fault:
    button->setRed(); 
    button->setTextColor(Qt::red);
    setStatusTip(tr("%1 Fault Detected - See logs for information.").arg(cmdStr));
    state = stat;
    break;

  case ToggleSwitch::Unknown:
  default:
    button->setOff();
    button->setTextColor(Qt::gray);
    setStatusTip(tr("%1 State Unknown - Click button to attempt to reset (%2).").arg(cmdStr,offArg));
    state = ToggleSwitch::Unknown;
    break;
  }

}

// Is this control widget enabled?

bool ToggleSwitch::isEnabled() const
{
  return enabled;
}

// Enable the control widget

void ToggleSwitch::enable()
{
  enabled = true;
  button->enable();
  setState(state);  // restore the state
}

// Disable the control widget

void ToggleSwitch::disable()
{
  enabled = false;
  button->disable();
}

// The Toggle Switch has been pressed.  The action depends on its
// current state 
//
//   If Off      -> Send the "On" command string
//   If On       -> Send the "Off" command string
//   If Changing -> Do nothing (ignore switch press)
//   If Fault    -> RESET, which usually means turn off
//   If Unknown  -> Try to turn it On 
//
// Once the command string is created, emit the cmdReady() signal and
// pass the cmdStr in it.

void ToggleSwitch::setSwitch()
{
  switch (state) {
  case ToggleSwitch::Off:
    fullCmdStr = QString("%1 %2").arg(cmdStr,onArg); // send the "ON" command
    setState(ToggleSwitch::Changing);
    emit switchOn();
    emit cmdReady(fullCmdStr);
    break;

  case ToggleSwitch::On:
    fullCmdStr = QString("%1 %2").arg(cmdStr,offArg); // send the "OFF" command
    setState(ToggleSwitch::Changing);
    emit switchOff();
    emit cmdReady(fullCmdStr);
    break;

  case ToggleSwitch::Changing:
    // Do nothing if we are changing
    break;

  case ToggleSwitch::Fault:
    fullCmdStr = QString("%1 RESET").arg(cmdStr);  // attempt to RESET
    setState(ToggleSwitch::Changing);
    emit cmdReady(fullCmdStr);
    break;

  case ToggleSwitch::Unknown:
    fullCmdStr = QString("%1 %2").arg(cmdStr,onArg); // try to turn it ON...
    setState(ToggleSwitch::Changing);
    emit switchOn();
    emit cmdReady(fullCmdStr);
    break;

  default:
    // Unknown state, so play it safe and do nothing
    break;

  }
}

// Return the current full command string

QString ToggleSwitch::command() const
{
  return fullCmdStr;
}

// Set the toggle switch state ON (convenience function)

void ToggleSwitch::setOn()
{
  setState(ToggleSwitch::On);
}

// Set the toggle switch state OFF (convenience function)

void ToggleSwitch::setOff()
{
  setState(ToggleSwitch::Off);
}

// Set the toggle switch FAULT state (convenience function)

void ToggleSwitch::setFault()
{
  setState(ToggleSwitch::Fault);
}

// Set the toggle switch CHANGING state (convenience function)

void ToggleSwitch::setChange()
{
  setState(ToggleSwitch::Changing);
}

// Set the toggle switch UNKNOWN state (convenience function)

void ToggleSwitch::setUnknown()
{
  setState(ToggleSwitch::Unknown);
}

// Set the toggle switch text

void ToggleSwitch::setText(const QString &newText)
{
  button->setText(newText);
}
