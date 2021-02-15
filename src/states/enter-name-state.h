#ifndef ENTERNAMESTATE_H
#define ENTERNAMESTATE_H

#include <string>

#include "gamestate.h"

class EnterNameState : public GameState
{
  public:
    void Prepare() override;
    void Update(bool forceUpdate = false) override;
    void HandleInput() override;

  private:
    std::string _nameEntered;
    std::string _seedEntered;

    const std::string kEnterNameString = "Enter your name";
    const std::string kEnterSeedString = "Enter seed (leave blank for random)";

    const size_t MaxNameLength = 24;
    const size_t MaxSeedStringLength = 64;

    bool _areEnteringName = true;

    size_t _x = 0;
    size_t _y = 0;
    size_t _cursorPos = 0;

    const size_t _maxNameHalf = MaxNameLength / 2;
    const size_t _maxSeedHalf = MaxSeedStringLength / 2;
};

#endif // ENTERNAMESTATE_H
