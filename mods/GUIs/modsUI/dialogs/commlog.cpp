#include <QtWidgets>

#include "commlog.h"

//
// CommLog is a modeless dialog to show data-taking system message
// traffic in a read-only QTextEdit window.  Messages are color coded
// by IMPv2 message type, but other types of messages can be sent and
// coded by a specified color.  This dialog box is used mostly for
// showing data-taking system interprocess communcations logs, but we
// can also use it to display other types of dynamic logs, for example
// it is used by the envpanel for sensor alert logging and display.
//

CommLog::CommLog(const QString &logTitle, int width, int height,
		 QWidget *parent) 
  : QDialog(parent)
{

  // Big text widget, readonly, to show the message traffic

  commDisplay = new QTextEdit(this);
  commDisplay->setReadOnly(true);
  commDisplay->setLineWrapMode(QTextEdit::NoWrap);

  // Clear and Dismiss buttons

  clearButton = new QPushButton("Clear",this);
  connect(clearButton, SIGNAL(clicked()), this, SLOT(clearLog()));
  clearButton->setToolTip(tr("Clear the contents of this log window"));
  dismissButton = new QPushButton("Dismiss",this);
  connect(dismissButton, SIGNAL(clicked()), this, SLOT(dismissLog()));
  dismissButton->setToolTip(tr("Dismiss (Hide) this log window"));
  dismissButton->setDefault(true);  // Make sure this is the default button

  QHBoxLayout *buttonLayout = new QHBoxLayout;
  buttonLayout->addStretch();
  buttonLayout->addWidget(clearButton);
  buttonLayout->addWidget(dismissButton);

  // Vertical layout with the log viewer and status layout over/under

  QVBoxLayout *commLayout = new QVBoxLayout;
  commLayout->addWidget(commDisplay);
  commLayout->addLayout(buttonLayout);
  setLayout(commLayout);  

  // put up the decor, size it, and let it go

  setWindowTitle(logTitle);
  setMinimumSize(width,height);

  // Allow the user to resize the window 

  setSizeGripEnabled(true);

  firstShow = true;
  timeInMSec = true;

}

// Append a text message string to the log viewer with the specified
// color.  This function takes care of inserting a date/time tag for
// when it was logged.

void CommLog::append(const QString &txt, const QColor &color)
{

  if (txt.isEmpty()) return;

  // Work with a copy so we can clean it up a little...

  QString commStr;
  commStr = txt;
  commStr.remove('\r');  // strip off any terminator characters

  // Put a timetag on the msgText string we use for display purposes

  QDateTime dateTime = QDateTime::currentDateTime().toUTC();

  QString msgText;

  if (timeInMSec)
    msgText = QString("%1 %2").arg(dateTime.toString("[yyyy-MM-ddThh:mm:ss.zzz]")).arg(commStr);
  else
    msgText = QString("%1 %2").arg(dateTime.toString("[yyyy-MM-ddThh:mm:ss]")).arg(commStr);

  // Color code the entry by message type

  if (commStr.contains("ERROR:",Qt::CaseInsensitive))
    commDisplay->setTextColor(Qt::red);
  else if (commStr.contains("WARNING:",Qt::CaseInsensitive))
    commDisplay->setTextColor(QColor(255,165,0,255));  // orange
  else if (commStr.contains("FATAL:",Qt::CaseInsensitive) ||
	   commStr.contains("CRITCAL:",Qt::CaseInsensitive))
    commDisplay->setTextColor(Qt::magenta);
  else if (commStr.contains("STATUS:",Qt::CaseInsensitive))
    commDisplay->setTextColor(Qt::blue);
  else if (commStr.contains("EXEC:",Qt::CaseInsensitive))
    commDisplay->setTextColor(Qt::darkYellow);
  else if (commStr.contains("DONE:",Qt::CaseInsensitive))
    commDisplay->setTextColor(Qt::darkGreen);
  else
    commDisplay->setTextColor(color);

  // And put it into the log display, setting the cursor so that the
  // latest entry is visible in the slider box

  commDisplay->append(msgText);
  commDisplay->setTextColor(Qt::black); 
  commDisplay->moveCursor(QTextCursor::End);
  commDisplay->moveCursor(QTextCursor::StartOfLine);
}

// hideEvent for this dialog so we can save the log geometry when it
// is hidden

void CommLog::hideEvent(QHideEvent *)
{
  geom = geometry();
}

// showEvent for this dialog, so we can restore the comm log's
// geometry when it is shown again

void CommLog::showEvent(QShowEvent *)
{
  if (!firstShow)
    setGeometry(geom);
  firstShow = false;
}

// Clear the log window (can be used as a slot)

void CommLog::clearLog()
{
  commDisplay->clear();
}

// Dismiss (aka Hide) the log window - emits the logHidden() signal

void CommLog::dismissLog()
{
  hide();
  emit logHidden();
}

// Change timetag precision to milliseconds

void CommLog::millisec()
{
  timeInMSec = true;
}

// Change timetag precision to seconds

void CommLog::seconds()
{
  timeInMSec = false;
}
