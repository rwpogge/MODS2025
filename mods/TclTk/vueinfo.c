/* vueinfo.c -- view, use, and execute information

  \mainpage vueinfo - View User Environment INFOrmation

  \author Staff, OSU Dept. of Astronomy rgonzale@astronomy.ohio-state.edu
 
  \date 2005 May 01
 
  \section Usage
  
  Usage: information and commands for ISL system and GUI(s)

  NOTE: This is a mess. It will have to be cleaned up some day.

  2025 June 24 - AlmaLinux 9 port and introduction of a WAGO-based
                 IMCS quad cell readout system with the new ARCHON
                 CCD controller update. [rwp/osu]

*/
#include <iostream>
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#define MAX_BUF 256
#define MAX_OUT 256
#define MAX_LEN 256+1
#define BLANK 8

#include "system_dep.h"     // OS dependent headers
#include "dpi.h"
#include "MATHCNST.H"
#include "instrutils.h"     // ISL Instrument header
#include "isl_funcs.h"      // function defines for all isl
#include "isl_types.h"      // general header file for all isl data structure definitions
#include "params.h"         // general isl parameter header
#include "islcommon.h"      // shared memory (Islcommon C data structure) layout
#include "isl_shmaddr.h"    // declaration of pointer to islcommon

#include "agwcomm.h" // AGW functions header file
#include "modscontrol.h" // AGW functions header file
#include "mmccontrol.h" // AGW functions header file

int vuecalq(char[]);
long rte_secs();

struct islcommon *ms;

int nsem_take(char [],int);
void what_help(char *, int , char *, int,int,int);
// void cnvrt(int,double,double,double*,double*,int [6],double,double);
void display_it(int , int , char *, char *);
char *getSensor(int,int);
char *makeUpper(char *);

extern tc_xy pxy;

//---------------------------------------------------------------------------
// StrUpper() - convert all characters in a string to uppercase
//
 
/*!
  \brief Convert all characters in a string to uppercase
 
  \param str String to be converted to uppercase. Changed by this function.
 
  Helper utility to convert all characters in a string to uppercase.
  All commands are required to be in uppercase.
 
*/

void
StrUpper(char *str)
{
  int i;
  for(i=0;i<strlen(str);i++)
    if(str[i] >= 'a' && str[i] <= 'z')
      str[i] = toupper(str[i]);
}
//---------------------------------------------------------------------------
//
// getMechanismID(dummy,reply) - Get the mechanism ID from Shared Memory
//

/*!
  \brief Get the mechanism ID from Shared Memory

  \param dummy mechanism
  \param reply reply message
  \return mechanismID  or error

  Get Mechanism ID

  <pre>
  </pre>
*/
int 
getMechanismID(char mechanism_name[], char dummy[])
{
  int dev;

  for(dev=0;
      !strstr(mechanism_name,shm_addr->MODS.who[dev]) && dev<=MAX_ML-1;
      dev++);

  if(dev<0 || dev>=MAX_ML-1) {
    sprintf(dummy,"No such mechanism '%s' available",mechanism_name);
    return -1;
  }
  return dev;
}

