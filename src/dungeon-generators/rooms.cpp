#include "rooms.h"

void Rooms::Generate(Position mapSize, Position roomSize, int maxIterations)
{
  _mapSize = mapSize;

  _map = CreateFilledMap(mapSize.X, mapSize.Y);

  for (int i = 0; i < maxIterations; i++)
  {
    int w = RNG::Instance().RandomRange(roomSize.X, roomSize.Y);
    int h = RNG::Instance().RandomRange(roomSize.X, roomSize.Y);

    int sx = RNG::Instance().RandomRange(1, mapSize.X - 1);
    int sy = RNG::Instance().RandomRange(1, mapSize.Y - 1);

    int ex = sx + w;
    int ey = sy + h;

    if (!IsInsideMap({ ex, ey }) || !IsAreaWalls({ sx, sy }, { ex, ey }))
    {
      continue;
    }

    CreateRoom({ sx, sy }, { ex, ey });
  }

  FillMapRaw();
}

void Rooms::CreateRoom(Position start, Position end)
{
  for (int x = start.X + 1; x <= end.X - 1; x++)
  {
    for (int y = start.Y + 1; y <= end.Y - 1; y++)
    {
      _map[x][y].Image = '.';
    }
  }
}

bool Rooms::IsAreaWalls(Position start, Position end)
{
  for (int x = start.X; x <= end.X; x++)
  {
    for (int y = start.Y; y <= end.Y; y++)
    {
      if (_map[x][y].Image == '.')
      {
        return false;
      }
    }
  }

  return true;
}
