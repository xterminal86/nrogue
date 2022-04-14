#ifndef REPAIRSTATE_H
#define REPAIRSTATE_H

#include "select-item-state-base.h"

class Player;
class ItemComponent;

class RepairState : public SelectItemStateBase
{
  public:
    void Init() override;
    void Cleanup() override;
    void Prepare() override;

    void SetRepairKitRef(ItemComponent* item, int inventoryIndex);

  protected:
    void DrawSpecific() final;
    void ProcessInput() final;

  private:
    Player* _playerRef;

    ItemComponent* _repairKit;
    int _inventoryIndex = 0;

    std::map<char, ItemComponent*> _itemRefByChar;

    void RepairItem(ItemComponent* itemToRepair);
};


#endif // REPAIRSTATE_H
