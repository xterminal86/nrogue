#ifndef AIMONSTERBASIC_H
#define AIMONSTERBASIC_H

#include <vector>

#include "ai-model-base.h"
#include "position.h"

class Player;

/// Rudimentary AI (wander, see player, chase, melee attack)
class AIMonsterBasic : public AIModelBase
{
  public:
    AIMonsterBasic();

    void ConstructAI() override;
};

#endif // AIMONSTERBASIC_H
