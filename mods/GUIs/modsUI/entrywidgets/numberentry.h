#ifndef NUMBERENTRY_H
#define NUMBERENTRY_H

#include <QWidget>

class QLineEdit;
class QLabel;
class QString;
class QDoubleValidator;

class NumberEntry : public QWidget
{
  Q_OBJECT

public:
  NumberEntry(const QString &labelStr, const QString &unitsStr, int maxw=0, 
	      double minVal=0, double maxVal=0, int decimals=0, QWidget *parent = 0);
  void setLabelText(const QString &);
  void setUnitsText(const QString &);
  void setValue(const double &); 
  double value() const;
  int  intValue() const;
  QString text() const;
  void clear();
  void setEntryColor(const QColor &);
  void setValueColor(const QColor &);
  void setLabelColor(const QColor &);
  void setUnitsColor(const QColor &);
  void setFont(const QFont &);   
  void setNormal();
  void setChange();
  void setFault();
  void setEditing();
  void setOutOfRange();
  void setNormalColors(const QColor &, const QColor &);
  void setChangeColors(const QColor &, const QColor &);
  void setFaultColors(const QColor &, const QColor &);
  void enable();
  void disable();
  void setInputMask(const QString &);

signals:
  void dataReady(const double &);
  void intReady(const int &);
  void dataCommit(const double &);
  void intCommit(const int &);

private slots:
  void getData();
  void getCommit(); 
  void entryChanged(); 

private:
  QLineEdit *entry;
  double    entryValue;
  double    minimum;
  double    maximum;
  int       dpmax;
  bool      validate;

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
