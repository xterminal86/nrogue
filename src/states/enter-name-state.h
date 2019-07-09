#ifndef ENTERNAMESTATE_H
#define ENTERNAMESTATE_H

#include <string>

#include "gamestate.h"

class EnterNameState : public GameState
{
  public:
    void Update(bool forceUpdate = false) override;
    void HandleInput() override;

  private:
    std::string _nameEntered;

    const std::string kHeaderString = "Enter your name";    
};

#endif // ENTERNAMESTATE_H
