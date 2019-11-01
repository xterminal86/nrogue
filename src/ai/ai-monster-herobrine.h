#ifndef AIMONSTERHEROBRINE_H
#define AIMONSTERHEROBRINE_H

#include "ai-model-base.h"

class AIMonsterHerobrine : public AIModelBase
{
  public:
    AIMonsterHerobrine();

  protected:
    void PrepareScript() override;
};

#endif // AIMONSTERHEROBRINE_H
