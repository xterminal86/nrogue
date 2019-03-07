#ifndef TARGETSTATE_H
#define TARGETSTATE_H

#include "gamestate.h"
#include "util.h"

class Player;
class ItemComponent;
class GameObject;

class TargetState : public GameState
{
  public:
    void Init() override;
    void Prepare() override;
    void HandleInput() override;
    void Update(bool forceUpdate) override;
    void Setup(ItemComponent* weapon);

  private:
    Player* _playerRef;
    ItemComponent* _weaponRef;

    Position _cursorPosition;

    void DrawCursor();
    void MoveCursor(int dx, int dy);
    void FireWeapon();    
    void FindTargets();
    void CycleTargets();
    void ProcessHit(GameObject* hitPoint);
    void DrawExplosion(Position pos);
    std::vector<Position> GetVisiblePointsFrom(Position from, int range);

    GameObject* LaunchProjectile(char image);

    std::vector<GameObject*> _targets;

    int _lastTargetIndex = -1;
};

#endif // TARGETSTATE_H
