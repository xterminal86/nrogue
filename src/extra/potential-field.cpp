#include "potential-field.h"

#include "map.h"
#include "game-object.h"
#include "util.h"

#include <sstream>

void PotentialField::Init(GameObject* owner, int fieldRadius)
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

  _fieldOrigin = _owner->GetPosition();

  int size = _fieldRadius * 2;

  _field.reserve(size + 1);

  for (int x = 0; x <= size; x++)
  {
    std::vector<Cell> row;

    row.reserve(size + 1);

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

void PotentialField::Emanate()
{
  std::queue<Position> cellsToVisit;

  _fieldOrigin = _owner->GetPosition();

  cellsToVisit.push(_fieldOrigin);

  int size = _fieldRadius * 2;
  for (int x = 0; x <= size; x++)
  {
    for (int y = 0; y <= size; y++)
    {
      _field[x][y].Cost = kBlockedCellCost;
      _field[x][y].Visited = false;
    }
  }

  // Center is on the actor
  _field[_fieldRadius][_fieldRadius].Cost = 0;

  while (!cellsToVisit.empty())
  {
    Position p = cellsToVisit.front();

    LookAround(p, cellsToVisit);

    cellsToVisit.pop();
  }

  _isDirty = false;
}

void PotentialField::LookAround(const Position& mapPos, std::queue<Position>& cellsToVisit)
{
  auto& curLvl = Map::Instance().CurrentLevel;

  Position fieldPos = MapToFieldCoords(mapPos);

  // Check bounds on point
  if (IsOutOfBounds(fieldPos))
  {
    return;
  }

  Cell& parent = _field[fieldPos.X][fieldPos.Y];
  if (parent.Cost == kBlockedCellCost)
  {
    return;
  }

  const std::vector<Position> pointsToCheck =
  {
    { fieldPos.X,     fieldPos.Y - 1 },
    { fieldPos.X,     fieldPos.Y + 1 },
    { fieldPos.X - 1, fieldPos.Y     },
    { fieldPos.X + 1, fieldPos.Y     }
  };

  bool isFlying = _owner->HasEffect(ItemBonusType::LEVITATION);

  auto IsCellBlocked = [this, isFlying, curLvl](const Position& pos)
  {
    if (!Util::Instance().IsInsideMap(pos, curLvl->MapSize))
    {
      return true;
    }

    bool groundBlock = (curLvl->MapArray[pos.X][pos.Y]->Blocking && !isFlying);

    bool staticBlock = false;
    if (curLvl->StaticMapObjects[pos.X][pos.Y] != nullptr)
    {
      staticBlock = curLvl->StaticMapObjects[pos.X][pos.Y]->Blocking;
    }

    return (groundBlock || staticBlock);
  };

  for (auto& p : pointsToCheck)
  {
    if (!IsOutOfBounds(p) && !_field[p.X][p.Y].Visited)
    {
      Position mapPos = FieldToMapCoords(p);

      bool cellBlocked = IsCellBlocked(mapPos);

      Cell& c = _field[p.X][p.Y];

      c.FieldPos = p;
      c.MapPos = mapPos;
      c.Cost = cellBlocked ? kBlockedCellCost : (parent.Cost + 1);
      c.Visited = true;

      cellsToVisit.push(mapPos);
    }
  }

  parent.Visited = true;
}

PotentialField::Cell* PotentialField::GetCell(int mapX, int mapY)
{
  Position p = MapToFieldCoords({ mapX, mapY });
  if (IsOutOfBounds(p))
  {
    return nullptr;
  }

  return &_field[p.X][p.Y];
}

bool PotentialField::IsOutOfBounds(const Position& fieldPos)
{
  const int size = _fieldRadius * 2;

  if (fieldPos.X < 0 || fieldPos.X > size
   || fieldPos.Y < 0 || fieldPos.Y > size)
  {
    return true;
  }

  return false;
}

void PotentialField::SetDirty()
{
  // Probably makes no sense in terms of optimization, but still...
  if (!_isDirty)
  {
    _isDirty = true;
  }
}

bool PotentialField::IsDirty()
{
  return _isDirty;
}

Position PotentialField::MapToFieldCoords(const Position& mapPos)
{
  int dx = mapPos.X - _fieldOrigin.X;
  int dy = mapPos.Y - _fieldOrigin.Y;

  int ix = dx + _fieldRadius;
  int iy = dy + _fieldRadius;

  // Because (2;1) in world coordinates is (1;2) in array coordinates.
  return { iy, ix };
}

Position PotentialField::FieldToMapCoords(const Position& fieldIndices)
{
  Position res;

  res.X = _owner->PosX + (fieldIndices.Y - _fieldRadius);
  res.Y = _owner->PosY + (fieldIndices.X - _fieldRadius);

  return res;
}

std::string PotentialField::GetFieldString()
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
