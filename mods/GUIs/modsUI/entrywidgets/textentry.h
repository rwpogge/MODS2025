#ifndef TEXTENTRY_H
#define TEXTENTRY_H

#include <QWidget>

class QLineEdit;
class QLabel;
class QString;

class TextEntry : public QWidget
{
  Q_OBJECT

public:
  TextEntry(const QString &labelStr, const QString &unitsStr, int maxw=0, 
	    QWidget *parent = 0);
  void setLabelText(const QString &);
  void setUnitsText(const QString &);
  void setText(const QString &); 
  QString value() const;
  void clear();
  void setEntryColor(const QColor &);
  void setTextColor(const QColor &);
  void setLabelColor(const QColor &);
  void setUnitsColor(const QColor &);
  void setEntryFocusPolicy(Qt::FocusPolicy);
  void setFont(const QFont &);   
  void setNormal();
  void setChange();
  void setFault();
  void setNormalColors(const QColor &, const QColor &);
  void setChangeColors(const QColor &, const QColor &);
  void setFaultColors(const QColor &, const QColor &);
  void enable();
  void disable();
  void setInputMask(const QString &);
  void setCommit(const bool &);

signals:
  void dataReady(const QString &);
  void textReady(const QString &);

private slots:
  void getData();      //!< get entry data on editingFinished and emit the textReady() signal
  void commitData();   //!< get entry data on Return commit and emit the dataReady() signal
  void entryChanged(); //!< entry is changing but not yet committed
 
private:
  QLineEdit *entry;
  QString   entryValue;
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

  bool commit;    //!< Entries must be committed by hitting Return or not.
};

#endif
