#ifndef SERVICESTATE_H
#define SERVICESTATE_H

#include "select-item-state-base.h"

class Player;
class TraderComponent;
class ItemComponent;

class ServiceState : public SelectItemStateBase
{
  public:
    void Prepare() override;
    void Setup(TraderComponent* shopOwner);

  protected:
    void ProcessInput() final;
    void DrawSpecific() final;

  private:
    Player* _playerRef = nullptr;
    TraderComponent* _shopOwner = nullptr;

    void FillItemsForRepair();
    void FillItemsForIdentify();
    void FillItemsForBlessing();

    void DisplayItems();

    void ProcessItem(int key);

    void ProcessRepair(int key);
    void ProcessIdentify(int key);
    void ProcessBlessing(int key);

    struct ServiceInfo
    {
      std::string Letter;
      std::string NameToDisplay;
      std::string Color = "#FFFFFF";
      ItemComponent* ItemComponentRef;
      int ServiceCost;
    };

    std::map<char, ServiceInfo> _serviceInfoByChar;

    int GetValidBonusesCount(ItemComponent* ic);

    void BlessItem(const ServiceInfo& si);

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

    // These bonuses cannot be "improved" by blessing
    const std::vector<ItemBonusType> _invalidBonusTypes =
    {
      ItemBonusType::INDESTRUCTIBLE,
      ItemBonusType::VISIBILITY,
      ItemBonusType::REMOVE_HUNGER,
      ItemBonusType::POISON_IMMUNE,
      ItemBonusType::FREE_ACTION,
      ItemBonusType::IGNORE_DEFENCE,
      ItemBonusType::IGNORE_ARMOR,
      ItemBonusType::MANA_SHIELD,
      ItemBonusType::REFLECT,
      ItemBonusType::TELEPATHY
    };

    size_t _maxStrLen;
};

#endif // SERVICESTATE_H
