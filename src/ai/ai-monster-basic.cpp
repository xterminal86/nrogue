#include "ai-monster-basic.h"
#include "ai-chase-player-state.h"
#include "ai-wander-state.h"
#include "ai-component.h"
#include "application.h"
#include "map.h"
#include "rng.h"
#include "map-level-base.h"

AIMonsterBasic::AIMonsterBasic()
{
  _hash = typeid(*this).hash_code();

  IsAgressive = true;
}

void AIMonsterBasic::Init()
{
  ChangeAIState<AIWanderState>();
}

void AIMonsterBasic::Update()
{
  AIModelBase::Update();

  if (CurrentStateIs<AIWanderState>())
  {
    if (IsPlayerVisible() && IsPlayerInRange(AgroRadius))
    {
      ChangeAIState<AIChasePlayerState>();
    }
  }
  else if (CurrentStateIs<AIChasePlayerState>())
  {
    // Player can be attacked
    if (IsPlayerVisible() && IsPlayerInRange())
    {
      if (!_playerRef->IsDestroyed)
      {
        Attack(_playerRef);
        AIComponentRef->OwnerGameObject->FinishTurn();
      }
    }
  }
}

std::vector<Position> AIMonsterBasic::SelectCellNearestToPlayer()
{
  std::vector<Position> res;

  Position c;

  int px = _playerRef->PosX;
  int py = _playerRef->PosY;

  int x = AIComponentRef->OwnerGameObject->PosX;
  int y = AIComponentRef->OwnerGameObject->PosY;

  int lx = x - 1;
  int ly = y - 1;
  int hx = x + 1;
  int hy = y + 1;

  int minD = AgroRadius + 1;

  bool cellFound = false;

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
          cellFound = true;
          minD = d;
          c.Set(cell->PosX, cell->PosY);
        }
      }
    }
  }

  if (cellFound)
  {
    res.push_back(c);
  }

  return res;
}

bool AIMonsterBasic::Attack(Player* player)
{  
  bool result = false;

  int attackChanceScale = 2;
  int defaultHitChance = 50;
  int hitChance = defaultHitChance;

  int d = AIComponentRef->OwnerGameObject->Attrs.Skl.Get() - player->Attrs.Skl.Get();

  hitChance += (d * attackChanceScale);

  hitChance = Util::Clamp(hitChance, GlobalConstants::MinHitChance, GlobalConstants::MaxHitChance);

  auto logMsg = Util::StringFormat("%s (SKL %i, LVL %i) attacks Player (SKL: %i, LVL %i): chance = %i",
                                   AIComponentRef->OwnerGameObject->ObjectName.data(),
                                   AIComponentRef->OwnerGameObject->Attrs.Skl.Get(),
                                   AIComponentRef->OwnerGameObject->Attrs.Lvl.Get(),
                                   player->Attrs.Skl.Get(),
                                   player->Attrs.Lvl.Get(),
                                   hitChance);
  Logger::Instance().Print(logMsg);

  if (Util::Rolld100(hitChance))
  {
    result = true;

    int dmg = AIComponentRef->OwnerGameObject->Attrs.Str.Get() - player->Attrs.Def.Get();
    if (dmg <= 0)
    {
      dmg = 1;
    }

    Application::Instance().DisplayAttack(player, GlobalConstants::DisplayAttackDelayMs, "", "#FF0000");

    player->ReceiveDamage(AIComponentRef->OwnerGameObject, dmg, false);

    if (!player->IsAlive(AIComponentRef->OwnerGameObject))
    {
      Application::Instance().ChangeState(GameStates::ENDGAME_STATE);
    }
  }
  else
  {
    result = false;

    auto msg = Util::StringFormat("%s missed", AIComponentRef->OwnerGameObject->ObjectName.data());
    Application::Instance().DisplayAttack(player, GlobalConstants::DisplayAttackDelayMs, msg, "#FFFFFF");
  }

  return result;
}
