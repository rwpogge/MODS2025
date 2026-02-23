/*!
  \mainpage redIMCS - Red Channel Image Motion Compensation System agent for MODS.
  
  \author R. Pogge, X. Carroll, OSU Astronomy Dept.

  Based on the old version by R. Gonzalez & R. Pogge, OSU Astronomy Dept.
  from 2029 December
 
  \date 2025 June 
  
  \section Usage
  
  redIMCS 
  
  \section Introduction
  
  Reads quad cell data from the Red channel HEB WAGO 4-port ADC
  module.  Quad cell data are reported by the WAGO as a 15-bit digital
  number corresponding to 0..10VDC at the WAGO analog input. This
  system replaces the older OSU Atwood HEB system that was readout
  using an RS232 serial interface.  This version supports the WAGO
  based HEB for the ARCHON CCD controller update in Summer 2025.

  The IMCS works by "autoguiding" an IR laser spot on the quad cell by
  steering the collimator using the three tip/tilt/focus (TTF)
  actuators. This program uses an algorithm for converting quad cell
  differential measurements (left-right, top-bottom) into collimator
  TTF actuator motions.

  This version uses the open-source libmodbus utilites to communicate
  with the WAGO TCP/modbus fieldbus module that operates the WAGO
  750-471 4-channel analog input module.

  A design choice here is to use libmodbus explicitly instead of using
  the wagoSetGet() function for the mmc server.
 
<pre>
  2009 Dec 06 - start of original development [rdg/osu]
  2010 Feb 28 - MODS1 "flight" version deployed to LBTO [rwp]
  2012 May 26 - MODS2 modifications (lab, deployed 2014)
  2015 May 16 - refinement of anti-false-ontarget logic [rwp/osu]
  ----
  2025 Jun 26 - start of port to the WAGO-based HEB for the ARCHON
                CCD controller update [rwp/osu]
  2025 Dec 31 - adjusted default signal threshold [rwp/osu]
</pre>

\todo

<ul>
</ul>
*/

/*!
  \file redIMCS.cpp
  \brief MODS Red Channel IMCS program
*/

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>  // For atoi()

#include <modbus.h> // libmodbus for WAGO Modbus/TCP
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

using namespace std;

#include <sys/sendfile.h>
#include <time.h>
#include "system_dep.h"

// #include "isisclient.h"

#include "instrutils.h"  

#include "isl_funcs.h"   // Common ISL functions
#include "isl_types.h"   // ISL data types
#include "params.h"      // Common parameters and defines
#include "islcommon.h"   // ISL Shared Memory defines
#include "isl_shmaddr.h" // Shared memory attachment.
#include "mmccontrol.h"  // MicroLYNX motor controller functions

#define MAX_WAGO 6   // maximum number of WAGO units, should be in params.h, someday...

#undef __DEBUG

#define MAX_WRITE_ERRORS 10    // Maximum of number of successive write errors befort aborting

const int RCVBUFSIZE = 24;    // Size of buffer for raw QCell data

// function prototypes we need (definitions after main())

char *getDateTime(void); // return date/time

int getMechanismID(char [], char []); // get mechanism ID in shared memory

int getQCID(char [], char []); // get quad cell ID in shared memory 

int rcolfoc_to(char *, char [], char [], long); // custom version of app/rcolfoc with a timeout

int getWagoQC(char*, int, int*); // read WAGO using libmodbus

float qc2vdc(int); // convert raw QC adc datum into units of DC volts

int rimcsWAGO, rTTFA, rTTFB, rTTFC; // ShMem IDs for WAGO Unit and collimator Tip/Tilt/Focus actuators

/*!
  \brief main program
*/

