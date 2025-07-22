#ifndef MODSCHANNEL_H
#define MODSCHANNEL_H

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
class QUdpSocket;
class QDateTime;
class QCheckBox;
class QTimer;
class QProgressBar;

// Custom widgets

class BiStateMechanism;
class IndexedMechanism;
class LinearMechanism;

class BiStateControl;
class IndexedSelector;
class LinearControl;
class ToggleSwitch;

class WarmUpLamp;

class CommLog;
class TextEntry;
class IntEntry;
class DoubleEntry;
class SelectEntry;
class TextHistEntry;
class TextDisplay;
class ValueDisplay;
class NumberEntry;
class LightButton;
class ActionButton;
class LED;
class ISISClient;

// MODSChannel Class definition

class MODSChannel : public QWidget
{
    Q_OBJECT

public:
  MODSChannel(const int &myMODS, const int &myChannel, QWidget *parent = 0);
  
  // Enums with various numerical codes we need

  enum channelNum {
    Blue,        //!< MODS Blue Channel
    Red          //!< MODS Red Channel
  };

  enum imageType {
    Object,      //!< Object Frame
    Bias,        //!< Bias Frame
    Dark,        //!< Dark Frame
    Flat,        //!< Flat Field
    Sky,         //!< Sky Flat Field
    Comp,        //!< Comparison Lamp 
    Std,         //!< Standard Star
    Focus        //!< Focus Plate
  };

  enum exposureStatus {
    expInit,        //!< Exposure Initializing
    expErase,       //!< Erasing CCD
    expIntegrating, //!< CCD Integrating
    expReadout,     //!< CCD Reading Out
    expPause,       //!< CCD Integration Paused
    expWrite,       //!< CCD Image being written
    expDone,        //!< Exposure done
    expIdle         //!< CCD system is idle
  };

  // Public methods
  
  void resetExp();  //!< Reset the exposure controls
  QString error();  //!< Return the last error string
  QString channel(); //!< Channel prefix (B or R) of this channel
  void setFilterName(QStringList *); //!< Set the camera filter name list
  QStringList filterName();          //!< Get the camera filter name list
  void setGratingName(QStringList *); //!< Set the grating name list
  QStringList gratingName();          //!< Get the grating name list
  bool execGo();                      //!< Execute an exposure go
  void execStop();                    //!< Execute an exposure stop
  void execAbort();                   //!< Execute an exposure abort
  int getNumExp();                    //!< Return the number of exposures to acquire
  void loadROITable(const QString &);  //!< Load a CCD readout ROI table
  void loadTiltTable(const QString &, const int &);  //!< Load a grating preset tilt table
  bool setCCDROI(const QString &);     //!< Set the CCD Region of Interest by name or full spec
  void setChange(const QString &);     //!< Set the change state of a mechanism widget 
  void loadConfigTable(const QString &); //!< Load the channel preset configuration table
  int  setConfig(const QString &);     //!< Set a channel preset configuration by name
  bool validConfig(const QString &);   //!< Test to see if a channel preset configuration is valid
  bool acquiring();                    //!< Query the image acquisition state
  QString expStatus();                 //!< String describing the current exposure status
  void setIMCS(const bool &);          //!< Enable/disable use of the IMCS during exposures
  bool usingIMCS();                    //!< Is the IMCS being used?
  void setIMCSExpTime(const double &); //!< Set the IMCS minimum exposure time threshold
  void imcsLockOn(const int &);        //!< Initiate IMCS lock-on request
  void imcsCancelLock();               //!< Cancel a pending IMCS lock-on request
  void forceExpDone();                 //!< Force exposure completion in the channel

public slots:
  void parse(const QString &, const int &, const QHash<QString,QString> &); //!< parse command replies
  void setImgType(const QString &);    //!< Set the image type for the current object name (oddity of the ICs)
  void setObject(const QString &);     //!< Set the object name for the current image type (oddity of the ICs)
  void setExpTime(const double &);     //!< Set the exposure time in seconds
  void setNumExp(const int &);         //!< Set the number of exposures to acquire
  void setCCDBinning(const int &,const int &); //!< Set the CCD binning factors
  void setCCDXBin(const int &);        //!< Set the CCD detector x-axis binning factor
  void setCCDYBin(const int &);        //!< Set the CCD detector y-axis binning factor
  void update();                       //!< Update/Refresh the channel control panel
  void pauseExposure();                //!< Pause/Resume an exposure in progress

signals:
  void cmdReady(const QString &, const QString &);  //!< command ready to send to ISIS 
  void cmdWait(const QString &, const QString &, const int &); //!< command ready, send to ISIS and wait up to a timeout interval
  void logMessage(const QString &);  //!< Signal with a message to log
  void doCommand(const QString &);   //!< Signal a command to execute by the parent
  void imcsLocked(const QString &,const bool &); //!< IMCS lock state, string is channel ID, bool is lock state

private slots:
  void sendIECommand(const QString &);  //!< Send a command to the IE Host
  void sendICCommand(const QString &);  //!< Send a command to the IC Host
  void sendCmdWait(const QString &, const QString &, const int &); // Send command and wait
  void launchCollimatorDialog();        //!< Launch the collimator mirror control panel dialog
  void ccdXBin(const QString &);        //!< Process the xBinEntry selection
  void ccdYBin(const QString &);        //!< Process the xBinEntry selection
  void ccdROI(const QString &);         //!< Process the roiEntry selection
  void ccdSpeed(const QString &);       //!< Process the speedEntry selection
  void setGratingTilt(const QString &); //!< Process the grtiltEntry selection
  void doExposure();                    //!< Start/Abort an exposure
  void stopExposure();                  //!< Stop an exposure and read it out
  void expStart();                      //!< Start exposure countdown timer
  void expUpdate();                     //!< Update the exposure countdown progress
  void loadTiltMenu(const int &);       //!< Build the grating tilt selector menu
  void setCurrentTilt(const int &,const int &); //!< Set the grating tilt menu to the corresponding item
  void reqImagingMode();               //!< Request imaging mode (requires a subsequent commit or clear)
  void reqGratingMode();               //!< Request grating mode (requires a subsequent commit or clear)
  void reqPrismMode();                 //!< Request prism mode (requires a subsequent commit or clear)
  // void reqHiResMode();                 //!< Request hires grating mode (requires a subsequent commit or clear)
  void commitConfig();                 //!< Commit the selected channel configuration
  void clearConfig();                  //!< Clear an uncommitted channel configuration
  void enableCCDControls(const bool &); //!< Enable/disable CCD setup controls
  void setUseIMCS();                   //!< Enable/disable using the IMCS (checkbox slot)
  void queryIMCSLock();                //!< Query the IMCS lock status
  void doIMCSLock();                   //!< Initiate IMCS lock-on from the control-panel button

private:
  int modsID;          //!< MODS Instrument ID (MODS1 or MODS2)
  QString channelID;   //!< Spectrograph Channel ID prefix (B or R)
  QString channelName; //!< Long name of the Channel