int
main(int argc, char *argv[])
{
  int i, j, iyear;
  int it[6];
  long ip[5];
  char what[10], cmd[100];
  char cnam[120];
  char runstr[120], cthp[10];
  int clength, unit;
  int negpos;
  int irah, iram, idecd, idecm;
  float thp, ras;
  double raxx,dcxx;
  double pos_ra,pos_dec,azim,elev;
  int ifs[3];
  int *ierr, posdeg=0, ivalue, iversion;
  char site[8];
  char buff[512]; 
  char ipascii[20]; 
  char log_name[15];
  time_t ti;
  struct tm *tm;

  // Power state words

  char pwrStates[3][6] = {"Fault","Off","On"};
  int istate;
  
  //  putpname("vueinfo"); // connect me to the ISL
  setup_ids();         // Give me the shmem, semaphores, and mclass ids.
  ms = shm_addr;       // assign a pointer to shared memory 

  /*
   * You need to ask politly......or know how to ask for infomation or command
   * a mechanism.
   */
  if (argc<2) {
    printf("vueinfo needs more information\n"); 
    //  available commands
    cout<<"Available commands\n"<<"vueinfo help who\n"<<"vueinfo pos\n"
	<<"vueinfo site\n"<<"vueinfo rte\n"<<"vueinfo time\n"
	<<"vueinfo logfile\n"<<"vueinfo MODS.standalone\n"<<"vueinfo gpstime\n"
	<<"vueinfo vuecalq\n"<<"vueinfo sour\n"
	<<"vueinfo imcs[g1:g2:t1:tm2:tr1:tmr2:m1:m2:m3:ACC1:ACC2:ACC3\n"
	<<"vueinfo gain or vueinfo gain[g1:g2:ttp:tMax\n"
	<<"vueinfo average_qcell or vueinfo average_qcell[1:2:3:4:5:6:7:8\n"
	<<"vueinfo qcell or vueinfo qcell[1:2:3:4:5:6:7:8]\n"
	<<"vueinfo osci or vueinfo osci[1:2:3]\n"<<"vueinfo errs\n"
	<<"vueinfo ipmap\n"<<"vueinfo wago_port\n"<<"vueinfo islcommon\n"
	<<"vueinfo MPOS\n"<<"vueinfo relsem\n"<<"vueinfo mfstatus\n"
	<<"vueinfo chksem\n"<<"vueinfo loginfo\n"<<"vueinfo freq\n"
	<<"vueinfo rate\n"<<"vueinfo closeLoop\n"<<"vueinfo targetStatus\n"
	<<"vueinfo islvueg\n"<<"vueinfo islvuey\n"<<"vueinfo islvuen\n"
	<<"vueinfo killLOGGING\n"<<"vueinfo icl[opr:box]\n"
	<<"vueinfo rsthreshold -1 or vueinfo rsthreshold value\n"
	<<"sthreshold for BLUE or rsthreshold RED\n"
	<<"vueinfo islcmd[1-30] value\n"
	<<"vueinfo masksm[S|I|A]   :Check slitmask\n"<<"vueinfo MLC[1-30]\n"
	<<"vueinfo mmccmd command\n"<<"vueinfo agwlocate\n"
	<<"vueinfo agwcmd command\n"<<"vueinfo agwlocate\n"
	<<"vueinfo tcscmd command\n"<<"vueinfo agwlocate\n"
	<<"vueinfo agwval or vueinfo agwval[X:Y:PF:FW:1:2:3:4]\n";

    exit(0);
  }
  /* ** */
  // Time to eat argv[n] and do something with what we have been given.
  strcpy(what,argv[1]);
  StrUpper(what);

  // if we have more then the one command other then vueinfo.
  if (argc==3) {
    strcpy(cmd,argv[2]);
  } else if (argc==4) {
    sprintf(cmd,"%s %s", argv[2], argv[3]);
  } else if (argc==5) {
    sprintf(cmd,"%s %s %s", argv[2], argv[3], argv[4]);
  } else cmd[0]='\0';
  /* ** */

  ti=time(0);
  tm=gmtime(&ti);

  // Look! for HELP
  
  if (!strcasecmp(what,"HELP")) {
    ierr=0;
    strcpy(cnam,argv[2]);
    i=strlen(cnam);
    cnam[i]='\0';
    runstr[0]='\0';
    what_help( cnam, i, runstr, i, 0, 0);
    strcpy(buff,"/home/dts/mods/shells/helpsh ");
    strcat(buff,runstr);
    system(buff);
    exit(0);
  }
  // Load a PLC

  else if (!strcasecmp(what,"LOAD")) {
    ierr=0;
    strcpy(cnam,argv[2]);
    i=strlen(cnam);
    cnam[i]='\0';
    runstr[0]='\0';
    strcpy(buff,"/home/dts/mods/shells/loadsh ");
    strcat(buff,argv[2]);
    strcat(buff," ");
    strcat(buff,argv[3]);
    printf("%s\n",buff);
    system(buff);
    exit(0);
  }
  // ISL real-time offsets

  else if (!strcasecmp(what,"RTE")) {
    printf("%0.4f\n",ms->time.rate[0]);
    printf("%0.4f\n",ms->time.rate[1]);
    printf("%d\n",ms->time.offset[0]);
    printf("%d\n",ms->time.offset[1]);
    printf("%d\n",ms->time.epoch[0]);
    printf("%d\n",ms->time.epoch[1]);
    printf("%d\n",ms->time.span[0]);
    printf("%d\n",ms->time.span[1]);
    //    ms->time.secs_off=1135544482;
    ms->time.secs_off=rte_secs();
    printf("%d\n",ms->time.secs_off);
    printf("%d\n",ms->time.index);
    printf("%d\n",ms->time.icomputer[0]);
    printf("%d\n",ms->time.icomputer[1]);
    ms->time.model=' ';
    printf("%c\n",ms->time.model);
    exit(0);
  }
  // ISL Date Jday and time UTC

  else if (strstr(what,"TIME")) {
    if (!strcasecmp(what,"TIMEU"))
      system("date -u +%F\":D%j\"\"%n\"%H:%M:%S\" \"UTC");
    else if (!strcasecmp(what,"TIMEL"))
      system("date +%F\":D%j\"\"%n\"%T\" \"LOC");
    else if (!strcasecmp(what,"TIMES")) {
      void getUT(long *,long *,long *,long *,long *,long *);
      void calcLST(double , double , double ,double* , double* );
      double dateToJD(int,int,int);
      void calcGMSTD (double,double,double *,double *);
      void secToTime(double ,char *,short);
      double lat = 40.001276, ra, dec, fmins, cmd_azpos, cmd_zagpos,ha;
      double fdecpos, frapos, faoha, aohapos;
      char rapos[18], decpos[18], hapos[18];
      long sign;
      int it[6], dinyr, iyear;
      long yr, mon, day, hrs, mins, secs;
      float fsec;
      double jd, lsttime, utsec, GSD, GMST, lstDay, lstSecs;
      double longit = -83.018351;
      char str[10];

      rte_time(it,&iyear);
      dinyr = 365;
      if ((iyear%400) == 0 ||
	 ((iyear%4) == 0 &&
	  (iyear%100)) != 0) dinyr = 366;
      shm_addr->epoch=(iyear + it[4])/float(dinyr);
      shm_addr->ep1950=shm_addr->epoch;

      getUT(&yr, &mon, &day, &hrs, &mins, &secs);
      utsec = hrs * 3600.0 + mins * 60.0 + secs;
      jd = dateToJD(yr,mon,day);
      calcGMSTD (jd, utsec, &GSD, &GMST);
      calcLST(GSD, GMST, 0.0, &lstDay, &lsttime);
      secToTime(lsttime, str, 0);
      longit=(longit/DEG_RAD)*SEC_RAD;

      calcLST(GSD, GMST, longit, &lstDay, &lstSecs);  // Local Sidereal Time.
      secToTime(lstSecs, str, 0);
      if (what[4]=='S') printf("%0.4f\n%s LST\n",jd, str);

      if (what[4]=='R') printf("%s\n",ms->targRA);
      
      if (what[4]=='D') printf("%s\n",ms->targDec);

      // Hour Angle
      ra=ms->TCSRA*RAD2SEC;
      faoha = (lstSecs - ra)/3600.0;
      if (faoha > 12.0)
	{
	  if (lstSecs > 12.0) {
	    faoha = ((24.0 - (lstSecs/3600.0 + ra/3600.0)) * (-1.0));
	 } else {
	    faoha = (24.0 - ra/3600.0) + lstSecs/3600.0;
	 }
	}

      // Hour Angle
      hrs = (abs)((long)faoha);
      fmins = (abs)((long)((faoha - hrs)));
      mins = (long)fmins;
      secs = (long)((fmins - mins) * 60.0);
      sprintf(hapos, "%02d:%02d:%02d", hrs, mins, secs);
      if (what[4]=='H' && what[5] != 'X') printf("%s\n", hapos);
      else if (what[5]=='X') printf("%04.1f\n", faoha);
    } else { }
     exit(0);

  }
  else if (!strcasecmp(what,"SETDATE")) {
    system("date +%F\":D%j\"\"%n\"%T\" \"LOC");
    exit(0);

  }
  else if (!strcasecmp(what,"LOGFILE")) {
    strcpy(log_name,ms->MODS.LLOG);
    printf("%s",log_name);
    exit(0);

  }
  // Run MODS.standalone without ISL
  else if (!strcasecmp(what,"MODS.STANDALONE")) {
    if (ms->MODS.standalone) ms->MODS.standalone = 1;
    else ms->MODS.standalone = 0;
    exit(0);

  }
  // ISL Mixed Value calculator
  else if (!strcasecmp(what,"VUECALQ")) {
    vuecalq(cmd);
    exit(0);
  }
  // SITE GPS
  else if (!strcasecmp(what,"GPSTIM")) {
    exit(0);

  }
  // mmcServer Counter
  else if (!strcasecmp(what,"MMCCOUNTER")) {
    //for(i=0;i<MAX_ML-1;i++)
    for(i=0;i<MAX_ML;i++)
      if (i==ms->MODS.mmcServerCounter)
	printf("->REQ: %s\n",ms->MODS.ieb_msg[i]);
      else
	printf("%s\n",ms->MODS.ieb_msg[i]);
    exit(0);
  }
  // IMCS parameters
  else if (strstr(what,"IMCS")) {
    if (strstr(what,"G1")) ms->MODS.blueQC_Gain = atof(cmd);
    else if (strstr(what,"G2")) ms->MODS.redQC_Gain = atof(cmd);
    if (strstr(what,"T1")) ms->MODS.qc_TTPustep = atof(cmd);
    else if (strstr(what,"TM2")) ms->MODS.qc_MAXTTPmove = atof(cmd);
    else if (strstr(what,"TR1")) printf("%0.4f\n",ms->MODS.qc_TTPustep);
    else if (strstr(what,"TMR2")) printf("%0.4f\n",ms->MODS.qc_MAXTTPmove);
    else if (strstr(what,"M1")) printf("%0.1f\n",ms->MODS.motorv[21]);
    else if (strstr(what,"M2")) printf("%0.1f\n",ms->MODS.motorv[22]);
    else if (strstr(what,"M3")) printf("%0.1f\n",ms->MODS.motorv[23]);
    else if (strstr(what,"M4")) printf("%0.1f\n",ms->MODS.motorv[2]);
    else if (strstr(what,"M5")) printf("%0.1f\n",ms->MODS.motorv[3]);
    else if (strstr(what,"M6")) printf("%0.1f\n",ms->MODS.motorv[4]);
    else if (strstr(what,"ACC1")) printf("%0.0f\n",ms->MODS.pos[23]*60.0);
    else if (strstr(what,"ACC2")) printf("%0.0f\n",ms->MODS.pos[24]*60.0);
    else if (strstr(what,"ACC3")) printf("%0.0f\n",ms->MODS.pos[25]*60.0);
    else if (strstr(what,"ACC4")) printf("%0.0f\n",ms->MODS.pos[2]*60.0);
    else if (strstr(what,"ACC5")) printf("%0.0f\n",ms->MODS.pos[3]*60.0);
    else if (strstr(what,"ACC6")) printf("%0.0f\n",ms->MODS.pos[4]*60.0);
    exit(0);

  }
  else if (!strcasecmp(what,"PARITY_R")) {
    if (cmd[0]=='-') ms->MODS.redQC_Z[0] = -1.0;
    else  ms->MODS.redQC_Z[0] = 1.0;
    if (cmd[1]=='-') ms->MODS.redQC_Z[1] = -1.0;
    else  ms->MODS.redQC_Z[1] = 1.0;

    exit(0);

  }
  else if (!strcasecmp(what,"PPARITY_R")) {
    if (ms->MODS.redQC_Z[0] == -1.0) printf("-");
    else  printf("+");
    if (ms->MODS.redQC_Z[1] == -1.0) printf("-");
    else  printf("+");

    exit(0);

  }
  else if (!strcasecmp(what,"PARITY_B")) {
    if (cmd[0]=='-') ms->MODS.blueQC_Z[0] = -1.0;
    else  ms->MODS.blueQC_Z[0] = 1.0;
    if (cmd[1]=='-') ms->MODS.blueQC_Z[1] = -1.0;
    else  ms->MODS.blueQC_Z[1] = 1.0;

    exit(0);

  }
  else if (!strcasecmp(what,"PPARITY_B")) {
    if (ms->MODS.blueQC_Z[0] == -1.0) printf("-");
    else  printf("+");
    if (ms->MODS.blueQC_Z[1] == -1.0) printf("-");
    else  printf("+");

    exit(0);

  }
  // IMCS gain parameters
  else if (strstr(what,"GAIN")) {
    if (strstr(what,"G1")) printf("%0.4f\n",ms->MODS.blueQC_Gain);
    else if (strstr(what,"G2")) printf("%0.4f\n",ms->MODS.redQC_Gain);
    else if (strstr(what,"TTP")) printf("%0.4f\n",ms->MODS.qc_TTPustep);
    else if (strstr(what,"TMAX")) printf("%0.4f\n",ms->MODS.qc_MAXTTPmove);
    else {
	printf("%0.4f\n",ms->MODS.blueQC_Gain);
	printf("%0.4f\n",ms->MODS.redQC_Gain);
	printf("%0.4f\n",ms->MODS.qc_TTPustep);
	printf("%0.4f\n",ms->MODS.qc_MAXTTPmove);
    }
    exit(0);
  }
  // IMCS change gain.
  else if (strstr(what,"CG")) {
    if (strstr(what,"1")) ms->MODS.blueQC_Gain = atof(cmd);
    else if (strstr(what,"2")) ms->MODS.redQC_Gain = atof(cmd);
    else printf("%0.1f,%0.1f\n",ms->MODS.blueQC_Gain,ms->MODS.redQC_Gain);
    exit(0);
  }
  // IMCS average quad-cell parameters
  else if (strstr(what,"AVERAGE_QCELL")) {
    if (strstr(what,"1"))
      printf("%0.3f",ms->MODS.blueQC_Average[0]);
    else if (strstr(what,"2"))
      printf("%0.3f",ms->MODS.blueQC_Average[1]);
    else if (strstr(what,"3"))
      printf("%0.3f",ms->MODS.blueQC_Average[2]);
    else if (strstr(what,"4"))
      printf("%0.3f",ms->MODS.blueQC_Average[3]);
    else if (strstr(what,"5"))
      printf("%0.3f",ms->MODS.redQC_Average[0]);
    else if (strstr(what,"6"))
      printf("%0.3f",ms->MODS.redQC_Average[1]);
    else if (strstr(what,"7"))
      printf("%0.3f",ms->MODS.redQC_Average[2]);
    else if (strstr(what,"8"))
      printf("%0.3f",ms->MODS.redQC_Average[3]);
    else {
      printf("Quadcells: %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f",
	     ms->MODS.blueQC_Average[0],
	     ms->MODS.blueQC_Average[1],
	     ms->MODS.blueQC_Average[2],
	     ms->MODS.blueQC_Average[3],
	     ms->MODS.redQC_Average[0],
	     ms->MODS.redQC_Average[1],
	     ms->MODS.redQC_Average[2],
	     ms->MODS.redQC_Average[3]);
    }
    exit(0);
  }
  // environmental sensors
  else if (!strcasecmp(what,"env")) {
    printf("%s Environmental Sensors:\n",ms->MODS.instID);
    printf("  Glycol Supply: P=%.2f psi-g  T=%.1f C\n",ms->MODS.glycolSupplyPressure,ms->MODS.glycolSupplyTemperature);
    printf("         Return: P=%.2f psi-g  T=%.1f C\n",ms->MODS.glycolReturnPressure,ms->MODS.glycolReturnTemperature);
    printf("  IUB Inside Air T=%.1f C  Ambient T=%.1f C  HeatSink T=%.1f C\n",
	   ms->MODS.utilBoxAirTemperature,ms->MODS.outsideAirTemperature,ms->MODS.agwHeatSinkTemperature);
    printf("  Blue IEB Inside Air T=%.1f C  Glycol Return T=%.1f C\n",ms->MODS.blueTemperature[0],ms->MODS.blueTemperature[1]);
    printf("   Red IEB Inside Air T=%.1f C  Glycol Return T=%.1f C\n",ms->MODS.redTemperature[0],ms->MODS.redTemperature[1]);
    printf("  Blue HEB Inside Air T=%.1f C  Dewar T=%.1f C P=%8.2e torr\n",ms->MODS.blueHEBTemperature,ms->MODS.blueDewarTemperature,
		    ms->MODS.blueDewarPressure);
    printf("   Red HEB Inside Air T=%.1f C  Dewar T=%.1f C P=%8.2e torr\n",ms->MODS.redHEBTemperature,ms->MODS.redDewarTemperature,
		    ms->MODS.redDewarPressure);
    printf("  MODS Air Top T=%.1f C  Bottom T=%.1fC\n",ms->MODS.redTemperature[2],ms->MODS.redTemperature[3]);
    printf("     Truss Top T=%.1f C  Bottom T=%.1fC\n",ms->MODS.blueTemperature[2],ms->MODS.blueTemperature[3]);
    exit(0);
  }
  // MODS dewar pressure and temperature
  else if (!strcasecmp(what,"dewars")) {
    sprintf(buff,"%.1f",ms->MODS.blueDewarTemperature);
    sprintf(buff,"%s %8.2e",buff,ms->MODS.blueDewarPressure);
    sprintf(buff,"%s %.1f",buff,ms->MODS.redDewarTemperature);
    sprintf(buff,"%s %8.2e",buff,ms->MODS.redDewarPressure);
    printf("%s\n",buff);
    exit(0);
  }
 
  // IEB currents and temperatures
  else if (!strcasecmp(what,"VandC")) {
    printf("%.3f Amps\n",ms->MODS.Current[0]);
    printf("%.3f Amps\n",ms->MODS.Current[1]);
    printf("%.3f Amps\n",ms->MODS.Current[2]);
    printf("%.3f Amps\n",ms->MODS.Current[3]);
    printf("%.1f C\n",ms->MODS.iebTemp[0]);
    printf("%.1f C\n",ms->MODS.iebTemp[1]);
    printf("%.1f C\n",ms->MODS.iebTemp[2]);
    printf("%.1f C\n",ms->MODS.iebTemp[3]);
    exit(0);
  }
  // WAGO Monitor and Control registers
  else if (!strcasecmp(what,"WAGOMC")) {
    printf("WAGO monitor and control sensors\n");
    for (i=0;i<30;i++) printf("[ %0d ]",ms->MODS.temps[i]);
    for (i=0;i<4;i++) printf("\n[%0.4f]",ms->MODS.blueTemperature[i]);
    printf("\nVoltage:%0.4f\n",ms->MODS.blueQC[0]*10.0);
    printf("Current:%0.4f\n",ms->MODS.blueQC[1]);
    printf("%0.4f\n",ms->MODS.blueQC[2]);
    printf("%0.4f\n",ms->MODS.blueQC[3]);
    printf("%0.4f\n",ms->MODS.redQC[0]);
    printf("%0.4f\n",ms->MODS.redQC[1]);
    printf("%0.4f\n",ms->MODS.redQC[2]);
    printf("%0.4f\n",ms->MODS.redQC[3]);
    exit(0);
  }
  // IMCS quad-cell parameters
  else if (strstr(what,"QCELL")) {
    if (strstr(what,"1")) printf("%0.3f",ms->MODS.blueQC[0]);
    else if (strstr(what,"2")) printf("%0.3f",ms->MODS.blueQC[1]);
    else if (strstr(what,"3")) printf("%0.3f",ms->MODS.blueQC[2]);
    else if (strstr(what,"4")) printf("%0.3f",ms->MODS.blueQC[3]);
    else if (strstr(what,"5")) printf("%0.3f",ms->MODS.redQC[0]);
    else if (strstr(what,"6")) printf("%0.3f",ms->MODS.redQC[1]);
    else if (strstr(what,"7")) printf("%0.3f",ms->MODS.redQC[2]);
    else if (strstr(what,"8")) printf("%0.3f",ms->MODS.redQC[3]);
    else {
      printf("Quadcells (BLUE): %0.4f %0.4f %0.4f %0.4f\n",
	     ms->MODS.blueQC[0],
	     ms->MODS.blueQC[1],
	     ms->MODS.blueQC[2],
	     ms->MODS.blueQC[3]);
      printf("Quadcells (RED): %0.4f %0.4f %0.4f %0.4f\n",
	     ms->MODS.redQC[0],
	     ms->MODS.redQC[1],
	     ms->MODS.redQC[2],
	     ms->MODS.redQC[3]);
      printf("Qcells Relative Moves: %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f\n",
	     ms->MODS.motorv[23],
	     ms->MODS.motorv[24],
	     ms->MODS.motorv[26],
	     ms->MODS.motorv[2],
	     ms->MODS.motorv[3],
	     ms->MODS.motorv[4]);
      printf("Motors Pos. : %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f\n",
	     ms->MODS.pos[23],
	     ms->MODS.pos[24],
	     ms->MODS.pos[25],
	     ms->MODS.pos[2],
	     ms->MODS.pos[3],
	     ms->MODS.pos[4]);
    }
    exit(0);
  }
  // IMCS quad-cell parameters
  else if (strstr(what,"HEBQC")) {
    if (strstr(what,"1")) printf("%d",ms->MODS.blueQC1);
    else if (strstr(what,"2")) printf("%d",ms->MODS.blueQC2);
    else if (strstr(what,"3")) printf("%d",ms->MODS.blueQC3);
    else if (strstr(what,"4")) printf("%d",ms->MODS.blueQC4);
    else if (strstr(what,"5")) printf("%d",ms->MODS.redQC1);
    else if (strstr(what,"6")) printf("%d",ms->MODS.redQC2);
    else if (strstr(what,"7")) printf("%d",ms->MODS.redQC3);
    else if (strstr(what,"8")) printf("%d",ms->MODS.redQC4);
    else {
      printf("Quadcells (BLUE): %d %d %d %d\n",
	     ms->MODS.blueQC1,
	     ms->MODS.blueQC2,
	     ms->MODS.blueQC3,
	     ms->MODS.blueQC4);
      printf("Quadcells (RED): %d %d %d %d\n",
	     ms->MODS.redQC1,
	     ms->MODS.redQC2,
	     ms->MODS.redQC3,
	     ms->MODS.redQC4);
      printf("Qcells Relative Moves: %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f\n",
	     ms->MODS.motorv[23],
	     ms->MODS.motorv[24],
	     ms->MODS.motorv[26],
	     ms->MODS.motorv[2],
	     ms->MODS.motorv[3],
	     ms->MODS.motorv[4]);
      printf("Motors Pos. : %0.4f %0.4f %0.4f %0.4f %0.4f %0.4f\n",
	     ms->MODS.pos[23],
	     ms->MODS.pos[24],
	     ms->MODS.pos[25],
	     ms->MODS.pos[2],
	     ms->MODS.pos[3],
	     ms->MODS.pos[4]);
    }
    exit(0);
  }
  // IMCS X,Y,Z motors
  else if (strstr(what,"OSCI")) {
    if (strstr(what,"1")) printf("%0.4f",ms->MODS.blueQC_X[0]);
    else if (strstr(what,"2")) printf("%0.4f",ms->MODS.blueQC_Y[0]);
    else if (strstr(what,"3")) printf("%0.4f",ms->MODS.blueQC_Z[0]);
    else if (strstr(what,"4")) printf("%0.4f",ms->MODS.redQC_X[0]);
    else if (strstr(what,"5")) printf("%0.4f",ms->MODS.redQC_Y[0]);
    else if (strstr(what,"6")) printf("%0.4f",ms->MODS.redQC_Z[0]);
    else {
      printf("IEB1 X:%0.4f Y:%0.4f Z:%0.4f - IEB2 X:%0.4f Y:%0.4f Z:%0.4f\n",
	     ms->MODS.blueQC_X[0],
	     ms->MODS.blueQC_Y[0],
	     ms->MODS.blueQC_Z[0],
	     ms->MODS.redQC_X[0],
	     ms->MODS.redQC_Y[0],
	     ms->MODS.redQC_Z[0]);
    }
    exit(0);
  }
  // list mechanisms
  else if (!strcasecmp(what,"WHO")) {
    for (i=0;i<MAX_ML;i++) {
      printf("Mechanism [%02d]: %s\n",i+1,makeUpper(ms->MODS.who[i]));
    }
  }
  // What mechanism is assign to this ID
  else if (!strcasecmp(what,"WHOIS")) {
    unit = atoi(cmd);
    printf("%s\n",ms->MODS.who[unit-1]);
    exit(0);
  }
  // IP addresses
  else if (!strcasecmp(what,"IPMAP")) {
    if (atoi(cmd) == -1)
      for (i=0;i<MAX_ML-1;i++) { 
	strcpy(shm_addr->MODS.TTYIP[i],"NONE:NONE");
        strcpy(shm_addr->MODS.commport[i].Port,"NONE:NONE");
        strcpy(shm_addr->MODS.who[i],"nomech");
        shm_addr->MODS.timeout[i]=0;
      }
    for (i=0;i<MAX_ML-1;i++) { 
      //if (strncasecmp(ms->MODS.TTYIP[i],"NONE",4)) {
      if (ms->MODS.ieb_i[i]==-1) {
	printf("IEB%d [ML%d]%s,  ",ms->MODS.ieb_i[i],i+1,ms->MODS.who[i]);
	printf("%s,  ",ms->MODS.TTYIP[i]);
	printf("BUSY[%d],  ",ms->MODS.busy[i]);
	printf("HOST[%d])\n",ms->MODS.host[i]);

      } else if (ms->MODS.ieb_i[i]==0) {
	printf("IEB%d [ML%d]%s,  ",ms->MODS.ieb_i[i],i+1,ms->MODS.who[i]);
	printf("%s,  ",ms->MODS.TTYIP[i]);
	printf("BUSY[%d],  ",ms->MODS.busy[i]);
	printf("HOST[%d])\n",ms->MODS.host[i]);

      } else if (ms->MODS.ieb_i[i]==1) {
	printf("IEB%d [ML%d]%s,  ",ms->MODS.ieb_i[i],i+1,ms->MODS.who[i]);
	printf("%s,  ",ms->MODS.TTYIP[i]);
	printf("BUSY[%d],  ",ms->MODS.busy[i]);
	printf("HOST[%d])\n",ms->MODS.host[i]);

      } else {
	//printf("IEB%d [ML%d]%s,  ",ms->MODS.ieb_i[i],i-17,ms->MODS.who[i]);
	printf("IEB%d [ML%d]%s,  ",ms->MODS.ieb_i[i],i+1,ms->MODS.who[i]);
	printf("%s,  ",ms->MODS.TTYIP[i]);
	printf("BUSY[%d],  ",ms->MODS.busy[i]);
	printf("HOST[%d])\n",ms->MODS.host[i]);
      }
    }

    printf("\nWAGO Port 502 on IP Addresses:\n");
    printf("  %s %s\n",ms->MODS.WAGOWHO[0],ms->MODS.WAGOIP[0]);
    printf("  %s %s\n",ms->MODS.WAGOWHO[1],ms->MODS.WAGOIP[1]);
    printf("  %s %s\n",ms->MODS.WAGOWHO[2],ms->MODS.WAGOIP[2]);
    printf("  %s %s\n",ms->MODS.WAGOWHO[3],ms->MODS.WAGOIP[3]);
    printf("  %s %s\n",ms->MODS.WAGOWHO[4],ms->MODS.WAGOIP[4]);
    printf("  %s %s\n",ms->MODS.WAGOWHO[5],ms->MODS.WAGOIP[5]);

    // qc_port[] -> qc_addr[] for new WAGO quad cell system [rwp/osu - 2025 June]
    
    printf("\nQuadCell WAGO info:\n");
    printf("  %s: ipAddr=%s regAddr=%d\n",ms->MODS.QC_WHO[0],ms->MODS.QC_IP[0],ms->MODS.QC_REG[0]);
    printf("  %s: ipAddr=%s regAddr=%d\n",ms->MODS.QC_WHO[1],ms->MODS.QC_IP[1],ms->MODS.QC_REG[1]);

    printf("\nServer IP Sockets: ");
    printf("  IIF=%d, ",ms->MODS.modsPorts[0]);
    printf("  AGW=%d, ",ms->MODS.modsPorts[1]);
    printf("  MMC=%d, ",ms->MODS.modsPorts[2]);
    printf("  OTHERS=%d\n",ms->MODS.modsPorts[3]);
    exit(0);
  }
  
  // Power state variables
  else if (!strcasecmp(what,"POWER")) {
    
    printf("Power States:\n");
    printf("         IUB: %s\n",((ms->MODS.utilState==0) ? "Off" : (ms->MODS.utilState==1) ? "On" : "Fault"));
    printf("         LLB: %s\n",((ms->MODS.llbState==0) ? "Off" : (ms->MODS.llbState==1) ? "On" : "Fault"));    
    printf("   Guide Cam: %s\n",((ms->MODS.guideCamState==0) ? "Off" : (ms->MODS.guideCamState==1) ? "On" : "Fault"));    
    printf("     WFS Cam: %s\n",((ms->MODS.wfsCamState==0) ? "Off" : (ms->MODS.wfsCamState==1) ? "On" : "Fault"));    
    printf(" Blue Channel:\n");
    printf("       IEB-B: %s\n",((ms->MODS.blueIEBState==0) ? "Off" : (ms->MODS.blueIEBState==1) ? "On" : "Fault"));    
    printf("       HEB-B: %s\n",((ms->MODS.blueHEBState==0) ? "Off" : (ms->MODS.blueHEBState==1) ? "On" : "Fault"));    
    printf("    Archon-B: %s\n", (ms->MODS.blueArchonState) ? "On" : "Off");
    printf("  IonGauge-B: %s\n", (ms->MODS.blueIonGaugeState) ? "On" : "Off");
    printf(" Red Channel:\n");
    printf("       IEB-R: %s\n",((ms->MODS.redIEBState==0) ? "Off" : (ms->MODS.redIEBState==1) ? "On" : "Fault"));    
    printf("       HEB-R: %s\n",((ms->MODS.redHEBState==0) ? "Off" : (ms->MODS.redHEBState==1) ? "On" : "Fault"));    
    printf("    Archon-R: %s\n", (ms->MODS.redArchonState) ? "On" : "Off");
    printf("  IonGauge-R: %s\n", (ms->MODS.redIonGaugeState) ? "On" : "Off");
    exit(0);
  }

  // Concise power status for 3rd party apps
  //
  // returns: IUB LLB GCAM WFS IEB_B HEB_B Archon_B IG_B IEB_R HEB_R Archon_R IG_R
  //
  else if (!strcasecmp(what,"PSTATUS")) {
    sprintf(buff,"%s",((ms->MODS.utilState==0) ? "Off" : (ms->MODS.utilState==1) ? "On" : "Fault"));
    sprintf(buff,"%s %s",buff,((ms->MODS.llbState==0) ? "Off" : (ms->MODS.llbState==1) ? "On" : "Fault"));    
    sprintf(buff,"%s %s",buff,((ms->MODS.guideCamState==0) ? "Off" : (ms->MODS.guideCamState==1) ? "On" : "Fault"));    
    sprintf(buff,"%s %s",buff,((ms->MODS.wfsCamState==0) ? "Off" : (ms->MODS.wfsCamState==1) ? "On" : "Fault"));    
    sprintf(buff,"%s %s",buff,((ms->MODS.blueIEBState==0) ? "Off" : (ms->MODS.blueIEBState==1) ? "On" : "Fault"));    
    sprintf(buff,"%s %s",buff,((ms->MODS.blueHEBState==0) ? "Off" : (ms->MODS.blueHEBState==1) ? "On" : "Fault"));    
    sprintf(buff,"%s %s",buff, (ms->MODS.blueArchonState) ? "On" : "Off");
    sprintf(buff,"%s %s",buff, (ms->MODS.blueIonGaugeState) ? "On" : "Off");
    sprintf(buff,"%s %s",buff,((ms->MODS.redIEBState==0) ? "Off" : (ms->MODS.redIEBState==1) ? "On" : "Fault"));     
    sprintf(buff,"%s %s",buff,((ms->MODS.redHEBState==0) ? "Off" : (ms->MODS.redHEBState==1) ? "On" : "Fault"));     
    sprintf(buff,"%s %s",buff, (ms->MODS.redArchonState) ? "On" : "Off");
    sprintf(buff,"%s %s",buff, (ms->MODS.redIonGaugeState) ? "On" : "Off");
    printf("%s\n",buff);
    exit(0);
  }
  
  // Common
  else if (!strcasecmp(what,"ISLCOMMON")) {
    for (i=0;i<MAX_ML-1;i++) {
      printf("[%s {ML%d:%0.4f}]\n",ms->MODS.who[i],i,ms->MODS.pos[i]);
    }
    exit(0);
  }

  // Mechanism "whois" by number
  else if (!strcasecmp(what,"WHOISIT")) {
    i=atoi(cmd);
    printf("%s\n", ms->MODS.who[i]);
    exit(0);
  }
  // MOTOR position
  else if (!strcasecmp(what,"MPOS")) {
    if (atoi(cmd)==1) {
      for (i=0;i<16;i++) {
	memset(buff,0,sizeof(buff));
	if (ms->MODS.busy[i]==0)
	  printf("%s[ML%d] %6.1f !IDLE!\n", ms->MODS.who[i],i+1,ms->MODS.pos[i]);
	else
	  printf("%s[ML%d] %6.1f @BUSY@\n", ms->MODS.who[i],i+1,ms->MODS.pos[i]);
      }
    } else if (atoi(cmd)==2) {
      for (i=16;i<MAX_ML-1;i++) {
	//for (i=17;i<MAX_ML-1;i++) {
	if (ms->MODS.busy[i]==0) 
	  printf("%s[ML%d] %6.1f !IDLE!\n", ms->MODS.who[i],i+1,ms->MODS.pos[i]);
	else 
	  printf("%s[ML%d] %6.1f @BUSY@\n", ms->MODS.who[i],i+1,ms->MODS.pos[i]);
      }
    }
    exit(0);
  }
  // MOTOR requested position
  else if (!strcasecmp(what,"RPOS")) {
    printf("(id) MECHANISM: Request.\n");
       for (i=0;i<MAX_ML-1;i++) printf("[ML%d] %s: %6.1f\n",
				 i+1,ms->MODS.who[i],ms->MODS.reqpos[i]);
    exit(0);

  }
  else if (!strcasecmp(what,"BUSY")) {
    printf("%d", ms->MODS.busy[atoi(cmd)]);
    exit(0);

  }
  else if (!strcasecmp(what,"QUED")) {
    printf("%d", ms->MODS.qued[atoi(cmd)]);
    exit(0);

  }
  else if (!strcasecmp(what,"HOST")) {
    printf("%d", ms->MODS.host[atoi(cmd)]);
    exit(0);

    // release a semaphore
  }
  else if (!strcasecmp(what,"RELSEM")) {
    nsem_take((char*)"islctl ",0);
    exit(0);

    // Check semaphore
  }
  else if (!strcasecmp(what,"CHKSEM")) {
    if ( 1 == nsem_take((char*)"isl    ",1)) {
      printf("isl already running");
    }
    exit(0);

    // IMCS frequency info.
  }
  else if (strstr(what,"FREQ")) {
    if (strstr(what,"1")) ms->MODS.blueQC_SampleRate = atoi(cmd);
    else if (strstr(what,"2")) ms->MODS.redQC_SampleRate = atoi(cmd);
    else printf("%d,%d\n",ms->MODS.blueQC_SampleRate,ms->MODS.redQC_SampleRate);
    exit(0);

    // IMCS data rate info.
  }
  else if (strstr(what,"RATE")) {
    if (strstr(what,"1")) ms->MODS.blueQC_Samples = atoi(cmd);
    else if (strstr(what,"2")) ms->MODS.redQC_Samples = atoi(cmd);
    else printf("%d,%d\n",ms->MODS.blueQC_Samples,ms->MODS.redQC_Samples);
    exit(0);

    // IMCS Loop info.
  }
  else  if (strstr(what,"CLOSELOOP")) {
    if (!strcasecmp(what,"CLOSELOOPOFF1")) {
      ms->MODS.redCloseLoop = 0;
      ms->MODS.redCloseLoopON = 0;
    } else if (!strcasecmp(what,"CLOSELOOPON1")) {
      ms->MODS.redCloseLoop = 1;
      ms->MODS.redCloseLoopON = 1;
    } else if (!strcasecmp(what,"CLOSELOOPOFF")) {
      ms->MODS.blueCloseLoop = 0;
      ms->MODS.blueCloseLoopON = 0;
    } else if (!strcasecmp(what,"CLOSELOOPON")) {
      ms->MODS.blueCloseLoop = 1;
      ms->MODS.blueCloseLoopON = 1;
    } else if (!strcasecmp(what,"RCLOSELOOP")) {
      printf("%d%d\n",ms->MODS.redCloseLoop,ms->MODS.redCloseLoopON);
    } else if (!strcasecmp(what,"BCLOSELOOP")) {
      printf("%d%d\n",ms->MODS.blueCloseLoop,ms->MODS.blueCloseLoopON);
    } else {
      printf("Blue: %d On: %d\n Red: %d On: %d\n",ms->MODS.blueCloseLoop,ms->MODS.blueCloseLoopON,
	     ms->MODS.redCloseLoop,ms->MODS.redCloseLoopON);
    }
    exit(0);

  }
  else if (!strcasecmp(what,"TARGETON")) {
    ms->MODS.blueQC_TARGET = 1;
    printf("%d\n",ms->MODS.blueQC_TARGET);
    exit(0);

  }
  else if (!strcasecmp(what,"TARGETOFF")) {
    ms->MODS.blueQC_TARGET = 0;
    printf("%d\n",ms->MODS.blueQC_TARGET);
    exit(0);

  }
  else if (!strcasecmp(what,"RTARGETON")) {
    ms->MODS.redQC_TARGET = 1;
    printf("%d\n",ms->MODS.redQC_TARGET);
    exit(0);

  }
  else if (!strcasecmp(what,"RTARGETOFF")) {
    ms->MODS.redQC_TARGET = 0;
    printf("%d\n",ms->MODS.redQC_TARGET);
    exit(0);

  }
  // RED IMCS Target status
  else if (!strcasecmp(what,"RTARGETSTATUS")) {
    printf("%d\n",ms->MODS.redQC_TARGET);
    exit(0);
  }
  // BLUE IMCS Target status
  else if (!strcasecmp(what,"TARGETSTATUS")) {
    printf("%d\n",ms->MODS.blueQC_TARGET);
    exit(0);
  }
  // kill IMCS or other logging
  else if (!strcasecmp(what,"KILLLOGGING0")) {
    system("killall 'logimcs 10'");
    exit(0);
  }
  // kill IMCS or other logging
  else if (!strcasecmp(what,"KILLLOGGING1")) {
    system("killall 'logimcs 11'");
    exit(0);
  }
  // IMCS motor thresholds
  else if (!strcasecmp(what,"STHRESHOLD")) {
    if (atoi(cmd)==-1) printf("%0.3f\n",ms->MODS.blueQC_Threshold[0]);
    else  ms->MODS.blueQC_Threshold[0] = atof(cmd);
    exit(0);

  }
  else if (!strcasecmp(what,"RSTHRESHOLD")) {
    if (atoi(cmd)==-1) printf("%0.3f\n",ms->MODS.redQC_Threshold[0]);
    else  ms->MODS.redQC_Threshold[0] = atof(cmd);
    exit(0);
  }
  // mechanism position
  else if (strstr(what,"MLCDEV")) {
    char temp[80];
    i=getMechanismID(cmd,temp); // Get mechanism device ID
    if (i==-1) printf("[%d]NO_MECH\n",i);
    else printf("microLynx Controller=%d\n",i);
    exit(0);
  }
  // mechanism position
  else if (strstr(what,"MLCPOS")) {
    char temp[80];
    i=getMechanismID(cmd,temp); // Get mechanism device ID
    if (i==-1) printf("[%d]NO_MECH\n",i);
    else printf("%s=%.0f\n",ms->MODS.who[i],ms->MODS.pos[i]);
    exit(0);
  
  }
  else if (strstr(what,"MLC")) {
    i=atoi(&what[3]);
    printf("%.0f\n",ms->MODS.pos[i]);
    exit(0);
  }
  // slitmask mechanism position
  else if (strstr(what,"MASKSM")) {
    if (what[6]=='S') printf("%c\n",ms->MODS.maskselect);
    else if (what[6]=='I') printf("%c\n",ms->MODS.maskinsert);
    else if (what[6]=='A') printf("%d\n",ms->MODS.active_smask);
    exit(0);
  }
  // command mechanism name with an SNTool command
  else if (!strcasecmp(what,"SNCMD")) {
    i = system(cmd);
    exit(0);
  }
  // command Mechanisms with the mmcServer 
  else if (!strcasecmp(what,"MMCU")) {
    sprintf(ipascii,"%d",shm_addr->MODS.modsPorts[2]);
    memset(buff,0,sizeof(buff));
    i=mmcu((char*)"localhost",ipascii,cmd,buff);
    if (strstr(cmd,"#")) {
      for(i=0;buff[i]!='=';i++);
      i++;
      printf("%s\n",&buff[i]);
    } else printf("%s\n",buff);
    exit(0);
  }
  // command AGW with the agwServer
  else if (!strcasecmp(what,"AGWCU")) {
    //sprintf(ipascii,"%d",shm_addr->MODS.modsPorts[1]);
    memset(buff,0,sizeof(buff));
    //i=agwcu("localhost",ipascii,cmd,buff);
    i=agwcu((char*)"localhost",0,cmd,buff);
    printf("%s\n",buff);
    exit(0);
  }
  // command TCS with the iifServer
  else if (!strcasecmp(what,"TCSCU")) {
    sprintf(ipascii,"%d",shm_addr->MODS.modsPorts[0]);
    memset(buff,0,sizeof(buff));
    //    fprintf(stderr,"%s, %s, %s",ipascii,cmd,buff);
    //    i=iifcu("localhost",ipascii,cmd,buff);
    printf("%s\n",buff);
    exit(0);
  }
  // Calibration Lamps bits
  else if (strstr(what,"LASERS")) {
    printf("IR Laser State:%d, BeamState:%d, Power:%f, Setpoint: %d, Temp:%f\n",
	   ms->MODS.lasers.irlaser_state,
	   ms->MODS.lasers.irbeam_state,
	   ms->MODS.lasers.irlaser_power,
	   ms->MODS.lasers.irlaser_setpoint,
	   ms->MODS.lasers.irlaser_temp);
    printf("(TimeON:%d, Age:%d, Cycle:%d)\n",
	   ms->MODS.lasers.irlaser_timeon,
	   ms->MODS.lasers.irlaser_age,
	   ms->MODS.lasers.irlaser_cycle);
    
    printf("VIS Laser State:%d, BeamState:%d, Power:%f\n",
	   ms->MODS.lasers.vislaser_state,
	   ms->MODS.lasers.visbeam_state,
	   ms->MODS.lasers.vislaser_power);
    printf("(TimeON:%d, Age:%d, Cycle:%d)\n",
	   ms->MODS.lasers.vislaser_timeon,
	   ms->MODS.lasers.vislaser_age,
	   ms->MODS.lasers.vislaser_cycle);
    exit(0);
  }
  // Calibration Lamps bits
  else if (strstr(what,"LAMPS")) {
    if (what[5]=='R') {
      for(i=0;i<9;i++) {
	ms->MODS.lamps.lamp_state[i] = 0;
	ms->MODS.lamps.lamp_timeon[i] = 0;
	ms->MODS.lamps.lamp_age[i] = 0;
	ms->MODS.lamps.lamp_cycle[i] = 0;
      }
      ms->MODS.lamps.lamplaser_all[0] = 0;
      ms->MODS.lasers.vislaser_age=ms->MODS.lasers.vislaser_cycle=0;
      ms->MODS.lasers.irlaser_age=ms->MODS.lasers.irlaser_cycle=0;
    } else if (what[5]=='V') {
      ms->MODS.lamps.lamplaser_all[0] = atoi(cmd);
    } else {
      char calibL[9][10]= \
	{"AR","XE","NE","HG","KR","V6PS","FLAT1","FLAT2","FLAT3"};
      for(i=0;i<9;i++) {
	printf("%s (State:%d, TimeON:%d, Age:%d, Cycle:%d)\n",
	       //	       ms->MODS.lamps.lamp_state[i],
	       calibL[i],
	       ms->MODS.lamps.lamp_state[i],
	       ms->MODS.lamps.lamp_timeon[i],
	       ms->MODS.lamps.lamp_age[i],
	       ms->MODS.lamps.lamp_cycle[i]);
      }
      printf("Lamps Register:%d\n", ms->MODS.lamps.lamplaser_all[0]);
    }
    exit(0);
  }
  // Mask and location of Select mechanism
  else if (!strcasecmp(what,"MODS.MASKS")) {
    for(i=0;i<13;i++) {
      if (i==0) { printf("TIME: %s\n",ms->MODS.slitmaskName[i]); i++;}
      if (ms->MODS.active_smask==i)
      	printf("@>MASK %d:%s\t %s\n",i,ms->MODS.slitmaskName[i],ms->MODS.slitmaskInfo[i]);
      else
	printf("MASK %d:%s\t %s\n",i,ms->MODS.slitmaskName[i],ms->MODS.slitmaskInfo[i]);
    }
    if (ms->MODS.active_smask==-1)
      printf("@>=======:BRACE\n");
    else
      printf("=======:BRACE\n");

    for(;i<25;i++) {
      if (ms->MODS.active_smask==i)
      	printf("@>MASK %d:%s\t %s\n",i,ms->MODS.slitmaskName[i],ms->MODS.slitmaskInfo[i]);
      else
	printf("MASK %d:%s\t %s\n",i,ms->MODS.slitmaskName[i],ms->MODS.slitmaskInfo[i]);
    }
    exit(0);
  }
  // Camera and AGw filters
  else if (!strcasecmp(what,"MODS.FILTERS")) {
    for(i=0;i<9;i++) {
      if (i==0) printf("TIME_Red: %s\n",ms->MODS.rcamfilters[i]);
      else printf("RED_FILTERS %d:%s\t %s\n",i,ms->MODS.rcamfilters[i],ms->MODS.rcamfiltInfo[i]);
    }

    for(i=0;i<9;i++) {
      if (i==0) printf("TIME_Blue: %s\n",ms->MODS.bcamfilters[i]);
      else printf("BLUE_FILTERS %d:%s\t %s\n",i,ms->MODS.bcamfilters[i],ms->MODS.rcamfiltInfo[i]);
    }

    for(i=0;i<5;i++) {
      if (i==0) printf("TIME_AGW: %s\n",ms->MODS.agwfilters[i]);
      else printf("AGW_FILTERS %d:%s\t %s\n",i,ms->MODS.agwfilters[i],ms->MODS.rcamfiltInfo[i]);
    }

    exit(0);
  }

  // Gratings
  else if (!strcasecmp(what,"MODS.GRATINGS")) {
    for(i=0;i<5;i++) {
      if (i==0) printf("TIME_Red: %s\n",ms->MODS.rgrating[i]);
      else printf("RED_GRATING %d:%s\t %s\n",i,ms->MODS.rgrating[i],ms->MODS.rgratInfo[i]);
    }

    for(i=0;i<5;i++) {
      if (i==0) printf("TIME_Blue: %s\n",ms->MODS.bgrating[i]);
      else printf("BLUE_GRATING %d:%s\t %s\n",i,ms->MODS.bgrating[i],ms->MODS.rgratInfo[i]);
    }
    exit(0);
  }
  // Gratings
  else if (!strcasecmp(what,"MODS.DICHROIC")) {
    for(i=0;i<4;i++) {
      if (i==0) printf("TIME: %s\n",ms->MODS.rgrating[i]);
      else printf("DICHROIC %d:%s\t %s\n",i,ms->MODS.dichroicName[i],ms->MODS.dichroicInfo[i]);
    }
    exit(0);

  }
  else if (!strcasecmp(what,"LOCKS")) {
    for(i=0;i<32;i++) {
      if (ms->MODS.LOCKS[i]==1)
      	printf("%s LOCKED OUT\n",ms->MODS.who[i]);
      else 
      	printf("%s UNLOCKED\n",ms->MODS.who[i]);
    }
    exit(0);

  }
  else if (!strcasecmp(what,"AGWLOCATE")) {
    exit(0);
  }
  // AGW location
  else if (strstr(what,"AGWVAL")) {
    if (strstr(what,"X")) ms->MODS.pos[19] = atoi(cmd);
    else if (strstr(what,"Y")) ms->MODS.pos[18] = atoi(cmd);
    else if (strstr(what,"FP")) ms->MODS.pos[20] = atoi(cmd);
    else if (strstr(what,"FW")) ms->MODS.pos[21] = atoi(cmd);
    else if (strstr(what,"1")) printf("%.1f\n",ms->MODS.pos[19]);
    else if (strstr(what,"2")) printf("%.1f\n",ms->MODS.pos[18]);
    else if (strstr(what,"3")) printf("%.1f\n",ms->MODS.pos[20]);
    else if (strstr(what,"4")) printf("%.1f\n",ms->MODS.pos[21]);
    else {
      printf("Xs=%.1f, Ys=%.1f, Focus=%.1f, Filter=%.1f\n",
	     ms->MODS.pos[19], ms->MODS.pos[18],
	     ms->MODS.pos[20], ms->MODS.pos[21]);
    }
    exit(0);
    
  }
  else if (!strcasecmp(what,"TCSRA")) {
    printf("%s\n",ms->targRA);
    exit(0);
  }
  else if (!strcasecmp(what,"TCSDEC")) {
    printf("%s\n",ms->targDec);
    exit(0);
  }
  else if (!strcasecmp(what,"TCSAZ")) {
    printf("%0.4f\n",ms->targAz);
    exit(0);
  }
  else if (!strcasecmp(what,"TCSEL")) {
    printf("%0.4f\n",ms->targEl);
    exit(0);
  }
  else if (!strcasecmp(what,"GSRA")) {
    i=atoi(&what[4]);
    printf("%s\n",ms->gsRA[i]);
    exit(0);
  }
  else if (!strcasecmp(what,"GSDEC")) {
    i=atoi(&what[5]);
    printf("%s\n",ms->gsDec[i]);
    exit(0);
  }
  else if (!strcasecmp(what,"GSNUM")) {
    printf("%d\n",ms->nGuideStars);
    exit(0);
  }
  else if (!strcasecmp(what,"GSMAG")) {
    i=atoi(&what[5]);
    printf("%0.4f\n",ms->gsMag[i]);
    exit(0);
  }
  else if (!strcasecmp(what,"IIFINIT")) {
    printf("%d\n",ms->Init);
    exit(0);
  }
  else if (!strcasecmp(what,"TCSAUTH")) {
    printf("%d\n",ms->Auth);
    exit(0);
  }
  else if (!strcasecmp(what,"TCSCOOR")) {
    printf("%s\n",ms->targCoord);
    exit(0);
  }
  else if (!strcasecmp(what,"TCSEQUINOX")) {
    printf("%s\n",ms->targEquinox);
    exit(0);
  }
  else if (!strcasecmp(what,"SHRMEM")) {
    cout << "*NOT* Operational yet!" << endl;
    exit(0);

  }
  else if (!strcasecmp(what,"AGWFILTERS")) {
    printf("AGW FILTERS date:%s\nFilter1=%s, Filter2=%s, Filter3=%s, Filter4=%s\n",
	   ms->MODS.agwfilters[0], ms->MODS.agwfilters[1],
	   ms->MODS.agwfilters[2], ms->MODS.agwfilters[3],
	   ms->MODS.agwfilters[4]);
    exit(0);

  }
  else if (!strcasecmp(what,"RFILTERS")) {
    printf("RED CAMERA FILTERS date:%s\nFilter1=%s, Filter2=%s, Filter3=%s, Filter4=%s\nFilter5=%s, Filter6=%s, Filter7=%s, Filter8=%s\n",
	   ms->MODS.rcamfilters[0], ms->MODS.rcamfilters[1],
	   ms->MODS.rcamfilters[2], ms->MODS.rcamfilters[3],
	   ms->MODS.rcamfilters[4], ms->MODS.rcamfilters[5],
	   ms->MODS.rcamfilters[6], ms->MODS.rcamfilters[7],
	   ms->MODS.rcamfilters[8]);
    exit(0);

  }
  else if (!strcasecmp(what,"BFILTERS")) {
    printf("RED CAMERA FILTERS date:%s\nFilter1=%s, Filter2=%s, Filter3=%s, Filter4=%s\nFilter5=%s, Filter6=%s, Filter7=%s, Filter8=%s\n",
	   ms->MODS.bcamfilters[0], ms->MODS.bcamfilters[1],
	   ms->MODS.bcamfilters[2], ms->MODS.bcamfilters[3],
	   ms->MODS.bcamfilters[4], ms->MODS.bcamfilters[5],
	   ms->MODS.bcamfilters[6], ms->MODS.bcamfilters[7],
	   ms->MODS.bcamfilters[8]);
    exit(0);
  }
  // Error messages
  else if (!strcasecmp(what,"ERRMSG")) {
    for(i=0;i<MAX_ML-1;i++)
      if (strlen(shm_addr->MODS.ieb_msg[i])>0)
	printf("%s\n",shm_addr->MODS.ieb_msg[i]);
    exit(0);
  }
  
  else {
    printf("vueinfo - unrecognized option %s\n",what);
    exit(0);
  }
}

