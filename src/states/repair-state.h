#ifndef REPAIRSTATE_H
#define REPAIRSTATE_H

#include "gamestate.h"

#include "player.h"
#include "util.h"

class RepairState : public GameState
{
  public:
    void Init() override;
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;
    void SetRepairKitRef(ItemComponent* item, int inventoryIndex);
    void Cleanup() override;

  private:
    Player* _playerRef;

    ItemComponent* _repairKit;
    int _inventoryIndex = 0;

    std::map<char, ItemComponent*> _itemRefByChar;

    void RepairItem(ItemComponent* itemToRepair);
};


#endif // REPAIRSTATE_H