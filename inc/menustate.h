#ifndef MENUSTATE_H
#define MENUSTATE_H

#include "gamestate.h"

#include <string>
#include <vector>

class MenuState : public GameState
{
  public:
    void HandleInput() override;
    void Update() override;

  private:   

  std::vector<std::string> _title =
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
};

#endif
