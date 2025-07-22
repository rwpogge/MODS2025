#include <QtWidgets>

//---------------------------------------------------------------------------
// 
// Graphical power button.  The icon is based on the recommendations
// of the IEEE 1621 standard.  We use black as "off", green for "on",
// and amber for "sleep".  We add to the IEEE 1621 recommendations two
// additional color cues: red for "fault" and yellow for "changing".
// We elect not to have power transition states blink.
//
// Power switches are of the "hard off" or "soft off" variety, set by
// the "hard" boolean on construction.  
//
// For hard power-off, we use the IEEE 1621 recommended icon of a full
// circle with a | inside.
//
// For soft power-off, we use the IEEE 1621 symbol of a broken circle
// cut by a |.  "Soft-Off" means we go into a low-power state with
// some power still active.
//
// A separate SleepButton class implements the IEEE 1621 crescent moon
// icon for "sleep" state.
//

#include "powerbutton.h"

PowerButton::PowerButton(const int &mySize, const bool &hard, QWidget *parent)
  : QToolButton(parent)
{
  buttonSize = mySize;
  hardOff = hard;
  QSize iconSize(buttonSize,buttonSize);
  setToolButtonStyle(Qt::ToolButtonIconOnly);
  setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
  if (hardOff)
    setIcon(QIcon(":/images/IEEE1621_PowerOff.png"));
  else
    setIcon(QIcon(":/images/IEEE1621_SoftOff.png"));
  setIconSize(iconSize);
  setFocusPolicy(Qt::ClickFocus);
}

// Set the button appearance to "On", icon bright green with bright
// green highlights

void PowerButton::setOn()
{
  if (hardOff)
    setIcon(QIcon(":/images/IEEE1621_PowerOn.png"));
  else
    setIcon(QIcon(":/images/IEEE1621_SoftOn.png"));
  QSize iconSize(buttonSize,buttonSize);
  setIconSize(iconSize);
}

// Set the button appearance to "Off", icon black with subtle black
// highlights

void PowerButton::setOff()
{
  if (hardOff)
    setIcon(QIcon(":/images/IEEE1621_PowerOff.png"));
  else
    setIcon(QIcon(":/images/IEEE1621_SoftOff.png"));
  QSize iconSize(buttonSize,buttonSize);
  setIconSize(iconSize);
}

// Set the button appearance to "Sleep", icon amber with subtle amber
// highlights

void PowerButton::setSleep()
{
  if (hardOff)
    setIcon(QIcon(":/images/IEEE1621_PowerSleep.png"));
  else
    setIcon(QIcon(":/images/IEEE1621_SoftSleep.png"));
  QSize iconSize(buttonSize,buttonSize);
  setIconSize(iconSize);
}

// Set the button appearance to "Power State Changing", which makes
// the icon bright yellow.  We elect not to blink.

void PowerButton::setChanging()
{
  if (hardOff)
    setIcon(QIcon(":/images/IEEE1621_PowerSleep.png"));
  else
    setIcon(QIcon(":/images/IEEE1621_SoftSleep.png"));
  QSize iconSize(buttonSize,buttonSize);
  setIconSize(iconSize);
}

// Set the button appearance to "Fault", red with bright red
// highlights

void PowerButton::setFault()
{
  if (hardOff)
    setIcon(QIcon(":/images/IEEE1621_PowerFault.png"));
  else
    setIcon(QIcon(":/images/IEEE1621_SoftFault.png"));
  QSize iconSize(buttonSize,buttonSize);
  setIconSize(iconSize);
}

// Enable the button

void PowerButton::enable()
{
  setDisabled(false);
}

// Disable the button

void PowerButton::disable()
{
  setDisabled(true);
}

