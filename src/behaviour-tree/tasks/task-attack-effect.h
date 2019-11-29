#ifndef TASKATTACKEFFECT_H
#define TASKATTACKEFFECT_H

#include "task-attack-basic.h"

class TaskAttackEffect : public TaskAttackBasic
{
  public:
    TaskAttackEffect(GameObject* objectToControl, const ItemBonusStruct& e, bool ignoreArmor);

    BTResult Run() override;

  private:
    ItemBonusStruct _effectToApply;
    bool _shouldIgnoreArmor;
};

#endif // TASKATTACKEFFECT_H
