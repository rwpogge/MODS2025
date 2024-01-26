#ifndef POWERBUTTON_H
#define POWERBUTTON_H

#include <QToolButton>
#include <QTimer>

class PowerButton : public QToolButton
{
  Q_OBJECT

public:
  PowerButton(const int &mySize=128, const bool &hard=true, QWidget *parent=0);
  void setOff();      //!< Set the power button to OFF
  void setOn();       //!< Set the power button to ON
  void setSleep();    //!< Set the power button to SLEEP
  void setChanging(); //!< Set the power button to Power State Changing
  void setFault();    //!< Set the power button to FAULT
  void enable();      //!< Enable the power button
  void disable();     //!< Disable the power button

private:
  int buttonSize;     //!< Size of the icon in pixels
  bool hardOff;       //!< True of a hard-off, false of soft-off (changes icon)

};

#endif
