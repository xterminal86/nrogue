#ifndef AIWANDERSTATE_H
#define AIWANDERSTATE_H

#include "ai-state-base.h"

class AIWanderState : public AIStateBase
{
  using AIStateBase::AIStateBase;

  public:
    void Run() override;
    void Exit() override;
    void Enter() override;
};

#endif // AIWANDERSTATE_H
