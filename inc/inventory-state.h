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

  private:
    Player* _playerRef;

    void PrintFooter();
    void DestroyItem();
    void DropItem();

    int _selectedIndex;
};

#endif // INVENTORYSTATE_H
