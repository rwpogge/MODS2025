#ifndef COMMVIEWER_H
#define COMMVIEWER_H

#include <QWidget>

class QString;
class QTextEdit;
class QLabel;
class QPushButton;
class QCheckBox;
class QDateTime;
class QLineEdit;

class CommViewer : public QWidget
{
  Q_OBJECT

public:
  CommViewer(const QString &logTitle, QWidget *parent=0);

public slots:
  void addMessage(const QString &);
  void showHeartBeats(const bool &);

private slots:
  void findNext();
  void findPrev();
  void setMatchCase();
  void setLineWrap();

private:
  QTextEdit   *commDisplay;    //!< QTextEdit for the communications viewer
  QLineEdit   *findEntry;      //!< line editor widget for the find feature
  QPushButton *nextButton;     //!< find next instance (forward)
  QPushButton *prevButton;     //!< find previous instance (backwards)
  QCheckBox   *matchCaseCheck; //!< checkbox to enable/disable case matching for search
  bool matchCase;              //!< Match case on search otherwise case insensitive
  QCheckBox   *lineWrapCheck;  //!< checkbox to enable/disable line wrapping
  bool wrapLines;              //!< Wrap lines - default is false
  QLabel      *notFound;       //!< label to set if the text is not found
  bool viewHeartBeats;         //!< If true, show heartbeat messages in the viewer
};

#endif
