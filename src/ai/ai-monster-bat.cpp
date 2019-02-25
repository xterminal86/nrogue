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

      int shouldChangePos = RNG::Instance().RandomRange(0, 2);

      if (shouldChangePos && AIComponentRef->OwnerGameObject->Attrs.ActionMeter > GlobalConstants::TurnReadyValue)
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
      if (_directionToMove.X == 0 && _directionToMove.Y == 0)
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

    if (Util::BlockDistance(c.X, c.Y, px, py) == 1)
    {
      validCells.push_back(c);
    }
  }

  int index = RNG::Instance().RandomRange(0, validCells.size());
  Position p = validCells[index];

  AIComponentRef->OwnerGameObject->MoveTo(p.X, p.Y);
  AIComponentRef->OwnerGameObject->FinishTurn();
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
  if (!AIComponentRef->OwnerGameObject->Move(_directionToMove.X, _directionToMove.Y))
  {
    GetNewDirection();
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

    int dx = posX - p.X;
    int dy = posY - p.Y;

    res.push_back({ dx, dy });
  }

  return res;
}

void AIMonsterBat::GetNewDirection()
{
  auto res = GetRandomDirectionToMove();
  if (res.size() != 0)
  {
    _directionToMove.Set(res[0].X, res[0].Y);
  }
}