/*
#define MAX_LEN 256+1

void display_it();
display_it(irow,icol,"M",buffer);
printf("\n");
display_it(irow,icol,"R",buffer);
printf("\n");
display_it(irow,icol,"Y",buffer);
printf("\n");
display_it(irow,icol,"B",buffer);
*/
/************************************************************************
examples:
display_it(0, 4, "Y5", string);
display_it(1, 4, "R5", string);
display_it(2, 4, "C", STRING);
display_it(2, 4, "Y", STING);
display_it(2, 40, "", string);
display_it(2, 105, "Y", "BAD ");
display_it(2,105,"", "GOOD");
display_it(2, 4, "B", "VAC  MKIII     STOP  ????????");
************************************************************************/
/* Function declarations:
 *   Row - Screen row position
 *   Col - Screen column position
 *   Attrib - Modifiying attributes (blink, underline, red, blue...)
 *   InLine - The text to print
 */
void
display_it(int Row, int Col, char *Attrib, char *InLine)
{
  int i;

  /* Check for attribute modifiers */
  if (strstr(Attrib, "R")){
    (void)printf("%c[1;31m", 27);
  }
  else if (strstr(Attrib, "G"))
    (void)printf("%c[1;32m", 27);
  else if (strstr(Attrib, "Y"))
    (void)printf("%c[1;33m", 27);
  else if (strstr(Attrib, "B")) {
    (void)printf("%c[1;34m", 27);
  }
  else if (strstr(Attrib, "M"))
    (void)printf("%c[1;35m", 27);
  else if (strstr(Attrib, "C"))
    (void)printf("%c[1;36m", 27);
  else if (strstr(Attrib, "W"))
    (void)printf("%c[1;37m", 27);
  else if (strstr(Attrib, "X"))
    (void)printf("%c[1;30m", 27);
  
  if (strstr(Attrib, "7"))
    (void)printf("%c[7m", 27);
  if (strstr(Attrib, "5"))
    (void)printf("%c[5m", 27);
  if (strstr(Attrib, "4"))
    (void)printf("%c[4m", 27);
  if (strstr(Attrib, "1"))
    (void)printf("%c[1m", 27);
  if (strstr(Attrib, "0"))
    (void)printf("%c[0m", 27);
  
/* Position the cursor and print the text. If Row and Col are 0, just print */
  if (Row== 0 && Col== 0)
    (void)printf("%s", InLine);
  else
    (void)printf("%c[%d;%dH%s", 27, Row, Col, InLine);
  
/* Reset the video attributes. Don't send anything if we don't have to */
  if (*Attrib!= '\0')
    (void)printf("%c[0m", 27);
  
}


//---------------------------------------------------------------------------
// makeUpper() - return all characters in a string in uppercase
//
 
/*!
  \brief Return uppercase string
 
  \param origStr string remains unchanged.

  \returns pointer to an uppercase string, 
 
  Helper utility will return requested string in uppercase, and leave
  the requested string unchanged.
 
*/
 
char* makeUpper(char *origStr)
{
  char *workStr = origStr;
  for (int i=0; workStr[i]!='\0'; i++) {
    if (workStr[i]>='a' && workStr[i]<='z')
       workStr[i] = workStr[i] - 32;
  }
  return workStr;
}
