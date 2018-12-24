#ifndef ROOMS_H
#define ROOMS_H

#include "dg-base.h"

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
    void Subdivide(BSPNode& parent, int ratio, bool splitX, int& currentDepth);
};

#endif // ROOMS_H
