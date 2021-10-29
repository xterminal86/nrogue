#ifndef POTENTIALFIELD_H
#define POTENTIALFIELD_H

#include <vector>
#include <queue>
#include <string>
#include <limits>

#include "position.h"

class GameObject;

class PotentialField
{
  public:
    struct Cell
    {
      Position MapPos;
      Position FieldPos;
      bool Visited = false;
      int Cost = 0;
    };

    void Init(GameObject* owner, int fieldRadius);
    void Emanate();
    void SetDirty();

    bool IsDirty();

    std::string GetFieldString();

    Cell* GetCell(int mapX, int mapY);

    Position FieldToMapCoords(const Position& fieldIndices);
    Position MapToFieldCoords(const Position& mapPos);

    static const int kBlockedCellCost = std::numeric_limits<int>::max();

  private:
    std::vector<std::vector<Cell>> _field;

    int _fieldRadius;

    bool _isDirty = true;

    Position _fieldOrigin;

    GameObject* _owner;

    void LookAround(const Position& mapPos, std::queue<Position>& cellsToVisit);

    bool IsOutOfBounds(const Position& fieldPos);
};

#endif // DIJKSTRAMAP_H
