#ifndef AIMONSTERTROLL_H
#define AIMONSTERTROLL_H

#include "ai-model-base.h"

class AIMonsterTroll : public AIModelBase
{
  public:
    AIMonsterTroll();

  protected:
    void PrepareScript() override;
};

#endif // AIMONSTERTROLL_H
