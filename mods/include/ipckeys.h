/* IPC keys parameter header */
#ifndef IPCKEYS_H
#define IPCKEYS_H


#ifdef NO_FTOK_ISL
#define SHM_KEY     1
#define CLS_KEY     2
#define SKD_KEY     3
#define BRK_KEY     4
#define SEM_KEY     5
#define NSEM_KEY    6
#define GO_KEY      7
#else
#define SHM_KEY     ftok("/home2/isl",1)
#define CLS_KEY     ftok("/home2/isl",2)
#define SKD_KEY     ftok("/home2/isl",3)
#define BRK_KEY     ftok("/home2/isl",4)
#define SEM_KEY     ftok("/home2/isl",5)
#define NSEM_KEY    ftok("/home2/isl",6)
#define GO_KEY      ftok("/home2/isl",7)
#endif


#endif // IPCKEYS_H
