/** \file agwcomm.h
 * \brief This file provides the data struct to get the actual state of
 * the MicroLYNX Controller and the several Axes with
 */

#ifndef AGWCOMM_H_
#define AGWCOMM_H_

#define P2S_SIZE 64 // number of entries in port to socket map table
#define LISTEN_Q_LEN 10
#define MODS_1_HOST  "mods1"
#define MODS_1_PORT  10433
#define MODS_2_PORT  10434

/** Structure describing the AGW status.
 */
typedef struct data {
	/** General state of AGW unit */
        char general_state[512];
        // int general_state;
	/** Errors on AGW unit */
  	// int general_errors;
	/** State of the rotational axis */
	// int my_state;
	/** Errors regarding the rotational axis */
	// int my_errors;
	/** State of the radial axis */
	// int mx_state;
	/** Errors regarding the radial axis */
	// int mx_errors;
	/** State of the focus axis */
	// int mf_state;
	/** Errors regarding the focus axis */
	// int mf_errors;
	/** State of the filter wheel axis */
	// int mw_state;
	/** Errors regarding the filter wheel axis */
	// int mw_errors;
	/** Temperature in the E-Box of AGW unit 
	 * NOT BEEING READ OUT YET */
	int temp_mods;
	/** Temperature outside the E-Box of AGW unit 
	 * NOT BEEING READ OUT YET */
	int temp_outside;
	/** Humidity in the E-Box of AGW unit 
	 * NOT BEEING READ OUT YET */
	int hum_mods;
}data;

/** XY coordinate system integer coordinate structure.
 * This structure is deprecated.
 */
typedef struct spos_xy_int {
	int posx; /** position x coordinate */
	int posy; /** position y coordinate */
}spos_xy_int;


/** XY coordinate system coordinate structure.
 */
typedef struct spos_xy {
	double posx; /** position x coordinate */
	double posy; /** position y coordinate */
}spos_xy;

typedef double angle;

struct agwdata {
  long XAG;
  long YAG;
  long XWFS; 
  long YWFS;
  long XS;
  long YS;
  long Focus;
  long F0;
  long AutoFocus;
  long Filter;
  long Drift;
  long Xrate;
  long Yrate;
  long Lock;
  long LockHost;
  long Calib;
};

typedef struct agwdata agwdata;

#endif

