#ifndef SELECTENTRY_H
#define SELECTENTRY_H

#include <QWidget>

class QComboBox;
class QLabel;
class QString;

class SelectEntry : public QWidget
{
  Q_OBJECT

public:
  SelectEntry(const QString &labelStr, const QString &unitsStr, QWidget *parent=0);
  void setLabelText(const QString &);
  void setUnitsText(const QString &);
  void addItem(const QString &, const QString &);
  void addItems(const QStringList &);
  void clear();
  void removeItem(const int &);
  void insertItem(const int &, const QString &, const QString &);
  void insertSeparator(const int &);
  void setItemText(const int &, const QString &);
  QString itemData(const int &);
  QString itemText(const int &);
  int currentIndex() const;
  void setCurrentIndex(const int &); 
  int findData(const QString &) const;
  int findText(const QString &) const;
  void setEntryColor(const QColor &);
  void setValueColor(const QColor &);
  void setLabelColor(const QColor &);
  void setUnitsColor(const QColor &);
  void setFont(const QFont &);
  void setNormal();
  void setChange();
  void setFault();
  void enable();
  void disable();
  void setMinimumContentsLength(const int &);
  void setMaxSelectorWidth(const int &);
  void scaleHeight(const double &);
  int count();

signals:
  void dataReady(const QString &);
  void indexReady(const int &);

private slots:
  void getData();

private:
  QComboBox *entry;
  QPalette  *entryPalette;
  int       iSelected;

  QLabel    *label;
  QString   labelText;
  QPalette  *labelPalette;

  QLabel    *units;
  QString   unitsText;
  QPalette  *unitsPalette;

};

#endif
