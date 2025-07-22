#ifndef INSTUTILS_H
#define INSTUTILS_H

#include <QWidget>
#include <QHostAddress>

#include "mods.h" // Parameters of the MODS User Interface

// Qt widgets

class QString;

// Custom widgets

class IEBPanel;
class LLBPanel;
class ISISClient;

// InstUtils - Instrument Box-Level Utility Control Panel Class definition

class InstUtils : public QWidget
{
  Q_OBJECT

public:
  InstUtils(const int &myMODS, QWidget *parent = 0);

  void enable();  //!< Enable the instrument utility control panel
  void disable(); //!< Disable the instrument utility control panel

public slots:
  void update(); //!< Update the instrument utilities control panel
  void parse(const QString &, const int &, const QHash<QString,QString> &); //!< parse command replies

signals:
  void cmdReady(const QString &, const QString &); //!< command for execution
  void cmdWait(const QString &, const QString &, const int &); // command for exec/wait
  void statusMsg(const QString &, const QColor &);  //!< A status message

private slots:
  void passCommand(const QString &, const QString &); //!< Pass an immediate command to the parent
  void passCmdWait(const QString &, const QString &, const int &); // Pass a send-and-wait command to the parent
  void passStatus(const QString &, const QColor &); //!< Pass a message back to the parent

private:
  int modsID;    //!< MODS instrument ID (MODS1 or MODS2) of this control panel
  IEBPanel *iebBlue; //!< Blue Instrument Electronics Box utilities control panel
  IEBPanel *iebRed;  //!< Red Instrument Electronics Box utilities control panel
  LLBPanel *llb;     //!< Lamp/Laser Box utilities control panel
};

#endif
