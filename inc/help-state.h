#ifndef HELPSTATE_H
#define HELPSTATE_H

#include <string>
#include <vector>

#include "gamestate.h"

class HelpState : public GameState
{
  public:
    void HandleInput() override;
    void Update() override;

  private:
    std::vector<std::string> _helpText =
    {
      "'q' - Cancel / Exit Game",
      "'m' - Display message log",
      "'l' - Enter \"look mode\"",
      "'i' - Interact",
      "'@' - Display character sheet"
    };
};

#endif // HELPSTATE_H
