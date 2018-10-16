#ifndef INVENTORYSTATE_H
#define INVENTORYSTATE_H

#include "gamestate.h"

class InventoryState : public GameState
{
  public:    
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;
    void Prepare() override;

  private:
    void PrintFooter();

    int _selectedIndex;
};

#endif // INVENTORYSTATE_H