int main(int argc, char *argv[]) {
  int i,j,k,ierr;
  float dataArr[4];   // working quad cell data array (single readout)
  float meanQC[4];    // average quad cell data array (multiple reads)
  int rawQC[4];       // raw quad cell data (integers in units of ADU)

  float tiltErr, tipErr;
  float topRight,topLeft,bottomRight,bottomLeft;
  float topSum, bottomSum, leftSum, rightSum, sumQcells;
  float dA_tip, dB_tip, dC_tip;
  float dA_tilt, dB_tilt, dC_tilt;
  float tiltCorr, tipCorr;
  float baseToHeight;
  char cmd[PAGE_SIZE];
  char dummy[PAGE_SIZE];
  char QC1[10];   // QuadCell 1
  char QC2[10];   // QuadCell 2
  char QC3[10];   // QuadCell 3
  char QC4[10];   // QuadCell 4
  char echoBuffer[RCVBUFSIZE];    // Buffer for echo string + \0
  int numQCSamp;  // Quad Cell sample counter
  float chkCorr;  // Aggregate correction size check variable
  int loopState;  // loop state (open or closed) to sense changes in state

  int numWriteErrs; // Number of successive comtrol write errors

  // Ratio of the base to the height of an Equilateral Triangle

  baseToHeight = 2.0/sqrt(3.0);

  // This sets up shared memory and signal handling
  
  setup_ids();

  // Set the SIGINT signal trap

  signal(SIGINT,HandleInt); // Ctrl+C sends a move abort to controller
  signal(SIGPIPE,SIG_IGN);  // ignore broken pipes

  for (i=0;i<4;i++) {  // arrays [4] values from HEB
    meanQC[i]=0.0;
    dataArr[i]=0.0;
  }
  
  shm_addr->MODS.redIMCS_OnOff=1; // Turn ON the Red IMCS<-WAGO data taking.

  // Red channel HEB WAGO ID
  
  rimcsWAGO = getQCID((char*)"rimcs",dummy); // Get red channel QC WAGO info
  if (rimcsWAGO<0) {
    printf("[%d] Could not find Red IMCS (rimcs) quad cell WAGO assignment\n",rimcsWAGO);
    exit(-1);
  } 

  // Red collimator mirror tip/tilt/focus actuator IDs
  
  rTTFA = getMechanismID((char*)"rcolttfa",dummy); // Get rTTFA mechanism ID
  if (rTTFA<0) {
    printf("[%d]Could not find RCOLTTFA mechanism assignment\n",rTTFA);
    exit(-1);
  } 

  rTTFB = getMechanismID((char*)"rcolttfb",dummy); // Get rTTFB mechanism ID
  if (rTTFB<0) {
    printf("[%d]Could not find RCOLTTFB mechanism assignment\n",rTTFB);
    exit(-1);
  } 

  rTTFC = getMechanismID((char*)"rcolttfc",dummy); // Get rTTFC mechanism ID
  if (rTTFC<0) {
    printf("[%d]Could not find RCOLTTFC mechanism assignment\n",rTTFC);
    exit(-1);
  } 

  // Initialization
  
  j=0;           // initialize j counter for average data samples
  numQCSamp = 0; // initialize the quad cell sampling counter

  // Initial data sampling, correction gain, and threshod values that
  // seem reasonable.  These are in shared memory and can be changed
  // during the session using the RIMCS command sent to the IE server

  shm_addr->MODS.redQC_SampleRate=1000;
  shm_addr->MODS.redQC_Samples=5;       // Number of quad cell measurements to average
  shm_addr->MODS.redQC_Gain=1.0;
  shm_addr->MODS.redQC_Threshold[0]=0.05; // reduced 2025 Dec 31 [rwp/osu]

  // Reset the signal>threshold and on-target flags [rwp]

  shm_addr->MODS.redCloseLoop = 0;
  shm_addr->MODS.redQC_TARGET = 0; 

  // Shared memory datum shm_addr->MODS.qc_Z[i] is the parity for
  // red IMCS quad cell: 0=tilt (X), 1=tip (Y)
  // Red Parity: tiltParity=-1, tipParity=+1 [measured 2009-09-13 rwp]

  shm_addr->MODS.redQC_Z[0] = -1.0;
  shm_addr->MODS.redQC_Z[1] = +1.0;

  numWriteErrs = 0; // Initialize the successive write error counter

  // Start
  
  try {

    // Try to readout the quad cell. This is a first read to make
    // sure the WAGO is live and ready.  It prevents us from getting
    // into the readout and processing loop if the HEB has not been
    // powered on or some other fault occurs (including running the
    // IMCS agent before the MODS IE instance has started).
    //
    // If there are any faults, exit as informatively as possible. libmodbus
    // uses errno, so we might get actionable info on what went wrong in
    // modbus_strerror(errno).
    //
    // This keeps us out of entering the readout loop on startup faults.

    if (getWagoQC(shm_addr->MODS.QC_IP[rimcsWAGO],shm_addr->MODS.QC_REG[rimcsWAGO],rawQC) < 0) {
      cout << "\n***ERROR: redIMCS cannot read Red HEB WAGO unit IP address " 
	      << shm_addr->MODS.QC_IP[rimcsWAGO]
	      << "\n          Reason: " << modbus_strerror(errno)
	      << "\n                  Is the red channel HEB powered on?" << endl;
        cout << "          redIMCS aborted at " << getDateTime() << "\n" << endl;
      exit(-2);
    } 
    else {
      cout << "\nredIMCS started on " << shm_addr->MODS.QC_IP[rimcsWAGO]
	      << " at " << getDateTime() << "\n" << endl;
    }

    // What is the state of the control loop before we enter it?

    loopState = shm_addr->MODS.redCloseLoopON;

    // Sleep one readout cadence step before starting the readout loop
    
    MilliSleep(shm_addr->MODS.redQC_SampleRate);

    // Top of the readout loop
    
    while (1) {

      // Read the quad cell. If we get a read error, set all QCs to 0,
      // otherwise process the data

      ierr=0;
      if (getWagoQC(shm_addr->MODS.QC_IP[rimcsWAGO],shm_addr->MODS.QC_REG[rimcsWAGO],rawQC) < 0) {
	if (errno == EMBXGTAR) { // no response from target device
	  cout << "\n***ERROR: redIMCS quad cell read failed" 
	    << "\n          Reason: " << modbus_strerror(errno)
	    << "\n          Try restarting the redIMCS agent."
	    << "\n          redIMCS Aborted at " << getDateTime() << "\n" << endl;
	  exit(-2);
	}
	else { // Might be recoverable, carry on but allow no bad data into the system
	  shm_addr->MODS.redQC1=0;
	  shm_addr->MODS.redQC2=0;
	  shm_addr->MODS.redQC3=0;
	  shm_addr->MODS.redQC4=0;
	}
      }
      else {
	ierr=0;

	// unpack the raw integer ADC data into shm_addr->MODS.redQCn = ...

	// *** HACK: M2R dewar wiring appears to swap QC3 and QC4, fix in software for now  ***
	
        shm_addr->MODS.redQC1 = rawQC[0];
        shm_addr->MODS.redQC2 = rawQC[1];
        shm_addr->MODS.redQC3 = rawQC[3]; // rawQC[2];
        shm_addr->MODS.redQC4 = rawQC[2]; // rawQC[3];
        	
	// If *any* of the QC raw values are 0, assume we have corrupted
	// data.  For example, this can happen when reading the quad
	// cells during detector erase or readout.  To avoid having a
	// thrashing system because of false error signals, we set all
	// the QC cell readings to zero to keep bad QC values from
	// turning into bad TTF corrections furhter down

	// Disable for the WAGO readout system [rwp/osu]

	/*
	if (shm_addr->MODS.redQC1 == 0 || shm_addr->MODS.redQC2 == 0 ||
	    shm_addr->MODS.redQC3 == 0 || shm_addr->MODS.redQC4 == 0) {
	  shm_addr->MODS.redQC1=0;
	  shm_addr->MODS.redQC2=0;
	  shm_addr->MODS.redQC3=0;
	  shm_addr->MODS.redQC4=0;
	}
	*/
      }

      // Convert the raw quad cell signal in ADU to decimal
      // equivalents in DC volts, range 0..10.0 VDC

      shm_addr->MODS.redQC[0] = qc2vdc(shm_addr->MODS.redQC1);
      shm_addr->MODS.redQC[1] = qc2vdc(shm_addr->MODS.redQC2);
      shm_addr->MODS.redQC[2] = qc2vdc(shm_addr->MODS.redQC3);
      shm_addr->MODS.redQC[3] = qc2vdc(shm_addr->MODS.redQC4);

      // Check the IR laser state - open the control loop if it is off

      if (shm_addr->MODS.lasers.irlaser_state==0) {
#ifdef __DEBUG
	printf("RIMCS: IR laser is OFF, opening control loop\n");
#endif
	shm_addr->MODS.redCloseLoop = 0; // 0 = signal<threshold by definition
	shm_addr->MODS.redQC_TARGET = 0; // 0 = off-target by definition if no signal
      }

      // Copy the quad cell values in the working (non-shmem) data array

      dataArr[0] = shm_addr->MODS.redQC[0];
      dataArr[1] = shm_addr->MODS.redQC[1];
      dataArr[2] = shm_addr->MODS.redQC[2];
      dataArr[3] = shm_addr->MODS.redQC[3];

      // If the loop state changed since the last pass, reset the
      // sample counter and data vector so we don't fold open-loop
      // data into closed-loop and vis-versa.

      if (loopState != shm_addr->MODS.redCloseLoopON) {
	for (i = 0; i < 4; i++) meanQC[i]=0.0;
	numQCSamp = 0;
	shm_addr->MODS.redQC_TARGET = 0;
#ifdef __DEBUG
	printf("RIMCS: Control loop state is now closed\n");
#endif	
      }

      // If we have a full set of samples, compute TTF corrections.
      // If we are still sampling, skip and get the next sample.

      if (numQCSamp >= shm_addr->MODS.redQC_Samples) {

	/*
	 * Red Quad Cell: index#(quad#)
	 * 
	 *            +------+------+
	 *            |      |      |
	 *   topLeft  | 3(4) | 2(3) | topRight
	 *            |      |      |
	 *            +------+------+
	 *            |      |      |
	 * bottomLeft | 0(1) | 1(2) | bottomRight
	 *            |      |      |
	 *            +------+------+
	 */

	if (shm_addr->MODS.redQC_Average==0) // divide by 0 check
	  shm_addr->MODS.redQC_Samples=1;

	// Average the Quad Cell Signals - this is where we would
	// eventually subtract a mean bias level per quadrant or
	// software descramble a defective quad cell.
	
	topRight    = meanQC[2]/shm_addr->MODS.redQC_Samples;
	bottomRight = meanQC[1]/shm_addr->MODS.redQC_Samples;
	
	topLeft    = meanQC[3]/shm_addr->MODS.redQC_Samples;
	bottomLeft = meanQC[0]/shm_addr->MODS.redQC_Samples;

	// update the averages saved in shmem

	shm_addr->MODS.redQC_Average[2] = topRight;
	shm_addr->MODS.redQC_Average[1] = bottomRight;
	shm_addr->MODS.redQC_Average[3] = topLeft;
	shm_addr->MODS.redQC_Average[0] = bottomLeft;
	
	// Do the quad-cell signal arithmetic

	topSum    = topRight   + topLeft;     // Top    = 4+3
	bottomSum = bottomRight + bottomLeft; // Bottom = 1+2
	leftSum   = bottomLeft  + topLeft;    // Left   = 1+4
	rightSum  = bottomRight + topRight;   // Right  = 2+3
	sumQcells = topSum + bottomSum;       // Top + Bottom
	
	// Check Summation for zero(0) divide

	if (!sumQcells) sumQcells=0xFFFF;
	
	// Tilt Error Signal: tiltErr = (Right-Left)/Sum

	tiltErr = (rightSum - leftSum)/sumQcells;
	
	// Tip Error Signal:  tipErr = (Top-Bottom)/Sum

	tipErr = (topSum - bottomSum)/sumQcells;

	// Open/Close Loop check

	if (!shm_addr->MODS.redCloseLoopON) {
#ifdef __DEBUG
	  printf("RIMCS: Control loop state is OPEN\n");
#endif	
	  // We are running mandatory open loop (redCloseLoopOn=F),
	  // just update shared memory with the X and Y error signals
	  // but do nothing else
	  
	  shm_addr->MODS.redQC_X[0]=tiltErr; 
	  shm_addr->MODS.redQC_Y[0]=tipErr; 

	  // Clear the averaging arrays and reset the sample counter

	  for (i = 0; i < 4; i++) meanQC[i]=0.0;
	  numQCSamp = 0;
	  shm_addr->MODS.redQC_TARGET = 0;  // in case this is stale

	}
	else {

#ifdef __DEBUG
	  printf("RIMCS: Control loop state is CLOSED\n");
#endif	
	  // Closed loop is enabled (redCloseLoopON=T)

	  // Check the most recent quad cell signals.  If all four are
	  // below threshold, make no correction, but update the T/T
	  // error values so we can monitor the system.

	  if (shm_addr->MODS.redQC[0] < shm_addr->MODS.redQC_Threshold[0]&&
	      shm_addr->MODS.redQC[1] < shm_addr->MODS.redQC_Threshold[0]&&
	      shm_addr->MODS.redQC[2] < shm_addr->MODS.redQC_Threshold[0]&&
	      shm_addr->MODS.redQC[3] < shm_addr->MODS.redQC_Threshold[0]){
#ifdef __DEBUG
	    printf("RIMCS: QCell signal < %.2f - opening control loop\n",
		   shm_addr->MODS.redQC_Threshold[0]);
#endif	    
	    shm_addr->MODS.redCloseLoop = 0; // 0=OPEN Loop signal<threshold
	    shm_addr->MODS.redQC_TARGET = 0; // cannot be "on-target" if no spot...
	    shm_addr->MODS.redQC_X[0]=tiltErr; 
	    shm_addr->MODS.redQC_Y[0]=tipErr; 
	  }

	  // Signals are good, compute a correction

	  else {
	    // Target Evaluation: the IMCS spot is reckoned to be "on-target" 
	    // if both error signals are less than 5%.  We only evaluate this
	    // if we are able to make a correction (signals above threshold
	    // and closed loop).

	    if ( fabs(tiltErr) <= 0.05 && fabs(tipErr) <= 0.05 )
	      shm_addr->MODS.redQC_TARGET=1; // 1=ON target
	    else
	      shm_addr->MODS.redQC_TARGET=0; // 0=OFF target
      
	    shm_addr->MODS.redCloseLoop = 1; // signal>threshold

	    // Load Shared Memory with the error signals in X and Y 

	    shm_addr->MODS.redQC_X[0]=tiltErr;
	    shm_addr->MODS.redQC_Y[0]=tipErr;
	
	    // Tip Error corrections are applied 2/3 to the A
	    // actuator and -1/3 to each of the B and C actuators to
	    // maintain collimator focus (i.e., to apply a zero net 
	    // piston tip correction).
	    //
	    // We have to increase the tip correction by the ratio
	    // of the base to the height of an equilateral triangle,
	    // since the A-to-BC actuator baseline for the tip
	    // correction is smaller than the B-to-C actuator
	    // baseline for tilt correction.

	    tipCorr = baseToHeight*(tipErr/3.0)*shm_addr->MODS.redQC_Gain;
	  
	    if (shm_addr->MODS.redQC_Z[1]==1) { // Tip (Y) parity flag
	      dA_tip = 2.0*tipCorr;
	      dB_tip = -tipCorr;
	      dC_tip = dB_tip;
	    } else {
	      dA_tip = -2.0*tipCorr;
	      dB_tip = tipCorr;
	      dC_tip = dB_tip;
	    }
	    
	    // Tilt error corrections are applied 50/50 equal and
	    // opposite to actuators B and C, with actuator A
	    // remaining fixed to maintain collimator focus (zero
	    // net piston tilt correction).

	    tiltCorr = 0.5*tiltErr*shm_addr->MODS.redQC_Gain;
	  
	    if (shm_addr->MODS.redQC_Z[0]==1) { // Tilt (X) parity flag
	      dA_tilt = 0.0;
	      dB_tilt = tiltCorr;
	      dC_tilt = -1.0*tiltCorr;
	    } else {
	      dA_tilt = 0.0;
	      dB_tilt = -1.0*tiltCorr;
	      dC_tilt = tiltCorr;
	    }
	  
	    // Compute the compound tip/tilt error corrections

	    shm_addr->MODS.motorv[rTTFA] = dA_tip + dA_tilt;
	    shm_addr->MODS.motorv[rTTFB] = dB_tip + dB_tilt;
	    shm_addr->MODS.motorv[rTTFC] = dC_tip + dC_tilt;
	    
	    // Apply the tip/tip error correction to the collimator mirror
	    // if the aggregate correction is more than one full motor step
	    // equivalent (0.3microns)

	    chkCorr = fabs(shm_addr->MODS.motorv[rTTFA]*60.0) +
	      fabs(shm_addr->MODS.motorv[rTTFB]*60.0) + 
	      fabs(shm_addr->MODS.motorv[rTTFC]*60.0);

	    if (chkCorr > 0.3) {
	      sprintf(cmd,"step %0.1f %0.1f %0.1f",
		      shm_addr->MODS.motorv[rTTFA]*60.0,
		      shm_addr->MODS.motorv[rTTFB]*60.0,
		      shm_addr->MODS.motorv[rTTFC]*60.0);
	      
	      ierr = rcolfoc_to((char*)"localhost",(char*)"10435",cmd,5);
	      
	      if (ierr < 0)
		fprintf(stderr,"[%s] Fault: RIMCS %s\n",getDateTime(),cmd);
	      ierr=0;
	      memset(cmd,0,sizeof(cmd));
	    }
	  }
	}

	// Clear the averaging arrays and reset the sample counter

	for (i = 0; i < 4; i++) meanQC[i]=0.0;
	numQCSamp = 0;
	
      } 
      else {

	// We don't yet have a full complement of quad cell samples,
	// so add the most recent values to the summation vectors.
	// However, only do this if we have uncorrupted data

	if (dataArr[0] > 0 && dataArr[1] > 0 && dataArr[2] > 0 && dataArr[3] > 0) {
	  meanQC[0] += dataArr[0];
	  meanQC[1] += dataArr[1];
	  meanQC[2] += dataArr[2];
	  meanQC[3] += dataArr[3];
	  if (numQCSamp > shm_addr->MODS.redQC_Samples) 
	    numQCSamp = 0;
	  else 
	    numQCSamp++;
	}

      }

      // Record the loop state (open or closed) during this pass

      loopState = shm_addr->MODS.redCloseLoopON;

      // Sleep for the sampling interval

      MilliSleep(shm_addr->MODS.redQC_SampleRate);

    } // bottom of the operation while() loop

    // Destructor closes the socket

  } catch(...) { // caught an exception?
    cerr << "redIMCS caught an exception, aborting" << endl;
    exit(1);
  }

  return 0;
}

