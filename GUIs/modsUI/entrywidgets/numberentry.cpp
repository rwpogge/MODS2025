//---------------------------------------------------------------------------
//
// NumberEntry - a labeled general-purpose number entry widget
//
// Combines QLabel and QLineEdit with a numbers-only validator to
// produce a general-purpose entry widget for numbers.  Uses double
// precision internally but provides an intValue() function for cast
// to integer, otherwise value() returns a double.
//
// Designed to be more generic than a spinbox.
//
// See intentry and doubleentry for spinbox-type numerical entry, and
// TextEntry for generic text entry.
//
// R. Pogge, OSU Astronomy Dept.
// pogge@astronomy.ohio-state.edu
// 2008 March 1
//
// Modification History
//   2009 Feb 20 - added distinction between data "ready" and data "committed"
//                 with appropriate signals.  See comments in the constructor
//                 for details.
//
//---------------------------------------------------------------------------

#include <QtWidgets>

#include "numberentry.h"

NumberEntry::NumberEntry(const QString &labelStr, const QString &unitsStr, 
			 int maxw, double minVal, double maxVal,
			 int decimals, QWidget *parent)
  : QWidget(parent)
{

  // Widgets we use: 2 QLabels (label and units), 1 QLineEdit

  label = new QLabel(labelStr);
  labelText = labelStr;

  entry = new QLineEdit(this);
  entry->setAlignment(Qt::AlignVCenter|Qt::AlignRight);

  units = new QLabel(unitsStr);
  unitsText = unitsStr;

  // Color palettes for the widget components

  entryPalette = new QPalette;
  *entryPalette = entry->palette();

  labelPalette = new QPalette;
  *labelPalette = label->palette();

  unitsPalette = new QPalette;
  *unitsPalette = units->palette();

  // Minimalist validator - ensures only that it is a number...

  entry->setValidator(new QDoubleValidator(entry));

  minimum = minVal;
  maximum = maxVal;
  dpmax = decimals;  // placeholder for future use...
  if (minimum==maximum)
    validate = false;
  else
    validate = true;

  //
  // Two ways to use this number entry widget
  //
  // Mode 1: Data Ready on editing finished.  Connect the QLineEdit's
  //         editingFinished() signal to our getData() slot to read the
  //         entry value and emit the dataReady() and intReady() signals with
  //         the entry datum as soon as return pressed or loss of focus.
  //         This makes the datum available for a deferred action (e.g.,
  //         command requiring 2 or more entries then hitting a "GO" button
  //         to execute, so xxxReady() signals are used to store the datum
  //         in the connecting object, but not perform the action).
  //
  // Mode 2: Data "Commit" on hitting Return.  Connect the QLineEdit's 
  //         returnPressed() signal to our getCommit() slot to read the entry 
  //         datum and emit the dataCommit() and intCommit() signals with that
  //         datum.  This mode is used when we want datum commit to 
  //         trigger an immediate action by whoever connects to the xxxCommit()
  //         signals.
  //

  // Mode 1: Data Ready

  connect(entry,SIGNAL(editingFinished()),this,SLOT(getData()));

  // Mode 2: Data Commit

  connect(entry,SIGNAL(returnPressed()),this,SLOT(getCommit()));
 
  // In either mode, provide a visual cue that the entry contents are
  // changing but not yet ready/committed by recoloring the background
  // of the entry widget to pale yellow.

  connect(entry,SIGNAL(textEdited(const QString &)),this,SLOT(entryChanged()));

  // If a maximum entry length is set, set an appropriate width for
  // the QLineEdit widget, otherwise let it autosize

  if (maxw > 0) {
    entry->setMaxLength(maxw);
    int mw = (int)(0.5*maxw*entry->minimumSizeHint().width());
    entry->setMaximumWidth(mw);
  }

  // This height looks good

  int hMax = (int)(0.8*entry->minimumSizeHint().height());
  entry->setMaximumHeight(hMax);

  // Layout of the elements of this widget: Label Editor Units

  QHBoxLayout *entryLayout = new QHBoxLayout;
  if (!labelText.isEmpty())
    entryLayout->addWidget(label);
  entryLayout->addWidget(entry);
  if (!unitsText.isEmpty())
    entryLayout->addWidget(units);
  if (maxw > 0)
    entryLayout->addStretch();  // left justify contents if not autosized
  entryLayout->setContentsMargins(0,0,0,0);  // removes extra margins
  setLayout(entryLayout);

  // Set the focus policy to click only (conservative, avoids accidental
  // tabbing or giving keyboard focus when not awares).

  entry->setFocusPolicy(Qt::ClickFocus);

  // Default colors for the widget states

  setNormalColors(Qt::white,Qt::black);
  setChangeColors(Qt::yellow,Qt::black);
  setFaultColors(Qt::red,Qt::black);

  // Assume we start out in the normal, enabled state

  enable();

}

// Set the contents of the QLineEdit widget

void NumberEntry::setValue(const double &val)
{
  entry->setText(QString::number(val));
  entryValue = val;
}

// Set the label text

void NumberEntry::setLabelText(const QString &txt)
{
  label->setText(txt);
  labelText = txt;
}

// Set the units text

void NumberEntry::setUnitsText(const QString &txt)
{
  units->setText(txt);
  unitsText = txt;
}

// Change font of all elements

void NumberEntry::setFont(const QFont &font)
{
  entry->setFont(font);
  label->setFont(font);
  units->setFont(font);
}

// getData() slot - emits the dataReady() and intReady() signals when
// the data are ready (entry widget editingFinished() signal).  We do
// our own validation if enabled.

