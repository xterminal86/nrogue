#ifndef TASKATTACKEFFECT_H
#define TASKATTACKEFFECT_H

#include "task-attack-basic.h"

#include "effect.h"

class TaskAttackEffect : public TaskAttackBasic
{
  public:
    TaskAttackEffect(GameObject* objectToControl, const Effect& e, bool ignoreArmor);

    BTResult Run() override;

  private:
    Effect _effectToApply;
    bool _shouldIgnoreArmor;
};

#endif // TASKATTACKEFFECT_H
