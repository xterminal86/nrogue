#ifndef TARGETSTATE_H
#define TARGETSTATE_H

#include "gamestate.h"
#include "position.h"

#include <memory>

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
    Player* _playerRef;
    ItemComponent* _weaponRef;
    int _throwingItemInventoryIndex = -1;

    Position _cursorPosition;
    Position _lastCursorPosition;

    int CalculateHitChance();

    bool SafetyCheck();
    void CheckCursorPositionBounds();

    void DrawHint();
    void DrawCursor();
    void MoveCursor(int dx, int dy);
    void FireWeapon(bool throwingFromInventory = false);
    void FindTargets();
    void CycleTargets();
    void ProcessHit(GameObject* hitPoint);

    std::vector<Position> GetVisiblePointsFrom(const Position& from, int range);

    GameObject* LaunchProjectile(char image, const std::string& color);
    GameObject* CheckHit(const Position& at, const Position& prev);

    int CalculateChance(const Position& startPoint, const Position& endPoint, int baseChance);

    std::vector<GameObject*> _targets;

    int _lastTargetIndex = -1;
    int _maxThrowingRange = 1;

    bool _drawHint = false;    
};

#endif // TARGETSTATE_H
