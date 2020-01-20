#ifndef MAINSTATE_H
#define MAINSTATE_H

#include <vector>
#include <memory>

#include "gamestate.h"
#include "position.h"

class RangedAttribute;
class Player;
class GameObject;
class ItemComponent;

class MainState : public GameState
{
  public:
    void Init() override;
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;    

  private:
    Player* _playerRef;

    std::string _debugInfo;

    void DisplayStartHint();
    void DisplayExitHint();
    void DisplayStatusIcons();
    void DisplayGameLog();
    void TryToPickupItem();
    void CheckStairs(int stairsSymbol);
    void DrawHPMP();
    void PrintDebugInfo();
    void ProcessRangedWeapon();
    void ProcessWand(ItemComponent* wand);
    void ProcessWeapon(ItemComponent* wand);
    void PrintNoAttackInTown();
    void ProcessMovement(const Position& dirOffsets);

    bool ProcessMoneyPickup(std::pair<int, GameObject*>& pair);

    void ProcessItemPickup(std::pair<int, GameObject*>& pair);

    void UpdateBar(int x, int y, RangedAttribute& attr);

    void GetActorsAround();

    void CheckIfSomethingElseIsLyingHere(const Position& pos);

    Position _cursorPosition;

    std::vector<uint64_t> _actorsForDebugDisplay;
};

#endif