// Internal functions

//------------------------------------------------------------------------
//
// qc2vcd() - convert raw quad cell ADC datum to DC volts
//
/*!
  \brief Convert raw quad cell ADC datum to DC volts

  \param rawQC integer with raw quad cell ADC value (0..2^15-1)

  Convenience function to convert raw WAGO analog input module ADC out
  datum to DC volts. Assumes the WAGO 750-471 module is properly
  configured for -10..10VDC conversion

  Output is a floating-point voltage.  Discretization is ~0.3mVDC

*/

// Rick's variant

float qc2vdc(int rawQC){
  float posMax = pow(2.0,15) - 1.0; // positive maximum raw datum
  float negMin = pow(2.0,16) - 2.0; // negative minimum raw datum

  if (float(rawQC) > posMax) 
    return 10.0*(((float)(rawQC) - negMin)/posMax);
  else
    return 10.0*((float)(rawQC)/posMax);
}

//----------------------------------------------------------------------------
//
// getWagoQC() - read the WAGO quad cell board
//

/*!
  \brief Readout the IMCS quad cell board using a WAGO modbus/TCP system
  
  \param wagoAddr   (char*) The address of the WAGO device to query 
  \param regAddr    (int) register address (range: 1 - 0x10000)
  \param rawData    (uint16) array of data to read
  
  \return 0 on send success, value or error code on faults
  
  We have adopted opensource libmodbus to replace the replace the defunct
  and unsupported proprietary FieldTalk code used in the original MODS system.
    
  Author: Xander Carroll & Rick Pogge, OSU Astronomy Dept:
  Updated:
     2025 June 28 - based on modbusutils.c for the modsHEB agent
*/

