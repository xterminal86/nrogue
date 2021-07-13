#ifndef SERVICESTATE_H
#define SERVICESTATE_H

#include "gamestate.h"

class Player;
class TraderComponent;

class ServiceState : public GameState
{
  public:
    void Prepare() override;
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;
    void Setup(TraderComponent* shopOwner);

  private:
    Player* _playerRef;
    TraderComponent* _shopOwner;

    const std::map<ServiceType, std::string> _serviceNameByType =
    {
      { ServiceType::NONE,     " !INVALID TYPE! " },
      { ServiceType::IDENTIFY, " IDENTIFY ITEMS " },
      { ServiceType::RECHARGE, " RECHARGE WANDS " },
      { ServiceType::REPAIR,   " REPAIR ITEMS "   }
    };
};

#endif // SERVICESTATE_H
