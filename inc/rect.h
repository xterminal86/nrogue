#ifndef RECT_H
#define RECT_H

#include <utility>
#include <vector>

#include "util.h"

class Rect
{
public:
  Rect(int x, int y, int w, int h)
  {
    X1 = x;
    Y1 = y;
    X2 = x + w;
    Y2 = y + h;
  }

  std::vector<Position> GetBoundaryElements()
  {
    std::vector<Position> res;

    for (int x = X1; x <= X2; x++)
    {
      for (int y = Y1; y <= Y2; y++)
      {
        bool cond = (x == X1 || x == X2 || y == Y1 || y == Y2);

        if (cond)
        {
          res.push_back(Position(x, y));
        }
      }
    }

    return res;
  }

  std::pair<int, int> Center()
  {
    int cx = (X1 + X2) / 2;
    int cy = (Y1 + Y2) / 2;

    return std::pair<int, int>(cx, cy);
  }

  bool Intersects(const Rect& other)
  {
    return (X1 <= other.X2 && X2 >= other.X1 && 
            Y1 <= other.Y2 && Y2 >= other.Y1);
  }

  int X1;
  int Y1;
  int X2;
  int Y2;
};

#endif
