#ifndef TASKATTACKEFFECT_H
#define TASKATTACKEFFECT_H

#include "task-attack-basic.h"

class TaskAttackEffect : public TaskAttackBasic
{
  public:
    TaskAttackEffect(GameObject* objectToControl,
                     const std::map<Effect, int>& effectsChanceMap)
      : TaskAttackBasic(objectToControl)
    {
      _effectsChanceMap = effectsChanceMap;
    }

    bool Run() override
    {
      TaskAttackBasic::Run();

      // Player was hit
      if (_attackResultPlayerHasArmor.first)
      {
        for (auto& kvp : _effectsChanceMap)
        {
          // If application should ignore armor
          // or player has no armor.
          if (kvp.first.ApplicationIgnoresArmor
          || !_attackResultPlayerHasArmor.second)
          {
            if (Util::Rolld100(kvp.second))
            {
              _playerRef->AddEffect(kvp.first);
            }
          }
        }
      }

      return true;
    }

  private:
    std::map<Effect, int> _effectsChanceMap;
};

#endif // TASKATTACKEFFECT_H
