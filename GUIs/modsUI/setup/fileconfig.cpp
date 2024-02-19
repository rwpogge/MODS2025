#include <QtWidgets>
#include <QtNetwork>
#include <math.h>

#include "fileconfig.h"
#include "actionbutton.h"
#include "textentry.h"
#include "valuedisplay.h"

#include "isisclient.h"  // needed for msgType codes

FileConfig::FileConfig(const int &myMODS, QWidget *parent)
  : QWidget(parent)
{
  modsID = myMODS;

  // Widget initialization

  bluePrefix = QString("mods%1b").arg(modsID+1);
  redPrefix  = QString("mods%1r").arg(modsID+1);
  haveBlueName = false;
  haveRedName = false;

  // Main elements are in a form layout

  QFormLayout *fileFormLayout = new QFormLayout;
  fileFormLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
  fileFormLayout->setFormAlignment(Qt::AlignCenter);  
  fileFormLayout->setLabelAlignment(Qt::AlignRight);

  // RootName and get-Date button

  QHBoxLayout *rootNameLayout = new QHBoxLayout;

  rootNameEntry = new TextEntry("","",8,this);
  connect(rootNameEntry,SIGNAL(dataReady(const QString &)),
	  this,SLOT(setRootName(const QString &)));

  getDateButton = new ActionButton(tr("Get Date"),this);
  getDateButton->setFont(QFont("Helvetica",appFontSize,QFont::Normal));
  connect(getDateButton,SIGNAL(clicked()),this,SLOT(getDateTag()));
  int bwid = (int)(getDateButton->minimumSizeHint().width());

  rootNameLayout->addWidget(rootNameEntry);
  rootNameLayout->addStretch();
  rootNameLayout->addWidget(getDateButton);
  fileFormLayout->addRow(tr("Root Name:"),rootNameLayout);

  // Blue CCD filename entry

  QHBoxLayout *blueNameLayout = new QHBoxLayout;
  blueRootDisplay = new ValueDisplay(QString("%1.").arg(bluePrefix),".",this);
  blueNumEntry = new TextEntry("",".fits",4,this);
  blueNumEntry->setCommit(false);
  connect(blueNumEntry,SIGNAL(textReady(const QString &)),
	  this,SLOT(setBlueNum(const QString &)));

  refreshButton = new ActionButton(tr("Refresh"),this);
  refreshButton->setFont(QFont("Helvetica",appFontSize,QFont::Normal));
  connect(refreshButton,SIGNAL(clicked()),this,SLOT(update()));
  bwid = qMax((int)(refreshButton->minimumSizeHint().width()),bwid);

  blueNameLayout->addWidget(blueRootDisplay);
  blueNameLayout->addWidget(blueNumEntry);
  blueNameLayout->addStretch();
  blueNameLayout->addWidget(refreshButton);
  fileFormLayout->addRow(tr("Blue CCD:"),blueNameLayout);

  // Red CCD filename entry

  QHBoxLayout *redNameLayout = new QHBoxLayout;
  redRootDisplay = new ValueDisplay(QString("%1.").arg(redPrefix),".",this);
  redNumEntry = new TextEntry("",".fits",4,this);
  redNumEntry->setCommit(false);
  connect(redNumEntry,SIGNAL(textReady(const QString &)),
	  this,SLOT(setRedNum(const QString &)));

  applyButton = new ActionButton(tr("Apply"),this);
  applyButton->setFont(QFont("Helvetica",appFontSize,QFont::Normal));
  connect(applyButton,SIGNAL(clicked()),this,SLOT(setFileNames()));
  bwid = qMax((int)(applyButton->minimumSizeHint().width()),bwid);

  redNameLayout->addWidget(redRootDisplay);
  redNameLayout->addWidget(redNumEntry);
  redNameLayout->addStretch();
  redNameLayout->addWidget(applyButton);

  fileFormLayout->addRow(tr("Red CCD:"),redNameLayout);

  // Make all the action buttons the same width

  refreshButton->setMinimumWidth(bwid);
  applyButton->setMinimumWidth(bwid);
  getDateButton->setMinimumWidth(bwid);

  // Build the group box with the filename form

  QGroupBox *mainGroupBox = new QGroupBox(tr("MODS%1").arg(modsID+1));

  QVBoxLayout *fileLayout = new QVBoxLayout;
  fileLayout->addLayout(fileFormLayout);
  mainGroupBox->setLayout(fileLayout);

  // Main widget layout

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(mainGroupBox);
  mainLayout->setContentsMargins(0,0,0,0);
  setLayout(mainLayout);

  // Put in some placeholders
  
  blueRootDisplay->setText("yyyymmdd");
  blueNumEntry->setText("####");
  redRootDisplay->setText("yyyymmdd");
  redNumEntry->setText("####");
  rootNameEntry->setText("yyyymmdd");
}

// Public Methods and Slots

// Clear the form and query for the current file info

void FileConfig::update()
{
  haveBlueName = false;
  haveRedName = false;
  emit cmdReady(modsBCHost[modsID],"FILENAME");
  emit cmdReady(modsRCHost[modsID],"FILENAME");
  return;
}

