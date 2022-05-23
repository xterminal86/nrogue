#ifndef TARGETSTATE_H
#define TARGETSTATE_H

#include <memory>
#include <queue>

#include "gamestate.h"
#include "position.h"

class Player;
class ItemComponent;
class GameObject;

class TargetState : public GameState
{
  public:
    void Init() override;
    void Prepare() override;
    void Cleanup() override;
    void HandleInput() override;
    void Update(bool forceUpdate) override;
    void Setup(ItemComponent* weapon);
    void SetupForThrowing(ItemComponent* itemToThrow, int inventoryIndex);

  private:
    Player* _playerRef = nullptr;
    ItemComponent* _weaponRef = nullptr;
    int _throwingItemInventoryIndex = -1;

    Position _cursorPosition;
    Position _lastCursorPosition;

    bool SafetyCheck();
    void CheckCursorPositionBounds();

    void DrawHint();
    void DrawCursor();
    void MoveCursor(int dx, int dy);
    void FireWeapon(bool throwingFromInventory = false);
    void FindTargets();
    void CycleTargets();
    void ProcessHit(GameObject* hitPoint);
    void ProcessHitInventoryThrownItem(GameObject* hitPoint);
    void PrintThrowResult(GameObject* tileRef);
    void DirtyHack();
    void UpdatePlayerPossibleKnockbackDir();

    GameObject* LaunchProjectile(char image, const std::string& color);
    GameObject* CheckHit(const Position& at, const Position& prev);

    std::vector<GameObject*> _targets;

    size_t _lastTargetIndex = -1;
    int _maxThrowingRange = 1;

    bool _drawHint = false;
};

#endif // TARGETSTATE_H
