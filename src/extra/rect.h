#ifndef RECT_H
#define RECT_H

#include <vector>
#include <string>

#include "position.h"

//
// Microsoft Visual Studio compiler fails to build
// with this particular forward declaration
// on github actions pipeline.
//
#ifdef MSVC_COMPILER
#include "position.h"
#else
class Position;
#endif

class Rect
{
  public:
    Rect() = default;
    Rect(int x1, int y1, int x2, int y2);
    Rect(const Position& p1, const Position& p2);

    const Position& Dimensions();
    int Perimeter() const;
    int Area() const;
    int Width() const;
    int Height() const;

    std::string ToString() const;

    std::vector<Position> GetBoundaryElements(bool excludeCorners = false);

    std::pair<int, int> Center();

    bool Intersects(const Rect& other);

    void Print();

    void PrintToLog();

    int X1;
    int Y1;
    int X2;
    int Y2;

  private:
    Position _dim;
};

#endif
