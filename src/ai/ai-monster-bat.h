#ifndef AIMONSTERBAT_H
#define AIMONSTERBAT_H

#include <map>

#include "ai-monster-basic.h"

class AIMonsterBat : public AIMonsterBasic
{
  public:
    void Update() override;

  private:
    std::vector<Position> _directionToMove;

    void GetNewDirection();
    std::vector<Position> GetRandomDirectionToMove();

    void MoveInDirection();
    void MoveToKill();
    void CircleAroundPlayer();

    std::map<int, int> _probabilityMap =
    {
      { 0, 1 },
      { 1, 7 }
    };
};

#endif // AIMONSTERBAT_H
