#ifndef LED_H
#define LED_H

#include <QLabel>

class LED : public QLabel
{
  Q_OBJECT

public:
  LED(QWidget *parent=0);
  void setOff();
  void setRed();
  void setGreen();
  void setBlue();
  void setYellow();

private:
  QPixmap *redLED;
  QPixmap *greenLED;
  QPixmap *blueLED;
  QPixmap *yellowLED;
  QPixmap *offLED;

  int scale;

};

#endif
