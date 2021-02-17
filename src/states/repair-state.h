#ifndef REPAIRSTATE_H
#define REPAIRSTATE_H

#include "gamestate.h"

class Player;
class ItemComponent;

class RepairState : public GameState
{
  public:
    void Init() override;
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;
    void Cleanup() override;
    void Prepare() override;

    void SetRepairKitRef(ItemComponent* item, int inventoryIndex);

  private:
    Player* _playerRef;

    ItemComponent* _repairKit;
    int _inventoryIndex = 0;

    std::map<char, ItemComponent*> _itemRefByChar;

    void RepairItem(ItemComponent* itemToRepair);

    const std::string _header = " REPAIR ITEMS ";
};


#endif // REPAIRSTATE_H