int getWagoQC(char *wagoAddr, int regAddr, int* rawData) {
  modbus_t *modbus;       // libmodbus for WAGO Modbus/TCP interface
  uint16_t readData[4];   // Result of a modbus read.

  int result = 0;   // Return value for this function. 0 on success, -1 on errors.
  int regLen = 4;   // quad cell board is readout using a 4-channel analog input module

  // Open a Modbus/TCP connection to the WAGO
  modbus = modbus_new_tcp(wagoAddr,502);
  if (modbus_connect(modbus) == -1) {
    printf("WARNING: redIMCS cannot connect to WAGO host %s: %s\n",wagoAddr,modbus_strerror(errno));
    modbus_free(modbus);
    return -1;
  }

  // Pause 10ms to give a slow TCP link a chance to catch up
  usleep(10000);

  // We are reading the WAGO register at the given address
  result = modbus_read_registers(modbus,regAddr,regLen,readData);

  // Cast the data into an array of integers
  for(int i=0; i<regLen; i++) rawData[i] = static_cast<int>(readData[i]);

  // close connection and free libmodbus context
  modbus_close(modbus);
  modbus_free(modbus);

  // Return status: 0 on success, -1 on errors with errno set by modbus_read_registers()
  return result;
}
//---------------------------------------------------------------------------
//
// getDateTime() - Get UTC date/time info
//

