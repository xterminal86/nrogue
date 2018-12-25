#ifndef ROOMS_H
#define ROOMS_H

#include "dg-base.h"

#include "rect.h"

struct BSPNode
{
  Position CornerStart;
  Position CornerEnd;

  int Depth = 0;

  std::unique_ptr<BSPNode> Left;
  std::unique_ptr<BSPNode> Right;
};

class Rooms : public DGBase
{
  public:
    void Generate(Position mapSize, int minRoomSize);

  private:
    int _minRoomSize = 0;

    std::pair<bool, float> GetSplitRatio(Rect area);

    void Subdivide(BSPNode& parent, float ratio, bool splitX);
    void FillArea(Rect area);
};

#endif // ROOMS_H
