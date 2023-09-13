#include "rooms.h"

#include "rng.h"

///
/// \brief Place rooms of various sizes and then connect them. Nethack-style.
///
/// \param { MapWidth, MapHeight }
/// \param { MinRoomWidth, MaxRoomHeight }
/// \param Attempts to generate.
///
void Rooms::Generate(const Position& mapSize,
                     const Position& roomSizes,
                     int maxIterations)
{
  _mapSize = mapSize;

  _map = CreateFilledMap(mapSize.X, mapSize.Y);

  int x = RNG::Instance().RandomRange(1, _mapSize.X - 1);
  int y = RNG::Instance().RandomRange(1, _mapSize.Y - 1);

  int w = RNG::Instance().RandomRange(roomSizes.X, roomSizes.Y + 1) - 1;
  int h = RNG::Instance().RandomRange(roomSizes.X, roomSizes.Y + 1) - 1;

  FillMapChunk(x, y, w, h, '.');
  VisitArea(x - 1, y - 1, w + 2, h + 2);

  auto AreaOK = [&](int x, int y, int w, int h)
  {
    int hx = x + w;
    int hy = y + h;

    if (hx > _mapSize.X - 2 || hy > _mapSize.Y - 2)
    {
      return false;
    }

    for (int j = x; j <= hx; j++)
    {
      for (int k = y; k <= hy; k++)
      {
        if (_map[j][k].Visited)
        {
          return false;
        }
      }
    }

    return true;
  };

  for (int i = 0; i < maxIterations; i++)
  {
    x = RNG::Instance().RandomRange(1, _mapSize.X - 1);
    y = RNG::Instance().RandomRange(1, _mapSize.Y - 1);

    w = RNG::Instance().RandomRange(roomSizes.X, roomSizes.Y + 1) - 1;
    h = RNG::Instance().RandomRange(roomSizes.X, roomSizes.Y + 1) - 1;

    if (AreaOK(x, y, w, h))
    {
      FillMapChunk(x, y, w, h, '.');
      VisitArea(x - 1, y - 1, w + 2, h + 2);
    }
  }

  ConnectIsolatedAreas();
  PlaceDoors(true);
  FillMapRaw();
}
