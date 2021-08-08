#include "dijkstra-map.h"

#include "map.h"
#include "game-object.h"
#include "util.h"

void DijkstraMap::Init(GameObject* owner, int fieldRadius)
{
  if (owner == nullptr)
  {
    DebugLog("!!! Trying to add potential field map on nullptr !!!");
    return;
  }

  if (fieldRadius <= 0)
  {
    DebugLog("!!! Wrong field radius = %i !!!", fieldRadius);
    return;
  }

  _field.clear();

  _owner = owner;
  _fieldRadius = fieldRadius;

  _fieldOrigin = { _owner->PosX, _owner->PosY };

  int size = _fieldRadius * 2;
  for (int x = 0; x <= size; x++)
  {
    std::vector<Cell> row;
    for (int y = 0; y <= size; y++)
    {
      Cell c;

      c.MapPos = FieldToMapCoords({ x, y });
      c.FieldPos = { x, y };

      row.push_back(c);
    }

    _field.push_back(row);
  }
}

void DijkstraMap::Emanate()
{
  std::vector<Position> cellsToVisit;

  _fieldOrigin = { _owner->PosX, _owner->PosY };

  cellsToVisit.push_back(_fieldOrigin);

  int size = _fieldRadius * 2;
  for (int x = 0; x <= size; x++)
  {
    for (int y = 0; y <= size; y++)
    {
      _field[x][y].Cost = 0;
      _field[x][y].Visited = false;
    }
  }

  while (!cellsToVisit.empty())
  {
    Position p = cellsToVisit[0];

    LookAround(p, cellsToVisit);

    cellsToVisit.erase(cellsToVisit.begin());
  }

  _isDirty = false;
}

void DijkstraMap::LookAround(const Position& mapPos, std::vector<Position>& cellsToVisit)
{
  auto& curLvl = Map::Instance().CurrentLevel;

  Position fieldPos = MapToFieldCoords(mapPos);

  // Check bounds on point
  if (IsOutOfBounds(fieldPos))
  {
    return;
  }

  // If already visited
  Cell& parent = _field[fieldPos.X][fieldPos.Y];
  if (parent.Visited)
  {
    return;
  }

  // Check bounds on four points around
  std::vector<Position> pointsToCheck =
  {
    { fieldPos.X,     fieldPos.Y - 1 },
    { fieldPos.X,     fieldPos.Y + 1 },
    { fieldPos.X - 1, fieldPos.Y     },
    { fieldPos.X + 1, fieldPos.Y     }
  };

  for (auto& p : pointsToCheck)
  {
    if (!IsOutOfBounds(p))
    {
      Position mapPos = FieldToMapCoords(p);
      bool cellBlocked = curLvl->IsCellBlocking(mapPos);

      Cell& c = _field[p.X][p.Y];

      c.FieldPos = p;
      c.MapPos = mapPos;
      c.Cost = cellBlocked ? _blockedCellCost : (parent.Cost + 1);

      cellsToVisit.push_back(mapPos);
    }
  }

  parent.Visited = true;
}

DijkstraMap::Cell* DijkstraMap::GetCell(int mapX, int mapY)
{
  Cell* res = nullptr;

  if (IsOutOfBounds({ mapX, mapY }))
  {
    DebugLog("!!! Outside field map array %i %i !!!", mapX, mapY);
  }
  else
  {
    res = &_field[mapX][mapY];
  }

  return res;
}

bool DijkstraMap::IsOutOfBounds(const Position& fieldPos)
{
  if (fieldPos.X < 0 || fieldPos.X > _fieldRadius * 2
   || fieldPos.Y < 0 || fieldPos.Y > _fieldRadius * 2)
  {
    return true;
  }

  return false;
}

void DijkstraMap::SetDirty()
{
  _isDirty = true;
}

bool DijkstraMap::IsDirty()
{
  return _isDirty;
}

Position DijkstraMap::MapToFieldCoords(const Position& mapPos)
{
  auto& curLvl = Map::Instance().CurrentLevel;

  Position res;

  if (mapPos.X < 0
   || mapPos.Y < 0
   || mapPos.X > curLvl->MapSize.X - 1
   || mapPos.Y > curLvl->MapSize.Y - 1)
  {
    DebugLog("!!! Wrong map coords: %i %i !!!", mapPos.X, mapPos.Y);
    return res;
  }

  int dx = mapPos.X - _fieldOrigin.X;
  int dy = mapPos.Y - _fieldOrigin.Y;

  int ix = dx + _fieldRadius;
  int iy = dy + _fieldRadius;

  return { ix, iy };
}

Position DijkstraMap::FieldToMapCoords(const Position& fieldIndices)
{
  Position res;

  if (fieldIndices.X < 0
   || fieldIndices.Y < 0
   || fieldIndices.X > _fieldRadius * 2
   || fieldIndices.Y > _fieldRadius * 2)
  {
    DebugLog("!!! Wrong index values: %i %i !!!", fieldIndices.X, fieldIndices.Y);
    return res;
  }

  res.X = _owner->PosX + (fieldIndices.Y - _fieldRadius);
  res.Y = _owner->PosY + (fieldIndices.X - _fieldRadius);

  return res;
}

std::string DijkstraMap::GetFieldString()
{
  std::stringstream ss;

  int size = _fieldRadius * 2;

  for (int x = 0; x <= size; x++)
  {
    for (int y = 0; y <= size; y++)
    {
      Cell& c = _field[x][y];
      ss << "(" << c.MapPos.X << ";" << c.MapPos.Y << ")"
         << " [" << c.Cost << "] ";
    }

    ss << "\n";
  }

  return ss.str();
}
