#ifndef STRCONV_H
#define STRCONV_H
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>

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

template<typename T> T strToUp(const std::string& s) {
  std::string data = s;
  std::transform(data.begin(),data.end(),data.begin(), ::toupper);
  //  int i;
  //  for(i=0;i<strlen(str);i++)
  //    if(str[i] >= 'a' && str[i] <= 'z')
  //      str[i] = toupper(str[i]);
  //  is >> t;
  //  return t;
}

#endif // STRCONV_H 
