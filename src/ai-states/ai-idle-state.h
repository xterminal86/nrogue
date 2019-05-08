#ifndef AI_IDLE_STATE_H
#define AI_IDLE_STATE_H

#include "ai-state-base.h"

class AIIdleState : public AIStateBase
{
  using AIStateBase::AIStateBase;

  public:    
    void Run() override;
    void Exit() override;
    void Enter() override;
};

#endif