/*!
  \brief Return the UTC date/time as an ISO 8601 coded string.

  Reads the system time clock and returns the UTC date and time coded
  as an ASCII string in ISO 6801 compliant format
  (www.iso.org/iso/iso8601).  The UTC clock time is expressed to
  microsecond precision (we make no claims to microsecond accuracy -
  YMMV).  For example 
  <pre> 
    2013-05-26T12:07:58.114391 
  </pre>

*/

char *
getDateTime(void)
{
  struct timeval tv;
  static char str[30];
  char *ptr;
  struct tm *gmt;
  time_t t;
  int monthNum;
  int ccyy;

  // First get the UTC time 

  t = time(NULL);
  gmt = gmtime(&t);
  monthNum = (gmt->tm_mon)+1;

  // ISO 8601 Date & time format: ccyy-mm-ddThh:mm:ss 

  ccyy = gmt->tm_year + 1900;
  sprintf(str,"%.4i-%.2i-%.2iT%.2i:%.2i:%.2i",ccyy,monthNum,
          gmt->tm_mday,gmt->tm_hour,gmt->tm_min,gmt->tm_sec);

  // Now get the microsecond precision part. If we're off a couple of
  // microseconds because of the time required to execute this
  // request, it is no big deal.

  gettimeofday(&tv,NULL);
  ptr = ctime(&tv.tv_sec);

  // Append it to the ISO8601 date+time string created above

  sprintf(str,"%s.%06ld",str,tv.tv_usec);

  return(str);

}

