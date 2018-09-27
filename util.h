#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <cstdio>

#include "colorpair.h"

namespace Util
{
  template<typename ... Args>
  std::string StringFormat(const std::string& format, Args ... args )
  {
    size_t size = snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    std::unique_ptr<char[]> buf( new char[ size ] ); 
    snprintf( buf.get(), size, format.c_str(), args ... );
    return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
  }
}

#endif