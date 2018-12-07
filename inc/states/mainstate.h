#ifndef MAINSTATE_H
#define MAINSTATE_H

#include <vector>
#include <memory>

#include "gamestate.h"
#include "util.h"

class Player;
class GameObject;

class MainState : public GameState
{
  public:
    void Init() override;
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;    

  private:
    Player* _playerRef;

    std::string _debugInfo;

    void DisplayExitHint();
    void DisplayStatusIcons();
    void DisplayGameLog();
    void TryToPickupItem();
    void CheckStairs(chtype stairsSymbol);
    void DrawHPMP();
    void DisplayHelp();
    void PrintDebugInfo();

    bool ProcessMoneyPickup(std::pair<int, GameObject*>& pair);

    void ProcessItemPickup(std::pair<int, GameObject*>& pair);

    std::string UpdateBar(int x, int y, Attribute attr);

    Position _cursorPosition;

    std::vector<std::string> _helpText =
    {
      R"('q' - Cancel / Close menu     )",
      R"('a' - Attack                  )",
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
