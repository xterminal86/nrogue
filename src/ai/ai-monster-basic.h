#ifndef AIMONSTERBASIC_H
#define AIMONSTERBASIC_H

#include <vector>

#include "ai-model-base.h"
#include "position.h"

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
