#ifndef SHOPPINGSTATE_H
#define SHOPPINGSTATE_H

#include "gamestate.h"

#include "game-object.h"

class TraderComponent;
class Player;
class ItemComponent;

class ShoppingState : public GameState
{
  public:
    void Init() override;
    void Prepare() override;
    void PassShopOwner(TraderComponent* tc);
    void Update(bool forceUpdate = false) override;
    void HandleInput() override;

  private:
    const float _kPlayerSellRate = 2.0f;
    const int _kJunkerBuyCost = 500;

    Player* _playerRef = nullptr;
    TraderComponent* _shopOwner = nullptr;

    std::string _shopTitle;

    int _inventoryItemIndex = 0;
    bool _playerSide = true;

    void DisplayPlayerInventory();
    void DisplayShopInventory();
    void CheckIndexLimits();
    void BuyOrSellItem();
    void CheckSide();
    void ShowItemInfo();

    bool CanBeBought(ItemComponent* ic);

    size_t GetItemStringTotalLen(std::vector<std::unique_ptr<GameObject>>& container);

    int GetCost(ItemComponent* ic, bool playerSide);

    std::string GetItemExtraInfo(ItemComponent* item);
};

#endif // SHOPPINGSTATE_H
