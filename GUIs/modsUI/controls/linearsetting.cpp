#include <QtWidgets>

#include "linearsetting.h"
#include "linearmechanism.h"
#include "numberentry.h"
#include "isisclient.h"

// LinearSetting Widget Constructor

LinearSetting::LinearSetting(LinearMechanism *lMech, const QString &labelStr, 
			     const QString &unitsStr, int lMaxW, QWidget *parent) 
  : QWidget(parent)
{
  mech = lMech;
  int maxw = 0;
  if (lMaxW < 0) 
    maxw = 0;
  else
    maxw = lMaxW;

  // Main entry widget is a NumberEntry widget. 

  entry = new NumberEntry(labelStr,unitsStr,maxw,mech->min(),mech->max(),
			  mech->decimals(),this);

  // Linear Settings are used in the "set value" mode that does not
  // require the user to hit Return to "commit" the datum.  Connect
  // the NumberEntry's dataReady() signal to our setEntry() slot.

  connect(entry,SIGNAL(dataReady(const double &)),this,SLOT(setEntry()));

  // Set the initial state to be unknown but enabled
  
  setState(LinearSetting::Unknown);
  enabled = true;
  cmdStr = mech->command();  // safe default: command w/o arguments is a status query

  // Connect the mechanism's posChanged() signal to the update() slot

  connect(mech,SIGNAL(posChanged()),this,SLOT(update()));
  
  // Widget Layout

  QHBoxLayout *layout = new QHBoxLayout;
  layout->addWidget(entry);
  layout->setContentsMargins(0,0,0,0);
  setLayout(layout);

}

// Set the widget state

void LinearSetting::setState(const int &newState)
{
  switch (newState) {
  case LinearSetting::Normal:
    entry->setNormal();
    setStatusTip("");
    state = newState;
    break;

  case LinearSetting::Changing:
    entry->setChange();
    setStatusTip(tr("%1 State Changing - Waiting for completion...").arg(mech->command()));
    state = newState;
    break;

  case LinearSetting::Fault:
    entry->setFault();
    setStatusTip(tr("%1 Fault Detected - See log for information.").arg(mech->command()));
    state = newState;
    break;

  case LinearSetting::Unknown:
  default:
    entry->setNormal();
    setStatusTip(tr("%1 State Unknown - Make an entry to set.").arg(mech->command()));
    entry->clear();
    state = LinearSetting::Unknown;
    break;
  }

}

// Is this setting widget enabled?

bool LinearSetting::isEnabled() const
{
  return enabled;
}

// Enable the setting widget

void LinearSetting::enable()
{
  enabled = true;
  entry->enable();
  setState(state);  // restore the state from before widget was disabled
}

// Disable the setting widget

void LinearSetting::disable()
{
  enabled = false;
  entry->disable();
}

// A value has been entered in the entry widget.  We do our own
// validation in place for the numerical range.  Create the command
// string and emit the cmdReady() signal with the command string and
// the dataReady() signal with the requested datum value.

void LinearSetting::setEntry()
{
  double req = entry->value();
  if (req >= mech->min() && req <= mech->max()) {
    cmdStr = QString("%1 %2").arg(mech->command()).arg(QString::number(req,'f',mech->decimals()));
    setState(LinearSetting::Changing);
    // Emit the signals with the command string and requested value datum
    emit cmdReady(cmdStr);
    emit dataReady(req);
  }
  else { // invalid input, disallow and indicate fault...
    setState(LinearSetting::Fault);
  }
}

// Return the command string to execute

QString LinearSetting::command() const
{
  return cmdStr;
}

// Return the value in the entry widget independent of the mechanism
// command context

double LinearSetting::value() const
{
  return entry->value();
}

// Integer representation of the entry widget value

int LinearSetting::intValue() const
{
  return (int)(entry->value());
}

// Update the entry widget contents - this is implemented as a public
// slot that would usually be connected to the posChanged() signal of
// the relevant LinearMechanism object.

void LinearSetting::update()
{
  entry->setValue(mech->pos());
}

// Set the setting widget state to Fault (convenience function)

void LinearSetting::setFault()
{
  setState(LinearSetting::Fault);
}

// Set the setting widget state to Normal (convenience function)

void LinearSetting::setNormal()
{
  setState(LinearSetting::Normal);
}

// Set the setting widget state to Changing (convenience function)

void LinearSetting::setChange()
{
  setState(LinearSetting::Changing);
}

// Set the setting widget state to Unknown(convenience function)

void LinearSetting::setUnknown()
{
  setState(LinearSetting::Unknown);
}

// Set the setting state based on the ISIS message type flag

void LinearSetting::setISISState(const int &msgType)
{
  switch (msgType) {
  case ISISClient::ERROR:
  case ISISClient::FATAL:
    setFault();
    break;

  case ISISClient::WARNING:
    // whatever we do on a warning class message
    break;

  default:
    // everything else is "normal"
    setNormal();
    break;
  }
}
