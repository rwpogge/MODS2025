#ifndef UTILPANEL_H
#define UTILPANEL_H

#include <QWidget>
#include <QHostAddress>

#include "mods.h" // Parameters of the MODS User Interface

// Qt widgets

class QString;

// Custom widgets

class PowerSwitch;
class PowerBreaker;
class PowerButton;
class IUBPanel;
class ISISClient;

// UtilPanel Control Panel Class definition

class UtilPanel : public QWidget
{
  Q_OBJECT

public:
  UtilPanel(QWidget *parent = 0);

  void enableMODS1(const bool &);        //!< enable/disable MODS1 utility box controls
  void enableMODS2(const bool &);        //!< enable/disable MODS2 utility box controls
  void setSleep(const int &modsNum=0);   //!< Put a MODSn in SLEEP mode (0=both)
  void setOff(const int &modsNum=0);     //!< Turn a MODSn power off (0=both)

public slots:
  void update(); //!< Update the power control panel
  void parse(const int &, const QString &, const QString &, const int &, 
	     const QHash<QString,QString> &); //!< parse command replies

signals:
  void cmdReady(const QString &, const QString &); //!< command for execution
  void cmdWait(const QString &, const QString &, const int &); // command for exec/wait
  void statusMsg(const QString &, const QColor &);  //!< Emit a status message

private slots:
  void passCommand(const QString &, const QString &); //!< Pass an immediate command to the parent
  void passCmdWait(const QString &, const QString &, const int &); // Pass a send-and-wait command to the parent
  void passStatus(const QString &, const QColor &); //!< Pass a status message to the parent

private:
  bool useMODS1;       //!< MODS1 utility box controls enabled/disabled
  IUBPanel *mods1;     //!< MODS1 utility box control panel

  bool useMODS2;       //!< MODS2 utility box controls enabled/disabled
  IUBPanel *mods2;     //!< MODS2 utility box control panel

};

#endif
