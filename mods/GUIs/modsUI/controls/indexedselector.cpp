#include <QtWidgets>

#include "indexedselector.h"
#include "indexedmechanism.h"
#include "isisclient.h"

// IndexedSelector control constructor

IndexedSelector::IndexedSelector(IndexedMechanism *iMech,
				 const QString &labelStr, 
				 QWidget *parent) 
  : QWidget(parent)
{
  mech = iMech;

  // Main selector is a QComboBox widget

  QLabel *label = new QLabel(labelStr);
  selector = new QComboBox(this);
  connect(selector,SIGNAL(activated(int)),this,SLOT(setCommand()));

  selectorPalette = new QPalette;
  *selectorPalette = selector->palette();

  // Using the indexed mechanism object, create the selector entries

  selector->clear();
  for (int i=0;i<mech->npos();i++) {
    int iPos = mech->min() + i;
    QString entry = mech->ID(iPos);
    QString cmd = QString("%1 %2").arg(mech->command()).arg(iPos);
    selector->addItem(entry,cmd);
  }

  // Connect the indexed mechanism idsChanged() signal to our
  // updateIDs() slot

  connect(mech,SIGNAL(idsChanged()),this,SLOT(updateIDs()));

  // Set the initial state to be unknown but enabled
  
  setState(IndexedSelector::Unknown);
  enabled = true;
  cmdStr = mech->command();  // safe default: command w/o arguments is a status query

  // This height looks good

  int hMax = (int)(0.8*selector->minimumSizeHint().height());
  selector->setMaximumHeight(hMax);

  // We use the QWindowsStyle because it makes the ComboBox look like
  // the other entry widgets: black text on white background...

  // selector->setStyle(new QWindowsStyle);

  // Set the focus policy to click only (conservative, avoids accidental
  // tabbing or giving keyboard focus when not awares).

  selector->setFocusPolicy(Qt::ClickFocus);

  // Layout of the elements of this widget: Label Selector Units

  QHBoxLayout *layout = new QHBoxLayout;
  if (!labelStr.isEmpty())
    layout->addWidget(label);
  layout->addWidget(selector);
  layout->addStretch();                 // left justify contents
  layout->setContentsMargins(0,0,0,0);  // removes extra margins
  setLayout(layout);

  // Connect the mechanism's posChanged() signal to the update() slot

  connect(mech,SIGNAL(posChanged()),this,SLOT(update()));
  curPos = selector->currentIndex();

}

// Set the current selector state.  A Fault or Unknown state shows the
// null position.

void IndexedSelector::setState(const int &stat)
{
  curPos = selector->currentIndex();
  switch (stat) {
  case IndexedSelector::Normal:
    setNormalColor();
    setStatusTip("");
    state = stat;
    break;

  case IndexedSelector::Changing:
    setChangeColor();
    setStatusTip(tr("%1 State Changing - Waiting for completion...").arg(mech->command()));
    state = stat;
    break;

  case IndexedSelector::Fault:
    setFaultColor();
    setStatusTip(tr("%1 Fault Detected - See logs for information.").arg(mech->command()));
    state = stat;
    break;

  case IndexedSelector::Unknown:
  default:
    setUnknownColor();
    setStatusTip(tr("%1 State Unknown - Make a selection to set or reset").arg(mech->command()));
    state = IndexedSelector::Unknown;
    break;
  }
}

// Is this control widget enabled?

bool IndexedSelector::isEnabled() const
{
  return enabled;
}

// Enable the control widget

void IndexedSelector::enable()
{
  enabled = true;
  selector->setEnabled(true);
  setState(state);  // restore the state
}

// Disable the control widget

void IndexedSelector::disable()
{
  enabled = false;
  selector->setEnabled(false);
}

// A selection has been made.  Create the command to execute, and then
// emit the cmdReady() signal with the command string.  

void IndexedSelector::setCommand()
{
  int req = selector->currentIndex();
  if (req != curPos) {
    cmdStr = selector->itemData(req).toString();
    setState(IndexedSelector::Changing);
    emit cmdReady(cmdStr);
  }
}

// Return the command string to execute

QString IndexedSelector::command() const
{
  return cmdStr;
}

// Update the selector - this is implemented as a public slot that
// would usually be connected to the posChanged() signal of the
// relevant IndexedMechanism object.

void IndexedSelector::update()
{
  int ipos = mech->pos() - mech->min();
  selector->setCurrentIndex(ipos);
  curPos = ipos;
}

// Set the selector to the normal state (convenience function)

void IndexedSelector::setNormal()
{
  setState(IndexedSelector::Normal);
}

// Set the selector to the changing state (convenience function)

void IndexedSelector::setChange()
{
  setState(IndexedSelector::Changing);
}

// Set the selector to the unknown state (convenience function)

void IndexedSelector::setUnknown()
{
  setState(IndexedSelector::Unknown);
}

// Set the selector to the fault state (convenience function)

void IndexedSelector::setFault()
{
  setState(IndexedSelector::Fault);
}

// Set the control state based on the ISIS message type flag

void IndexedSelector::setISISState(const int &msgType)
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

// Set the selector widget to the normal color

void IndexedSelector::setNormalColor()
{
  selectorPalette->setColor(QPalette::Text,Qt::black);
  selectorPalette->setColor(QPalette::Button,palette().color(QPalette::Window));
  selector->setPalette(*selectorPalette);
}

// Set the selector widget to the value-changing color
// This is chroma amber

void IndexedSelector::setChangeColor()
{
  selectorPalette->setColor(QPalette::Text,QColor(255,191,0,255));
  selectorPalette->setColor(QPalette::Button,Qt::darkYellow);
  selector->setPalette(*selectorPalette);
}

// Set the selector widget to the fault color

void IndexedSelector::setFaultColor()
{
  selectorPalette->setColor(QPalette::Text,Qt::red);
  selectorPalette->setColor(QPalette::Button,palette().color(QPalette::Window));
  selector->setPalette(*selectorPalette);
}

// Set the selector widget to the "unknown state" color

void IndexedSelector::setUnknownColor()
{
  selectorPalette->setColor(QPalette::Text,Qt::gray);
  selectorPalette->setColor(QPalette::Button,palette().color(QPalette::Window));
  selector->setPalette(*selectorPalette);
}

// Update the ID table - invoked when the ID table has been changed
// (either by method, or via the indexedmechanism's idsChanged()
// signal)

void IndexedSelector::updateIDs()
{
  selector->clear();
  for (int i=0;i<mech->npos();i++) {
    int iPos = mech->min() + i;
    QString entry = mech->ID(iPos);
    QString cmd = QString("%1 %2").arg(mech->command()).arg(iPos);
    selector->addItem(entry,cmd);
  }
}
