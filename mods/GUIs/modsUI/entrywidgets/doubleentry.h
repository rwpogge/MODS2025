#ifndef DOUBLEENTRY_H
#define DOUBLEENTRY_H

#include <QWidget>

class QDoubleSpinBox;
class QLabel;
class QString;

class DoubleEntry : public QWidget
{
  Q_OBJECT

public:
  DoubleEntry(const QString &labelStr, const QString &unitsStr, 
	      double minVal=0.0, double maxVal=99.0, int decimals=2, 
	      QWidget *parent = 0);
  void   setLabelText(const QString &);
  void   setUnitsText(const QString &);
  double value() const;
  void   setValue(const double &);
  void   setEntryColor(const QColor &);
  void   setValueColor(const QColor &);
  void   setLabelColor(const QColor &);
  void   setUnitsColor(const QColor &);
  void   setFont(const QFont &);   
  void   setNormal();
  void   setChange();
  void   setFault();
  void   setNormalColors(const QColor &, const QColor &);
  void   setChangeColors(const QColor &, const QColor &);
  void   setFaultColors(const QColor &, const QColor &);
  void   enable();
  void   disable();

signals:
  void dataReady(const double &);

private slots:
  void getData();

private:
  QDoubleSpinBox *entry;
  QPalette  *entryPalette;
  double numValue;
  double numMin;
  double numMax;

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
