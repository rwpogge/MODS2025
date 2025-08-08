/* general isl parameter header */

// 2025 June 21 - AlmaLinux 9 port updates [rwp/osu]

#ifndef TRUE
#define TRUE        1
#define FALSE       0
#endif

#define WORD_BIT    32
#define PAGE_SIZE   4096

#define C_RES       35*PAGE_SIZE      // reserves bytes for Islcom
#define SHM_SIZE    C_RES+2*PAGE_SIZE // should be a multiple of 4096

#define CLS_SIZE    20480
#define MAX_CLS     40

#define SKD_SIZE    4096
#define ONEK_SIZE   1024

#define BRK_SIZE    1024

#define SEM_GO       0
#define SEM_SEM      1
#define SEM_CLS      2

#define SEM_NUM     40

#define MAX_PTS     3

#ifndef TRUE
#define TRUE        1
#define FALSE       0
#endif

#define BAD_ADDR    (char *)(-1)

#define ISLPGM_CTL "/home2/isl/control/islpgm.ctl"

#define ADDR_ST   "addr"              
#define TEST      "test"
#define REBOOT    "reboot"
#define BAD_VALUE "BAD_VALUE"

#define MAX_ML     40        // maximum number of microlynx controllers
#define MAX_MOTORS 40        // maximum number of motors in a mods
#define MAX_DIST   2
#define MAX_USER_DEV  40

#define DEV_MLC     "im"

// devices

#define FW         0x400	// Filter Wheel
#define CALIB      0x200	// Calibration
#define AGW        0x100	// AGW 
#define SM         0x080	// Slit Masks 
#define DICRO      0x040	// Dichroic
#define SPAREA     0x020
#define SPAREB     0x010
#define SPAREC     0x008
#define SPARED     0x004
#define SPAREE     0x002
#define SPAREF     0x001

// MicroLYNX Controller and other drive _types

#define MLC1        0x1000
#define BRUCE       0x2000
#define OTHER       0x4000
#define SPARE1      0x8000
#define SPARE2      0x10000
#define SPARE3      0x20000
#define SPARE4      0x40000
#define SPARE5      0x80000
#define SPARE6      0x100000
#define SPARE7      0x200000
#define SPARE8      0x400000
#define SPARE9      0x800000
#define SPARE10     0x1000000
#define SPARE11     0x2000000
#define SPARE12     0x4000000
#define SPARE13     0x8000000

//
// The number of ISL allowed must be less than 8, currently we allow 2
// also add additional MLC commands in /home2/isl/control/islcmd.ctl
// and creates equivalent help file soft-links in isl/help
//

#define MAX_ISL	2	        // Max no of ISL allowed
#define ISL_TEMP_MAX	45	// MODS module temperature limit (C)
#define ISL_V_TOLER	5.0	// MODS supply voltage tolerance (%)

// Weather type of hardware

#define UNKNOWN     0x1 // Unknown Sensor for now.

// Data paths

#define ISL_ROOT      "/home"
#define ISL_PATH      "/home/isl"
#define MODS_ROOT     "/home"
#define MODS_PATH     "/home/dts/mods"
#define MLC_DATA      "/home/dts/mods/data/shrmem.dat"
#define AGWFW_DATA    "/home/dts/mods/data/agwfilt.dat"
#define RCAMFW_DATA   "/home/dts/mods/data/rcamfilt.dat"
#define BCAMFW_DATA   "/home/dts/mods/data/bcamfilt.dat"
#define RGRAT_DATA    "/home/dts/mods/data/rgrating.dat"
#define BGRAT_DATA    "/home/dts/mods/data/bgrating.dat"
#define MASKS_DATA    "/home/dts/mods/data/slitmasks.dat"
#define BARCODES_DATA "/home/dts/mods/data/barcodes.dat" // retired

// Public instrument configuration tables (filters, gratings, slits, etc.)
// MODS is the "active" config, MODS1/MODS2 are instance-specific

#define MODS_CONFIG   "/home/dts/mods/Config/MODS"
#define MODS1_CONFIG  "/home/dts/mods/Config/MODS1"
#define MODS2_CONFIG  "/home/dts/mods/Config/MODS2"

#define CL_PRIOR      -8
#define MOD1_PRIOR   -12
#define MOD2_PRIOR   -16
#define MAX_RXGAIN 20
#define MAX_FLUX   50

// WAGO quad cell system parameters

#define MAX_QC  2  // maximum number of IMCS quad cell systems per MODS
