#ifndef DIJKSTRAMAP_H
#define DIJKSTRAMAP_H

#include <vector>
#include <string>

#include "position.h"

class GameObject;

class DijkstraMap
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

    std::vector<Cell> GetCell(int mapX, int mapY);

    Position FieldToMapCoords(const Position& fieldIndices);
    Position MapToFieldCoords(const Position& mapPos);

  private:
    std::vector<std::vector<Cell>> _field;

    int _fieldRadius;

    bool _isDirty = false;

    Position _fieldOrigin;

    GameObject* _owner;

    const int _blockedCellCost = 500;

    void LookAround(const Position& mapPos, std::vector<Position>& cellsToVisit);

    bool IsOutOfBounds(const Position& fieldPos);
};

#endif // DIJKSTRAMAP_H