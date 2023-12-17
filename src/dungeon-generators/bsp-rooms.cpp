#include "bsp-rooms.h"

#include "pathfinder.h"

///
/// \brief Use BSP to split area into big "cells"
/// and place randomly sized rooms inside them.
///
void BSPRooms::Generate(const Position& mapSize,
                     const Position& splitRatio,
                     int minRoomSize)
{
  _mapSize = mapSize;

  _minRoomSize = minRoomSize;
  _minRoomArea = minRoomSize * minRoomSize;

  _splitRatio = splitRatio;

  _splitRatio.X = Util::Clamp(_splitRatio.X, 10, 90);
  _splitRatio.Y = Util::Clamp(_splitRatio.Y, 10, 90);

  _map = CreateFilledMap(mapSize.X, mapSize.Y);

  if (minRoomSize <= 1)
  {
    DebugLog("minRoomSize is <= 1, not generating shit!");
    FillMapRaw();
    return;
  }

  Rect area = { 0, 0, mapSize.X, mapSize.Y };
  auto split = GetSplitRatio(area);

  BSPNode root;
  root.CornerStart.Set(0, 0);
  root.CornerEnd.Set(mapSize.X - 1, mapSize.Y - 1);

  Subdivide(root, split.second, split.first);

  int depth = 0;
  Traverse(&root, depth);

  if (!_connectionPoints.empty())
  {
    for (size_t i = 0; i < _connectionPoints.size() - 1; i++)
    {
      ConnectPoints(_connectionPoints[i], _connectionPoints[i + 1]);
    }

    PlaceDoors();
  }

  FillMapRaw();
}

// =============================================================================

void BSPRooms::Subdivide(BSPNode& parent, double ratio, bool splitX)
{
  int sx = parent.CornerStart.X;
  int sy = parent.CornerStart.Y;

  int ex = parent.CornerEnd.X;
  int ey = parent.CornerEnd.Y;

  int w = ey - sy;
  int h = ex - sx;

  Rect leaf1, leaf2;

  if (splitX)
  {
    int add = ratio * h;
    if (add == 0)
    {
      return;
    }

    leaf1 = { { sx, sy }, { sx + add, ey } };
    leaf2 = { { sx + add, sy }, { ex, ey } };

    //leaf1.LogPrint();
    //leaf2.LogPrint();
  }
  else
  {
    int add = ratio * w;
    if (add == 0)
    {
      return;
    }

    leaf1 = { { sx, sy }, { ex, sy + add } };
    leaf2 = { { sx, sy + add }, { ex, ey } };

    //leaf1.LogPrint();
    //leaf2.LogPrint();
  }

  if (!DoesRoomFit(leaf1) && !DoesRoomFit(leaf2))
  {
    return;
  }

  auto splitChance1 = GetSplitRatio(leaf1);
  auto splitChance2 = GetSplitRatio(leaf2);

  BSPNode* left = new BSPNode();

  left->CornerStart = { leaf1.X1, leaf1.Y1 };
  left->CornerEnd   = { leaf1.X2, leaf1.Y2 };

  BSPNode* right = new BSPNode();

  right->CornerStart = { leaf2.X1, leaf2.Y1 };
  right->CornerEnd   = { leaf2.X2, leaf2.Y2 };

  parent.Left.reset(left);
  parent.Right.reset(right);

  Subdivide(*parent.Left.get(), splitChance1.second, splitChance1.first);
  Subdivide(*parent.Right.get(), splitChance2.second, splitChance2.first);
}

// =============================================================================

std::pair<bool, double> BSPRooms::GetSplitRatio(Rect& area)
{
  std::pair<bool, double> res;

  int r = Util::RandomRange(_splitRatio.X, _splitRatio.Y + 1, _rng);

  double ratio = (double)r / 100.0;

  if (area.Dimensions().X >= area.Dimensions().Y)
  {
    res = { false, ratio };
  }
  else
  {
    res = { true, ratio };
  }

  return res;
}

// =============================================================================

void BSPRooms::FillArea(const Rect& area, char ch)
{
  for (int x = area.X1 + 1; x <= area.X2 - 1; x++)
  {
    for (int y = area.Y1 + 1; y <= area.Y2 - 1; y++)
    {
      _map[x][y].Image = ch;
    }
  }
}

// =============================================================================

bool BSPRooms::DoesRoomFit(Rect& area)
{
  int w = area.Dimensions().X;
  int h = area.Dimensions().Y;

  return (w > _minRoomSize) && (h > _minRoomSize);
}

// =============================================================================

bool BSPRooms::WasFilled(const Rect& area)
{
  for (int x = area.X1 + 1; x <= area.X2 - 1; x++)
  {
    for (int y = area.Y1 + 1; y <= area.Y2 - 1; y++)
    {
      if (_map[x][y].Image != '#')
      {
        return true;
      }
    }
  }

  return false;
}

// =============================================================================

void BSPRooms::Traverse(BSPNode* node, int depth)
{
  if (node->Left && node->Right)
  {
    Traverse(node->Left.get(), depth + 1);
    Traverse(node->Right.get(), depth + 1);
  }
  else
  {
    Rect roomArea(node->CornerStart, node->CornerEnd);
    if (DoesRoomFit(roomArea))
    {
      //
      // Randomize room size inside valid cells.
      // Start from 4 because FillArea fills
      // [X + 1, Y + 1] to [X - 1; Y - 1]
      // + 1 at the end because RandomRange traditionally excludes last number.
      //
      int roomW = Util::RandomRange(4, roomArea.Dimensions().X, _rng);
      int roomH = Util::RandomRange(4, roomArea.Dimensions().Y, _rng);

      Position newEnd =
      {
        node->CornerStart.X + roomH,
        node->CornerStart.Y + roomW
      };

      Rect r(node->CornerStart, newEnd);

      FillArea(r);

      int indX = Util::RandomRange(node->CornerStart.X + 1, newEnd.X, _rng);
      int indY = Util::RandomRange(node->CornerStart.Y + 1, newEnd.Y, _rng);

      _connectionPoints.push_back({ indX, indY });
    }
  }
}

// =============================================================================

void BSPRooms::ConnectPoints(const Position& p1, const Position& p2)
{
  int dirX = (p1.X > p2.X) ? -1 : 1;
  int dirY = (p1.Y > p2.Y) ? -1 : 1;

  Position tmp1 = p1;
  Position tmp2 = p2;

  while (tmp1.X != tmp2.X)
  {
    _map[tmp1.X][tmp1.Y].Image = '.';
    tmp1.X += dirX;
  }

  while (tmp1.Y != tmp2.Y)
  {
    _map[tmp1.X][tmp1.Y].Image = '.';
    tmp1.Y += dirY;
  }
}

