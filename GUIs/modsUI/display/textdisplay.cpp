//---------------------------------------------------------------------------
//
// TextDisplay - a labeled text display widget
//
// Combines QLabel and QLineEdit to produce a readonly text display widget
// that avoids some of the bizarre scaling problems of QLabel
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

#include <QtGui>

#include "textdisplay.h"

TextDisplay::TextDisplay(const QString &labelStr, const QString &unitsStr, 
		     int maxw, QWidget *parent)
  : QWidget(parent)
{

  // Widgets we use: 2 QLabels (label and units), 1 QLineEdit (readonly)

  label = new QLabel(labelStr);
  labelText = labelStr;

  display = new QLineEdit(this);
  display->setReadOnly(true);

  units = new QLabel(unitsStr);
  unitsText = unitsStr;

  // The color palettes

  displayPalette = new QPalette;
  *displayPalette = display->palette();

  labelPalette = new QPalette;
  *labelPalette = label->palette();

  unitsPalette = new QPalette;
  *unitsPalette = units->palette();

  // If a maximum display window width is set, set an appropriate width for
  // the QLineEdit widget, otherwise let it autosize

  if (maxw > 0) {
    display->setMaxLength(maxw);
    int mxw = (int)(0.5*maxw*display->minimumSizeHint().width());
    display->setMaximumWidth(mxw);
  }

  // This height looks good

  int hMax = (int)(0.8*display->minimumSizeHint().height());
  display->setMaximumHeight(hMax);

  // Default Color scheme - display "base" same as window base (gray or whatever).
  //   Normal = Black
  //   Change = SAE/ECE Amber, better contrast than chroma Amber
  //   Fault  = Red

  setNormalColors(palette().color(QPalette::Window),Qt::black);
  setChangeColors(palette().color(QPalette::Window),QColor(255,126,0,255));
  setFaultColors(Qt::red,Qt::black);

  setNormal();

  //displayPalette->setColor(QPalette::Base,palette().color(QPalette::Window));
  //display->setPalette(*displayPalette);

  // Layout of the elements of this widget: Label Display Units

  QHBoxLayout *dispLayout = new QHBoxLayout;
  if (!labelText.isEmpty())
    dispLayout->addWidget(label);
  dispLayout->addWidget(display);
  if (!unitsText.isEmpty())
    dispLayout->addWidget(units);
  if (maxw>0)
    dispLayout->addStretch();
  dispLayout->setContentsMargins(0,0,0,0);  // removes extra margins

  setLayout(dispLayout);

  // We're just a display, do not accept focus

  display->setFocusPolicy(Qt::NoFocus); 
  
  // By default, all messages displayed in the text plane are 
  // persistent, so set useTimer false.

  useTimer = false;
  displayTimer = 0;

}

// Set the text in the QLineEdit widget and its color

void TextDisplay::setText(const QString &txt, const QColor &color)
{
  display->setText(txt);
  setTextColor(color);
  display->home(false);
  // If using the timer, restart it for this
  if (useTimer) {
    if (displayTimer->isActive())
      displayTimer->stop();
    displayTimer->start(displayTime);
    displayTimer->setSingleShot(true);
  }    
}

// Set the label text

void TextDisplay::setLabelText(const QString &txt)
{
  label->setText(txt);
  labelText = txt;
}

// Set the units text

void TextDisplay::setUnitsText(const QString &txt)
{
  units->setText(txt);
  unitsText = txt;
}

// Change the color of the displayed text

void TextDisplay::setTextColor(const QColor &color)
{
  displayPalette->setColor(QPalette::Text,color);
  display->setPalette(*displayPalette);
}

// Change the color of the label

void TextDisplay::setLabelColor(const QColor &color)
{
  labelPalette->setColor(QPalette::WindowText,color);
  label->setPalette(*labelPalette);
}

// Change the color of the units (probably never done...)

void TextDisplay::setUnitsColor(const QColor &color)
{
  unitsPalette->setColor(QPalette::WindowText,color);
  units->setPalette(*unitsPalette);
}

// Change font of all elements

void TextDisplay::setFont(const QFont &font)
{
  display->setFont(font);
  label->setFont(font);
  units->setFont(font);
}

// Return the contents of the disp text

QString TextDisplay::text() const
{
  return displayText;
}

// Clear the disp box

void TextDisplay::clear()
{
  display->clear();
}

// Setup the display timer
// 
// argument: displayTO = display time in seconds

void TextDisplay::setTimer(const int &dispTime)
{
  if (dispTime <= 0) return;

  // Instantiate a new single-shot timer, but don't start it yet.

  if (!displayTimer) {
    displayTimer = new QTimer;
    connect(displayTimer,SIGNAL(timeout()),this,SLOT(clearDisplay()));
  }
  displayTimer->setSingleShot(true);
  useTimer = true;
  displayTime = 1000*dispTime; // convert to millseconds
}

//
// Uninstall the display timer
//

void TextDisplay::stopTimer()
{
  useTimer = false;
  if (!displayTimer) return;
  displayTimer->stop();
}

// Clear the disp box - private slot implementation, kind of redundant
// but in the future we may wish to do some other things in private.

void TextDisplay::clearDisplay()
{
  display->clear();
}

// Change the display color for the "normal" condition

void TextDisplay::setNormal()
{
  displayPalette->setColor(QPalette::Base,bgNormal);
  displayPalette->setColor(QPalette::Text,fgNormal);
  display->setPalette(*displayPalette);
}

// Change the display color for the "changing" condition

void TextDisplay::setChange()
{
  displayPalette->setColor(QPalette::Base,bgChange);
  displayPalette->setColor(QPalette::Text,fgChange);
  display->setPalette(*displayPalette);
}

// Change the display color for the "fault" condition

void TextDisplay::setFault()
{
  displayPalette->setColor(QPalette::Base,bgFault);
  displayPalette->setColor(QPalette::Text,fgFault);
  display->setPalette(*displayPalette);
}

// Set the colors for the widget states
//   Does not actually change any colors until one of setNormal() etc. are
//   invoked.  Also does not (cannot) validate colors

void TextDisplay::setNormalColors(const QColor &bg, const QColor &fg)
{
  bgNormal = bg;
  fgNormal = fg;
}

void TextDisplay::setChangeColors(const QColor &bg, const QColor &fg)
{
  bgChange = bg;
  fgChange = fg;
}

void TextDisplay::setFaultColors(const QColor &bg, const QColor &fg)
{
  bgFault = bg;
  fgFault = fg;
}

// Set the alignment of the text in the display box, following Qt::Alignment

void TextDisplay::setAlignment(Qt::Alignment align)
{
  display->setAlignment(align);
}

// Set the display width

void TextDisplay::setWidth(const int &maxw)
{
  if (maxw < 1) return;
  display->setMaxLength(maxw);
  int mxw = (int)(0.5*maxw*display->minimumSizeHint().width());
  display->setMaximumWidth(mxw);
}
