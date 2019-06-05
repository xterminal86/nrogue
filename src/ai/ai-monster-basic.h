#ifndef AIMONSTERBASIC_H
#define AIMONSTERBASIC_H

#include <vector>

#include "ai-model-base.h"
#include "position.h"

class Player;

/// Rudimentary AI mainly for testing purposes
class AIMonsterBasic : public AIModelBase
{
  public:
    AIMonsterBasic();

    void ConstructAI() override;
};

#endif // AIMONSTERBASIC_H
