#ifndef TEXTDISPLAY_H
#define TEXTDISPLAY_H

#include <QWidget>

class QLineEdit;
class QLabel;
class QString;
class QPalette;

class TextDisplay : public QWidget
{
  Q_OBJECT

public:
  TextDisplay(const QString &labelStr, const QString &unitsStr, 
	      int maxw=0, QWidget *parent = 0);
  void setLabelText(const QString &);
  void setLabelColor(const QColor &);
  void setUnitsText(const QString &);
  void setUnitsColor(const QColor &);
  void setFont(const QFont &);   
  void setText(const QString &, const QColor &); 
  void setTextColor(const QColor &); 
  void setAlignment(Qt::Alignment);
  void setWidth(const int &);
  QString text() const;
  void clear();
  void setTimer(const int &);
  void stopTimer();
  void setNormal();
  void setChange();
  void setFault();
  void setNormalColors(const QColor &, const QColor &);
  void setChangeColors(const QColor &, const QColor &);
  void setFaultColors(const QColor &, const QColor &);

private slots:
  void clearDisplay();

private:
  QLineEdit *display;
  QString   displayText;
  QPalette  *displayPalette;

  QLabel    *label;
  QString   labelText;
  QPalette  *labelPalette;

  QLabel    *units;
  QString   unitsText;
  QPalette  *unitsPalette;

  QColor fgNormal, bgNormal;
  QColor fgChange, bgChange;
  QColor fgFault,  bgFault;

  QTimer    *displayTimer;
  int       displayTime;
  bool      useTimer;
};

#endif
