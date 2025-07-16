#ifndef STRCONV_H
#define STRCONV_H
#include <string>
#include <sstream>
//
// StrConv.h - application header
//

/*!
  \file StrConv.h
  \brief Application Header

  Function templates to convert to and from strings.

  \date 2005 May 05 [rdg]
*/

// 
 
template<typename T> T fromString(const std::string& s) {
  std::istringstream is(s);
  T t;
  is >> t;
  return t;
}
 
template<typename T> std::string toString(const T& t) {
  std::ostringstream s;
  s << t;
  return s.str();
}
#endif // STRCONV_H 
