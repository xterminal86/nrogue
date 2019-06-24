#ifndef MAINSTATE_H
#define MAINSTATE_H

#include <vector>
#include <memory>

#include "gamestate.h"
#include "position.h"

class Attribute;
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
    void DisplayHelp();
    void PrintDebugInfo();
    void ProcessRangedWeapon();
    void ProcessWand(ItemComponent* wand);
    void ProcessWeapon(ItemComponent* wand);
    void PrintNoAttackInTown();
    void ProcessMovement(const Position& dirOffsets);

    bool ProcessMoneyPickup(std::pair<int, GameObject*>& pair);

    void ProcessItemPickup(std::pair<int, GameObject*>& pair);

    void UpdateBar(int x, int y, const Attribute& attr);

    Position _cursorPosition;

    std::vector<std::string> _helpText =
    {
      R"('q' - Cancel / Close menu     )",
      R"('a' - Attack                  )",
      R"('f' - Fire weapon / wand      )",
      R"('i' - Interact                )",
      R"('l' - Enter "look mode"       )",
      R"('m' - Display message log     )",
      R"('@' - Display character sheet )",
      R"('e' - Display inventory       )",
      R"('$' - Count your money        )",
      R"('g' - Get item from the ground)",
      R"('>' - Go down the stairs      )",
      R"('<' - Go up the stairs        )",
      R"('Q' - Exit game               )"
    };
};

#endif
