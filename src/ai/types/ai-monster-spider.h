#ifndef AIMONSTERSPIDER_H
#define AIMONSTERSPIDER_H

#include "ai-model-base.h"

class AIMonsterSpider : public AIModelBase
{
  public:
    AIMonsterSpider();

  protected:
    void PrepareScript() override;
};

#endif // AIMONSTERSPIDER_H
