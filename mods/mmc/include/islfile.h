#ifndef ISLFILE_H
#define ISLFILE_H

//
// islfile.h - Test for errors conditions
//

/*!
  \file islfile.h
  \brief ISL Application Header

  header for the isl application.

  \date 2005 May 05 [rdg]
*/

#include <cstdio>
#include <cstdlib>
#include <fstream>

inline void islfile(bool islfilereq,
  const char* msg = "Islfile failed") {
  using namespace std;
  if(!islfilereq) {
    fputs(msg, stderr);
    fputs("\n", stderr);
    exit(EXIT_FAILURE);
  }
}

inline void islfileArgs(int argc, int args,
  const char* msg = "Use %d arguments") {
  using namespace std;
  if(argc != args + 1) {
    fprintf(stderr, msg, args);
    fputs("\n", stderr);
    exit(EXIT_FAILURE);
  }
}

inline void islfileMinArgs(int argc, int minArgs,
  const char* msg = "Use at least %d arguments") {
  using namespace std;
  if(argc < minArgs + 1) {
    fprintf(stderr, msg, minArgs);
    fputs("\n", stderr);
    exit(EXIT_FAILURE);
  }
}

inline void mksure(std::ifstream& in,
  const char* filename = "") {
  using namespace std;
  if(!in) {
    fprintf(stderr, "Could not open file %s\n", filename);
    exit(EXIT_FAILURE);
  }
}

inline void mksure(std::ofstream& in,
  const char* filename = "") {
  using namespace std;
  if(!in) {
    fprintf(stderr, "Could not open file %s\n", filename);
    exit(EXIT_FAILURE);
  }
}

inline void mksure(std::fstream& in,
  const char* filename = "") {
  using namespace std;  if(!in) {
    fprintf(stderr, "Could not open file %s\n", filename);
    exit(EXIT_FAILURE);
  }
}
#endif // ISLFILE_H
