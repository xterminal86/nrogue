#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <cstdio>
#include <vector>
#include <utility>
#include <memory>

#include <math.h>

#include "colorpair.h"

struct Position
{
  Position() {}
  Position(int x, int y)
  {
    X = x;
    Y = y;
  }

  int X;
  int Y;
};

namespace Util
{
  template<typename ... Args>
  inline std::string StringFormat(const std::string& format, Args ... args )
  {
    size_t size = snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    std::unique_ptr<char[]> buf( new char[ size ] ); 
    snprintf( buf.get(), size, format.c_str(), args ... );
    return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
  }

  /// Bresenham line including end point
  inline std::vector<Position> BresenhamLine(int sx, int sy, int ex, int ey)
  {
    int x1 = sx;
    int y1 = sy;
    int x2 = ex;
    int y2 = ey;
    
    std::vector<Position> result;

    // Bresenham's line algorithm
    const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));

    if(steep)
    {
      std::swap(x1, y1);
      std::swap(x2, y2);
    }
   
    if(x1 > x2)
    {
      std::swap(x1, x2);
      std::swap(y1, y2);
    }
   
    const float dx = x2 - x1;
    const float dy = fabs(y2 - y1);
   
    float error = dx / 2.0f;
    const int ystep = (y1 < y2) ? 1 : -1;
    int y = (int)y1;
   
    const int maxX = (int)x2;
   
    Position pos;

    for(int x = (int)x1; x < maxX; x++)
    {
      if(steep)
      {        
        pos.X = y;
        pos.Y = x;        
      }
      else
      {
        pos.X = x;
        pos.Y = y;        
      }

      result.push_back(pos);
   
      error -= dy;
      if(error < 0)
      {
        y += ystep;
        error += dx;
      }
    }

    if(steep)
    {        
      pos.X = y2;
      pos.Y = x2;        
    }
    else
    {
      pos.X = x2;
      pos.Y = y2;        
    }

    result.push_back(pos);

    // Ensure that line always goes from source point
    bool cond = ( (sx > ex && sy == ey) 
               || (sx == ex && sy > ey)
               || (sx > ex && sy > ey) );
    
    if (cond)
    {    
      std::reverse(result.begin(), result.end());   
    } 
  
    return result;
  }

  inline std::vector<Position> BresenhamLine(Position& start, Position& end)
  {
    return BresenhamLine(start.X, start.Y, end.X, end.Y);
  }  

  inline int Clamp(int value, int min, int max)
  {
    return std::max(min, std::min(value, max));
  }
  
  inline float LinearDistance(int x1, int y1, int x2, int y2)
  {
    float d = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
    return d;
  }
  
  inline float LinearDistance(Position& s, Position& e)
  {
    return LinearDistance(s.X, s.Y, e.X, e.Y);
  }
}

#endif
