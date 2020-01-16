#include "task-attack-basic.h"

#include "application.h"
#include "game-object.h"
#include "logger.h"

BTResult TaskAttackBasic::Run()
{
  //printf("[TaskAttackBasic]\n");

  bool result = false;

  int attackChanceScale = 2;
  int defaultHitChance = 50;
  int hitChance = defaultHitChance;

  auto& playerRef = Application::Instance().PlayerInstance;

  int d = _objectToControl->Attrs.Skl.Get() - playerRef.Attrs.Skl.Get();

  hitChance += (d * attackChanceScale);

  hitChance = Util::Clamp(hitChance, GlobalConstants::MinHitChance, GlobalConstants::MaxHitChance);

  auto logMsg = Util::StringFormat("%s (SKL %i, LVL %i) attacks Player (SKL: %i, LVL %i): chance = %i",
                                   _objectToControl->ObjectName.data(),
                                   _objectToControl->Attrs.Skl.Get(),
                                   _objectToControl->Attrs.Lvl.Get(),
                                   playerRef.Attrs.Skl.Get(),
                                   playerRef.Attrs.Lvl.Get(),
                                   hitChance);
  Logger::Instance().Print(logMsg);

  if (Util::Rolld100(hitChance))
  {
    int dmg = _objectToControl->Attrs.Str.Get() - playerRef.Attrs.Def.Get();
    if (dmg <= 0)
    {
      dmg = 1;
    }

    Application::Instance().DisplayAttack(&playerRef, GlobalConstants::DisplayAttackDelayMs, "", "#FF0000");

    playerRef.ReceiveDamage(_objectToControl, dmg, false);

    result = true;    
  }
  else
  {
    result = false;

    auto msg = Util::StringFormat("%s missed", _objectToControl->ObjectName.data());
    Application::Instance().DisplayAttack(&playerRef, GlobalConstants::DisplayAttackDelayMs, msg, "#FFFFFF");
  }

  _objectToControl->FinishTurn();

  ItemComponent* armor = playerRef.EquipmentByCategory[EquipmentCategory::TORSO][0];

  _attackResultPlayerHasArmor.first = result;
  _attackResultPlayerHasArmor.second = (armor != nullptr);

  return BTResult::Success;
}
