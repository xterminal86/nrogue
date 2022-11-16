#include "cellular-automata.h"

#include "util.h"

/// \brief Use cellular automata to generate cave-like area.
void CellularAutomata::Generate(const Position& mapSize,
                                int initialWallChance,
                                int birthThreshold,
                                int deathThreshold,
                                int maxIterations)
{
  _mapSize = mapSize;

  _map = CreateRandomlyFilledMap(mapSize.X, mapSize.Y, initialWallChance);

  //
  // If we change data in-place we would mix old and
  // new results, so we should perform check on initial map
  // and store results in temporary one and copy them after
  // whole initial map was scanned.
  //
  auto tmp = CreateEmptyMap(mapSize.X, mapSize.Y);

  for (int i = 0; i < maxIterations; i++)
  {
    // copy original map contents to temp
    for (int x = 0; x < mapSize.X; x++)
    {
      for (int y = 0; y < mapSize.Y; y++)
      {
        tmp[x][y].Image = _map[x][y].Image;
      }
    }

    for (int x = 0; x < mapSize.X; x++)
    {
      for (int y = 0; y < mapSize.Y; y++)
      {
        if (_map[x][y].Visited)
        {
          continue;
        }

        int aliveCells = CountAround(x, y, '.');
        if (_map[x][y].Image == '.')
        {
          tmp[x][y].Image = (aliveCells < deathThreshold) ? '#' : '.';
        }
        else if (_map[x][y].Image == '#')
        {
          tmp[x][y].Image = (aliveCells > birthThreshold) ? '.' : '#';
        }
      }
    }

    // copy temp array to original map
    for (int x = 0; x < mapSize.X; x++)
    {
      for (int y = 0; y < mapSize.Y; y++)
      {
        _map[x][y].Image = tmp[x][y].Image;
      }
    }
  }

  ConnectIsolatedAreas();
  CutProblemCorners();
  FillMapRaw();
}
