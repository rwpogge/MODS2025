#ifndef ACTIONBUTTON_H
#define ACTIONBUTTON_H

#include <QToolButton>

class ActionButton : public QToolButton
{
  Q_OBJECT

public:
  ActionButton(const QString &text, QWidget *parent=0);
  void setTextColor(const QColor &);
  void setColor(const QColor &);
  void setNormal();
  void setChange();
  void setFault();
  void highLight();
  void setNormalColors(const QColor &, const QColor &);
  void setChangeColors(const QColor &, const QColor &);
  void setFaultColors(const QColor &, const QColor &);
  void setHighLightColors(const QColor &, const QColor &);
  void enable();
  void disable();

private:
  QColor bgNormal, fgNormal;
  QColor bgChange, fgChange;
  QColor bgFault, fgFault;
  QColor fgHiLight, bgHiLight;
};

#endif
