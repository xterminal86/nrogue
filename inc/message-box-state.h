#ifndef MESSAGEBOXSTATE_H
#define MESSAGEBOXSTATE_H

#include <string>
#include <vector>

#include "gamestate.h"

class MessageBoxState : public GameState
{
  public:
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;

    void SetMessage(std::string header, std::vector<std::string> message)
    {
      _header = header;
      _message = message;
    }

    const std::string kBackgroundColor = "#222222";

  private:
    std::string _header;
    std::vector<std::string> _message;
};

#endif // MESSAGEBOXSTATE_H
