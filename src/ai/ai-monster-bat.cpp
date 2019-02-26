#include "ai-monster-bat.h"
#include "ai-component.h"
#include "game-object.h"
#include "map.h"
#include "player.h"

void AIMonsterBat::Update()
{
  if (AIComponentRef->OwnerGameObject->Attrs.ActionMeter < GlobalConstants::TurnReadyValue)
  {
    AIComponentRef->OwnerGameObject->WaitForTurn();
  }
  else
  {
    if (IsPlayerInRange() && !_playerRef->IsDestroyed)
    {
      Attack(_playerRef);

      auto shouldChangePos = Util::WeightedRandom(_probabilityMap);
      if (shouldChangePos.first && AIComponentRef->OwnerGameObject->Attrs.ActionMeter > GlobalConstants::TurnReadyValue)
      {
        CircleAroundPlayer();
      }
    }
    else if (IsPlayerVisible())
    {
      MoveToKill();
    }
    else
    {
      if (_directionToMove.size() == 0)
      {
        GetNewDirection();
      }
      else
      {
        MoveInDirection();
      }
    }

    AIComponentRef->OwnerGameObject->FinishTurn();
  }
}

/// Try to find empty cells around player to move to
void AIMonsterBat::CircleAroundPlayer()
{
  std::vector<Position> validCells;

  int goX = AIComponentRef->OwnerGameObject->PosX;
  int goY = AIComponentRef->OwnerGameObject->PosY;

  int px = _playerRef->PosX;
  int py = _playerRef->PosY;

  auto cells = Map::Instance().GetUnoccupiedCellsAround({ px, py });
  for (auto& c : cells)
  {
    if (c.X == goX && c.Y == goY)
    {
      continue;
    }

    // Select only those cells that are also near this actor
    if (Util::BlockDistance(c.X, c.Y, goX, goY) == 1)
    {
      validCells.push_back(c);
    }
  }

  if (validCells.size() != 0)
  {
    int index = RNG::Instance().RandomRange(0, validCells.size());
    Position p = validCells[index];

    AIComponentRef->OwnerGameObject->MoveTo(p.X, p.Y);
    AIComponentRef->OwnerGameObject->FinishTurn();
  }
  else
  {
    Attack(_playerRef);
    AIComponentRef->OwnerGameObject->FinishTurn();
  }
}

void AIMonsterBat::MoveToKill()
{
  auto c = SelectCellNearestToPlayer();
  if (c.size() != 0)
  {
    bool res = AIComponentRef->OwnerGameObject->MoveTo(c[0].X, c[0].Y);
    if (!res)
    {
      GetNewDirection();
    }    
  }
  else
  {
    GetNewDirection();
  }
}

void AIMonsterBat::MoveInDirection()
{
  if (!AIComponentRef->OwnerGameObject->Move(_directionToMove[0].X, _directionToMove[0].Y))
  {
    _directionToMove.clear();
  }
}

std::vector<Position> AIMonsterBat::GetRandomDirectionToMove()
{
  std::vector<Position> res;

  int posX = AIComponentRef->OwnerGameObject->PosX;
  int posY = AIComponentRef->OwnerGameObject->PosY;

  auto cellsAround = Map::Instance().GetUnoccupiedCellsAround({ posX, posY });
  if (cellsAround.size() != 0)
  {
    int index = RNG::Instance().RandomRange(0, cellsAround.size());
    Position p = cellsAround[index];

    int dx = p.X - posX;
    int dy = p.Y - posY;

    res.push_back({ dx, dy });
  }

  return res;
}

void AIMonsterBat::GetNewDirection()
{  
  auto res = GetRandomDirectionToMove();
  if (res.size() != 0)
  {
    Position dir = res[0];
    _directionToMove.push_back(dir);
  }
}
