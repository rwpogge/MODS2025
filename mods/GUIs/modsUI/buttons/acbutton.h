#ifndef ACBUTTON_H
#define ACBUTTON_H

#include <QToolButton>
#include <QTimer>

class ACButton : public QToolButton
{
  Q_OBJECT

public:
  ACButton(const int &mySize=128, QWidget *parent=0);
  void setTurnOn();   //!< Set the button to show the Turn On icon
  void setTurnOff();  //!< Set the button to show the Turn Off icon
  void setWake();     //!< Set the button to show the Wake icon
  void setChanging(); //!< Set the button to the power-transition state
  void enable();      //!< Enable the power button
  void disable();     //!< Disable the power button

private:
  int buttonSize;     //!< Size of the icon in pixels
};

#endif