//---------------------------------------------------------------------------
//
// HandleInt - Handle Ctrl+C Interrupt (SIGINT) signals
//

/*!
  \brief Handle Ctrl+C Interrupts (SIGINT)

  \param signalValue integer value passed by the SIGINT signal catcher

  At this point all we do is print that the process has been
  halted by a Ctrl+C/SIGINT signal and exit gracefully.

  Later we might add more shutdown handling, but so far none appears
  indicated.

*/

void HandleInt(int signalValue) 
{
  fprintf(stderr,"\n*** redIMCS stopped by Ctrl+C (SIGINT) at %s\n\n",getDateTime());
  exit(0);
}

//---------------------------------------------------------------------------
//
// getMechanismID(mechName,reply) - Get the mechanism ID from Shared Memory
//

/*!
  \brief Get the mechanism ID from Shared Memory

  \param mechanism_name name of the mechanism
  \param reply message string for errors)
  \return integer mechanism ID code or -1 on errors

  Get the integer Mechanism ID code corresponding to a named mechanism.

*/

int 
getMechanismID(char mechanism_name[], char dummy[])
{
  int dev;

  for(dev=0;
      !strstr(mechanism_name,shm_addr->MODS.who[dev]) && dev<=MAX_ML;
      dev++);

  if(dev<0 || dev>=MAX_ML) {
    sprintf(dummy,"No such mechanism '%s' available",mechanism_name);
    return -1;
  }
  return dev;
}

