#include "ai-monster-basic.h"
#include "ai-component.h"
#include "application.h"
#include "map.h"
#include "rng.h"
#include "map-level-base.h"

AIMonsterBasic::AIMonsterBasic()
{
  _hash = typeid(*this).hash_code();

  _playerRef = &Application::Instance().PlayerInstance;

  IsAgressive = true;
}

void AIMonsterBasic::Update()
{
  if (AIComponentRef->OwnerGameObject->Attrs.ActionMeter < 100)
  {
    AIComponentRef->OwnerGameObject->WaitForTurn();
  }
  else
  {
    if (IsPlayerInRange() && !_playerRef->IsDestroyed)
    {
      Attack(_playerRef);
    }
    else
    {
      if (IsPlayerVisible() && !_playerRef->IsDestroyed)
      {
        MoveToKill();
      }
      else
      {
        RandomMovement();
      }
    }

    AIComponentRef->OwnerGameObject->FinishTurn();
  }
}

void AIMonsterBasic::MoveToKill()
{
  auto c = SelectCell();
  if (c.X != -1 && c.Y != -1)
  {
    bool res = AIComponentRef->OwnerGameObject->MoveTo(c.X, c.Y);
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

  AIComponentRef->OwnerGameObject->Move(dx, dy);
}

bool AIMonsterBasic::IsPlayerVisible()
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

Position AIMonsterBasic::SelectCell()
{
  Position c(-1, -1);

  int px = _playerRef->PosX;
  int py = _playerRef->PosY;

  int x = AIComponentRef->OwnerGameObject->PosX;
  int y = AIComponentRef->OwnerGameObject->PosY;

  int lx = x - 1;
  int ly = y - 1;
  int hx = x + 1;
  int hy = y + 1;

  int minD = AgroRadius + 1;

  for (int i = lx; i <= hx; i++)
  {
    for (int j = ly; j <= hy; j++)
    {
      auto cell = Map::Instance().CurrentLevel->MapArray[i][j].get();
      if (!cell->Blocking)
      {
        int d = Util::BlockDistance(px, py, cell->PosX, cell->PosY);
        if (d < minD)
        {
          minD = d;
          c.Set(cell->PosX, cell->PosY);
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

void AIMonsterBasic::Attack(Player* player)
{  
  int defaultHitChance = 50;
  int hitChance = defaultHitChance;

  int d = AIComponentRef->OwnerGameObject->Attrs.Skl.CurrentValue - player->Attrs.Skl.Get();

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
                                   AIComponentRef->OwnerGameObject->ObjectName.data(),
                                   AIComponentRef->OwnerGameObject->Attrs.Skl.CurrentValue,
                                   AIComponentRef->OwnerGameObject->Attrs.Lvl.CurrentValue,
                                   player->Attrs.Skl.CurrentValue,
                                   player->Attrs.Lvl.CurrentValue,
                                   hitChance);
  Logger::Instance().Print(logMsg);

  if (Util::RollDice(hitChance))
  {
    Application::Instance().DisplayAttack(player, GlobalConstants::DisplayAttackDelayMs, "#FF0000");

    int dmg = AIComponentRef->OwnerGameObject->Attrs.Str.CurrentValue - player->Attrs.Def.Get();
    if (dmg <= 0)
    {
      dmg = 1;
    }

    player->ReceiveDamage(AIComponentRef->OwnerGameObject, dmg);

    Application::Instance().DrawCurrentState();
    Application::Instance().DisplayAttack(player, GlobalConstants::DisplayAttackDelayMs);

    if (!player->IsAlive(AIComponentRef->OwnerGameObject))
    {
      Application::Instance().ChangeState(GameStates::ENDGAME_STATE);
    }
  }
  else
  {
    Application::Instance().DisplayAttack(player, GlobalConstants::DisplayAttackDelayMs, "#FFFFFF");

    auto str = Util::StringFormat("%s misses", AIComponentRef->OwnerGameObject->ObjectName.data());
    Printer::Instance().AddMessage(str);

    Application::Instance().DrawCurrentState();
    Application::Instance().DisplayAttack(player, GlobalConstants::DisplayAttackDelayMs);
  }
}
