#ifndef COMMLOG_H
#define COMMLOG_H

#include <QDialog>

class QString;
class QTextEdit;
class QLabel;
class QPushButton;
class QDateTime;

class CommLog : public QDialog
{
  Q_OBJECT

public:
  CommLog(const QString &logTitle, int width=640, int height=480, 
	  QWidget *parent=0);
  void append(const QString &, const QColor &);  //!< Append string with color to the log
  void millisec();  //!< Set the time-tag precision to milliseconds
  void seconds();   //!< Set the time-tag precision to seconds

protected:
  void hideEvent(QHideEvent *event); //!< hide the dialog
  void showEvent(QShowEvent *event); //!< show the dialog

public slots:
  void clearLog();   //!< clear dialog box text
  void dismissLog(); //!< dismiss the dialog box

signals:
  void logHidden(); 

private:
  QTextEdit   *commDisplay;   //!< text window for dialog box text
  QPushButton *clearButton;   //!< clear text button
  QPushButton *dismissButton; //!< dismiss (hide) dialog button;
  QRect geom;                 //!< geometry of the box (save changes on hide/show)
  bool firstShow;             //!< boolean: this is the first time the box has appeared
  bool timeInMSec;            //!< if true, time in msec, else time in sec
};

#endif
