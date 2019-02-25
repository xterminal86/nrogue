#ifndef AIMONSTERBASIC_H
#define AIMONSTERBASIC_H

#include "ai-model-base.h"
#include "util.h"

class Player;

/// Rudimentary AI mainly for testing purposes
class AIMonsterBasic : public AIModelBase
{
  public:
    AIMonsterBasic();

    void Update() override;

  protected:
    void RandomMovement();
    void MoveToKill();

    bool Attack(Player* player);

    std::vector<Position> SelectCellNearestToPlayer();
};

#endif // AIMONSTERBASIC_H
