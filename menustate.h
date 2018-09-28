#ifndef MENUSTATE_H
#define MENUSTATE_H

#include "gamestate.h"

#include <string>

class MenuState : public GameState
{
  public:
    void HandleInput() override;
    void Update() override;

  private:   

    std::string _title = R"(
                      #     # ######   #####   #####  #     # #######
                      #     # #     # #     # #     # #     # #
                      ##    # #     # #     # #     # #     # #
                      # #   # #     # #     # #       #     # #
                      #  #  # ######  #     # #       #     # ####
                      #   # # #     # #     # #   ##  #     # #
                      #    ## #     # #     # #     # #     # #
                      #     # #     # #     # #     # #     # #
                      #     # #     #  #####   #####   #####  #######
    )";
};

#endif