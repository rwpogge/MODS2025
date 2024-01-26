
#include <QtGui>

#include "lightbutton.h"

LightButton::LightButton(const QString &text, QWidget *parent)
  : QToolButton(parent)
{
  QSize iconSize(16,16);
  setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
  setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  setIcon(QIcon(":/images/offled.png"));
  setIconSize(iconSize);
  setText(text);
  setFocusPolicy(Qt::ClickFocus);
}

void LightButton::setTextColor(const QColor &color)
{
  QPalette newPalette = palette();
  newPalette.setColor(QPalette::ButtonText,color);
  setPalette(newPalette);
}

// convenience functions for changing the LED color

void LightButton::setOff()
{
  QSize iconSize(16,16);
  setIcon(QIcon(":/images/offled.png"));
  setIconSize(iconSize);
}

void LightButton::setRed()
{
  QSize iconSize(16,16);
  setIcon(QIcon(":/images/redled.png"));
  setIconSize(iconSize);
}

void LightButton::setGreen()
{
  QSize iconSize(16,16);
  setIcon(QIcon(":/images/greenled.png"));
  setIconSize(iconSize);
}

void LightButton::setBlue()
{
  QSize iconSize(16,16);
  setIcon(QIcon(":/images/blueled.png"));
  setIconSize(iconSize);
}

void LightButton::setYellow()
{
  QSize iconSize(16,16);
  setIcon(QIcon(":/images/yellowled.png"));
  setIconSize(iconSize);
}

// Enable the button

void LightButton::enable()
{
  setDisabled(false);
}

// Disable the button

void LightButton::disable()
{
  setDisabled(true);
}
