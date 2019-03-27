#ifndef TARGETSTATE_H
#define TARGETSTATE_H

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
    void HandleInput() override;
    void Update(bool forceUpdate) override;
    void Setup(ItemComponent* weapon);

  private:
    Player* _playerRef;
    ItemComponent* _weaponRef;

    Position _cursorPosition;

    int CalculateHitChance();

    void DrawHint();
    void DrawCursor();
    void MoveCursor(int dx, int dy);
    void FireWeapon();    
    void FindTargets();
    void CycleTargets();
    void ProcessHit(GameObject* hitPoint);
    void DrawExplosion(Position pos);
    std::vector<Position> GetVisiblePointsFrom(Position from, int range);

    GameObject* LaunchProjectile(char image, const std::string& color);
    GameObject* CheckHit(const Position& at, const Position& prev);

    int CalculateChance(const Position& startPoint, const Position& endPoint, int baseChance);

    std::vector<GameObject*> _targets;

    int _lastTargetIndex = -1;

    bool _drawHint = false;
};

#endif // TARGETSTATE_H
