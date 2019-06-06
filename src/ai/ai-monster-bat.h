#ifndef AIMONSTERBAT_H
#define AIMONSTERBAT_H

#include <map>

#include "ai-monster-basic.h"

class AIMonsterBat : public AIMonsterBasic
{
  public:
    AIMonsterBat();

    void ConstructAI() override;
};

#endif // AIMONSTERBAT_H
