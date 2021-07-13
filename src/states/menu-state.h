#ifndef MENUSTATE_H
#define MENUSTATE_H

#include <string>
#include <vector>

#include "gamestate.h"

class MenuState : public GameState
{
  public:
    void Init() override;
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;

  private:   

  const std::vector<std::string> _title =
  {
    { "#     #   ######     #####     #####    #     #   #######" },
    { "#     #   #     #   #     #   #     #   #     #   #      " },
    { "##    #   #     #   #     #   #         #     #   #      " },
    { "# #   #   #     #   #     #   #         #     #   #      " },
    { "#  #  #   ######    #     #   #         #     #   ####   " },
    { "#   # #   #     #   #     #   #   ##    #     #   #      " },
    { "#    ##   #     #   #     #   #     #   #     #   #      " },
    { "#     #   #     #   #     #   #     #   #     #   #      " },
    { "#     #   #     #    #####     #####     #####    #######" }
  };

  const std::vector<std::string> _signature =
  {
    "by xterminal86",
    "(c) 2018-2021"
  };

  const std::string _welcome = "(press 'Enter' to start, 'q' to exit)";

  #ifdef USE_SDL
  const std::string _builtWith = "powered by SDL2";
  #else
    #if defined(__unix__) || defined(__linux__)
    const std::string _builtWith = "powered by ncurses";
    #else
    const std::string _builtWith = "powered by pdcurses";
    #endif
  #endif

  std::string _debugInfo;

  int _tw = 0;
  int _th = 0;

  int _twHalf = 0;
  int _thHalf = 0;

  int _titleX = 0;
  int _titleY = 0;
};

#endif
