#include <QtWidgets>
#include <QtNetwork>
#include <math.h>

#include "envpanel.h"
#include "actionbutton.h"
#include "textdisplay.h"
#include "valuedisplay.h"

#include "isisclient.h"

EnvPanel::EnvPanel(const int &myMODS, QWidget *parent)
  : QWidget(parent)
{
  // Initialize parameters and state variables
  
  modsID = myMODS;
  panelName = QString("MODS%1").arg(modsID+1);

  //
  // Top Panel: Instrument Environment, Glycol, and IUB Monitors
  //

  // Instrument Air and Truss Temperature Monitor

  QFormLayout *instTopLayout = new QFormLayout;
  instTopLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
  instTopLayout->setFormAlignment(Qt::AlignCenter);  
  instTopLayout->setLabelAlignment(Qt::AlignRight);
  instTopLayout->setVerticalSpacing(0);

  sensorDisplay[TopAirTemp] = new TextDisplay("Top","C",5,this);
  instTopLayout->addRow(tr("Air"),sensorDisplay[TopAirTemp]);

  sensorDisplay[TopTrussTemp] = new TextDisplay("Top","C",5,this);
  instTopLayout->addRow(tr("Truss"),sensorDisplay[TopTrussTemp]);

  QFormLayout *instBotLayout = new QFormLayout;
  instBotLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
  instBotLayout->setFormAlignment(Qt::AlignCenter);  
  instBotLayout->setLabelAlignment(Qt::AlignRight);
  instBotLayout->setVerticalSpacing(0);

  sensorDisplay[BotAirTemp] = new TextDisplay("","C",5,this);
  instBotLayout->addRow("Bot",sensorDisplay[BotAirTemp]);

  sensorDisplay[BotTrussTemp] = new TextDisplay("","C",5,this);
  instBotLayout->addRow("Bot",sensorDisplay[BotTrussTemp]);

  QFormLayout *instDiffLayout = new QFormLayout;
  instDiffLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
  instDiffLayout->setFormAlignment(Qt::AlignCenter);  
  instDiffLayout->setLabelAlignment(Qt::AlignRight);
  instDiffLayout->setVerticalSpacing(0);

  sensorDisplay[DTInstAir] = new TextDisplay("","C",5,this);
  instDiffLayout->addRow("dT",sensorDisplay[DTInstAir]);

  sensorDisplay[DTTruss] = new TextDisplay("","C",5,this);
  instDiffLayout->addRow("dT",sensorDisplay[DTTruss]);

  // Layout horizontally in a group box

  QGroupBox *instGroupBox = new QGroupBox(tr("Interior Temperatures"));
  QHBoxLayout *instLayout = new QHBoxLayout;
  instLayout->addLayout(instTopLayout);
  instLayout->addStretch();
  instLayout->addLayout(instBotLayout);
  instLayout->addStretch();
  instLayout->addLayout(instDiffLayout);
  instGroupBox->setLayout(instLayout);
  
  // Instrument Glycol Supply and Return monitor

  QFormLayout *glycolSupplyLayout = new QFormLayout;
  glycolSupplyLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
  glycolSupplyLayout->setFormAlignment(Qt::AlignCenter);  
  glycolSupplyLayout->setLabelAlignment(Qt::AlignRight);
  glycolSupplyLayout->setVerticalSpacing(0);

  sensorDisplay[GSPres] = new TextDisplay("In","psi",5,this);
  glycolSupplyLayout->addRow(tr("Pres"),sensorDisplay[GSPres]);

  sensorDisplay[GSTemp] = new TextDisplay("In","C",5,this);
  glycolSupplyLayout->addRow(tr("Temp"),sensorDisplay[GSTemp]);

  QFormLayout *glycolReturnLayout = new QFormLayout;
  glycolReturnLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
  glycolReturnLayout->setFormAlignment(Qt::AlignCenter);  
  glycolReturnLayout->setLabelAlignment(Qt::AlignRight);
  glycolReturnLayout->setVerticalSpacing(0);

  sensorDisplay[GRPres] = new TextDisplay("","psi",5,this);
  glycolReturnLayout->addRow("Out",sensorDisplay[GRPres]);

  sensorDisplay[GRTemp] = new TextDisplay("","C",5,this);
  glycolReturnLayout->addRow("Out",sensorDisplay[GRTemp]);

  QFormLayout *glycolDiffLayout = new QFormLayout;
  glycolDiffLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
  glycolDiffLayout->setFormAlignment(Qt::AlignCenter);  
  glycolDiffLayout->setLabelAlignment(Qt::AlignRight);
  glycolDiffLayout->setVerticalSpacing(0);

  sensorDisplay[DGPres] = new TextDisplay("","psi",5,this);
  glycolDiffLayout->addRow("dP",sensorDisplay[DGPres]);

  sensorDisplay[DGTemp] = new TextDisplay("","C",5,this);
  glycolDiffLayout->addRow("dT",sensorDisplay[DGTemp]);

  // Layout horizontally in a group box

  QGroupBox *glycolGroupBox = new QGroupBox(tr("Glycol Cooling"));
  QHBoxLayout *glycolLayout = new QHBoxLayout;
  glycolLayout->addLayout(glycolSupplyLayout);
  glycolLayout->addStretch();
  glycolLayout->addLayout(glycolReturnLayout);
  glycolLayout->addStretch();
  glycolLayout->addLayout(glycolDiffLayout);
  glycolGroupBox->setLayout(glycolLayout);

  // Utility Box Power States

  QFormLayout *utilPowerLayout = new QFormLayout;
  utilPowerLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
  utilPowerLayout->setFormAlignment(Qt::AlignCenter);  
  utilPowerLayout->setLabelAlignment(Qt::AlignRight);
  utilPowerLayout->setVerticalSpacing(0);

  sensorDisplay[GCam_ACPower] = new TextDisplay("","",5,this);
  utilPowerLayout->addRow(tr("Guide Camera"),sensorDisplay[GCam_ACPower]);

  sensorDisplay[WFS_ACPower] = new TextDisplay("","",5,this);
  utilPowerLayout->addRow(tr("WFS Camera"),sensorDisplay[WFS_ACPower]);

  sensorDisplay[LLB_ACPower] = new TextDisplay("","",5,this);
  utilPowerLayout->addRow(tr("Lamp/Laser Box"),sensorDisplay[LLB_ACPower]);

  QFormLayout *utilTempLayout = new QFormLayout;
  utilTempLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
  utilTempLayout->setFormAlignment(Qt::AlignCenter);  
  utilTempLayout->setLabelAlignment(Qt::AlignRight);
  utilTempLayout->setVerticalSpacing(0);

  sensorDisplay[AmbientTemp] = new TextDisplay("","C",5,this);
  utilTempLayout->addRow(tr("Ambient"),sensorDisplay[AmbientTemp]);

  sensorDisplay[IUB_AirTemp] = new TextDisplay("","C",5,this);
  utilTempLayout->addRow(tr("Inside Air"),sensorDisplay[IUB_AirTemp]);

  sensorDisplay[IUB_AGwTemp] = new TextDisplay("","C",5,this);
  utilTempLayout->addRow(tr("Heat Sink"),sensorDisplay[IUB_AGwTemp]);

  // Layout horizontally in a group box

  QGroupBox *utilGroupBox = new QGroupBox(tr("Utility Box"));
  QHBoxLayout *utilLayout = new QHBoxLayout;
  utilLayout->addLayout(utilPowerLayout);
  utilLayout->addLayout(utilTempLayout);
  utilLayout->addStretch();
  utilGroupBox->setLayout(utilLayout);

  // Arrange Glycol, Instrument Interior, and IUB displays

  QHBoxLayout *topLayout = new QHBoxLayout;
  topLayout->addWidget(utilGroupBox);
  topLayout->addWidget(glycolGroupBox);
  topLayout->addWidget(instGroupBox);
  topLayout->addStretch();

  // Put them all into a group box...

  QGroupBox *topGroupBox = new QGroupBox("Instrument");
  QPalette *topPalette = new QPalette;
  *topPalette = topGroupBox->palette();
  topPalette->setColor(QPalette::Mid,Qt::darkGreen);
  topGroupBox->setPalette(*topPalette);
  topGroupBox->setContentsMargins(3,10,3,3);
  topGroupBox->setLayout(topLayout);

  //--------------------------------------------------
  //
  // Middle Panel: Blue CCD, HEB, and IEB Monitors
  //

  // Blue CCD vacuum and temperatures

  QFormLayout *blueCCDPTLayout = new QFormLayout;
  blueCCDPTLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
  blueCCDPTLayout->setFormAlignment(Qt::AlignCenter);  
  blueCCDPTLayout->setLabelAlignment(Qt::AlignRight);
  blueCCDPTLayout->setVerticalSpacing(0);

  sensorDisplay[Blue_DewPres] = new TextDisplay("","torr",8,this);
  blueCCDPTLayout->addRow(tr("Vacuum"),sensorDisplay[Blue_DewPres]);

  sensorDisplay[Blue_CCDTemp] = new TextDisplay("","C",8,this);
  blueCCDPTLayout->addRow(tr("CCD"),sensorDisplay[Blue_CCDTemp]);

  sensorDisplay[Blue_DewTemp] = new TextDisplay("","C",8,this);
  blueCCDPTLayout->addRow(tr("LN2 Tank"),sensorDisplay[Blue_DewTemp]);

  QGroupBox *blueCCDGroupBox = new QGroupBox(tr("Blue CCD"));
  blueCCDGroupBox->setLayout(blueCCDPTLayout);

  // Blue HEB status

  // This column has AC power, Glycol Supply, and Heat Sink Temp

  QFormLayout *blueHEBInLayout = new QFormLayout;
  blueHEBInLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
  blueHEBInLayout->setFormAlignment(Qt::AlignCenter);  
  blueHEBInLayout->setLabelAlignment(Qt::AlignRight);
  blueHEBInLayout->setVerticalSpacing(0);

  sensorDisplay[HEB_B_ACPower] = new TextDisplay("","",5,this);
  blueHEBInLayout->addRow(tr("AC Power"),sensorDisplay[HEB_B_ACPower]);

  sensorDisplay[HEB_B_GSTemp] = new TextDisplay("","C",5,this);
  blueHEBInLayout->addRow(tr("Glycol In"),sensorDisplay[HEB_B_GSTemp]);

  sensorDisplay[HEB_B_HSTemp] = new TextDisplay("","C",5,this);
  blueHEBInLayout->addRow(tr("Heat Sink"),sensorDisplay[HEB_B_HSTemp]);

  // This column as the sequencer, glycol return, and air temp

  QFormLayout *blueHEBOutLayout = new QFormLayout;
  blueHEBOutLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
  blueHEBOutLayout->setFormAlignment(Qt::AlignCenter);  
  blueHEBOutLayout->setLabelAlignment(Qt::AlignRight);
  blueHEBOutLayout->setVerticalSpacing(0);

  sensorDisplay[Blue_Sequencer] = new TextDisplay("","",5,this);
  blueHEBOutLayout->addRow(tr("SEQ"),sensorDisplay[Blue_Sequencer]);

  sensorDisplay[HEB_B_GRTemp] = new TextDisplay("","C",5,this);
  blueHEBOutLayout->addRow(tr("Out"),sensorDisplay[HEB_B_GRTemp]);

  sensorDisplay[HEB_B_AirTemp] = new TextDisplay("","C",5,this);
  blueHEBOutLayout->addRow(tr("Air"),sensorDisplay[HEB_B_AirTemp]);

  // This column has TEC power, and delta-T for glycol and air

  QFormLayout *blueHEBDTLayout = new QFormLayout;
  blueHEBDTLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
  blueHEBDTLayout->setFormAlignment(Qt::AlignCenter);  
  blueHEBDTLayout->setLabelAlignment(Qt::AlignRight);
  blueHEBDTLayout->setVerticalSpacing(0);

  sensorDisplay[Blue_TEDPower] = new TextDisplay("","",5,this);
  blueHEBDTLayout->addRow(tr("TEC"),sensorDisplay[Blue_TEDPower]);

  sensorDisplay[HEB_B_DTGlycol] = new TextDisplay("","C",5,this);
  blueHEBDTLayout->addRow(tr("dT"),sensorDisplay[HEB_B_DTGlycol]);

  sensorDisplay[HEB_B_DTAir] = new TextDisplay("","C",5,this);
  blueHEBDTLayout->addRow(tr("dT"),sensorDisplay[HEB_B_DTAir]);

  // Layout horizontally in a group box

  QGroupBox *blueHEBGroupBox = new QGroupBox(tr("Blue HEB"));
  QHBoxLayout *blueHEBLayout = new QHBoxLayout;
  blueHEBLayout->addLayout(blueHEBInLayout);
  blueHEBLayout->addStretch();
  blueHEBLayout->addLayout(blueHEBOutLayout);
  blueHEBLayout->addStretch();
  blueHEBLayout->addLayout(blueHEBDTLayout);
  blueHEBGroupBox->setLayout(blueHEBLayout);

  // Blue IEB temperature and power state

  QFormLayout *blueIEBLayout = new QFormLayout;
  blueIEBLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
  blueIEBLayout->setFormAlignment(Qt::AlignCenter);  
  blueIEBLayout->setLabelAlignment(Qt::AlignRight);
  blueIEBLayout->setVerticalSpacing(0);

  sensorDisplay[IEB_B_ACPower] = new TextDisplay("","",5,this);
  blueIEBLayout->addRow(tr("AC Power"),sensorDisplay[IEB_B_ACPower]);

  sensorDisplay[IEB_B_AirTemp] = new TextDisplay("","C",5,this);
  blueIEBLayout->addRow(tr("Inside Air"),sensorDisplay[IEB_B_AirTemp]);

  sensorDisplay[IEB_B_GRTemp] = new TextDisplay("","C",5,this);
  blueIEBLayout->addRow(tr("Glycol"),sensorDisplay[IEB_B_GRTemp]);

  QGroupBox *blueIEBGroupBox = new QGroupBox(tr("Blue IEB"));
  blueIEBGroupBox->setLayout(blueIEBLayout);

  // Organize the contents horizontally

  QHBoxLayout *blueLayout = new QHBoxLayout;
  blueLayout->addWidget(blueCCDGroupBox);
  blueLayout->addWidget(blueHEBGroupBox);
  blueLayout->addWidget(blueIEBGroupBox);
  blueLayout->addStretch();

  // Put it into the a group box...

  QGroupBox *blueGroupBox = new QGroupBox("Blue Channel");
  blueGroupBox->setContentsMargins(3,10,3,3);
  blueGroupBox->setLayout(blueLayout);
  QPalette *bluePalette = new QPalette;
  *bluePalette = blueGroupBox->palette();
  bluePalette->setColor(QPalette::Mid,Qt::blue);
  blueGroupBox->setPalette(*bluePalette);

  //--------------------------------------------------
  //
  // Bottom Panel: Red CCD, HEB, and IEB Monitors
  //

  // Red CCD vacuum and temperature

  QFormLayout *redCCDPTLayout = new QFormLayout;
  redCCDPTLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
  redCCDPTLayout->setFormAlignment(Qt::AlignCenter);  
  redCCDPTLayout->setLabelAlignment(Qt::AlignRight);
  redCCDPTLayout->setVerticalSpacing(0);

  sensorDisplay[Red_DewPres] = new TextDisplay("","torr",8,this);
  redCCDPTLayout->addRow(tr("Vacuum"),sensorDisplay[Red_DewPres]);

  sensorDisplay[Red_CCDTemp] = new TextDisplay("","C",8,this);
  redCCDPTLayout->addRow(tr("CCD"),sensorDisplay[Red_CCDTemp]);

  sensorDisplay[Red_DewTemp] = new TextDisplay("","C",8,this);
  redCCDPTLayout->addRow(tr("LN2 Tank"),sensorDisplay[Red_DewTemp]);

  QGroupBox *redCCDGroupBox = new QGroupBox(tr("Red CCD"));
  redCCDGroupBox->setLayout(redCCDPTLayout);

  // Red HEB states

  // This column has AC power, glycol supply and heat sink temperatures

  QFormLayout *redHEBInLayout = new QFormLayout;
  redHEBInLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
  redHEBInLayout->setFormAlignment(Qt::AlignCenter);  
  redHEBInLayout->setLabelAlignment(Qt::AlignRight);
  redHEBInLayout->setVerticalSpacing(0);

  sensorDisplay[HEB_R_ACPower] = new TextDisplay("","",5,this);
  redHEBInLayout->addRow(tr("AC Power"),sensorDisplay[HEB_R_ACPower]);

  sensorDisplay[HEB_R_GSTemp] = new TextDisplay("","C",5,this);
  redHEBInLayout->addRow(tr("Glycol In"),sensorDisplay[HEB_R_GSTemp]);

  sensorDisplay[HEB_R_HSTemp] = new TextDisplay("","C",5,this);
  redHEBInLayout->addRow(tr("Heat Sink"),sensorDisplay[HEB_R_HSTemp]);

  // This column has sequencer state, glycol return, and air temperatures

  QFormLayout *redHEBOutLayout = new QFormLayout;
  redHEBOutLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
  redHEBOutLayout->setFormAlignment(Qt::AlignCenter);  
  redHEBOutLayout->setLabelAlignment(Qt::AlignRight);
  redHEBOutLayout->setVerticalSpacing(0);

  sensorDisplay[Red_Sequencer] = new TextDisplay("","",5,this);
  redHEBOutLayout->addRow(tr("SEQ"),sensorDisplay[Red_Sequencer]);

  sensorDisplay[HEB_R_GRTemp] = new TextDisplay("","C",5,this);
  redHEBOutLayout->addRow(tr("Out"),sensorDisplay[HEB_R_GRTemp]);

  sensorDisplay[HEB_R_AirTemp] = new TextDisplay("","C",5,this);
  redHEBOutLayout->addRow(tr("Air"),sensorDisplay[HEB_R_AirTemp]);

  // This column has TEC power, and delta-T for glycol and air

  QFormLayout *redHEBDTLayout = new QFormLayout;
  redHEBDTLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
  redHEBDTLayout->setFormAlignment(Qt::AlignCenter);  
  redHEBDTLayout->setLabelAlignment(Qt::AlignRight);
  redHEBDTLayout->setVerticalSpacing(0);

  sensorDisplay[Red_TEDPower] = new TextDisplay("","",5,this);
  redHEBDTLayout->addRow(tr("TEC"),sensorDisplay[Red_TEDPower]);

  sensorDisplay[HEB_R_DTGlycol] = new TextDisplay("","C",5,this);
  redHEBDTLayout->addRow(tr("dT"),sensorDisplay[HEB_R_DTGlycol]);

  sensorDisplay[HEB_R_DTAir] = new TextDisplay("","C",5,this);
  redHEBDTLayout->addRow(tr("dT"),sensorDisplay[HEB_R_DTAir]);

  // Layout horizontally in a group box

  QGroupBox *redHEBGroupBox = new QGroupBox(tr("Red HEB"));
  QHBoxLayout *redHEBLayout = new QHBoxLayout;
  redHEBLayout->addLayout(redHEBInLayout);
  redHEBLayout->addStretch();
  redHEBLayout->addLayout(redHEBOutLayout);
  redHEBLayout->addStretch();
  redHEBLayout->addLayout(redHEBDTLayout);
  redHEBGroupBox->setLayout(redHEBLayout);

  // Red IEB temperature and power state

  QFormLayout *redIEBLayout = new QFormLayout;
  redIEBLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);
  redIEBLayout->setFormAlignment(Qt::AlignCenter);  
  redIEBLayout->setLabelAlignment(Qt::AlignRight);
  redIEBLayout->setVerticalSpacing(0);

  sensorDisplay[IEB_R_ACPower] = new TextDisplay("","",5,this);
  redIEBLayout->addRow(tr("AC Power"),sensorDisplay[IEB_R_ACPower]);

  sensorDisplay[IEB_R_AirTemp] = new TextDisplay("","C",5,this);
  redIEBLayout->addRow(tr("Inside Air"),sensorDisplay[IEB_R_AirTemp]);

  sensorDisplay[IEB_R_GRTemp] = new TextDisplay("","C",5,this);
  redIEBLayout->addRow(tr("Glycol"),sensorDisplay[IEB_R_GRTemp]);

  QGroupBox *redIEBGroupBox = new QGroupBox(tr("Red IEB"));
  redIEBGroupBox->setLayout(redIEBLayout);

  // Organize the contents horizontally

  QHBoxLayout *redLayout = new QHBoxLayout;
  redLayout->addWidget(redCCDGroupBox);
  redLayout->addWidget(redHEBGroupBox);
  redLayout->addWidget(redIEBGroupBox);
  redLayout->addStretch();

  // Put it into the a group box...

  QGroupBox *redGroupBox = new QGroupBox("Red Channel");
  redGroupBox->setContentsMargins(3,10,3,3);
  redGroupBox->setLayout(redLayout);
  QPalette *redPalette = new QPalette;
  *redPalette = redGroupBox->palette();
  redPalette->setColor(QPalette::Mid,Qt::red);
  redGroupBox->setPalette(*redPalette);

  //
  // Ambient temperature, update time, and the update button
  //

  updateDisplay = new ValueDisplay("Last Update:","UTC",this);
  updateButton = new ActionButton(tr("Update"),this);
  connect(updateButton,SIGNAL(clicked()),this,SLOT(update()));

  QHBoxLayout *ambientLayout = new QHBoxLayout;
  ambientLayout->addWidget(updateDisplay);
  ambientLayout->addStretch();
  ambientLayout->addWidget(updateButton);

  // Layout the three panels vertically

  QVBoxLayout *envLayout = new QVBoxLayout;
  envLayout->addLayout(ambientLayout);
  envLayout->addWidget(topGroupBox);
  envLayout->addWidget(blueGroupBox);
  envLayout->addWidget(redGroupBox);
  envLayout->addStretch();

  // Put it into the a group box...

  QGroupBox *mainGroupBox = new QGroupBox(tr("%1 Instrument Sensors").arg(panelName));
  mainGroupBox->setAlignment(Qt::AlignHCenter);
  mainGroupBox->setContentsMargins(3,10,3,3);
  mainGroupBox->setLayout(envLayout);

  // ... and build the final layout.

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(mainGroupBox);
  mainLayout->setContentsMargins(0,0,0,0);
  setLayout(mainLayout);

  // Set the sensor display widget attributes here, and initialize
  // (clear) the alarms.

  for (int i=0;i<NUMSENSORS;i++) {
    sensorDisplay[i]->setNormalColors(Qt::black,Qt::green);
    sensorDisplay[i]->setFaultColors(Qt::black,Qt::red);
    sensorDisplay[i]->setChangeColors(Qt::black,QColor(255,165,0,255));
    sensorDisplay[i]->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
    sensorDisplay[i]->clear();
    sensorDisplay[i]->setNormal();
    clearAlarm(i);
  }

}

