#ifndef VALUEDISPLAY_H
#define VALUEDISPLAY_H

#include <QWidget>

class QLabel;
class QString;
class QPalette;

class ValueDisplay : public QWidget
{
  Q_OBJECT

public:
  ValueDisplay(const QString &labelStr, const QString &unitsStr, 
	       QWidget *parent = 0);
  void setLabelText(const QString &);
  void setLabelColor(const QColor &);
  void setUnitsText(const QString &);
  void setUnitsColor(const QColor &);
  void setText(const QString &); 
  void setTextColor(const QColor &); 
  QString text() const;
  void clear();
  void setFont(const QFont &);   
  void setNormal();
  void setChange();
  void setFault();
  void setNormalColor(const QColor &);
  void setChangeColor(const QColor &);
  void setFaultColor(const QColor &);

private:
  QLabel    *display;
  QString   displayText;
  QPalette  *displayPalette;

  QLabel    *label;
  QString   labelText;
  QPalette  *labelPalette;

  QLabel    *units;
  QString   unitsText;
  QPalette  *unitsPalette;

  QColor fgNormal;
  QColor fgChange;
  QColor fgFault;

};

#endif
