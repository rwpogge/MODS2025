#ifndef ANGLEENTRY_H
#define ANGLEENTRY_H

#include <QWidget>

class QLineEdit;
class QLabel;
class QString;

class AngleEntry : public QWidget
{
  Q_OBJECT

public:
  AngleEntry(const QString &labelStr, const QString &unitsStr, int maxw=0, 
	     double minData=0, double maxData=0, QWidget *parent=0);
  void setLabelText(const QString &);
  void setUnitsText(const QString &);
  void setText(const QString &); 
  QString value() const;
  QString angle() const; 
  double decimal() const;
  void clear();
  void setEntryColor(const QColor &);
  void setTextColor(const QColor &);
  void setLabelColor(const QColor &);
  void setUnitsColor(const QColor &);
  void setEntryFocusPolicy(Qt::FocusPolicy);
  void setFont(const QFont &);   
  void setNormal();
  void setEditing();
  void setChange();
  void setFault();
  void setNormalColors(const QColor &, const QColor &);
  void setChangeColors(const QColor &, const QColor &);
  void setFaultColors(const QColor &, const QColor &);
  void enable();
  void disable();
  void setInputMask(const QString &);
  void setCommit(const bool &);
  QString toSex(const double &, const int &, const bool &);
  double fromSex(const QString &);

signals:
  void dataReady(const QString &);
  void textReady(const QString &);

private slots:
  void getData();      //!< get entry data on editingFinished and emit the textReady() signal
  void commitData();   //!< get entry data on Return commit and emit the dataReady() signal
  void entryChanged(); //!< entry is changing but not yet committed, process string
 
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

  QString angleStr;   //!< angular value as a sexagesimal string
  double angleDec;    //!< angular value as a double-precision decimal number
  double angMajor;    //!< Major part (hours or degrees) of the angle
  double angMinutes;  //!< Minutes part of the angle
  double angSeconds;  //!< Seconds part of the angle

  bool validate;      //!< Validate the entry?
  double angMin;      //!< Minimum decimal angle value
  double angMax;      //!< Maximum decimal angle value

  bool hasEntry;      //!< Do we have any entry data?
  bool majEntry;      //!< Currently entering the major part of the sexagesimal number
  bool minEntry;      //!< Currently entering the minutes part of the sexagesimal number
  bool secEntry;      //!< Currently entering the seconds part of the sexagesimal number

  QColor bgNormal, fgNormal;
  QColor bgChange, fgChange;
  QColor bgFault, fgFault;

  bool commit;    //!< Entries must be committed by hitting Return or not.
};

#endif
