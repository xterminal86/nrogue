#ifndef AIMONSTERSPIDER_H
#define AIMONSTERSPIDER_H

#include "ai-model-base.h"

class AIMonsterSpider : public AIModelBase
{
  public:
    AIMonsterSpider();

    void ConstructAI() override;
};

#endif // AIMONSTERSPIDER_H
