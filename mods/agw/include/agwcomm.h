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

typedef struct data {
  struct agwpar_t;
  char general_state[512];
  /** Temperature in the UTIL-Box of AGW unit 
   * NOT BEING READ OUT YET */
  int temp_mods;
  /** Temperature outside the UTIL-Box of AGW unit 
   * NOT BEING READ OUT YET */
  int temp_outside;
  /** Humidity in the UTIL-Box of AGW unit 
   * NOT BEING READ OUT YET */
  int hum_mods;
}data;

/** Structure describing the AGW status.
 */
//typedef struct data {
	/** General state of AGW unit */
        //  char general_state;
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
	 * NOT BEING READ OUT YET */
//	int temp_mods;
	/** Temperature outside the E-Box of AGW unit 
	 * NOT BEING READ OUT YET */
//	int temp_outside;
	/** Humidity in the E-Box of AGW unit 
//	 * NOT BEING READ OUT YET */
//	int hum_mods;
//}data;

/** XY coordinate system integer coordinate structure.
 * This structure is deprecated.
 */
typedef struct spos_xy_int {
	int posx; /** position x coordinate */
	int posy; /** position y coordinate */
} spos_xy_int;


/** XY coordinate system coordinate structure.
 */
typedef struct spos_xy {
	double posx; /** position x coordinate */
	double posy; /** position y coordinate */
} spos_xy;

// Values convertions
typedef struct tc_xy
{
  double sfp_x; // AGW to SFP
  double sfp_y; // AGW to SFP
  double gp_x;  // SFP to AGW
  double gp_y;  // SFP to AGW
  double dgp_x;  // CCD to AGW
  double dgp_y;  // CCD to AGW
  double dsfp_x;  // CCD to SFP
  double dsfp_y;  // CCD to SFP

} tc_xy;

typedef double angle;

/** MODS error messages **/
struct mods_errs {
        char err_reply[128];
};
 
typedef struct mods_errs mods_errs;

#endif

