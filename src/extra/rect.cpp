#include "rect.h"

#include "util.h"
#include "logger.h"

Rect::Rect(const Position &p1, const Position &p2)
{
  X1 = p1.X;
  Y1 = p1.Y;
  X2 = p2.X;
  Y2 = p2.Y;
}

Rect::Rect(int x, int y, int w, int h)
{
  X1 = x;
  Y1 = y;
  X2 = x + w;
  Y2 = y + h;
}

Position Rect::Dimensions()
{
  Position res;

  res.Set(Y2 - Y1, X2 - X1);

  return res;
}

std::vector<Position> Rect::GetBoundaryElements(bool excludeCorners)
{
  std::vector<Position> res;

  for (int x = X1; x <= X2; x++)
  {
    for (int y = Y1; y <= Y2; y++)
    {
      bool condCorners = (x == X1 && y == Y1)
                      || (x == X1 && y == Y2)
                      || (x == X2 && y == Y1)
                      || (x == X2 && y == Y2);

      if (excludeCorners && condCorners)
      {
        continue;
      }

      bool cond = (x == X1 || x == X2 || y == Y1 || y == Y2);
      if (cond)
      {
        res.push_back(Position(x, y));
      }
    }
  }

  return res;
}

std::pair<int, int> Rect::Center()
{
  int cx = (X1 + X2) / 2;
  int cy = (Y1 + Y2) / 2;

  return std::pair<int, int>(cx, cy);
}

bool Rect::Intersects(const Rect &other)
{
  return (X1 <= other.X2 && X2 >= other.X1 &&
          Y1 <= other.Y2 && Y2 >= other.Y1);
}

void Rect::Print()
{
  DebugLog("[%i; %i] -> [%i %i]\n", X1, Y1, X2, Y2);
}

void Rect::LogPrint()
{
  auto str = Util::StringFormat("[%i; %i] -> [%i %i]\n", X1, Y1, X2, Y2);
  Logger::Instance().Print(str);
}

