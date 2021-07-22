#ifndef SERVICESTATE_H
#define SERVICESTATE_H

#include "gamestate.h"

class Player;
class TraderComponent;
class ItemComponent;

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

    void FillItemsForRepair();
    void FillItemsForIdentify();
    void FillItemsForBlessing();

    void DisplayItems();

    void ProcessItem(int key);

    void ProcessRepair(int key);
    void ProcessIdentify(int key);
    void ProcessBlessing(int key);

    const std::map<ServiceType, std::string> _displayOnEmptyItems =
    {
      { ServiceType::NONE,     "!!! INVALID TYPE !!!" },
      { ServiceType::IDENTIFY, "Nothing to identify"  },
      { ServiceType::RECHARGE, "Nothing to recharge"  },
      { ServiceType::BLESS,    "Nothing to bless"     },
      { ServiceType::REPAIR,   "Nothing to repair"    }
    };

    const std::map<ServiceType, std::string> _serviceNameByType =
    {
      { ServiceType::NONE,     " !!! INVALID TYPE !!! " },
      { ServiceType::IDENTIFY, " IDENTIFY ITEMS "       },
      { ServiceType::RECHARGE, " RECHARGE WANDS "       },
      { ServiceType::BLESS,    " BLESS ITEMS "          },
      { ServiceType::REPAIR,   " REPAIR ITEMS "         }
    };

    struct ServiceInfo
    {
      std::string Letter;
      std::string NameToDisplay;
      std::string Color = "#FFFFFF";
      ItemComponent* ItemComponentRef;
      int ServiceCost;
    };

    std::map<char, ServiceInfo> _serviceInfoByChar;

    size_t _maxStrLen;
};

#endif // SERVICESTATE_H
