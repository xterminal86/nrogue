#ifndef AIMONSTERSMART_H
#define AIMONSTERSMART_H

#include "ai-model-base.h"

/// Variation of basic AI that follows last seen position of player
/// when he's no longer visible.
class AIMonsterSmart : public AIModelBase
{
  public:
    AIMonsterSmart();

  protected:
    void PrepareScript() override;
};

#endif // AIMONSTERSMART_H
