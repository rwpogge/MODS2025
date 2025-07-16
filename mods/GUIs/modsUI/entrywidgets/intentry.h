#ifndef INTENTRY_H
#define INTENTRY_H

#include <QWidget>

class QSpinBox;
class QLabel;
class QString;

class IntEntry : public QWidget
{
  Q_OBJECT

public:
  IntEntry(const QString &labelStr, const QString &unitsStr, int minval=0, 
	   int maxval=99, QWidget *parent = 0);
  void setLabelText(const QString &);
  void setUnitsText(const QString &);
  void setValue(const int &);
  int  value() const;
  void setEntryColor(const QColor &);
  void setValueColor(const QColor &);
  void setLabelColor(const QColor &);
  void setUnitsColor(const QColor &);
  void setFont(const QFont &);   
  void setNormal();
  void setChange();
  void setFault();
  void setNormalColors(const QColor &, const QColor &);
  void setChangeColors(const QColor &, const QColor &);
  void setFaultColors(const QColor &, const QColor &);
  void enable();
  void disable();

signals:
  void dataReady(const int &);

private slots:
  void getData();

private:
  QSpinBox  *entry;
  int       entryValue;
  QPalette  *entryPalette;

  QLabel    *label;
  QString   labelText;
  QPalette  *labelPalette;

  QLabel    *units;
  QString   unitsText;
  QPalette  *unitsPalette;

  QColor bgNormal, fgNormal;
  QColor bgChange, fgChange;
  QColor bgFault, fgFault;
};

#endif
