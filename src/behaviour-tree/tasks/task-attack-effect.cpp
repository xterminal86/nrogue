#include "task-attack-effect.h"

#include "player.h"

TaskAttackEffect::TaskAttackEffect(GameObject* objectToControl,
                                   const ItemBonusStruct& e,
                                   bool ignoreArmor)
  : TaskAttackBasic(objectToControl)
{
  _effectToApply = e;
  _effectToApply.Id = objectToControl->ObjectId();
  _shouldIgnoreArmor = ignoreArmor;
}

BTResult TaskAttackEffect::Run()
{
  TaskAttackBasic::Run();

  //DebugLog("\t[TaskAttackEffect]\n");

  // If basic attack didn't miss
  if (_attackResult.Success)
  {
    if (_shouldIgnoreArmor || !_attackResult.HasArmor)
    {
      _playerRef->AddEffect(_effectToApply);
    }
  }

  return BTResult::Success;
}
