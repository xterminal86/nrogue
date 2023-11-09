#ifndef MESSAGEBOXSTATE_H
#define MESSAGEBOXSTATE_H

#include <string>
#include <vector>

#include "position.h"
#include "gamestate.h"

class MessageBoxState : public GameState
{
  public:
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;

    void SetMessage(MessageBoxType type,
                    const std::string& header,
                    const std::vector<std::string>& message,
                    const uint32_t& borderColor = Colors::ShadesOfGrey::Six,
                    const uint32_t& bgColor = Colors::ShadesOfGrey::Two);

  private:
    std::string _header;
    std::vector<std::string> _message;

    uint32_t _bgColor     = Colors::BlackColor;
    uint32_t _borderColor = Colors::WhiteColor;

    Position _leftCorner;
    Position _windowSize;

    MessageBoxType _type = MessageBoxType::ANY_KEY;
};

#endif // MESSAGEBOXSTATE_H
