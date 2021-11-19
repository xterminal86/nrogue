#ifndef AIMONSTERSHELOB_H
#define AIMONSTERSHELOB_H

#include "ai-model-base.h"

class AIMonsterShelob : public AIModelBase
{
  public:
    AIMonsterShelob();

  protected:
    void PrepareScript() override;
};

#endif // AIMONSTERSHELOB_H
