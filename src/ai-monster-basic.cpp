#include "ai-monster-basic.h"
#include "application.h"
#include "map.h"
#include "rng.h"

AIMonsterBasic::AIMonsterBasic()
{
  _hash = typeid(*this).hash_code();

  _playerRef = &Application::Instance().PlayerInstance;
}

void AIMonsterBasic::Update()
{
  if (((GameObject*)OwnerGameObject)->Attrs.ActionMeter < 100)
  {
    ((GameObject*)OwnerGameObject)->WaitForTurn();
  }
  else
  {
    if (IsPlayerInRange())
    {
      Attack(_playerRef);
    }
    else
    {
      if (IsPlayerVisible())
      {
        MoveToKill();
      }
      else
      {
        RandomMovement();
      }
    }

    ((GameObject*)OwnerGameObject)->FinishTurn();
  }
}

void AIMonsterBasic::MoveToKill()
{
  auto c = SelectCell();
  if (c.X != -1 && c.Y != -1)
  {
    bool res = ((GameObject*)OwnerGameObject)->MoveTo(c.X, c.Y);
    if (!res)
    {
      RandomMovement();
    }
  }
  else
  {
    RandomMovement();
  }
}

void AIMonsterBasic::RandomMovement()
{
  int dx = RNG::Instance().Random() % 2;
  int dy = RNG::Instance().Random() % 2;

  int signX = (RNG::Instance().Random() % 2) == 0 ? -1 : 1;
  int signY = (RNG::Instance().Random() % 2) == 0 ? -1 : 1;

  dx *= signX;
  dy *= signY;

  // See component.h
  ((GameObject*)OwnerGameObject)->Move(dx, dy);
}

bool AIMonsterBasic::IsPlayerVisible()
{
  int px = _playerRef->PosX;
  int py = _playerRef->PosY;
  int x = ((GameObject*)OwnerGameObject)->PosX;
  int y = ((GameObject*)OwnerGameObject)->PosY;

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
      auto& cell = Map::Instance().MapArray[c.X][c.Y];
      if (cell.Blocking || cell.BlocksSight)
      {
        return false;
      }
    }

    return true;
  }
}

Position AIMonsterBasic::SelectCell()
{
  Position c(-1, -1);

  int px = _playerRef->PosX;
  int py = _playerRef->PosY;

  int x = ((GameObject*)OwnerGameObject)->PosX;
  int y = ((GameObject*)OwnerGameObject)->PosY;

  int lx = x - 1;
  int ly = y - 1;
  int hx = x + 1;
  int hy = y + 1;

  int minD = AgroRadius + 1;

  for (int i = lx; i <= hx; i++)
  {
    for (int j = ly; j <= hy; j++)
    {
      auto& cell = Map::Instance().MapArray[i][j];
      if (!cell.Blocking)
      {
        int d = Util::BlockDistance(px, py, cell.PosX, cell.PosY);
        if (d < minD)
        {
          minD = d;
          c.Set(cell.PosX, cell.PosY);
        }
      }
    }
  }

  return c;
}

bool AIMonsterBasic::IsPlayerInRange()
{
  int px = _playerRef->PosX;
  int py = _playerRef->PosY;

  int x = ((GameObject*)OwnerGameObject)->PosX;
  int y = ((GameObject*)OwnerGameObject)->PosY;

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

void AIMonsterBasic::Attack(Player* player)
{
  GameObject* monster = static_cast<GameObject*>(OwnerGameObject);

  int defaultHitChance = 50;
  int hitChance = defaultHitChance;

  int d = monster->Attrs.Skl.CurrentValue - player->Attrs.Skl.CurrentValue;

  if (d > 0)
  {
    hitChance += (d * 5);
  }
  else
  {
    hitChance -= (d * 5);
  }

  hitChance = Util::Clamp(hitChance, GlobalConstants::MinHitChance, GlobalConstants::MaxHitChance);

  auto logMsg = Util::StringFormat("%s (SKL %i, LVL %i) attacks Player (SKL: %i, LVL %i): chance = %i",
                                   monster->ObjectName.data(),
                                   monster->Attrs.Skl.CurrentValue,
                                   monster->Attrs.Lvl.CurrentValue,
                                   player->Attrs.Skl.CurrentValue,
                                   player->Attrs.Lvl.CurrentValue,
                                   hitChance);
  Logger::Instance().Print(logMsg);

  if (Util::RollDice(hitChance))
  {
    int dmg = monster->Attrs.Str.CurrentValue - player->Attrs.Def.CurrentValue;
    if (dmg <= 0)
    {
      dmg = 1;
    }

    player->ReceiveDamage(monster, dmg);
  }
  else
  {
    auto str = Util::StringFormat("%s misses", monster->ObjectName.data());
    Printer::Instance().AddMessage(str);
  }
}
