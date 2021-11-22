#include "task-apply-effect.h"

#include "player.h"

TaskApplyEffect::TaskApplyEffect(GameObject* objectToControl,
                                 const ItemBonusStruct& effectToApply)
  : Node(objectToControl)
{
  _effectToApply = effectToApply;
}

BTResult TaskApplyEffect::Run()
{
  _playerRef->AddEffect(_effectToApply);

  return BTResult::Success;
}