  QString icHostID;    //!< ISIS client ID of the CCD Controller (IC) host
  QString dmHostID;    //!< ISIS client ID of the data manager (DM or CB) host

  QString errorStr;    //!< Error strings for error conditions

  // Preset configurations

  ActionButton  *imagingButton;   //!< Imaging mode button
  ActionButton  *gratingButton;   //!< Grating mode button
  ActionButton  *prismButton;     //!< Prism mode button
  // ActionButton  *hiresButton;     //!< HiRes mode button [currently disabled]

  QHash<QString,QStringList> configTable; //!< Preset Configuration Table

  QString reqConfig;              //!< Requested configuration to commit
  ActionButton  *commitButton;    //!< Commit selected configuration
  ActionButton  *clearButton;     //!< Clear uncommitted configuration request

  ActionButton  *resetButton;     //!< Reset the channel control panel

  // Channel Grating Select/Tilt Widgets

  IndexedMechanism *grating;         //!< Grating select turret mechanism 
  //IndexedSelector  *gratingControl;  //!< Grating select control widget
  TextDisplay *gratingDisplay;       //!< Grating Selection Display
  
  LinearMechanism  *grtilt;          //!< Grating Tilt Mechanism
  //LinearControl    *grtiltControl;   //!< Grating Tilt control widget
  //SelectEntry *grtiltEntry;          //!< Grating Tilt preset position selector
  TextDisplay *grtiltDisplay;    //!< Grating Tilt preset position display
  QStringList *grtiltTable[2];       //!< Grating Tilt Preset Tables for GRTILT1 and GRTILT2
  int gratingTilt[2];                //!< Grating Tilt values for GRTILT1 and GRTILT2
  
  int nominalTTFA[4];                //!< Nominal Collimator TTF A for the grating turret position
  int nominalTTFB[4];                //!< Nominal Collimator TTF B for the grating turret position
  int nominalTTFC[4];                //!< Nominal Collimator TTF C for the grating turret position
  
  // Channel Camera Mechanism (Filter/Focus/Shutter) Widgets
  
  IndexedMechanism *filter;          //!< Camera filter wheel
  IndexedSelector  *filterControl;   //!< Camera filter wheel control widget
  
  LinearMechanism  *camfoc;          //!< Camera focus mechanism
  //LinearControl    *camfocControl;   //!< Camera focus control widget
  TextDisplay      *camfocDisplay;   //!< CAmera focus display widget

  BiStateMechanism *shutter;         //!< Camera shutter mechanism
  ValueDisplay     *shutterDisplay;  //!< Camera shutter display widget
  ValueDisplay     *expNumDisplay;   //!< Current exposure number display widget
  
  // Channel Collimator Mechanism (TTFA/B/C) Widgets
  
  // LinearMechanism  *colfoc;          //!< Collimator focus mechanism
  // LinearControl    *colfocControl;   //!< Collimator focus control widget
  
