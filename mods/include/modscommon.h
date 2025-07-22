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
*/
 
// Various site-dependent but system-independent default values
 
// ISL shared memory layout
typedef struct islcommon {
  long nums[MAX_CLS];

  /*  From here define ISL variables. */
  long iclbox;
  long iclopr;
  int KHALT;
  int KECHO;

  /*
   * IEBs, LLBs, and HEB's for MODS1 and MODS2
   * IEB1, IEB2, IEB3, IEB4
   * LLB1, LLB2, LLB3
   * HEB1, HEB2
   *
   * MODS1, MODS2
   * IEBMODS[mods(0..1)][ieb(0..3)]
   */
  struct {
    /*
     * Instrument Electronix Boxes(IEB) for MODS1, MODS2
     * MODS1[0], MODS2[1]
     */
    float  reqpos[MAX_ML];      // Requested Mechanism position
    float  pos[MAX_ML];         // Actual Mechanism position
    int    ieb_i[MAX_ML];       // MicroLynx Controller int's deposit
    int    host[MAX_ML];        // MicroLynx Controller hosting
    int    busy[MAX_ML];        //            "         busy
    int    qued[MAX_ML];        //            "         Qued service
    char   who[MAX_ML][10];     //            "         IEB1 mechanism name
    char   ieb_msg[MAX_ML][80]; // Message holders for IEBs
    char   TTYIP[MAX_ML][64];   // Comtrol Box IP's and Ports
    char   WAGOIP[6][64];       // WAGO's IP's and Ports
    char   WAGOWHO[6][24];      // WAGO's names
    int    portcnt;
    int    loginfo[2]; // IMCS logging
    char   LLOG[79];
    long   logchg;
    int    erchk;
    int    active_smask;
    int    closeLoop;
    int    closeLoopON;
    float  motorv[MAX_ML];
    float  qc_GAIN[2];
    float  qc_X[4];
    float  qc_Y[4];
    float  qc_Z[4];
    float  qc_MAXTTPmove;
    float  qc_TTPustep;
    float  quadcells[8];
    float  average_quadcells[8];
    int    standalone;
    char   maskselect;  // B = busy, S = not busy
    char   maskinsert;  // B = busy, S = not busy
    char   qc_port[64];
    char   qc_port1[64];
    char   qc_who[24];
    char   qc_who1[24];
    int    qc_srate[2];
    int    qc_srate1[2];
    int    qc_average[2];
    int    qc_average1[2];
    int    qc_ONTARGET[2];
    int    qc_ONTARGET1[2];
    float  focusv[3];
    float  focusv1[3];
    float  temperature[4];
    float  temperature1[4];
    float  sthreshold[2];
    float  sthreshold1[2];
    int    modsx;        // 0 = NON-MODS, 1 = MODS(1), 2 = MODS(2)
    int    key_init;   
    int    imcs_onoff;
    int    imcs_onoff1;
    int    mlunit;
    int    TIMEOUT[MAX_ML];      // MicroLynx mechanism timeouts
    
    float  Current[4];            // IEB Currents

    ttyport_t commport[MAX_ML];   // Comtrol to MicroLynx TCP/IP communication
    ttyport_t wagoport[5];        // WAGO TCP/IP Port communication
    int modsPorts[3];             // Three ports:
                                  // [0]=IIFAgent, [1]=AGW Stage, [2]=rest

    char barcodes[24][20]; // Slit Mask Barcodes

    /*
     * Head Electronix Boxes(HEB) for MODS1 and MODS2
     * MODS1[0], MODS2[1]
     */
    double heb_d[MAX_ML];         // Head Electronix Boxes double's deposit
    float  heb_f[MAX_ML];         //          "            float's    "
    int    heb_i[MAX_ML];         //          "            int's      "
    char   heb_msg[MAX_ML][80];   // Message holders for HEBs

    /*
     * Lamps and Laser Boxes(LLB) for MODS1 and MODS2
     * Auxiliary Power Boxes(APB) for MODS1 and MODS2
     * MODS1, MODS2
     */
    float  LLB_f[MAX_ML];       // Lamps and Lasers Boxes float's deposit
    int    LLB_i[MAX_ML];       //          "             int's deposit
    char   LLB_msg[MAX_ML][80]; // Message holders for LLBs

    float  APB_f[MAX_ML];       // Aux. Power Boxes float's deposit
    int    APB_i[MAX_ML];       //         "        int's deposit
    char   APB_msg[MAX_ML][80]; // Message holders for APB

    /* IR laser */
    int irlaser_state;          // IR Laser state
    int irbeam_state;           // IR Laser Beam state
    int irlaser_timeon[2];      // How long has it been ON
    int irlaser_age[2];         // How long since it was replaced
    int irlaser_cycle[2];       // How many times has it been turned ON and OFF
    float irlaser_power;        // IR Laser Power
    int irlaser_setpoint;       // IR Laser Setpoint
    float irlaser_temp;         // IR Laser Temperature

    /* Visiable laser */
    int vislaser_state;         // Visiable Laser state
    int visbeam_state;          // Visiable Laser Beam state
    int vislaser_timeon[2];     // How long has it been ON
    int vislaser_age[2];        // How long since it was replaced
    int vislaser_cycle[2];      // How many times has it been turned ON and OFF
    float vislaser_power;       // Visiable Laser Power

    int lamp_state[9];          // Lamps status
    int lamp_timeon[9];         // How long has it been ON
    int lamp_age[9];            // How long since it was replaced
    int lamp_cycle[9];          // How many times has it been turned ON and OFF
    int lamp_all;               // Lamps ON/OFF monitor
    short lamplaser_all[1];     // Lamps and/or Lasers ON/OFF monitor

  } MODS1;

  struct {
    /*
     * Instrument Electronix Boxes(IEB) for MODS1, MODS2
     * MODS1[0], MODS2[1]
     */
    float  reqpos[MAX_ML];      // Requested Mechanism position
    float  pos[MAX_ML];         // Actual Mechanism position
    int    ieb_i[MAX_ML];       // MicroLynx Controller int's deposit
    int    host[MAX_ML];        // MicroLynx Controller hosting
    int    busy[MAX_ML];        //            "         busy
    int    qued[MAX_ML];        //            "         Qued service
    char   who[MAX_ML][10];     //            "         IEB1 mechanism name
    char   ieb_msg[MAX_ML][80]; // Message holders for IEBs
    char   TTYIP[MAX_ML][64];   // Comtrol Box IP's and Ports
    char   WAGOIP[6][64];       // WAGO's IP's and Ports
    char   WAGOWHO[6][24];      // WAGO's names
    int    portcnt;
    int    loginfo[2]; // IMCS logging
    char   LLOG[79];
    long   logchg;
    int    erchk;
    int    active_smask;
    int    closeLoop;
    int    closeLoopON;
    float  motorv[MAX_ML];
    float  qc_GAIN[2];
    float  qc_X[4];
    float  qc_Y[4];
    float  qc_Z[4];
    float  qc_MAXTTPmove;
    float  qc_TTPustep;
    float  quadcells[8];
    float  average_quadcells[8];
    int    standalone;
    char   maskselect;  // B = busy, S = not busy
    char   maskinsert;  // B = busy, S = not busy
    char   qc_port[64];
    char   qc_port1[64];
    char   qc_who[24];
    char   qc_who1[24];
    int    qc_srate[2];
    int    qc_srate1[2];
    int    qc_average[2];
    int    qc_average1[2];
    int    qc_ONTARGET[2];
    int    qc_ONTARGET1[2];
    float  focusv[3];
    float  focusv1[3];
    float  temperature[4];
    float  temperature1[4];
    float  sthreshold[2];
    float  sthreshold1[2];
    int    modsx;        // 0 = NON-MODS, 1 = MODS(1), 2 = MODS(2)
    int    key_init;   
    int    imcs_onoff;
    int    imcs_onoff1;
    int    mlunit;
    int    TIMEOUT[MAX_ML];      // MicroLynx mechanism timeouts
    
    float  Current[4];            // IEB Currents

    ttyport_t commport[MAX_ML];   // Comtrol to MicroLynx TCP/IP communication
    ttyport_t wagoport[5];        // WAGO TCP/IP Port communication
    int modsPorts[3];             // Three ports:
                                  // [0]=IIFAgent, [1]=AGW Stage, [2]=rest

    char barcodes[24][20]; // Slit Mask Barcodes

    /*
     * Head Electronix Boxes(HEB) for MODS1 and MODS2
     * MODS1[0], MODS2[1]
     */
    double heb_d[MAX_ML];         // Head Electronix Boxes double's deposit
    float  heb_f[MAX_ML];         //          "            float's    "
    int    heb_i[MAX_ML];         //          "            int's      "
    char   heb_msg[MAX_ML][80];   // Message holders for HEBs

    /*
     * Lamps and Laser Boxes(LLB) for MODS1 and MODS2
     * Auxiliary Power Boxes(APB) for MODS1 and MODS2
     * MODS1, MODS2
     */
    float  LLB_f[MAX_ML];       // Lamps and Lasers Boxes float's deposit
    int    LLB_i[MAX_ML];       //          "             int's deposit
    char   LLB_msg[MAX_ML][80]; // Message holders for LLBs

    float  APB_f[MAX_ML];       // Aux. Power Boxes float's deposit
    int    APB_i[MAX_ML];       //         "        int's deposit
    char   APB_msg[MAX_ML][80]; // Message holders for APB

    /* IR laser */
    int irlaser_state;          // IR Laser state
    int irbeam_state;           // IR Laser Beam state
    int irlaser_timeon[2];      // How long has it been ON
    int irlaser_age[2];         // How long since it was replaced
    int irlaser_cycle[2];       // How many times has it been turned ON and OFF
    float irlaser_power;        // IR Laser Power
    int irlaser_setpoint;       // IR Laser Setpoint
    float irlaser_temp;         // IR Laser Temperature

    /* Visiable laser */
    int vislaser_state;         // Visiable Laser state
    int visbeam_state;          // Visiable Laser Beam state
    int vislaser_timeon[2];     // How long has it been ON
    int vislaser_age[2];        // How long since it was replaced
    int vislaser_cycle[2];      // How many times has it been turned ON and OFF
    float vislaser_power;       // Visiable Laser Power

    int lamp_state[9];          // Lamps status
    int lamp_timeon[9];         // How long has it been ON
    int lamp_age[9];            // How long since it was replaced
    int lamp_cycle[9];          // How many times has it been turned ON and OFF
    int lamp_all;               // Lamps ON/OFF monitor
    short lamplaser_all[1];     // Lamps and/or Lasers ON/OFF monitor

  } MODS2;

  /* Define some MicroLYNX varibles */
  float REQPOS[MAX_ML]; // Requested Mechanism position
  float POS[MAX_ML];    // Actual Mechanism position
  int HOST[MAX_ML]; 
  int QUED[MAX_ML];
  int BUSY[MAX_ML];   // still busy(TRUE), deferent then HOMING
  char REPLY[MAX_ML][128];

  struct {
    int allocated; 
    char name[SEM_NUM][7];
  } go;

  struct {
    int allocated;
    char name[SEM_NUM][7];
  } sem;

  /* Crazy about time. */
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

  /* Telescope */
  float RAOFF;
  float DECOFF;
  float AZOFF;
  float ELOFF;

  /* AGW */
  float agwRAOFF;
  float agwDECOFF;
  float agwAZOFF;
  float agwELOFF;

  float XOFF;
  float YOFF;

  /* other cord. */
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

  // Telescope RA, DEC, AZ, EL, and offsets in Radians.
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

} modscommon;

#endif // ISLCOMMON_H 
