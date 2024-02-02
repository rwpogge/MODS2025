#ifndef TEXTHISTENTRY_H
#define TEXTHISTENTRY_H

#include <QtWidgets/QWidget>

class QLineEdit;
class QLabel;
class QString;
class QStringList;

class TextHistEntry : public QWidget
{
  Q_OBJECT

public:
  TextHistEntry(const QString &labelStr, const QString &unitsStr, 
		int maxw=0, int maxHist=100, QWidget *parent = 0);
  void setLabelText(const QString &);
  void setUnitsText(const QString &);
  void setText(const QString &); 
  QString value() const;
  void clearHistory();
  void clear();
  void setEntryColor(const QColor &);
  void setTextColor(const QColor &);
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

protected:
  void keyPressEvent(QKeyEvent *event);

signals:
  void dataReady(const QString &);

private slots:
  void getData();
  void addHistory();

private:
  QLineEdit   *entry;
  QString     entryText;
  QPalette    *entryPalette;
  QStringList *entryHistory;
  int         maxHistory;
  int         numHistory;

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
