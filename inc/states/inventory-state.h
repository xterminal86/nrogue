#ifndef INVENTORYSTATE_H
#define INVENTORYSTATE_H

#include <string>

#include "gamestate.h"

class Player;
class ItemComponent;

class InventoryState : public GameState
{
  public:    
    void Init() override;
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;
    void Prepare() override;

    const int kEquipmentMaxNameLength = 10;

  private:
    Player* _playerRef;

    void PrintFooter();
    void DestroyInventoryItem();
    void DropItem();
    void DisplayEquipment();
    void DrawEquipmentField(int x, int y, std::string fieldName, ItemComponent* eq);
    void DrawSelectionBar(int yOffset, std::string& text, std::string& textColor);

    int _selectedIndex;
};

#endif // INVENTORYSTATE_H
