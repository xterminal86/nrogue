#ifndef TASKPLAYERHASEFFECT_H
#define TASKPLAYERHASEFFECT_H

#include "behaviour-tree.h"

#include "application.h"

class TaskPlayerHasEffect : public Node
{
  public:
    TaskPlayerHasEffect(GameObject* objectToControl,
                        const EffectType& effectToCheck)
      : Node(objectToControl)
    {
      _effectToCheck = effectToCheck;
    }

    bool Run() override
    {
      return _playerRef->HasEffect(_effectToCheck);
    }

  private:
    EffectType _effectToCheck;
};

#endif // TASKPLAYERHASEFFECT_H
