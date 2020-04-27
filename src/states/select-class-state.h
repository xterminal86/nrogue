#ifndef SELECTCLASSSTATE_H
#define SELECTCLASSSTATE_H

#include <string>
#include <vector>

#include "gamestate.h"

class SelectClassState : public GameState
{
  public:
    void Update(bool forceUpdate = false) override;
    void HandleInput() override;

  private:
    int _menuIndex = 0;

    std::vector<std::string> _menuItems =
    {
      " Soldier  ",
      "  Thief   ",
      " Arcanist ",
      "  No one  "
    };
};

#endif // SELECTCLASSSTATE_H
