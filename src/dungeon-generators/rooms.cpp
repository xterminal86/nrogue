#include "rooms.h"

void Rooms::Generate(Position mapSize, int minRoomSize)
{
  /*
  _mapSize = mapSize;

  _map = CreateFilledMap(mapSize.X, mapSize.Y);

  int depth = std::max(mapSize.X, mapSize.Y);

  BSPNode root;

  root.CornerStart.Set(0, 0);
  root.CornerEnd.Set(mapSize.X - 1, mapSize.Y - 1);
  root.Depth = depth;

  int r = RNG::Instance().RandomRange(10, 90);
  int splitX = RNG::Instance().RandomRange(0, 2);

  Subdivide(root, r, (splitX == 0), depth);

  FillMapRaw();
  */
}

void Rooms::Subdivide(BSPNode& parent, int ratio, bool splitX, int& currentDepth)
{
  if (currentDepth < 3)
  {
    return;
  }

  int w = parent.CornerEnd.X - parent.CornerStart.X;
  int h = parent.CornerEnd.Y - parent.CornerStart.Y;

  int addX = (((float)ratio / 100.0f) * (float)w);
  int addY = (((float)ratio / 100.0f) * (float)h);

  int sx = parent.CornerStart.X;
  int sy = parent.CornerStart.Y;

  int ex = parent.CornerEnd.X;
  int ey = parent.CornerEnd.Y;

  int nsx = splitX ? sx + addX : sx;
  int nsy = !splitX ? sy + addY : sy;

  int nex = splitX ? sx + addX : ex;
  int ney = !splitX ? sy + addY : ey;

  Position c1Start(sx, sy);
  Position c1End(nex, ney);

  auto leftLeaf = std::make_unique<BSPNode>();

  leftLeaf->CornerStart = c1Start;
  leftLeaf->CornerEnd = c1End;

  int lW = c1End.X - c1Start.X;
  int lH = c1End.Y - c1Start.Y;

  leftLeaf->Depth = std::max(lW, lH);

  parent.Left = std::move(leftLeaf);

  Position c2Start(nsx, nsy);
  Position c2End(nex, ney);

  auto rightLeaf = std::make_unique<BSPNode>();

  rightLeaf->CornerStart = c2Start;
  rightLeaf->CornerEnd = c2End;

  int rW = c2End.X - c2Start.X;
  int rH = c2End.Y - c2Start.Y;

  rightLeaf->Depth = std::max(rW, rH);

  parent.Right = std::move(rightLeaf);

  int r = RNG::Instance().RandomRange(10, 90);
  int newSplitX = RNG::Instance().RandomRange(0, 2);

  Subdivide(*parent.Left.get(), r, (newSplitX == 0), currentDepth);
  Subdivide(*parent.Right.get(), r, (newSplitX == 0), currentDepth);
}

