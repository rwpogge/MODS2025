#include <QtGui>

#include "bistatecontrol.h"
#include "bistatemechanism.h"
#include "actionbutton.h"
#include "isisclient.h"

// BiStateControl Widget Constructor

BiStateControl::BiStateControl(BiStateMechanism *bMech, const QString &labelStr,
			       const QString &lab0, const QString &lab1, 
			       QWidget *parent) 
  : QWidget(parent)
{
  mech = bMech;

  QHBoxLayout *layout = new QHBoxLayout;
  
  if (!labelStr.isEmpty()) {
    label = new QLabel(labelStr);
    layout->addWidget(label);
  }

  button0 = new ActionButton(lab0,this);
  connect(button0,SIGNAL(clicked()),this,SLOT(setState0()));
  layout->addWidget(button0);

  button1 = new ActionButton(lab1,this);
  connect(button1,SIGNAL(clicked()),this,SLOT(setState1()));
  layout->addWidget(button1);

  // Make sure the buttons are the same width

  int bw = qMax(button0->minimumSizeHint().width(),button1->minimumSizeHint().width());
  button0->setMinimumWidth(bw);
  button1->setMinimumWidth(bw);

  // Finish the layout

  layout->addStretch();
  layout->setContentsMargins(0,0,0,0);  
  setLayout(layout);

  // Set the initial state to be unknown but enabled.
  
  setState(BiStateControl::Unknown,BiStateControl::Unknown);
  enabled = true;
  cmdStr = mech->command();  // safe default: command w/o arguments is a status query

  // Connect the mechanism's stateChanged() signal to the update() slot

  connect(mech,SIGNAL(stateChanged()),this,SLOT(update()));
  
}

// Set the control button state
//
//   Selected:   Button down, green text on black background
//   Deselected: Button up, black text on light gray background
//   Changing:   Button down, black text on yellow background
//   Fault:      Button up, black text on red background
//   Unknown:    Button up, gray on light gray background
//  

void BiStateControl::setState(const int &stat0, const int &stat1)
{

  // Set the Button 0 status

  switch (stat0) {
  case BiStateControl::Deselected:
    button0->setNormal();
    button0->setTextColor(Qt::black);
    button0->setDown(false);
    status0 = stat0;
    break;

  case BiStateControl::Selected:
    button0->setNormal();
    button0->setTextColor(Qt::darkGreen);
    // button0->setColor(Qt::black);
    button0->setDown(true);
    setStatusTip(tr("%1 is %2").arg(mech->command(),mech->ID(0)));
    status0 = stat0;
    break;

  case BiStateControl::Changing:
    button0->setChange();
    button0->setDown(true);
    setStatusTip(tr("%1 State Changing - Waiting for completion...").arg(mech->command()));
    status0 = stat0;
    break;

  case BiStateControl::Fault:
    button0->setNormal();
    button0->setTextColor(Qt::red);
    button0->setDown(false);
    setStatusTip(tr("%1 Fault - Click any red button to reset mechanism.").arg(mech->command()));
    status0 = stat0;
    break;

  case BiStateControl::Unknown:
  default:
    button0->setNormal();
    button0->setDown(false);
    button0->setTextColor(Qt::gray);
    setStatusTip(tr("%1 State Unknown - Click either grayed button to activate.").arg(mech->command()));
    status0 = BiStateControl::Unknown;
    break;
  }

  // Set the Button 1 status

  switch (stat1) {
  case BiStateControl::Deselected:
    button1->setNormal();
    button1->setTextColor(Qt::black);
    button1->setDown(false);
    status1 = stat1;
    break;

  case BiStateControl::Selected:
    button1->setNormal();
    button1->setTextColor(Qt::darkGreen);
    //button1->setColor(Qt::black);
    button1->setDown(true);
    setStatusTip(tr("%1 is %2").arg(mech->command(),mech->ID(1)));
    status1 = stat1;
    break;

  case BiStateControl::Changing:
    button1->setChange();
    button1->setDown(true);
    setStatusTip(tr("%1 State Changing - Waiting for completion...").arg(mech->command()));
    status1 = stat1;
    break;

  case BiStateControl::Fault:
    button1->setNormal();
    button1->setTextColor(Qt::red);
    button1->setDown(false);
    setStatusTip(tr("%1 Fault - Click any red button to reset mechanism.").arg(mech->command()));
    status1 = stat1;
    break;

  case BiStateControl::Unknown:
  default:
    button1->setNormal();
    button1->setDown(false);
    button1->setTextColor(Qt::gray);
    setStatusTip(tr("%1 State Unknown - Click either grayed button to activate.").arg(mech->command()));
    status1 = BiStateControl::Unknown;
    break;
  }

}

// Is this control widget enabled?

bool BiStateControl::isEnabled() const
{
  return enabled;
}

