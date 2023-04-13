#ifndef MENUSTATE_H
#define MENUSTATE_H

#include <string>
#include <vector>

#include "version-config.h"

#include "position.h"
#include "gamestate.h"

class MenuState : public GameState
{
  public:
    void Init() override;
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;

  private:
    void PrepareGrassTiles();
    void DrawPicture();

  const std::vector<std::string> _title =
  {
    { "#     #   ######     #####     #####    #     #   ####### " },
    { "##    #s  #sssss#s  #sssss#s  #sssss#s  #s    #s  #sssssss" },
    { "#s#   #s  #s    #s  #s    #s  #s    ss  #s    #s  #s      " },
    { "#ss#  #s  ######s   #s    #s  #s        #s    #s  ####s   " },
    { "#s s# #s  #sssss#s  #s    #s  #s  ##    #s    #s  #ssss   " },
    { "#s  s##s  #s    #s  #s    #s  #s  ss#s  #sssss#s  #s      " },
    { "#s   s#s  #s    #s   #####ss   #####ss   #####ss  #######s" },
    { "ss    ss  ss    ss    sssss     sssss     sssss    sssssss" }
  };

  std::vector<std::string> _signature =
  {
    "by xterminal86",
    "(c) 2018-2023"
  };

  const std::string _welcome = "(press 'Enter' to start, 'q' to exit)";

  const std::vector<std::string> _picture =
  {
    "...T...T...T....#   ",
    ".T...T...T...T..#   ",
    "...T...T...T....#   ",
    "pppppppppppppppp+   ",
    "pppppppppppppp@p+   ",
    "...T...T...T....#   ",
    ".T...T...T...T..#   ",
    "...T...T...T....#   ",
  };

  #ifdef USE_SDL
  const std::string _builtWith = "powered by SDL2";
  #else
    #if defined(__unix__) || defined(__linux__)
    const std::string _builtWith = "powered by ncurses";
    #else
    const std::string _builtWith = "powered by pdcurses";
    #endif
  #endif

  #ifndef BUILD_VERSION_TEXT
  #define BUILD_VERSION_TEXT "build-dev"
  #endif

  const std::string _buildVersionText = BUILD_VERSION_TEXT;

  std::string _terminalSize;

  int _titleX = 0;
  int _titleY = 0;

  int _pictureX = 0;
  int _pictureY = 0;

  Position _borderSize;

  std::map<std::pair<int, int>, uint32_t> _grassColorByPosition;
};

#endif
