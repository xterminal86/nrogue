#include "ai-model-base.h"
#include "ai-component.h"
#include "application.h"
#include "map.h"
#include "util.h"

AIModelBase::AIModelBase()
{
  _playerRef = &Application::Instance().PlayerInstance;
}

bool AIModelBase::IsPlayerVisible()
{
  int px = _playerRef->PosX;
  int py = _playerRef->PosY;
  int x = AIComponentRef->OwnerGameObject->PosX;
  int y = AIComponentRef->OwnerGameObject->PosY;

  int d = Util::LinearDistance(x, y, px, py);
  if (d > AgroRadius)
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

bool AIModelBase::IsPlayerInRange()
{
  int px = _playerRef->PosX;
  int py = _playerRef->PosY;

  int x = AIComponentRef->OwnerGameObject->PosX;
  int y = AIComponentRef->OwnerGameObject->PosY;

  int lx = x - 1;
  int ly = y - 1;
  int hx = x + 1;
  int hy = y + 1;

  for (int i = lx; i <= hx; i++)
  {
    for (int j = ly; j <= hy; j++)
    {
      if (px == i && py == j)
      {
        return true;
      }
    }
  }

  return false;
}
