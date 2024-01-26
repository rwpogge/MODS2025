#include <QtGui>

//---------------------------------------------------------------------------
// 
// Graphical sleep button.  Icon is based on the recommendations of
// the IEEE 1621 standard.  The normal state is an amber crescent moon
// glyph, unknown power state is black.
//
// A separate PowerButton class implements an IEEE 1621 power on/off
// button.
//

#include "sleepbutton.h"

SleepButton::SleepButton(const int &mySize, QWidget *parent)
  : QToolButton(parent)
{
  buttonSize = mySize;
  QSize iconSize(buttonSize,buttonSize);
  setToolButtonStyle(Qt::ToolButtonIconOnly);
  setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
  setIcon(QIcon(":/images/IEEE1621_SleepAmber.png"));
  setIconSize(iconSize);
  setFocusPolicy(Qt::ClickFocus);
}

// Set the button appearance to normal, amber icon with subtle amber
// highlights

void SleepButton::setNormal()
{
  setIcon(QIcon(":/images/IEEE1621_SleepAmber.png"));
  QSize iconSize(buttonSize,buttonSize);
  setIconSize(iconSize);
}

// Set the button appearance to "unknown", black icon with subtle
// black highlights

void SleepButton::setUnknown()
{
  setIcon(QIcon(":/images/IEEE1621_SleepBlack.png"));
  QSize iconSize(buttonSize,buttonSize);
  setIconSize(iconSize);
}

// Enable the button

void SleepButton::enable()
{
  setDisabled(false);
}

// Disable the button

void SleepButton::disable()
{
  setDisabled(true);
}