// Return the most recent error string

QString FileConfig::error() const
{
  return errorStr;
}

//---------------------------------------------------------------------------
// 
// parse - process replies from an IMPv2 client
//
// Parse messages, picking off those keyword=value pairs we care about
// and ignorning the rest
//
// This slot is meant to be invoked by the parent that operates the
// ISISClient instance.
//

void FileConfig::parse(const QString &remHost, const QString &cmdStr, 
		       const int &msgType, const QHash<QString,QString> &keyDict)
{
  // TODO: Unused flags. Commented out on 2-19-24
  // bool cmdDone = false;   // Flag: message is a command completion
  // bool cmdFault = false;  // Flag: command completed with a fault
  bool isRed;
  bool ok;

  QString msgStr = cmdStr;
  msgStr = msgStr.simplified(); // TODO: Evaluate this line. Why is it here? 

  if (msgStr.isEmpty()) return; // Empty messages are ignored
  

  // TODO: Unused flags. Commented out on 2-19-24
  // Our response depends on the type of message we received. 
  switch (msgType) {
  case ISISClient::ERROR:
  //   cmdDone = true;
  //   cmdFault = true;
    break;

  case ISISClient::WARNING:
  //   cmdDone = true;
  //   cmdFault = false;
    break;

  case ISISClient::FATAL:
  //   cmdDone = true;
  //   cmdFault = true;
    break;

  case ISISClient::DONE:
  //   cmdDone = true;
  //   cmdFault = false;
    break;

  case ISISClient::STATUS:
  //   cmdDone = false;
  //   cmdFault = false;
    break;

  default:
    // Nothing to be done here, return (we don't handle REQ: or EXEC: requests)
    return;
    break;
  }

  // The first word in the message body is the command that
  // generated this response.  We only parse FILENAME, NEXTFILE and EXPNUM returns

  QString cmdWord = msgStr.section(" ",0,0);

  if (cmdWord.compare("FILENAME",Qt::CaseInsensitive)!=0 &&
      cmdWord.compare("NEXTFILE",Qt::CaseInsensitive)!=0 &&
      cmdWord.compare("EXPNUM",Qt::CaseInsensitive)!=0) return;

  // Who did we get this from?  We only respond to our blue or red host

  if (remHost.compare(modsBCHost[modsID],Qt::CaseInsensitive)==0)
    isRed = false;
  else if (remHost.compare(modsRCHost[modsID],Qt::CaseInsensitive)==0)
    isRed = true;
  else
    return;

  // Were we passed any keyword=value pairs to interpret?

  int numKeys = keyDict.size();

  if (numKeys > 0) { // use Qt's built-in Java-style iterator to traverse the hash table
    QHashIterator<QString,QString> i(keyDict);
    while (i.hasNext()) {
      i.next();
      QString keyStr = i.key();
      QString valStr = i.value();

      // What is the current filename?

      bool haveFile = false;
      QString fileName;

      // FILENAME - next FITS file name w/o path

      if (keyStr.compare("FILENAME",Qt::CaseInsensitive)==0) {
	fileName = valStr;
	haveFile = true;
      }

      // NEXTFILE - next FITS file name with path

      else if (keyStr.compare("NEXTFILE",Qt::CaseInsensitive)==0) {
	QFileInfo fullName(valStr);
        fileName = fullName.fileName();
	haveFile = true;
      }
      else
	haveFile = false;

      // Do we have a filename string to interpret?

      if (haveFile) {
	size_t BufSize = 10;
	char buf[BufSize];
	QString ctrStr;
	QStringList fileBits = fileName.split(".");
	if (fileBits.count() == 3) {  // 3 components, we have something "normal"
	  int ctr = fileBits.at(2).toInt(&ok,10);
	  if (ok) {
	    ::snprintf(buf,BufSize,"%04d",ctr);
	    ctrStr = QString::fromUtf8(buf);
	  }
	  if (isRed) {
	    redRootName = fileBits.at(1);
	    redRootDisplay->setText(redRootName);
	    if (ok) {
	      redCounter = ctr;
	      redFileNum = ctrStr;
	      redNumEntry->setText(redFileNum);
	      haveRedName = true;
	    }
	  }
	  else {
	    blueRootName = fileBits.at(1);
	    blueRootDisplay->setText(blueRootName);
	    if (ok) {
	      blueCounter = ctr;
	      blueFileNum = ctrStr;
	      blueNumEntry->setText(blueFileNum);
	      haveBlueName = true;
	    }
	  }
	}
	else if (fileBits.count() == 2) { // 2-part name, wing it
	  int ctr = fileBits.at(1).toInt(&ok,10);
	  if (ok) {
	    ::snprintf(buf,BufSize,"%04d",ctr);
	    ctrStr = QString::fromUtf8(buf);
	  }
	  if (isRed) {
	    redRootName = fileBits.at(0);
	    redRootDisplay->setText(redRootName);
	    if (ok) {
	      redCounter = ctr;
	      redFileNum = ctrStr;
	      redNumEntry->setText(redFileNum);
	      haveRedName = true;
	    }
	  }
	  else {
	    blueRootName = fileBits.at(0);
	    blueRootDisplay->setText(blueRootName);
	    if (ok) {
	      blueCounter = ctr;
	      blueFileNum = ctrStr;
	      blueNumEntry->setText(blueFileNum);
	      haveBlueName = true;
	    }
	  }
	}
	else { // uninterpretable mess (rare), clear the entry boxes to signal badness
	  if (isRed) {
	    redRootDisplay->clear();
	    redNumEntry->clear();
	    haveRedName = false;
	  }
	  else {
	    blueRootDisplay->clear();
	    blueNumEntry->clear();
	    haveBlueName = false;
	  }
	}
      }
    }
  }
  
  // Other things to do?  Do them here...

}

