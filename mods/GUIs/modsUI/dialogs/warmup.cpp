#include <QtWidgets>

#include "warmup.h"

//
// A modeless dialog to show lamp warmup progress
//

WarmUpLamp::WarmUpLamp(const QString &lampStr, int mTO, QWidget *parent)
  : QDialog(parent)
{
  lamp  = lampStr;
  timeout = mTO;

  // Setup the warmup timer

  timer = new QTimer(this);
  connect(timer,SIGNAL(timeout()),this,SLOT(update()));

  tick = (int)((double)(timeout)/100.0)*1000;   // tick size in milliseconds

  // QProgressDialog

  QString label = lamp+" Lamp Warming Up...";
  dialog = new QProgressDialog(label,"Dismiss",0,100);
  connect(dialog,SIGNAL(canceled()),this,SLOT(cancel()));

  // Dialog decor, fonts, etc.

  QString titleStr = lamp+" Lamp Warm-Up Monitor";
  dialog->setWindowTitle(titleStr);
 
  // reset the step counter

  steps = 0;

}

// Update the progress timer.  If we are out of time, Let them know and
// change the button to "dismiss"

void WarmUpLamp::update()
{
  dialog->setValue(steps);
  steps++;
  if (steps > dialog->maximum()) {
    timer->stop();
    QString labelStr = lamp+" Lamp Warmup Complete";
    dialog->setLabelText(labelStr);
  }
  else {
    int remaining = timeout - (int)(timeout*dialog->value()/dialog->maximum());
    QString labelStr = lamp+" Lamp Warming Up, "+QString::number(remaining)+" sec remaining...";
    dialog->setLabelText(labelStr);
  }
}

// Start the warm-up timer

void WarmUpLamp::start(const int &time)
{
  timeout = time;
  tick = (int)((double)(timeout)/100.0)*1000;   // tick size in milliseconds
  dialog->setValue(0);
  steps = 0;
  timer->stop();
  timer->start(tick);
}

// Warm up timer was canceled or dismissed

void WarmUpLamp::cancel() 
{
  timer->stop();
  // any other cleanup we need?
  
}

// Set the warm-up time

void WarmUpLamp::setWarmUp(const int &to)
{
  timeout = to;
}

// Set the lamp ID

void WarmUpLamp::setLampID(const QString &str)
{
  lamp = str;
  QString titleStr = lamp+" Lamp Warm-Up Monitor";
  dialog->setWindowTitle(titleStr);
}

