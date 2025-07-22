#include <QtWidgets>

#include "commviewer.h"

// A widget to show the communications traffic in a readonly QTextEdit
// window, with color coding on message types

CommViewer::CommViewer(const QString &logTitle, QWidget *parent) 
  : QWidget(parent)
{
  // Default initializations

  wrapLines = true;
  matchCase = false;
  viewHeartBeats = true;

  // Main layout setup

  setContentsMargins(0,0,0,0);

  // Nice decor to outline the viewer widget

  QGroupBox *mainBox = new QGroupBox(logTitle,this);
  mainBox->setAlignment(Qt::AlignHCenter);

  // Big text widget, readonly, to show the comm traffic.  Wrap
  // lines by default.

  commDisplay = new QTextEdit(this);
  commDisplay->setReadOnly(true);

  // A search box

  QLabel *findLabel = new QLabel(tr("Find:"));

  findEntry = new QLineEdit(this);
  connect(findEntry,SIGNAL(textChanged(const QString &)),this,SLOT(findPrev()));
  int mxw = (int)(8*findEntry->minimumSizeHint().width());
  findEntry->setMaximumWidth(mxw);
  int hmx = (int)(0.8*findEntry->minimumSizeHint().height());
  findEntry->setMaximumHeight(hmx);
  findEntry->setFont(QFont("Helvetica",10,QFont::Normal));

  // Search forward (next occurance) 

  QSize iconSize(16,16);
  nextButton = new QPushButton(QIcon(":/images/downArrow.png"),tr("&Next"),this);
  //nextButton->setIconSize(iconSize);
  nextButton->setFlat(true);
  nextButton->setFont(QFont("Helvetica",10,QFont::Normal));
  connect(nextButton,SIGNAL(clicked()),this,SLOT(findNext()));

  // Search backwards (previous occurance)

  prevButton = new QPushButton(QIcon(":/images/upArrow.png"),tr("&Previous"),this);
  //prevButton->setIconSize(iconSize);
  prevButton->setFlat(true);
  prevButton->setFont(QFont("Helvetica",10,QFont::Normal));
  connect(prevButton,SIGNAL(clicked()),this,SLOT(findPrev()));

  // Check box - Match case (default is case-insensitive)

  matchCaseCheck = new QCheckBox(tr("Mat&ch case"),this);
  matchCaseCheck->setCheckState(Qt::Unchecked);
  matchCaseCheck->setFont(QFont("Helvetica",10,QFont::Normal));
  connect(matchCaseCheck,SIGNAL(stateChanged(int)),
	  this,SLOT(setMatchCase()));

  notFound = new QLabel("");
  notFound->setFont(QFont("Helvetica",10,QFont::Normal));

  // Check box - Wrap lines in the text display.  We wrap lines
  // by default.  User can elect to unwrap later.

  lineWrapCheck = new QCheckBox(tr("&Wrap Lines"),this);
  lineWrapCheck->setCheckState(Qt::Checked);
  lineWrapCheck->setFont(QFont("Helvetica",10,QFont::Normal));
  connect(lineWrapCheck,SIGNAL(stateChanged(int)),
	  this,SLOT(setLineWrap()));
  commDisplay->setLineWrapMode(QTextEdit::WidgetWidth);

  // Find-text layout

  QHBoxLayout *findLayout = new QHBoxLayout;
  findLayout->addWidget(findLabel);
  findLayout->addWidget(findEntry);
  findLayout->addWidget(nextButton);
  findLayout->addWidget(prevButton);
  findLayout->addWidget(matchCaseCheck);
  findLayout->addWidget(notFound);
  findLayout->addStretch();
  findLayout->addWidget(lineWrapCheck);
  findLayout->setContentsMargins(0,0,0,0);

  // Vertical layout with the log viewer and status layout over/under

  QVBoxLayout *commLayout = new QVBoxLayout;
  commLayout->addWidget(commDisplay);
  commLayout->addLayout(findLayout);
  mainBox->setLayout(commLayout);
  mainBox->setContentsMargins(0,20,0,0);

  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(mainBox);
  layout->setContentsMargins(0,0,0,0);

  setLayout(layout);  

}

