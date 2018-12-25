#include "rooms.h"

void Rooms::Generate(Position mapSize, int minRoomSize)
{
  _mapSize = mapSize;

  _minRoomSize = minRoomSize;

  _map = CreateFilledMap(mapSize.X, mapSize.Y);

  Rect area = { 0, 0, mapSize.X, mapSize.Y };
  auto split = GetSplitRatio(area);

  BSPNode root;
  root.CornerStart.Set(0, 0);
  root.CornerEnd.Set(mapSize.X - 1, mapSize.Y - 1);

  Subdivide(root, split.second, split.first);

  FillMapRaw();
}

void Rooms::Subdivide(BSPNode& parent, float ratio, bool splitX)
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

    leaf1 = { { sx, sy }, { sx + add, ey } };
    leaf2 = { { sx + add, sy }, { ex, ey } };

    //leaf1.LogPrint();
    //leaf2.LogPrint();
  }
  else
  {
    int add = ratio * w;

    leaf1 = { { sx, sy }, { ex, sy + add } };
    leaf2 = { { sx, sy + add }, { ex, ey } };

    //leaf1.LogPrint();
    //leaf2.LogPrint();
  }

  if (w < _minRoomSize || h < _minRoomSize)
  {
    return;
  }

  auto splitChance1 = GetSplitRatio(leaf1);
  auto splitChance2 = GetSplitRatio(leaf2);

  BSPNode* left = new BSPNode();

  left->CornerStart = { leaf1.X1, leaf1.Y1 };
  left->CornerEnd = { leaf1.X2, leaf1.Y2 };

  BSPNode* right = new BSPNode();

  right->CornerStart = { leaf2.X1, leaf2.Y1 };
  right->CornerEnd = { leaf2.X2, leaf2.Y2 };

  parent.Left.reset(left);
  parent.Right.reset(right);

  Subdivide(*parent.Left.get(), splitChance1.second, splitChance1.first);
  Subdivide(*parent.Right.get(), splitChance2.second, splitChance2.first);

  Rect area({ sx, sy }, { ex, ey });
  FillArea(area);
}

std::pair<bool, float> Rooms::GetSplitRatio(Rect area)
{
  std::pair<bool, float> res;

  int r = RNG::Instance().RandomRange(10, 91);

  float ratio = (float)r / 100.0f;

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

void Rooms::FillArea(Rect area)
{
  for (int x = area.X1 + 1; x <= area.X2 - 1; x++)
  {
    for (int y = area.Y1 + 1; y <= area.Y2 - 1; y++)
    {
      if (_map[x][y].Image == '.')
      {
        return;
      }
    }
  }

  for (int x = area.X1 + 1; x <= area.X2 - 1; x++)
  {
    for (int y = area.Y1 + 1; y <= area.Y2 - 1; y++)
    {
      _map[x][y].Image = '.';
    }
  }
}
