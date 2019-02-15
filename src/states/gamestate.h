#ifndef GAMESTATE_H
#define GAMESTATE_H

#ifndef USE_SDL
#include <ncurses.h>
#else
#include <SDL2/SDL.h>
#include <string>
#include <algorithm>

#include "constants.h"
#endif

#include <typeinfo>

class GameState
{
  public:
    virtual ~GameState() {}

    virtual void Init() {}
    virtual void Cleanup() {}
    virtual void Prepare() {}
    virtual void HandleInput() = 0;
    virtual void Update(bool forceUpdate = false) = 0;

    int GetKeyDown();

  protected:
    int _keyPressed;    

#ifdef USE_SDL
    bool ShouldPassThrough(std::string& keyName);

    std::vector<std::string> _validKeys =
    {
      "Space",
      "Return",
      "Backspace",
      "Keypad 1",
      "Keypad 2",
      "Keypad 3",
      "Keypad 4",
      "Keypad 5",
      "Keypad 6",
      "Keypad 7",
      "Keypad 8",
      "Keypad 9",
      "Up",
      "Down",
      "Left",
      "Right"
    };
#endif

};

#endif
