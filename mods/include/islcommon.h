#ifndef ISLCOMMON_H
#define ISLCOMMON_H

//
// islcommon.h - ISL common for SHRMEM.
//
 
/*!
  \file islcommon.h
  \brief SHRMEM structure for all ISL and other non-application header
 
  ISL common SHRMEM header for all who need it or want it.
 
  \date 2005 May 05 [rdg]

  \date 2025 June - AlmaLinux 9 port and switching to a WAGO-based
                    IMCS quadcell readout system [rwp/osu]

  Note: ttyport_t is defined in instrutils.h

*/
 
// Various site-dependent but system-independent default values
 
// ISL shared memory layout

typedef struct islcommon {
  long nums[MAX_CLS];

  // This will allow us to find out if there was a reboot or 
  // a computer crash has been detected.
  int COMPUTER_RESET; // Shared Memory or Common has been reset
                      // Computer has been rebooted

  // ISL flags
  
  long iclbox;
  long iclopr;
  int KHALT;
  int KECHO;

  struct {

    // Instrument Electronix Boxes(IEB) for a MODS instance

    float  reqpos[MAX_ML];      // Requested Mechanism position
    float  pos[MAX_ML];         // Actual Mechanism position
    int    ieb_i[MAX_ML];       // Mechanism IEB assignment
    int    host[MAX_ML];        // MicroLynx Controller hosting
    int    busy[MAX_ML];        //            "         busy
    int    qued[MAX_ML];        // Qued service
    char   who[MAX_ML][10];     //            "         IEB1 mechanism name
    char   ieb_msg[MAX_ML][80]; // Message holders for IEBs
    char   TTYIP[MAX_ML][64];   // Comtrol Box IP's and Ports
    char   WAGOIP[6][64];       // WAGO tcp/modbus controller IP's and Ports
    char   WAGOWHO[6][24];      // WAGO tcp/modbus controller names
    int    portcnt;
    int    loginfo[2];          // IMCS logging (blue/red channel)
    char   LLOG[79];
    long   logchg;
    int    erchk;
    int    active_smask;
    int    blueCloseLoop;       // BLUE Closed Loop
    int    blueCloseLoopON;     // BLUE On/Off Target
    int    redCloseLoop;        // RED Closed Loop index 1=Blue index 2=Red
    int    redCloseLoopON;      // RED On/Off Target index 1=Blue index 2=Red
    float  motorv[MAX_ML];
    float  blueQC_Gain;         // quad cell correction gain factors (usually 1)
    float  redQC_Gain;
    float  blueQC_X[2];         // BLUE TTFA Delta
    float  blueQC_Y[2];         // BLUE TTFB Delta
    float  blueQC_Z[2];         // BLUE TTFC -Delta
    float  redQC_X[2];          // RED TTFA Delta
    float  redQC_Y[2];          // RED TTFB Delta
    float  redQC_Z[2];          // RED TTFC -Delta
    float  qc_MAXTTPmove;       // IMCS max collimator TTF correction move
    float  qc_TTPustep;         // IMCS collimator TTF microstep
    float  blueQC[4];           // BLUE IMCS Quad Cells array(QC1,QC2,QC3,QC4)
    float  blueQC_Average[4];   // BLUE average IMCS Quad Cells array
    float  redQC[4];            // RED IMCS Quad Cells array(QC1,QC2,QC3,QC4)
    float  redQC_Average[4];    // RED average IMCS Q,uad Cells array
    int    standalone;
    char   maskselect;          // B = busy, S = not busy
    char   maskinsert;          // B = busy, S = not busy
    char   maskpos[24];         // Mask positions - STOW,IN,EMPTY,UNKNOWN
    char   maskinfo[40];        // Mask Information - string name, UNKNOWN
    char   QC_IP[MAX_QC][64];   // IMCS quad cell WAGO IP address
    int    QC_REG[MAX_QC];      // IMCS quad cell WAGO analog input register address
    char   QC_WHO[MAX_QC][24];  // IMCS system ID (e.g. bimcs, rimcs)
    int    blueQC_SampleRate;   // BLUE Quadcell sample rate
    int    redQC_SampleRate;    // RED Quadcell sample rate
    int    blueQC_TARGET;       // BLUE ON target
    int    redQC_TARGET;        // RED ON target
    int    blueQC_Samples;      // BLUE Number of Quadcell value to average.
    int    redQC_Samples;       // REd Number of Quadcell value to average.
    int    blueQC1;             // BLUE Quadcell 1 value from HEB.
    int    blueQC2;             // BLUE Quadcell 2 value from HEB.
    int    blueQC3;             // BLUE Quadcell 3 value from HEB.
    int    blueQC4;             // BLUE Quadcell 4 value from HEB.
    int    redQC1;              // RED Quadcell 1 value from HEB.
    int    redQC2;              // RED Quadcell 2 value from HEB.
    int    redQC3;              // RED Quadcell 3 value from HEB.
    int    redQC4;              // RED Quadcell 4 value from HEB.
    float  blueQC_Focus[3];     // BLUE Collimator Focus Values 
    float  redQC_Focus[3];      // RED Collimator Focus Values 
    float  blueTemperature[4];  // Blue IEB temperature sensors [BoxAir,GlycolReturn,CollTop,CollBottom]
    float  redTemperature[4];   // Red IEB temperature sensors [BoxAir,GlycolReturn,AirTop,AirBottom]
    float  blueQC_Threshold[2]; // BLUE IMCS collimator sample threshold
    float  redQC_Threshold[2];  // RED IMCS collimator sample threshold
    int    blueIMCS_OnOff;      // BLUE IMCS unit on/off
    int    redIMCS_OnOff;       // RED IMCS unit on/off
    int    modsx;               // 0 = NON-MODS, 1 = MODS(1), 2 = MODS(2)
    int    key_init;   
    int    mlunit;
    int    timeout[MAX_ML];     // MicroLynx mechanism timeouts
    float  min[MAX_ML];         // mininum value
    float  max[MAX_ML];         // maximum value
    float  convf[MAX_ML];       // conversion factor
    bool   LOCKS[MAX_ML];       // Mechanism Locks F=Unlocked T=locked
    int    mmcServerCounter;
    
    float  Current[4];          // IEB Currents
    float  iebTemp[4];          // IEB Temperatures

    ttyport_t commport[MAX_ML]; // Comtrol to MicroLynx TCP/IP communication
    ttyport_t wagoport[6];      // WAGO TCP/IP Port communication

    // For tty ports if needed
    
    int modsPorts[4];  // 0=IIFAgent, 1=AGW Stage, 2=MMC, 3=other

    // AGW Filter Wheel names and info
    
    char agwfilters[5][40];    // AGW Filter Wheel filter ID, index 0 = timestamp
    char agwfiltInfo[5][128];  // AGW Filter Wheel info, index 0 = timestamp

    // Red Camera Filter Wheel names and info

    char rcamfilters[9][40];   // Red Camera Filter ID, index 0 = timestamp
    char rcamfiltInfo[9][128]; // Red Camera Filter info, index 0 = timestamp

    // Blue Camera Filter Wheel names and info
    
    char bcamfilters[9][40];   // Blue Camera Filter ID, index 0 = timestamp
    char bcamfiltInfo[9][128]; // Blue Camera Filter info, index 0 = timestamp

    // Red Grating Select Turret IDs and info
    
    char rgrating[5][40];   // Red Grating Select Name, index 0 = timestamp
    char rgratInfo[5][128]; // Red Grating Select Info, index 0 = timestamp

    // Blue Grating Select Turret IDs and info
    
    char bgrating[5][40];   // Blue Grating Select Name, index 0 = timestamp
    char bgratInfo[5][128]; // Blue Grating Select Info, index 0 = timestamp

    // Dichroic Select Turret
    
    char dichroicName[4][40];  // Dichroic Select Name, index 0 = timestamp 
    char dichroicInfo[4][128]; // Dichroic Select Info, index 0 = timestamp

    // Slit Mask ID and info
    
    char slitmaskName[25][80];  // mask name, index 0 = timestamp
    char slitmaskInfo[25][128]; // mask info, index 0 = timestamp

    // Head Electronix Boxes(HEB) for a MODS instance

    double heb_d[MAX_ML];         // Head Electronix Boxes double's deposit
    float  heb_f[MAX_ML];         //          "            float's    "
    int    heb_i[MAX_ML];         //          "            int's      "
    char   heb_msg[MAX_ML][80];   // Message holders for HEBs

    // WAGO address defines for a MODS instance
    //   Lamps and Laser Boxes (llb)
    //   Auxilliary Power Boxes (apb) - never used

    int    WAGOBASE_ADDR[6];    // BASE address for WAGO systems
    int    WAGOMONITOR_ADDR[6]; // Address for WAGO system monitors
    int    WAGOTEMP_ADDR[6];    // Address for WAGO temperatures
    int    WAGOLLB_ADDR[6];     // Address for WAGO LLB's
    int    WAGOAPB_ADDR[6];     // Address for WAGO APB's
    int    WAGOMISC_ADDR[6];    // Address for WAGO Misc...

    // Extras
    
    float  LLB_f[MAX_ML];       // Lamp and Laser Box float's deposit
    int    LLB_i[MAX_ML];       //          "           int's deposit
    char   LLB_msg[MAX_ML][80]; // Message holders for LLB
    float  UTIL_f[MAX_ML];      // UTIL Power Box float's deposit

    // Instrument Utility Box (IUB) sensors
    
    float  glycolSupplyPressure;     // glycol supply pressure in psi-g
    float  glycolReturnPressure;     // glycol return pressure  in psi-g
    float  glycolSupplyTemperature;  // glycol supply temperature in C
    float  glycolReturnTemperature;  // glycol return temperature in C
    float  utilBoxAirTemperature;    // IUB inside box air temperature in C
    float  outsideAirTemperature;    // IUB outside box air temperature in C
    float  agwHeatSinkTemperature;   // AGW controller heat sink temperature in C
    
    int    UTIL_i[MAX_ML];       //         "      int's deposit
    char   UTIL_msg[MAX_ML][80]; // Message holders for UTIL

    // WAGO HEB temperatures (new in 2025)

    float  redHEBTemperature;   // Red HEB inside box air temperature (C)
    float  redDewarTemperature; // Red CCD dewar LN2 reservoir temperature (C)
    float  redDewarPressure;    // Red CCD dewar vacuum pressure (torr)
    
    float  blueHEBTemperature;   // Blue HEB inside box air temperature (C)
    float  blueDewarTemperature; // Blue CCD dewar LN2 reservoir temperature (C)
    float  blueDewarPressure;    // Blue CCD dewar vacuum pressure (torr)

    // IMCS lasers data structure
    
    struct {

      // IMCS IR Laser
      
      int irlaser_state;            // IR Laser state
      int irbeam_state;             // IR Laser Beam state
      int irlaser_timeon;           // How long has it been ON 
      int irlaser_age;              // How long since it was replaced
      int irlaser_cycle;            // How many times has it been turned ON and OFF
      float irlaser_power;          // IR Laser Power
      float irlaser_setpoint;       // IR Laser Power Setpoint
      float irlaser_temp;           // IR Laser Temperature
      float irlaser_tempSet;        // IR Laser Temperature Setpoint

      // Coefficients for converting between physical units and WAGO units.

      int irlaserRegToPSetNCoeff;
      float irlaserRegToPSetCoeff[5];
      int irlaserRegToPOutNCoeff;
      float irlaserRegToPOutCoeff[5];
      int irlaserRegToTSetNCoeff;
      float irlaserRegToTSetCoeff[5];
      int irlaserRegToTOutNCoeff;
      float irlaserRegToTOutCoeff[5];
      int irlaserPSetToRNCoeff;
      float irlaserPSetToRCoeff[5];

      float irlaser_maxPower;      // Maximum power that a user can set for the IR Laser
      float irlaser_minPower;      // Minumum power that a user can set for the IR Laser

      int irlaser_spares[3];       // Spares for IR laser
      
      // Visible-light alignment laser

      int vislaser_state;       // Visiable Laser state
      int visbeam_state;        // Visiable Laser Beam state
      int vislaser_timeon;      // How long has it been ON
      int vislaser_age;         // How long since it was replaced
      int vislaser_cycle;       // How many times has it been turned ON and OFF

      float vislaser_maxPower;  // Maximum power that a user can request for the visible laser
      float vislaser_minPower;  // Maximum power that a user can request for the visible laser

      float vislaser_power;     // Visible Laser Power
      float vislaser_setpoint;  // Visible Laser Power

      // Coefficients for converting between physical units and WAGO units

      int vislaserRegToPSetNCoeff;
      float vislaserRegToPSetCoeff[5];
      int vislaserRegToPOutNCoeff;
      float vislaserRegToPOutCoeff[5];
      int vislaserPSetToRNCoeff;
      float vislaserPSetToRCoeff[5];

      int vislaser_spares[3];   // Spares for VIS laser

    } lasers;

    // Calibration lamps
    
    struct {
      int lamp_state[9];        // Lamps status
      int lamp_timeon[9];       // How long has it been ON
      int lamp_age[9];          // How long since it was replaced
      int lamp_cycle[9];        // How many times has it been turned ON and OFF
      int lamp_all;             // Lamps ON/OFF monitor
      short lamplaser_all[1];   // Lamps and/or Lasers ON/OFF monitor
      short int lampLaser[6];   // WAGO bit words lamps and lasers status
                                // BLUE IEB,RED IEB,LLB,UTIL....
    } lamps;

    float  vflat_power;         // VFLAT Lamps Power

    short int temps[30];        // temp for WAGO address information.
    char REPLY[MAX_ML][120];
    char state_word[MAX_ML][8]; // Mechanism word states IN,OUT,CLOSE..etc
    char hkUpdate[80];          // mmcHouseKeeker updated time.
    char mmcUpdate1[80];        // Another Timer if needed.
    char mmcUpdate2[80];        // Another Timer if needed.

    int b_imcsNominal[3];// BLUE ttfA, ttfB, ttfC respectively nominal position
    int r_imcsNominal[3];// RED ttfA, ttfB, ttfC respectively nominal position
    int b_camfocNominal; // BLUE Camera Focus nominal position
    int r_camfocNominal; // RED Camera Focus nominal position
    int b_gtiltNominal;  // BLUE Grating Tilt nominal position
    int r_gtiltNominal;  // RED  Grating Tilt nominal position

    double deposit_d[MAX_ML];     // A deposit of doubles

    int wagoR_Addr[40];
    int wagoRW_Addr[40];
    char wagoWho_Addr[40][80];

    // AGW state

    int agwState;

    // IUB, LLB, and red/blue IEB, HEB, Archon, and IonGauge power states

    int utilState;
    int llbState;
    int blueIEBState;
    int redIEBState;
    int blueHEBState;
    int redHEBState;
    int redArchonState;
    int blueArchonState;
    int redIonGaugeState;
    int blueIonGaugeState;
     
    // Calibration and Observing Mode, CALMODE = 1, OBSMODE = 0

    int instrMode;

    // AGW XGP and YGP saved position

    float xgpLast;
    float ygpLast;

  } MODS;

  // Semphors (not implemented)
  
  struct {
    int allocated; 
    char name[SEM_NUM][7];
  } go;

  struct {
    int allocated;
    char name[SEM_NUM][7];
  } sem;

  // times

  struct {
    float rate[2];
    long offset[2];
    long epoch[2];
    long span[2];
    long secs_off;
    int index;
    int icomputer[2];
    char model;
  } time;

  int islerr;
  int mclass_count;

  struct {
    int normal_end;
    int other_error;
  } abend;

  char system_argv[80];   // arguments passing for system or isl tasks 
  char lsorna[10];        // Source Name
  
  int agwTRAK;            // [0] 0 = not-track, 1 = tracking
  int tcsTRAK;            // [0] 0 = not-track, 1 = tracking

  /***************************************************************** */

  // Telescope parameters (not implemented)

  float RAOFF;
  float DECOFF;
  float AZOFF;
  float ELOFF;

  // AGW offsets in celestial, alt/az, and focal plane XY coords

  float agwRAOFF;         // Right Ascension offset
  float agwDECOFF;        // Declination offset
  float agwAZOFF;         // Azimuth offset
  float agwELOFF;         // Elevation offset

  float XOFF;
  float YOFF;

  // Celestial coordinates (unimplemented)

  float ep1950;
  float epoch;

  double ra50;
  double dec50;
  double radat;
  double decdat;
  double alat;
  double wlong;

  // TCS/IIF/CSQ values.

  char targRA[16];  //!< Target RA (hh:mm:ss.sss format)
  char targDec[16]; //!< Target Dec (hh:mm:ss.sss format)
  char instID[12]; //!< Instrument ID (e.g, "MODS1")
  char focalStation[64]; //!< Active focal station and registered side of instID (e.g., "directGregorian left")
  char side[32]; //!< Active telescope side (left, right, both)
  char opMode[32]; //!< Operation mode for preset (MODE_PSV, MODE_GUD, MODE_ACT, MODE_ADP, MODE_INF)
  char trackMode[32]; //!< Rotator Tracking Mode for preset (TRACK_PSTN, TRACK_VERT, TRACK_IDLE)
  char targCoord[16]; //!< Target coordinate system (COORD_RADEC or COORD_ALTAZ only)
  char targEquinox[16]; //!< Target coordinate equinox (usually J2000)
  double targEl; //!< Target Elevation (decimal degrees), if targCoord=COORD_ALTAZ
  double targAz; //!< Target Azimuth (decimal degrees), if targCoord=COORD_ALTAZ
  double rotAngle; //!< Instrument Rotator Angle in decimal degrees (convention determined by trackMode)
  long nGuideStars; //!< Number of guide stars for this preset (up to IIFAGENT_MAXGS)
  char gsRA[32][16]; //!< Array of pointers to guide star RAs, up to IIFAGENT_MAXGS
  char gsDec[32][16]; //!< Array of pointers to guide star Decs, up to IIFAGENT_MAXGS
  double gsMag[32]; //!< Array of pointers to guide star Magnitudes, up to IIFAGENT_MAXGS
  int Init; //!< IIF Interface initialization status (0=not initialized, 1=initalized)
  int Auth; //!< Authorization status flag (0=not authorized/idle,1=authorized)
  int Preset; //!< Preset status (0=no preset active, 1=preset data ready)
  int Debug; //!< Super-Verbose Debug output mode (0=disabled/1=enabled)
  int Abort; //!< Abort flag, normally 0, 1 if abort requested
  char reply[4096]; //!< servers error return

  // Telescope RA, DEC, AZ, EL, and offsets in radians.

  double TCSRA;
  double TCSDEC;
  double TCSRA_OFF;
  double TCSDEC_OFF;
  double TCSAZ;
  double TCSEL;
  double TCSAZ_OFF;
  double TCSEL_OFF;

  char Header1[100][79];
  char Header2[100][79];
  char Header3[100][79];
  char Header4[100][79];

  // float  vflat_power;         // VFLAT Lamps Power
  //  ttyport_t comm2port[MAX_ML];

} Islcommon;

#endif // ISLCOMMON_H 