  LinearMechanism  *colttfa;      //!< Collimator Tip/Tilt/Focus Actuator A
  LinearMechanism  *colttfb;      //!< Collimator Tip/Tilt/Focus Actuator B
  LinearMechanism  *colttfc;      //!< Collimator Tip/Tilt/Focus Actuator C
  TextDisplay     *ttfaDisplay;  //!< TTFA position display
  TextDisplay     *ttfbDisplay;  //!< TTFB position display
  TextDisplay     *ttfcDisplay;  //!< TTFC position display
  
  //LinearControl    *ttfaControl;     //!< TTFA control widget
  //LinearControl    *ttfbControl;     //!< TTFB control widget
  //LinearControl    *ttfcControl;     //!< TTFC control widget
  
  // Exposure Setup and Control Widgets 
  
  TextEntry *objNameEntry;     //!< Object Name entry widget
  SelectEntry *imgTypeEntry;   //!< Image type selector widget
  NumberEntry *expTimeEntry;   //!< Exposure Time entry widget
  NumberEntry *numExpEntry;    //!< Number of exposures in sequence entry widget
  IndexedMechanism *xCCDBin;   //!< X-axis on-chip binning factor "mechanism"
  IndexedSelector  *xBinEntry; //!< X-axis on-chip binning factor selector widget
  IndexedMechanism *yCCDBin;   //!< Y-axis on-chip binning factor "mechanism"
  IndexedSelector  *yBinEntry; //!< Y-axis on-chip binning factor selector widget
  SelectEntry *roiEntry;       //!< CCD region-of-interest (ROI) selector widget
  SelectEntry *speedEntry;     //!< CCD readout speed (fast/slow) selector widget
  ActionButton *goButton;      //!< Image Acquisition GO/ABORT button
  ActionButton *pauseButton;   //!< Image Acquisition Pause/Resume button
  ActionButton *stopButton;    //!< Image Acquisition STOP button
  QProgressBar *expProgress;   //!< Image Exposure Progress Bar
  QProgressBar *readProgress;  //!< Image Readout Progress Bar
  TextDisplay  *acqStatus;     //!< Image Acquisition Status Display
  TextDisplay  *nextFileDisp;  //!< Display the next image file to be written
  TextDisplay  *lastFileDisp;  //!< Display the last image file written
  
  QString imgType;  //!< Image type (object, bias, flat, etc.)
  QString objName;  //!< Object Name
  QString fileName; //!< Filename of the next FITS image to be written
  int fileNum;      //!< File number...
  QString lastFile; //!< Filename of the last FITS image written
  QString imgPath;  //!< Path to the raw FITS image directory
  
  double expTime;   //!< Unit exposure time in seconds
  int numExp;       //!< Number of exposures to acquire
  int kExp;         //!< Completed exposure count
  int xBin;         //!< X-axis on-chip binning factor
  int yBin;         //!< Y-axis on-chip binning factor
  
  QTimer *expTimer; //!< Exposure countdown timer
  int expElapsed;   //!< elapsed exposure time in integer seconds
  int expRemaining; //!< remaining exposure time in integers seconds
  
  // IMCS Controls and Displays
  
  QCheckBox *useIMCSCheck;       //!< Enable/Disable use of the IMCS during an exposure
  ValueDisplay *imcsLoopDisplay; //!< Status of the IMCS control loop
  ActionButton *imcsLockButton;  //!< Initiate IMCS Lock-on

  // Exposure Control Parameters
  
  bool imgAcquire;      //!< An image is being acquired
  bool imgPause;        //!< Exposure is paused
  bool imgAbort;        //!< Abort an exposure sequence
  int expState;         //!< Exposure state, see the exposureStatus enum
  int pctReadout;       //!< Percentage of the CCD readout (0..100)
  QString expStatusStr; //!< String describing the current exposure status
  bool shutterOpen;     //!< is the camera shutter open (T) or closed (F)
  
  // IMCS Control Parameters
  
  bool    imcsEnabled;     //!< Is the IMCS enabled?
  double  imcsMinExpTime;  //!< Minimum exposure time for using the IMCS
  bool    useIMCS;         //!< Is the IMCS to be used for this exposure?
  bool    imcsLoopClosed;  //!< IMCS correction loop is closed (T) or open (F)
  bool    imcsOnTarget;    //!< Is the IMCS beam centered?
  double  imcsGain;        //!< IMCS correction gain factor (usually ~1.0)
  double  imcsThresh;      //!< IMCS quad-cell correction signal threshold
  int     imcsNumAverage;  //!< Number of measurements to avarge
  int     imcsSampTime;    //!< IMCS sampling interval in milliseconds
  int     imcsCycleTime;   //!< IMCS correction cycle time in milliseconds (=imcsNumAverage*imcsSampTime)
  int     imcsWaitForLock; //!< Time to wait in milliseconds for IMCS lock or timeout
  int     imcsTimeLeft;    //!< Amount of time left in the current lock-on wait in milliseconds
  QTimer  *imcsTimer;      //!< IMCS query/lock timer
  bool    imcsLockPending; //!< There is a pending lock request
  int     imcsLockCount;   //!< Number of cycles the IMCS showed lock

};

#endif
