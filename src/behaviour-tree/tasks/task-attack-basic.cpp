#include "task-attack-basic.h"

#include "application.h"
#include "game-object.h"
#include "logger.h"

TaskAttackBasic::TaskAttackBasic(GameObject* objectToControl,
                                bool alwaysHitOverride)
  : Node(objectToControl)
{
  _alwaysHitOverride = alwaysHitOverride;
}

BTResult TaskAttackBasic::Run()
{
  //DebugLog("[TaskAttackBasic]\n");

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

  int dmg = 0;

  if (Util::Rolld100(hitChance))
  {
    dmg = _objectToControl->Attrs.Str.Get() - playerRef.Attrs.Def.Get();
    if (dmg <= 0)
    {
      dmg = 1;
    }

    Application::Instance().DisplayAttack(&playerRef,
                                          GlobalConstants::DisplayAttackDelayMs,
                                          "",
                                          Colors::RedColor);

    playerRef.ReceiveDamage(_objectToControl, dmg, false);

    result = true;
  }
  else
  {
    result = false;

    auto msg = Util::StringFormat("%s missed", _objectToControl->ObjectName.data());
    Application::Instance().DisplayAttack(&playerRef,
                                          GlobalConstants::DisplayAttackDelayMs,
                                          msg,
                                          Colors::WhiteColor);
  }

  _objectToControl->FinishTurn();

  ItemComponent* armor = playerRef.Equipment->EquipmentByCategory[EquipmentCategory::TORSO][0];

  _attackResult.Success = result;
  _attackResult.HasArmor = (armor != nullptr);
  _attackResult.DamageDone = dmg;

  return BTResult::Success;
}
