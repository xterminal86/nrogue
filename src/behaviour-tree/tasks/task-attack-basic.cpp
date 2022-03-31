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

  auto& playerRef = Application::Instance().PlayerInstance;

  ItemComponent* weapon = nullptr;

  EquipmentComponent* ec = _objectToControl->GetComponent<EquipmentComponent>();
  if (ec != nullptr)
  {
    weapon = ec->EquipmentByCategory[EquipmentCategory::WEAPON][0];
  }

  int hitChance = Util::CalculateHitChance(_objectToControl, &playerRef);

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
    dmg = Util::CalculateDamageValue(_objectToControl,
                                     &playerRef,
                                     weapon,
                                     false);

    Application::Instance().DisplayAttack(&playerRef,
                                          GlobalConstants::DisplayAttackDelayMs,
                                          "",
                                          Colors::RedColor);

    playerRef.ReceiveDamage(_objectToControl, dmg, false);

    Util::TryToDamageEquipment(_objectToControl, weapon, -1);

    result = true;
  }
  else
  {
    result = false;

    auto msg = Util::StringFormat("%s missed", Util::GetGameObjectDisplayCharacter(_objectToControl).data());
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
