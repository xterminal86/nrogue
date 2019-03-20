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
    void SetRepairKitRef(ItemComponent* item, int inventoryIndex);
    void Cleanup() override;
    void Prepare() override;

  private:
    Player* _playerRef;

    ItemComponent* _repairKit;
    int _inventoryIndex = 0;

    std::map<char, ItemComponent*> _itemRefByChar;

    void RepairItem(ItemComponent* itemToRepair);
};


#endif // REPAIRSTATE_H
