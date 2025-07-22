/** \file modscomm.h
 * \brief This file provides the data struct to get the actual state of
 * the MicroLYNX Controller and the several Axes with
 */

#ifndef MODSCOMM_H_
#define MODSCOMM_H_

#define P2S_SIZE 64 // number of entries in port to socket map table
#define LISTEN_Q_LEN 10
#define MODS_1_HOST  "mods1"
#define MODS_1_PORT  10435
#define MODS_2_PORT  10436

struct data {
	int general_state[30];
	int general_errors[30];
	int temp_mlc[4];
	int temp_outside[4];
	int hum_mlc[4];
};
typedef struct data data;

/*
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
	int posx;
	int posy;
};
typedef struct spos_xy spos_xy;
*/

struct mods_errs {
	char err_reply[128];
};

typedef struct mods_errs mods_errs;

#endif

