#ifndef AIMONSTERVAMPIREBAT_H
#define AIMONSTERVAMPIREBAT_H

#include "ai-monster-basic.h"

class AIMonsterVampireBat : public AIMonsterBasic
{
  public:
    AIMonsterVampireBat();

  protected:
    void PrepareScript() override;
};

#endif // AIMONSTERVAMPIREBAT_H
