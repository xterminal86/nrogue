#ifndef AIMONSTERMADMINER_H
#define AIMONSTERMADMINER_H

#include "ai-model-base.h"

class AIMonsterMadMiner : public AIModelBase
{
  public:
    AIMonsterMadMiner();

  protected:
    void PrepareScript() override;
};

#endif // AIMONSTERMADMINER_H
