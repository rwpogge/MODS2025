//---------------------------------------------------------------------------
//
// ValueDisplay - a labeled text display widget
//
// Combines QLabel widgets to produce a text-based value display widget
// with label and units.
//
// R. Pogge, OSU Astronomy Dept.
// pogge@astronomy.ohio-state.edu
// 2008 Feb 25
//
// Modification History:
//   2008 Mar 01 - color manipulation now uses the QPalette for greater
//                 consistency with general Qt practice [rwp/osu]
//
//---------------------------------------------------------------------------

#include <QtWidgets>

#include "valuedisplay.h"

ValueDisplay::ValueDisplay(const QString &labelStr, const QString &unitsStr, 
			   QWidget *parent)
  : QWidget(parent)
{

  label = new QLabel(labelStr);
  labelText = labelStr;

  display = new QLabel;
  display->setAlignment(Qt::AlignVCenter|Qt::AlignRight);

  units = new QLabel(unitsStr);
  unitsText = unitsStr;

  // The color palettes

  displayPalette = new QPalette;
  *displayPalette = display->palette();

  labelPalette = new QPalette;
  *labelPalette = label->palette();

  unitsPalette = new QPalette;
  *unitsPalette = units->palette();

  // Layout of the elements of this widget: Label Display Units

  QHBoxLayout *dispLayout = new QHBoxLayout;
  if (!labelText.isEmpty())
    dispLayout->addWidget(label);
  dispLayout->addWidget(display);
  if (!unitsText.isEmpty())
    dispLayout->addWidget(units);
  dispLayout->addStretch();
  dispLayout->setContentsMargins(0,0,0,0);  // removes extra margins

  setLayout(dispLayout);

  // Default colors for the widget states
  //    Normal = blue
  //    Change = SAE/ECE amber, a better contrast than chroma Amber
  //    Fault  = red

  setNormalColor(Qt::blue);
  setChangeColor(QColor(255,126,0,255));
  setFaultColor(Qt::red);

  setNormal(); // start out normal

}

// Set the text in the display widget

void ValueDisplay::setText(const QString &txt)
{
  display->setText(txt);
}

// Set the label text

void ValueDisplay::setLabelText(const QString &txt)
{
  label->setText(txt);
  labelText = txt;
}

// Set the units text

void ValueDisplay::setUnitsText(const QString &txt)
{
  units->setText(txt);
  unitsText = txt;
}

// Change the color of the displayed text

void ValueDisplay::setTextColor(const QColor &color)
{
  displayPalette->setColor(QPalette::WindowText,color);
  display->setPalette(*displayPalette);
}

// Change the color of the label

void ValueDisplay::setLabelColor(const QColor &color)
{
  labelPalette->setColor(QPalette::WindowText,color);
  label->setPalette(*labelPalette);
}

// Change the color of the units (probably never done...)

void ValueDisplay::setUnitsColor(const QColor &color)
{
  unitsPalette->setColor(QPalette::WindowText,color);
  units->setPalette(*unitsPalette);
}

// Change font of all elements

void ValueDisplay::setFont(const QFont &font)
{
  display->setFont(font);
  label->setFont(font);
  units->setFont(font);
}

// Return the contents of the display text

QString ValueDisplay::text() const
{
  return displayText;
}

// Clear the display box

void ValueDisplay::clear()
{
  display->clear();
  setNormal();
}

// Change the display color for the "normal" condition

void ValueDisplay::setNormal()
{
  displayPalette->setColor(QPalette::WindowText,fgNormal);
  display->setPalette(*displayPalette);
}

// Change the display color for the "changing" condition

void ValueDisplay::setChange()
{
  displayPalette->setColor(QPalette::WindowText,fgChange);
  display->setPalette(*displayPalette);
}

// Change the display color for the "fault" condition

void ValueDisplay::setFault()
{
  displayPalette->setColor(QPalette::WindowText,fgFault);
  display->setPalette(*displayPalette);
}

// Set the colors for the widget states
//   Does not actually change any colors until one of setNormal() etc. are
//   invoked.  Also does not (cannot) validate colors

void ValueDisplay::setNormalColor(const QColor &fg)
{
  fgNormal = fg;
}

void ValueDisplay::setChangeColor(const QColor &fg)
{
  fgChange = fg;
}

void ValueDisplay::setFaultColor(const QColor &fg)
{
  fgFault = fg;
}
