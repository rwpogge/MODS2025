//---------------------------------------------------------------------------
//
// IntEntry - a labeled spin-box widget for entering integer numbers
//            with a restricted range.
//
// Combines QLabel and QSpinBox to produce a number-entry widget
//
// For double-precision number entry, see DoubleEntry
//
// R. Pogge, OSU Astronomy Dept.
// pogge@astronomy.ohio-state.edu
// 2008 May 20
//
//---------------------------------------------------------------------------

#include <QtGui>

#include "intentry.h"

IntEntry::IntEntry(const QString &labelStr, const QString &unitsStr, 
		   int minVal, int maxVal, QWidget *parent)
  : QWidget(parent)
{
  entry = new QSpinBox(this);

  label = new QLabel(labelStr);
  labelText = labelStr;

  units = new QLabel(unitsStr);
  unitsText = unitsStr;

  // Color palettes for the widget components

  entryPalette = new QPalette;
  *entryPalette = entry->palette();

  labelPalette = new QPalette;
  *labelPalette = label->palette();

  unitsPalette = new QPalette;
  *unitsPalette = units->palette();

  // Establish the limits for the number entry

  entry->setMinimum(minVal);
  entry->setMaximum(maxVal);
  
  // Connect the QSpinBox's valueChanged() signal to our getData() slot

  connect(entry,SIGNAL(valueChanged(int)),this, SLOT(getData()));

  // This height looks good

  int hMax = (int)(0.8*entry->minimumSizeHint().height());
  entry->setMaximumHeight(hMax);

  // Layout of the elements of this widget: Label Entry Units

  QHBoxLayout *layout = new QHBoxLayout;
  if (!labelText.isEmpty())
    layout->addWidget(label);
  layout->addWidget(entry);
  if (!unitsText.isEmpty())
    layout->addWidget(units);
  layout->addStretch();                 // left justify contents
  layout->setContentsMargins(0,0,0,0);  // removes extra margins
  setLayout(layout);

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

// Set the value visible in the QSpinBox.  Note that this will cause
// valueChanged() to be emitted by the QSpinBox...

void IntEntry::setValue(const int &val)
{
  entry->setValue(val);
}

// Set the label text

void IntEntry::setLabelText(const QString &txt)
{
  label->setText(txt);
  labelText = txt;
}

// Set the units text

void IntEntry::setUnitsText(const QString &txt)
{
  units->setText(txt);
  unitsText = txt;
}

// Change font of all elements

void IntEntry::setFont(const QFont &font)
{
  entry->setFont(font);
  label->setFont(font);
  units->setFont(font);
}

// getData() slot - emits the dataReady() signal

void IntEntry::getData()
{
  entryValue = entry->value();
  emit dataReady(entryValue);
}

// Return the value in the entry box

int IntEntry::value() const
{
  return entryValue;
}

// Set the color of the SpinBox background

void IntEntry::setEntryColor(const QColor &color)
{  
  entryPalette->setColor(QPalette::Base,color);
  entry->setPalette(*entryPalette);
}

// Set the color of the spinbox value text 

void IntEntry::setValueColor(const QColor &color)
{
  entryPalette->setColor(QPalette::Text,color);
  entry->setPalette(*entryPalette);
}

// Set the color of the widget label

void IntEntry::setLabelColor(const QColor &color)
{  
  labelPalette->setColor(QPalette::WindowText,color);
  label->setPalette(*labelPalette);
}

// Set the color of the widget units

void IntEntry::setUnitsColor(const QColor &color)
{  
  unitsPalette->setColor(QPalette::WindowText,color);
  units->setPalette(*unitsPalette);
}

// Change the entry color for the "normal" condition

void IntEntry::setNormal()
{
  if (!entry->isEnabled()) return;
  entryPalette->setColor(QPalette::Base,bgNormal);
  entryPalette->setColor(QPalette::Text,fgNormal);
  entry->setPalette(*entryPalette);
}

// Change the entry color for the "changing" condition

void IntEntry::setChange()
{
  if (!entry->isEnabled()) return;
  entryPalette->setColor(QPalette::Base,bgChange);
  entryPalette->setColor(QPalette::Text,fgChange);
  entry->setPalette(*entryPalette);
}

// Change the entry color for the "fault" condition

void IntEntry::setFault()
{
  if (!entry->isEnabled()) return;
  entryPalette->setColor(QPalette::Base,bgFault);
  entryPalette->setColor(QPalette::Text,fgFault);
  entry->setPalette(*entryPalette);
}

// Set the colors for the widget states
//   Does not actually change any colors until one of setNormal() etc. are
//   invoked.  Also does not (cannot) validate colors

void IntEntry::setNormalColors(const QColor &bg, const QColor &fg)
{
  bgNormal = bg;
  fgNormal = fg;
}

void IntEntry::setChangeColors(const QColor &bg, const QColor &fg)
{
  bgChange = bg;
  fgChange = fg;
}

void IntEntry::setFaultColors(const QColor &bg, const QColor &fg)
{
  bgFault = bg;
  fgFault = fg;
}

// Enable the widget - also sets the colors to "Normal" state

void IntEntry::enable()
{
  entry->setEnabled(true);
  setNormal();
}

// Disable the widget

void IntEntry::disable()
{
  entry->setEnabled(false);
  entryPalette->setColor(QPalette::Base,entryPalette->color(QPalette::Window));
  entryPalette->setColor(QPalette::Text,Qt::darkGray);
  entry->setPalette(*entryPalette);
}