// Send a command to a particular host and wait for reply up to a
// timeout interval if timeout>0 otherwise queue to the command
// sequencer if timeout<0, or just send and forget if timeout=0

void EnvPanel::sendCmdWait(const QString &hostStr, const QString &cmdStr, 
			   const int &timeout)
{
  if (hostStr.isEmpty() || cmdStr.isEmpty()) return;
  
  if (timeout == 0)
    emit cmdReady(hostStr,cmdStr);
  else
    emit cmdWait(hostStr,cmdStr,timeout);
}

//---------------------------------------------------------------------------
// 
// parse - process replies from an IMPv2 client
//
// Parse messages, picking off those keyword=value pairs we care about
// and ignorning the rest.
//
// This is the place where we test status data against alarm
// thresholds and emit new alerts or cancel old alerts as needed.
//

void EnvPanel::parse(const QString &remHost, 
		     const QString &cmdStr,
		     const int &msgType, 
		     const QHash<QString,QString> &keyDict)
{
	// TODO: Unused flags. Commented out on 2-19-24
//   bool cmdDone = false;   // Flag: message is a command completion
//   bool cmdFault = false;  // Flag: command completed with a fault
  bool ok;
  bool fromIE = false; 
  bool isRed = false;

  QString msgStr = cmdStr;
  msgStr = msgStr.simplified(); // TODO: Evaluate this line. Why is it here? 

  if (msgStr.isEmpty()) return; // Empty messages are ignored
  
  // TODO: Unused flags. Commented out on 2-19-24
  // Our response depends on the type of message we received. 
  switch (msgType) {
  case ISISClient::ERROR:
    // cmdDone = true;
    // cmdFault = true;
    break;

  case ISISClient::WARNING:
    // cmdDone = true;
    // cmdFault = false;
    break;

  case ISISClient::FATAL:
    // cmdDone = true;
    // cmdFault = true;
    break;

  case ISISClient::DONE:
    // cmdDone = true;
    // cmdFault = false;
    break;

  case ISISClient::STATUS:
    // cmdDone = false;
    // cmdFault = false;
    break;

  default:
    // Nothing to be done here, return (we don't handle REQ: or EXEC: requests)
    return;
    break;
  }

  // The first word in the message body is the command that generated
  // this response.  The conservative implementation of this parser
  // demands that the command word be relevant for sensor monitoring
  // otherwise it gets ignored so we don't process irrelevant info.
  //
  // The "command" word "UNABLE" allows us to trap broadcast warning
  // messages from an IC that the sequencer has gone offline.  This
  // is a bit of old-style (pre-IMPv2) legacy communications we must
  // trap for, grandfathering in bits of older systems.  Not pretty
  // but it does the job.

  QString cmdWord = msgStr.section(" ",0,0);
  if (cmdWord.compare("UTIL",   Qt::CaseInsensitive)!=0 &&
      cmdWord.compare("IEB",    Qt::CaseInsensitive)!=0 &&
      cmdWord.compare("ESTATUS",Qt::CaseInsensitive)!=0 &&
      cmdWord.compare("STATUS", Qt::CaseInsensitive)!=0 &&
      cmdWord.compare("UNABLE", Qt::CaseInsensitive)!=0) return;

  // Were we passed any keyword=value pairs to interpret?

  int numKeys = keyDict.size();
  if (numKeys < 1) return;

  // Who did this come from?  Treat IMHost like IEHost...

  if (remHost.compare(modsIEHost[modsID],Qt::CaseInsensitive)==0 ||
      remHost.compare(modsIMHost[modsID],Qt::CaseInsensitive)==0) {
    fromIE = true;
    isRed = false;
  }
  else if (remHost.compare(modsBCHost[modsID],Qt::CaseInsensitive)==0) {
    fromIE = false;
    isRed = false;
  }
  else if (remHost.compare(modsRCHost[modsID],Qt::CaseInsensitive)==0) {
    fromIE = false;
    isRed = true;
  }
  else {
    return; // nobody we care to talk to...
  }

  // We have keys to interpret, do it.  Use the built-in Java-style
  // iterator to traverse the keyword dictionary hash.

  QHashIterator<QString,QString> i(keyDict);
  while (i.hasNext()) {
    i.next();
    QString keyStr = i.key();
    QString valStr = i.value();
    
    int numUpdated = 0;
    
    // Is the message from our IE server?

    if (fromIE) { 

      // Glycol Supply in-line pressure sensor
      
      if (keyStr.compare("GSPRES",Qt::CaseInsensitive)==0) {
	double dtmp = valStr.toDouble(&ok);
	if (ok) {
	  sensorDisplay[GSPres]->setText(QString::number(dtmp,'f',1),Qt::green);
	  sensorDisplay[GSPres]->setNormal();
	  sensorData[GSPres] = dtmp;
	  sensorData[DGPres] = sensorData[GRPres]-sensorData[GSPres];
	  sensorDisplay[DGPres]->setText(QString::number(sensorData[DGPres],'f',1),Qt::green);
	  if (dtmp < MODS_MIN_GLYPRES) {
	    sensorDisplay[GSPres]->setFault();
	    if (setAlarm(GSPres)) 
	      emit alert(tr("%1 Glycol supply pressure is below %2 psi - cooling failure?").arg(panelName).arg(MODS_MIN_GLYPRES));
	  }
	  else {
	    sensorDisplay[GSPres]->setNormal();
	    if (sensorAlarm[GSPres]) // about to clear an active alarm
	      emit alertMsg(tr("%1 Glycol supply pressure is back above minimum").arg(panelName),Qt::blue);
	    clearAlarm(GSPres);
	  }
	}
	else {
	  sensorDisplay[GSPres]->clear();
	  sensorDisplay[DGPres]->clear();
	  clearAlarm(GSPres);
	}
	numUpdated++;
      }
      
      // Main Glycol Supply in-line temperature sensor
      
      else if (keyStr.compare("GSTEMP",Qt::CaseInsensitive)==0) {
	double dtmp = valStr.toDouble(&ok);
	if (ok) {
	  sensorDisplay[GSTemp]->setText(QString::number(dtmp,'f',1),Qt::green);
	  sensorDisplay[GSTemp]->setNormal();
	  sensorData[GSTemp] = dtmp;
	  sensorData[DGTemp] = sensorData[GRTemp]-sensorData[GSTemp];
	  sensorDisplay[DGTemp]->setText(QString::number(sensorData[DGTemp],'f',1),Qt::green);
	  sensorDisplay[DGTemp]->setNormal();
	}
	else {
	  sensorDisplay[GSTemp]->clear();
	  sensorDisplay[DGTemp]->clear();
	}
	numUpdated++;
      }
      
      // Main Glycol Return in-line pressure sensor
      
      else if (keyStr.compare("GRPRES",Qt::CaseInsensitive)==0) {
	double dtmp = valStr.toDouble(&ok);
	if (ok) {
	  sensorDisplay[GRPres]->setText(QString::number(dtmp,'f',1),Qt::green);
	  sensorDisplay[GRPres]->setNormal();
	  sensorData[GRPres] = dtmp;
	  sensorData[DGPres] = sensorData[GRPres]-sensorData[GSPres];
	  sensorDisplay[DGPres]->setText(QString::number(sensorData[DGPres],'f',1),Qt::green);
	  sensorDisplay[DGPres]->setNormal();
	  if (dtmp < MODS_MIN_GLYPRES) {
	    sensorDisplay[GRPres]->setFault();
	    if (setAlarm(GRPres))
	      emit alert(tr("%1 Glycol return pressure below %2 psi - cooling failure?").arg(panelName).arg(MODS_MIN_GLYPRES));
	  }
	  else {
	    sensorDisplay[GRPres]->setNormal();	
	    if (sensorAlarm[GRPres]) // about to clear an active alarm
	      emit alertMsg(tr("%1 Glycol return pressure is back above minimum").arg(panelName),Qt::blue);
	    clearAlarm(GRPres);
	  }
	}
	else {
	  sensorDisplay[GRPres]->clear();
	  sensorDisplay[DGPres]->clear();
	  clearAlarm(GRPres);
	}
	numUpdated++;
      }
      
      // Main Glycol Return in-line temperature sensor
      
      else if (keyStr.compare("GRTEMP",Qt::CaseInsensitive)==0) {
	double dtmp = valStr.toDouble(&ok);
	if (ok) {
	  sensorDisplay[GRTemp]->setText(QString::number(dtmp,'f',1),Qt::green);
	  sensorDisplay[GRTemp]->setNormal();
	  sensorData[GRTemp] = dtmp;
	  sensorData[DGTemp] = sensorData[GRTemp]-sensorData[GSTemp];
	  sensorDisplay[DGTemp]->setText(QString::number(sensorData[DGTemp],'f',1),Qt::green);
	  sensorDisplay[DGTemp]->setNormal();
	}
	else {
	  sensorDisplay[GRTemp]->clear();
	  sensorDisplay[DGTemp]->clear();
	}
	numUpdated++;
      }
      
      // Outside-Instrument ambient air temperature near the IUB
      
      else if (keyStr.compare("AMBTEMP",Qt::CaseInsensitive)==0) {
	double dtmp = valStr.toDouble(&ok);
	if (ok && dtmp < 200.0) {
	  sensorData[AmbientTemp] = dtmp;
	  sensorDisplay[AmbientTemp]->setText(QString::number(dtmp,'f',1),Qt::green);
	  sensorDisplay[AmbientTemp]->setNormal();
	}
	else 
	  sensorDisplay[AmbientTemp]->clear();
	numUpdated++;
      }
      
      // Air temperature inside the IUB
      
      else if (keyStr.compare("IUBTAIR",Qt::CaseInsensitive)==0) {
	double dtmp = valStr.toDouble(&ok);
	if (ok && dtmp < 200.0) {
	  sensorData[IUB_AirTemp] = dtmp;
	  sensorDisplay[IUB_AirTemp]->setText(QString::number(dtmp,'f',1),Qt::green);
	  if (dtmp > MODS_MAX_BOXTEMP) {
	    sensorDisplay[IUB_AirTemp]->setFault();
	    if (setAlarm(IUB_AirTemp))
	      emit alert(tr("%1 IUB inside air temperature of %2C exceeds %3C").arg(panelName).arg(QString::number(dtmp,'f',1)).arg(MODS_MAX_BOXTEMP));
	  }
	  else {
	    sensorDisplay[IUB_AirTemp]->setNormal();
	    if (sensorAlarm[IUB_AirTemp]) // about to clear an active alarm
	      emit alertMsg(tr("%1 IUB inside air temperature is back in normal range.").arg(panelName),Qt::blue);
	    clearAlarm(IUB_AirTemp);
	  }
	}
	else {
	  sensorDisplay[IUB_AirTemp]->clear();
	  clearAlarm(IUB_AirTemp);
	}
	numUpdated++;
      }
      
      // Temperature of the AGw controller heat sink
      
      else if (keyStr.compare("AGHSTEMP",Qt::CaseInsensitive)==0) {
	double dtmp = valStr.toDouble(&ok);
	if (ok && dtmp < 200.0) {
	  sensorData[IUB_AGwTemp] = dtmp;
	  sensorDisplay[IUB_AGwTemp]->setText(QString::number(dtmp,'f',1),Qt::green);
	  if (dtmp > MODS_MAX_BOXTEMP)
	    sensorDisplay[IUB_AGwTemp]->setFault();
	  else
	    sensorDisplay[IUB_AGwTemp]->setNormal();
	}
	else 
	  sensorDisplay[IUB_AGwTemp]->clear();
	numUpdated++;
      }
      
      // Blue IEB Temperature Sensors
      
      else if (keyStr.compare("IEBTEMPB",Qt::CaseInsensitive)==0) {
	double dtmp = valStr.toDouble(&ok);
	if (ok && dtmp < 200.0) {
	  sensorData[IEB_B_AirTemp] = dtmp;
	  sensorDisplay[IEB_B_AirTemp]->setText(QString::number(dtmp,'f',1),Qt::green);
	  if (dtmp > MODS_MAX_BOXTEMP) {
	    sensorDisplay[IEB_B_AirTemp]->setFault();
	    if (setAlarm(IEB_B_AirTemp))
	      emit alert(tr("%1 Blue IEB inside air temperature of %2C exceeds %3C").arg(panelName).arg(QString::number(dtmp,'f',1)).arg(MODS_MAX_BOXTEMP));
	  }
	  else {
	    sensorDisplay[IEB_B_AirTemp]->setNormal();
	    if (sensorAlarm[IEB_B_AirTemp]) // about to clear an active alarm
	      emit alertMsg(tr("%1 Blue IEB inside air temperature is back in normal range.").arg(panelName),Qt::blue);
	    clearAlarm(IEB_B_AirTemp);
	  }
	}
	else {
	  sensorDisplay[IEB_B_AirTemp]->clear();
	  clearAlarm(IEB_B_AirTemp);
	}
	numUpdated++;
      }
      
      else if (keyStr.compare("IEBGRT_B",Qt::CaseInsensitive)==0) {
	double dtmp = valStr.toDouble(&ok);
	if (ok && dtmp < 200.0) {
	  sensorData[IEB_B_GRTemp] = dtmp;
	  sensorDisplay[IEB_B_GRTemp]->setText(QString::number(dtmp,'f',1),Qt::green);
	  sensorDisplay[IEB_B_GRTemp]->setNormal();
	}
	else 
	  sensorDisplay[IEB_B_GRTemp]->clear();
	numUpdated++;
      }
      
      // Red IEB Temperature Sensors
      
      else if (keyStr.compare("IEBTEMPR",Qt::CaseInsensitive)==0) {
	double dtmp = valStr.toDouble(&ok);
	if (ok && dtmp < 200.0) {
	  sensorData[IEB_R_AirTemp] = dtmp;
	  sensorDisplay[IEB_R_AirTemp]->setText(QString::number(dtmp,'f',1),Qt::green);
	  if (dtmp > MODS_MAX_BOXTEMP) {
 	    sensorDisplay[IEB_R_AirTemp]->setFault();
	    if (setAlarm(IEB_R_AirTemp))
	      emit alert(tr("%1 Red IEB inside air temperature of %2C exceeds %3C").arg(panelName).arg(QString::number(dtmp,'f',1)).arg(MODS_MAX_BOXTEMP));
	  }
	  else {
 	    sensorDisplay[IEB_R_AirTemp]->setNormal();
	    if (sensorAlarm[IEB_R_AirTemp]) // about to clear an active alarm
	      emit alertMsg(tr("%1 Red IEB inside air temperature is back in normal range").arg(panelName),Qt::blue);
	    clearAlarm(IEB_R_AirTemp);
	  }
	}
	else {
	  sensorDisplay[IEB_R_AirTemp]->clear();
	  clearAlarm(IEB_R_AirTemp);
	}
	numUpdated++;
      }
      
      else if (keyStr.compare("IEBGRT_R",Qt::CaseInsensitive)==0) {
	double dtmp = valStr.toDouble(&ok);
	if (ok && dtmp < 200.0) {
	  sensorData[IEB_R_GRTemp] = dtmp;
	  sensorDisplay[IEB_R_GRTemp]->setText(QString::number(dtmp,'f',1),Qt::green);
	  sensorDisplay[IEB_R_GRTemp]->setNormal();
	}
	else 
	  sensorDisplay[IEB_R_GRTemp]->clear();
	numUpdated++;
      }

      // Instrument Inside Air top and bottom temperature sensors

      else if (keyStr.compare("TAIRTOP",Qt::CaseInsensitive)==0) {
	double dtmp = valStr.toDouble(&ok);
	if (ok && dtmp < 200.0) {
	  sensorData[TopAirTemp] = dtmp;
	  sensorDisplay[TopAirTemp]->setText(QString::number(dtmp,'f',1),Qt::green);
	  sensorDisplay[TopAirTemp]->setNormal();
	  sensorData[DTInstAir] = sensorData[TopAirTemp]-sensorData[BotAirTemp];
	  sensorDisplay[DTInstAir]->setText(QString::number(sensorData[DTInstAir],'f',1),
					    Qt::green);
	  sensorDisplay[DTInstAir]->setNormal();
	}
	else {
	  sensorDisplay[TopAirTemp]->clear();
	  sensorDisplay[DTInstAir]->clear();
	}
	numUpdated++;
      }
      
      else if (keyStr.compare("TAIRBOT",Qt::CaseInsensitive)==0) {
	double dtmp = valStr.toDouble(&ok);
	if (ok && dtmp < 200.0) {
	  sensorData[BotAirTemp] = dtmp;
	  sensorDisplay[BotAirTemp]->setText(QString::number(dtmp,'f',1),Qt::green);
	  sensorDisplay[BotAirTemp]->setNormal();
	  sensorData[DTInstAir] = sensorData[TopAirTemp]-sensorData[BotAirTemp];
	  sensorDisplay[DTInstAir]->setText(QString::number(sensorData[DTInstAir],'f',1),
					    Qt::green);
	  sensorDisplay[DTInstAir]->setNormal();
	}
	else {
	  sensorDisplay[BotAirTemp]->clear();
	  sensorDisplay[DTInstAir]->clear();
	}
	numUpdated++;
      }
      
      // Instrument Collimator Truss top and bottom temperature sensors

      else if (keyStr.compare("TCOLLTOP",Qt::CaseInsensitive)==0) {
	double dtmp = valStr.toDouble(&ok);
	if (ok && dtmp < 200.0) {
	  sensorData[TopTrussTemp] = dtmp;
	  sensorDisplay[TopTrussTemp]->setText(QString::number(dtmp,'f',1),Qt::green);
	  sensorDisplay[TopTrussTemp]->setNormal();
	  sensorData[DTTruss] = sensorData[TopTrussTemp]-sensorData[BotTrussTemp];
	  sensorDisplay[DTTruss]->setText(QString::number(sensorData[DTTruss],'f',1),
					    Qt::green);
	  sensorDisplay[DTTruss]->setNormal();
	}
	else {
	  sensorDisplay[TopTrussTemp]->clear();
	  sensorDisplay[DTTruss]->clear();
	}
	numUpdated++;
      }
      
      else if (keyStr.compare("TCOLLBOT",Qt::CaseInsensitive)==0) {
	double dtmp = valStr.toDouble(&ok);
	if (ok && dtmp < 200.0) {
	  sensorData[BotTrussTemp] = dtmp;
	  sensorDisplay[BotTrussTemp]->setText(QString::number(dtmp,'f',1),Qt::green);
	  sensorDisplay[BotTrussTemp]->setNormal();
	  sensorData[DTTruss] = sensorData[TopTrussTemp]-sensorData[BotTrussTemp];
	  sensorDisplay[DTTruss]->setText(QString::number(sensorData[DTTruss],'f',1),
					    Qt::green);
	  sensorDisplay[DTTruss]->setNormal();
	}
	else {
	  sensorDisplay[BotTrussTemp]->clear();
	  sensorDisplay[DTTruss]->clear();
	}
	numUpdated++;
      }
      
      // Power State parameters returned by modsenv v2, as well as the
      // usual UTIL state variables.  Starting to regret the
      // idiosyncrasies in the XYZ_BRK keyword, but we'll have to
      // grandfather it in as best we can.  In this context,
      // XYZ_BRK=UNKNOWN is equivalent to XYZ_PWR=OFF. 
      // [rwp/osu - 2013 Aug 22]

      // Blue and Red Instrument Electronics Box Power state (no manual on/ofF)

      else if (keyStr.compare("IEB_B_PWR",Qt::CaseInsensitive)==0 ||
	       keyStr.compare("IEB_B_BRK",Qt::CaseInsensitive)==0) {
	if (valStr.compare("ON",Qt::CaseInsensitive)==0 || valStr.compare("OK",Qt::CaseInsensitive)==0) {
	  sensorData[IEB_B_ACPower] = 1;
	  sensorDisplay[IEB_B_ACPower]->setNormal();
	  sensorDisplay[IEB_B_ACPower]->setText("ON",Qt::green);
	  if (sensorAlarm[IEB_B_ACPower]) // about to clear an active alarm
	    emit alertMsg(tr("%1 Blue IEB AC power restored").arg(panelName),Qt::blue);
	  clearAlarm(IEB_B_ACPower);
	}
	else if (valStr.compare("OFF",Qt::CaseInsensitive)==0 || 
		 valStr.compare("UNKNOWN",Qt::CaseInsensitive)==0) {
	  sensorData[IEB_B_ACPower] = 0;
	  sensorDisplay[IEB_B_ACPower]->setNormal();
	  sensorDisplay[IEB_B_ACPower]->setText("OFF",Qt::white);
	  if (sensorAlarm[IEB_B_ACPower]) // about to clear an active alarm
	    emit alertMsg(tr("%1 Blue IEB AC power turned OFF normally.").arg(panelName),Qt::blue);
	  clearAlarm(IEB_B_ACPower);
	}
	else if (valStr.compare("FAULT",Qt::CaseInsensitive)==0) {
	  sensorData[IEB_B_ACPower] = -1;
	  sensorDisplay[IEB_B_ACPower]->setText("OFF",Qt::red); // trip fault means power off, turn run
	  sensorDisplay[IEB_B_ACPower]->setFault();
	  if (setAlarm(IEB_B_ACPower))
	    emit alert(tr("%1 Blue IEB AC Power Fault (breaker tripped?)").arg(panelName));
	}
	numUpdated++;
      }
      
      else if (keyStr.compare("IEB_R_PWR",Qt::CaseInsensitive)==0 ||
	       keyStr.compare("IEB_R_BRK",Qt::CaseInsensitive)==0) {
	if (valStr.compare("ON",Qt::CaseInsensitive)==0 || valStr.compare("OK",Qt::CaseInsensitive)==0) {
	  sensorData[IEB_R_ACPower] = 1;
	  sensorDisplay[IEB_R_ACPower]->setNormal();
	  sensorDisplay[IEB_R_ACPower]->setText("ON",Qt::green);
	  if (sensorAlarm[IEB_R_ACPower]) // about to clear an active alarm
	    emit alertMsg(tr("%1 Red IEB AC power restored").arg(panelName),Qt::blue);
	  clearAlarm(IEB_R_ACPower);
	}
	else if (valStr.compare("OFF",Qt::CaseInsensitive)==0 || 
		 valStr.compare("UNKNOWN",Qt::CaseInsensitive)==0) {
	  sensorData[IEB_R_ACPower] = 0;
	  sensorDisplay[IEB_R_ACPower]->setNormal();
	  sensorDisplay[IEB_R_ACPower]->setText("OFF",Qt::white);
	  if (sensorAlarm[IEB_R_ACPower]) // about to clear an active alarm
	    emit alertMsg(tr("%1 Red IEB AC power turned off normally").arg(panelName),Qt::blue);
	  clearAlarm(IEB_R_ACPower);
	}
	else if (valStr.compare("FAULT",Qt::CaseInsensitive)==0) {
	  sensorData[IEB_R_ACPower] = -1;
	  sensorDisplay[IEB_R_ACPower]->setText("OFF",Qt::red);
	  sensorDisplay[IEB_R_ACPower]->setFault();
	  if (setAlarm(IEB_R_ACPower))
	    emit alert(tr("%1 Red IEB AC Power Fault (breaker tripped?)").arg(panelName));
	}
	numUpdated++;
      }

      // Lamp/Laser Box (LLB) AC Power State (no manual on/off)

      else if (keyStr.compare("LLB_PWR",Qt::CaseInsensitive)==0 ||
	       keyStr.compare("LLB_BRK",Qt::CaseInsensitive)==0) {
	if (valStr.compare("ON",Qt::CaseInsensitive)==0 || valStr.compare("OK",Qt::CaseInsensitive)==0) {
	  sensorData[LLB_ACPower] = 1;
	  sensorDisplay[LLB_ACPower]->setNormal();
	  sensorDisplay[LLB_ACPower]->setText("ON",Qt::green);
	  if (sensorAlarm[LLB_ACPower]) // about to clear an active alarm
	    emit alertMsg(tr("%1 Lamp/Laser Box AC power restored").arg(panelName),Qt::blue);
	  clearAlarm(LLB_ACPower);
	}
	else if (valStr.compare("OFF",Qt::CaseInsensitive)==0 || 
		 valStr.compare("UNKNOWN",Qt::CaseInsensitive)==0) {
	  sensorData[LLB_ACPower] = 0;
	  sensorDisplay[LLB_ACPower]->setNormal();
	  sensorDisplay[LLB_ACPower]->setText("OFF",Qt::white);
	  if (sensorAlarm[LLB_ACPower]) // about to clear an active alarm
	    emit alertMsg(tr("%1 Lamp/Laser Box AC powered off normally").arg(panelName),Qt::blue);
	  clearAlarm(LLB_ACPower);
	}
	else if (valStr.compare("FAULT",Qt::CaseInsensitive)==0) {
	  sensorData[LLB_ACPower] = -1;
	  sensorDisplay[LLB_ACPower]->setText("OFF",Qt::red);
	  sensorDisplay[LLB_ACPower]->setFault();
	  if (setAlarm(LLB_ACPower))
	    emit alert(tr("%1 Lamp/Laser Box AC Power Fault (breaker tripped?)").arg(panelName));
	}
	numUpdated++;
      }

      // Blue and Red HEB Power State (allows for manual on sense)
      //
      // We look at _PWR or _BRK to sense state changes

      else if (keyStr.compare("HEB_B_PWR",Qt::CaseInsensitive)==0 ||
	       keyStr.compare("HEB_B_BRK",Qt::CaseInsensitive)==0) {
	if (valStr.compare("ON",Qt::CaseInsensitive)==0 || valStr.compare("OK",Qt::CaseInsensitive)==0) {
	  sensorData[HEB_B_ACPower] = 1;
	  sensorDisplay[HEB_B_ACPower]->setNormal();
	  sensorDisplay[HEB_B_ACPower]->setText("ON",Qt::green);
	  if (sensorAlarm[HEB_B_ACPower]) // about to clear an active alarm
	    emit alertMsg(tr("%1 Blue HEB Box AC power restored").arg(panelName),Qt::blue);
	  clearAlarm(HEB_B_ACPower);
	}
	else if (valStr.compare("OFF",Qt::CaseInsensitive)==0 || 
		 valStr.compare("UNKNOWN",Qt::CaseInsensitive)==0) {
	  sensorData[HEB_B_ACPower] = 0;
	  sensorDisplay[HEB_B_ACPower]->setNormal();
	  sensorDisplay[HEB_B_ACPower]->setText("OFF",Qt::white);
	  if (sensorAlarm[HEB_B_ACPower]) // about to clear an active alarm
	    emit alertMsg(tr("%1 Blue HEB Box AC powered off normally").arg(panelName),Qt::blue);
	  clearAlarm(HEB_B_ACPower);
	}
	else if (valStr.compare("MANUAL",Qt::CaseInsensitive)==0) {
	  sensorData[HEB_B_ACPower] = 0;
	  sensorDisplay[HEB_B_ACPower]->setNormal();
	  sensorDisplay[HEB_B_ACPower]->setText("ON",QColor(255,126,0,255));
	  if (sensorAlarm[HEB_B_ACPower]) // about to clear an active alarm
	    emit alertMsg(tr("%1 Blue HEB Box AC power restored with manual override").arg(panelName),Qt::blue);
	  clearAlarm(HEB_B_ACPower);
	}
	else if (valStr.compare("FAULT",Qt::CaseInsensitive)==0) {
	  sensorData[HEB_B_ACPower] = -1;
	  sensorDisplay[HEB_B_ACPower]->setText("OFF",Qt::red);
	  sensorDisplay[HEB_B_ACPower]->setFault();
	  if (setAlarm(HEB_B_ACPower))
	    emit alert(tr("%1 Blue HEB AC Power Fault (breaker trip or thermal fault?)").arg(panelName));
	}
	numUpdated++;
      }
      
      else if (keyStr.compare("HEB_R_PWR",Qt::CaseInsensitive)==0 ||
	       keyStr.compare("HEB_R_BRK",Qt::CaseInsensitive)==0) {
	if (valStr.compare("ON",Qt::CaseInsensitive)==0 || valStr.compare("OK",Qt::CaseInsensitive)==0) {
	  sensorData[HEB_R_ACPower] = 1;
	  sensorDisplay[HEB_R_ACPower]->setNormal();
	  sensorDisplay[HEB_R_ACPower]->setText("ON",Qt::green);
	  if (sensorAlarm[HEB_R_ACPower]) // about to clear an active alarm
	    emit alertMsg(tr("%1 Red HEB Box AC power restored").arg(panelName),Qt::blue);
	  clearAlarm(HEB_R_ACPower);
	}
	else if (valStr.compare("OFF",Qt::CaseInsensitive)==0 || 
		 valStr.compare("UNKNOWN",Qt::CaseInsensitive)==0) {
	  sensorData[HEB_R_ACPower] = 0;
	  sensorDisplay[HEB_R_ACPower]->setNormal();
	  sensorDisplay[HEB_R_ACPower]->setText("OFF",Qt::white);
	  if (sensorAlarm[HEB_R_ACPower]) // about to clear an active alarm
	    emit alertMsg(tr("%1 Red HEB Box AC powered off normally").arg(panelName),Qt::blue);
	  clearAlarm(HEB_R_ACPower);
	}
	else if (valStr.compare("MANUAL",Qt::CaseInsensitive)==0) {
	  sensorData[HEB_R_ACPower] = 0;
	  sensorDisplay[HEB_R_ACPower]->setNormal();
	  sensorDisplay[HEB_R_ACPower]->setText("ON",QColor(255,126,0,255));
	  if (sensorAlarm[HEB_R_ACPower]) // about to clear an active alarm
	    emit alertMsg(tr("%1 Red HEB Box AC power restored with manual override").arg(panelName),Qt::blue);
	  clearAlarm(HEB_R_ACPower);
	}
	else if (valStr.compare("FAULT",Qt::CaseInsensitive)==0) {
	  sensorData[HEB_R_ACPower] = -1;
	  sensorDisplay[HEB_R_ACPower]->setText("OFF",Qt::red);
	  sensorDisplay[HEB_R_ACPower]->setFault();
	  if (setAlarm(HEB_R_ACPower))
	    emit alert(tr("%1 Red HEB AC Power Fault (breaker trip or thermal fault?)").arg(panelName));
	}
	numUpdated++;
      }
      
      // AGw Guide and WFS Camera controller Power State (allows for manual on)

      else if (keyStr.compare("GCAM_PWR",Qt::CaseInsensitive)==0 ||
	       keyStr.compare("AGC_BRK",Qt::CaseInsensitive)==0) {
	if (valStr.compare("ON",Qt::CaseInsensitive)==0 || valStr.compare("OK",Qt::CaseInsensitive)==0) {
	  sensorData[GCam_ACPower] = 1;
	  sensorDisplay[GCam_ACPower]->setNormal();
	  sensorDisplay[GCam_ACPower]->setText("ON",Qt::green);
	  if (sensorAlarm[GCam_ACPower]) // about to clear an active alarm
	    emit alertMsg(tr("%1 AGw Guide Camera controller AC power restored").arg(panelName),Qt::blue);
	  clearAlarm(GCam_ACPower);
	}
	else if (valStr.compare("OFF",Qt::CaseInsensitive)==0 || 
		 valStr.compare("UNKNOWN",Qt::CaseInsensitive)==0) {
	  sensorData[GCam_ACPower] = 0;
	  sensorDisplay[GCam_ACPower]->setNormal();
	  sensorDisplay[GCam_ACPower]->setText("OFF",Qt::white);
	  if (sensorAlarm[GCam_ACPower]) // about to clear an active alarm
	    emit alertMsg(tr("%1 AGw Guide Camera controller AC powered off normally").arg(panelName),Qt::blue);
	  clearAlarm(GCam_ACPower);
	}
	else if (valStr.compare("MANUAL",Qt::CaseInsensitive)==0) {
	  sensorData[GCam_ACPower] = 0;
	  sensorDisplay[GCam_ACPower]->setNormal();
	  sensorDisplay[GCam_ACPower]->setText("ON",QColor(255,126,0,255));
	  if (sensorAlarm[GCam_ACPower]) // about to clear an active alarm
	    emit alertMsg(tr("%1 AGw Guide Camera controller AC power restored with manual override").arg(panelName),Qt::blue);
	  clearAlarm(GCam_ACPower);
	}
	else if (valStr.compare("FAULT",Qt::CaseInsensitive)==0) {
	  sensorData[GCam_ACPower] = -1;
	  sensorDisplay[GCam_ACPower]->setText("OFF",Qt::red);
	  sensorDisplay[GCam_ACPower]->setFault();
	  if (setAlarm(GCam_ACPower))
	    emit alert(tr("%1 AGw Guide Camera controller Power Fault (breaker trip?)").arg(panelName));
	}
	numUpdated++;
      }
      
      else if (keyStr.compare("WFS_PWR",Qt::CaseInsensitive)==0 ||
	       keyStr.compare("WFS_BRK",Qt::CaseInsensitive)==0) {
	if (valStr.compare("ON",Qt::CaseInsensitive)==0 || valStr.compare("OK",Qt::CaseInsensitive)==0) {
	  sensorData[WFS_ACPower] = 1;
	  sensorDisplay[WFS_ACPower]->setNormal();
	  sensorDisplay[WFS_ACPower]->setText("ON",Qt::green);
	  if (sensorAlarm[WFS_ACPower]) // about to clear an active alarm
	    emit alertMsg(tr("%1 AGw WFS Camera controller AC power restored").arg(panelName),Qt::blue);
	  clearAlarm(WFS_ACPower);
	}
	else if (valStr.compare("OFF",Qt::CaseInsensitive)==0 || 
		 valStr.compare("UNKNOWN",Qt::CaseInsensitive)==0) {
	  sensorData[WFS_ACPower] = 0;
	  sensorDisplay[WFS_ACPower]->setNormal();
	  sensorDisplay[WFS_ACPower]->setText("OFF",Qt::white);
	  if (sensorAlarm[WFS_ACPower]) // about to clear an active alarm
	    emit alertMsg(tr("%1 AGw WFS Camera controller AC powered off normally").arg(panelName),Qt::blue);
	  clearAlarm(WFS_ACPower);
	}
	else if (valStr.compare("MANUAL",Qt::CaseInsensitive)==0) {
	  sensorData[WFS_ACPower] = 0;
	  sensorDisplay[WFS_ACPower]->setNormal();
	  sensorDisplay[WFS_ACPower]->setText("ON",QColor(255,126,0,255));
	  if (sensorAlarm[WFS_ACPower]) // about to clear an active alarm
	    emit alertMsg(tr("%1 AGw WFS Camera controller AC power restored with manual override").arg(panelName),Qt::blue);
	  clearAlarm(WFS_ACPower);
	}
	else if (valStr.compare("FAULT",Qt::CaseInsensitive)==0) {
	  sensorData[WFS_ACPower] = -1;
	  sensorDisplay[WFS_ACPower]->setText("OFF",Qt::red);
	  sensorDisplay[WFS_ACPower]->setFault();
	  if (setAlarm(WFS_ACPower))
	    emit alert(tr("%1 AGw WFS Camera controller Power Fault (breaker trip?)").arg(panelName));
	}
	numUpdated++;
      }
      
      // Future Expansion: add other instrument sensors would go here

      else {
	// unrecognized keyword, move along...
      }
      
    }
    else { // Message is from one of the ICs
      
      // HEB glycol return temperature

      if (keyStr.compare("CTEMPOUT",Qt::CaseInsensitive)==0) {
	double dtmp = valStr.toDouble(&ok);
	if (isRed) {
	  if (ok && dtmp < 200.0) {
	    sensorData[HEB_R_GRTemp] = dtmp;
	    sensorDisplay[HEB_R_GRTemp]->setText(QString::number(dtmp,'f',1),Qt::green);
	    sensorDisplay[HEB_R_GRTemp]->setNormal();
	    sensorData[HEB_R_DTGlycol] = sensorData[HEB_R_GRTemp]-sensorData[HEB_R_GSTemp];
	    sensorDisplay[HEB_R_DTGlycol]->setText(QString::number(sensorData[HEB_R_DTGlycol],
								   'f',1),Qt::green);
	    sensorDisplay[HEB_R_DTGlycol]->setNormal();
	  }
	  else {
	    sensorDisplay[HEB_R_GRTemp]->clear();
	    sensorDisplay[HEB_R_DTGlycol]->clear();
	  }
	}
	else {
	  if (ok && dtmp < 200.0) {
	    sensorData[HEB_B_GRTemp] = dtmp;
	    sensorDisplay[HEB_B_GRTemp]->setText(QString::number(dtmp,'f',1),Qt::green);
	    sensorDisplay[HEB_B_GRTemp]->setNormal();
	    sensorData[HEB_B_DTGlycol] = sensorData[HEB_B_GRTemp]-sensorData[HEB_B_GSTemp];
	    sensorDisplay[HEB_B_DTGlycol]->setText(QString::number(sensorData[HEB_B_DTGlycol],
								   'f',1),Qt::green);
	    sensorDisplay[HEB_B_DTGlycol]->setNormal();
	  }
	  else {
	    sensorDisplay[HEB_B_GRTemp]->clear();
	    sensorDisplay[HEB_B_DTGlycol]->clear();
	  }
	}
	numUpdated++;
      }
      
      else if (keyStr.compare("CTEMPIN",Qt::CaseInsensitive)==0) {
	double dtmp = valStr.toDouble(&ok);
	if (isRed) {
	  if (ok && dtmp < 200.0) {
	    sensorData[HEB_R_GSTemp] = dtmp;
	    sensorDisplay[HEB_R_GSTemp]->setText(QString::number(dtmp,'f',1),Qt::green);
	    sensorDisplay[HEB_R_GSTemp]->setNormal();
	    sensorData[HEB_R_DTGlycol] = sensorData[HEB_R_GRTemp]-sensorData[HEB_R_GSTemp];
	    sensorDisplay[HEB_R_DTGlycol]->setText(QString::number(sensorData[HEB_R_DTGlycol],
								   'f',1),Qt::green);
	    sensorDisplay[HEB_R_DTGlycol]->setNormal();
	  }
	  else {
	    sensorDisplay[HEB_R_GSTemp]->clear();
	    sensorDisplay[HEB_R_DTGlycol]->clear();
	  }
	}
	else {
	  if (ok && dtmp < 200.0) {
	    sensorData[HEB_B_GSTemp] = dtmp;
	    sensorDisplay[HEB_B_GSTemp]->setText(QString::number(dtmp,'f',1),Qt::green);
	    sensorDisplay[HEB_B_GSTemp]->setNormal();
	    sensorData[HEB_B_DTGlycol] = sensorData[HEB_B_GRTemp]-sensorData[HEB_B_GSTemp];
	    sensorDisplay[HEB_B_DTGlycol]->setText(QString::number(sensorData[HEB_B_DTGlycol],
								   'f',1),Qt::green);
	    sensorDisplay[HEB_B_DTGlycol]->setNormal();
	  }
	  else {
	    sensorDisplay[HEB_B_GSTemp]->clear();
	    sensorDisplay[HEB_B_DTGlycol]->clear();
	  }
	}
	numUpdated++;
      }
      
      // HEB pre-heat sink Air temperature
      
      else if (keyStr.compare("HEBTEMP",Qt::CaseInsensitive)==0) {
	double dtmp = valStr.toDouble(&ok);
	if (isRed) {
	  if (ok && dtmp < 200.0) {
	    sensorData[HEB_R_AirTemp] = dtmp;
	    sensorDisplay[HEB_R_AirTemp]->setText(QString::number(dtmp,'f',1),Qt::green);
	    sensorDisplay[HEB_R_AirTemp]->setNormal();
	    sensorData[HEB_R_DTAir] = sensorData[HEB_R_HSTemp]-sensorData[HEB_R_AirTemp];
	    sensorDisplay[HEB_R_DTAir]->setText(QString::number(sensorData[HEB_R_DTAir],
								   'f',1),Qt::green);
	    if (dtmp > MODS_MAX_BOXTEMP) {
	      sensorDisplay[HEB_R_AirTemp]->setFault();
	      if (setAlarm(HEB_R_AirTemp))
		emit alert(tr("%1 Red HEB inside air temperature of %2C exceeds %3C").arg(panelName).arg(QString::number(dtmp,'f',1)).arg(MODS_MAX_BOXTEMP));
	    }
	    else {
	      sensorDisplay[HEB_R_AirTemp]->setNormal();
	      if (sensorAlarm[HEB_R_AirTemp]) // about to clear an active alarm
		emit alertMsg(tr("%1 Red HEB air temperature is back in normal range").arg(panelName),Qt::blue);
	      clearAlarm(HEB_R_AirTemp);
	    }
	  }
	  else {
	    sensorDisplay[HEB_R_AirTemp]->clear();
	    sensorDisplay[HEB_R_DTAir]->clear();
	    clearAlarm(HEB_R_AirTemp);
	  }
	}
	else {
	  if (ok && dtmp < 200.0) {
	    sensorData[HEB_B_AirTemp] = dtmp;
	    sensorDisplay[HEB_B_AirTemp]->setText(QString::number(dtmp,'f',1),Qt::green);
	    sensorDisplay[HEB_B_AirTemp]->setNormal();
	    sensorData[HEB_B_DTAir] = sensorData[HEB_B_HSTemp]-sensorData[HEB_B_AirTemp];
	    sensorDisplay[HEB_B_DTAir]->setText(QString::number(sensorData[HEB_B_DTAir],
								   'f',1),Qt::green);
	    if (dtmp > MODS_MAX_BOXTEMP) {
	      sensorDisplay[HEB_B_AirTemp]->setFault();
	      if (setAlarm(HEB_B_AirTemp))
		emit alert(tr("%1 Blue HEB inside air temperature of %2C exceeds %3C").arg(panelName).arg(QString::number(dtmp,'f',1)).arg(MODS_MAX_BOXTEMP));
	    }
	    else {
	      sensorDisplay[HEB_B_AirTemp]->setNormal();
	      if (sensorAlarm[HEB_B_AirTemp]) // about to clear an active alarm
		emit alertMsg(tr("%1 Blue HEB air temperature is back in normal range").arg(panelName),Qt::blue);
	      clearAlarm(HEB_B_AirTemp);
	    }
	  }
	  else {
	    sensorDisplay[HEB_B_AirTemp]->clear();
	    sensorDisplay[HEB_B_DTAir]->clear();
	    clearAlarm(HEB_B_AirTemp);
	  }
	}
	numUpdated++;
      }
      
      // HEB post-heat sink Air temperature
      
      else if (keyStr.compare("HSTEMP",Qt::CaseInsensitive)==0) {
	double dtmp = valStr.toDouble(&ok);
	if (isRed) {
	  if (ok && dtmp < 200.0) {
	    sensorData[HEB_R_HSTemp] = dtmp;
	    sensorDisplay[HEB_R_HSTemp]->setText(QString::number(dtmp,'f',1),Qt::green);
	    sensorDisplay[HEB_R_HSTemp]->setNormal();
	    sensorData[HEB_R_DTAir] = sensorData[HEB_R_HSTemp]-sensorData[HEB_R_AirTemp];
	    sensorDisplay[HEB_R_DTAir]->setText(QString::number(sensorData[HEB_R_DTAir],
								   'f',1),Qt::green);
	    if (dtmp > MODS_MAX_BOXTEMP) {
	      sensorDisplay[HEB_R_HSTemp]->setFault();
	      if (setAlarm(HEB_R_HSTemp))
		emit alert(tr("%1 Red HEB heat sink temperature of %2C exceeds %3C").arg(panelName).arg(QString::number(dtmp,'f',1)).arg(MODS_MAX_BOXTEMP));
	    }
	    else {
	      sensorDisplay[HEB_R_HSTemp]->setNormal();
	      if (sensorAlarm[HEB_R_HSTemp]) // about to clear an active alarm
		emit alertMsg(tr("%1 Red HEB heat sink temperature is back in normal range").arg(panelName),Qt::blue);
	      clearAlarm(HEB_R_HSTemp);
	    }
	  }
	  else {
	    sensorDisplay[HEB_R_HSTemp]->clear();
	    sensorDisplay[HEB_R_DTAir]->clear();
	    clearAlarm(HEB_R_HSTemp);
	  }
	}
	else {
	  if (ok && dtmp < 200.0) {
	    sensorData[HEB_B_HSTemp] = dtmp;
	    sensorDisplay[HEB_B_HSTemp]->setText(QString::number(dtmp,'f',1),Qt::green);
	    sensorDisplay[HEB_B_HSTemp]->setNormal();
	    sensorData[HEB_B_DTAir] = sensorData[HEB_B_HSTemp]-sensorData[HEB_B_AirTemp];
	    sensorDisplay[HEB_B_DTAir]->setText(QString::number(sensorData[HEB_B_DTAir],
								   'f',1),Qt::green);
	    if (dtmp > MODS_MAX_BOXTEMP) {
	      sensorDisplay[HEB_B_HSTemp]->setFault();
	      if (setAlarm(HEB_B_HSTemp))
		emit alert(tr("%1 Blue HEB heat sink temperature of %2C exceeds %3C").arg(panelName).arg(QString::number(dtmp,'f',1)).arg(MODS_MAX_BOXTEMP));
	    }
	    else {
	      sensorDisplay[HEB_B_HSTemp]->setNormal();
	      if (sensorAlarm[HEB_B_HSTemp]) // about to clear an active alarm
		emit alertMsg(tr("%1 Blue HEB heat sink temperature is back in normal range").arg(panelName),Qt::blue);
	      clearAlarm(HEB_B_HSTemp);
	    }
	  }
	  else {
	    sensorDisplay[HEB_B_HSTemp]->clear();
	    sensorDisplay[HEB_B_DTAir]->clear();
	    clearAlarm(HEB_B_HSTemp);
	  }
	}
	numUpdated++;
      }
      
      // CCD Dewar Pressure in torr, display in scientific notation as x.yzE-ab
      
      else if (keyStr.compare("DEWPRES",Qt::CaseInsensitive)==0) {
	double dtmp = valStr.toDouble(&ok);
	if (isRed) {
	  if (ok) {
	    sensorData[Red_DewPres] = dtmp;
	    sensorDisplay[Red_DewPres]->setText(QString::number(dtmp,'E',2),Qt::green);
	    if (dtmp > MODS_MAX_DEWPRES) {
	      sensorDisplay[Red_DewPres]->setFault();
	      if (setAlarm(Red_DewPres))
		emit alert(tr("%1 Red CCD dewar pressure of %2 torr exceeds %3 torr").arg(panelName).arg(QString::number(dtmp,'E',2)).arg(MODS_MAX_DEWPRES));
	    }
	    else {
	      sensorDisplay[Red_DewPres]->setNormal();
	      if (sensorAlarm[Red_DewPres]) // about to clear an active alarm
		emit alertMsg(tr("%1 Red CCD dewar pressure is back in normal range").arg(panelName),Qt::blue);
	      clearAlarm(Red_DewPres);
	    }
	  }
	  else {
	    sensorDisplay[Red_DewPres]->clear();
	    clearAlarm(Red_DewPres);
	  }
	}
	else {
	  if (ok) {
	    sensorData[Blue_DewPres] = dtmp;
	    sensorDisplay[Blue_DewPres]->setText(QString::number(dtmp,'E',2),Qt::green);
	    sensorDisplay[Blue_DewPres]->setNormal();
	    if (dtmp > MODS_MAX_DEWPRES) {
	      sensorDisplay[Blue_DewPres]->setFault();
	      if (setAlarm(Blue_DewPres))
		emit alert(tr("%1 Blue CCD dewar pressure of %2 torr exceeds %3 torr").arg(panelName).arg(QString::number(dtmp,'E',2)).arg(MODS_MAX_DEWPRES));
	    }
	    else {
	      sensorDisplay[Blue_DewPres]->setNormal();
	      if (sensorAlarm[Blue_DewPres]) // about to clear an active alarm
		emit alertMsg(tr("%1 Blue CCD dewar pressure is back in normal range").arg(panelName),Qt::blue);
	      clearAlarm(Blue_DewPres);
	    }
	  }
	  else {
	    sensorDisplay[Blue_DewPres]->clear();
	    clearAlarm(Blue_DewPres);
	  }
	}
	numUpdated++;
      }

      // CCD Detector working plate temperature
      
      else if (keyStr.compare("CCDTEMP",Qt::CaseInsensitive)==0) {
	double dtmp = valStr.toDouble(&ok);
	if (isRed) {
	  if (ok) {
	    sensorData[Red_CCDTemp] = dtmp;
	    sensorDisplay[Red_CCDTemp]->setText(QString::number(dtmp,'f',1),Qt::green);
	    sensorDisplay[Red_CCDTemp]->setNormal();
	    if (dtmp > MODS_MAX_CCDTEMP) {
	      sensorDisplay[Red_CCDTemp]->setFault();
	      if (setAlarm(Red_CCDTemp))
		emit alert(tr("%1 Red CCD temperature of %2C exceeds %3C - detector warming up!").arg(panelName).arg(QString::number(dtmp,'f',1)).arg(MODS_MAX_CCDTEMP));
	    }
	    else {
	      sensorDisplay[Red_CCDTemp]->setNormal();
	      if (sensorAlarm[Red_CCDTemp]) // about to clear an active alarm
		emit alertMsg(tr("%1 Red CCD temperature is back in normal range").arg(panelName),Qt::blue);
	      clearAlarm(Red_CCDTemp);
	    }
	  }
	  else {
	    sensorDisplay[Red_CCDTemp]->clear();
	    clearAlarm(Red_CCDTemp);
	  }
	  numUpdated++;
	}
	else {
	  if (ok) {
	    sensorData[Blue_CCDTemp] = dtmp;
	    sensorDisplay[Blue_CCDTemp]->setText(QString::number(dtmp,'f',1),Qt::green);
	    if (dtmp > MODS_MAX_CCDTEMP) {
	      sensorDisplay[Blue_CCDTemp]->setFault();
	      if (setAlarm(Blue_CCDTemp))
		emit alert(tr("%1 Blue CCD temperature of %2C exceeds %3C - detector warming up!").arg(panelName).arg(QString::number(dtmp,'f',1)).arg(MODS_MAX_CCDTEMP));
	    }
	    else {
	      sensorDisplay[Blue_CCDTemp]->setNormal();
	      if (sensorAlarm[Blue_CCDTemp]) // about to clear an active alarm
		emit alertMsg(tr("%1 Blue CCD temperature is back in normal range").arg(panelName),Qt::blue);
	      clearAlarm(Blue_CCDTemp);
	    }
	  }
	  else {
	    sensorDisplay[Blue_CCDTemp]->clear();
	    clearAlarm(Blue_CCDTemp);
	  }
	  numUpdated++;
	}
      }

      // CCD Dewar LN2 Tank temperature
      
      else if (keyStr.compare("DEWTEMP",Qt::CaseInsensitive)==0) {
	double dtmp = valStr.toDouble(&ok);
	if (isRed) {
	  if (ok) {
	    sensorData[Red_DewTemp] = dtmp;
	    sensorDisplay[Red_DewTemp]->setText(QString::number(dtmp,'f',1),Qt::green);
	    sensorDisplay[Red_DewTemp]->setNormal();
	    if (dtmp > MODS_MAX_DEWTEMP) {
	      sensorDisplay[Red_DewTemp]->setFault();
	      if (setAlarm(Red_DewTemp))
		emit alert(tr("%1 Red Dewar temperature of %2C exceeds %3C - is dewar out of LN2?").arg(panelName).arg(QString::number(dtmp,'f',1)).arg(MODS_MAX_DEWTEMP));
	    }
	    else {
	      sensorDisplay[Red_DewTemp]->setNormal();
	      if (sensorAlarm[Red_DewTemp]) // about to clear an active alarm
		emit alertMsg(tr("%1 Red Dewar temperature is back in normal range").arg(panelName),Qt::blue);
	      clearAlarm(Red_DewTemp);
	    }
	  }
	  else {
	    sensorDisplay[Red_DewTemp]->clear();
	    clearAlarm(Red_DewTemp);
	  }
	  numUpdated++;
	}
	else {
	  if (ok) {
	    sensorData[Blue_DewTemp] = dtmp;
	    sensorDisplay[Blue_DewTemp]->setText(QString::number(dtmp,'f',1),Qt::green);
	    sensorDisplay[Blue_DewTemp]->setNormal();
	    if (dtmp > MODS_MAX_DEWTEMP) {
	      sensorDisplay[Blue_DewTemp]->setFault();
	      if (setAlarm(Blue_DewTemp))
		emit alert(tr("%1 Blue Dewar temperature of %2C exceeds %3C - is dewar out of LN2?").arg(panelName).arg(QString::number(dtmp,'f',1)).arg(MODS_MAX_DEWTEMP));
	    }
	    else {
	      sensorDisplay[Blue_DewTemp]->setNormal();
	      if (sensorAlarm[Blue_DewTemp]) // about to clear an active alarm
		emit alertMsg(tr("%1 Blue Dewar temperature is back in normal range").arg(panelName),Qt::blue);
	      clearAlarm(Blue_DewTemp);
	    }
	  }
	  else {
	    sensorDisplay[Blue_DewTemp]->clear();
	    clearAlarm(Blue_DewTemp);
	  }
	  numUpdated++;
	}
      }
     
      // Thermo-Electric Device (aka Cooler) Power State

      else if (keyStr.compare("TEDPOWER",Qt::CaseInsensitive)==0) {
	if (isRed) {
	  if (valStr.compare("ON",Qt::CaseInsensitive)==0) {
	    sensorData[Red_TEDPower] = 1;
	    sensorDisplay[Red_TEDPower]->setNormal();
	    sensorDisplay[Red_TEDPower]->setText("ON",Qt::green);
	    clearAlarm(Red_TEDPower);
	  }
	  else if (valStr.compare("OFF",Qt::CaseInsensitive)==0) {
	    sensorData[Red_TEDPower] = 0;
	    sensorDisplay[Red_TEDPower]->setNormal();
	    sensorDisplay[Red_TEDPower]->setText("OFF",QColor(255,126,0,255));
	    clearAlarm(Red_TEDPower);
	  }
	  else {	
	    sensorData[Red_TEDPower] = -1;
	    sensorDisplay[Red_TEDPower]->clear();
	    clearAlarm(Red_TEDPower);
	  }
	  numUpdated++;
	}
	else {
	  if (valStr.compare("ON",Qt::CaseInsensitive)==0) {
	    sensorData[Blue_TEDPower] = 1;
	    sensorDisplay[Blue_TEDPower]->setNormal();
	    sensorDisplay[Blue_TEDPower]->setText("ON",Qt::green);
	    clearAlarm(Blue_TEDPower);
	  }
	  else if (valStr.compare("OFF",Qt::CaseInsensitive)==0) {
	    sensorData[Blue_TEDPower] = 0;
	    sensorDisplay[Blue_TEDPower]->setNormal();
	    sensorDisplay[Blue_TEDPower]->setText("OFF",QColor(255,126,0,255));
	    clearAlarm(Blue_TEDPower);
	  }
	  else {
	    sensorData[Blue_TEDPower] = -1;
	    sensorDisplay[Blue_TEDPower]->clear();
	    clearAlarm(Blue_TEDPower);
	  }
	  numUpdated++;
	}
      }
     
      // CCD controller sequencer state (T=online/F=offline)

      else if (keyStr.compare("SEQ",Qt::CaseInsensitive)==0) {
	if (isRed) {
	  if (valStr.compare("T",Qt::CaseInsensitive)==0) {
	    sensorData[Red_Sequencer] = 1;
	    sensorDisplay[Red_Sequencer]->setNormal();
	    sensorDisplay[Red_Sequencer]->setText("ON",Qt::green);
	    if (sensorAlarm[Red_Sequencer]) // about to clear an active alarm
		emit alertMsg(tr("%1 Red CCD Sequencer is back online.").arg(panelName),Qt::blue);
	    clearAlarm(Red_Sequencer);
	  }
	  else if (valStr.compare("F",Qt::CaseInsensitive)==0) {
	    sensorData[Red_Sequencer] = 0;
	    sensorDisplay[Red_Sequencer]->setFault();
	    sensorDisplay[Red_Sequencer]->setText("OFF",Qt::red);
	    if (setAlarm(Red_Sequencer))
	      emit alert(tr("%1 Red CCD Sequencer is offline.").arg(panelName));
	  }
	  numUpdated++;
	}
	else {
	  if (valStr.compare("T",Qt::CaseInsensitive)==0) {
	    sensorData[Blue_Sequencer] = 1;
	    sensorDisplay[Blue_Sequencer]->setNormal();
	    sensorDisplay[Blue_Sequencer]->setText("ON",Qt::green);
	    if (sensorAlarm[Blue_Sequencer]) // about to clear an active alarm
		emit alertMsg(tr("%1 Blue CCD Sequencer is back online.").arg(panelName),Qt::blue);
	    clearAlarm(Blue_Sequencer);
	  }
	  else if (valStr.compare("F",Qt::CaseInsensitive)==0) {
	    sensorData[Blue_Sequencer] = 0;
	    sensorDisplay[Blue_Sequencer]->setFault();
	    sensorDisplay[Blue_Sequencer]->setText("OFF",Qt::red);
	    if (setAlarm(Blue_Sequencer))
	      emit alert(tr("%1 Blue CCD Sequencer is offline.").arg(panelName));
	  }
	  numUpdated++;
	}
      }
     
      else {
	// unrecognized keyword, move along...
      }

    }

    // Any alarms triggered?

    int numAlerts = 0;
    for (int i=0;i<NUMSENSORS;i++)
      if (sensorAlarm[i]) numAlerts++;
    if (numAlerts==0) 
      emit cancelAlerts(); // no active alerts on any channel

    // Did we update any of the panel?  If so, mark the time, and
    // trigger the appropriate log

    if (numUpdated>0) {
      QDateTime dateTime = QDateTime::currentDateTime().toUTC();
      updateDisplay->setText(dateTime.toString("HH:mm:ss"));
    }

  } // Bottom of the hash iterator
   
}

