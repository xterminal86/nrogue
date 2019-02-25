#ifndef AIMONSTERBAT_H
#define AIMONSTERBAT_H

#include "ai-monster-basic.h"

class AIMonsterBat : public AIMonsterBasic
{
  public:
    void Update() override;

  private:
    Position _directionToMove;

    void GetNewDirection();
    std::vector<Position> GetRandomDirectionToMove();

    void MoveInDirection();
    void MoveToKill();
    void CircleAroundPlayer();
};

#endif // AIMONSTERBAT_H
