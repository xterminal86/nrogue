#ifndef INVENTORYSTATE_H
#define INVENTORYSTATE_H

#include "gamestate.h"
#include "player.h"

class InventoryState : public GameState
{
  public:    
    void Init() override;
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;
    void Prepare() override;

    const int kEquipmentMaxNameLength = 10;
    const int kInventoryMaxNameLength = 20;

  private:
    Player* _playerRef;

    void PrintFooter();
    void DestroyInventoryItem();
    void DropItem();
    void DisplayEquipment();
    void DrawEquipmentField(int x, int y, std::string fieldName, ItemComponent* eq);

    int _selectedIndex;
};

#endif // INVENTORYSTATE_H
