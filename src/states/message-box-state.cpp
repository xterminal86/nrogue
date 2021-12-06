#include "message-box-state.h"
#include "printer.h"
#include "application.h"

#ifndef USE_SDL
#include "util.h"
#endif

void MessageBoxState::HandleInput()
{
  // Closed elsewhere
  //
  // Looks like unused and can lead to endless loop in Application::Run()
  // due to if() check on MessageBoxState.
  /*
  if (_type == MessageBoxType::IGNORE_INPUT)
  {
    return;
  }
  */

  _keyPressed = GetKeyDown();

  if (_keyPressed != -1)
  {
    if (_type == MessageBoxType::WAIT_FOR_INPUT && (_keyPressed == VK_ENTER || _keyPressed == 'q'))
    {
      Application::Instance().CloseMessageBox();
    }
    else if (_type == MessageBoxType::ANY_KEY)
    {
      Application::Instance().CloseMessageBox();
    }
  }
}

void MessageBoxState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    std::string headerBgColor = Colors::MessageBoxHeaderBgColor;
    if (_borderColor == Colors::MessageBoxRedBorderColor)
    {
      headerBgColor = "#660000";
    }

    Printer::Instance().DrawWindow(_leftCorner,
                                   _windowSize,
                                   _header,
                                   Colors::WhiteColor,
                                   headerBgColor,
                                   _borderColor,
                                   Colors::BlackColor,
                                   _bgColor);

    int offset = 0;
    for (auto& s : _message)
    {
      Printer::Instance().PrintFB(_tw / 2,
                                  _th / 2 - _message.size() / 2 + offset,
                                  s,
                                  Printer::kAlignCenter,
                                  Colors::WhiteColor,
                                  _bgColor);
      offset++;
    }

    Printer::Instance().Render();
  }
}

void MessageBoxState::SetMessage(MessageBoxType type,
                                 const std::string& header,
                                 const std::vector<std::string>& message,
                                 const std::string& borderColor,
                                 const std::string& bgColor)
{
  _type = type;
  _header = header;
  _message = message;
  _bgColor = bgColor;
  _borderColor = borderColor;

  size_t len = 0;
  for (auto& s : _message)
  {
    if (s.length() > len)
    {
      len = s.length();
    }
  }

  // In case header is longer than message
  if (len < _header.length())
  {
    len = _header.length();
  }

  // Taking into account message size (usually > 0, so as is),
  // and that it's not actually a total count of number of rows,
  // but addition that we must add to the y1,
  // so 5 rows means we must add 4 if message.size = 1, 5 if 2 and so on
  int rows = 3 + _message.size();

  int x1 = _tw / 2 - len / 2 - 3;
  int y1 = _th / 2 - _message.size() / 2 - 2;
  int x2 = x1 + len + 5;
  int y2 = y1 + rows;

  _leftCorner = { x1, y1 };

#ifdef USE_SDL
  _windowSize = { x2 - x1, y2 - y1 };
#else
  _windowSize = { x2 - x1 + 1, y2 - y1 + 1 };
#endif
}