// Enable the control widget

void BiStateControl::enable()
{
  enabled = true;
  button0->enable();
  button1->enable();
  setState(status0,status1);
}

// Disable the control widget

void BiStateControl::disable()
{
  enabled = false;
  button0->disable();
  button1->disable();
}

// Button 0 has been pressed.  What we do depends on the state of this
// and Button1 so we don't inadvertently send conflicting commands.
//   If this button is deselected, try to set State0, but only
//      if State1 is not changing (no conflicts)
//   If we're already in State0, do nothing
//   If we're changing to State0, do nothing (future: ABORT?)
//   If State0 shows a fault condition, attempt a reset(??)
//   If State0 is unknown, perform the requested action if clicked
//   If we don't know what to do, play it safe and do nothing

void BiStateControl::setState0()
{
  switch (status0) {
  case BiStateControl::Deselected:  // emit a command to set State0
  case BiStateControl::Unknown:
    if (status1 != BiStateControl::Changing) { // but only if State1 is not changing
      cmdStr = QString("%1 %2").arg(mech->command(),mech->ID(0));
      setState(BiStateControl::Changing,BiStateControl::Deselected);
      emit cmdReady(cmdStr);
    }
    break;

  case BiStateControl::Selected:  // State0 is already selected, do nothing
    break;

  case BiStateControl::Changing:  // State0 is changing, do nothing (future: ABORT?)
    break;

  case BiStateControl::Fault:   // Fault, attempt a device reset
    cmdStr = QString("%1 RESET").arg(mech->command());
    setState(BiStateControl::Changing,BiStateControl::Changing);
    emit cmdReady(cmdStr);
    break;

  default:  // play it safe and do nothing if the status makes no sense
    break;
  }

}

// Button 1 has been pressed.  What we do depends on the state of this
// and Button0 so we don't inadvertently send conflicting commands.
//   If this button is deselected, try to set State1, but only
//      if State0 is not changing (no conflicts)
//   If we're already in State1, do nothing
//   If we're changing to State1, do nothing (future: ABORT?)
//   If Button1 shows a fault condition, attempt a reset(??)
//   If State1 is unknown, query for the current status
//   If we don't know what to do, play it safe and do nothing

void BiStateControl::setState1()
{
  switch (status1) {
  case BiStateControl::Deselected:  // emit a command to set State1
  case BiStateControl::Unknown:
    if (status0 != BiStateControl::Changing) { // but only if State0 is not changing
      cmdStr = QString("%1 %2").arg(mech->command(),mech->ID(1));
      setState(BiStateControl::Changing,BiStateControl::Deselected);
      emit cmdReady(cmdStr);
    }
    break;

  case BiStateControl::Selected:  // State1 is already selected, do nothing
    break;

  case BiStateControl::Changing:  // State1 is changing, do nothing (future: ABORT?)
    break;

  case BiStateControl::Fault:   // Fault, attempt a device reset
    cmdStr = QString("%1 RESET").arg(mech->command());
    setState(BiStateControl::Changing,BiStateControl::Changing);
    emit cmdReady(cmdStr);
    break;

  default:  // play it safe and do nothing if the status makes no sense
    break;
  }

}

// Return the current full command string

QString BiStateControl::command() const
{
  return cmdStr;
}

// Update the control widget - this is implemented as a public slot
// that would usually be connected to the stateChanged() signal of the
// relevant BiStateMechanism object

void BiStateControl::update()
{
  int istate = mech->state();

  if (istate == 0)
    setState(BiStateControl::Selected,BiStateControl::Deselected);
  else if (istate == 1)
    setState(BiStateControl::Deselected,BiStateControl::Selected);
  else
    setState(BiStateControl::Unknown,BiStateControl::Unknown);
  
}

// A fault has occurred, set the fault state.  This is a convenience
// function that puts both buttons in the Fault state

void BiStateControl::setFault()
{
  setState(BiStateControl::Fault,BiStateControl::Fault);
}

// Set the bistate control appearce to "changing"

void BiStateControl::setChange()
{
  setState(BiStateControl::Changing,BiStateControl::Changing);
}

// Set to the normal state for the given state

void BiStateControl::setNormal()
{
  int istate = mech->state();

  if (istate == 0)
    setState(BiStateControl::Selected,BiStateControl::Deselected);
  else if (istate == 1)
    setState(BiStateControl::Deselected,BiStateControl::Selected);
  else
    setState(BiStateControl::Unknown,BiStateControl::Unknown);
  
}

// Set the control state to Unknown (convenience function).  This returns
// the control to the state set in the constructor.

void BiStateControl::setUnknown()
{
  setState(BiStateControl::Unknown,BiStateControl::Unknown);
}

// Set the control state based on the ISIS message type flag

void BiStateControl::setISISState(const int &msgType)
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
