#include <QtGui>

#include "linearcontrol.h"
#include "linearmechanism.h"
#include "numberentry.h"
#include "isisclient.h"

// LinearControl Widget Constructor

LinearControl::LinearControl(LinearMechanism *lMech, const QString &labelStr, 
			     const QString &unitsStr, int lMaxW, QWidget *parent) 
  : QWidget(parent)
{
  mech = lMech;
  int maxw = 0;
  if (lMaxW < 0) 
    maxw = 0;
  else
    maxw = lMaxW;

  // Main entry widget is a NumberEntry widget, but we do our own validation
  // of the input

  entry = new NumberEntry(labelStr,unitsStr,maxw,mech->min(),mech->max(),
			  mech->decimals(),this);

  // Linear Controls are always used in the "set and commit" mode that
  // requires the user to hit Return to "commit" the datum in the
  // entry widget.  Connect the NumberEntry dataCommit() signal to our
  // setCommand slot.

  connect(entry,SIGNAL(dataCommit(const double &)),this,SLOT(setCommand()));

  // Set the initial state to be unknown but enabled
  
  setState(LinearControl::Unknown);
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

void LinearControl::setState(const int &newState)
{
  switch (newState) {
  case LinearControl::Normal:
    entry->setNormal();
    setStatusTip("");
    state = newState;
    break;

  case LinearControl::Changing:
    entry->setChange();
    setStatusTip(tr("%1 State Changing - Waiting for completion...").arg(mech->command()));
    state = newState;
    break;

  case LinearControl::Fault:
    entry->setFault();
    setStatusTip(tr("%1 Fault Detected - See log for information.").arg(mech->command()));
    state = newState;
    break;

  case LinearControl::Unknown:
  default:
    entry->setNormal();
    setStatusTip(tr("%1 State Unknown - Make an entry to set.").arg(mech->command()));
    entry->clear();
    state = LinearControl::Unknown;
    break;
  }

}

// Is this control widget enabled?

bool LinearControl::isEnabled() const
{
  return enabled;
}

// Enable the control widget

void LinearControl::enable()
{
  enabled = true;
  entry->enable();
  setState(state);  // restore the state from before widget was disabled
}

// Disable the control widget

void LinearControl::disable()
{
  enabled = false;
  entry->disable();
}

// A value has been entered in the entry widget.  We validate it, and
// if OK, create the command string and emit the cmdReady() signal
// with the command string and the dataReady() signal with the
// requested datum value.  If it is invalid, we complain rudely
// with a QMessageBox pop-up.

void LinearControl::setCommand()
{
  double req = entry->value();
  if (req >= mech->min() && req <= mech->max()) {
    cmdStr = QString("%1 %2").arg(mech->command()).arg(QString::number(req,'f',mech->decimals()));
    setState(LinearControl::Changing);
    // Emit the signals with the command string and requested value datum
    emit cmdReady(cmdStr);
    emit dataReady(req);
  }
  else { // Invalid, indicate fault...
    setState(LinearControl::Fault);
  }
}

// Return the command string to execute

QString LinearControl::command() const
{
  return cmdStr;
}

// Return the value in the entry widget independent of the mechanism
// command context

double LinearControl::value() const
{
  return entry->value();
}

// Integer representation of the entry widget value

int LinearControl::intValue() const
{
  return (int)(entry->value());
}

// Update the entry widget contents - this is implemented as a public
// slot that would usually be connected to the posChanged() signal of
// the relevant LinearMechanism object.

void LinearControl::update()
{
  entry->setValue(mech->pos());
}

// Set the control widget state to Fault (convenience function)

void LinearControl::setFault()
{
  setState(LinearControl::Fault);
}

// Set the control widget state to Normal (convenience function)

void LinearControl::setNormal()
{
  setState(LinearControl::Normal);
}

// Set the control widget state to Changing (convenience function)

void LinearControl::setChange()
{
  setState(LinearControl::Changing);
}

// Set the control widget state to Unknown(convenience function)

void LinearControl::setUnknown()
{
  setState(LinearControl::Unknown);
}

// Set the control state based on the ISIS message type flag

void LinearControl::setISISState(const int &msgType)
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

