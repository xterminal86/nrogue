#ifndef AIMONSTERBASIC_H
#define AIMONSTERBASIC_H

#include "ai-model-base.h"
#include "util.h"

class Player;

class AIMonsterBasic : public AIModelBase
{
  public:
    AIMonsterBasic();

    void Update() override;

    int AgroRadius;    

  private:
    void RandomMovement();
    void MoveToKill();
    void Attack(Player* player);

    Position SelectCell();

    bool IsPlayerVisible();
    bool IsPlayerInRange();

    Player* _playerRef;
};

#endif // AIMONSTERBASIC_H
