#ifndef TASKATTACKBASIC_H
#define TASKATTACKBASIC_H

#include "behaviour-tree.h"

class TaskAttackBasic : public Node
{
  using Node::Node;

  public:
    bool Run() override
    {
      bool result = false;

      int attackChanceScale = 2;
      int defaultHitChance = 50;
      int hitChance = defaultHitChance;

      int d = _objectToControl->Attrs.Skl.Get() - _playerRef->Attrs.Skl.Get();

      hitChance += (d * attackChanceScale);

      hitChance = Util::Clamp(hitChance, GlobalConstants::MinHitChance, GlobalConstants::MaxHitChance);

      auto logMsg = Util::StringFormat("%s (SKL %i, LVL %i) attacks Player (SKL: %i, LVL %i): chance = %i",
                                       _objectToControl->ObjectName.data(),
                                       _objectToControl->Attrs.Skl.Get(),
                                       _objectToControl->Attrs.Lvl.Get(),
                                       _playerRef->Attrs.Skl.Get(),
                                       _playerRef->Attrs.Lvl.Get(),
                                       hitChance);
      Logger::Instance().Print(logMsg);

      if (Util::Rolld100(hitChance))
      {
        result = true;

        int dmg = _objectToControl->Attrs.Str.Get() - _playerRef->Attrs.Def.Get();
        if (dmg <= 0)
        {
          dmg = 1;
        }

        Application::Instance().DisplayAttack(_playerRef, GlobalConstants::DisplayAttackDelayMs, "", "#FF0000");

        _playerRef->ReceiveDamage(_objectToControl, dmg, false);

        if (!_playerRef->IsAlive())
        {
          Application::Instance().ChangeState(GameStates::ENDGAME_STATE);
          return false;
        }
      }
      else
      {
        result = false;

        auto msg = Util::StringFormat("%s missed", _objectToControl->ObjectName.data());
        Application::Instance().DisplayAttack(_playerRef, GlobalConstants::DisplayAttackDelayMs, msg, "#FFFFFF");
      }

      _objectToControl->FinishTurn();

      ItemComponent* armor = _playerRef->EquipmentByCategory[EquipmentCategory::TORSO][0];

      _attackResultPlayerHasArmor.first = result;
      _attackResultPlayerHasArmor.second = (armor != nullptr);

      return true;
    }

  protected:
    // first means successful hit, second means if player has armor
    std::pair<bool, bool> _attackResultPlayerHasArmor;
};

#endif // TASKATTACKBASIC_H
