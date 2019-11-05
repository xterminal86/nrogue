#ifndef AIMONSTERBASIC_H
#define AIMONSTERBASIC_H

#include "ai-model-base.h"

/// Rudimentary AI (wander, see player, chase, melee attack)
class AIMonsterBasic : public AIModelBase
{
  public:
    AIMonsterBasic();

  protected:
    void PrepareScript() override;
};

#endif // AIMONSTERBASIC_H
