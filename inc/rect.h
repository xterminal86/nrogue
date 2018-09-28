#ifndef RECT_H
#define RECT_H

#include <utility>

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