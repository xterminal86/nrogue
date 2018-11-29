#ifndef SHOPPINGSTATE_H
#define SHOPPINGSTATE_H

#include "gamestate.h"

#include "constants.h"

class TraderComponent;
class Player;

class ShoppingState : public GameState
{
  public:
    void Init() override;
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

    void GetBonusStringMaxLen(int& maxLen);
};

#endif // SHOPPINGSTATE_H
