#ifndef TARGETPANEL_H
#define TARGETPANEL_H

#include <QWidget>
#include <QHostAddress>
#include <QFile>

#include "mods.h" // Parameters of the MODS User Interface

// Qt widgets

class QString;
class QTextEdit;
class QLineEdit;
class QLabel;
class QPushButton;

// Custom widgets

class SelectEntry;
class NumberEntry;
class TextEntry;
class ActionButton;
class CatalogViewer;
class ISISClient;

// TargetPanel Class definition

class TargetPanel : public QWidget
{
  Q_OBJECT

public:
  TargetPanel(const int &myMODS, QWidget *parent = 0);
  void setLBTSide(const QString &);     //!< Set the LBT side for the instrument
  QString lbtSide();                    //!< Return the LBT side of the instrument
  void setTarget(const QString &, const QString &, const QString &, const QString &, const QString &, const QString &); //!< Set the target name and coordinates
  void setTargetName(const QString &);  //!< Set the target name
  QString targetName();                 //!< Return the target name
  void setTargetRA(const QString &);    //!< Set the target RA (sexagesimal hours)
  QString targetRA();                   //!< Return the target RA
  void setTargetDec(const QString &);   //!< Set the target Dec (sexagesimal degrees)
  QString targetDec();                  //!< Return the target Dec
  void setTargetPM(const double &, const double &, const double &); //!< Set the target proper motion
  void setRotator(const double &,const QString &);    //!< Set the rotator parameters
  void setRotatorPA(const double &);    //!< Set the rotator PA (decimal degrees)
  double rotatorPA();                   //!< Return the rotator PA
  void setRotatorMode(const QString &); //!< Set the rotator mode (one of IDLE|POSITION|PARALLACTIC)
  QString rotatorMode();                //!< Return the rotator mode
  void setGuideStar(const QString &, const QString &, const QString &, const QString &, 
		    const QString &, const QString &, const QString &, const QString &); //!< Set the guide star name and coordinates
  void setGuideName(const QString &);   //!< Set the guide star name
  QString guideName();                  //!< Return the guide star name
  void setGuideRA(const QString &);     //!< Set the guide star RA (sexagesimal hours)
  QString guideRA();                    //!< Return the guide star RA
  void setGuideDec(const QString &);    //!< Set the guide star Dec (sexagesimal degrees)
  QString guideDec();                   //!< Return the guide star Dec
  void setGuidePM(const double &, const double &, const double &); //!< Set the guide star proper motion
  void setGuideMag(const double &, const double &);  //!< Set the guide star magnitude and color

  void setPreset(const QString &);      //!< Set the preset mode (one of ACTIVE|TRACK|GUIDE|ACQUIRE)
  QString presetMode();                 //!< Return the preset mode

  void setOffset(const double &, const double &, const double &, const QString &, const QString &); // set the offset parameters
  void setOffsetRA(const double &);     //!< Set the offset delta RA (decimal arcsec)
  void setOffsetDec(const double &);    //!< Set the offset delta Dec (decimal arcsec)
  void setOffsetPA(const double &);     //!< Set the rotator offset (decimal degrees)
  void setOffsetMode(const QString &);  //!< Set the offset mode (REL|ABS)
  void setOffsetCoords(const QString &); //!< Set the offset coords (RADEC|DETXY)
  void reset();                         //!< Reset the preset and offset control panels and state variables

public slots:
  void parse(const QString &, const int &, const QHash<QString,QString> &); //!< parse command replies
  void cmdParse(const QString &);  //!< Parse tcs commands and extract information

signals:
  void cmdReady(const QString &, const QString &);  //!< command ready to send to ISIS
  void cmdWait(const QString &, const QString &, const int &); //!< command ready, send to ISIS and wait up to a timeout interval
  void logMessage(const QString &);                 //!< a message for the log
  
private slots:
  void sendCommand(const QString &); //!< Send a command w/preprocessing as needed
  void sendCmdWait(const QString &, const int &); // Send command and wait
  void clearPreset();            //!< Clear target preset entries
  void launchCatalogDialog(); //!< Launch the catalog dialog
  void setPresetMode(const QString &); //!< Set the requested preset mode
  void setTargName(const QString &);  //!< Set the requested target name
  void setTargRA(const QString &);  //!< Set the requested target RA
  void setTargDec(const QString &); //!< Set the requested target Dec
  void setRotMode(const QString &); //!< Set the requested rotator mode
  void setRotPA(const double &);    //!< Set the requested rotator PA
  void setGuiName(const QString &);   //!< Set the requested guide star name
  void setGuiRA(const QString &);   //!< Set the requested guide star RA
  void setGuiDec(const QString &);  //!< Set the requested guide star Dec
  void setOffMode(const QString &); //!< Set the requested offset mode (relative or absolute)
  void setOffCoords(const QString &); //!< Set the requested coordinate system (RADEC or SLIT)
  void setOffX(const double &);    //!< Set the requested RA/X offset (arcsec)
  void setOffY(const double &);    //!< Set the requested Dec/Y offset (arcsec)
  void setOffPA(const double &);    //!< Set the requested PA offset (degrees)
  void clearOffset();               //!< Clear the offset entry widget and restore defaults
  void sendPreset(); //!< Send a preset to the telescope
  void sendOffset(); //!< Send an offset to the telescope
  void absorbOffset();              //!< "Absorb" the offsets and update the pointing reference
  void setCatalog(const QString &); //!< Set target/guide-star from a catalog entry

private:
  int modsID;         //!< MODS instrument ID (MODS1 or MODS2)

