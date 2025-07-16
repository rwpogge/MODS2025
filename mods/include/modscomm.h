/** \file modscomm.h
 * \brief This file provides the data struct to get the actual state of
 * the MicroLYNX Controller and the several Axes with
 */

#ifndef MODSCOMM_H_
#define MODSCOMM_H_

#define P2S_SIZE 64 // number of entries in port to socket map table
#define LISTEN_Q_LEN 5
#define MODS_1_HOST  "lbt8"
#define MODS_1_PORT  10434
#define MODS_2_HOST  "lbt7"
#define MODS_2_PORT  10435

struct data {
	int general_state;
	int general_errors;
	int my_state;
	int my_errors;
	int mx_state;
	int mx_errors;
	int mf_state;
	int mf_errors;
	int mw_state;
	int mw_errors;
	int temp_umac;
	int temp_outside;
	int hum_umac;
};
typedef struct data data;

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

struct spos_xy {
	double posx;
	double posy;
};

typedef struct spos_xy spos_xy;

struct mods_errs {
	char err_reply[128];
};

typedef struct mods_errs mods_errs;

#endif

