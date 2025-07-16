#ifndef SLEEPBUTTON_H
#define SLEEPBUTTON_H

#include <QToolButton>

class SleepButton : public QToolButton
{
  Q_OBJECT

public:
  SleepButton(const int &mySize=128, QWidget *parent=0);
  void setNormal();  //!< Set the sleep button to the normal state
  void setUnknown(); //!< Set the sleep button to the unknown state
  void enable();     //!< Enable the sleep button
  void disable();    //!< Disable the sleep button

private:
  int buttonSize;    //!< Size of the icon in pixels

};

#endif