// Update the control panel

void EnvPanel::update()
{
  // Clear the readout displays

  for (int i=0;i<NUMSENSORS;i++)
    sensorDisplay[i]->clear();

  // Queue the various instrument environmental sensor status queries
  // This will make sure all alarms are fresh

  emit statusMsg(tr("Updating %1 utility status...").arg(panelName),Qt::blue);
  sendCmdWait(modsIMHost[modsID],"ESTATUS",MODS_QUEUE_REQUEST);
  sendCmdWait(modsBCHost[modsID],"ESTATUS",MODS_QUEUE_REQUEST);
  sendCmdWait(modsRCHost[modsID],"ESTATUS",MODS_QUEUE_REQUEST);
  sendCmdWait(modsBCHost[modsID],"STATUS",MODS_QUEUE_REQUEST);
  sendCmdWait(modsRCHost[modsID],"STATUS",MODS_QUEUE_REQUEST);
}

// Enable the utility box control panel

void EnvPanel::enable()
{
  setDisabled(false);
}

// Disable the utility box control panel

void EnvPanel::disable()
{
  setDisabled(true);
}

//
// Set an alarm condition - this handles repeat message suppression
//
// Return true if make a report, false if no report
// 
// See clearAlarm() to clear an alarm condition
//

bool EnvPanel::setAlarm(const int &alarmID) 
{
  if (alarmID < 0 || alarmID >= NUMSENSORS) return false;
  if (sensorAlarm[alarmID]) { // repeat alarm
    if (numAlarmRepeats[alarmID] < MODS_ALARM_REPEAT) { // suppress up to MODS_ALARM_REPEAT repetitions
      numAlarmRepeats[alarmID]++;
      return false;
    }
    else {
      numAlarmRepeats[alarmID] = 0;  // time to repeat the alarm
      return true;
    }
  }
  else { // new alarm
    sensorAlarm[alarmID] = true;
    numAlarmRepeats[alarmID] = 0;
    return true;
  }
}

//
// Clear an alarm condition
//
// See setAlarm() for how to set an alarm
//

void EnvPanel::clearAlarm(const int &alarmID)
{
  if (alarmID < 0 || alarmID >= NUMSENSORS) return;
  sensorAlarm[alarmID] = false;
  numAlarmRepeats[alarmID] = 0;
}

// Clear all alarms 

void EnvPanel::clearAll()
{
  for (int i=0;i<NUMSENSORS;i++) 
    clearAlarm(i);
}
