#ifndef MESSAGEBOXSTATE_H
#define MESSAGEBOXSTATE_H

#include <string>

#include "gamestate.h"

class MessageBoxState : public GameState
{
  public:
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;

    void SetMessage(std::string message)
    {
      _message = message;
    }

    const std::string kBackgroundColor = "#222222";

  private:
    std::string _message;
};

#endif // MESSAGEBOXSTATE_H
