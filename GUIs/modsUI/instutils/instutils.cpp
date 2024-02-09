#include <QtWidgets>
#include <QtNetwork>
#include <math.h>

#include "instutils.h"
#include "iebpanel.h"
#include "llbpanel.h"

#include "isisclient.h"  // needed for msgType codes

InstUtils::InstUtils(const int &myMODS, QWidget *parent)
  : QWidget(parent)
{
  modsID = myMODS;

  //----------------------------------------------------------------
  //
  // Build the Main Power control panel
  //

  // Create the Blue IEB utility control panel and connect its signals

  iebBlue = new IEBPanel(modsID,"B",this);
  connect(iebBlue,SIGNAL(cmdReady(const QString &,const QString &)),
	  this,SLOT(passCommand(const QString &,const QString &)));
  connect(iebBlue,SIGNAL(cmdWait(const QString &,const QString &, const int &)),
	  this,SLOT(passCmdWait(const QString &,const QString &, const int &)));
  connect(iebBlue,SIGNAL(statusMsg(const QString &,const QColor &)),
	  this,SLOT(passStatus(const QString &,const QColor &)));

  // Create the Red IEB utility control panel and connect its signals

  iebRed = new IEBPanel(modsID,"R",this);
  connect(iebRed,SIGNAL(cmdReady(const QString &,const QString &)),
	  this,SLOT(passCommand(const QString &,const QString &)));
  connect(iebRed,SIGNAL(cmdWait(const QString &,const QString &, const int &)),
	  this,SLOT(passCmdWait(const QString &,const QString &, const int &)));
  connect(iebRed,SIGNAL(statusMsg(const QString &,const QColor &)),
	  this,SLOT(passStatus(const QString &,const QColor &)));

  // Create the LLB utility control panel and connect its signals 

  llb = new LLBPanel(modsID,this);
  connect(llb,SIGNAL(cmdReady(const QString &,const QString &)),
	  this,SLOT(passCommand(const QString &,const QString &)));
  connect(llb,SIGNAL(cmdWait(const QString &,const QString &, const int &)),
	  this,SLOT(passCmdWait(const QString &,const QString &, const int &)));
  connect(llb,SIGNAL(statusMsg(const QString &,const QColor &)),
	  this,SLOT(passStatus(const QString &,const QColor &)));

  // Build the control panel vertical layout

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addStretch();
  mainLayout->addWidget(iebBlue);
  mainLayout->addStretch();
  mainLayout->addWidget(iebRed);
  mainLayout->addStretch();
  mainLayout->addWidget(llb);
  mainLayout->addStretch();
  setLayout(mainLayout);

}

// Enable the control panel

void InstUtils::enable()
{
  setDisabled(false);
}

// Disable the control panel

void InstUtils::disable()
{
  setDisabled(true);
}

// SLOT: Pass a command from a box control panel to the parent for
// transmission (parent is the ISIS client)

void InstUtils::passCommand(const QString &host, const QString &cmd)
{
  if (cmd.isEmpty() || host.isEmpty()) return;
  emit cmdReady(host,cmd);
}

// SLOT: Pass a send-and-wait command from a box control panel to the
// parent for transmission (parent is the ISIS client).  Which
// cmdXXX() signal is emitted depends on the timeout value.

void InstUtils::passCmdWait(const QString &host, const QString &cmd, const int &timeout)
{
  if (cmd.isEmpty() || host.isEmpty()) return;
  if (timeout == 0)
    emit cmdReady(host,cmd);
  else
    emit cmdWait(host,cmd,timeout);
}

// SLOT: Pass a status message to the parent

void InstUtils::passStatus(const QString &msg, const QColor &col)
{
  if (msg.isEmpty()) return;
  emit statusMsg(msg,col);
}

// Convenience function: pass the incoming ISIS message to the
// individual box control panel parsers.

void InstUtils::parse(const QString &cmdStr, 
		      const int &msgType, 
		      const QHash<QString,QString> &keyDict)
{
  iebBlue->parse(cmdStr,msgType,keyDict);
  iebRed->parse(cmdStr,msgType,keyDict);
  llb->parse(cmdStr,msgType,keyDict);
}

// Update the control panels (convenience function)

void InstUtils::update()
{
  iebBlue->update();
  iebRed->update();
  llb->update();
}

