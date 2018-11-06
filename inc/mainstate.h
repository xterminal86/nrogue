#ifndef MAINSTATE_H
#define MAINSTATE_H

#include <vector>
#include <memory>

#include "gamestate.h"
#include "util.h"

class Player;

class MainState : public GameState
{
  public:
    void Init() override;
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;    

  private:
    Player* _playerRef;

    std::string _debugInfo;

    void DisplayGameLog();
    void TryToPickupItem();
    void CheckStairs(chtype stairsSymbol);
    void DrawHPMP();
    void DisplayHelp();

    std::string UpdateBar(int x, int y, Attribute attr);

    Position _cursorPosition;

    std::vector<std::string> _helpText =
    {
      R"('q' - Cancel / Exit Game     )",
      R"('a' - Attack                 )",
      R"('i' - Interact               )",
      R"('l' - Enter "look mode"      )",
      R"('m' - Display message log    )",
      R"('@' - Display character sheet)",
      R"('e' - Display inventory      )"
    };
};

#endif