// Private Slots

// Generate a root name by reading the UTC system clock, and then fill
// In the various entry widgets as needed

void FileConfig::getDateTag()
{
  QDateTime utcDate = QDateTime::currentDateTime().toUTC();

  rootName = utcDate.toString("yyyyMMdd");
  rootNameEntry->setText(rootName);

  blueRootName = rootName;
  blueCounter = 1;
  blueRootDisplay->setText(blueRootName);
  blueNumEntry->setText("0001");
  blueFileNum = "0001";
  haveBlueName = true;

  redRootName = rootName;
  redCounter = 1;
  redRootDisplay->setText(redRootName);
  redNumEntry->setText("0001");
  redFileNum = "0001";
  haveRedName = true;

}

// Set the file rootname.  On setting a new rootname, reset the file counters

void FileConfig::setRootName(const QString &newRoot)
{
  if (newRoot.isEmpty()) return;
  rootName = newRoot;
  rootNameEntry->setText(rootName);

  blueRootName = rootName;
  blueCounter = 1;
  blueRootDisplay->setText(blueRootName);
  blueNumEntry->setText("0001");
  blueFileNum = "0001";
  haveBlueName = true;

  redRootName = rootName;
  redCounter = 1;
  redRootDisplay->setText(redRootName);
  redNumEntry->setText("0001");
  redFileNum = "0001";
  haveRedName = true;
}

// Set the blue channel filename counter

void FileConfig::setBlueNum(const QString &numStr)
{
  bool ok;
  if (numStr.isEmpty()) {
    blueCounter = 0;
    blueNumEntry->setChange();
    haveBlueName = false;
    return;
  }
  int ctr = numStr.toInt(&ok,10);
  QString ctrStr;
  if (ok) {
    size_t BufSize = 10;
    char buf[BufSize];
    ::snprintf(buf,BufSize,"%04d",ctr);
    ctrStr = QString::fromUtf8(buf);
  }
  if (ok && (ctr > 0 && ctr < MODS_MAX_FILENUM)) {
    blueCounter = ctr;
    blueFileNum = ctrStr;
    blueNumEntry->setText(blueFileNum);
    blueNumEntry->setNormal();
    haveBlueName = true;
  }
  else {
    blueNumEntry->setFault();
    haveBlueName = false;
  }
}

// Set the red channel filename counter

void FileConfig::setRedNum(const QString &numStr)
{
  bool ok;
  if (numStr.isEmpty()) {
    redCounter = 0;
    redNumEntry->setChange();
    haveRedName = false;
    return;
  }
  int ctr = numStr.toInt(&ok,10);
  QString ctrStr;
  if (ok) {
    size_t BufSize = 10;
    char buf[BufSize];
    ::snprintf(buf,BufSize,"%04d",ctr);
    ctrStr = QString::fromUtf8(buf);
  }
  if (ok && (ctr > 0 && ctr < MODS_MAX_FILENUM)) {
    redCounter = ctr;
    redFileNum = ctrStr;
    redNumEntry->setText(redFileNum);
    redNumEntry->setNormal();
    haveRedName = true;
  }
  else {
    redNumEntry->setFault();
    haveRedName = false;
  }
}

// Send the the filenames to the respective CCD controller hosts

void FileConfig::setFileNames()
{
  if (haveBlueName && haveRedName) {
    QString blueCmd = QString("FILENAME %1.%2.%3"
			      ).arg(bluePrefix).arg(blueRootName).arg(blueFileNum);
    emit cmdReady(modsBCHost[modsID],blueCmd);

    QString redCmd = QString("FILENAME %1.%2.%3"
			     ).arg(redPrefix).arg(redRootName).arg(redFileNum);
    emit cmdReady(modsRCHost[modsID],redCmd);
  }
  else {
    QString titleStr = tr("MODS%1 Image Filename Entry Error").arg(modsID+1);
    QString msgStr = tr("One or both image filename entries is either invalid or blank\n\n"
			"Please correct and try again\n");
    QMessageBox::critical(this,titleStr,msgStr,QMessageBox::Close);
  }
}

// Enable/Disable the widget

void FileConfig::enable()
{
  setEnabled(true);
}

void FileConfig::disable()
{
  setEnabled(false);
}

