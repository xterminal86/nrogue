#include "rect.h"

#include "util.h"

#ifdef DEBUG_BUILD
#include "logger.h"
#endif

Rect::Rect(const Position &p1, const Position &p2)
{
  X1 = p1.X;
  Y1 = p1.Y;
  X2 = p2.X;
  Y2 = p2.Y;
}

// =============================================================================

Rect::Rect(int x1, int y1, int x2, int y2)
{
  X1 = x1;
  Y1 = y1;
  X2 = x2;
  Y2 = y2;
}

// =============================================================================

const Position& Rect::Dimensions()
{
  _dim.Set(Y2 - Y1 + 1, X2 - X1 + 1);
  return _dim;
}

// =============================================================================

int Rect::Perimeter()
{
  return 2 * ( Width() + Height() );
}

// =============================================================================

int Rect::Area()
{
  return Width() * Height();
}

// =============================================================================

int Rect::Width()
{
  return (X2 - X1 + 1);
}

int Rect::Height()
{
  return (Y2 - Y1 + 1);
}

// =============================================================================

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

// =============================================================================

std::pair<int, int> Rect::Center()
{
  int cx = (X1 + X2) / 2;
  int cy = (Y1 + Y2) / 2;

  return std::pair<int, int>(cx, cy);
}

// =============================================================================

bool Rect::Intersects(const Rect &other)
{
  return (X1 <= other.X2 && X2 >= other.X1 &&
          Y1 <= other.Y2 && Y2 >= other.Y1);
}

// =============================================================================

void Rect::Print()
{
  DebugLog("[%i; %i] -> [%i %i]\n", X1, Y1, X2, Y2);
}

// =============================================================================

void Rect::PrintToLog()
{
  auto str = Util::StringFormat("[%i; %i] -> [%i %i]\n", X1, Y1, X2, Y2);
  LogPrint(str);
}
