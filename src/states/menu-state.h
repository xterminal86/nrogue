#ifndef MENUSTATE_H
#define MENUSTATE_H

#include <string>
#include <vector>

#include "gamestate.h"

class MenuState : public GameState
{
  public:
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;

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
