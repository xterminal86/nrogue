#ifndef RECT_H
#define RECT_H

#include <vector>

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
    Rect(const Position& p1, const Position& p2);
    Rect(int x, int y, int w, int h);

    Position Dimensions();

    std::vector<Position> GetBoundaryElements(bool excludeCorners = false);

    std::pair<int, int> Center();

    bool Intersects(const Rect& other);

    void Print();

    void PrintToLog();

    int X1;
    int Y1;
    int X2;
    int Y2;
};

#endif
