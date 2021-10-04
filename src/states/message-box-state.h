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

    void SetMessage(MessageBoxType type,
                    const std::string& header,
                    const std::vector<std::string>& message,
                    const std::string& borderColor = Colors::ShadesOfGrey::Six,
                    const std::string& bgColor = Colors::ShadesOfGrey::Two);

  private:
    std::string _header;
    std::vector<std::string> _message;

    std::string _bgColor;
    std::string _borderColor;

    MessageBoxType _type = MessageBoxType::ANY_KEY;
};

#endif // MESSAGEBOXSTATE_H
