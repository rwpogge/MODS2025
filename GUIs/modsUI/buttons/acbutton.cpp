#include <QtWidgets>

//---------------------------------------------------------------------------
// 
// Graphical power button.  Icon is based on the recommendations
// of the IEEE 1621 standard.  This is a main power button that uses
// the following color scheme:
//
//   TurnOn:  Green = power is off, press to power on
//   TurnOff: Red   = power is on,  press to power off
//   Wake:    Inverted Amber = power is sleep mode, press to wake
//
// A separate SleepButton class implements the IEEE 1621 crescent moon
// icon for the "put to sleep" action.
//
// This behaves differently from other buttons.  We also clue in what
// is going on using text queues in higher-level widgets that
// incorporate this icon-based toolbutton.
// 
// NOTE: The name of the button icon is the function that will be
// performed when the button is pressed, not the power state
// indication, as with the other power button classes in this suite
// (e.g., PowerButton and BreakerButton).
//

#include "acbutton.h"

ACButton::ACButton(const int &mySize, QWidget *parent)
  : QToolButton(parent)
{
  buttonSize = mySize;
  QSize iconSize(buttonSize,buttonSize);
  setToolButtonStyle(Qt::ToolButtonIconOnly);
  setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
  setIcon(QIcon(":/images/IEEE1621_TurnOn.png"));
  setIconSize(iconSize);
  setFocusPolicy(Qt::ClickFocus);
}

// Set the button appearance to the "Turn On" action: bright red
// button with white IEEE 1621 Power On/Off icon

void ACButton::setTurnOn()
{
  setIcon(QIcon(":/images/IEEE1621_TurnOn.png"));
  QSize iconSize(buttonSize,buttonSize);
  setIconSize(iconSize);
}

// Set the button appearance to the "Turn Off" action: bright green
// button with white IEEE 1621 Power On/Off icon

void ACButton::setTurnOff()
{
  setIcon(QIcon(":/images/IEEE1621_TurnOff.png"));
  QSize iconSize(buttonSize,buttonSize);
  setIconSize(iconSize);
}

// Set the button appearance to the "Wake" action: gray button with
// an amber IEEE 1621 Power On/Off icon

void ACButton::setWake()
{
  setIcon(QIcon(":/images/IEEE1621_Wake.png"));
  QSize iconSize(buttonSize,buttonSize);
  setIconSize(iconSize);
}

// Set the button appearance to "Power State Changing".  Since blinking
// is rude, we turn the button a bright yellow instead.

void ACButton::setChanging()
{
  setIcon(QIcon(":/images/IEEE1621_PowerChange.png"));
  QSize iconSize(buttonSize,buttonSize);
  setIconSize(iconSize);
}

// Enable the button

void ACButton::enable()
{
  setDisabled(false);
}

// Disable the button

void ACButton::disable()
{
  setDisabled(true);
}

