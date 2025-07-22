#ifndef WARMUPLAMP_H
#define WARMUPLAMP_H

#include <QDialog>

class QString;
class QTimer;
class QProgressDialog;

class WarmUpLamp : public QDialog
{
  Q_OBJECT

public:
  WarmUpLamp(const QString &labelStr, int mTO, QWidget *parent=0);
  void setWarmUp(const int &);     //!< set the warmup interval in seconds
  void setLampID(const QString &); //!< set the lamp ID
  void start(const int &);  //!< start the warm-up timer

private slots:
  void update();  //!< update the progress bar each timer "tick"
  void cancel();  //!< cancel the timer and cleanup

private:
  QString lamp;   //!< lamp that is being warmed up
  int timeout;    //!< timeout interval in seconds
  
  QProgressDialog *dialog; //!< the progress bar dialog
  QTimer *timer;  //!< warmup timer
  int tick;       //!< timer "tick" interval in milliseconds
  int steps;      //!< step counter


};

#endif