//---------------------------------------------------------------------------
//
// getQCID(qc_name,reply) - Get the IMCS quad cell system ID from shared memory
//

/*!
  \brief Get the quad cell system ID from Shared Memory

  \param qc_name name of the IMCS quad cell system in shared memory
  \param reply message string for errors
  \return integer QC unit ID code or -1 on errors

  Get the integer ID code corresponding to a named WAGO-based IMCS
  quad cell system.  This ID code is used to get the IP address and
  analog input module register address from shared memory.

*/

int
getQCID(char qc_name[], char dummy[])
{
  int qc;

  for (qc=0;
       !strstr(qc_name,shm_addr->MODS.QC_WHO[qc]) && qc<=MAX_QC;
       qc++);

  if (qc<0 || qc>=MAX_QC) {
    sprintf(dummy,"No such QC '%s' available",qc_name);
    return -1;
  }
  return qc;
}

//---------------------------------------------------------------------------
//
// A custom version of app/rcolfoc() that implements a timeout to
// prevent this from blocking on a momentary comm fault and causing an
// IMCS lockup.
//
// This uses the API "backdoor" into the mmcServer
// 

#include "islapi.h"           // API for service communication

int rcolfoc_to(char *host, char str[], char str1[], long timeout)
{
  islcomp comp;
  islnum app;
  islconn conn;
  char send_buff[PAGE_SIZE];
  char recv_buff[PAGE_SIZE];
  int expect, received, len;
  int nextparam, bcnt, scnt;
  int ierr;

  struct timeval tv;
  fd_set readFDs;
  int numIO;

  ierr=0;

  // Setup the timeout interval.  The interval is given in long 
  // integer seconds.  If zero, we block (wait forever)

  if (timeout <= 0L) {
    tv.tv_sec = 0;
    tv.tv_usec = 0;
  }
  else {
    tv.tv_sec = timeout;
    tv.tv_usec = 0;
  }

  // Convert the arguments to binary format comp and islnum

  comp = islCnameToComp(host);
  if (comp == -1) {
    (void) fprintf(stderr, "invalid host '%s' - %s\n", host, strerror(errno));
    return -1;
  }

  app = (islnum) atoi(str);
  if (app == -1) {
    (void) fprintf(stderr, "invalid port number %s\n", str);
    return -1;
  }

  // Get the file descriptor for the server connection

  conn = islMakeContact(comp, app);
  if (conn < 0) {
    (void) fprintf(stderr,"cannot make contact - %s\n",strerror(errno));
    return -1;
  }

  // Bits we need for the select() call

  FD_ZERO(&readFDs);         // clear all fds
  FD_SET(conn,&readFDs);     // set to watch the input file descriptor

  // Create the command string

  sprintf(send_buff, "rcolfoc %s", str1);
  len=strlen(send_buff)+1;
  send_buff[len]='\0';

  // Send the command string to the server

  (void) send(conn, send_buff, len, 0);

  // Read and print same no. of bytes from the server.  We use select()
  // to handle timeouts (prevent block forever on the recv() call)

  numIO = select(conn+1,&readFDs,(fd_set *)NULL,(fd_set *)NULL,&tv);
  
  if (numIO == 0) {
    strcat(str1," - Comm Timeout");
    ierr = -1;
  }
  else if (numIO < 0) {
    sprintf(str1,"%s - %s",str1,strerror(errno));
    ierr = -1;
  }
  else {
    len = recv(conn, recv_buff, sizeof(recv_buff), 0);
    if (len < 0) {
      sprintf(str1,"%s - %s",str1,strerror(errno));
      ierr = -1;
    } 
    else {
      recv_buff[len]='\0';
      strcpy(str1,recv_buff);
      ierr = 0;
    }
  }
  (void) islSendEOF(conn);
  return ierr;
}
