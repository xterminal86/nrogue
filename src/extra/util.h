#ifndef UTIL_H
#define UTIL_H

#include <chrono>
#include <string>
#include <cstdio>
#include <vector>
#include <utility>
#include <memory>
#include <algorithm>
#include <climits>

#include <math.h>

#include "colorpair.h"
#include "constants.h"
#include "rng.h"
#include "printer.h"
#include "logger.h"

struct Position
{
  Position()
  {
    X = 0;
    Y = 0;
  }

  Position(int x, int y)
  {
    X = x;
    Y = y;
  }

  Position(const Position& from)
  {
    X = from.X;
    Y = from.Y;
  }

  virtual ~Position()
  {
  }

  void Set(int x, int y)
  {
    X = x;
    Y = y;
  }

  void Set(const Position& from)
  {
    X = from.X;
    Y = from.Y;
  }

  // For use inside std::map as a key
  bool operator< (const Position& rhs) const
  {
    return (X < rhs.X || Y < rhs.Y);
  }

  bool operator== (const Position& rhs) const
  {
    return (X == rhs.X && Y == rhs.Y);
  }

  int X;
  int Y;
};

namespace Util
{
  inline bool CheckLimits(Position posToCheck, Position limits)
  {
    if (posToCheck.X >= 0 && posToCheck.X < limits.X
     && posToCheck.Y >= 0 && posToCheck.Y < limits.Y)
    {
      return true;
    }

    return false;
  }

  template<typename ... Args>
  inline std::string StringFormat(const std::string& format, Args ...args)
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
    
    bool cond = (result[0].X == sx && result[0].Y == sy);
    if (!cond)
    {    
      std::reverse(result.begin(), result.end());   
    } 
    
    // result.erase(result.begin());

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
  
  inline int BlockDistance(int x1, int y1, int x2, int y2)
  {
     return abs(y2 - y1) + abs(x2 - x1);
  }

