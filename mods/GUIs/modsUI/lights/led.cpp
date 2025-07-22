//
// Silly little widget that shows a colored LED indicator
//

#include <QtWidgets>

#include "led.h"

LED::LED(QWidget *parent) : QLabel(parent)
{

  // The LED pixmaps

  redLED    = new QPixmap(":/images/redled.png");
  greenLED  = new QPixmap(":/images/greenled.png");
  blueLED   = new QPixmap(":/images/blueled.png");
  yellowLED = new QPixmap(":/images/yellowled.png");
  offLED    = new QPixmap(":/images/offled.png");

  scale = 16;  // default size

  // By default, we use the "off" LED

  setPixmap(offLED->scaled(scale,scale));

}

// convenience functions for changing the LED color

void LED::setOff()
{
  setPixmap(offLED->scaled(scale,scale));
}

void LED::setRed()
{
  setPixmap(redLED->scaled(scale,scale));
}

void LED::setGreen()
{
  setPixmap(greenLED->scaled(scale,scale));
}

void LED::setBlue()
{
  setPixmap(blueLED->scaled(scale,scale));
}

void LED::setYellow()
{
  setPixmap(yellowLED->scaled(scale,scale));
}
