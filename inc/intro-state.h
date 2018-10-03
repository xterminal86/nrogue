#ifndef INTROSTATE_H
#define INTROSTATE_H

#include <string>
#include <vector>
#include <chrono>

#include "gamestate.h"

class IntroState : public GameState
{
  public:
    void Prepare() override
    {
      clear();

      _lastTime = std::chrono::high_resolution_clock::now();

      _textPositionCursor = 0;
      _textPositionX = 0;
      _textPositionY = 0;

      refresh();
    }

    void Update() override;
    void HandleInput() override;

  private:
    std::vector<std::string> _introStrings =
    {
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.",
      "This is a Knight's introduction text",
      "This is a Rogue's introduction text",
      "This is an Arcanist's introduction text",
      "This is a custom class introduction text",
    };

    int _textPositionCursor = 0;

    int _textPositionX = 0;
    int _textPositionY = 0;

    std::chrono::high_resolution_clock::time_point _lastTime;

    const std::chrono::milliseconds kTextPrintDelay = std::chrono::milliseconds(10);
};

#endif // INTROSTATE_H
