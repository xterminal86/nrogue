#ifndef INVENTORYSTATE_H
#define INVENTORYSTATE_H

#include <string>

#include "gamestate.h"

class Player;
class ItemComponent;
class GameObject;

class InventoryState : public GameState
{
  public:
    void Init() override;
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;
    void Prepare() override;

    const int kEquipmentMaxNameLength = 10;

  private:
    Player* _playerRef = nullptr;

    void PrintFooter();
    void DestroyInventoryItem();
    void DropItem(ItemComponent* ic);
    void DisplayEquipment();
    void SortInventory();

    void DrawEquipmentField(int x,
                            int y,
                            const std::string& fieldName,
                            ItemComponent* eq);

    void DrawSelectionBar(int yOffset,
                          const std::string& text,
                          const uint32_t& textColor);

    int _selectedIndex = 0;

    const std::vector<std::string> _footer =
    {
      "'i' - inspect",
      "'e' - equip",
      "'s' - sort",
      "'u' - use",
      "'d' - drop",
      "'t' - throw"
    };
};

#endif // INVENTORYSTATE_H
