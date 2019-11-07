#include "task-attack-effect.h"

#include "player.h"

TaskAttackEffect::TaskAttackEffect(GameObject* objectToControl,
                                   const Effect &e,
                                   bool ignoreArmor)
  : TaskAttackBasic(objectToControl)
{
  _effectToApply = e;
  _shouldIgnoreArmor = ignoreArmor;
}

BTResult TaskAttackEffect::Run()
{
  TaskAttackBasic::Run();

  //printf("\t[TaskAttackEffect]\n");

  // TODO: should 'attack_effect' task have additional parameter
  // to indicate whether an attack should ignore armor?

  // If basic attack didn't miss
  if (_attackResultPlayerHasArmor.first)
  {
    if (_shouldIgnoreArmor || !_attackResultPlayerHasArmor.second)
    {
      _playerRef->AddEffect(_effectToApply);
    }
  }

  return BTResult::Success;
}
