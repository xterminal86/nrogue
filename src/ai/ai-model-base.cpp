#include "ai-model-base.h"
#include "ai-component.h"
#include "application.h"
#include "map.h"
#include "util.h"

AIModelBase::AIModelBase()
{
  _playerRef = &Application::Instance().PlayerInstance;
}

void AIModelBase::Update()
{
  if (_currentState != nullptr)
  {
    _currentState->Run();
  }
}

bool AIModelBase::IsPlayerVisible()
{
  int px = _playerRef->PosX;
  int py = _playerRef->PosY;
  int x = AIComponentRef->OwnerGameObject->PosX;
  int y = AIComponentRef->OwnerGameObject->PosY;

  if (!IsPlayerInRange(AgroRadius))
  {
    return false;
  }
  else
  {
    auto line = Util::BresenhamLine(x, y, px, py);
    for (auto& c : line)
    {
      auto& cell = Map::Instance().CurrentLevel->MapArray[c.X][c.Y];
      if (cell->Blocking || cell->BlocksSight)
      {
        return false;
      }
    }

    return true;
  }
}

bool AIModelBase::IsPlayerInRange(int range)
{
  int px = _playerRef->PosX;
  int py = _playerRef->PosY;

  int x = AIComponentRef->OwnerGameObject->PosX;
  int y = AIComponentRef->OwnerGameObject->PosY;

  int lx = x - range;
  int ly = y - range;
  int hx = x + range;
  int hy = y + range;

  return (px >= lx && px <= hx
       && py >= ly && py <= hy);
}

bool AIModelBase::RandomMovement()
{
  int dx = RNG::Instance().Random() % 2;
  int dy = RNG::Instance().Random() % 2;

  int signX = (RNG::Instance().Random() % 2) == 0 ? -1 : 1;
  int signY = (RNG::Instance().Random() % 2) == 0 ? -1 : 1;

  dx *= signX;
  dy *= signY;

  return AIComponentRef->OwnerGameObject->Move(dx, dy);
}
