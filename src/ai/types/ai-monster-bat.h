#ifndef AIMONSTERBAT_H
#define AIMONSTERBAT_H

#include "ai-monster-basic.h"

class AIMonsterBat : public AIMonsterBasic
{
  public:
    AIMonsterBat();

  protected:
    void PrepareScript() override;
};

#endif // AIMONSTERBAT_H
