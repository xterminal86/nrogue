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
      uint32_t Color = Colors::WhiteColor;
      ItemComponent* ItemComponentRef = nullptr;
      int ServiceCost = 0;
    };

    //
    // Must be std::map so that service items are sorted by alphabet.
    //
    std::map<char, ServiceInfo> _serviceInfoByChar;

    int GetValidBonusesCount(ItemComponent* ic);

    void BlessItem(const ServiceInfo& si);

    const std::unordered_map<ServiceType, std::string> _displayOnEmptyItems =
    {
      { ServiceType::NONE,     "!!! INVALID TYPE !!!" },
      { ServiceType::IDENTIFY, "Nothing to identify"  },
      { ServiceType::RECHARGE, "Nothing to recharge"  },
      { ServiceType::BLESS,    "Nothing to bless"     },
      { ServiceType::REPAIR,   "Nothing to repair"    }
    };

    const std::unordered_map<ServiceType, std::string> _serviceNameByType =
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
      ItemBonusType::TELEPATHY,
      ItemBonusType::TRUE_SEEING
    };

    size_t _maxStrLen = 0;
};

#endif // SERVICESTATE_H
