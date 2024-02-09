#include <QtWidgets>
#include <QtNetwork>
#include <math.h>

#include "utilpanel.h"

#include "powerswitch.h"
#include "powerbreaker.h"
#include "iubpanel.h"

#include "isisclient.h"  // needed for msgType codes

UtilPanel::UtilPanel(QWidget *parent)
  : QWidget(parent)
{

  useMODS1 = true;  // allow disable later
  useMODS2 = true;

  //----------------------------------------------------------------
  //
  // Build the Main Utility control panel
  //

  // Create the MODS1 IUB control panel and connect its signals

  mods1 = new IUBPanel(MODS1,this);
  connect(mods1,SIGNAL(cmdReady(const QString &,const QString &)),
	  this,SLOT(passCommand(const QString &,const QString &)));
  connect(mods1,SIGNAL(cmdWait(const QString &,const QString &, const int &)),
	  this,SLOT(passCmdWait(const QString &,const QString &, const int &)));
  connect(mods1,SIGNAL(statusMsg(const QString &, const QColor &)),
	  this,SLOT(passStatus(const QString &, const QColor &)));

  // Create the MODS2 IUB control panel and connect its signals

  mods2 = new IUBPanel(MODS2,this);
  connect(mods2,SIGNAL(cmdReady(const QString &,const QString &)),
	  this,SLOT(passCommand(const QString &,const QString &)));
  connect(mods2,SIGNAL(cmdWait(const QString &,const QString &, const int &)),
	  this,SLOT(passCmdWait(const QString &,const QString &, const int &)));
  connect(mods2,SIGNAL(statusMsg(const QString &, const QColor &)),
	  this,SLOT(passStatus(const QString &, const QColor &)));

  // Build the control panel vertical layout

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addStretch();
  mainLayout->addWidget(mods1);
  mainLayout->addStretch();
  mainLayout->addWidget(mods2);
  mainLayout->addStretch();
  setLayout(mainLayout);

}

// Enable/Disable the MODS1 main utility controls

void UtilPanel::enableMODS1(const bool &useit)
{
  useMODS1 = useit;
  if (useMODS1)
    mods1->enable();
  else
    mods1->disable();
}

// Enable/Disable the MODS2 main utility controls

void UtilPanel::enableMODS2(const bool &useit)
{
  useMODS2 = useit;
  if (useMODS2)
    mods2->enable();
  else
    mods2->disable();
}

// SLOT: Pass a command from an instrument utility panel to the parent
// for transmission (parent is the ISIS client)

void UtilPanel::passCommand(const QString &host, const QString &cmd)
{
  if (cmd.isEmpty() || host.isEmpty()) return;
  emit cmdReady(host,cmd);
}

// SLOT: Pass a send-and-wait command from an instrument utility panel
// to the parent for transmission (parent is the ISIS client).  Which
// cmdXXX() signal is emitted depends on the timeout value.

void UtilPanel::passCmdWait(const QString &host, const QString &cmd, const int &timeout)
{
  if (cmd.isEmpty() || host.isEmpty()) return;
  if (timeout == 0)
    emit cmdReady(host,cmd);
  else
    emit cmdWait(host,cmd,timeout);
}

// SLOT: Pass a status message back to the parent

void UtilPanel::passStatus(const QString &msg, const QColor &col)
{
  if (msg.isEmpty()) return;
  emit statusMsg(msg,col);
}

// Convenience function: passes the incoming ISIS message to the
// utility panel parsers.

void UtilPanel::parse(const int &modsNum, const QString &remHost,
		      const QString &cmdStr, const int &msgType, 
		      const QHash<QString,QString> &keyDict)
{
  switch (modsNum) {
  case MODS1:
    mods1->parse(remHost,cmdStr,msgType,keyDict);
    break;
  case MODS2:
    mods2->parse(remHost,cmdStr,msgType,keyDict);
    break;
  }
}

// Turn off instrument power (convenience function). 

void UtilPanel::setOff(const int &modsNum)
{
  switch (modsNum) {
  case MODS1:
    if (useMODS1) mods1->allOff();
    break;
  case MODS2:
    if (useMODS2) mods2->allOff();
    break;
  }
}

// Put the instruments in sleep mode (convenience function).

void UtilPanel::setSleep(const int &modsNum)
{
  switch (modsNum) {
  case MODS1:
    if (useMODS1) mods1->sleep();
    break;

  case MODS2:
    if (useMODS2) mods2->sleep();
    break;
  }
}

// Update the control panel (convenience function)

void UtilPanel::update()
{
  if (useMODS1) mods1->update();
  if (useMODS2) mods2->update();
}
