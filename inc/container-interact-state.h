#ifndef CONTAINERINTERACTSTATE_H
#define CONTAINERINTERACTSTATE_H

#include "gamestate.h"

class ContainerComponent;
class Player;

class ContainerInteractState : public GameState
{
  public:
    void Init() override;
    void Cleanup() override;
    void Prepare() override;
    void Update(bool forceUpdate = false) override;
    void HandleInput() override;

  private:
    Player* _playerRef;

    int _inventoryItemIndex = 0;
    bool _playerSide = true;

    void DisplayPlayerInventory();
    void DisplayContainerInventory();

    void CheckIndexLimits();
    void TryToTransferItem();
};

#endif // CONTAINERINTERACTSTATE_H
