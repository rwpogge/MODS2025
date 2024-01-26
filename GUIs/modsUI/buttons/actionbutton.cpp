
#include <QtGui>

#include "actionbutton.h"

ActionButton::ActionButton(const QString &text, QWidget *parent)
  : QToolButton(parent)
{
  setText(text);
  setFocusPolicy(Qt::ClickFocus);

  // Normal state is black text on the default windows background
  // HighLighted Normal state is blue text on the default windows background
  // Change state is black text on a chroma amber background
  // Fault state is black text on a red background

  setNormalColors(palette().color(QPalette::Window),Qt::black);
  setHighLightColors(palette().color(QPalette::Window),Qt::darkGreen);
  setChangeColors(QColor(255,191,0,255),Qt::black);
  setFaultColors(Qt::red,Qt::black);
}

void ActionButton::setTextColor(const QColor &color)
{
  QPalette newPalette = palette();
  newPalette.setColor(QPalette::ButtonText,color);
  setPalette(newPalette);
}

void ActionButton::setColor(const QColor &color)
{
  QPalette newPalette = palette();
  newPalette.setColor(QPalette::Button,color);
  setPalette(newPalette);
}

// Change the button color to the "normal", but unhighlighed state

void ActionButton::setNormal()
{
  QPalette newPalette = palette();
  newPalette.setColor(QPalette::Button,bgNormal);
  newPalette.setColor(QPalette::ButtonText,fgNormal);
  setPalette(newPalette);
}

// Change the button color to the "changing" state

void ActionButton::setChange()
{
  QPalette newPalette = palette();
  newPalette.setColor(QPalette::Button,bgChange);
  newPalette.setColor(QPalette::ButtonText,fgChange);
  setPalette(newPalette);
}

// Change the button color to the "fault" state

void ActionButton::setFault()
{
  QPalette newPalette = palette();
  newPalette.setColor(QPalette::Button,bgFault);
  newPalette.setColor(QPalette::ButtonText,fgFault);
  setPalette(newPalette);
}

// Change the button color to the normal but highlighted state

void ActionButton::highLight()
{
  QPalette newPalette = palette();
  newPalette.setColor(QPalette::Button,bgHiLight);
  newPalette.setColor(QPalette::ButtonText,fgHiLight);
  setPalette(newPalette);
}


// Set the colors for the different button states
//   Does not actually change any colors until one of setNormal() etc. are
//   invoked.  Also does not (cannot) validate colors

void ActionButton::setNormalColors(const QColor &bg, const QColor &fg)
{
  bgNormal = bg;
  fgNormal = fg;
}

void ActionButton::setChangeColors(const QColor &bg, const QColor &fg)
{
  bgChange = bg;
  fgChange = fg;
}

void ActionButton::setFaultColors(const QColor &bg, const QColor &fg)
{
  bgFault = bg;
  fgFault = fg;
}

void ActionButton::setHighLightColors(const QColor &bg, const QColor &fg)
{
  bgHiLight = bg;
  fgHiLight = fg;
}

// Enable the widget - also sets the colors to "Normal" state

void ActionButton::enable()
{
  setDisabled(false);
  setNormal();
}

// Disable the widget

void ActionButton::disable()
{
  setDisabled(true);
  setNormal();
  setTextColor(Qt::darkGray);
}

