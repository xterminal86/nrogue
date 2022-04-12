#ifndef AIMONSTERKOBOLD_H
#define AIMONSTERKOBOLD_H

#include "ai-model-base.h"

class AIMonsterKobold : public AIModelBase
{
  public:
    AIMonsterKobold();

  protected:
    void PrepareScript() override;
};

#endif // AIMONSTERKOBOLD_H
