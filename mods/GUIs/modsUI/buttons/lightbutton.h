#ifndef LIGHTBUTTON_H
#define LIGHTBUTTON_H

#include <QToolButton>

class LightButton : public QToolButton
{
  Q_OBJECT

public:
  LightButton(const QString &text, QWidget *parent=0);
  void setTextColor(const QColor &);
  void setOff();
  void setRed();
  void setGreen();
  void setBlue();
  void setYellow();
  void enable();
  void disable();

};

#endif
