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
    void DisplayHungerStatus(const int& startPos);
    void DisplayWeaponCondition(const int& startPos);
    void DisplayArmorCondition(const int& startPos);
    void DisplayAmmoCondition(const int& startPos);
    void DisplayActiveEffects(const int& startPos);
    void DisplayGameLog();
    void DisplayScenarioInformation();
    void CheckItemsOnGround();
    void TryToPickupItems();
    void PickupSingleItem(std::pair<int, GameObject*>& item);
    void DrawHPMP();
    void GetActorsAround();
    void PrintDebugInfo();
    void PrintNoAttackInTown();
    void ProcessItemPickup(std::pair<int, GameObject*>& pair);
    void ProcessRangedWeapon();
    void ProcessWand(ItemComponent* wand);
    void ProcessWeapon(ItemComponent* wand);
    void ProcessMovement(const Position& dirOffsets);
    void UpdateBar(int x, int y, RangedAttribute& attr);
    void ClimbStairs(const std::pair<GameObject*, bool>& stairsTileInfo);

    bool ProcessMoneyPickup(std::pair<int, GameObject*>& pair);

    std::pair<GameObject*, bool> _stairsTileInfo;
    std::pair<GameObject*, bool> CheckStairs(int stairsSymbol);

    Position _cursorPosition;

    std::vector<uint64_t> _actorsForDebugDisplay;
};

#endif