// Append a communications string to the comm log viewer with the
// specified color and note the date/time it was logged.  This public
// slot is often connected to a logMessage() signal.

void CommViewer::addMessage(const QString &txt)
{

  if (txt.isEmpty()) return;

  // Work with a copy so we can clean it a little...

  QString commStr;
  commStr = txt;
  commStr.remove('\r');  // strip off any terminator characters

  // Count the number of words in the string

  QStringList wordList = commStr.split(" ");
  if (wordList.size() == 1 && !viewHeartBeats) return;

  // Put a timetag on the msgText string we use for display purposes

  QDateTime dateTime = QDateTime::currentDateTime().toUTC();

  QString msgText = QString("%1 %2").arg(dateTime.toString("[yyyy-MM-ddThh:mm:ss.zzz]")).arg(commStr);

  // Color code the entry by message type

  if (commStr.contains("ERROR:",Qt::CaseInsensitive))
    commDisplay->setTextColor(Qt::red);
  else if (commStr.contains("WARNING:",Qt::CaseInsensitive))
    commDisplay->setTextColor(QColor(255,165,0,255)); // orange
  else if (commStr.contains("FATAL:",Qt::CaseInsensitive) ||
	   commStr.contains("CRITCAL:",Qt::CaseInsensitive))
    commDisplay->setTextColor(Qt::magenta);
  else if (commStr.contains("STATUS:",Qt::CaseInsensitive))
    commDisplay->setTextColor(Qt::blue);
  else if (commStr.contains("EXEC:",Qt::CaseInsensitive))
    commDisplay->setTextColor(Qt::darkYellow);
  else if (commStr.contains("DONE:",Qt::CaseInsensitive))
    commDisplay->setTextColor(Qt::darkGreen);
  else if (commStr.contains("PONG",Qt::CaseInsensitive))
    commDisplay->setTextColor(Qt::blue);
  else
    commDisplay->setTextColor(Qt::black);

  // And put it into the communications traffic display, setting the
  // cursor so that the latest entry is visible in the slider box

  commDisplay->append(msgText);
  commDisplay->setTextColor(Qt::black); 
  commDisplay->moveCursor(QTextCursor::End);
  commDisplay->moveCursor(QTextCursor::StartOfLine);
}

// Set the match case boolean

void CommViewer::setMatchCase()
{
  if (matchCaseCheck->isChecked())
    matchCase = true;
  else
    matchCase = false;
}

// Find the next occurance of the search string

void CommViewer::findNext()
{
  QString findStr = findEntry->text();
  if (findStr.isEmpty()) {
    notFound->setText("");
    return;
  }

  bool textFound;

  // Look for it from the current cursor position forward

  if (matchCase)
    textFound = commDisplay->find(findStr,QTextDocument::FindCaseSensitively);
  else
    textFound = commDisplay->find(findStr);

  if (textFound)
    notFound->setText("");
  else
    notFound->setText("Phrase not found");

}

// Find the previous occurance of the search string.  This is invoked
// automatically in the search box, since we want to search *back*
// through the communications traffic log.

void CommViewer::findPrev()
{
  QString findStr = findEntry->text();
  if (findStr.isEmpty()) {
    notFound->setText("");
    return;
  }

  bool textFound;

  // Look for it backwards from the cursor position forward, which is
  // usually the bottom of the log

  if (matchCase)
    textFound = commDisplay->find(findStr,
				  (QTextDocument::FindCaseSensitively|QTextDocument::FindBackward));
  else
    textFound = commDisplay->find(findStr,QTextDocument::FindBackward);

  if (textFound)
    notFound->setText("");
  else
    notFound->setText("Phrase not found");

}

// Set line wrapping as per the line-wrap checkbox

void CommViewer::setLineWrap()
{
  if (lineWrapCheck->isChecked()) {
    commDisplay->setLineWrapMode(QTextEdit::WidgetWidth);
    wrapLines = true;
  }
  else {
    commDisplay->setLineWrapMode(QTextEdit::NoWrap);
    wrapLines = false;
  }
}

// Enable/disable whether the viewer shows heartbeat messages

void CommViewer::showHeartBeats(const bool &show)
{
  viewHeartBeats = show;
}
