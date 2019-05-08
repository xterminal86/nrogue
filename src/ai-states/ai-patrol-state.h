#ifndef AIPATROLSTATE_H
#define AIPATROLSTATE_H

#include "ai-state-base.h"

class AIPatrolState : public AIStateBase
{
  using AIStateBase::AIStateBase;

  public:
    void Run() override;
    void Exit() override;
    void Enter() override;
};

#endif // AIPATROLSTATE_H