void NumberEntry::getData()
{
  double val = entry->text().toDouble();
  if (validate) {
    if (val >= minimum && val <= maximum) {
      entryValue = val;
      emit dataReady(entryValue);
      emit intReady((int)(entryValue));
    }
    else {
      setOutOfRange();  // Invalid entry, set the fault condition
    }
  }
  else { // Validation disabled, we take anything that's a number...
    entryValue = val;
    emit dataReady(entryValue);
    emit intReady((int)(entryValue));
  }
}

// getCommit() slot - emits the dataCommit() and intCommit() signals
// when the numerical data entry has been "commited" by hitting return
// (entry widget returnPressed() signal).  We do our own validation if
// enabled.

void NumberEntry::getCommit()
{
  double val = entry->text().toDouble();
  if (validate) {
    if (val >= minimum && val <= maximum) {
      entryValue = val;
      emit dataCommit(entryValue);
      emit intCommit((int)(entryValue));
    }
    else {
      setOutOfRange();  // Invalid entry, set the fault condition
    }
  }
  else { // Validation disabled, we take anything that's a number...
    entryValue = val;
    emit dataCommit(entryValue);
    emit intCommit((int)(entryValue));
  }
}

// Return the numerical value in the entry box (double)

double NumberEntry::value() const
{
  return entryValue;
}

// Return the integer cast of the value in the entry box

int NumberEntry::intValue() const
{
  return (int)(entryValue);
}

// Return the text in the entry box, regardless of validity.  This is
// a useful convenience function.

QString NumberEntry::text() const
{
  return entry->text();
}

// Clear the entry box

void NumberEntry::clear()
{
  entry->clear();
  setNormal();
}

// Set the color of the entry background

void NumberEntry::setEntryColor(const QColor &color)
{  
  entryPalette->setColor(QPalette::Base,color);
  entry->setPalette(*entryPalette);
}

// Set the color of the entry value

void NumberEntry::setValueColor(const QColor &color)
{
  entryPalette->setColor(QPalette::Text,color);
  entry->setPalette(*entryPalette);
}

// Set the color of the widget label

void NumberEntry::setLabelColor(const QColor &color)
{  
  labelPalette->setColor(QPalette::WindowText,color);
  label->setPalette(*labelPalette);
}

// Set the color of the widget units

void NumberEntry::setUnitsColor(const QColor &color)
{  
  unitsPalette->setColor(QPalette::WindowText,color);
  units->setPalette(*unitsPalette);
}

// Change the entry color for the "normal" condition

void NumberEntry::setNormal()
{
  if (!entry->isEnabled()) return;
  entryPalette->setColor(QPalette::Base,bgNormal);
  entryPalette->setColor(QPalette::Text,fgNormal);
  entry->setPalette(*entryPalette);
}

// Change the entry color for the "changing" condition

void NumberEntry::setChange()
{
  if (!entry->isEnabled()) return;
  entryPalette->setColor(QPalette::Base,bgChange);
  entryPalette->setColor(QPalette::Text,fgChange);
  entry->setPalette(*entryPalette);
}

// Change the entry color for the "fault" condition

void NumberEntry::setFault()
{
  if (!entry->isEnabled()) return;
  entryPalette->setColor(QPalette::Base,bgFault);
  entryPalette->setColor(QPalette::Text,fgFault);
  entry->setPalette(*entryPalette);
}

// Set the colors for the widget states
//   Does not actually change any colors until one of setNormal() etc. are
//   invoked.  Also does not (cannot) validate colors

void NumberEntry::setNormalColors(const QColor &bg, const QColor &fg)
{
  bgNormal = bg;
  fgNormal = fg;
}

void NumberEntry::setChangeColors(const QColor &bg, const QColor &fg)
{
  bgChange = bg;
  fgChange = fg;
}

void NumberEntry::setFaultColors(const QColor &bg, const QColor &fg)
{
  bgFault = bg;
  fgFault = fg;
}

// Enable the widget - also sets the colors to "Normal" state

void NumberEntry::enable()
{
  entry->setEnabled(true);
  setNormal();
}

// Disable the widget

void NumberEntry::disable()
{
  entry->setEnabled(false);
  entryPalette->setColor(QPalette::Base,entryPalette->color(QPalette::Window));
  entryPalette->setColor(QPalette::Text,Qt::darkGray);
  entry->setPalette(*entryPalette);
}

// Set/unset an input entry mask

void NumberEntry::setInputMask(const QString &mask)
{
  entry->setInputMask(mask);
}

// If the contents of the entry have been changed, but not committed
// by hitting Return, turn the background a light yellow.  We check
// the validity of the entry continously if validation is enabled.

void NumberEntry::entryChanged() 
{
  if (validate) {
    double val = entry->text().toDouble();
    if (val >= minimum && val <= maximum) {
      setEditing();
      setStatusTip("");
    }
    else {
      setStatusTip(tr("Value out of Range: must be %1..%2").arg(minimum).arg(maximum));
      setOutOfRange(); // we are unhappy, indicate that.
    }
  }
  else {
    if (entry->text().isEmpty())
      setNormal();
    else 
      setEditing();
  }
}

// The "entry being edited" color is a pale yellow.

void NumberEntry::setEditing() 
{
  entryPalette->setColor(QPalette::Base,QColor(255,255,204,255));
  entryPalette->setColor(QPalette::Text,Qt::black);
  entry->setPalette(*entryPalette);
}

// The entry in the widget is out of its valid numerical range

void NumberEntry::setOutOfRange()
{
  entryPalette->setColor(QPalette::Base,QColor(255,0,0,127));
  entryPalette->setColor(QPalette::Text,Qt::white);
  entry->setPalette(*entryPalette);
}