  inline int BlockDistance(Position from, Position to)
  {
     return abs(to.Y - from.Y) + abs(to.X - from.X);
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
  
  inline std::vector<Position> GetScreenRect(int x1, int y1, int x2, int y2)
  {
    std::vector<Position> result;

    int tw = Printer::Instance().TerminalWidth;
    int th = Printer::Instance().TerminalHeight;

    int lx = x1;
    int ly = y1;
    int hx = x2;
    int hy = y2;

    lx = Clamp(lx, 0, tw - 1);
    ly = Clamp(ly, 0, th - 1);
    hx = Clamp(hx, 0, tw - 1);
    hy = Clamp(hy, 0, th - 1);

    for (int x = lx; x <= hx; x++)
    {
      for (int y = ly; y <= hy; y++)
      {
        Position p(x, y);
        result.push_back(p);
      }
    }

    return result;
  }

  inline std::vector<Position> GetScreenRectPerimeter(int x1, int y1, int x2, int y2, bool includeCorners = true)
  {
    std::vector<Position> res;

    int tw = Printer::Instance().TerminalWidth;
    int th = Printer::Instance().TerminalHeight;

    int lx = x1;
    int ly = y1;
    int hx = x2;
    int hy = y2;

    lx = Clamp(lx, 0, tw - 1);
    ly = Clamp(ly, 0, th - 1);
    hx = Clamp(hx, 0, tw - 1);
    hy = Clamp(hy, 0, th - 1);

    for (int x = lx; x <= hx; x++)
    {
      for (int y = ly; y <= hy; y++)
      {
        bool condCorners = (x == x1 && y == y1)
                        || (x == x1 && y == y2)
                        || (x == x2 && y == y1)
                        || (x == x2 && y == y2);

        if (!includeCorners && condCorners)
        {
          continue;
        }

        bool cond = (x == x1 || x == x2 || y == y1 || y == y2);
        if (cond)
        {
          res.push_back(Position(x, y));
        }
      }
    }

    return res;
  }

  /// Clamps values against terminal size,
  /// useful for drawing GUI related stuff and everything.
  inline std::vector<Position> GetScreenRectAroundPoint(int pointX, int pointY, int rangeX, int rangeY)
  {
    std::vector<Position> result;

    int tw = Printer::Instance().TerminalWidth;
    int th = Printer::Instance().TerminalHeight;

    int lx = pointX - rangeX;
    int ly = pointY - rangeY;
    int hx = pointX + rangeX;
    int hy = pointY + rangeY;

    lx = Clamp(lx, 0, tw - 1);
    ly = Clamp(ly, 0, th - 1);
    hx = Clamp(hx, 0, tw - 1);
    hy = Clamp(hy, 0, th - 1);

    for (int x = lx; x <= hx; x++)
    {
      for (int y = ly; y <= hy; y++)
      {
        Position p(x, y);
        result.push_back(p);
      }
    }

    return result;
  }

  inline std::vector<Position> GetRectAroundPoint(int pointX, int pointY, int rangeX, int rangeY, Position mapSize)
  {    
    std::vector<Position> result;

    int mw = mapSize.X;
    int mh = mapSize.Y;
      
    int lx = pointX - rangeX;
    int ly = pointY - rangeY;
    int hx = pointX + rangeX;
    int hy = pointY + rangeY;
    
    lx = Clamp(lx, 0, mw - 1);
    ly = Clamp(ly, 0, mh - 1);
    hx = Clamp(hx, 0, mw - 1);
    hy = Clamp(hy, 0, mh - 1);
    
    for (int x = lx; x <= hx; x++)
    {
      for (int y = ly; y <= hy; y++)
      {
        Position p(x, y);
        result.push_back(p);
      }
    }
    
    return result;
  }

  inline std::vector<Position> GetPerimeter(int x, int y, int w, int h, bool includeCorners = true)
  {
    std::vector<Position> res;

    int x1 = x;
    int x2 = x + w;
    int y1 = y;
    int y2 = y + h;

    for (int i = x1; i <= x2; i++)
    {
      for (int j = y1; j <= y2; j++)
      {
        bool condCorners = (i == x1 && j == y1)
                        || (i == x1 && j == y2)
                        || (i == x2 && j == y1)
                        || (i == x2 && j == y2);

        if (!includeCorners && condCorners)
        {
          continue;
        }

        bool cond = (i == x1 || i == x2 || j == y1 || j == y2);
        if (cond)
        {
          res.push_back(Position(i, j));
        }
      }
    }

    return res;
  }

  inline std::vector<Position> GetPerimeterAroundPoint(int x, int y, int w, int h, bool includeCorners = true)
  {
    return GetPerimeter(x - w, y - h, w * 2, h * 2, includeCorners);
  }

  inline std::string ChooseRandomName()
  {
    int index = RNG::Instance().Random() % GlobalConstants::RandomNames.size();
    return GlobalConstants::RandomNames[index];
  }

  /// Rotates room text layout, provided all lines are of equal length
  inline std::vector<std::string> RotateRoomLayout(const std::vector<std::string>& layout, RoomLayoutRotation r)
  {
    std::vector<std::string> res;

    switch (r)
    {
      case RoomLayoutRotation::CCW_90:
      {
        // Swap dimensions
        int lineLen = layout[0].length();

        for (int i = 0; i < lineLen; i++)
        {
          res.push_back(std::string(layout.size(), '_'));
        }

        int maxX = res.size() - 1;
        int maxY = res[0].length() - 1;

        // In case of 3x5 size:
        //
        // 0;0 -> 3;0
        // 0;1 -> 2;0
        // 0;2 -> 1;0
        // 0;3 -> 0;0
        //
        // 1;0 -> 3;1
        // 1;1 -> 2;1
        // ...

        for (int x = 0; x < layout.size(); x++)
        {
          for (int y = 0; y < layout[x].length(); y++)
          {
            char c = layout[x][y];

            switch (c)
            {
              case '_':
              case '-':
                c = '|';
                break;

              case '|':
                c = '-';
                break;

              case '/':
                c = '\\';
                break;

              case '\\':
                c = '/';
                break;
            }

            res[maxX - y][x] = c;
          }
        }
      }
      break;

      case RoomLayoutRotation::CCW_180:
      {        
        res = layout;

        int maxY = res[0].length() - 1;

        // Swap columns then rows
        for (int x = 0; x < layout.size(); x++)
        {
          for (int y = 0; y < layout[x].length(); y++)
          {
            res[x][maxY - y] = layout[x][y];
          }
        }

        std::reverse(res.begin(), res.end());        
      }
      break;

      case RoomLayoutRotation::CCW_270:
      {
        // Swap dimensions
        int lineLen = layout[0].length();

        for (int i = 0; i < lineLen; i++)
        {
          res.push_back(std::string(layout.size(), '_'));
        }

        int maxX = res.size() - 1;
        int maxY = res[0].length() - 1;

        // In case of 3x5 size:
        //
        // 0;0 -> 0;5
        // 0;1 -> 1;5
        // 0;2 -> 2;5
        // 0;3 -> 3;5
        //
        // 1;0 -> 0;4
        // 1;1 -> 1;4
        // ...

        for (int x = 0; x < layout.size(); x++)
        {
          for (int y = 0; y < layout[x].length(); y++)
          {
            char c = layout[x][y];

            switch (c)
            {
              case '_':
              case '-':
                c = '|';
                break;

              case '|':
                c = '-';
                break;

              case '/':
                c = '\\';
                break;

              case '\\':
                c = '/';
                break;
            }

            res[y][maxY - x] = c;
          }
        }
      }
      break;

      default:
        res = layout;
        break;
    }

    return res;
  }

  inline bool Rolld100(int successChance)
  {
    int dice = RNG::Instance().RandomRange(1, 101);

    auto logMsg = Util::StringFormat("\tRollDice(bool): chance = %i, rolled = %i",
                                     successChance,
                                     dice);
    Logger::Instance().Print(logMsg);

    if (dice <= successChance)
    {
      return true;
    }

    return false;
  }

  inline int Rolld100()
  {
    int dice = RNG::Instance().RandomRange(1, 101);

    auto str = Util::StringFormat("\tRollDice(void): rolled = %i", dice);
    Logger::Instance().Print(str);

    return dice;
  }

  inline void Sleep(int delayMs)
  {
    using timer = std::chrono::high_resolution_clock;
    using ms = std::chrono::milliseconds;

    auto delay = ms(delayMs);

    auto lastTime = timer::now();

    auto elapsed = timer::now() - lastTime;
    auto msPassed = std::chrono::duration_cast<ms>(elapsed);

    while (msPassed < delay)
    {
      elapsed = timer::now() - lastTime;
      msPassed = std::chrono::duration_cast<ms>(elapsed);
    }
  }

  inline void PrintLayout(std::vector<std::string> l)
  {
    std::string dbg = "\n";

    for (int x = 0; x < l.size(); x++)
    {
      for (int y = 0; y < l[x].length(); y++)
      {
        dbg += l[x][y];
      }

      dbg += "\n";
    }

    Logger::Instance().Print(dbg);
  }

  template <typename T>
  inline std::pair<T, int> WeightedRandom(std::map<T, int> weightsByType)
  {
    int sum = 0;
    for (auto& i : weightsByType)
    {
      sum += i.second;
    }

    int target = RNG::Instance().RandomRange(1, sum + 1);

    for (auto& i : weightsByType)
    {
      if (target <= i.second)
      {
        std::pair<T, int> res = { i.first, i.second };
        return res;
      }

      target -= i.second;
    }

    return *weightsByType.begin();
  }  
}

#endif
