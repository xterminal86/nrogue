#ifndef MESSAGEBOXSTATE_H
#define MESSAGEBOXSTATE_H

#include <string>
#include <vector>

#include "gamestate.h"
#include "constants.h"

class MessageBoxState : public GameState
{
  public:
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;

    void SetMessage(bool waitForInput, std::string header, std::vector<std::string> message,
                    std::string borderColor = GlobalConstants::MessageBoxDefaultBorderColor,
                    std::string bgColor = GlobalConstants::MessageBoxDefaultBgColor)
    {
      _waitForInput = waitForInput;
      _header = header;
      _message = message;
      _bgColor = bgColor;
      _borderColor = borderColor;
    }    

  private:
    std::string _header;
    std::vector<std::string> _message;

    std::string _bgColor;
    std::string _borderColor;

    bool _waitForInput;
};

#endif // MESSAGEBOXSTATE_H
