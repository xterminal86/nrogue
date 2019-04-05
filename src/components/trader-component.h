#ifndef TRADERCOMPONENT_H
#define TRADERCOMPONENT_H

#include "component.h"
#include "constants.h"

#include "game-object.h"

class AINPC;

class TraderComponent : public Component
{
  public:
    TraderComponent();

    void Update() override;

    void Init(TraderRole traderType, int stockRefreshTurns, int maxItems);
    void RefreshStock();

    std::vector<std::unique_ptr<GameObject>> Items;

    std::string ShopTitle;

    AINPC* NpcRef = nullptr;

    TraderRole Type();

  private:    
    int _maxItems = 0;
    int _itemsToCreate = 0;
    int _stockResetCounter = 0;
    int _stockRefreshTurns = 0;

    TraderRole _traderType = TraderRole::NONE;

    void CreateItems();

    void CreateClericItems();
    void CreateCookItems();
    void CreateJunkerItems();
    void CreateBlacksmithItems();
};

#endif // TRADERCOMPONENT_H
