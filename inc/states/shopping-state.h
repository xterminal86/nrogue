#ifndef SHOPPINGSTATE_H
#define SHOPPINGSTATE_H

#include "gamestate.h"

#include "constants.h"
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
    const int _kPlayerSellRate = 4;

    Player* _playerRef;
    TraderComponent* _shopOwner;

    std::string _shopTitle;

    int _inventoryItemIndex = 0;
    bool _playerSide = true;

    void DisplayPlayerInventory();
    void DisplayShopInventory();
    void CheckIndexLimits();
    void BuyOrSellItem();
    void CheckSide();

    int GetItemStringTotalLen(std::vector<std::unique_ptr<GameObject>>& container);

    std::string GetItemExtraInfo(ItemComponent* item);
    std::string GetItemTextColor(ItemComponent* item);
};

#endif // SHOPPINGSTATE_H