  // Target and Guide Star "preset" entry widgets

  TextEntry *targNameEntry;      //!< Target name entry widget
  TextEntry *targRAEntry;        //!< Target RA entry widget
  TextEntry *targDecEntry;       //!< Target Dec entry widget
  
  TextEntry *guiNameEntry;       //!< Guide Star name entry widget
  TextEntry *guiRAEntry;         //!< Guide Star RA entry widget
  TextEntry *guiDecEntry;        //!< Guide Star Dec entry widget

  NumberEntry *rotPAEntry;       //!< Rotator PA entry widget
  SelectEntry *rotModeEntry;     //!< Rotator mode selection widget
  
  SelectEntry *presetModeEntry;  //!< Preset mode selection widget
  ActionButton *presetButton;    //!< Button to send the preset
  ActionButton *clrPresetButton; //!< Button to clear the telescope preset entries

  ActionButton *catalogButton; //!< Button to launch the target catalog dialog
  CatalogViewer *catDialog;    //!< Target catalog dialog widget

  // Telescope offset entry widgets

  NumberEntry *offRAEntry;       //!< offset RA entry widget
  NumberEntry *offDecEntry;      //!< offset Dec entry widget
  NumberEntry *offPAEntry;       //!< offset PA entry widget
  SelectEntry *offModeEntry;     //!< offset mode selection widget
  SelectEntry *offCoordsEntry;   //!< offset coordinate system selection widget
  ActionButton *offsetButton;    //!< Button to send the offset
  ActionButton *clrOffsetButton; //!< Button to clear the telescope offset entries
  ActionButton *absorbButton;    //!< Button to "absorb" offset (Update Pointing Reference)

  // Private variables

  QString targName;    //!< Target name
  QString targRA;      //!< Target RA (sexagesimal hours)
  QString targDec;     //!< Target Dec (sexagesimal degrees)
  double  targPMRA;    //!< Target RA proper motion (mas/yr)
  double  targPMDec;   //!< Target Dec proper motion (mas/yr)
  QString targEquinox; //!< Target Equinox (J2000 or B1950)
  double  targEpoch;   //!< Target Epoch (decimal years)
  double  rotPA;       //!< Rotator PA (decimal degrees)
  
  QString guiName;    //!< Guide Star name
  QString guiRA;      //!< Guide Star RA (sexagesimal hours)
  QString guiDec;     //!< Guide Star Dec (sexagesimal degrees)
  double  guiPMRA;    //!< Guide Star RA proper motion (mas/yr)
  double  guiPMDec;   //!< Guide Star Dec proper motion (mas/yr)
  QString guiEquinox; //!< Guide Star Equinox (J2000 or B1950)
  double  guiEpoch;   //!< Guide Star Epoch (decimal years)
  double  guiMag;     //!< Guide Star magnitude
  double  guiColor;   //!< Guide Star color

  QString rotMode;    //!< Rotator Mode requested
  QString preMode;    //!< Preset mode requested

  double dX;          //!< X/RA offset in arcsec
  double dY;          //!< Y/Dec offset in arcsec
  double dPA;         //!< Rotator offset in decimal degrees
  QString offMode;    //!< Offset mode requested
  QString offCoords;  //!< Offset coordinate system

  // Authorization status info

  bool authorized;         //!< Are we authorized to send active commands?
  QString leftInstID;      //!< ID of the instrument authorized on the left side
  QString leftFocStation;  //!< Authorized focal station on the left side
  QString rightInstID;     //!< ID of the instrument authorized on the right side
  QString rightFocStation; //!< Authorized focal station on the right side
  
  // Preset and Offset Status Info

  bool presetPending;      //!< Is a preset completion pending?
  bool offsetPending;      //!< Is an offset completion pending?
};

#endif
