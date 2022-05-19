#ifndef AIMONSTERWRAITH_H
#define AIMONSTERWRAITH_H

#include "ai-model-base.h"

class AIMonsterWraith : public AIModelBase
{
  public:
    AIMonsterWraith();

  protected:
    void PrepareScript() override;
};

#endif // AIMONSTERWRAITH_H
