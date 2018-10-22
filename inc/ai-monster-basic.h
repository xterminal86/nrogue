#ifndef AIMONSTERBASIC_H
#define AIMONSTERBASIC_H

#include "component.h"
#include "player.h"
#include "util.h"

class AIMonsterBasic : public Component
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
